/*!	@file
	@brief DIFF�����\���_�C�A���O�{�b�N�X

	@author MIK
	@date 2002.5.27
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2001, Stonee, genta, JEPRO, YAZAKI
	Copyright (C) 2002, aroka, MIK, Moca
	Copyright (C) 2003, MIK, genta
	Copyright (C) 2004, MIK, genta, ���イ��
	Copyright (C) 2006, ryoji
	Copyright (C) 2009, ryoji

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/

#include "StdAfx.h"
#include "dlg/CDlgDiff.h"
#include "dlg/CDlgOpenFile.h"
#include "window/CEditWnd.h"
#include "func/Funccode.h"
#include "util/shell.h"
#include "util/string_ex2.h"
#include "util/file.h"
#include "sakura_rc.h"
#include "sakura.hh"

const DWORD p_helpids[] = {	//13200
	IDC_BUTTON_DIFF_DST,		HIDC_BUTTON_DIFF_DST,
	IDC_CHECK_DIFF_OPT_BLINE,	HIDC_CHECK_DIFF_OPT_BLINE,
	IDC_CHECK_DIFF_OPT_CASE,	HIDC_CHECK_DIFF_OPT_CASE,
	IDC_CHECK_DIFF_OPT_SPACE,	HIDC_CHECK_DIFF_OPT_SPACE,
	IDC_CHECK_DIFF_OPT_SPCCHG,	HIDC_CHECK_DIFF_OPT_SPCCHG,
	IDC_CHECK_DIFF_OPT_TABSPC,	HIDC_CHECK_DIFF_OPT_TABSPC,
	IDC_EDIT_DIFF_DST,			HIDC_EDIT_DIFF_DST,
	IDC_FRAME_DIFF_FILE12,		HIDC_RADIO_DIFF_FILE1,
	IDC_RADIO_DIFF_FILE1,		HIDC_RADIO_DIFF_FILE1,
	IDC_RADIO_DIFF_FILE2,		HIDC_RADIO_DIFF_FILE2,
	IDC_FRAME_DIFF_DST,			HIDC_RADIO_DIFF_DST1,
	IDC_RADIO_DIFF_DST1,		HIDC_RADIO_DIFF_DST1,
	IDC_RADIO_DIFF_DST2,		HIDC_RADIO_DIFF_DST2,
	IDC_LIST_DIFF_FILES,		HIDC_LIST_DIFF_FILES,
	IDC_STATIC_DIFF_SRC,		HIDC_STATIC_DIFF_SRC,
	IDOK,						HIDC_DIFF_IDOK,
	IDCANCEL,					HIDC_DIFF_IDCANCEL,
	IDC_BUTTON_HELP,			HIDC_BUTTON_DIFF_HELP,
	IDC_CHECK_DIFF_EXEC_STATE,	HIDC_CHECK_DIFF_EXEC_STATE,	//DIFF������������Ȃ��Ƃ��Ƀ��b�Z�[�W��\��  2003.05.12 MIK
	IDC_CHECK_NOTIFYNOTFOUND,	HIDC_CHECK_DIFF_NOTIFYNOTFOUND,	// ������Ȃ��Ƃ��Ƀ��b�Z�[�W��\��	// 2006.10.10 ryoji
	IDC_CHECK_SEARCHALL,		HIDC_CHECK_DIFF_SEARCHALL,		// �擪�i�����j����Č�������	// 2006.10.10 ryoji
//	IDC_FRAME_SEARCH_MSG,		HIDC_FRAME_DIFF_SEARCH_MSG,
//	IDC_STATIC,					-1,
	0, 0
};

static const SAnchorList anchorList[] = {
	{IDC_BUTTON_DIFF_DST,       ANCHOR_RIGHT},
	{IDC_CHECK_DIFF_OPT_BLINE,  ANCHOR_BOTTOM},
	{IDC_CHECK_DIFF_OPT_CASE,   ANCHOR_BOTTOM},
	{IDC_CHECK_DIFF_OPT_SPACE,  ANCHOR_BOTTOM},
	{IDC_CHECK_DIFF_OPT_SPCCHG, ANCHOR_BOTTOM},
	{IDC_CHECK_DIFF_OPT_TABSPC, ANCHOR_BOTTOM},
	{IDC_EDIT_DIFF_DST,         ANCHOR_LEFT_RIGHT},
	{IDC_FRAME_DIFF_FILE12,     ANCHOR_BOTTOM},
	{IDC_RADIO_DIFF_FILE1,      ANCHOR_BOTTOM},
	{IDC_RADIO_DIFF_FILE2,      ANCHOR_BOTTOM},
	{IDC_FRAME_DIFF_DST,        ANCHOR_ALL},
	{IDC_RADIO_DIFF_DST1,		ANCHOR_TOP_LEFT},
	{IDC_RADIO_DIFF_DST2,		ANCHOR_TOP_LEFT},
	{IDC_LIST_DIFF_FILES,       ANCHOR_ALL},
	{IDC_STATIC_DIFF_SRC,       ANCHOR_LEFT_RIGHT},
	{IDOK,                      ANCHOR_BOTTOM},
	{IDCANCEL,                  ANCHOR_BOTTOM},
	{IDC_BUTTON_HELP,           ANCHOR_BOTTOM},
	{IDC_CHECK_DIFF_EXEC_STATE, ANCHOR_BOTTOM},
	{IDC_CHECK_NOTIFYNOTFOUND,  ANCHOR_BOTTOM},
	{IDC_CHECK_SEARCHALL,       ANCHOR_BOTTOM},
	{IDC_FRAME_SEARCH_MSG,      ANCHOR_BOTTOM},
};


CDlgDiff::CDlgDiff()
	: m_nIndexSave( 0 )
{
	/* �T�C�Y�ύX���Ɉʒu�𐧌䂷��R���g���[���� */
	assert( _countof(anchorList) == _countof(m_rcItems) );

	m_szFile1[0] = 0;
	m_szFile2[0] = 0;
	m_nDiffFlgOpt    = 0;
	m_bIsModified    = false;
	m_bIsModifiedDst = false;
	m_hWnd_Dst       = NULL;
	return;
}

