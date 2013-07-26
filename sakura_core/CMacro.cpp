/*!	@file
	�L�[�{�[�h�}�N��

	@author Norio Nakatani
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2000-2001, jepro
	Copyright (C) 2001, hor
	Copyright (C) 2002, YAZAKI, aroka, genta, Moca, hor
	Copyright (C) 2003, �S, ryoji, Moca
	Copyright (C) 2004, genta, zenryaku
	Copyright (C) 2005, MIK, genta, maru, zenryaku, FILE
	Copyright (C) 2006, �����, ryoji
	Copyright (C) 2007, ryoji, maru, genta
	Copyright (C) 2008, nasukoji, ryoji, syat
	Copyright (C) 2009, ryoji, nasukoji
	Copyright (C) 2011, syat

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/

#include "StdAfx.h"
#include "Funccode.h"
#include "CMacro.h"
#include "CControlTray.h"
#include "CEditView.h" //2002/2/10 aroka
#include "CSMacroMgr.h" //2002/2/10 aroka
#include "etc_uty.h" //2002/2/10 aroka
#include "CEditDoc.h"	//	2002/5/13 YAZAKI �w�b�_����
#include "Debug.h"
#include "OleTypes.h" //2003-02-21 �S
#include "CDlgInput1.h"
#include "CWaitCursor.h"

CMacro::CMacro( int nFuncID )
{
	m_nFuncID = nFuncID;
	m_pNext = NULL;
	m_pParamTop = m_pParamBot = NULL;
}

CMacro::~CMacro( void )
{
	CMacroParam* p = m_pParamTop;
	CMacroParam* del_p;
	while (p){
		del_p = p;
		p = p->m_pNext;
		delete del_p->m_pData;
		delete del_p;
	}
	return;
}

/*	�����̌^�U�蕪��
	�@�\ID�ɂ���āA���҂���^�͈قȂ�܂��B
	�����ŁA�����̌^���@�\ID�ɂ���ĐU�蕪���āAAddParam���܂��傤�B
	���Ƃ��΁AF_INSTEXT��1�߁A2�߂̈����͕�����A3�߂̈�����int�������肷��̂��A�����ł��܂��U�蕪�����邱�Ƃ����҂��Ă��܂��B

	lParam�́AHandleCommand��param�ɒl��n���Ă���R�}���h�̏ꍇ�ɂ̂ݎg���܂��B
*/
void CMacro::AddLParam( LPARAM lParam, const CEditView* pcEditView )
{
	switch( m_nFuncID ){
	/*	������p�����[�^��ǉ� */
	case F_INSTEXT:
	case F_FILEOPEN:
	case F_EXECMD:
	case F_EXECEXTMACRO:
		{
			AddStringParam( (const char *)lParam );	//	lParam��ǉ��B
			LPARAM lFlag = 0x00;
			lFlag = pcEditView->m_pShareData->m_nExecFlgOpt;
			AddIntParam( lFlag );
		}
		break;

	case F_JUMP:	//	�w��s�փW�����v�i������PL/SQL�R���p�C���G���[�s�ւ̃W�����v�͖��Ή��j
		{
			AddIntParam( pcEditView->m_pcEditDoc->m_cDlgJump.m_nLineNum );
			LPARAM lFlag = 0x00;
			lFlag |= pcEditView->m_pShareData->m_bLineNumIsCRLF		? 0x01 : 0x00;
			lFlag |= pcEditView->m_pcEditDoc->m_cDlgJump.m_bPLSQL	? 0x02 : 0x00;
			AddIntParam( lFlag );
		}
		break;

	case F_BOOKMARK_PATTERN:	//2002.02.08 hor
	case F_SEARCH_NEXT:
	case F_SEARCH_PREV:
		{
			AddStringParam( pcEditView->m_pShareData->m_sSearchKeywords.m_szSEARCHKEYArr[0] );	//	lParam��ǉ��B

			LPARAM lFlag = 0x00;
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_sSearchOption.bWordOnly		? 0x01 : 0x00;
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_sSearchOption.bLoHiCase		? 0x02 : 0x00;
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_sSearchOption.bRegularExp		? 0x04 : 0x00;
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_bNOTIFYNOTFOUND				? 0x08 : 0x00;
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_bAutoCloseDlgFind				? 0x10 : 0x00;
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_bSearchAll					? 0x20 : 0x00;
			AddIntParam( lFlag );
		}
		break;
	case F_REPLACE:
	case F_REPLACE_ALL:
		{
			AddStringParam( pcEditView->m_pShareData->m_sSearchKeywords.m_szSEARCHKEYArr[0] );	//	lParam��ǉ��B
			AddStringParam( pcEditView->m_pShareData->m_sSearchKeywords.m_szREPLACEKEYArr[0] );	//	lParam��ǉ��B

			LPARAM lFlag = 0x00;
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_sSearchOption.bWordOnly		? 0x01 : 0x00;
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_sSearchOption.bLoHiCase		? 0x02 : 0x00;
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_sSearchOption.bRegularExp		? 0x04 : 0x00;
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_bNOTIFYNOTFOUND				? 0x08 : 0x00;
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_bAutoCloseDlgFind				? 0x10 : 0x00;
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_bSearchAll					? 0x20 : 0x00;
			lFlag |= pcEditView->m_pcEditDoc->m_cDlgReplace.m_nPaste					? 0x40 : 0x00;	//	CShareData�ɓ���Ȃ��Ă����́H
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_bSelectedArea					? 0x80 : 0x00;	//	�u�����鎞�͑I�ׂȂ�
			lFlag |= pcEditView->m_pcEditDoc->m_cDlgReplace.m_nReplaceTarget << 8;	//	8bit�V�t�g�i0x100�Ŋ|���Z�j
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_bConsecutiveAll	? 0x0400: 0x00;	// 2007.01.16 ryoji
			AddIntParam( lFlag );
		}
		break;
	case F_GREP:
		{
			AddStringParam( pcEditView->m_pShareData->m_sSearchKeywords.m_szSEARCHKEYArr[0] );	//	lParam��ǉ��B
			AddStringParam( pcEditView->m_pShareData->m_sSearchKeywords.m_szGREPFILEArr[0] );	//	lParam��ǉ��B
			AddStringParam( pcEditView->m_pShareData->m_sSearchKeywords.m_szGREPFOLDERArr[0] );	//	lParam��ǉ��B

			LPARAM lFlag = 0x00;
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_bGrepSubFolder				? 0x01 : 0x00;
			//			���̕ҏW���̃e�L�X�g���猟������(0x02.������)
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_sSearchOption.bLoHiCase		? 0x04 : 0x00;
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_sSearchOption.bRegularExp		? 0x08 : 0x00;
			lFlag |= (pcEditView->m_pShareData->m_Common.m_sSearch.m_nGrepCharSet == CODE_AUTODETECT) ? 0x10 : 0x00;	//	2002/09/21 Moca ���ʌ݊����̂��߂̏���
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_bGrepOutputLine				? 0x20 : 0x00;
			lFlag |= (pcEditView->m_pShareData->m_Common.m_sSearch.m_nGrepOutputStyle == 2)		? 0x40 : 0x00;	//	CShareData�ɓ���Ȃ��Ă����́H
			lFlag |= pcEditView->m_pShareData->m_Common.m_sSearch.m_nGrepCharSet << 8;
			AddIntParam( lFlag );
		}
		break;
	/*	���l�p�����[�^��ǉ� */
	case F_CHAR:
		AddIntParam( lParam );
		break;

	/*	�W�����p�����[�^��ǉ� */
	default:
		AddIntParam( lParam );
		break;
	}
}

/*	�����ɕ������ǉ��B
*/
void CMacro::AddStringParam( const char* szParam )
{
	CMacroParam* param = new CMacroParam;
	param->m_pNext = NULL;

	//	�K�v�ȗ̈���m�ۂ��ăR�s�[�B
	int nLen = _tcslen( szParam );
	param->m_pData = new char[nLen + 1];
	memcpy((char*)param->m_pData, szParam, nLen );
	param->m_pData[nLen] = _T('\0');

	//	���X�g�̐�������ۂ�
	if (m_pParamTop){
		m_pParamBot->m_pNext = param; 
		m_pParamBot = param;
	}
	else {
		m_pParamTop = param;
		m_pParamBot = m_pParamTop;
	}
}

/*	�����ɐ��l��ǉ��B
*/
void CMacro::AddIntParam( const int nParam )
{
	CMacroParam* param = new CMacroParam;
	param->m_pNext = NULL;

	//	�K�v�ȗ̈���m�ۂ��ăR�s�[�B
	param->m_pData = new char[16];	//	���l�i�[�i�ő�16���j�p
	itoa(nParam, param->m_pData, 10);

	//	���X�g�̐�������ۂ�
	if (m_pParamTop){
		m_pParamBot->m_pNext = param; 
		m_pParamBot = param;
	}
	else {
		m_pParamTop = param;
		m_pParamBot = m_pParamTop;
	}
}

