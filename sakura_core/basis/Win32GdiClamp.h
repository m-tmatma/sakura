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
#include <algorithm>
#include <climits>

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

//! 座標・サイズ計算の `long long` 結果を `int` 範囲へ飽和（`BitBlt` / `StretchBlt` の引数向け）。
inline int ClampIntExprForGdi(long long v) noexcept
{
	return static_cast<int>(std::clamp(v, static_cast<long long>(INT_MIN), static_cast<long long>(INT_MAX)));
}

//! `RECT` の辺の差分（幅・高さ）を `long long` で算出し、負値は 0、`INT_MAX` 超は飽和。`BitBlt` / `PatBlt` / `StretchBlt` のサイズ引数向け。
inline LONG ClampRectWidthHeightForGdi(LONG edge0, LONG edge1) noexcept
{
	const long long d = static_cast<long long>(edge1) - static_cast<long long>(edge0);
	const int di = static_cast<int>(std::clamp(d, 0LL, static_cast<long long>(INT_MAX)));
	return ClampIntToLongForGdi(Int(di));
}

#endif
