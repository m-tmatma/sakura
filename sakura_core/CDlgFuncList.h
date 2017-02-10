/*!	@file
	@brief �A�E�g���C����̓_�C�A���O�{�b�N�X

	@author Norio Nakatani
	@date 1998/06/23 �V�K�쐬
	@date 1998/12/04 �č쐬
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2001, genta, hor
	Copyright (C) 2002, aroka, hor, YAZAKI, frozen
	Copyright (C) 2003, little YOSHI
	Copyright (C) 2005, genta
	Copyright (C) 2006, aroka
	Copyright (C) 2007, ryoji

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/

#ifndef SAKURA_CDLGFUNCLIST_H_
#define SAKURA_CDLGFUNCLIST_H_

#include <windows.h>
#include <vector>
#include "CDialog.h"
#include "CMemory.h"

class CFuncInfo;
class CFuncInfoArr; // 2002/2/10 aroka

//! �c���[�r���[���\�[�g����
#define SORTTYPE_DEFAULT       0 //!< �f�t�H���g(�m�[�h�Ɋ֘A�Â����ꂽ�l��,����)
#define SORTTYPE_DEFAULT_DESC  1 //!< �f�t�H���g(�m�[�h�Ɋ֘A�Â����ꂽ�l��,�~��)
#define SORTTYPE_ATOZ          2 //!< �A���t�@�x�b�g��(����)
#define SORTTYPE_ZTOA          3 //!< �A���t�@�x�b�g��(�~��)



//!	�A�E�g���C����̓_�C�A���O�{�b�N�X
class CDlgFuncList : public CDialog
{
public:
	/*
	||  Constructors
	*/
	CDlgFuncList();
	/*
	||  Attributes & Operations
	*/
	INT_PTR DispatchEvent( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam );	// 2007.11.07 ryoji �W���ȊO�̃��b�Z�[�W��ߑ�����
	HWND DoModeless( HINSTANCE, HWND, LPARAM, CFuncInfoArr*, CLayoutInt, CLayoutInt, int, bool );/* ���[�h���X�_�C�A���O�̕\�� */
	void ChangeView( LPARAM );	/* ���[�h���X���F�����ΏۂƂȂ�r���[�̕ύX */

	/*! ���݂̎�ʂƓ����Ȃ�
	*/
	bool CheckListType( int nOutLineType ) const { return nOutLineType == m_nListType; }
	void Redraw( int nOutLineType, CFuncInfoArr*, CLayoutInt nCurLine, CLayoutInt nCurCol );

	CFuncInfoArr*	m_pcFuncInfoArr;	/* �֐����z�� */
	CLayoutInt		m_nCurLine;			/* ���ݍs */
	CLayoutInt		m_nCurCol;			/* ���݌� */
	int				m_nSortCol;			/* �\�[�g�����ԍ� */
	int				m_nListType;		/* �ꗗ�̎�� */
	CMemory			m_cmemClipText;		/* �N���b�v�{�[�h�R�s�[�p�e�L�X�g */
	bool			m_bLineNumIsCRLF;	/* �s�ԍ��̕\�� false=�܂�Ԃ��P�ʁ^true=���s�P�� */
protected:
	BOOL OnInitDialog( HWND, WPARAM, LPARAM );
	BOOL OnBnClicked( int );
	BOOL OnNotify( WPARAM, LPARAM );
	BOOL OnSize( WPARAM, LPARAM );
	BOOL OnDestroy(void); // 20060201 aroka
	BOOL OnCbnSelEndOk( HWND hwndCtl, int wID );
	void SetData();	/* �_�C�A���O�f�[�^�̐ݒ� */
	int GetData( void );	/* �_�C�A���O�f�[�^�̎擾 */

	/*
	||  �����w���p�֐�
	*/
	BOOL OnJump( bool bCheckAutoClose = true );	//	bCheckAutoClose�F�u���̃_�C�A���O�������I�ɕ���v���`�F�b�N���邩�ǂ���
	void SetTreeJava( HWND, BOOL );	/* �c���[�R���g���[���̏������FJava���\�b�h�c���[ */
	void SetTree(bool tagjump = false);		/* �c���[�R���g���[���̏������F�ėp�i */
	void SetListVB( void );			/* ���X�g�r���[�R���g���[���̏������FVisualBasic */		// Jul 10, 2003  little YOSHI

	// 2002/11/1 frozen 
	void SortTree(HWND hWndTree,HTREEITEM htiParent);//!< �c���[�r���[�̍��ڂ��\�[�g����i�\�[�g���m_nSortType���g�p�j
#if 0
2002.04.01 YAZAKI SetTreeTxt()�ASetTreeTxtNest()�͔p�~�BGetTreeTextNext�͂��Ƃ��Ǝg�p����Ă��Ȃ������B
	void SetTreeTxt( HWND );	/* �c���[�R���g���[���̏������F�e�L�X�g�g�s�b�N�c���[ */
	int SetTreeTxtNest( HWND, HTREEITEM, int, int, HTREEITEM*, int );
	void GetTreeTextNext( HWND, HTREEITEM, int );
#endif

	//	Apr. 23, 2005 genta ���X�g�r���[�̃\�[�g���֐��Ƃ��ēƗ�������
	void SortListView(HWND hwndList, int sortcol);

	// 2001.12.03 hor
//	void SetTreeBookMark( HWND );		/* �c���[�R���g���[���̏������F�u�b�N�}�[�N */
	LPVOID GetHelpIdTable(void);	//@@@ 2002.01.18 add
	void Key2Command( WORD );		//	�L�[���쁨�R�}���h�ϊ�

private:
	//	May 18, 2001 genta
	/*!
		@brief �A�E�g���C����͎��

		0: List, 1: Tree
	*/
	int	m_nViewType;

	// 2002.02.16 hor Tree�̃_�u���N���b�N�Ńt�H�[�J�X�ړ��ł���悤�� 1/4
	// (������Ȃ̂łǂȂ����C�����肢���܂�)
	bool m_bWaitTreeProcess;

	int m_nSortType;						//!< �c���[�r���[���\�[�g����
	int m_nTreeItemCount;
	bool m_bDummyLParamMode;				//!< m_vecDummylParams�L��/����
	std::vector<int> m_vecDummylParams;		//!< �_�~�[�v�f�̎��ʒl

	// �I�𒆂̊֐����
	CFuncInfo* m_cFuncInfo;

};



///////////////////////////////////////////////////////////////////////
#endif /* SAKURA_CDLGFUNCLIST_H_ */


/*[EOF]*/