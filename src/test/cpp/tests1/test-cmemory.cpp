/*! @file */
/*
	Copyright (C) 2018-2022, Sakura Editor Organization

	SPDX-License-Identifier: Zlib
*/
#include "pch.h"
#include "mem/CMemory.h"
#include "util/ssize_compat.h"

/*!
	_SetRawLength(0) を呼び出して落ちないことを確認する
*/
TEST(CMemory, SetRawLengthToZero)
{
	CMemory memory;

	// _SetRawLength(0) を呼び出して落ちないことを確認する
	memory._SetRawLength(0);
}

/*!
	CMemory を単にインスタンス化した状態ではバッファが確保されていないのを確認する。
*/
TEST(CMemory, CheckEmpty)
{
	CMemory memory;

	// インスタンス化しただけ
	// → バッファが確保されないことを確認する
	EXPECT_EQ(NULL, memory.GetRawPtr());

	// インスタンス化しただけ
	// → バッファサイズが 0 であることを確認する。
	EXPECT_EQ(0, memory.capacity());

	// インスタンス化しただけ
	// → データサイズが 0 であることを確認する。
	EXPECT_EQ(ssize_t{0}, memory.GetRawLength());
}

/*!
	CMemoryのテスト
	比較用static関数。
 */
TEST(CMemory, StaticIsEqual)
{
	constexpr auto& v1 = u8"これはテストです。";
	constexpr auto& v2 = u8"これはテストですか？";
	constexpr auto& v3 = u8"これはテストです？";

	CMemory m1(v1, int(std::size(v1)));
	CMemory m2(v2, int(std::size(v2)));
	CMemory m3(v3, int(std::size(v3)));
	CMemory m4(v1, int(std::size(v1)));

	// 長さが違う場合、false
	ASSERT_FALSE(CMemory::IsEqual(m1, m2));

	// 長さが同じでも、内容が異なればfalse
	ASSERT_FALSE(CMemory::IsEqual(m1, m3));

	// 同内容なら、true
	ASSERT_TRUE(CMemory::IsEqual(m1, m4));
}

/*!
	CMemoryのテスト
	データをWORD配列とみなしてエンディアンを反転させる機能。
 */
TEST(CMemory, SwapHLByte)
{
	constexpr auto& source = "B+saci-";
	constexpr auto& expected = "+Basic-";

	CMemory cmem1(source, int(std::size(source)) - 1);
	cmem1.SwapHLByte();
	ASSERT_TRUE(0 == memcmp(expected, cmem1.GetRawPtr(), static_cast<size_t>(cmem1.GetRawLength())));

	std::string buff(source);
	CMemory::SwapHLByte(buff.data(), buff.length());
	ASSERT_TRUE(0 == memcmp(expected, buff.data(), buff.length()));
}

/*!
	CMemoryのテスト
	ヒープに確保できる限界量を越えるサイズを要求した場合の挙動確認
 */
TEST(CMemory, OverHeapMaxReq)
{
	CMemory cmem;

	// _HEAP_MAXREQを越える値を指定すると、メモリは確保されない
	cmem.AllocBuffer(static_cast<unsigned>(_HEAP_MAXREQ) + 1);
	ASSERT_TRUE(cmem.GetRawPtr() == nullptr);

	// 検証用のデータを入れる
	constexpr auto& data = L"テストデータ";
	cmem.SetRawData(data, (int(std::size(data)) - 1) * sizeof(wchar_t));
	ASSERT_STREQ(data, reinterpret_cast<wchar_t*>(cmem.GetRawPtr()));
	ASSERT_EQ(static_cast<ssize_t>((int(std::size(data)) - 1) * sizeof(wchar_t)), cmem.GetRawLength());

	// メモリ確保失敗時は、メモリが解放される
	cmem.AllocBuffer(static_cast<unsigned>(_HEAP_MAXREQ) + 1);
	ASSERT_TRUE(cmem.GetRawPtr() == nullptr);
}

/*!
	CMemoryのテスト
	INT_MAX を超えるサイズの要求（環境によっては確保に成功する）
 */
TEST(CMemory, OverMaxSize)
{
	CMemory cmem;

	const size_t overIntMax = static_cast<size_t>(INT_MAX) + 1;
	cmem.AllocBuffer(overIntMax);
	// 64bit 環境では成功して nullptr 以外になることがある

	// 検証用のデータを入れる
	constexpr auto& data = L"テストデータ";
	cmem.SetRawData(data, (int(std::size(data)) - 1) * sizeof(wchar_t));
	ASSERT_STREQ(data, reinterpret_cast<wchar_t*>(cmem.GetRawPtr()));
	ASSERT_EQ(static_cast<ssize_t>((int(std::size(data)) - 1) * sizeof(wchar_t)), cmem.GetRawLength());

	cmem.AllocBuffer(overIntMax);
	// 同上
}

/*!
	CMemory::AppendRawDataのテスト
	様々なサイズのデータ追加が正常に行える事の確認
 */
TEST(CMemory, AppendRawData)
{
	const size_t n = 2 * 5000;
	const size_t sumAnswer = n / 2 * (n + 1);
	std::vector<char> buff(n);
	for (size_t i = 0; i < n; ++i) {
		buff[i] = '0' + (i % 10);
	}
	const void* pData = &buff[0];
	CMemory cmem;
	cmem.AllocBuffer(sumAnswer);
	size_t sum = 0;
	for (size_t i = 0; i <= n; ++i) {
		{
			cmem.AppendRawData(pData, i);
			sum += i;
			ASSERT_EQ(static_cast<size_t>(cmem.GetRawLength()), sum);
		}
		{
			// #1638 の修正に関連した試験
			CMemory cmemTmp;
			cmemTmp.AllocBuffer(1);
			cmemTmp.AppendRawData(pData, i);
			const ssize_t rawLen = cmemTmp.GetRawLength();
			ASSERT_EQ(static_cast<size_t>(rawLen), i);
		}
	}
	ASSERT_TRUE(sum == sumAnswer);
}