/**	�R�}���h�����s����ipcEditView->HandleCommand�𔭍s����j
	m_nFuncID�ɂ���āA�����̌^�𐳊m�ɓn���Ă����܂��傤�B
	
	@note
	paramArr�͉����̃|�C���^�i�A�h���X�j��LONG�ł���킵���l�ɂȂ�܂��B
	������char*�̂Ƃ��́AparamArr[i]�����̂܂�HandleCommand�ɓn���Ă��܂��܂���B
	������int�̂Ƃ��́A*((int*)paramArr[i])�Ƃ��ēn���܂��傤�B
	
	���Ƃ��΁AF_INSTEXT��1�߁A2�߂̈����͕�����A3�߂̈�����int�A4�߂̈����������B�������肷��ꍇ�́A���̂悤�ɂ��܂��傤�B
	pcEditView->HandleCommand( m_nFuncID, true, paramArr[0], paramArr[1], *((int*)paramArr[2]), 0);
	
	@date 2007.07.20 genta : flags�ǉ��DFA_FROMMACRO��flags�Ɋ܂߂ēn�����̂Ƃ���D
		(1�R�}���h���s���ɖ��񉉎Z����K�v�͂Ȃ��̂�)
*/
void CMacro::Exec( CEditView* pcEditView, int flags ) const
{
	const char* paramArr[4] = {NULL, NULL, NULL, NULL};	//	4�Ɍ���B
	
	CMacroParam* p = m_pParamTop;
	int i = 0;
	for (i = 0; i < 4; i++) {
		if (!p) break;	//	p���������break;
		paramArr[i] = p->m_pData;
		p = p->m_pNext;
	}
	CMacro::HandleCommand(pcEditView, m_nFuncID | flags, paramArr, i);
}

/*	CMacro���Č����邽�߂̏���hFile�ɏ����o���܂��B

	InsText("�Ȃ�Ƃ�");
	�̂悤�ɁB
*/
void CMacro::Save( HINSTANCE hInstance, HFILE hFile ) const
{
	TCHAR			szFuncName[1024];
	TCHAR			szFuncNameJapanese[500];
	int				nTextLen;
	const char*		pText;
	char			szLine[1024];
	CMemory			cmemWork;

	/* 2002.2.2 YAZAKI CSMacroMgr�ɗ��� */
	if (CSMacroMgr::GetFuncInfoByID( hInstance, m_nFuncID, szFuncName, szFuncNameJapanese)){
		switch ( m_nFuncID ){
		case F_INSTEXT:
		case F_FILEOPEN:
		case F_EXECEXTMACRO:
			//	�����ЂƂ������ۑ�
			pText = m_pParamTop->m_pData;
			nTextLen = _tcslen(pText);
			cmemWork.SetString( pText, nTextLen );
			cmemWork.Replace_j( _T("\\"), _T("\\\\") );
			cmemWork.Replace_j( _T("\'"), _T("\\\'") );
			wsprintf(
				szLine,
				_T("S_%s(\'%s\');\t// %s\r\n"),
				szFuncName,
				cmemWork.GetStringPtr(),
				szFuncNameJapanese
			);
			_lwrite( hFile, szLine, _tcslen( szLine ) );
			break;
		case F_JUMP:		//	�w��s�փW�����v�i������PL/SQL�R���p�C���G���[�s�ւ̃W�����v�͖��Ή��j
			wsprintf(
				szLine,
				_T("S_%s(%d, %d);\t// %s\r\n"),
				szFuncName,
				(m_pParamTop->m_pData ? atoi(m_pParamTop->m_pData) : 1),
				m_pParamTop->m_pNext->m_pData ? atoi(m_pParamTop->m_pNext->m_pData) : 0,
				szFuncNameJapanese
			);
			_lwrite( hFile, szLine, _tcslen( szLine ) );
			break;
		case F_BOOKMARK_PATTERN:	//2002.02.08 hor
		case F_SEARCH_NEXT:
		case F_SEARCH_PREV:
			pText = m_pParamTop->m_pData;
			nTextLen = _tcslen(pText);
			cmemWork.SetString( pText, nTextLen );
			cmemWork.Replace_j( _T("\\"), _T("\\\\") );
			cmemWork.Replace_j( _T("\'"), _T("\\\'") );
			wsprintf( szLine, "S_%s(\'%s\', %d);\t// %s\r\n", szFuncName, cmemWork.GetStringPtr(), m_pParamTop->m_pNext->m_pData ? atoi(m_pParamTop->m_pNext->m_pData) : 0, szFuncNameJapanese );
			_lwrite( hFile, szLine, _tcslen( szLine ) );
			break;
		case F_EXECMD:
			//	�����ЂƂ������ۑ�
			pText = m_pParamTop->m_pData;
			nTextLen = _tcslen(pText);
			cmemWork.SetString( pText, nTextLen );
			cmemWork.Replace_j( _T("\\"), _T("\\\\") );
			cmemWork.Replace_j( _T("\'"), _T("\\\'") );
			wsprintf( szLine, "S_%s(\'%s\', %d);\t// %s\r\n", szFuncName, cmemWork.GetStringPtr(), m_pParamTop->m_pNext->m_pData ? atoi(m_pParamTop->m_pNext->m_pData) : 0, szFuncNameJapanese );
			_lwrite( hFile, szLine, _tcslen( szLine ) );
			break;
		case F_REPLACE:
		case F_REPLACE_ALL:
			pText = m_pParamTop->m_pData;
			nTextLen = _tcslen(pText);
			cmemWork.SetString( pText, nTextLen );
			cmemWork.Replace_j( _T("\\"), _T("\\\\") );
			cmemWork.Replace_j( _T("\'"), _T("\\\'") );
			{
				CMemory cmemWork2(m_pParamTop->m_pNext->m_pData, _tcslen(m_pParamTop->m_pNext->m_pData));
				cmemWork2.Replace_j( "\\", "\\\\" );
				cmemWork2.Replace_j( "\'", "\\\'" );
				wsprintf(
					szLine,
					_T("S_%s(\'%s\', \'%s\', %d);\t// %s\r\n"),
					szFuncName,
					cmemWork.GetStringPtr(),
					cmemWork2.GetStringPtr(),
					m_pParamTop->m_pNext->m_pNext->m_pData ? atoi(m_pParamTop->m_pNext->m_pNext->m_pData) : 0,
					szFuncNameJapanese
				);
				_lwrite( hFile, szLine, _tcslen( szLine ) );
			}
			break;
		case F_GREP:
			pText = m_pParamTop->m_pData;
			nTextLen = _tcslen(pText);
			cmemWork.SetString( pText, nTextLen );
			cmemWork.Replace_j( _T("\\"), _T("\\\\") );
			cmemWork.Replace_j( _T("\'"), _T("\\\'") );
			{
				CMemory cmemWork2(m_pParamTop->m_pNext->m_pData, _tcslen(m_pParamTop->m_pNext->m_pData));
				cmemWork2.Replace_j( _T("\\"), _T("\\\\") );
				cmemWork2.Replace_j( _T("\'"), _T("\\\'") );

				CMemory cmemWork3(m_pParamTop->m_pNext->m_pNext->m_pData, _tcslen(m_pParamTop->m_pNext->m_pNext->m_pData));
				cmemWork3.Replace_j( _T("\\"), _T("\\\\") );
				cmemWork3.Replace_j( _T("\'"), _T("\\\'") );
				wsprintf(
					szLine,
					_T("S_%s(\'%s\', \'%s\', \'%s\', %d);\t// %s\r\n"),
					szFuncName,
					cmemWork.GetStringPtr(),
					cmemWork2.GetStringPtr(),
					cmemWork3.GetStringPtr(),
					m_pParamTop->m_pNext->m_pNext->m_pNext->m_pData ? atoi(m_pParamTop->m_pNext->m_pNext->m_pNext->m_pData) : 0,
					szFuncNameJapanese
				);
				_lwrite( hFile, szLine, _tcslen( szLine ) );
			}
			break;
		default:
			if( 0 == m_pParamTop ){
				wsprintf( szLine, "S_%s();\t// %s\r\n", szFuncName, szFuncNameJapanese );
			}else{
				wsprintf( szLine, "S_%s(%d);\t// %s\r\n", szFuncName, m_pParamTop->m_pData ? atoi(m_pParamTop->m_pData) : 0, szFuncNameJapanese );
			}
			_lwrite( hFile, szLine, _tcslen( szLine ) );
			break;
		}
		return;
	}
	wsprintf( szLine, _T("CMacro::GetFuncInfoByID()�ɁA�o�O������̂ŃG���[���o�܂���������������������\r\n") );
	_lwrite( hFile, szLine, _tcslen( szLine ) );
}

