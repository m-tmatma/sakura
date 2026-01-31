/*! @file */
/*
	Copyright (C) 2008, kobake
	Copyright (C) 2018-2022, Sakura Editor Organization

	SPDX-License-Identifier: Zlib
*/
#ifndef SAKURA_CCOLOR_KEYWORDSET_AAB04E86_EB95_447C_B07B_AB44395B2F7F_H_
#define SAKURA_CCOLOR_KEYWORDSET_AAB04E86_EB95_447C_B07B_AB44395B2F7F_H_
#pragma once

#include "view/colors/CColorStrategy.h"

class CColor_KeywordSet final : public CColorStrategy{
public:
	CColor_KeywordSet();
	EColorIndexType GetStrategyColor() const override{ return (EColorIndexType)(COLORIDX_KEYWORD1 + m_nKeywordIndex); }
	void InitStrategyStatus() override{ m_nCOMMENTEND = 0; }
	bool BeginColor(const CStringRef& cStr, ssize_t nPos) override;
	bool EndColor(const CStringRef& cStr, ssize_t nPos) override;
	bool Disp() const override{ return true; }
private:
	int m_nKeywordIndex;
	ssize_t m_nCOMMENTEND;
};
#endif /* SAKURA_CCOLOR_KEYWORDSET_AAB04E86_EB95_447C_B07B_AB44395B2F7F_H_ */
