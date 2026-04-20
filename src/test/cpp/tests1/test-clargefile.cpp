/*! @file */
/*
	Copyright (C) 2018-2025, Sakura Editor Organization

	SPDX-License-Identifier: Zlib
*/
#include "pch.h"
#include "io/CFileLoad.h"
#include "io/CBinaryStream.h"
#include "mem/CMemory.h"
#include "debug/CRunningTimer.h"

#include <cstdint>
#include <vector>

namespace {

// tests1.exe の場所から 2 階層上がるとリポジトリルート
// 例: x64\Debug\tests1.exe → x64\Debug → x64 → <root>
std::wstring GetRepoRoot()
{
	wchar_t buf[MAX_PATH];
	::GetModuleFileNameW(nullptr, buf, _countof(buf));
	std::wstring path(buf);
	for (int i = 0; i < 3; ++i) {
		const auto p = path.rfind(L'\\');
		if (p == std::wstring::npos) break;
		path = path.substr(0, p);
	}
	return path;
}

std::wstring GetTestDataPath(std::wstring_view filename)
{
	return GetRepoRoot() + L"\\" + std::wstring(filename);
}

bool FileExists(const std::wstring& path)
{
	return ::GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

} // namespace

// ============================================================
// A: 2GB 境界の正常動作確認テスト
// ============================================================

/*!
 * A-1: CFileLoad::IsLoadableSize の境界値テスト（ファイル不要）
 *
 * x64 ビルドでは常に true。
 * 32bit では 2GB 以上で false となる仕様のため、ここでは x64 のみ全 true を検証する。
 */
TEST(LargeFile, CFileLoad_IsLoadableSize_Boundary)
{
#ifdef _WIN64
	EXPECT_TRUE(CFileLoad::IsLoadableSize(0));
	EXPECT_TRUE(CFileLoad::IsLoadableSize(0x7FFFFFFFULL));   // 2GB-1
	EXPECT_TRUE(CFileLoad::IsLoadableSize(0x80000000ULL));   // 2GB
	EXPECT_TRUE(CFileLoad::IsLoadableSize(0x80000001ULL));   // 2GB+1
	EXPECT_TRUE(CFileLoad::IsLoadableSize(0xFFFFFFFFULL));   // 4GB-1
	EXPECT_TRUE(CFileLoad::IsLoadableSize(0x100000000ULL));  // 4GB
	EXPECT_TRUE(CFileLoad::IsLoadableSize(0x100000001ULL));  // 4GB+1
	EXPECT_TRUE(CFileLoad::IsLoadableSize(0x140000000ULL));  // 5GB
#else
	// 32bit: 2GB 以上は false
	EXPECT_TRUE(CFileLoad::IsLoadableSize(0x7FFFFFFFULL));
	EXPECT_FALSE(CFileLoad::IsLoadableSize(0x80000000ULL));
	EXPECT_FALSE(CFileLoad::IsLoadableSize(0x100000000ULL));
#endif
}

/*!
 * A-2: CFileLoad::GetLimitSize の返値確認（ファイル不要）
 *
 * x64 は ULLONG_MAX（無制限）、32bit は 2GB。
 */
TEST(LargeFile, CFileLoad_GetLimitSize)
{
#ifdef _WIN64
	EXPECT_EQ(ULLONG_MAX, CFileLoad::GetLimitSize());
#else
	EXPECT_EQ(0x80000000ULL, CFileLoad::GetLimitSize());
#endif
}

/*!
 * A-3: 4GB ファイルを CBinaryInputStream で開き、サイズが 4GB 超であることを確認
 *
 * testData_4G.txt が存在しない場合はスキップ。
 */
TEST(LargeFile, CBinaryInputStream_FileSize_4G)
{
	const auto path = GetTestDataPath(L"testData_4G.txt");
	if (!FileExists(path)) GTEST_SKIP() << "testData_4G.txt not found";

	CBinaryInputStream stream(path.c_str());
	const ssize_t len = stream.GetLength();
	EXPECT_GT(len, static_cast<ssize_t>(0xFFFFFFFFLL)); // > 4GB-1 つまり >= 4GB
}

/*!
 * A-4: 4GB-1 / 4GB+1 のファイルサイズが DWORD 境界をまたいでいることを確認
 *
 * GetLength() が ssize_t で 4GB を超える値を返せるかの確認。
 */
TEST(LargeFile, CBinaryInputStream_FileSize_4GBoundary)
{
	const auto path_minus = GetTestDataPath(L"testData_4G_minus1.txt");
	const auto path_plus  = GetTestDataPath(L"testData_4G_plus1.txt");

	if (!FileExists(path_minus) || !FileExists(path_plus))
		GTEST_SKIP() << "testData_4G_minus1.txt or testData_4G_plus1.txt not found";

	CBinaryInputStream minus_stream(path_minus.c_str());
	CBinaryInputStream plus_stream(path_plus.c_str());

	const ssize_t minus_len = minus_stream.GetLength();
	const ssize_t plus_len  = plus_stream.GetLength();

	// minus は 4GB 未満、plus は 4GB 超
	EXPECT_LT(minus_len, static_cast<ssize_t>(0x100000000LL));
	EXPECT_GT(plus_len,  static_cast<ssize_t>(0x100000000LL));

	// plus - minus は少なくとも 2 バイト以上の差
	EXPECT_GE(plus_len - minus_len, static_cast<ssize_t>(2));
}

/*!
 * A-5: CBinaryInputStream::Read で 4GB ファイルの先頭チャンクが読めることを確認
 *
 * 分割読み込み（DWORD チャンク）が機能していることを検証する軽量テスト。
 */
TEST(LargeFile, CBinaryInputStream_Read_Initial_Chunk)
{
	const auto path = GetTestDataPath(L"testData_4G.txt");
	if (!FileExists(path)) GTEST_SKIP() << "testData_4G.txt not found";

	CBinaryInputStream stream(path.c_str());
	constexpr size_t kChunkSize = 64 * 1024; // 64 KiB
	std::vector<std::byte> buf(kChunkSize);
	const ssize_t read = stream.Read(buf.data(), kChunkSize);
	EXPECT_EQ(static_cast<ssize_t>(kChunkSize), read);
}

/*!
 * A-6: CMemory が 2GB バッファを正常に確保できる（x64 のみ）
 *
 * メモリ不足の場合はスキップ扱い（失敗ではない）。
 */
#ifdef _WIN64
TEST(LargeFile, CMemory_AllocBuffer_2GB)
{
	constexpr size_t k2GB = 0x80000000ULL;
	CMemory mem;
	mem.AllocBuffer(k2GB);
	if (mem.GetRawPtr() == nullptr) {
		GTEST_SKIP() << "Insufficient memory to allocate 2GB buffer";
	}
	EXPECT_GE(mem.capacity(), k2GB);
}
#endif

// ============================================================
// B: タイム計測付きテスト（DISABLED_ → --gtest_also_run_disabled_tests で実行）
// ============================================================

/*!
 * B-1: 1GB ファイルを全読み込みしてスループットを計測
 *
 * testData_1G.txt が存在しない場合はスキップ。
 * 実行時間: ディスク速度次第で数秒〜数十秒。
 */
TEST(LargeFile, DISABLED_CBinaryInputStream_Timing_1GFile)
{
	const auto path = GetTestDataPath(L"testData_1G.txt");
	if (!FileExists(path)) GTEST_SKIP() << "testData_1G.txt not found";

	CRunningTimer timer(L"CBinaryInputStream read 1GB", CRunningTimer::OutputMode::OnWriteTrace);

	CBinaryInputStream stream(path.c_str());
	const ssize_t file_size = stream.GetLength();
	ASSERT_GT(file_size, static_cast<ssize_t>(0));

	constexpr size_t kChunkSize = 1024 * 1024; // 1 MiB
	std::vector<std::byte> buf(kChunkSize);
	ssize_t total = 0;
	ssize_t n;
	while ((n = stream.Read(buf.data(), kChunkSize)) > 0) {
		total += n;
	}

	const uint32_t elapsed_ms = timer.Read();
	const double throughput_mbs = elapsed_ms > 0
		? static_cast<double>(total) / 1024.0 / 1024.0 / (elapsed_ms / 1000.0)
		: 0.0;
	timer.WriteTraceFormat(L"1GB: %u ms, %.1f MB/s", elapsed_ms, throughput_mbs);

	EXPECT_EQ(file_size, total);
	std::wcout << L"[TIMING] 1GB read: " << elapsed_ms << L" ms, "
	           << static_cast<int>(throughput_mbs) << L" MB/s" << std::endl;
}

/*!
 * B-2: 4GB ファイルを全読み込みしてスループットを計測
 *
 * testData_4G.txt が存在しない場合はスキップ。
 * 実行時間: 数十秒〜数分。CI では除外推奨。
 */
TEST(LargeFile, DISABLED_CBinaryInputStream_Timing_4GFile)
{
	const auto path = GetTestDataPath(L"testData_4G.txt");
	if (!FileExists(path)) GTEST_SKIP() << "testData_4G.txt not found";

	CRunningTimer timer(L"CBinaryInputStream read 4GB", CRunningTimer::OutputMode::OnWriteTrace);

	CBinaryInputStream stream(path.c_str());
	const ssize_t file_size = stream.GetLength();
	ASSERT_GT(file_size, static_cast<ssize_t>(0));

	constexpr size_t kChunkSize = 1024 * 1024; // 1 MiB
	std::vector<std::byte> buf(kChunkSize);
	ssize_t total = 0;
	ssize_t n;
	while ((n = stream.Read(buf.data(), kChunkSize)) > 0) {
		total += n;
	}

	const uint32_t elapsed_ms = timer.Read();
	const double throughput_mbs = elapsed_ms > 0
		? static_cast<double>(total) / 1024.0 / 1024.0 / (elapsed_ms / 1000.0)
		: 0.0;
	timer.WriteTraceFormat(L"4GB: %u ms, %.1f MB/s", elapsed_ms, throughput_mbs);

	EXPECT_EQ(file_size, total);
	std::wcout << L"[TIMING] 4GB read: " << elapsed_ms << L" ms, "
	           << static_cast<int>(throughput_mbs) << L" MB/s" << std::endl;
}

/*!
 * B-3: CMemory の 2GB AppendRawData スループット計測（x64 のみ）
 *
 * 1MiB ずつ 2048 回 Append して 2GB に達するまでの所要時間を計測する。
 * メモリ不足の場合はスキップ。
 */
#ifdef _WIN64
TEST(LargeFile, DISABLED_CMemory_AppendThroughput_2GB)
{
	constexpr size_t kChunkSize = 1024 * 1024;   // 1 MiB
	constexpr size_t kChunkCount = 2048;          // → 2 GiB total
	const std::vector<std::byte> chunk(kChunkSize, std::byte{0x41}); // 'A'

	CRunningTimer timer(L"CMemory Append 2GB", CRunningTimer::OutputMode::OnWriteTrace);

	CMemory mem;
	mem.AllocBuffer(kChunkSize * kChunkCount); // 事前確保
	if (mem.GetRawPtr() == nullptr) {
		GTEST_SKIP() << "Insufficient memory for 2GB CMemory test";
	}

	mem.SetRawData(nullptr, 0); // 長さをリセット
	for (size_t i = 0; i < kChunkCount; ++i) {
		mem.AppendRawData(chunk.data(), kChunkSize);
	}

	const uint32_t elapsed_ms = timer.Read();
	const double throughput_mbs = elapsed_ms > 0
		? static_cast<double>(kChunkSize) * kChunkCount / 1024.0 / 1024.0 / (elapsed_ms / 1000.0)
		: 0.0;
	timer.WriteTraceFormat(L"CMemory 2GB Append: %u ms, %.1f MB/s", elapsed_ms, throughput_mbs);

	EXPECT_EQ(kChunkSize * kChunkCount, mem.GetRawLength());
	std::wcout << L"[TIMING] CMemory 2GB append: " << elapsed_ms << L" ms, "
	           << static_cast<int>(throughput_mbs) << L" MB/s" << std::endl;
}
#endif
