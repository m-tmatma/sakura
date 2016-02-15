/*!	@file
	@brief �t�@�C���I�[�v���_�C�A���O�{�b�N�X

	@author Norio Nakatani
	@date	1998/08/10 �쐬
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2001, genta
	Copyright (C) 2002, YAZAKI, Moca
	Copyright (C) 2003, ryoji
	Copyright (C) 2004, genta, MIK
	Copyright (C) 2005, ryoji
	Copyright (C) 2006, Moca, ryoji
	Copyright (C) 2008, nasukoji

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/

#ifndef _CDLGOPENFILE_H_
#define _CDLGOPENFILE_H_

#include <windows.h>
#include <vector>
#include "design_template.h"
#include "CShareData.h"
#include "CEol.h"
#include "COsVersionInfo.h"	// 2005.11.02 ryoji

struct OPENFILENAMEZ;
class CDlgOpenFileMem;


/*!	�t�@�C���I�[�v���_�C�A���O�{�b�N�X

	@date 2002.2.17 YAZAKI CShareData�̃C���X�^���X�́ACProcess�ɂЂƂ���̂݁B
*/
class CDlgOpenFile
{
public:
	//�R���X�g���N�^�E�f�X�g���N�^
	CDlgOpenFile();
	~CDlgOpenFile();
	void Create(
		HINSTANCE					hInstance,
		HWND						hwndParent,
		const TCHAR*				pszUserWildCard,
		const TCHAR*				pszDefaultPath,
		const std::vector<LPCTSTR>& vMRU			= std::vector<LPCTSTR>(),
		const std::vector<LPCTSTR>& vOPENFOLDER		= std::vector<LPCTSTR>()
	);

	//����
	bool DoModal_GetOpenFileName( TCHAR*, bool bSetCurDir = false );	/* �J���_�C�A���O ���[�_���_�C�A���O�̕\�� */	//2002/08/21 moca	�����ǉ�
	bool DoModal_GetSaveFileName( TCHAR*, bool bSetCurDir = false );	/* �ۑ��_�C�A���O ���[�_���_�C�A���O�̕\�� */	//2002/08/21 30,2002 moca	�����ǉ�
	bool DoModalOpenDlg( char* , ECodeType*, bool* );	/* �J���_�C�A�O ���[�_���_�C�A���O�̕\�� */
	bool DoModalSaveDlg( char* , ECodeType*, CEol*, bool* );	/* �ۑ��_�C�A���O ���[�_���_�C�A���O�̕\�� */

protected:
	CDlgOpenFileMem*	m_mem;

	/*
	||  �����w���p�֐�
	*/

	//	May 29, 2004 genta �G���[�������܂Ƃ߂� (advised by MIK)
	void	DlgOpenFail(void);

	// 2005.11.02 ryoji OS �o�[�W�����Ή��� OPENFILENAME �������p�֐�
	void InitOfn( OPENFILENAMEZ* );

	// 2005.11.02 ryoji �������C�A�E�g�ݒ菈��
	static void InitLayout( HWND hwndOpenDlg, HWND hwndDlg, HWND hwndBaseCtrl );

	// 2005.10.29 ryoji �R���{�{�b�N�X�̃h���b�v�_�E��������
	static void OnCbnDropDown( HWND hwnd );

	// 2006.09.03 Moca �t�@�C���_�C�A���O�̃G���[���
	//! ���g���C�@�\�t�� GetOpenFileName
	bool _GetOpenFileNameRecover( OPENFILENAMEZ* ofn );
	//! ���g���C�@�\�t�� GetOpenFileName
	bool GetSaveFileNameRecover( OPENFILENAMEZ* ofn );

	BOOL CheckPathLengthOverflow( const char *pszPath, int nLength, BOOL bErrDisp = TRUE );	// 2008.11.23 nasukoji	�w��̃t�@�C���p�X�̃o�b�t�@�I�[�o�[�t���[���`�F�b�N����

	friend UINT_PTR CALLBACK OFNHookProc( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam );

private:
	DISALLOW_COPY_AND_ASSIGN(CDlgOpenFile);
};



///////////////////////////////////////////////////////////////////////
#endif /* _CDLGOPENFILE_H_ */


/*[EOF]*/