/*! @file */
/*
	Copyright (C) 2018-2022, Sakura Editor Organization

	SPDX-License-Identifier: Zlib
*/
#include "StdAfx.h"
#include "CBinaryStream.h"
#include <algorithm>
#include <limits>

CBinaryInputStream::CBinaryInputStream(LPCWSTR pszFilePath)
: CStream(pszFilePath,L"rb")
{
}

//! ストリームの「残り」サイズを取得
ssize_t CBinaryInputStream::GetLength()
{
	__int64 nCur = _ftelli64(GetFp());
	_fseeki64(GetFp(), 0, SEEK_END);
	__int64 nDataLen = _ftelli64(GetFp());
	_fseeki64(GetFp(), nCur, SEEK_SET);
	return static_cast<ssize_t>(nDataLen);
}

//! データを無変換で読み込む。戻り値は読み込んだバイト数。
//! P2: 巨大バッファ要求時は `DWORD` 最大バイト以下に分割（`Write` と対称）。
ssize_t CBinaryInputStream::Read(void* pBuffer, size_t nSizeInBytes)
{
	auto* p = static_cast<unsigned char*>(pBuffer);
	ssize_t nTotal = 0;
	constexpr size_t kMaxChunk = static_cast<size_t>(std::numeric_limits<DWORD>::max());
	while (nSizeInBytes > 0) {
		const size_t nChunk = (std::min)(nSizeInBytes, kMaxChunk);
		const size_t nRet = fread(p, 1, nChunk, GetFp());
		nTotal += static_cast<ssize_t>(nRet);
		if (nRet == 0) {
			break;
		}
		p += nRet;
		nSizeInBytes -= nRet;
		if (nRet < nChunk) {
			break;
		}
	}
	return nTotal;
}

CBinaryOutputStream::CBinaryOutputStream(LPCWSTR pszFilePath, bool bExceptionMode)
: COutputStream(pszFilePath,L"wb",bExceptionMode)
{
}
