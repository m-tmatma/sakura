/*!	@file
	@brief �������v���Ԃ̌v���N���X

	�f�o�b�O�ړI�ŗp����

	@author Norio Nakatani
	@date 1998/03/06  �V�K�쐬
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2002, genta

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/

#include "StdAfx.h"
#include <MMSystem.h>
#include "CRunningTimer.h"
#include "global.h"
#include "Debug.h"

#ifdef _DEBUG

#pragma comment(lib, "winmm.lib")

int CRunningTimer::m_nNestCount = 0;

CRunningTimer::CRunningTimer( const char* pszText )
{
	Reset();
	if( pszText != NULL )
		strcpy( m_szText, pszText );
	else
		m_szText[0] = '\0';
	m_nDeapth = m_nNestCount++;
	MYTRACE( _T("%3d:\"%s\" : Enter \n"), m_nDeapth, m_szText );
	return;
}


CRunningTimer::~CRunningTimer()
{
	WriteTrace("Exit Scope");
	m_nNestCount--;
	return;
}


void CRunningTimer::Reset()
{
	m_nStartTime = timeGetTime();
}


DWORD CRunningTimer::Read()
{
	return timeGetTime() - m_nStartTime;
}

/*!
	@date 2002.10.15 genta
*/
void CRunningTimer::WriteTrace(const char* msg) const
{
	MYTRACE( _T("%3d:\"%s\", %d�_�b : %s\n"), m_nDeapth, m_szText, timeGetTime() - m_nStartTime, msg );
}
#endif

/*[EOF]*/