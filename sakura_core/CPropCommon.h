/*!	@file
	@brief ���ʐݒ�_�C�A���O�{�b�N�X�̏���

	@author	Norio Nakatani
	@date 1998/12/24 �V�K�쐬
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2000, genta, jepro
	Copyright (C) 2001, genta
	Copyright (C) 2002, YAZAKI, aroka, Moca
	Copyright (C) 2005, MIK, Moca, aroka
	Copyright (C) 2006, ryoji
	Copyright (C) 2007, genta, ryoji
	Copyright (C) 2008, Uchi
	Copyright (C) 2010, Uchi

	This source code is designed for sakura editor.
	Please contact the copyright holders to use this code for other purpose.
*/

#ifndef SAKURA_PROP_CPROPCOMMON_H_
#define SAKURA_PROP_CPROPCOMMON_H_

#include "CShareData.h"
#include "sakura_rc.h"
#include "CFuncLookup.h"
#include "etc_uty.h"
#include "shell.h"

class CImageListMgr;
class CSMacroMgr;
class CMenuDrawer;// 2002/2/10 aroka to here

/*! �v���p�e�B�V�[�g�ԍ�
	@date 2008.6.22 Uchi #define -> enum �ɕύX	
	@date 2008.6.22 Uchi�����ύX Win,Toolbar,Tab,Statusbar�̏��ɁAFile,FileName ����
*/
enum PropComSheetOrder {
	ID_PROPCOM_PAGENUM_GENERAL = 0,		//!< �S��
	ID_PROPCOM_PAGENUM_WIN,				//!< �E�B���h�E
	ID_PROPCOM_PAGENUM_TOOLBAR,			//!< �c�[���o�[
	ID_PROPCOM_PAGENUM_TAB,				//!< �^�u�o�[
	ID_PROPCOM_PAGENUM_EDIT,			//!< �ҏW
	ID_PROPCOM_PAGENUM_FILE,			//!< �t�@�C��
	ID_PROPCOM_PAGENUM_FILENAME,		//!< �t�@�C�����\��
	ID_PROPCOM_PAGENUM_BACKUP,			//!< �o�b�N�A�b�v
	ID_PROPCOM_PAGENUM_FORMAT,			//!< ����
	ID_PROPCOM_PAGENUM_GREP,			//!< ����
	ID_PROPCOM_PAGENUM_KEYBOARD,		//!< �L�[���蓖��
	ID_PROPCOM_PAGENUM_CUSTMENU,		//!< �J�X�^�����j���[
	ID_PROPCOM_PAGENUM_KEYWORD,			//!< �����L�[���[�h
	ID_PROPCOM_PAGENUM_HELPER,			//!< �x��
	ID_PROPCOM_PAGENUM_MACRO,			//!< �}�N��
	ID_PROPCOM_PAGENUM_MAX,
};
/*-----------------------------------------------------------------------
�N���X�̐錾
-----------------------------------------------------------------------*/
/*!
	@brief ���ʐݒ�_�C�A���O�{�b�N�X�N���X

	1�̃_�C�A���O�{�b�N�X�ɕ����̃v���p�e�B�y�[�W���������\����
	�Ȃ��Ă���ADialog procedure��Event Dispatcher���y�[�W���Ƃɂ���D

	@date 2002.2.17 YAZAKI CShareData�̃C���X�^���X�́ACProcess�ɂЂƂ���̂݁B
*/
class CPropCommon
{
public:
	/*
	||  Constructors
	*/
	CPropCommon();
	~CPropCommon();
	//	Sep. 29, 2001 genta �}�N���N���X��n���悤��;
//@@@ 2002.01.03 YAZAKI m_tbMyButton�Ȃǂ�CShareData����CMenuDrawer�ֈړ��������Ƃɂ��C���B
	void Create( HINSTANCE, HWND, CImageListMgr*, CMenuDrawer* );	/* ������ */

	/*
	||  Attributes & Operations
	*/
	INT_PTR DoPropertySheet( int );	/* �v���p�e�B�V�[�g�̍쐬 */

