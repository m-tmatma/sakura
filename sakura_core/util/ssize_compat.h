/*! @file
	@brief POSIX ssize_t の MSVC 向け定義

	MSVC の標準 C/C++ ヘッダには ssize_t が無いため、ptrdiff_t 相当で代用する。
*/
/*
	Copyright (C) 2026, Sakura Editor Organization

	SPDX-License-Identifier: Zlib
 */
#pragma once

#include <cstddef>

#if defined(_MSC_VER)
using ssize_t = std::ptrdiff_t;
#else
#include <sys/types.h>
#endif
