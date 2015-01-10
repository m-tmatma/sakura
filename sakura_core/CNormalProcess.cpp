/*!	@file
	@brief �G�f�B�^�v���Z�X�N���X

	@author aroka
	@date 2002/01/07 Create
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2000-2001, genta
	Copyright (C) 2002, aroka CProcess��蕪��
	Copyright (C) 2002, YAZAKI, Moca, genta
	Copyright (C) 2003, genta, Moca, MIK
	Copyright (C) 2004, Moca, naoh
	Copyright (C) 2006, ryoji
	Copyright (C) 2007, ryoji, maru
	Copyright (C) 2008, ryoji

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/


#include "StdAfx.h"
#include "CNormalProcess.h"
#include "CCommandLine.h"
#include "CControlTray.h"
#include "CEditWnd.h" // 2002/2/3 aroka
#include "CShareData.h"
#include "CDocLine.h" // 2003/03/28 MIK
#include "etc_uty.h"
#include "mymessage.h" // 2002/2/3 aroka
#include "Debug.h"
#include "CRunningTimer.h"

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
//               �R���X�g���N�^�E�f�X�g���N�^                  //
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

CNormalProcess::CNormalProcess( HINSTANCE hInstance, LPCTSTR lpCmdLine )
: CProcess( hInstance, lpCmdLine )
, m_pcEditApp( NULL )
{
}

CNormalProcess::~CNormalProcess()
{
}


// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
//                     �v���Z�X�n���h��                        //
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

/*!
	@brief �G�f�B�^�v���Z�X������������
	
	CEditWnd���쐬����B
	
	@author aroka
	@date 2002/01/07

	@date 2002.2.17 YAZAKI CShareData�̃C���X�^���X�́ACProcess�ɂЂƂ���̂݁B
	@date 2004.05.13 Moca CEditWnd::Create()�Ɏ��s�����ꍇ��false��Ԃ��悤�ɁD
	@date 2007.06.26 ryoji �O���[�vID���w�肵�ĕҏW�E�B���h�E���쐬����
	@date 2012.02.25 novice �����t�@�C���ǂݍ���
*/
bool CNormalProcess::InitializeProcess()
{
	MY_RUNNINGTIMER( cRunningTimer, "NormalProcess::Init" );

	/* �v���Z�X�������̖ڈ� */
	HANDLE	hMutex = _GetInitializeMutex();	// 2002/2/8 aroka ���ݓ����Ă����̂ŕ���
	if( NULL == hMutex ){
		return false;
	}

	/* ���L������������������ */
	if ( !CProcess::InitializeProcess() ){
		return false;
	}

	/* �R�}���h���C���I�v�V���� */
	bool			bReadOnly;
	bool			bDebugMode;
	bool			bGrepMode;
	bool			bGrepDlg;
	GrepInfo		gi;
	EditInfo		fi;
	
	/* �R�}���h���C���Ŏ󂯎�����t�@�C�����J����Ă���ꍇ�� */
	/* ���̕ҏW�E�B���h�E���A�N�e�B�u�ɂ��� */
	CCommandLine::getInstance()->GetEditInfo(&fi); // 2002/2/8 aroka �����Ɉړ�
	if( fi.m_szPath[0] != _T('\0') ){
		//	Oct. 27, 2000 genta
		//	MRU����J�[�\���ʒu�𕜌����鑀���CEditDoc::FileRead��
		//	�s����̂ł����ł͕K�v�Ȃ��D

		HWND hwndOwner;
		/* �w��t�@�C�����J����Ă��邩���ׂ� */
		// 2007.03.13 maru �����R�[�h���قȂ�Ƃ��̓��[�j���O���o���悤��
		if( m_pcShareData->ActiveAlreadyOpenedWindow( fi.m_szPath, &hwndOwner, fi.m_nCharCode ) ){
			//	From Here Oct. 19, 2001 genta
			//	�J�[�\���ʒu�������Ɏw�肳��Ă�����w��ʒu�ɃW�����v
			if( fi.m_ptCursor.y >= 0 ){	//	�s�̎w�肪���邩
				POINT& pt = *(POINT*)CProcess::m_pShareData->m_sWorkBuffer.m_szWork;
				if( fi.m_ptCursor.x < 0 ){
					//	���̎w�肪�����ꍇ
					::SendMessage( hwndOwner, MYWM_GETCARETPOS, 0, 0 );
				}
				else {
					pt.x = fi.m_ptCursor.x;
				}
				pt.y = fi.m_ptCursor.y;
				::SendMessage( hwndOwner, MYWM_SETCARETPOS, 0, 0 );
			}
			//	To Here Oct. 19, 2001 genta
			/* �A�N�e�B�u�ɂ��� */
			ActivateFrameWindow( hwndOwner );
			::ReleaseMutex( hMutex );
			::CloseHandle( hMutex );
			return false;
		}
	}

	MY_TRACETIME( cRunningTimer, "CheckFile" );

	// �O���[�vID���擾
	int nGroupId = CCommandLine::getInstance()->GetGroupId();
	if( m_pShareData->m_Common.m_sTabBar.m_bNewWindow && nGroupId == -1 ){
		nGroupId = CShareData::getInstance()->GetFreeGroupId();
	}
	// CEditApp���쐬
	m_pcEditApp = CEditApp::getInstance();
	m_pcEditApp->Create( m_hInstance, m_pShareData->m_sHandles.m_hwndTray, nGroupId );
	CEditWnd* pEditWnd = m_pcEditApp->m_pcEditWnd;
	if( NULL == pEditWnd->m_hWnd ){
		::ReleaseMutex( hMutex );
		::CloseHandle( hMutex );
		return false;	// 2009.06.23 ryoji CEditWnd::Create()���s�̂��ߏI��
	}

	/* �R�}���h���C���̉�� */	 // 2002/2/8 aroka �����Ɉړ�
	bDebugMode = CCommandLine::getInstance()->IsDebugMode();
	bGrepMode  = CCommandLine::getInstance()->IsGrepMode();
	bGrepDlg   = CCommandLine::getInstance()->IsGrepDlg();

	// -1: SetDocumentTypeWhenCreate �ł̋����w��Ȃ�
	const int nType = (fi.m_szDocType[0] == '\0' ? -1 : m_pcShareData->GetDocumentTypeOfExt(fi.m_szDocType));

	if( bDebugMode ){
		/* �f�o�b�O���j�^���[�h�ɐݒ� */
		pEditWnd->SetDebugModeON();
		if( !CCommandLine::getInstance()->IsDebugMode() ){
			// �f�o�b�O�ł͂Ȃ���(����)
			CShareData::getInstance()->GetNoNameNumber( pEditWnd->m_hWnd );
			pEditWnd->UpdateCaption();
		}
		// 2004.09.20 naoh �A�E�g�v�b�g�p�^�C�v�ʐݒ�
		// �����R�[�h��L���Ƃ��� Uchi 2008/6/8
		// 2010.06.16 Moca �A�E�g�v�b�g�� CCommnadLine�� -TYPE=output �����Ƃ���
		pEditWnd->SetDocumentTypeWhenCreate( fi.m_nCharCode, false, nType );
	}
	else if( bGrepMode ){
		/* GREP */
		// 2010.06.16 Moca Grep�ł��I�v�V�����w���K�p
		pEditWnd->SetDocumentTypeWhenCreate( fi.m_nCharCode, false, nType );
		CCommandLine::getInstance()->GetGrepInfo(&gi); // 2002/2/8 aroka �����Ɉړ�
		if( !bGrepDlg ){
			TCHAR szWork[MAX_PATH];
			/* �����O�t�@�C�������擾���� */
			if( ::GetLongFileName( gi.cmGrepFolder.GetStringPtr(), szWork ) ){
				gi.cmGrepFolder.SetString( szWork, _tcslen( szWork ) );
			}
			// 2003.06.23 Moca GREP���s�O��Mutex���J��
			//	�������Ȃ���Grep���I���܂ŐV�����E�B���h�E���J���Ȃ�
			m_hWnd = pEditWnd->m_hWnd;
			::ReleaseMutex( hMutex );
			::CloseHandle( hMutex );
			pEditWnd->GetActiveView().DoGrep(
				&gi.cmGrepKey,
				&gi.cmGrepFile,
				&gi.cmGrepFolder,
				gi.bGrepSubFolder,
				gi.sGrepSearchOption,
				gi.nGrepCharSet,	//	2002/09/21 Moca
				gi.bGrepOutputLine,
				gi.nGrepOutputStyle
			);
			return true; // 2003.06.23 Moca
		}
		else{
			CShareData::getInstance()->GetNoNameNumber( pEditWnd->m_hWnd );
			pEditWnd->UpdateCaption();
			
			//-GREPDLG�Ń_�C�A���O���o���B�@���������f�i2002/03/24 YAZAKI�j
			CShareData::getInstance()->AddToSearchKeyArr( gi.cmGrepKey.GetStringPtr() );
			CShareData::getInstance()->AddToGrepFileArr( gi.cmGrepFile.GetStringPtr() );
			CShareData::getInstance()->AddToGrepFolderArr( gi.cmGrepFolder.GetStringPtr() );
			m_pShareData->m_Common.m_sSearch.m_bGrepSubFolder = gi.bGrepSubFolder;
			m_pShareData->m_Common.m_sSearch.m_sSearchOption = gi.sGrepSearchOption;
			m_pShareData->m_Common.m_sSearch.m_nGrepCharSet = gi.nGrepCharSet;
			m_pShareData->m_Common.m_sSearch.m_bGrepOutputLine = gi.bGrepOutputLine;
			m_pShareData->m_Common.m_sSearch.m_nGrepOutputStyle = gi.nGrepOutputStyle;
			// 2003.06.23 Moca GREP�_�C�A���O�\���O��Mutex���J��
			//	�������Ȃ���Grep���I���܂ŐV�����E�B���h�E���J���Ȃ�
			m_hWnd = pEditWnd->m_hWnd;;
			::ReleaseMutex( hMutex );
			::CloseHandle( hMutex );
			
			//	Oct. 9, 2003 genta �R�}���h���C������GERP�_�C�A���O��\���������ꍇ��
			//	�����̐ݒ肪BOX�ɔ��f����Ȃ�
			_tcscpy( pEditWnd->m_cDlgGrep.m_szText, gi.cmGrepKey.GetStringPtr() );		/* ���������� */
			_tcscpy( pEditWnd->m_cDlgGrep.m_szFile, gi.cmGrepFile.GetStringPtr() );		/* �����t�@�C�� */
			_tcscpy( pEditWnd->m_cDlgGrep.m_szFolder, gi.cmGrepFolder.GetStringPtr() );	/* �����t�H���_ */

			
			// Feb. 23, 2003 Moca Owner window���������w�肳��Ă��Ȃ�����
			int nRet = pEditWnd->m_cDlgGrep.DoModal( m_hInstance, pEditWnd->m_hWnd, NULL);
			if( FALSE != nRet ){
				pEditWnd->GetActiveView().HandleCommand(F_GREP, true, 0, 0, 0, 0);
			}
			return true; // 2003.06.23 Moca
		}
	}
	else{
		// 2004.05.13 Moca �����if���̒�����O�Ɉړ�
		// �t�@�C�������^�����Ȃ��Ă�ReadOnly�w���L���ɂ��邽�߁D
		bReadOnly = CCommandLine::getInstance()->IsReadOnly(); // 2002/2/8 aroka �����Ɉړ�
		if( fi.m_szPath[0] != _T('\0') ){
			//	Mar. 9, 2002 genta �����^�C�v�w��
			pEditWnd->OpenDocumentWhenStart(
				fi.m_szPath,
				fi.m_nCharCode,
				bReadOnly
			);
			//	Nov. 6, 2000 genta
			//	�L�����b�g�ʒu�̕����̂���
			//	�I�v�V�����w�肪�Ȃ��Ƃ��͉�ʈړ����s��Ȃ��悤�ɂ���
			//	Oct. 19, 2001 genta
			//	���ݒ聁-1�ɂȂ�悤�ɂ����̂ŁC���S�̂��ߗ��҂��w�肳�ꂽ�Ƃ�����
			//	�ړ�����悤�ɂ���D || �� &&
			if( ( 0 <= fi.m_nViewTopLine && 0 <= fi.m_nViewLeftCol )
				&& fi.m_nViewTopLine < pEditWnd->m_pcEditDoc->m_cLayoutMgr.GetLineCount() ){
				pEditWnd->GetActiveView().m_nViewTopLine = fi.m_nViewTopLine;
				pEditWnd->GetActiveView().m_nViewLeftCol = fi.m_nViewLeftCol;
			}

			//	�I�v�V�����w�肪�Ȃ��Ƃ��̓J�[�\���ʒu�ݒ���s��Ȃ��悤�ɂ���
			//	Oct. 19, 2001 genta
			//	0���ʒu�Ƃ��Ă͗L���Ȓl�Ȃ̂Ŕ���Ɋ܂߂Ȃ��Ă͂Ȃ�Ȃ�
			if( 0 <= fi.m_ptCursor.x || 0 <= fi.m_ptCursor.y ){
				/*
				  �J�[�\���ʒu�ϊ�
				  �����ʒu(�s������̃o�C�g���A�܂�Ԃ������s�ʒu)
				  ��
				  ���C�A�E�g�ʒu(�s������̕\�����ʒu�A�܂�Ԃ�����s�ʒu)
				*/
				int		nPosX;
				int		nPosY;
				pEditWnd->m_pcEditDoc->m_cLayoutMgr.LogicToLayout(
					fi.m_ptCursor.x,
					fi.m_ptCursor.y,
					&nPosX,
					&nPosY
				);

				// 2004.04.03 Moca EOF�����̍s�ŏI����Ă���ƁAEOF�̈��̍s�Ɉړ����Ă��܂��o�O�C��
				// MoveCursor���␳����̂ł�����x�s��Ȃ��ėǂ��Ȃ���
				// From Here Mar. 28, 2003 MIK
				// ���s�̐^�񒆂ɃJ�[�\�������Ȃ��悤�ɁB
				// 2008.08.20 ryoji ���s�P�ʂ̍s�ԍ���n���悤�ɏC��
				const CDocLine *pTmpDocLine = pEditWnd->m_pcEditDoc->m_cDocLineMgr.GetLine( fi.m_ptCursor.y );
				if( pTmpDocLine ){
					if( pTmpDocLine->GetLengthWithoutEOL() < fi.m_ptCursor.x ) nPosX--;
				}
				// To Here Mar. 28, 2003 MIK

				pEditWnd->GetActiveView().MoveCursor( nPosX, nPosY, true );
				pEditWnd->GetActiveView().m_nCaretPosX_Prev =
					pEditWnd->GetActiveView().m_ptCaretPos.x;
			}
			pEditWnd->GetActiveView().RedrawAll();
		}
		else{
			// 2004.05.13 Moca �t�@�C�������^�����Ȃ��Ă�ReadOnly�ƃ^�C�v�w���L���ɂ���
			pEditWnd->SetDocumentTypeWhenCreate(
				fi.m_nCharCode,
				bReadOnly,	// �ǂݎ���p��
				nType
			);
		}
		if( !pEditWnd->m_pcEditDoc->IsValidPath() ){
			CShareData::getInstance()->GetNoNameNumber( pEditWnd->m_hWnd );
			pEditWnd->UpdateCaption();
		}
	}

	m_hWnd = pEditWnd->m_hWnd;

	//	YAZAKI 2002/05/30 IME�E�B���h�E�̈ʒu�����������̂��C���B
	pEditWnd->GetActiveView().SetIMECompFormPos();

	//�ĕ`��
	::InvalidateRect( pEditWnd->m_hWnd, NULL, TRUE );

	::ReleaseMutex( hMutex );
	::CloseHandle( hMutex );

	// 2006.09.03 ryoji �I�[�v���㎩�����s�}�N�������s����
	if( pEditWnd->m_hWnd && !( bDebugMode || bGrepMode ) )
		pEditWnd->m_pcEditDoc->RunAutoMacro( m_pShareData->m_Common.m_sMacro.m_nMacroOnOpened );


	// �N�����}�N���I�v�V����
	LPCSTR pszMacro = CCommandLine::getInstance()->GetMacro();
	if( pEditWnd->m_hWnd  &&  pszMacro  &&  pszMacro[0] != '\0' ){
		LPCSTR pszMacroType = CCommandLine::getInstance()->GetMacroType();
		if( pszMacroType == NULL || pszMacroType[0] == '\0' || strcmpi(pszMacroType, "file") == 0 ){
			pszMacroType = NULL;
		}
		CEditView* view = &pEditWnd->GetActiveView();
		view->HandleCommand( F_EXECEXTMACRO, true, (LPARAM)pszMacro, (LPARAM)pszMacroType, 0, 0 );
	}

	// �����t�@�C���ǂݍ���
	int fileNum = CCommandLine::getInstance()->GetFileNum();
	if( fileNum > 0 ){
		int nDropFileNumMax = m_pShareData->m_Common.m_sFile.m_nDropFileNumMax - 1;
		// �t�@�C���h���b�v���̏���ɍ��킹��
		if( fileNum > nDropFileNumMax ){
			fileNum = nDropFileNumMax;
		}
		EditInfo openFileInfo = fi;
		int i;
		for( i = 0; i < fileNum; i++ ){
			// �t�@�C���������ւ�
			_tcscpy(openFileInfo.m_szPath, CCommandLine::getInstance()->GetFileName(i));
			bool ret = CControlTray::OpenNewEditor2( m_hInstance, pEditWnd->m_hWnd, &openFileInfo, bReadOnly );
			if( ret == false ){
				break;
			}
		}
		// �p�ς݂Ȃ̂ō폜
		CCommandLine::getInstance()->ClearFile();
	}

	return pEditWnd->m_hWnd ? true : false;
}