/**	�}�N�������ϊ�

	Macro�R�}���h��CEditView::HandleCommand�Ɉ����n���D
	�������Ȃ��}�N���������C�}�N����HandleCommand�ł̑Ή��������Œ�`����K�v������D

	@param pcEditView	[in]	����Ώ�EditView
	@param Index	[in] ����16bit: �@�\ID, ��ʃ��[�h�͂��̂܂�CMacro::HandleCommand()�ɓn���D
	@param Argument [in] ����
	@param ArgSize	[in] �����̐�
	
	@date 2007.07.08 genta Index�̃R�}���h�ԍ������ʃ��[�h�ɐ���
*/
void CMacro::HandleCommand(
	CEditView*			pcEditView,
	const int			Index,
	const char*			Argument[],
	const int			ArgSize
)
{
	const TCHAR EXEC_ERROR_TITLE[] = _T("Macro���s�G���[");

	switch ( LOWORD(Index) ) 
	{
	case F_CHAR:		//	�������́B���l�͕����R�[�h
	case F_IME_CHAR:	//	���{�����
	case F_CHGMOD_EOL:	//	���͉��s�R�[�h�w��BEEolType�̐��l���w��B2003.06.23 Moca
		//	Jun. 16, 2002 genta
		if( Argument[0] == NULL ){
			::MYMESSAGEBOX(
				NULL,
				MB_OK | MB_ICONSTOP | MB_TOPMOST,
				EXEC_ERROR_TITLE,
				_T("�}�����ׂ������R�[�h���w�肳��Ă��܂���D")
			);
			break;
		}
		/* NO BREAK */
	case F_PASTE:	// 2011.06.26 Moca
	case F_PASTEBOX:	// 2011.06.26 Moca
	case F_TEXTWRAPMETHOD:	//	�e�L�X�g�̐܂�Ԃ����@�̎w��B���l�́A0x0�i�܂�Ԃ��Ȃ��j�A0x1�i�w�茅�Ő܂�Ԃ��j�A0x2�i�E�[�Ő܂�Ԃ��j	// 2008.05.30 nasukoji
	case F_GOLINETOP:	//	�s���Ɉړ��B���l�́A0x0�i�f�t�H���g�j�A0x1�i�󔒂𖳎����Đ擪�Ɉړ��j�A0x2�i����`�j�A0x4�i�I�����Ĉړ��j�A0x8�i���s�P�ʂŐ擪�Ɉړ��F�������j
		//	��ڂ̈��������l�B
		pcEditView->HandleCommand( Index, false, (Argument[0] != NULL ? atoi(Argument[0]) : 0 ), 0, 0, 0 );
		break;
	case F_INSTEXT:		//	�e�L�X�g�}��
	case F_ADDTAIL:		//	���̑���̓L�[�{�[�h����ł͑��݂��Ȃ��̂ŕۑ����邱�Ƃ��ł��Ȃ��H
	case F_SET_QUOTESTRING:	// Jan. 29, 2005 genta �ǉ� �e�L�X�g����1�����}�N���͂����ɓ������Ă������D
		//	��ڂ̈�����������B
		//	������2�ڂ̈����͕������B
		if( Argument[0] == NULL ){
			::MYMESSAGEBOX(
				NULL,
				MB_OK | MB_ICONSTOP | MB_TOPMOST,
				EXEC_ERROR_TITLE,
				_T("����(������)���w�肳��Ă��܂���D")
			);
			break;
		}
		{
			int len = _tcslen(Argument[0]);
			pcEditView->HandleCommand( Index, false, (LPARAM)Argument[0], len, 0, 0 );	//	�W��
		}
		break;
	/* ��ځA��ڂƂ������͐��l */
	case F_JUMP:		//	�w��s�փW�����v�i������PL/SQL�R���p�C���G���[�s�ւ̃W�����v�͖��Ή��j
		//	Argument[0]�փW�����v�B�I�v�V������Argument[1]�ɁB
		//		******** �ȉ��u�s�ԍ��̒P�ʁv ********
		//		0x00	�܂�Ԃ��P�ʂ̍s�ԍ�
		//		0x01	���s�P�ʂ̍s�ԍ�
		//		**************************************
		//		0x02	PL/SQL�R���p�C���G���[�s����������
		//		����`	�e�L�X�g�́��s�ڂ��u���b�N��1�s�ڂƂ���
		//		����`	���o���ꂽPL/SQL�p�b�P�[�W�̃u���b�N����I��
		if( Argument[0] == NULL ){
			::MYMESSAGEBOX(
				NULL,
				MB_OK | MB_ICONSTOP | MB_TOPMOST,
				EXEC_ERROR_TITLE,
				_T("�W�����v��s�ԍ����w�肳��Ă��܂���D")
			);
			break;
		}
		{
			pcEditView->m_pcEditDoc->m_cDlgJump.m_nLineNum = atoi(Argument[0]);	//�W�����v��
			LPARAM lFlag = Argument[1] != NULL ? atoi(Argument[1]) : 1; // �f�t�H���g1
			pcEditView->m_pShareData->m_bLineNumIsCRLF = lFlag & 0x01 ? 1 : 0;
			pcEditView->m_pcEditDoc->m_cDlgJump.m_bPLSQL = lFlag & 0x02 ? 1 : 0;
			pcEditView->HandleCommand( Index, false, 0, 0, 0, 0 );	//	�W��
		}
		break;
	/*	��ڂ̈����͕�����A��ڂ̈����͐��l	*/
	case F_BOOKMARK_PATTERN:	//2002.02.08 hor
		if( Argument[0] == NULL ){
			::MYMESSAGEBOX(
				NULL,
				MB_OK | MB_ICONSTOP | MB_TOPMOST,
				EXEC_ERROR_TITLE,
				_T("�}�[�N�s�̃p�^�[�����w�肳��Ă��܂���D")
			);
			break;
		}
		/* NO BREAK */
	case F_SEARCH_NEXT:
	case F_SEARCH_PREV:
		//	Argument[0]�������B�I�v�V������Argument[1]�ɁB
		//	Argument[1]:
		//		0x01	�P��P�ʂŒT��
		//		0x02	�p�啶���Ə���������ʂ���
		//		0x04	���K�\��
		//		0x08	������Ȃ��Ƃ��Ƀ��b�Z�[�W��\��
		//		0x10	�����_�C�A���O�������I�ɕ���
		//		0x20	�擪�i�����j����Č�������
		{
			LPARAM lFlag = Argument[1] != NULL ? atoi(Argument[1]) : 0;
			if( 0 < _tcslen( Argument[0] ) ){
				/* ���K�\�� */
				if( lFlag & 0x04
					&& !CheckRegexpSyntax( Argument[0], NULL, true )
				)
				{
					break;
				}

				/* ���������� */
				CShareData::getInstance()->AddToSearchKeyArr( (const char*)Argument[0] );
			}
			//	�ݒ�l�o�b�N�A�b�v
			//	�}�N���p�����[�^���ݒ�l�ϊ�
			pcEditView->m_pShareData->m_Common.m_sSearch.m_sSearchOption.bWordOnly	= lFlag & 0x01 ? 1 : 0;
			pcEditView->m_pShareData->m_Common.m_sSearch.m_sSearchOption.bLoHiCase	= lFlag & 0x02 ? 1 : 0;
			pcEditView->m_pShareData->m_Common.m_sSearch.m_sSearchOption.bRegularExp	= lFlag & 0x04 ? 1 : 0;
			pcEditView->m_pShareData->m_Common.m_sSearch.m_bNOTIFYNOTFOUND	= lFlag & 0x08 ? 1 : 0;
			pcEditView->m_pShareData->m_Common.m_sSearch.m_bAutoCloseDlgFind	= lFlag & 0x10 ? 1 : 0;
			pcEditView->m_pShareData->m_Common.m_sSearch.m_bSearchAll			= lFlag & 0x20 ? 1 : 0;

			//	�R�}���h���s
		//	pcEditView->HandleCommand( Index, false, (LPARAM)Argument[0], 0, 0, 0);
			pcEditView->HandleCommand( Index, false, 0, 0, 0, 0);
		}
		break;
	case F_DIFF:
		//	Argument[0]��Diff�����\���B�I�v�V������Argument[1]�ɁB
		//	Argument[1]:
		//		���̐��l�̘a�B
		//		0x0001 -i ignore-case         �啶�����������ꎋ
		//		0x0002 -w ignore-all-space    �󔒖���
		//		0x0004 -b ignore-space-change �󔒕ύX����
		//		0x0008 -B ignore-blank-lines  ��s����
		//		0x0010 -t expand-tabs         TAB-SPACE�ϊ�
		//		0x0020    (�ҏW���̃t�@�C�������t�@�C��)
		//		0x0040    (DIFF�������Ȃ��Ƃ��Ƀ��b�Z�[�W�\��)
		/* NO BREAK */

	case F_EXECMD:
		//	Argument[0]�����s�B�I�v�V������Argument[1]�ɁB
		//	Argument[1]:
		//		���̐��l�̘a�B
		//		0x01	�W���o�͂𓾂�
		//		0x02	�W���o�͂��L�����b�g�ʒu��	//	2007.01.02 maru �����̊g��
		//		0x04	�ҏW���t�@�C����W�����͂�	//	2007.01.02 maru �����̊g��
		/* NO BREAK */

	case F_TRACEOUT:		// 2006.05.01 �}�N���p�A�E�g�v�b�g�E�C���h�E�ɏo��
		//	Argument[0]���o�́B�I�v�V������Argument[1]�ɁB
		//	Argument[1]:
		//		���̐��l�̘a�B
		//		0x01	ExpandParameter�ɂ�镶����W�J���s��
		//		0x02	�e�L�X�g�����ɉ��s�R�[�h��t�����Ȃ�
		if( Argument[0] == NULL ){
			::MYMESSAGEBOX(
				NULL,
				MB_OK | MB_ICONSTOP | MB_TOPMOST,
				EXEC_ERROR_TITLE,
				_T(	"����(������)���w�肳��Ă��܂���D" )
			);
			break;
		}
		{
			pcEditView->HandleCommand( Index, false, (LPARAM)Argument[0], (LPARAM)(Argument[1] != NULL ? atoi(Argument[1]) : 0 ), 0, 0);
		}
		break;

	/* �͂��߂̈����͕�����B�Q�ڂƂR�ڂ͐��l */
	case F_PUTFILE:		// 2006.12.10 ��ƒ��t�@�C���̈ꎞ�o��
		//	Argument[0]�ɏo�́BArgument[1]�ɕ����R�[�h�B�I�v�V������Argument[2]�ɁB
		//	Argument[2]:
		//		���̒l�̘a
		//		0x01	�I��͈͂��o�́i��I����ԂȂ��t�@�C���𐶐��j
		// no break

	case F_INSFILE:		// 2006.12.10 �L�����b�g�ʒu�Ƀt�@�C���}��
		//	Argument[0]�ɏo�́BArgument[1]�ɕ����R�[�h�B�I�v�V������Argument[2]�ɁB
		//	Argument[2]:
		//		���݂͓��ɂȂ�
		if( Argument[0] == NULL ){
			::MYMESSAGEBOX( NULL, MB_OK | MB_ICONSTOP | MB_TOPMOST, EXEC_ERROR_TITLE,
				_T(	"�t�@�C�������w�肳��Ă��܂���D" ));
			break;
		}
		{
			pcEditView->HandleCommand(
				Index,
				false,
				(LPARAM)Argument[0], 
				(LPARAM)(Argument[1] != NULL ? atoi(Argument[1]) : 0 ),
				(LPARAM)(Argument[2] != NULL ? atoi(Argument[2]) : 0 ),
				0
			);
		}
		break;

	/* �͂��߂�2�̈����͕�����B3�ڂ͐��l */
	case F_REPLACE:
	case F_REPLACE_ALL:
		//	Argument[0]���AArgument[1]�ɒu���B�I�v�V������Argument[2]�Ɂi�����\��j
		//	Argument[2]:
		//		���̐��l�̘a�B
		//		0x001	�P��P�ʂŒT��
		//		0x002	�p�啶���Ə���������ʂ���
		//		0x004	���K�\��
		//		0x008	������Ȃ��Ƃ��Ƀ��b�Z�[�W��\��
		//		0x010	�����_�C�A���O�������I�ɕ���
		//		0x020	�擪�i�����j����Č�������
		//		0x040	�N���b�v�{�[�h����\��t����
		//		******** �ȉ��u�u���͈́v ********
		//		0x000	�t�@�C���S��
		//		0x080	�I��͈�
		//		**********************************
		//		******** �ȉ��u�u���Ώہv ********
		//		0x000	��������������ƒu��
		//		0x100	��������������̑O�ɑ}��
		//		0x200	��������������̌�ɒǉ�
		//		**********************************
		//		0x400	�u���ׂĒu���v�͒u���̌J�Ԃ��iON:�A���u��, OFF:�ꊇ�u���j
		if( Argument[0] == NULL || Argument[0][0] == _T('\0') ){
			::MYMESSAGEBOX( NULL, MB_OK | MB_ICONSTOP | MB_TOPMOST, EXEC_ERROR_TITLE,
				_T("�u�����p�^�[�����w�肳��Ă��܂���D"));
			break;
		}
		if( Argument[1] == NULL ){
			::MYMESSAGEBOX( NULL, MB_OK | MB_ICONSTOP | MB_TOPMOST, EXEC_ERROR_TITLE,
				_T("�u����p�^�[�����w�肳��Ă��܂���D"));
			break;
		}
		{
			LPARAM lFlag = Argument[2] != NULL ? atoi(Argument[2]) : 0;
			/* ���K�\�� */
			if( lFlag & 0x04
				&& !CheckRegexpSyntax( Argument[0], NULL, true )
			)
			{
				break;
			}

			/* ���������� */
			CShareData::getInstance()->AddToSearchKeyArr( (const char*)Argument[0] );

			/* ���������� */
			CShareData::getInstance()->AddToReplaceKeyArr( (const char*)Argument[1] );

			pcEditView->m_pShareData->m_Common.m_sSearch.m_sSearchOption.bWordOnly		= lFlag & 0x01 ? 1 : 0;
			pcEditView->m_pShareData->m_Common.m_sSearch.m_sSearchOption.bLoHiCase		= lFlag & 0x02 ? 1 : 0;
			pcEditView->m_pShareData->m_Common.m_sSearch.m_sSearchOption.bRegularExp		= lFlag & 0x04 ? 1 : 0;
			pcEditView->m_pShareData->m_Common.m_sSearch.m_bNOTIFYNOTFOUND	= lFlag & 0x08 ? 1 : 0;
			pcEditView->m_pShareData->m_Common.m_sSearch.m_bAutoCloseDlgFind	= lFlag & 0x10 ? 1 : 0;
			pcEditView->m_pShareData->m_Common.m_sSearch.m_bSearchAll			= lFlag & 0x20 ? 1 : 0;
			pcEditView->m_pcEditDoc->m_cDlgReplace.m_nPaste			= lFlag & 0x40 ? 1 : 0;	//	CShareData�ɓ���Ȃ��Ă����́H
//			pcEditView->m_pShareData->m_Common.m_sSearch.m_bSelectedArea		= 0;	//	lFlag & 0x80 ? 1 : 0;
			pcEditView->m_pShareData->m_Common.m_sSearch.m_bConsecutiveAll	= lFlag & 0x0400 ? 1 : 0;	// 2007.01.16 ryoji
			if (LOWORD(Index) == F_REPLACE) {	// 2007.07.08 genta �R�}���h�͉��ʃ��[�h
				//	�u�����鎞�͑I�ׂȂ�
				pcEditView->m_pShareData->m_Common.m_sSearch.m_bSelectedArea	= 0;
			}
			else if (LOWORD(Index) == F_REPLACE_ALL) {	// 2007.07.08 genta �R�}���h�͉��ʃ��[�h
				//	�S�u���̎��͑I�ׂ�H
				pcEditView->m_pShareData->m_Common.m_sSearch.m_bSelectedArea	= lFlag & 0x80 ? 1 : 0;
			}
			pcEditView->m_pcEditDoc->m_cDlgReplace.m_nReplaceTarget	= (lFlag >> 8) & 0x03;	//	8bit�V�t�g�i0x100�Ŋ���Z�j	// 2007.01.16 ryoji ���� 2bit�������o��
			//	�R�}���h���s
			pcEditView->HandleCommand( Index, false, 0, 0, 0, 0);
		}
		break;
	case F_GREP:
		//	Argument[0]	����������
		//	Argument[1]	�����Ώۂɂ���t�@�C����
		//	Argument[2]	�����Ώۂɂ���t�H���_��
		//	Argument[3]:
		//		���̐��l�̘a�B
		//		0x01	�T�u�t�H���_�������������
		//		0x02	���̕ҏW���̃e�L�X�g���猟������i�������j
		//		0x04	�p�啶���Ɖp����������ʂ���
		//		0x08	���K�\��
		//		0x10	�����R�[�h��������
		//		******** �ȉ��u���ʏo�́v ********
		//		0x00	�Y���s
		//		0x20	�Y������
		//		**********************************
		//		******** �ȉ��u�o�͌`���v ********
		//		0x00	�m�[�}��
		//		0x40	�t�@�C����
		//		**********************************
		//		0x0100 �` 0xff00	�����R�[�h�Z�b�g�ԍ� * 0x100
		if( Argument[0] == NULL ){
			::MYMESSAGEBOX( NULL, MB_OK | MB_ICONSTOP | MB_TOPMOST, EXEC_ERROR_TITLE,
				_T("GREP�p�^�[�����w�肳��Ă��܂���D"));
			break;
		}
		if( Argument[1] == NULL ){
			::MYMESSAGEBOX( NULL, MB_OK | MB_ICONSTOP | MB_TOPMOST, EXEC_ERROR_TITLE,
				_T("�t�@�C����ʂ��w�肳��Ă��܂���D"));
			break;
		}
		if( Argument[2] == NULL ){
			::MYMESSAGEBOX( NULL, MB_OK | MB_ICONSTOP | MB_TOPMOST, EXEC_ERROR_TITLE,
				_T("������t�H���_���w�肳��Ă��܂���D"));
			break;
		}
		{
			//	��ɊO���E�B���h�E�ɁB
			/*======= Grep�̎��s =============*/
			/* Grep���ʃE�B���h�E�̕\�� */
			CMemory cmWork1;	cmWork1.SetString( Argument[0] );	cmWork1.Replace_j( _T("\""), _T("\"\"") );	//	����������
			CMemory cmWork2;	cmWork2.SetString( Argument[1] );	cmWork2.Replace_j( _T("\""), _T("\"\"") );	//	�t�@�C����
			CMemory cmWork3;	cmWork3.SetString( Argument[2] );	cmWork3.Replace_j( _T("\""), _T("\"\"") );	//	�t�H���_��

			LPARAM lFlag = Argument[3] != NULL ? atoi(Argument[3]) : 5;

			// 2002/09/21 Moca �����R�[�h�Z�b�g
			int		nCharSet;
			{
				nCharSet = CODE_SJIS;
				if( lFlag & 0x10 ){	// �����R�[�h��������(���ʌ݊��p)
					nCharSet = CODE_AUTODETECT;
				}
				int nCode = (lFlag >> 8) & 0xff; // ������ 7-15 �r�b�g��(0�J�n)���g��
				if( IsValidCodeTypeExceptSJIS(nCode) || CODE_AUTODETECT == nCode ){
					nCharSet = nCode;
				}
			}

			// -GREPMODE -GKEY="1" -GFILE="*.*;*.c;*.h" -GFOLDER="c:\" -GCODE=0 -GOPT=S
			char	pCmdLine[1024];
			TCHAR	pOpt[64];
			wsprintf(
				pCmdLine,
				_T("-GREPMODE -GKEY=\"%s\" -GFILE=\"%s\" -GFOLDER=\"%s\" -GCODE=%d"),
				cmWork1.GetStringPtr(),
				cmWork2.GetStringPtr(),
				cmWork3.GetStringPtr(),
				nCharSet
			);

			pOpt[0] = '\0';
			if( lFlag & 0x01 ) _tcscat( pOpt, _T("S") );	/* �T�u�t�H���_������������� */
			if( lFlag & 0x04 ) _tcscat( pOpt, _T("L") );	/* �p�啶���Ɖp����������ʂ��� */
			if( lFlag & 0x08 ) _tcscat( pOpt, _T("R") );	/* ���K�\�� */
			if( lFlag & 0x20 ) _tcscat( pOpt, _T("P") );	/* �s���o�͂��邩�Y�����������o�͂��邩 */
			if( lFlag & 0x40 ) _tcscat( pOpt, _T("2") );	/* Grep: �o�͌`�� */
			else _tcscat( pOpt, _T("1") );
			if( pOpt[0] !=  _T('\0') ){
				_tcscat( pCmdLine, _T(" -GOPT=") );
				_tcscat( pCmdLine, pOpt );
			}

			/* �V�K�ҏW�E�B���h�E�̒ǉ� ver 0 */
			CControlTray::OpenNewEditor( pcEditView->m_hInstance, pcEditView->m_hWnd, pCmdLine, CODE_NONE, false );
			/*======= Grep�̎��s =============*/
			/* Grep���ʃE�B���h�E�̕\�� */
		}
		break;
	case F_FILEOPEN:
		//	Argument[0]���J���B
		if( Argument[0] == NULL ){
			::MYMESSAGEBOX( NULL, MB_OK | MB_ICONSTOP | MB_TOPMOST, EXEC_ERROR_TITLE,
				_T("�ǂݍ��݃t�@�C�������w�肳��Ă��܂���D"));
			break;
		}
		{
			pcEditView->HandleCommand( Index, false, (LPARAM)Argument[0], 0, 0, 0);
		}
		break;
	case F_FILESAVEAS:
		//	Argument[0]��ʖ��ŕۑ��B
		if( Argument[0] == NULL ){
			::MYMESSAGEBOX( NULL, MB_OK | MB_ICONSTOP | MB_TOPMOST, EXEC_ERROR_TITLE,
				_T("�ۑ��t�@�C�������w�肳��Ă��܂���D"));
			break;
		}
		{
			/* �f�t�H���g�l */
			//	Sep. 11, 2004 genta �����l���u�ύX���Ȃ��v��
			//	0����SJIS�w��ƂȂ��Ă��܂�����
			ECodeType nCharCode = CODE_AUTODETECT;
			int nSaveLineCode = 0;
			
			if (Argument[1] != NULL){
				nCharCode = (ECodeType)atoi( Argument[1] );
			}
			if (Argument[2] != NULL){
				nSaveLineCode = atoi( Argument[2] );
			}
			// �����R�[�h�Z�b�g
			//	Sep. 11, 2004 genta �����R�[�h�ݒ�͈̔̓`�F�b�N
			if(	IsValidCodeType(nCharCode) && nCharCode != pcEditView->m_pcEditDoc->m_nCharCode ){
				pcEditView->m_pcEditDoc->m_nCharCode = nCharCode;
				//	From Here Jul. 26, 2003 ryoji BOM��Ԃ�������
				switch( pcEditView->m_pcEditDoc->m_nCharCode ){
				case CODE_UNICODE:
				case CODE_UNICODEBE:
					pcEditView->m_pcEditDoc->m_bBomExist = true;
					break;
				case CODE_UTF8:
				default:
					pcEditView->m_pcEditDoc->m_bBomExist = false;
					break;
				}
				//	To Here Jul. 26, 2003 ryoji BOM��Ԃ�������
			}

			// ���s�R�[�h
			switch (nSaveLineCode){
			case 0:		pcEditView->m_pcEditDoc->m_cSaveLineCode = EOL_NONE;	break;
			case 1:		pcEditView->m_pcEditDoc->m_cSaveLineCode = EOL_CRLF;	break;
			case 2:		pcEditView->m_pcEditDoc->m_cSaveLineCode = EOL_LF;		break;
			case 3:		pcEditView->m_pcEditDoc->m_cSaveLineCode = EOL_CR;		break;
			default:	pcEditView->m_pcEditDoc->m_cSaveLineCode = EOL_NONE;	break;
			}
			
			pcEditView->HandleCommand( Index, false, (LPARAM)Argument[0], 0, 0, 0);
			pcEditView->m_pcEditDoc->m_cSaveLineCode = EOL_NONE;	// 2008.03.20 ryoji ���s�R�[�h�͈����p���Ȃ��i�㏑���ۑ��ł͏�Ɂu�ϊ��Ȃ��v�ɂȂ�悤�Ɂj
		}
		break;
	// Jul. 5, 2002 genta
	case F_EXTHTMLHELP:
	case F_EXECEXTMACRO:				// 2008.10.22 syat
		pcEditView->HandleCommand( Index, false, (LPARAM)Argument[0], (LPARAM)Argument[1], 0, 0);
		break;
	//	From Here Dec. 4, 2002 genta
	case F_FILE_REOPEN				://�J������
	case F_FILE_REOPEN_SJIS			://SJIS�ŊJ������
	case F_FILE_REOPEN_JIS			://JIS�ŊJ������
	case F_FILE_REOPEN_EUC			://EUC�ŊJ������
	case F_FILE_REOPEN_UNICODE		://Unicode�ŊJ������
	case F_FILE_REOPEN_UNICODEBE	://UnicodeBE�ŊJ������
	case F_FILE_REOPEN_UTF8			://UTF-8�ŊJ������
	case F_FILE_REOPEN_UTF7			://UTF-7�ŊJ������
		{
			int noconfirm = 0;
			if (Argument[0] != NULL){
				noconfirm = ( atoi( Argument[0] ) != 0 );
			}
			pcEditView->HandleCommand( Index, false, noconfirm, 0, 0, 0 );
		}
		break;
	//	To Here Dec. 4, 2002 genta
	case F_TOPMOST:
		{
			int lparam1;
			if( Argument[0] != NULL ){
				lparam1 = atoi( Argument[0] );
				pcEditView->HandleCommand( Index, false, lparam1, 0, 0, 0 );
			}
		}
		break;	//	Jan. 29, 2005 genta �����Ă���
	case F_TAGJUMP_KEYWORD:	// @@ 2005.03.31 MIK
		{
			//������NULL�ł�OK
			pcEditView->HandleCommand( Index, false, (LPARAM)Argument[0], 0, 0, 0);
		}
		break;
	case F_NEXTWINDOW:
	case F_PREVWINDOW:
		pcEditView->m_pcEditDoc->HandleCommand( Index );	// 2009.04.11 ryoji F_NEXTWINDOW/F_PREVWINDOW�����삵�Ȃ������̂��C��
		break;
	case F_CHGTABWIDTH:		//  �^�u�T�C�Y���擾�A�ݒ肷��i�L�[�}�N���ł͎擾�͖��Ӗ��j
	case F_CHGWRAPCOLUMN:	//  �܂�Ԃ������擾�A�ݒ肷��i�L�[�}�N���ł͎擾�͖��Ӗ��j
		{
			VARIANT vArg[1];			// HandleFunction�ɓn������
			VARIANT vResult;			// HandleFunction����Ԃ�l
			//	��ڂ̈��������l�B
			vArg[0].vt = VT_I4;
			vArg[0].intVal = (Argument[0] != NULL ? atoi(Argument[0]) : 0 );
			HandleFunction( pcEditView, Index, vArg, 1, vResult );
		}
		break;
	case F_SETFONTSIZE:
		{
			int val0 = Argument[0] != NULL ? atoi(Argument[0]) : 0;
			int val1 = Argument[1] != NULL ? atoi(Argument[1]) : 0;
			pcEditView->HandleCommand( Index, false, (LPARAM)val0, (LPARAM)val1, 0, 0 );
		}
		break;
	default:
		//	�����Ȃ��B
		pcEditView->HandleCommand( Index, false, 0, 0, 0, 0 );	//	�W��
		break;
	}
}

