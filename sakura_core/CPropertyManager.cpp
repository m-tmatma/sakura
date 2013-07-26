#include "StdAfx.h"
#include "CPropertyManager.h"
#include "window/CEditWnd.h"
#include "CEditApp.h"
#include "env/DLLSHAREDATA.h"
#include "macro/CSMacroMgr.h"

CPropertyManager::CPropertyManager( HWND hwndOwner, CImageListMgr* pImageList, CMenuDrawer* menu )
{
	/* �ݒ�v���p�e�B�V�[�g�̏������P */
	m_cPropCommon.Create( hwndOwner, pImageList, menu );
	m_cPropTypes.Create( G_AppInstance(), hwndOwner );
}

/*! ���ʐݒ� �v���p�e�B�V�[�g */
BOOL CPropertyManager::OpenPropertySheet( int nPageNum )
{
	// 2002.12.11 Moca ���̕����ōs���Ă����f�[�^�̃R�s�[��CPropCommon�Ɉړ��E�֐���
	// ���ʐݒ�̈ꎞ�ݒ�̈��SharaData���R�s�[����
	m_cPropCommon.InitData();
	
	/* �v���p�e�B�V�[�g�̍쐬 */
	if( m_cPropCommon.DoPropertySheet( nPageNum ) ){

		// 2002.12.11 Moca ���̕����ōs���Ă����f�[�^�̃R�s�[��CPropCommon�Ɉړ��E�֐���
		// ShareData �� �ݒ��K�p�E�R�s�[����
		// 2007.06.20 ryoji �O���[�v���ɕύX���������Ƃ��̓O���[�vID�����Z�b�g����
		BOOL bGroup = (GetDllShareData().m_Common.m_sTabBar.m_bDispTabWnd && !GetDllShareData().m_Common.m_sTabBar.m_bDispTabWndMultiWin);

		// ������ɃL�[���[�h���㏑�����Ȃ��悤��
		CShareDataLockCounter* pLock = NULL;
		CShareDataLockCounter::WaitLock( m_cPropCommon.m_hwndParent, &pLock );

		m_cPropCommon.ApplyData();
		// note: ��{�I�ɂ����œK�p���Ȃ��ŁAMYWM_CHANGESETTING���炽�ǂ��ēK�p���Ă��������B
		// ���E�B���h�E�ɂ͍Ō�ɒʒm����܂��B���́AOnChangeSetting �ɂ���܂��B
		// �����ł����K�p���Ȃ��ƁA�ق��̃E�B���h�E���ύX����܂���B
		
		if( CEditApp::getInstance() ){
			CEditApp::getInstance()->m_pcSMacroMgr->UnloadAll();	// 2007.10.19 genta �}�N���o�^�ύX�𔽉f���邽�߁C�ǂݍ��ݍς݂̃}�N����j������
		}
		if( bGroup != (GetDllShareData().m_Common.m_sTabBar.m_bDispTabWnd && !GetDllShareData().m_Common.m_sTabBar.m_bDispTabWndMultiWin ) ){
			CAppNodeManager::getInstance()->ResetGroupId();
		}

		/* �A�N�Z�����[�^�e�[�u���̍č쐬 */
		::SendMessageAny( GetDllShareData().m_sHandles.m_hwndTray, MYWM_CHANGESETTING,  (WPARAM)0, (LPARAM)PM_CHANGESETTING_ALL );


		/* �ݒ�ύX�𔽉f������ */
		HWND hWnd = NULL;
		if( CEditWnd::getInstance() ){
			hWnd = CEditWnd::getInstance()->GetHwnd();
		}
		/* �S�ҏW�E�B���h�E�փ��b�Z�[�W���|�X�g���� */
		CAppNodeGroupHandle(0).SendMessageToAllEditors(
			MYWM_CHANGESETTING,
			(WPARAM)0,
			(LPARAM)PM_CHANGESETTING_ALL,
			hWnd
		);

		delete pLock;
		return TRUE;
	}else{
		return FALSE;
	}
}



/*! �^�C�v�ʐݒ� �v���p�e�B�V�[�g */
BOOL CPropertyManager::OpenPropertySheetTypes( int nPageNum, CTypeConfig nSettingType )
{
	STypeConfig& types = CDocTypeManager().GetTypeSetting(nSettingType);
	m_cPropTypes.SetTypeData( types );
	// Mar. 31, 2003 genta �������팸�̂��߃|�C���^�ɕύX��ProperySheet���Ŏ擾����悤��
	//m_cPropTypes.m_CKeyWordSetMgr = GetDllShareData().m_Common.m_sSpecialKeyword.m_CKeyWordSetMgr;

	/* �v���p�e�B�V�[�g�̍쐬 */
	if( m_cPropTypes.DoPropertySheet( nPageNum ) ){
		/* �ύX���ꂽ�ݒ�l�̃R�s�[ */
		int nTextWrapMethodOld = -1;
		if( CEditWnd::getInstance() ){
			nTextWrapMethodOld = CEditWnd::getInstance()->GetDocument().m_cDocType.GetDocumentAttribute().m_nTextWrapMethod;
		}
		// 2013.06.10 Moca ����I���܂őҋ@����
		CShareDataLockCounter* pLock = NULL;
		CShareDataLockCounter::WaitLock( m_cPropTypes.GetHwndParent(), &pLock );
		
		m_cPropTypes.GetTypeData( types );

		// 2008.06.01 nasukoji	�e�L�X�g�̐܂�Ԃ��ʒu�ύX�Ή�
		// �^�C�v�ʐݒ���Ăяo�����E�B���h�E�ɂ��ẮA�^�C�v�ʐݒ肪�ύX���ꂽ��
		// �܂�Ԃ����@�̈ꎞ�ݒ�K�p�����������ă^�C�v�ʐݒ��L���Ƃ���B
		if( CEditWnd::getInstance() ){
			if( nTextWrapMethodOld != CEditWnd::getInstance()->GetDocument().m_cDocType.GetDocumentAttribute().m_nTextWrapMethod ){		// �ݒ肪�ύX���ꂽ
				CEditWnd::getInstance()->GetDocument().m_bTextWrapMethodCurTemp = false;	// �ꎞ�ݒ�K�p��������
			}
		}

		/* �A�N�Z�����[�^�e�[�u���̍č쐬 */
		::SendMessageAny( GetDllShareData().m_sHandles.m_hwndTray, MYWM_CHANGESETTING,  (WPARAM)0, (LPARAM)PM_CHANGESETTING_ALL );

		/* �ݒ�ύX�𔽉f������ */
		/* �S�ҏW�E�B���h�E�փ��b�Z�[�W���|�X�g���� */
		HWND hWnd = NULL;
		if( CEditWnd::getInstance() ){
			hWnd = CEditWnd::getInstance()->GetHwnd();
		}
		CAppNodeGroupHandle(0).SendMessageToAllEditors(
			MYWM_CHANGESETTING,
			(WPARAM)0,
			(LPARAM)PM_CHANGESETTING_ALL,
			hWnd
		);

		delete pLock;
		return TRUE;
	}else{
		return FALSE;
	}
}
