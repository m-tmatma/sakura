/*!	@file
	@brief 編集操作要素ブロック

	@author Norio Nakatani
	@date 1998/06/09 新規作成
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2018-2022, Sakura Editor Organization

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/
#include "StdAfx.h"
#include <stdlib.h>
#include "cmd/COpeBlk.h"

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
//               コンストラクタ・デストラクタ                  //
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

COpeBlk::COpeBlk()
{
	m_refCount = 0;
}

COpeBlk::~COpeBlk()
{
	/* 操作の配列を削除する */
	const ssize_t size = static_cast<ssize_t>(m_ppCOpeArr.size());
	for( ssize_t i = 0; i < size; ++i ){
		SAFE_DELETE(m_ppCOpeArr[static_cast<size_t>(i)]);
	}
	m_ppCOpeArr.clear();
}

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
//                     インターフェース                        //
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

/* 操作の追加 */
bool COpeBlk::AppendOpe( COpe* pcOpe )
{
	if(pcOpe->m_ptCaretPos_PHY_Before.HasNegative() || pcOpe->m_ptCaretPos_PHY_After.HasNegative()){
		TopErrorMessage( nullptr,
			L"COpeBlk::AppendOpe() error.\n"
			L"Bug.\n"
			L"pcOpe->m_ptCaretPos_PHY_Before = %d,%d\n"
			L"pcOpe->m_ptCaretPos_PHY_After = %d,%d\n",
			int(pcOpe->m_ptCaretPos_PHY_Before.x),
			int(pcOpe->m_ptCaretPos_PHY_Before.y),
			int(pcOpe->m_ptCaretPos_PHY_After.x),
			int(pcOpe->m_ptCaretPos_PHY_After.y)
		);
	}

	/* 配列のメモリサイズを調整 */
	m_ppCOpeArr.push_back(pcOpe);
	return true;
}

/* 操作を返す */
COpe* COpeBlk::GetOpe( ssize_t nIndex )
{
	if( nIndex < 0 || GetNum() <= nIndex ){
		return nullptr;
	}
	return m_ppCOpeArr[static_cast<size_t>(nIndex)];
}

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
//                         デバッグ                            //
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

/* 編集操作要素ブロックのダンプ */
void COpeBlk::DUMP( void )
{
#ifdef _DEBUG
	ssize_t i;
	ssize_t size = GetNum();
	for( i = 0; i < size; ++i ){
		MYTRACE( L"\tCOpeBlk.m_ppCOpeArr[%d]----\n", static_cast<int>(i) );
		m_ppCOpeArr[i]->DUMP();
	}
#endif
}