/* ���[�_���_�C�A���O�̕\�� */
int CDlgDiff::DoModal(
	HINSTANCE			hInstance,
	HWND				hwndParent,
	LPARAM				lParam,
	const TCHAR*		pszPath,		//���t�@�C��
	bool				bIsModified		//���t�@�C���ҏW���H
)
{
	_tcscpy(m_szFile1, pszPath);
	m_bIsModified = bIsModified;

	return (int)CDialog::DoModal( hInstance, hwndParent, IDD_DIFF, lParam );
}

BOOL CDlgDiff::OnBnClicked( int wID )
{
	switch( wID )
	{
	case IDC_BUTTON_HELP:
		/* �w���v */
		MyWinHelp( GetHwnd(), HELP_CONTEXT, ::FuncID_To_HelpContextID(F_DIFF_DIALOG) );	// 2006.10.10 ryoji MyWinHelp�ɕύX�ɕύX
		return TRUE;

	case IDC_BUTTON_DIFF_DST:	/* �Q�� */
		{
			CDlgOpenFile	cDlgOpenFile;
			TCHAR			szPath[_MAX_PATH];
			_tcscpy( szPath, m_szFile2 );
			/* �t�@�C���I�[�v���_�C�A���O�̏����� */
			cDlgOpenFile.Create(
				m_hInstance,
				GetHwnd(),
				_T("*.*"),
				m_szFile1 /*m_szFile2*/
			);
			if( cDlgOpenFile.DoModal_GetOpenFileName( szPath ) )
			{
				_tcscpy( m_szFile2, szPath );
				::DlgItem_SetText( GetHwnd(), IDC_EDIT_DIFF_DST, m_szFile2 );
				//�O���t�@�C����I����Ԃ�
				::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_DST1, TRUE );
				::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_DST2, FALSE );
				List_SetCurSel( ::GetDlgItem( GetHwnd(), IDC_LIST_DIFF_FILES), -1 );
			}
		}
		return TRUE;

	case IDOK:			/* ���E�ɕ\�� */
		/* �_�C�A���O�f�[�^�̎擾 */
		::EndDialog( GetHwnd(), GetData() );
		return TRUE;

	case IDCANCEL:
		::EndDialog( GetHwnd(), FALSE );
		return TRUE;

	case IDC_RADIO_DIFF_DST1:
		::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_DST2, FALSE );
		//::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_EDIT_DIFF_DST ), TRUE );
		//::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_BUTTON_DIFF_DST ), TRUE );
		//::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_LIST_DIFF_FILES ), FALSE );
		//	Feb. 28, 2004 genta �I�������O�ɑO��̈ʒu���L��
		{
			int n = List_GetCurSel( GetDlgItem( GetHwnd(), IDC_LIST_DIFF_FILES ) );
			if( n != LB_ERR ){
				m_nIndexSave = n;
			}
		}
		List_SetCurSel( ::GetDlgItem( GetHwnd(), IDC_LIST_DIFF_FILES), -1 );
		return TRUE;

	case IDC_RADIO_DIFF_DST2:
		::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_DST1, FALSE );
		//::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_EDIT_DIFF_DST ), FALSE );
		//::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_BUTTON_DIFF_DST ), FALSE );
		//::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_LIST_DIFF_FILES ), TRUE );
		{
			//	Aug. 9, 2003 genta
			//	ListBox���I������Ă��Ȃ�������C�擪�̃t�@�C����I������D
			HWND hwndList = GetDlgItem( GetHwnd(), IDC_LIST_DIFF_FILES );
			if( List_GetCurSel( hwndList ) == LB_ERR )
			{
				List_SetCurSel( hwndList, m_nIndexSave );
			}
		}
		return TRUE;

	case IDC_RADIO_DIFF_FILE1:
		::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_FILE2, FALSE );
		return TRUE;

	case IDC_RADIO_DIFF_FILE2:
		::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_FILE1, FALSE );
		return TRUE;
	}

	/* ���N���X�����o */
	return CDialog::OnBnClicked( wID );
}

