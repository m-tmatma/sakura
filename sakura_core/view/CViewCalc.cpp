#include "StdAfx.h"
#include "CViewCalc.h"
#include "mem/CMemoryIterator.h"
#include "view/CEditView.h"
#include "doc/CEditDoc.h"
#include "doc/layout/CLayout.h"

//�O���ˑ�
CLayoutInt CViewCalc::GetTabSpace() const
{
	return m_pOwner->m_pcEditDoc->m_cLayoutMgr.GetTabSpace();
}

CTsvModeInfo& CViewCalc::GetTsvMode() const
{
	return m_pOwner->m_pcEditDoc->m_cLayoutMgr.m_tsvInfo;
}

CPixelXInt CViewCalc::GetCharSpacing() const
{
	return m_pOwner->m_pcEditDoc->m_cLayoutMgr.GetCharSpacing();
}

/* �w�肳�ꂽ���ɑΉ�����s�̃f�[�^���̈ʒu�𒲂ׂ� Ver1
	
	@@@ 2002.09.28 YAZAKI CDocLine��
*/
CLogicInt CViewCalc::LineColumnToIndex( const CDocLine* pcDocLine, CLayoutInt nColumn ) const
{
	CLogicInt i2 = CLogicInt(0);
	CMemoryIterator it( pcDocLine, GetTabSpace(), GetTsvMode(), m_pOwner->m_pcEditDoc->m_cLayoutMgr.GetWidthPerKeta(), GetCharSpacing() );
	while( !it.end() ){
		it.scanNext();
		if ( it.getColumn() + it.getColumnDelta() > nColumn ){
			break;
		}
		it.addDelta();
	}
	i2 += it.getIndex();
	return i2;
}


/* �w�肳�ꂽ���ɑΉ�����s�̃f�[�^���̈ʒu�𒲂ׂ� Ver1
	
	@@@ 2002.09.28 YAZAKI CLayout���K�v�ɂȂ�܂����B
*/
CLogicInt CViewCalc::LineColumnToIndex( const CLayout* pcLayout, CLayoutInt nColumn ) const
{
	CLogicInt i2 = CLogicInt(0);
	CMemoryIterator it = m_pOwner->m_pcEditDoc->m_cLayoutMgr.CreateCMemoryIterator(pcLayout);
	while( !it.end() ){
		it.scanNext();
		if ( it.getColumn() + it.getColumnDelta() > nColumn ){
			break;
		}
		it.addDelta();
	}
	i2 += it.getIndex();
	return i2;
}



/* �w�肳�ꂽ���ɑΉ�����s�̃f�[�^���̈ʒu�𒲂ׂ� Ver0 */
/* �w�肳�ꂽ�����A�s���Z���ꍇ��pnLineAllColLen�ɍs�S�̂̕\��������Ԃ� */
/* ����ȊO�̏ꍇ��pnLineAllColLen�ɂO���Z�b�g����
	
	@@@ 2002.09.28 YAZAKI CLayout���K�v�ɂȂ�܂����B
*/
CLogicInt CViewCalc::LineColumnToIndex2( const CLayout* pcLayout, CLayoutInt nColumn, CLayoutInt* pnLineAllColLen ) const
{
	*pnLineAllColLen = CLayoutInt(0);

	CLogicInt i2 = CLogicInt(0);
	CLayoutInt nPosX2 = CLayoutInt(0);
	CMemoryIterator it = m_pOwner->m_pcEditDoc->m_cLayoutMgr.CreateCMemoryIterator(pcLayout);
	while( !it.end() ){
		it.scanNext();
		if ( it.getColumn() + it.getColumnDelta() > nColumn ){
			break;
		}
		it.addDelta();
	}
	i2 += it.getIndex();
	if( i2 >= pcLayout->GetLengthWithEOL() ){
		nPosX2 += it.getColumn();
		*pnLineAllColLen = nPosX2;
	}
	return i2;
}





/*
||	�w�肳�ꂽ�s�̃f�[�^���̈ʒu�ɑΉ����錅�̈ʒu�𒲂ׂ�
||
||	@@@ 2002.09.28 YAZAKI CLayout���K�v�ɂȂ�܂����B
*/
CLayoutInt CViewCalc::LineIndexToColumn( const CLayout* pcLayout, CLogicInt nIndex ) const
{
	//	�ȉ��Aiterator��
	CLayoutInt nPosX2 = CLayoutInt(0);
	CMemoryIterator it = m_pOwner->m_pcEditDoc->m_cLayoutMgr.CreateCMemoryIterator(pcLayout);
	while( !it.end() ){
		it.scanNext();
		if ( it.getIndex() + it.getIndexDelta() > nIndex ){
			break;
		}
		it.addDelta();
	}
	nPosX2 += it.getColumn();
	return nPosX2;
}


/*
||	�w�肳�ꂽ�s�̃f�[�^���̈ʒu�ɑΉ����錅�̈ʒu�𒲂ׂ�
||
||	@@@ 2002.09.28 YAZAKI CDocLine��
*/
CLayoutInt CViewCalc::LineIndexToColumn( const CDocLine* pcDocLine, CLogicInt nIndex ) const
{
	CLayoutInt nPosX2 = CLayoutInt(0);
	CMemoryIterator it( pcDocLine, GetTabSpace(), GetTsvMode(), m_pOwner->m_pcEditDoc->m_cLayoutMgr.GetWidthPerKeta(), GetCharSpacing() );
	while( !it.end() ){
		it.scanNext();
		if ( it.getIndex() + it.getIndexDelta() > nIndex ){
			break;
		}
		it.addDelta();
	}
	nPosX2 += it.getColumn();
	return nPosX2;
}


