/*!	@file
	@brief �e�L�X�g�̃��C�A�E�g���

	@author Norio Nakatani
	@date 1998/3/11 �V�K�쐬
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2002, YAZAKI, aroka
	Copyright (C) 2009, nasukoji

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/

class CLayout;

#ifndef _CLAYOUT_H_
#define _CLAYOUT_H_



#include "CEol.h"// 2002/2/10 aroka
#include "CDocLine.h"// 2002/4/21 YAZAKI
#include "CMemory.h"// 2002/4/21 YAZAKI


/*-----------------------------------------------------------------------
�N���X�̐錾
-----------------------------------------------------------------------*/
class CLayout
{
public:
	/*
	||  Constructors
	*/
	CLayout(
	)
	{
		m_pPrev = NULL;
		m_pNext = NULL;
		m_pCDocLine = NULL;
		m_ptLogicPos.x = 0;	// �Ή�����_���s�̐擪����̃I�t�Z�b�g
		m_ptLogicPos.y = 0;	// �Ή�����_���s�ԍ�
		m_nLength = 0;	// �Ή�����_���s�̃n�C�g��
		m_nTypePrev = COLORIDX_DEFAULT;// �^�C�v 0=�ʏ� 1=�s�R�����g 2=�u���b�N�R�����g 3=�V���O���N�H�[�e�[�V���������� 4=�_�u���N�H�[�e�[�V����������
		m_nIndent = 0;	// ���̃��C�A�E�g�s�̃C���f���g�� @@@ 2002.09.23 YAZAKI */
		m_nLayoutWidth = 0;		// ���̃��C�A�E�g�s�̉��s���܂ރ��C�A�E�g���i�u�܂�Ԃ��Ȃ��v�I�����̂݁j	// 2009.08.28 nasukoji
	}
	~CLayout();
	void DUMP( void );
	
	// m_ptLogicPos.x�ŕ␳�������Ƃ̕�����𓾂�
	char*	GetPtr() const {	return m_pCDocLine->m_cLine.GetStringPtr() + m_ptLogicPos.x;	}
	int		GetLengthWithEOL() const {	return m_nLength;	}	//	������EOL�͏��1�����ƃJ�E���g�H�H
	int		GetLengthWithoutEOL() const {	return m_nLength - (m_cEol.GetLen() ? 1 : 0);	}
	int		GetLength() const {	return m_nLength;	}	//	CMemoryIterator�p�iEOL�܂ށj
	int		GetIndent() const {	return m_nIndent;	}	//	CMemoryIterator�p

public:
	CLayout*		m_pPrev;
	CLayout*		m_pNext;

	const CDocLine*	m_pCDocLine;		//!< ���f�[�^�ւ̎Q��
	CLogicPoint		m_ptLogicPos;		//!< �Ή����郍�W�b�N�Q�ƈʒu
	int				m_nLength;			//!< ���̃��C�A�E�g�s�̒���(�n�C�g��)

	EColorIndexType	m_nTypePrev;		//!< �^�C�v 0=�ʏ� 1=�s�R�����g 2=�u���b�N�R�����g 3=�V���O���N�H�[�e�[�V���������� 4=�_�u���N�H�[�e�[�V����������
	int				m_nIndent;			//!< ���̃��C�A�E�g�s�̃C���f���g�� @@@ 2002.09.23 YAZAKI
	CEol			m_cEol;
	int				m_nLayoutWidth;		//!< ���̃��C�A�E�g�s�̉��s���܂ރ��C�A�E�g���i�u�܂�Ԃ��Ȃ��v�I�����̂݁j // 2009.08.28 nasukoji
};


///////////////////////////////////////////////////////////////////////
#endif /* _CLAYOUT_H_ */


/*[EOF]*/