/* �_�C�A���O�f�[�^�̐ݒ� */
void CDlgDiff::SetData( void )
{
	//�I�v�V����
	m_nDiffFlgOpt = m_pShareData->m_nDiffFlgOpt;
	if( m_nDiffFlgOpt & 0x0001 ) ::CheckDlgButton( GetHwnd(), IDC_CHECK_DIFF_OPT_CASE,   TRUE );
	if( m_nDiffFlgOpt & 0x0002 ) ::CheckDlgButton( GetHwnd(), IDC_CHECK_DIFF_OPT_SPACE,  TRUE );
	if( m_nDiffFlgOpt & 0x0004 ) ::CheckDlgButton( GetHwnd(), IDC_CHECK_DIFF_OPT_SPCCHG, TRUE );
	if( m_nDiffFlgOpt & 0x0008 ) ::CheckDlgButton( GetHwnd(), IDC_CHECK_DIFF_OPT_BLINE,  TRUE );
	if( m_nDiffFlgOpt & 0x0010 ) ::CheckDlgButton( GetHwnd(), IDC_CHECK_DIFF_OPT_TABSPC, TRUE );

	//�V���t�@�C��
	if( m_nDiffFlgOpt & 0x0020 )
	{
		::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_FILE1, FALSE );
		::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_FILE2, TRUE );
	}
	else
	{
		::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_FILE1, TRUE );
		::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_FILE2, FALSE );
	}
	//::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_FRAME_DIFF_FILE12 ), FALSE );
	//::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_RADIO_DIFF_FILE1 ), FALSE );
	//::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_RADIO_DIFF_FILE2 ), FALSE );

	//DIFF������������Ȃ��Ƃ��Ƀ��b�Z�[�W��\�� 2003.05.12 MIK
	if( m_nDiffFlgOpt & 0x0040 ) ::CheckDlgButton( GetHwnd(), IDC_CHECK_DIFF_EXEC_STATE, TRUE );

	/* ������Ȃ��Ƃ����b�Z�[�W��\�� */
	::CheckDlgButton( GetHwnd(), IDC_CHECK_NOTIFYNOTFOUND, m_pShareData->m_Common.m_sSearch.m_bNOTIFYNOTFOUND );
	
	/* �擪�i�����j����Č��� */
	::CheckDlgButton( GetHwnd(), IDC_CHECK_SEARCHALL, m_pShareData->m_Common.m_sSearch.m_bSearchAll );

	/* �ҏW���̃t�@�C���ꗗ���쐬���� */
	{
		HWND		hwndList;
		int			nRowNum;
		EditNode	*pEditNode;
		EditInfo	*pFileInfo;
		int			i;
		int			nItem;
		WIN_CHAR	szName[_MAX_PATH];
		int			count = 0;
		int			selIndex = 0;
		ECodeType	code;
		int			selCode = CODE_NONE;

		// �����̕����R�[�h���擾
		::SendMessageAny( CEditWnd::getInstance()->GetHwnd(), MYWM_GETFILEINFO, 0, 0 );
		pFileInfo = &m_pShareData->m_sWorkBuffer.m_EditInfo_MYWM_GETFILEINFO;
		code = pFileInfo->m_nCharCode;

		/* ���X�g�̃n���h���擾 */
		hwndList = :: GetDlgItem( GetHwnd(), IDC_LIST_DIFF_FILES );

		/* ���݊J���Ă���ҏW���̃��X�g�����j���[�ɂ��� */
		nRowNum = CAppNodeManager::getInstance()->GetOpenedWindowArr( &pEditNode, TRUE );
		if( nRowNum > 0 )
		{
			// �����X�N���[�����͎��ۂɕ\�����镶����̕����v�����Č��߂�	// 2009.09.26 ryoji
			HDC hDC = ::GetDC( hwndList );
			HFONT hFont = (HFONT)::SendMessageAny(hwndList, WM_GETFONT, 0, 0);
			HFONT hFontOld = (HFONT)::SelectObject(hDC, hFont);
			int nExtent = 0;	// ������̉���
			int score = 0;
			TCHAR		szFile1[_MAX_PATH];
			SplitPath_FolderAndFile(m_szFile1, NULL, szFile1);
			for( i = 0; i < nRowNum; i++ )
			{
				/* �g���C����G�f�B�^�ւ̕ҏW�t�@�C�����v���ʒm */
				::SendMessageAny( pEditNode[i].GetHwnd(), MYWM_GETFILEINFO, 0, 0 );
				pFileInfo = (EditInfo*)&m_pShareData->m_sWorkBuffer.m_EditInfo_MYWM_GETFILEINFO;

				/* �����Ȃ�X�L�b�v */
				if ( pEditNode[i].GetHwnd() == CEditWnd::getInstance()->GetHwnd() )
				{
					// �����`���ɂ��Ă����B�������A�N�Z�X�L�[�ԍ��͂Ȃ�
					CFileNameManager::getInstance()->GetMenuFullLabel_WinListNoEscape( szName, _countof(szName), pFileInfo, pEditNode[i].m_nId, -1 );
					::DlgItem_SetText( GetHwnd(), IDC_STATIC_DIFF_SRC, szName );
					continue;
				}

				// �ԍ��̓E�B���h�E�ꗗ�Ɠ����ԍ����g��
				CFileNameManager::getInstance()->GetMenuFullLabel_WinListNoEscape( szName, _countof(szName), pFileInfo, pEditNode[i].m_nId, i );


				/* ���X�g�ɓo�^���� */
				nItem = ::List_AddString( hwndList, szName );
				List_SetItemData( hwndList, nItem, pEditNode[i].GetHwnd() );
				count++;

				// �������v�Z����
				SIZE sizeExtent;
				if( ::GetTextExtentPoint32( hDC, szName, _tcslen(szName), &sizeExtent ) && sizeExtent.cx > nExtent ){
					nExtent = sizeExtent.cx;
				}

				// �t�@�C������v�̃X�R�A���v�Z����
				TCHAR szFile2[_MAX_PATH];
				SplitPath_FolderAndFile( pFileInfo->m_szPath, NULL, szFile2 );
				int scoreTemp = FileMatchScoreSepExt( szFile1, szFile2 );
				if( score < scoreTemp ||
					(selCode != code && code == pFileInfo->m_nCharCode && score == scoreTemp) ){
					// �X�R�A�̂������̂�I��. �����Ȃ當���R�[�h���������̂�I��
					score = scoreTemp;
					selIndex = nItem;
					selCode = pFileInfo->m_nCharCode;
				}
			}

			delete [] pEditNode;
			// 2002/11/01 Moca �ǉ� ���X�g�r���[�̉�����ݒ�B��������Ȃ��Ɛ����X�N���[���o�[���g���Ȃ�
			::SelectObject(hDC, hFontOld);
			::ReleaseDC( hwndList, hDC );
			List_SetHorizontalExtent( hwndList, nExtent + 8 );

			/* �ŏ���I�� */
			//List_SetCurSel( hwndList, 0 );
		}

		//	From Here 2004.02.22 ���イ��
		//	�J���Ă���t�@�C��������ꍇ�ɂ͏�����Ԃł������D��
		if( count == 0 )
		{
			/* ����t�@�C���̑I�� */
			::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_DST1, TRUE );
			::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_DST2, FALSE );
			/* ���̑��̕ҏW�����X�g�͂Ȃ� */
			::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_RADIO_DIFF_DST2 ), FALSE );
			::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_LIST_DIFF_FILES ), FALSE );
		}
		else
		{
			/* ����t�@�C���̑I�� */
			::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_DST1, FALSE );
			::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_DST2, TRUE );
			//	ListBox���I������Ă��Ȃ�������C�擪�̃t�@�C����I������D
			HWND hwndList = GetDlgItem( GetHwnd(), IDC_LIST_DIFF_FILES );
			if( List_GetCurSel( hwndList ) == LB_ERR )
			{
			    List_SetCurSel( hwndList, selIndex );
			}
		}
		//	To Here 2004.02.22 ���イ��
		//	Feb. 28, 2004 genta ��ԏ��I���ʒu�Ƃ���D
		m_nIndexSave = selIndex;
	}

	return;
}

