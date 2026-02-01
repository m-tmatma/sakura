/*! @file */
/*
	Copyright (C) 2008, kobake
	Copyright (C) 2018-2022, Sakura Editor Organization

	SPDX-License-Identifier: Zlib
*/
#ifndef SAKURA_CCONVERT_TABTOSPACE_DDBB0EE5_D2FB_4624_9A68_4DD4B81C0744_H_
#define SAKURA_CCONVERT_TABTOSPACE_DDBB0EE5_D2FB_4624_9A68_4DD4B81C0744_H_
#pragma once

#include "CConvert.h"

class CConvert_TabToSpace final : public CConvert{
public:
	CConvert_TabToSpace(ssize_t nTabWidth, ssize_t nStartColumn, bool bExtEol)
	: m_nTabWidth(nTabWidth), m_nStartColumn(nStartColumn), m_bExtEol(bExtEol)
	{
	}

	bool DoConvert(CNativeW* pcData) override;

private:
	ssize_t m_nTabWidth;
	ssize_t m_nStartColumn;
	bool m_bExtEol;
};
#endif /* SAKURA_CCONVERT_TABTOSPACE_DDBB0EE5_D2FB_4624_9A68_4DD4B81C0744_H_ */
