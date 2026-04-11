/*! @file */
/*
	Win32 GDI / `POINT` / `RECT`（`LONG`）へ渡す直前の論理座標クリップ（P2）。

	Copyright (C) 2018-2022, Sakura Editor Organization
	SPDX-License-Identifier: Zlib
*/
#ifndef SAKURA_WIN32GDICLAMP_H_
#define SAKURA_WIN32GDICLAMP_H_
#pragma once

#include "basis/primitive.h"
#include <Windows.h>

//! 論理座標（`Int`）を Win32 `LONG` に収める。超過は飽和。`USE_STRICT_INT` でも `constexpr Int` は使わない。
inline LONG ClampIntToLongForGdi(Int v) noexcept
{
	if (v < static_cast<Int>(LONG_MIN)) {
		return LONG_MIN;
	}
	if (v > static_cast<Int>(LONG_MAX)) {
		return LONG_MAX;
	}
	return static_cast<LONG>(v);
}

#endif