/* �_�C�A���O�f�[�^�̎擾 */
/* TRUE==����  FALSE==���̓G���[ */
int CDlgDiff::GetData( void )
{
	BOOL	ret = TRUE;

	//DIFF�I�v�V����
	m_nDiffFlgOpt = 0;
	if( ::IsDlgButtonChecked( GetHwnd(), IDC_CHECK_DIFF_OPT_CASE   ) == BST_CHECKED ) m_nDiffFlgOpt |= 0x0001;
	if( ::IsDlgButtonChecked( GetHwnd(), IDC_CHECK_DIFF_OPT_SPACE  ) == BST_CHECKED ) m_nDiffFlgOpt |= 0x0002;
	if( ::IsDlgButtonChecked( GetHwnd(), IDC_CHECK_DIFF_OPT_SPCCHG ) == BST_CHECKED ) m_nDiffFlgOpt |= 0x0004;
	if( ::IsDlgButtonChecked( GetHwnd(), IDC_CHECK_DIFF_OPT_BLINE  ) == BST_CHECKED ) m_nDiffFlgOpt |= 0x0008;
	if( ::IsDlgButtonChecked( GetHwnd(), IDC_CHECK_DIFF_OPT_TABSPC ) == BST_CHECKED ) m_nDiffFlgOpt |= 0x0010;
	//�t�@�C���V��
	if( ::IsDlgButtonChecked( GetHwnd(), IDC_RADIO_DIFF_FILE2      ) == BST_CHECKED ) m_nDiffFlgOpt |= 0x0020;
	//DIFF������������Ȃ��Ƃ��Ƀ��b�Z�[�W��\�� 2003.05.12 MIK
	if( ::IsDlgButtonChecked( GetHwnd(), IDC_CHECK_DIFF_EXEC_STATE ) == BST_CHECKED ) m_nDiffFlgOpt |= 0x0040;
	m_pShareData->m_nDiffFlgOpt = m_nDiffFlgOpt;

	//����t�@�C����
	_tcscpy( m_szFile2, _T("") );
	m_hWnd_Dst = NULL;
	m_bIsModifiedDst = false;
	if( ::IsDlgButtonChecked( GetHwnd(), IDC_RADIO_DIFF_DST1 ) == BST_CHECKED )
	{
		::DlgItem_GetText( GetHwnd(), IDC_EDIT_DIFF_DST, m_szFile2, _countof2(m_szFile2) );
		//	2004.05.19 MIK �O���t�@�C�����w�肳��Ă��Ȃ��ꍇ�̓L�����Z��
		//����t�@�C�����w�肳��ĂȂ���΃L�����Z��
		if( m_szFile2[0] == '\0' ) ret = FALSE;

	}
	else if( ::IsDlgButtonChecked( GetHwnd(), IDC_RADIO_DIFF_DST2 ) == BST_CHECKED )
	{
		HWND		hwndList;
		int			nItem;
		EditInfo	*pFileInfo;

		/* ���X�g���瑊��̃E�C���h�E�n���h�����擾 */
		hwndList = :: GetDlgItem( GetHwnd(), IDC_LIST_DIFF_FILES );
		nItem = List_GetCurSel( hwndList );
		if( nItem != LB_ERR )
		{
			m_hWnd_Dst = (HWND)List_GetItemData( hwndList, nItem );

			/* �g���C����G�f�B�^�ւ̕ҏW�t�@�C�����v���ʒm */
			::SendMessageAny( m_hWnd_Dst, MYWM_GETFILEINFO, 0, 0 );
			pFileInfo = (EditInfo*)&m_pShareData->m_sWorkBuffer.m_EditInfo_MYWM_GETFILEINFO;

			_tcscpy( m_szFile2, pFileInfo->m_szPath );
			m_bIsModifiedDst = pFileInfo->m_bIsModified;
		}
		else
		{
			ret = FALSE;
		}
	}
	else
	{
		ret = FALSE;
	}

	/* ������Ȃ��Ƃ����b�Z�[�W��\�� */
	if( ::IsDlgButtonChecked( GetHwnd(), IDC_CHECK_NOTIFYNOTFOUND ) == BST_CHECKED )
		m_pShareData->m_Common.m_sSearch.m_bNOTIFYNOTFOUND = TRUE;
	else
		m_pShareData->m_Common.m_sSearch.m_bNOTIFYNOTFOUND = FALSE;

	/* �擪�i�����j����Č��� */
	if( ::IsDlgButtonChecked( GetHwnd(), IDC_CHECK_SEARCHALL ) == BST_CHECKED )
		m_pShareData->m_Common.m_sSearch.m_bSearchAll = TRUE;
	else
		m_pShareData->m_Common.m_sSearch.m_bSearchAll = FALSE;

	//����t�@�C�����w�肳��ĂȂ���΃L�����Z��
	//	2004.02.21 MIK ���肪���肾�Ɣ�r�ł��Ȃ��̂Ŕ���폜
	//if( m_szFile2[0] == '\0' ) ret = FALSE;

	return ret;
}

