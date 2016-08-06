/*!	@file
	@brief CLayout��CDocLine�̃C�e���[�^

	@author Yazaki
	@date 2002/09/25 �V�K�쐬
*/
/*
	Copyright (C) 2002, Yazaki
	Copyright (C) 2003, genta
	Copyright (C) 2005, D.S.Koba

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/
#ifndef _CMEMORYITERATOR_H_
#define _CMEMORYITERATOR_H_

//	sakura
#include "_main/global.h"
#include "charset/charcode.h"
#include "doc/layout/CTsvModeInfo.h"

/*-----------------------------------------------------------------------
�N���X�̐錾
-----------------------------------------------------------------------*/
// 2007.10.23 kobake �e���v���[�g�ł���K�v�������̂ŁA��e���v���[�g�ɕύX�B

#include "doc/layout/CLayout.h"
#include "doc/logic/CDocLine.h"

//! �u���b�N�R�����g�f���~�^���Ǘ�����
class CMemoryIterator
{
public:
	//CDocLine�p�R���X�g���N�^
	CMemoryIterator( const CDocLine* pcT, CLayoutInt nTabSpace, const CTsvModeInfo& tsvInfo, CPixelXInt nCharDx, CPixelXInt nSpacing )
	: m_pLine( pcT ? pcT->GetPtr() : NULL )
	, m_nLineLen( pcT ? pcT->GetLengthWithEOL() : 0 )
	, m_nTabSpace( nTabSpace )
	, m_tsvInfo( tsvInfo )
	, m_nIndent( CLayoutInt(0) )
	, m_nSpacing(nSpacing)
	, m_nTabPadding(nCharDx - 1)
	, m_nTabSpaceDx((Int)nTabSpace + nCharDx - 1)
	{
		first();
	}

	//CLayout�p�R���X�g���N�^
	CMemoryIterator( const CLayout* pcT, CLayoutInt nTabSpace, const CTsvModeInfo& tsvInfo, CPixelXInt nCharDx, CPixelXInt nSpacing )
	: m_pLine( pcT ? pcT->GetPtr() : NULL )
	, m_nLineLen( pcT ? pcT->GetLengthWithEOL() : 0 )
	, m_nTabSpace( nTabSpace )
	, m_tsvInfo( tsvInfo )
	, m_nIndent( pcT ? pcT->GetIndent() : CLayoutInt(0) )
	, m_nSpacing(nSpacing)
	, m_nTabPadding(nCharDx - 1)
	, m_nTabSpaceDx((Int)nTabSpace + nCharDx - 1)
	{
		first();
	}

	//! ���ʒu���s�̐擪�ɃZ�b�g
	void first()
	{
		m_nIndex = CLogicInt(0);
		m_nColumn = m_nIndent;
		m_nIndex_Delta = CLogicInt(0);
		m_nColumn_Delta = CLayoutInt(0);
	}

	/*! �s�����ǂ���
		@return true: �s��, false: �s���ł͂Ȃ�
	 */
	bool end() const
	{
		return (m_nLineLen <= m_nIndex);
	}

	//	���̕������m�F���Ď��̕����Ƃ̍������߂�
	void scanNext()
	{
		// 2005-09-02 D.S.Koba GetSizeOfChar
		// 2007.09.04 kobake UNICODE���F�f�[�^�����ƌ�������ʁX�̒l�Ƃ��Čv�Z����B

		//�f�[�^�������v�Z
		m_nIndex_Delta = CNativeW::GetSizeOfChar(m_pLine, m_nLineLen, m_nIndex);
		if( 0 == m_nIndex_Delta )
			m_nIndex_Delta = CLogicInt(1);

		//���������v�Z
		if (m_pLine[m_nIndex] == WCODE::TAB){
			if (m_tsvInfo.m_nTsvMode == TSV_MODE_TSV) {
				m_nColumn_Delta = m_tsvInfo.GetActualTabLength(m_nColumn, m_tsvInfo.m_nMaxCharLayoutX);
			}
			else if (m_tsvInfo.m_nTsvMode == TSV_MODE_CSV) {
				m_nColumn_Delta = m_nTabPadding;
			}
			else {
				m_nColumn_Delta = m_nTabSpaceDx - (m_nColumn + m_nTabPadding) % m_nTabSpace;
			}
		} else if (m_pLine[m_nIndex] == L',' && m_tsvInfo.m_nTsvMode == TSV_MODE_CSV){
			m_nColumn_Delta = m_tsvInfo.GetActualTabLength(m_nColumn, m_tsvInfo.m_nMaxCharLayoutX);
		}else{
			m_nColumn_Delta = CNativeW::GetColmOfChar( m_pLine, m_nLineLen, m_nIndex );
			if( m_nSpacing ){
				m_nColumn_Delta += CLayoutXInt(CNativeW::GetKetaOfChar(m_pLine, m_nLineLen, m_nIndex) * m_nSpacing);
			}
//			if( 0 == m_nColumn_Delta )				// �폜 �T���Q�[�g�y�A�΍�	2008/7/5 Uchi
//				m_nColumn_Delta = CLayoutInt(1);
		}
	}
	
	/*! �\�ߌv�Z�������������ʒu�ɉ�����D
		@sa scanNext()
	 */
	void addDelta(){
		m_nColumn += m_nColumn_Delta;
		m_nIndex += m_nIndex_Delta;
	}	//	�|�C���^�����炷
	
	CLogicInt	getIndex()			const {	return m_nIndex;	}
	CLayoutInt	getColumn()			const {	return m_nColumn;	}
	CLogicInt	getIndexDelta()		const {	return m_nIndex_Delta;	}
	CLayoutInt	getColumnDelta()	const {	return m_nColumn_Delta;	}

	//	2002.10.07 YAZAKI
	const wchar_t getCurrentChar(){	return m_pLine[m_nIndex];	}
	//	Jul. 20, 2003 genta �ǉ�
	//	memcpy������̂Ƀ|�C���^���Ƃ�Ȃ��Ɩʓ|
	const wchar_t* getCurrentPos(){	return m_pLine + m_nIndex;	}


private:
	//�R���X�g���N�^�Ŏ󂯎�����p�����[�^ (�Œ�)
	const wchar_t*		m_pLine;
	const int			m_nLineLen;  //�f�[�^���B�����P�ʁB
	const CLayoutInt	m_nTabSpace;
	const CTsvModeInfo&	m_tsvInfo;
	const CLayoutInt	m_nIndent;

	const CPixelXInt	m_nSpacing;		//��������(px)
	const CPixelXInt	m_nTabPadding;	//�^�u���ŏ��l-1
	const CPixelXInt	m_nTabSpaceDx;	//�^�u���v�Z�p(m_nTabSpace + m_nTabPadding - 1)

	//��ԕϐ�
	CLogicInt	m_nIndex;        //�f�[�^�ʒu�B�����P�ʁB
	CLayoutInt	m_nColumn;       //���C�A�E�g�ʒu�B��(���p��)�P�ʁB
	CLogicInt	m_nIndex_Delta;  //index����
	CLayoutInt	m_nColumn_Delta; //column����

};


///////////////////////////////////////////////////////////////////////
#endif /* _CBLOCKCOMMENT_H_ */