/*!
	@brief �G�f�B�^�v���Z�X�̃��b�Z�[�W���[�v
	
	@author aroka
	@date 2002/01/07
*/
bool CNormalProcess::MainLoop()
{
	if( m_hWnd ){
		m_pcEditApp->m_pcEditWnd->MessageLoop();	/* ���b�Z�[�W���[�v */
		return true;
	}
	return false;
}

/*!
	@brief �G�f�B�^�v���Z�X���I������
	
	@author aroka
	@date 2002/01/07
	�����͂Ȃɂ����Ȃ��B��n����dtor�ŁB
*/
void CNormalProcess::OnExitProcess()
{
}



// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
//                         �����⏕                            //
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

/*!
	@brief Mutex(�v���Z�X�������̖ڈ�)���擾����

	���������ɋN������ƃE�B���h�E���\�ɏo�Ă��Ȃ����Ƃ�����B
	
	@date 2002/2/8 aroka InitializeProcess����ړ�
	@retval Mutex �̃n���h����Ԃ�
	@retval ���s�������̓����[�X���Ă��� NULL ��Ԃ�
*/
HANDLE CNormalProcess::_GetInitializeMutex() const
{
	MY_RUNNINGTIMER( cRunningTimer, "NormalProcess::_GetInitializeMutex" );
	HANDLE hMutex;
	hMutex = ::CreateMutex( NULL, TRUE, GSTR_MUTEX_SAKURA_INIT );
	if( NULL == hMutex ){
		ErrorBeep();
		TopErrorMessage( NULL, _T("CreateMutex()���s�B\n�I�����܂��B") );
		return NULL;
	}
	if( ::GetLastError() == ERROR_ALREADY_EXISTS ){
		DWORD dwRet = ::WaitForSingleObject( hMutex, 15000 );	// 2002/2/8 aroka ������������
		if( WAIT_TIMEOUT == dwRet ){// �ʂ̒N�����N����
			TopErrorMessage( NULL, _T("�G�f�B�^�܂��̓V�X�e�����r�W�[��Ԃł��B\n���΂炭�҂��ĊJ���Ȃ����Ă��������B") );
			::CloseHandle( hMutex );
			return NULL;
		}
	}
	return hMutex;
}

/*[EOF]*/