BOOL CDlgDiff::OnLbnSelChange( HWND hwndCtl, int wID )
{
	HWND	hwndList;

	hwndList = GetDlgItem( GetHwnd(), IDC_LIST_DIFF_FILES );

	if( hwndList == hwndCtl )
	{
		::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_DST1, FALSE );
		::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_DST2, TRUE );
		return TRUE;
	}

	/* ���N���X�����o */
	return CDialog::OnLbnSelChange( hwndCtl, wID );
}

BOOL CDlgDiff::OnEnChange( HWND hwndCtl, int wID )
{
	HWND	hwndEdit;

	hwndEdit = GetDlgItem( GetHwnd(), IDC_EDIT_DIFF_DST );

	if( hwndEdit == hwndCtl )
	{
		::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_DST1, TRUE );
		::CheckDlgButton( GetHwnd(), IDC_RADIO_DIFF_DST2, FALSE );
		//	Feb. 28, 2004 genta �I�������O�ɑO��̈ʒu���L�����đI������
		int n = List_GetCurSel( GetDlgItem( GetHwnd(), IDC_LIST_DIFF_FILES ) );
		if( n != LB_ERR ){
			m_nIndexSave = n;
		}
		List_SetCurSel( ::GetDlgItem( GetHwnd(), IDC_LIST_DIFF_FILES), -1 );
		return TRUE;
	}

	/* ���N���X�����o */
	return CDialog::OnEnChange( hwndCtl, wID );
}

