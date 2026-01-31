/*! @file */
/*
	Copyright (C) 2018-2022, Sakura Editor Organization

	SPDX-License-Identifier: Zlib
*/
#include "StdAfx.h"
#include "CBinaryStream.h"

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
ssize_t CBinaryInputStream::Read(void* pBuffer, size_t nSizeInBytes)
{
	return static_cast<ssize_t>(fread(pBuffer,1,nSizeInBytes,GetFp()));
}

CBinaryOutputStream::CBinaryOutputStream(LPCWSTR pszFilePath, bool bExceptionMode)
: COutputStream(pszFilePath,L"wb",bExceptionMode)
{
}