	// 2002.12.11 Moca �ǉ�
	void InitData( void );		//!< DLLSHAREDATA����ꎞ�f�[�^�̈�ɐݒ�𕡐�����
	void ApplyData( void );		//!< �ꎞ�f�[�^�̈悩���DLLSHAREDATA�ݒ���R�s�[����
	int GetPageNum(){ return m_nPageNum; }

	//
	static INT_PTR CALLBACK DlgProc_page(
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

	//	Jun. 2, 2001 genta
	//	�����ɂ�����Event Handler��protected�G���A�Ɉړ������D

	HINSTANCE			m_hInstance;	/* �A�v���P�[�V�����C���X�^���X�̃n���h�� */
	HWND				m_hwndParent;	/* �I�[�i�[�E�B���h�E�̃n���h�� */
	HWND				m_hwndThis;		/* ���̃_�C�A���O�̃n���h�� */
	PropComSheetOrder	m_nPageNum;
	DLLSHAREDATA*		m_pShareData;
	//	Oct. 16, 2000 genta
	CImageListMgr*	m_pcIcons;	//	Image List
	
	//	Oct. 2, 2001 genta �O���}�N���ǉ��ɔ����C�Ή������̕ʃN���X��
	//	Oct. 15, 2001 genta Lookup�̓_�C�A���O�{�b�N�X���ŕʃC���X�^���X�����悤��
	//	(�����ΏۂƂ��āC�ݒ�pcommon�̈���w���悤�ɂ��邽�߁D)
	CFuncLookup			m_cLookup;

	CMenuDrawer*		m_pcMenuDrawer;
	/*
	|| �_�C�A���O�f�[�^
	*/
	CommonSetting	m_Common;

	// 2005.01.13 MIK �Z�b�g������
	int				m_Types_nKeyWordSetIdx[MAX_TYPES][MAX_KEYWORDSET_PER_TYPE];

protected:
	/*
	||  �����w���p�֐�
	*/
	void OnHelp( HWND, int );	/* �w���v */
	int	SearchIntArr( int , int* , int );
//	void DrawToolBarItemList( DRAWITEMSTRUCT* );	/* �c�[���o�[�{�^�����X�g�̃A�C�e���`�� */
//	void DrawColorButton( DRAWITEMSTRUCT* , COLORREF );	/* �F�{�^���̕`�� */ // 2002.11.09 Moca ���g�p
	BOOL SelectColor( HWND , COLORREF* );	/* �F�I���_�C�A���O */

	//	Jun. 2, 2001 genta
	//	Event Handler, Dialog Procedure�̌�����
	//	Global�֐�������Dialog procedure��class��static method�Ƃ���
	//	�g�ݍ��񂾁D
	//	��������ȉ� Macro�܂Ŕz�u�̌�������static method�̒ǉ�

	//! �ėp�_�C�A���O�v���V�[�W��
	static INT_PTR DlgProc(
		INT_PTR (CPropCommon::*DispatchPage)( HWND, UINT, WPARAM, LPARAM ),
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	typedef	INT_PTR (CPropCommon::*pDispatchPage)( HWND, UINT, WPARAM, LPARAM );

	int nLastPos_Macro; //!< �O��t�H�[�J�X�̂������ꏊ
	int m_nLastPos_FILENAME; //!< �O��t�H�[�J�X�̂������ꏊ �t�@�C�����^�u�p

	//! Message Handler
	INT_PTR DispatchEvent( HWND, UINT, WPARAM, LPARAM );
	void SetData( HWND );	//!< �_�C�A���O�f�[�^�̐ݒ�
	int  GetData( HWND );	//!< �_�C�A���O�f�[�^�̎擾
	void Import( HWND );	//!< �C���|�[�g����
	void Export( HWND );	//!< �G�N�X�|�[�g����
};


/*!
	@brief ���ʐݒ�v���p�e�B�y�[�W�N���X

	1�̃v���p�e�B�y�[�W���ɒ�`
	Dialog procedure��Event Dispatcher���y�[�W���Ƃɂ���D
	�ϐ��̒�`��CPropCommon�ōs��
*/
//==============================================================
//!	�S�ʃy�[�W
class CPropGeneral : CPropCommon
{
public:
	//!	Dialog Procedure
	static INT_PTR CALLBACK DlgProc_page(
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	//! Message Handler
	INT_PTR DispatchEvent( HWND, UINT, WPARAM, LPARAM );
	void SetData( HWND );	//!< �_�C�A���O�f�[�^�̐ݒ�
	int  GetData( HWND );	//!< �_�C�A���O�f�[�^�̎擾
};

//==============================================================
//!	�t�@�C���y�[�W
class CPropFile : CPropCommon
{
public:
	//!	Dialog Procedure
	static INT_PTR CALLBACK DlgProc_page(
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	//! Message Handler
	INT_PTR DispatchEvent( HWND, UINT, WPARAM, LPARAM );
	void SetData( HWND );	//!< �_�C�A���O�f�[�^�̐ݒ�
	int  GetData( HWND );	//!< �_�C�A���O�f�[�^�̎擾

private:
	//	Aug. 21, 2000 genta
	void EnableFilePropInput(HWND hwndDlg);	//	�t�@�C���ݒ��ON/OFF
};

//==============================================================
//!	�L�[���蓖�ăy�[�W
class CPropKeybind : CPropCommon
{
public:
	//!	Dialog Procedure
	static INT_PTR CALLBACK DlgProc_page(
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	//! Message Handler
	INT_PTR DispatchEvent( HWND, UINT, WPARAM, LPARAM );
	void SetData( HWND );	//!< �_�C�A���O�f�[�^�̐ݒ�
	int  GetData( HWND );	//!< �_�C�A���O�f�[�^�̎擾

	void Import( HWND );	//!< �C���|�[�g����
	void Export( HWND );	//!< �G�N�X�|�[�g����

private:
	void ChangeKeyList( HWND ); /* �L�[���X�g���`�F�b�N�{�b�N�X�̏�Ԃɍ��킹�čX�V����*/
};

//==============================================================
//!	�c�[���o�[�y�[�W
class CPropToolbar : CPropCommon
{
public:
	//!	Dialog Procedure
	static INT_PTR CALLBACK DlgProc_page(
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	//! Message Handler
	INT_PTR DispatchEvent( HWND, UINT, WPARAM, LPARAM );
	void SetData( HWND );	//!< �_�C�A���O�f�[�^�̐ݒ�
	int  GetData( HWND );	//!< �_�C�A���O�f�[�^�̎擾

private:
	void DrawToolBarItemList( DRAWITEMSTRUCT* );	/* �c�[���o�[�{�^�����X�g�̃A�C�e���`�� */
};

//==============================================================
//!	�L�[���[�h�y�[�W
class CPropKeyword : CPropCommon
{
public:
	//!	Dialog Procedure
	static INT_PTR CALLBACK DlgProc_page(
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	//! Message Handler
	INT_PTR DispatchEvent( HWND, UINT, WPARAM, LPARAM );
	void SetData( HWND );	//!< �_�C�A���O�f�[�^�̐ݒ�
	int  GetData( HWND );	//!< �_�C�A���O�f�[�^�̎擾

private:
	void SetKeyWordSet( HWND , int );	/* �w��L�[���[�h�Z�b�g�̐ݒ� */
	void GetKeyWordSet( HWND , int );	/* �w��L�[���[�h�Z�b�g�̎擾 */
	void DispKeywordCount( HWND hwndDlg );

	void Edit_List_KeyWord( HWND, HWND );		//!< ���X�g���őI������Ă���L�[���[�h��ҏW����
	void Delete_List_KeyWord( HWND , HWND );	//!< ���X�g���őI������Ă���L�[���[�h���폜����
	void Import_List_KeyWord( HWND , HWND );	//!< ���X�g���̃L�[���[�h���C���|�[�g����
	void Export_List_KeyWord( HWND , HWND );	//!< ���X�g���̃L�[���[�h���G�N�X�|�[�g����
	void Clean_List_KeyWord( HWND , HWND );		//!< ���X�g���̃L�[���[�h�𐮗����� 2005.01.26 Moca
};

//==============================================================
//!	�J�X�^�����j���[�y�[�W
class CPropCustmenu : CPropCommon
{
public:
	//!	Dialog Procedure
	static INT_PTR CALLBACK DlgProc_page(
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	//! Message Handler
	INT_PTR DispatchEvent( HWND, UINT, WPARAM, LPARAM );
	void SetData( HWND );	//!< �_�C�A���O�f�[�^�̐ݒ�
	int  GetData( HWND );	//!< �_�C�A���O�f�[�^�̎擾
	void Import( HWND );	//!< �J�X�^�����j���[�ݒ���C���|�[�g����
	void Export( HWND );	//!< �J�X�^�����j���[�ݒ���G�N�X�|�[�g����
};

//==============================================================
//!	�����y�[�W
class CPropFormat : CPropCommon
{
public:
	//!	Dialog Procedure
	static INT_PTR CALLBACK DlgProc_page(
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	//! Message Handler
	INT_PTR DispatchEvent( HWND, UINT, WPARAM, LPARAM );
	void SetData( HWND );	//!< �_�C�A���O�f�[�^�̐ݒ�
	int  GetData( HWND );	//!< �_�C�A���O�f�[�^�̎擾

private:
	void ChangeDateExample( HWND hwndDlg );
	void ChangeTimeExample( HWND hwndDlg );

	//	Sept. 10, 2000 JEPRO	���s��ǉ�
	void EnableFormatPropInput( HWND hwndDlg );	//	�����ݒ��ON/OFF
};

//==============================================================
//!	�x���y�[�W
class CPropHelper : CPropCommon
{
public:
	//!	Dialog Procedure
	static INT_PTR CALLBACK DlgProc_page(
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	//! Message Handler
	INT_PTR DispatchEvent( HWND, UINT, WPARAM, LPARAM );
	void SetData( HWND );	//!< �_�C�A���O�f�[�^�̐ݒ�
	int  GetData( HWND );	//!< �_�C�A���O�f�[�^�̎擾
};

//==============================================================
//!	�o�b�N�A�b�v�y�[�W
class CPropBackup : CPropCommon
{
public:
	//!	Dialog Procedure
	static INT_PTR CALLBACK DlgProc_page(
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	//! Message Handler
	INT_PTR DispatchEvent( HWND, UINT, WPARAM, LPARAM );
	void SetData( HWND );	//!< �_�C�A���O�f�[�^�̐ݒ�
	int  GetData( HWND );	//!< �_�C�A���O�f�[�^�̎擾

private:
	//	Aug. 16, 2000 genta
	void EnableBackupInput(HWND hwndDlg);	//	�o�b�N�A�b�v�ݒ��ON/OFF
	//	20051107 aroka
	void UpdateBackupFile(HWND hwndDlg);	//	�o�b�N�A�b�v�t�@�C���̏ڍאݒ�
};

//==============================================================
//!	�E�B���h�E�y�[�W
class CPropWin : CPropCommon
{
public:
	//!	Dialog Procedure
	static INT_PTR CALLBACK DlgProc_page(
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	//! Message Handler
	INT_PTR DispatchEvent( HWND, UINT, WPARAM, LPARAM );
	void SetData( HWND );	//!< �_�C�A���O�f�[�^�̐ݒ�
	int  GetData( HWND );	//!< �_�C�A���O�f�[�^�̎擾

private:
	//	Sept. 9, 2000 JEPRO		���s��ǉ�
	void EnableWinPropInput( HWND hwndDlg) ;	//	�E�B���h�E�ݒ��ON/OFF
};

//==============================================================
//!	�^�u����y�[�W
class CPropTab : CPropCommon
{
public:
	//!	Dialog Procedure
	static INT_PTR CALLBACK DlgProc_page(
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	//! Message Handler
	INT_PTR DispatchEvent( HWND, UINT, WPARAM, LPARAM );
	void SetData( HWND );	//!< �_�C�A���O�f�[�^�̐ݒ�
	int  GetData( HWND );	//!< �_�C�A���O�f�[�^�̎擾

private:
	void EnableTabPropInput(HWND hwndDlg);
};

//==============================================================
//!	�ҏW�y�[�W
class CPropEdit : CPropCommon
{
public:
	//!	Dialog Procedure
	static INT_PTR CALLBACK DlgProc_page(
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	//! Message Handler
	INT_PTR DispatchEvent( HWND, UINT, WPARAM, LPARAM );
	void SetData( HWND );	//!< �_�C�A���O�f�[�^�̐ݒ�
	int  GetData( HWND );	//!< �_�C�A���O�f�[�^�̎擾

private:
	void EnableEditPropInput( HWND hwndDlg );
};

//==============================================================
//!	�����y�[�W
class CPropGrep : CPropCommon
{
public:
	//!	Dialog Procedure
	static INT_PTR CALLBACK DlgProc_page(
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	//! Message Handler
	INT_PTR DispatchEvent( HWND, UINT, WPARAM, LPARAM );
	void SetData( HWND );	//!< �_�C�A���O�f�[�^�̐ݒ�
	int  GetData( HWND );	//!< �_�C�A���O�f�[�^�̎擾

private:
	void SetRegexpVersion( HWND ); // 2007.08.12 genta �o�[�W�����\��
};

//==============================================================
//!	�}�N���y�[�W
class CPropMacro : CPropCommon
{
public:
	//!	Dialog Procedure
	static INT_PTR CALLBACK DlgProc_page(
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	//! Message Handler
	INT_PTR DispatchEvent( HWND, UINT, WPARAM, LPARAM );
	void SetData( HWND );	//!< �_�C�A���O�f�[�^�̐ݒ�
	int  GetData( HWND );	//!< �_�C�A���O�f�[�^�̎擾

private:
	void InitDialog( HWND hwndDlg );//!< Macro�y�[�W�̏�����
	//	To Here Jun. 2, 2001 genta
	void SetMacro2List_Macro( HWND hwndDlg );//!< Macro�f�[�^�̐ݒ�
	void SelectBaseDir_Macro( HWND hwndDlg );//!< Macro�f�B���N�g���̑I��
	void OnFileDropdown_Macro( HWND hwndDlg );//!< �t�@�C���h���b�v�_�E�����J�����Ƃ�
	void CheckListPosition_Macro( HWND hwndDlg );//!< ���X�g�r���[��Focus�ʒu�m�F
	static int CALLBACK DirCallback_Macro( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData );
};

//==============================================================
//!	�t�@�C�����\���y�[�W
class CPropFileName : CPropCommon
{
public:
	//!	Dialog Procedure
	static INT_PTR CALLBACK DlgProc_page(
		HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	//! Message Handler
	INT_PTR DispatchEvent( HWND, UINT, WPARAM, LPARAM );
	void SetData( HWND );	//!< �_�C�A���O�f�[�^�̐ݒ�
	int  GetData( HWND );	//!< �_�C�A���O�f�[�^�̎擾

private:
	static int SetListViewItem_FILENAME( HWND hListView, int, LPTSTR, LPTSTR, bool );//!<ListView�̃A�C�e����ݒ�
	static void GetListViewItem_FILENAME( HWND hListView, int, LPTSTR, LPTSTR );//!<ListView�̃A�C�e�����擾
	static int MoveListViewItem_FILENAME( HWND hListView, int, int );//!<ListView�̃A�C�e�����ړ�����
};



///////////////////////////////////////////////////////////////////////
#endif /* SAKURA_PROP_CPROPCOMMON_H_ */


/*[EOF]*/