LPVOID CDlgDiff::GetHelpIdTable( void )
{
	return (LPVOID)p_helpids;
}


INT_PTR CDlgDiff::DispatchEvent( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
	INT_PTR result;
	result = CDialog::DispatchEvent( hWnd, wMsg, wParam, lParam );

	if( wMsg == WM_GETMINMAXINFO ){
		return OnMinMaxInfo( lParam );
	}
	return result;
}

BOOL CDlgDiff::OnInitDialog( HWND hwndDlg, WPARAM wParam, LPARAM lParam )
{
	_SetHwnd(hwndDlg);

	CreateSizeBox();
	CDialog::OnSize();
	
	LONG style;
		style = ::GetWindowLong( GetDlgItem(GetHwnd(), IDC_FRAME_DIFF_DST ), GWL_EXSTYLE );
		::SetWindowLong( GetDlgItem(GetHwnd(), IDC_FRAME_DIFF_DST ), GWL_EXSTYLE, style | WS_EX_TRANSPARENT );
		style = ::GetWindowLong( GetDlgItem(GetHwnd(), IDC_FRAME_DIFF_FILE12 ), GWL_EXSTYLE );
		::SetWindowLong( GetDlgItem(GetHwnd(), IDC_FRAME_DIFF_FILE12 ), GWL_EXSTYLE, style | WS_EX_TRANSPARENT );
		style = ::GetWindowLong( GetDlgItem(GetHwnd(), IDC_FRAME_SEARCH_MSG ), GWL_EXSTYLE );
		::SetWindowLong( GetDlgItem(GetHwnd(), IDC_FRAME_SEARCH_MSG ), GWL_EXSTYLE, style | WS_EX_TRANSPARENT );

	RECT rc;
	::GetWindowRect( hwndDlg, &rc );
	m_ptDefaultSize.x = rc.right - rc.left;
	m_ptDefaultSize.y = rc.bottom - rc.top;

	for( int i = 0; i < _countof(anchorList); i++){
		GetItemClientRect( anchorList[i].id, m_rcItems[i] );
	}

	RECT rcDialog = GetDllShareData().m_Common.m_sOthers.m_rcDiffDialog;
	if( rcDialog.left != 0 ||
		rcDialog.bottom != 0 ){
		m_xPos = rcDialog.left;
		m_yPos = rcDialog.top;
		m_nWidth = rcDialog.right - rcDialog.left;
		m_nHeight = rcDialog.bottom - rcDialog.top;
	}

	return CDialog::OnInitDialog( hwndDlg, wParam, lParam );
}