/**	�l��Ԃ��֐�����������

	@param View      [in] �ΏۂƂȂ�View
	@param ID        [in] ����16bit: �֐��ԍ�
	@param Arguments [in] �����̔z��
	@param ArgSize   [in] �����̐�(Argument)
	@param Result  [out] ���ʂ̒l��Ԃ��ꏊ�B�߂�l��false�̂Ƃ��͕s��B
	
	@return true: ����, false: ���s

	@author �S
	@date 2003.02.21 �S
	@date 2003.06.01 Moca �֐��ǉ�
	@date 2005.08.05 maru,zenryaku �֐��ǉ�
	@date 2005.11.29 FILE VariantChangeType�Ή�
*/
bool CMacro::HandleFunction(CEditView *View, int ID, VARIANT *Arguments, int ArgSize, VARIANT &Result)
{
	Variant varCopy;	// VT_BYREF���ƍ���̂ŃR�s�[�p

	//2003-02-21 �S
	switch(LOWORD(ID))
	{
	case F_GETFILENAME:
		{
			const TCHAR* FileName = View->m_pcEditDoc->GetFilePath();
			SysString S(FileName, _tcslen(FileName));
			Wrap(&Result)->Receive(S);
		}
		return true;
	case F_GETSAVEFILENAME:
		//	2006.09.04 ryoji �ۑ����̃t�@�C���̃p�X
		{
			const TCHAR* FileName = View->m_pcEditDoc->GetSaveFilePath();
			SysString S(FileName, _tcslen(FileName));
			Wrap(&Result)->Receive(S);
		}
		return true;
	case F_GETSELECTED:
		{
			if(View->IsTextSelected())
			{
				CMemory cMem;
				if(!View->GetSelectedData(&cMem, FALSE, NULL, FALSE, false)) return false;
				SysString S(cMem.GetStringPtr(), cMem.GetStringLength());
				Wrap(&Result)->Receive(S);
			}
			else
			{
				Result.vt = VT_BSTR;
				Result.bstrVal = SysAllocString(L"");
			}
		}
		return true;
	case F_EXPANDPARAMETER:
		// 2003.02.24 Moca
		{
			if(ArgSize != 1) return false;
			if(VariantChangeType(&varCopy.Data, &(Arguments[0]), 0, VT_BSTR) != S_OK) return false;	// VT_BSTR�Ƃ��ĉ���
			//void ExpandParameter(const char* pszSource, char* pszBuffer, int nBufferLen);
			//pszSource��W�J���āApszBuffer�ɃR�s�[
			char *Source;
			int SourceLength;
			Wrap(&varCopy.Data.bstrVal)->Get(&Source, &SourceLength);
			char Buffer[2048];
			View->m_pcEditDoc->ExpandParameter(Source, Buffer, 2047);
			delete[] Source;
			SysString S(Buffer, _tcslen(Buffer));
			Wrap(&Result)->Receive(S);
		}
		return true;
	case F_GETLINESTR:
		//	2003.06.01 Moca �}�N���ǉ�
		{
			if( ArgSize != 1 ) return false;
			if(VariantChangeType(&varCopy.Data, &(Arguments[0]), 0, VT_I4) != S_OK) return false;	// VT_I4�Ƃ��ĉ���
			if( -1 < varCopy.Data.lVal ){
				const char *Buffer;
				int nLength, nLine;
				if( 0 == varCopy.Data.lVal ){
					nLine = View->m_ptCaretPos_PHY.y;
				}else{
					nLine = varCopy.Data.lVal - 1;
				}
				Buffer = View->m_pcEditDoc->m_cDocLineMgr.GetLineStr( nLine, &nLength );
				if( Buffer != NULL ){
					SysString S( Buffer, nLength );
					Wrap( &Result )->Receive( S );
				}else{
					Result.vt = VT_BSTR;
					Result.bstrVal = SysAllocString(L"");
				}
			}else{
				return false;
			}
		}
		return true;
	case F_GETLINECOUNT:
		//	2003.06.01 Moca �}�N���ǉ�
		{
			if( ArgSize != 1 ) return false;
			if(VariantChangeType(&varCopy.Data, &(Arguments[0]), 0, VT_I4) != S_OK) return false;	// VT_I4�Ƃ��ĉ���
			if( 0 == varCopy.Data.lVal ){
				int nLineCount;
				nLineCount = View->m_pcEditDoc->m_cDocLineMgr.GetLineCount();
				Wrap( &Result )->Receive( nLineCount );
			}else{
				return false;
			}
		}
		return true;
	case F_CHGTABWIDTH:
		//	2004.03.16 zenryaku �}�N���ǉ�
		{
			if( ArgSize != 1 ) return false;
			if(VariantChangeType(&varCopy.Data, &(Arguments[0]), 0, VT_I4) != S_OK) return false;	// VT_I4�Ƃ��ĉ���
			Wrap( &Result )->Receive( View->m_pcEditDoc->m_cLayoutMgr.GetTabSpace() );
			View->m_pcEditDoc->ChangeLayoutParam(
				false, 
				varCopy.Data.iVal,
				View->m_pcEditDoc->m_cLayoutMgr.GetMaxLineKetas()
			);

			// 2009.08.28 nasukoji	�u�܂�Ԃ��Ȃ��v�I������TAB�����ύX���ꂽ��e�L�X�g�ő啝�̍ĎZ�o���K�v
			if( View->m_pcEditDoc->m_nTextWrapMethodCur == WRAP_NO_TEXT_WRAP && varCopy.Data.iVal ){
				// �ő啝�̍ĎZ�o���Ɋe�s�̃��C�A�E�g���̌v�Z���s��
				View->m_pcEditDoc->m_cLayoutMgr.CalculateTextWidth();
				View->m_pcEditDoc->RedrawAllViews( NULL );		// �X�N���[���o�[�̍X�V���K�v�Ȃ̂ōĕ\�������s����
			}
		}
		return true;
	case F_ISTEXTSELECTED:
		//	2005.07.30 maru �}�N���ǉ�
		{
			if(View->IsTextSelected()) {
				if(View->m_bBeginBoxSelect) {
					Wrap( &Result )->Receive( 2 );	//��`�I��
				} else {
					Wrap( &Result )->Receive( 1 );	//�I��
				}
			} else {
				Wrap( &Result )->Receive( 0 );		//��I��
			}
		}
		return true;
	case F_GETSELLINEFROM:
		//	2005.07.30 maru �}�N���ǉ�
		{
			Wrap( &Result )->Receive( View->m_sSelect.m_ptFrom.y + 1 );
		}
		return true;
	case F_GETSELCOLUMNFROM:
		//	2005.07.30 maru �}�N���ǉ�
		{
			Wrap( &Result )->Receive( View->m_sSelect.m_ptFrom.x + 1 );
		}
		return true;
	case F_GETSELLINETO:
		//	2005.07.30 maru �}�N���ǉ�
		{
			Wrap( &Result )->Receive( View->m_sSelect.m_ptTo.y + 1 );
		}
		return true;
	case F_GETSELCOLUMNTO:
		//	2005.07.30 maru �}�N���ǉ�
		{
			Wrap( &Result )->Receive( View->m_sSelect.m_ptTo.x + 1);
		}
		return true;
	case F_ISINSMODE:
		//	2005.07.30 maru �}�N���ǉ�
		{
			Wrap( &Result )->Receive(View->IsInsMode() /* Oct. 2, 2005 genta */);
		}
		return true;
	case F_GETCHARCODE:
		//	2005.07.31 maru �}�N���ǉ�
		{
			Wrap( &Result )->Receive(View->m_pcEditDoc->m_nCharCode);
		}
		return true;
	case F_GETLINECODE:
		//	2005.08.04 maru �}�N���ǉ�
		{
			switch( View->m_pcEditDoc->GetNewLineCode() ){
			case EOL_CRLF:
				Wrap( &Result )->Receive( 0 );
				break;
			case EOL_CR:
				Wrap( &Result )->Receive( 1 );
				break;
			case EOL_LF:
				Wrap( &Result )->Receive( 2 );
				break;
			}
		}
		return true;
	case F_ISPOSSIBLEUNDO:
		//	2005.08.04 maru �}�N���ǉ�
		{
			Wrap( &Result )->Receive( View->m_pcEditDoc->IsEnableUndo() );
		}
		return true;
	case F_ISPOSSIBLEREDO:
		//	2005.08.04 maru �}�N���ǉ�
		{
			Wrap( &Result )->Receive( View->m_pcEditDoc->IsEnableRedo() );
		}
		return true;
	case F_CHGWRAPCOLUMN:
		//	2008.06.19 ryoji �}�N���ǉ�
		{
			if( ArgSize != 1 ) return false;
			if(VariantChangeType(&varCopy.Data, &(Arguments[0]), 0, VT_I4) != S_OK) return false;	// VT_I4�Ƃ��ĉ���
			Wrap( &Result )->Receive( View->m_pcEditDoc->m_cLayoutMgr.GetMaxLineKetas() );
			if( varCopy.Data.iVal < MINLINEKETAS || varCopy.Data.iVal > MAXLINEKETAS )
				return true;
			View->m_pcEditDoc->m_nTextWrapMethodCur = WRAP_SETTING_WIDTH;
			View->m_pcEditDoc->m_bTextWrapMethodCurTemp = !( View->m_pcEditDoc->m_nTextWrapMethodCur == View->m_pcEditDoc->GetDocumentAttribute().m_nTextWrapMethod );
			View->m_pcEditDoc->ChangeLayoutParam(
				false, 
				View->m_pcEditDoc->m_cLayoutMgr.GetTabSpace(),
				varCopy.Data.iVal
			);
		}
		return true;
	case F_ISCURTYPEEXT:
		//	2006.09.04 ryoji �w�肵���g���q�����݂̃^�C�v�ʐݒ�Ɋ܂܂�Ă��邩�ǂ����𒲂ׂ�
		{
			if( ArgSize != 1 ) return false;

			TCHAR *Source;
			int SourceLength;

			int nType1 = View->m_pcEditDoc->GetDocumentType();	// ���݂̃^�C�v

			if(VariantChangeType(&varCopy.Data, &(Arguments[0]), 0, VT_BSTR) != S_OK) return false;	// VT_BSTR�Ƃ��ĉ���
			Wrap(&varCopy.Data.bstrVal)->Get(&Source, &SourceLength);
			int nType2 = CShareData::getInstance()->GetDocumentTypeOfExt(Source);	// �w��g���q�̃^�C�v
			delete[] Source;

			Wrap( &Result )->Receive( (nType1 == nType2)? 1: 0 );	// �^�C�v�ʐݒ�̈�v�^�s��v
		}
		return true;
	case F_ISSAMETYPEEXT:
		//	2006.09.04 ryoji �Q�̊g���q�������^�C�v�ʐݒ�Ɋ܂܂�Ă��邩�ǂ����𒲂ׂ�
		{
			if( ArgSize != 2 ) return false;

			TCHAR *Source;
			int SourceLength;

			if(VariantChangeType(&varCopy.Data, &(Arguments[0]), 0, VT_BSTR) != S_OK) return false;	// VT_BSTR�Ƃ��ĉ���
			Wrap(&varCopy.Data.bstrVal)->Get(&Source, &SourceLength);
			int nType1 = CShareData::getInstance()->GetDocumentTypeOfExt(Source);	// �g���q�P�̃^�C�v
			delete[] Source;

			if(VariantChangeType(&varCopy.Data, &(Arguments[1]), 0, VT_BSTR) != S_OK) return false;	// VT_BSTR�Ƃ��ĉ���
			Wrap(&varCopy.Data.bstrVal)->Get(&Source, &SourceLength);
			int nType2 = CShareData::getInstance()->GetDocumentTypeOfExt(Source);	// �g���q�Q�̃^�C�v
			delete[] Source;

			Wrap( &Result )->Receive( (nType1 == nType2)? 1: 0 );	// �^�C�v�ʐݒ�̈�v�^�s��v
		}
		return true;
	case F_INPUTBOX:
		//	2011.03.18 syat �e�L�X�g���̓_�C�A���O�̕\��
		{
			if( ArgSize < 1 ) return false;
			TCHAR *Source;
			int SourceLength;

			if(VariantChangeType(&varCopy.Data, const_cast<VARIANTARG*>( &(Arguments[0]) ), 0, VT_BSTR) != S_OK) return false;	// VT_BSTR�Ƃ��ĉ���
			Wrap(&varCopy.Data.bstrVal)->Get(&Source, &SourceLength);
			std::tstring sMessage = Source;	// �\�����b�Z�[�W
			delete[] Source;

			std::tstring sDefaultValue = _T("");
			if( ArgSize >= 2 ){
				if(VariantChangeType(&varCopy.Data, const_cast<VARIANTARG*>( &(Arguments[1]) ), 0, VT_BSTR) != S_OK) return false;	// VT_BSTR�Ƃ��ĉ���
				Wrap(&varCopy.Data.bstrVal)->Get(&Source, &SourceLength);
				sDefaultValue = Source;	// �f�t�H���g�l
				delete[] Source;
			}

			int nMaxLen = _MAX_PATH;
			if( ArgSize >= 3 ){
				HRESULT r = VariantChangeType(&varCopy.Data, const_cast<VARIANTARG*>( &(Arguments[2]) ), 0, VT_I4);
				if( r != S_OK) return false;	// VT_I4�Ƃ��ĉ���
				nMaxLen = varCopy.Data.intVal;	// �ő���͒�
				if( nMaxLen <= 0 ){
					nMaxLen = _MAX_PATH;
				}
			}

			TCHAR *Buffer = new TCHAR[ nMaxLen+1 ];
			_tcscpy( Buffer, sDefaultValue.c_str() );
			CDlgInput1 cDlgInput1;
			if( cDlgInput1.DoModal( View->m_hInstance, View->m_hWnd, _T("sakura macro"), sMessage.c_str(), nMaxLen, Buffer ) ) {
				SysString S( Buffer, _tcslen(Buffer) );
				Wrap( &Result )->Receive( S );
			}else{
				Result.vt = VT_BSTR;
				Result.bstrVal = SysAllocString(L"");
			}
			delete[] Buffer;
		}
		return true;
	case F_MESSAGEBOX:	// ���b�Z�[�W�{�b�N�X�̕\��
	case F_ERRORMSG:	// ���b�Z�[�W�{�b�N�X�i�G���[�j�̕\��
	case F_WARNMSG:		// ���b�Z�[�W�{�b�N�X�i�x���j�̕\��
	case F_INFOMSG:		// ���b�Z�[�W�{�b�N�X�i���j�̕\��
	case F_OKCANCELBOX:	// ���b�Z�[�W�{�b�N�X�i�m�F�FOK�^�L�����Z���j�̕\��
	case F_YESNOBOX:	// ���b�Z�[�W�{�b�N�X�i�m�F�F�͂��^�������j�̕\��
		//	2011.03.18 syat ���b�Z�[�W�{�b�N�X�̕\��
		{
			if( ArgSize < 1 ) return false;
			TCHAR *Source;
			int SourceLength;

			if(VariantChangeType(&varCopy.Data, const_cast<VARIANTARG*>( &(Arguments[0]) ), 0, VT_BSTR) != S_OK) return false;	// VT_BSTR�Ƃ��ĉ���
			Wrap(&varCopy.Data.bstrVal)->Get(&Source, &SourceLength);
			std::tstring sMessage = Source;	// �\��������
			delete[] Source;

			UINT uType = 0;		// ���b�Z�[�W�{�b�N�X���
			switch( LOWORD(ID) ) {
			case F_MESSAGEBOX:
				if( ArgSize >= 2 ){
					if(VariantChangeType(&varCopy.Data, const_cast<VARIANTARG*>( &(Arguments[1]) ), 0, VT_I4) != S_OK) return false;	// VT_I4�Ƃ��ĉ���
					uType = varCopy.Data.uintVal;
				}else{
					uType = MB_OK;
				}
				break;
			case F_ERRORMSG:
				uType |= MB_OK | MB_ICONSTOP;
				break;
			case F_WARNMSG:
				uType |= MB_OK | MB_ICONEXCLAMATION;
				break;
			case F_INFOMSG:
				uType |= MB_OK | MB_ICONINFORMATION;
				break;
			case F_OKCANCELBOX:
				uType |= MB_OKCANCEL | MB_ICONQUESTION;
				break;
			case F_YESNOBOX:
				uType |= MB_YESNO | MB_ICONQUESTION;
				break;
			}
			int ret = ::MessageBox( View->m_hWnd, sMessage.c_str(), _T("sakura macro"), uType );
			Wrap( &Result )->Receive( ret );
		}
		return true;
	case F_COMPAREVERSION:
		//	2011.03.18 syat �o�[�W�����ԍ��̔�r
		{
			if( ArgSize != 2 ) return false;
			TCHAR *Source;
			int SourceLength;

			if(VariantChangeType(&varCopy.Data, const_cast<VARIANTARG*>( &(Arguments[0]) ), 0, VT_BSTR) != S_OK) return false;	// VT_BSTR�Ƃ��ĉ���
			Wrap(&varCopy.Data.bstrVal)->Get(&Source, &SourceLength);
			std::tstring sVerA = Source;	// �o�[�W����A
			delete[] Source;

			if(VariantChangeType(&varCopy.Data, const_cast<VARIANTARG*>( &(Arguments[1]) ), 0, VT_BSTR) != S_OK) return false;	// VT_BSTR�Ƃ��ĉ���
			Wrap(&varCopy.Data.bstrVal)->Get(&Source, &SourceLength);
			std::tstring sVerB = Source;	// �o�[�W����B
			delete[] Source;

			Wrap( &Result )->Receive( CompareVersion( sVerA.c_str(), sVerB.c_str() ) );
		}
		return true;
	case F_MACROSLEEP:
		//	2011.03.18 syat �w�肵�����ԁi�~���b�j��~����
		{
			if( ArgSize != 1 ) return false;

			if(VariantChangeType(&varCopy.Data, const_cast<VARIANTARG*>( &(Arguments[0]) ), 0, VT_UI4) != S_OK) return false;	// VT_UI4�Ƃ��ĉ���
			CWaitCursor cWaitCursor( View->m_hWnd );	// �J�[�\���������v�ɂ���
			::Sleep( varCopy.Data.uintVal );
			Wrap( &Result )->Receive( 0 );	//�߂�l�͍��̂Ƃ���0�Œ�
		}
		return true;
	case F_FILEOPENDIALOG:
	case F_FILESAVEDIALOG:
		//	2011.03.18 syat �t�@�C���_�C�A���O�̕\��
		{
			TCHAR *Source;
			int SourceLength;
			std::tstring sDefault;
			std::tstring sFilter;

			if( ArgSize >= 1 ){
				if(VariantChangeType(&varCopy.Data, const_cast<VARIANTARG*>( &(Arguments[0]) ), 0, VT_BSTR) != S_OK) return false;	// VT_BSTR�Ƃ��ĉ���
				Wrap(&varCopy.Data.bstrVal)->Get(&Source, &SourceLength);
				sDefault = Source;	// ����̃t�@�C����
				delete[] Source;
			}

			if( ArgSize >= 2 ){
				if(VariantChangeType(&varCopy.Data, const_cast<VARIANTARG*>( &(Arguments[1]) ), 0, VT_BSTR) != S_OK) return false;	// VT_BSTR�Ƃ��ĉ���
				Wrap(&varCopy.Data.bstrVal)->Get(&Source, &SourceLength);
				sFilter = Source;	// �t�B���^������
				delete[] Source;
			}

			CDlgOpenFile cDlgOpenFile;
			cDlgOpenFile.Create(
				View->m_hInstance, View->m_hWnd,
				sFilter.c_str(),
				sDefault.c_str()
			);
			bool bRet;
			TCHAR szPath[ _MAX_PATH ];
			_tcscpy( szPath, sDefault.c_str() );
			if( LOWORD(ID) == F_FILEOPENDIALOG ){
				bRet = cDlgOpenFile.DoModal_GetOpenFileName( szPath );
			}else{
				bRet = cDlgOpenFile.DoModal_GetSaveFileName( szPath );
			}
			if( bRet ){
				SysString S( szPath, _tcslen(szPath) );
				Wrap( &Result )->Receive( S );
			}else{
				Result.vt = VT_BSTR;
				Result.bstrVal = SysAllocString(L"");
			}
		}
		return true;
	case F_FOLDERDIALOG:
		//	2011.03.18 syat �t�H���_�_�C�A���O�̕\��
		{
			TCHAR *Source;
			int SourceLength;
			std::tstring sMessage;
			std::tstring sDefault;

			if( ArgSize >= 1 ){
				if(VariantChangeType(&varCopy.Data, const_cast<VARIANTARG*>( &(Arguments[0]) ), 0, VT_BSTR) != S_OK) return false;	// VT_BSTR�Ƃ��ĉ���
				Wrap(&varCopy.Data.bstrVal)->Get(&Source, &SourceLength);
				sMessage = Source;	// �\�����b�Z�[�W
				delete[] Source;
			}

			if( ArgSize >= 2 ){
				if(VariantChangeType(&varCopy.Data, const_cast<VARIANTARG*>( &(Arguments[1]) ), 0, VT_BSTR) != S_OK) return false;	// VT_BSTR�Ƃ��ĉ���
				Wrap(&varCopy.Data.bstrVal)->Get(&Source, &SourceLength);
				sDefault = Source;	// ����̃t�@�C����
				delete[] Source;
			}

			TCHAR szPath[ _MAX_PATH ];
			int nRet = SelectDir( View->m_hWnd, sMessage.c_str(), sDefault.c_str(), szPath );
			if( nRet == IDOK ){
				SysString S( szPath, _tcslen(szPath) );
				Wrap( &Result )->Receive( S );
			}else{
				Result.vt = VT_BSTR;
				Result.bstrVal = SysAllocString(L"");
			}
		}
		return true;
	case F_GETCLIPBOARD:
		//	2011.03.18 syat �N���b�v�{�[�h�̕�������擾
		{
			int nOpt = 0;

			if( ArgSize >= 1 ){
				if(VariantChangeType(&varCopy.Data, const_cast<VARIANTARG*>( &(Arguments[0]) ), 0, VT_I4) != S_OK) return false;	// VT_I4�Ƃ��ĉ���
				nOpt = varCopy.Data.intVal;	// �I�v�V����
			}

			CMemory memBuff;
			bool bColumnSelect = false;
			bool bLineSelect = false;
			bool bRet = View->MyGetClipboardData( memBuff, &bColumnSelect, &bLineSelect );
			if( bRet ){
				SysString S( memBuff.GetStringPtr(), memBuff.GetStringLength() );
				Wrap( &Result )->Receive( S );
			}else{
				Result.vt = VT_BSTR;
				Result.bstrVal = SysAllocString(L"");
			}
		}
		return true;
	case F_SETCLIPBOARD:
		//	2011.03.18 syat �N���b�v�{�[�h�ɕ������ݒ�
		{
			TCHAR *Source;
			int SourceLength;
			std::tstring sValue;
			int nOpt = 0;

			if( ArgSize >= 1 ){
				if(VariantChangeType(&varCopy.Data, const_cast<VARIANTARG*>( &(Arguments[0]) ), 0, VT_I4) != S_OK) return false;	// VT_I4�Ƃ��ĉ���
				nOpt = varCopy.Data.intVal;	// �I�v�V����
			}

			if( ArgSize >= 2 ){
				if(VariantChangeType(&varCopy.Data, const_cast<VARIANTARG*>( &(Arguments[1]) ), 0, VT_BSTR) != S_OK) return false;	// VT_BSTR�Ƃ��ĉ���
				Wrap(&varCopy.Data.bstrVal)->Get(&Source, &SourceLength);
				sValue = Source;	// �ݒ肷�镶����
				delete[] Source;
			}

			bool bColumnSelect = false;
			bool bLineSelect = false;
			bool bRet = View->MySetClipboardData( sValue.c_str(), sValue.size(), bColumnSelect, bLineSelect );
			Wrap( &Result )->Receive( bRet );
		}
		return true;
	default:
		return false;
	}
}

/*[EOF]*/