BOOL CDlgDiff::OnSize( WPARAM wParam, LPARAM lParam )
{
	/* ���N���X�����o */
	CDialog::OnSize( wParam, lParam );

	::GetWindowRect( GetHwnd(), &GetDllShareData().m_Common.m_sOthers.m_rcDiffDialog );

	RECT  rc;
	POINT ptNew;
	::GetWindowRect( GetHwnd(), &rc );
	ptNew.x = rc.right - rc.left;
	ptNew.y = rc.bottom - rc.top;

	for( int i = 0; i < _countof(anchorList); i++){
		ResizeItem( GetItemHwnd(anchorList[i].id), m_ptDefaultSize, ptNew, m_rcItems[i], anchorList[i].anchor );
	}
	::InvalidateRect( GetHwnd(), NULL, TRUE );
	return TRUE;
}

BOOL CDlgDiff::OnMove( WPARAM wParam, LPARAM lParam )
{
	::GetWindowRect( GetHwnd(), &GetDllShareData().m_Common.m_sOthers.m_rcDiffDialog );
	
	return CDialog::OnMove( wParam, lParam );
}

BOOL CDlgDiff::OnMinMaxInfo( LPARAM lParam )
{
	LPMINMAXINFO lpmmi = (LPMINMAXINFO) lParam;
	lpmmi->ptMinTrackSize.x = m_ptDefaultSize.x;
	lpmmi->ptMinTrackSize.y = m_ptDefaultSize.y;
	lpmmi->ptMaxTrackSize.x = m_ptDefaultSize.x*2;
	lpmmi->ptMaxTrackSize.y = m_ptDefaultSize.y*2;
	return 0;
}