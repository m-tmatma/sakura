/*!	@file
	@brief CEditView�N���X�̃C���N�������^���T�[�`�֘A�R�}���h�����n�֐��Q

	@author genta
	@date	2005/01/10 �쐬
*/
/*
	Copyright (C) 2004, isearch
	Copyright (C) 2005, genta, Moca

	This source code is designed for sakura editor.
	Please contact the copyright holders to use this code for other purpose.
*/
#include "StdAfx.h"
#include "CEditView.h"
#include "CEditWnd.h"
#include "CEditDoc.h"
#include "CDocLine.h"
#include "CMigemo.h"
#include "etc_uty.h"
#include "Debug.h"
#include "sakura_rc.h"

/*!
	�R�}���h�R�[�h�̕ϊ�(ISearch��)�y��
	�C���N�������^���T�[�`���[�h�𔲂��锻��

	@return true: �R�}���h�����ς� / false: �R�}���h�����p��

	@date 2004.09.14 isearch �V�K�쐬
	@date 2005.01.10 genta �֐���, UNINDENT�ǉ�

	@note UNINDENT��ʏ핶���Ƃ��Ĉ����̂́C
		SHIFT+�����̌��SPACE����͂���悤�ȃP�[�X��
		SHIFT�̉�����x��Ă�����������Ȃ��Ȃ邱�Ƃ�h�����߁D
*/
void CEditView::TranslateCommand_isearch(
	int&	nCommand,
	bool&	bRedraw,
	LPARAM&	lparam1,
	LPARAM&	lparam2,
	LPARAM&	lparam3,
	LPARAM&	lparam4
)
{
	if (m_nISearchMode <= SEARCH_NONE )
		return;

	switch (nCommand){
		//�����̋@�\�̂Ƃ��A�C���N�������^���T�[�`�ɓ���
		case F_ISEARCH_NEXT:
		case F_ISEARCH_PREV:
		case F_ISEARCH_REGEXP_NEXT:
		case F_ISEARCH_REGEXP_PREV:
		case F_ISEARCH_MIGEMO_NEXT:
		case F_ISEARCH_MIGEMO_PREV:
			break;

		//�ȉ��̋@�\�̂Ƃ��A�C���N�������^���T�[�`���͌����������͂Ƃ��ď���
		case F_CHAR:
		case F_IME_CHAR:
			nCommand = F_ISEARCH_ADD_CHAR;
			break;
		case F_INSTEXT:
			nCommand = F_ISEARCH_ADD_STR;
			break;

		case F_INDENT_TAB:	// TAB�̓C���f���g�ł͂Ȃ��P�Ȃ�TAB�����ƌ��Ȃ�
		case F_UNINDENT_TAB:	// genta�ǉ�
			nCommand = F_ISEARCH_ADD_CHAR;
			lparam1 = '\t';
			break;
		case F_INDENT_SPACE:	// �X�y�[�X�̓C���f���g�ł͂Ȃ��P�Ȃ�TAB�����ƌ��Ȃ�
		case F_UNINDENT_SPACE:	// genta�ǉ�
			nCommand = F_ISEARCH_ADD_CHAR;
			lparam1 = ' ';
			break;
		case F_DELETE_BACK:
			nCommand = F_ISEARCH_DEL_BACK;
			break;

		default:
			//��L�ȊO�̃R�}���h�̏ꍇ�̓C���N�������^���T�[�`�𔲂���
			ISearchExit();
	}
}

/*!
	ISearch �R�}���h����

	@date 2005.01.10 genta �e�R�}���h�ɓ����Ă���������1�J���Ɉړ�
*/
bool CEditView::ProcessCommand_isearch(
	int	nCommand,
	bool	bRedraw,
	LPARAM	lparam1,
	LPARAM	lparam2,
	LPARAM	lparam3,
	LPARAM	lparam4
)
{
	switch( nCommand ){
		//	����������̕ύX����
		case F_ISEARCH_ADD_CHAR:
			ISearchExec((WORD)lparam1);
			return true;
		
		case F_ISEARCH_DEL_BACK:
			ISearchBack();
			return true;

		case F_ISEARCH_ADD_STR:
			ISearchExec((const char*)lparam1);
			return true;

		//	�������[�h�ւ̈ڍs
		case F_ISEARCH_NEXT:
			ISearchEnter(SEARCH_NORMAL, SEARCH_FORWARD);	//�O���C���N�������^���T�[�` //2004.10.13 isearch
			return true;
		case F_ISEARCH_PREV:
			ISearchEnter(SEARCH_NORMAL, SEARCH_BACKWARD);	//����C���N�������^���T�[�` //2004.10.13 isearch
			return true;
		case F_ISEARCH_REGEXP_NEXT:
			ISearchEnter(SEARCH_REGEXP, SEARCH_FORWARD);	//�O�����K�\���C���N�������^���T�[�`  //2004.10.13 isearch
			return true;
		case F_ISEARCH_REGEXP_PREV:
			ISearchEnter(SEARCH_REGEXP, SEARCH_BACKWARD);	//������K�\���C���N�������^���T�[�`  //2004.10.13 isearch
			return true;
		case F_ISEARCH_MIGEMO_NEXT:
			ISearchEnter(SEARCH_MIGEMO, SEARCH_FORWARD);	//�O��MIGEMO�C���N�������^���T�[�`    //2004.10.13 isearch
			return true;
		case F_ISEARCH_MIGEMO_PREV:
			ISearchEnter(SEARCH_MIGEMO, SEARCH_BACKWARD);	//���MIGEMO�C���N�������^���T�[�`    //2004.10.13 isearch
			return true;
	}
	return false;
}

/*!
	�C���N�������^���T�[�`���[�h�ɓ���

	@param mode [in] �������@ 1:�ʏ�, 2:���K�\��, 3:MIGEMO
	@param direction [in] �������� 0:���(���), 1:�O��(����)

	@author isearch
	@date 2011.12.15 Moca m_sCurSearchOption/m_sSearchOption�Ɠ������Ƃ�
	@date 2012.10.11 novice m_sCurSearchOption/m_sSearchOption�̓�����switch�̑O�ɕύX
	@date 2012.10.11 novice MIGEMO�̏�����case���Ɉړ�
*/
void CEditView::ISearchEnter( ESearchMode mode, ESearchDirection direction)
{

	if (m_nISearchMode == mode ) {
		//�Ď��s
		m_nISearchDirection =  direction;
		
		if ( m_bISearchFirst ){
			m_bISearchFirst = false;
		}
		//������ƏC��
		ISearchExec(true);

	}else{
		//�C���N�������^���T�[�`���[�h�ɓ��邾��.		
		//�I��͈͂̉���
		if(IsTextSelected())	
			DisableSelectArea( true );

		m_sCurSearchOption = m_pShareData->m_Common.m_sSearch.m_sSearchOption;
		switch( mode ) {
			case SEARCH_NORMAL: // �ʏ�C���N�������^���T�[�`
				m_sCurSearchOption.bRegularExp = false;
				m_sCurSearchOption.bLoHiCase = false;
				m_sCurSearchOption.bWordOnly = false;
				//SendStatusMessage(_T("I-Search: "));
				break;
			case SEARCH_REGEXP: // ���K�\���C���N�������^���T�[�`
				if (!m_CurRegexp.IsAvailable()){
					WarningBeep();
					SendStatusMessage(_T("���K�\�����C�u�������g�p�ł��܂���B"));
					return;
				}
				m_sCurSearchOption.bRegularExp = true;
				m_sCurSearchOption.bLoHiCase = false;
				//SendStatusMessage(_T("[RegExp] I-Search: "));
				break;
			case SEARCH_MIGEMO: // MIGEMO�C���N�������^���T�[�`
				if (!m_CurRegexp.IsAvailable()){
					WarningBeep();
					SendStatusMessage(_T("���K�\�����C�u�������g�p�ł��܂���B"));
					return;
				}
				if(m_pcmigemo==NULL){
					m_pcmigemo = CMigemo::getInstance();
					m_pcmigemo->Init();
				}
				//migemo dll �`�F�b�N
				//	Jan. 10, 2005 genta �ݒ�ύX�Ŏg����悤�ɂȂ��Ă���
				//	�\��������̂ŁC�g�p�\�łȂ���Έꉞ�����������݂�
				if ( !m_pcmigemo->IsAvailable() && !m_pcmigemo->Init() ){
					WarningBeep();
					SendStatusMessage(_T("MIGEMO.DLL���g�p�ł��܂���B"));
					return;
				}
				m_pcmigemo->migemo_load_all();
				if (m_pcmigemo->migemo_is_enable()) {
					m_sCurSearchOption.bRegularExp = true;
					m_sCurSearchOption.bLoHiCase = false;
					//SendStatusMessage(_T("[MIGEMO] I-Search: "));
				}else{
					WarningBeep();
					SendStatusMessage(_T("MIGEMO�͎g�p�ł��܂���B "));
					return;
				}
				break;
		}
		
		//	Feb. 04, 2005 genta	�����J�n�ʒu���L�^
		//	�C���N�������^���T�[�`�ԂŃ��[�h��؂�ւ���ꍇ�ɂ͊J�n�ƌ��Ȃ��Ȃ�
		if( m_nISearchMode == SEARCH_NONE ){
			m_ptSrchStartPos_PHY.x = m_ptCaretPos_PHY.x;
			m_ptSrchStartPos_PHY.y = m_ptCaretPos_PHY.y;
		}
		
		m_bCurSrchKeyMark = false;
		m_nISearchDirection = direction;
		m_nISearchMode = mode;
		
		m_nISearchHistoryCount = 0;
		m_sISearchHistory[m_nISearchHistoryCount].m_ptFrom = m_ptCaretPos;
		m_sISearchHistory[m_nISearchHistoryCount].m_ptTo   = m_ptCaretPos;

		Redraw();
		
		CMemory msg;
		ISearchSetStatusMsg(&msg);
		SendStatusMessage(msg.GetStringPtr());
		
		m_bISearchWrap = false;
		m_bISearchFirst = true;
	}

	//�}�E�X�J�[�\���ύX
	if (direction == 1){
		::SetCursor( ::LoadCursor( m_hInstance,MAKEINTRESOURCE(IDC_CURSOR_ISEARCH_F)));
	}else{
		::SetCursor( ::LoadCursor( m_hInstance,MAKEINTRESOURCE(IDC_CURSOR_ISEARCH_B)));
	}
}

//!	�C���N�������^���T�[�`���[�h���甲����
void CEditView::ISearchExit()
{
	CShareData::getInstance()->AddToSearchKeyArr( m_szCurSrchKey );
	m_pShareData->m_Common.m_sSearch.m_sSearchOption = m_sCurSearchOption;
	m_pcEditWnd->AcceptSharedSearchKey();
	m_nISearchDirection = SEARCH_BACKWARD;
	m_nISearchMode = SEARCH_NONE;

	if (m_nISearchHistoryCount == 0){
		m_szCurSrchKey[0] = '\0';
	}

	//�}�E�X�J�[�\�������ɖ߂�
	POINT point1;
	GetCursorPos(&point1);
	OnMOUSEMOVE(0,point1.x,point1.y);

	//�X�e�[�^�X�\���G���A���N���A
	SendStatusMessage(_T(""));

}

/*!
	@brief �C���N�������^���T�[�`�̎��s(1�����ǉ�)
	
	@param wChar [in] �ǉ����镶�� (1byte or 2byte)
*/
void CEditView::ISearchExec(WORD wChar)
{
	//���ꕶ���͏������Ȃ�
	switch ( wChar){
		case '\r':
		case '\n':
			ISearchExit();
			return;
		//case '\t':
		//	break;
	}
	
	int l;
	if (m_bISearchFirst){
		m_bISearchFirst = false;
		l = 0 ;
	}else	
		l = _tcslen(m_szCurSrchKey) ;

	if( wChar <= 255 ){
		if( l < _countof(m_szCurSrchKey) - 1 ){
			m_szCurSrchKey[l] =(char)wChar;
			m_szCurSrchKey[l+1] = '\0';
		}
	}else{
		if( l < _countof(m_szCurSrchKey) - 2 ){
			m_szCurSrchKey[l]   = (char)(wChar>>8);
			m_szCurSrchKey[l+1] = (char)wChar;
			m_szCurSrchKey[l+2] = '\0';
		}
	}

	ISearchExec(false);
	return ;
}

/*!
	@brief �C���N�������^���T�[�`�̎��s(������ǉ�)
	
	@param pszText [in] �ǉ����镶����
*/
void CEditView::ISearchExec(const char* pszText)
{
	//�ꕶ�����������Ď��s

	const char* p;
	WORD  c;
	p = pszText;
	
	while(*p!='\0'){
		if(IsDBCSLeadByte(*p) ){
			c = ( ((WORD)*p) * 256) | (unsigned char)*(p+1);
			p++;
		}else{
			c = *p;
		}
		ISearchExec(c);
		p++;
	}
	return ;
}

/*!
	@brief �C���N�������^���T�[�`�̎��s

	@param bNext [in] true:���̌�������, false:���݂̃J�[�\���ʒu�̂܂܌���
*/
void CEditView::ISearchExec(bool bNext) 
{
	//���������s����.

	if ( (m_szCurSrchKey[0] == '\0') || (m_nISearchMode == SEARCH_NONE)){
		//�X�e�[�^�X�̕\��
		CMemory msg;
		ISearchSetStatusMsg(&msg);
		SendStatusMessage(msg.GetStringPtr());
		return ;
	}
	
	ISearchWordMake();
	
	int nLine;
	int nIdx1;
	
	if ( bNext && m_bISearchWrap ) {
		switch (m_nISearchDirection)
		{
		case SEARCH_FORWARD:
			nLine = 0;
			nIdx1 = 0;
			break;
		case SEARCH_BACKWARD:
			//�Ōォ�猟��
			int nLineP;
			int nIdxP;
			nLineP =  m_pcEditDoc->m_cDocLineMgr.GetLineCount() -1 ;
			const CDocLine* pDocLine = m_pcEditDoc->m_cDocLineMgr.GetLine( nLineP );
			nIdxP = pDocLine->GetLength() -1;
			m_pcEditDoc->m_cLayoutMgr.LogicToLayout(nIdxP,nLineP,&nIdx1,&nLine);
		}
	}else if (IsTextSelected()){
		switch( m_nISearchDirection * 2 + (bNext ? 1: 0)){
			case (SEARCH_FORWARD * 2): //�O�������Ō��݈ʒu���猟���̂Ƃ�
			case (SEARCH_BACKWARD * 2 + 1): //��������Ŏ��������̂Ƃ�
				//�I��͈͂̐擪�������J�n�ʒu��
				nLine = m_sSelect.m_ptFrom.y;
				nIdx1 = m_sSelect.m_ptFrom.x;
				break;
			case (SEARCH_BACKWARD * 2): //��������Ō��݈ʒu���猟��
			case (SEARCH_FORWARD * 2 + 1): //�O�������Ŏ�������
				//�I��͈͂̌�납��
				nLine = m_sSelect.m_ptTo.y;
				nIdx1 = m_sSelect.m_ptTo.x;
				break;
		}
	}else{
		nLine = m_ptCaretPos.y;
		nIdx1  = m_ptCaretPos.x;
	}

	//���ʒu����index�ɕϊ�
	const CLayout* pCLayout = m_pcEditDoc->m_cLayoutMgr.SearchLineByLayoutY( nLine );
	int nIdx = LineColumnToIndex( pCLayout, nIdx1 );

	m_nISearchHistoryCount ++ ;

	CMemory msg;
	ISearchSetStatusMsg(&msg);

	if (m_nISearchHistoryCount >= 256) {
		m_nISearchHistoryCount = 156;
		for(int i = 100 ; i<= 255 ; i++){
			m_bISearchFlagHistory[i-100] = m_bISearchFlagHistory[i];
			m_sISearchHistory[i-100] = m_sISearchHistory[i];
		}
	}
	m_bISearchFlagHistory[m_nISearchHistoryCount] = bNext;

	CLayoutRange sMatchRange;

	int nSearchResult = m_pcEditDoc->m_cLayoutMgr.SearchWord(
		nLine,						// �����J�n�s
		nIdx,						// �����J�n�ʒu
		m_szCurSrchKey,				// ��������
		m_nISearchDirection,		// ��������
		m_sCurSearchOption,			// 2011.12.15 Moca �F�����u�������v�Ɠ������Ƃ邽��m_sCurSearchOption�����̂܂܎w��
		&sMatchRange.m_ptFrom.y,	// �}�b�`���C�A�E�g�sfrom
		&sMatchRange.m_ptFrom.x,	// �}�b�`���C�A�E�g�ʒufrom
		&sMatchRange.m_ptTo.y,		// �}�b�`���C�A�E�g�sto
		&sMatchRange.m_ptTo.x,		// �}�b�`���C�A�E�g�ʒuto
		&m_CurRegexp
	);
	if( nSearchResult == 0 ){
		/*�������ʂ��Ȃ�*/
		msg.AppendString(_T(" (������܂���)"));
		SendStatusMessage(msg.GetStringPtr());
		
		if (bNext) 	m_bISearchWrap = true;
		if (IsTextSelected()){
			m_sISearchHistory[m_nISearchHistoryCount] = m_sSelect;
		}else{
			m_sISearchHistory[m_nISearchHistoryCount].m_ptFrom = m_ptCaretPos;
			m_sISearchHistory[m_nISearchHistoryCount].m_ptTo   = m_ptCaretPos;
		}
	}else{
		//�������ʂ���
		//�L�����b�g�ړ�
		MoveCursor( sMatchRange.m_ptFrom.x, sMatchRange.m_ptFrom.y, true, _CARETMARGINRATE / 3 );
		//	2005.06.24 Moca
		SetSelectArea( sMatchRange );

		m_bISearchWrap = false;
		m_sISearchHistory[m_nISearchHistoryCount] = sMatchRange;
	}

	m_bCurSrchKeyMark = true;

	Redraw();	
	SendStatusMessage(msg.GetStringPtr());
	return ;
}

//!	�o�b�N�X�y�[�X�������ꂽ�Ƃ��̏���
void CEditView::ISearchBack(void) {
	if(m_nISearchHistoryCount==0) return;
	
	if(m_nISearchHistoryCount==1){
		m_bCurSrchKeyMark = false;
		m_bISearchFirst = true;
	}else if( m_bISearchFlagHistory[m_nISearchHistoryCount] == false){
		//�����������ւ炷
		long l = _tcslen(m_szCurSrchKey);
		if (l > 0 ){
			//�Ō�̕����̈�O
			char* p = CharPrev(m_szCurSrchKey,&m_szCurSrchKey[l]);
			*p = '\0';
			//m_szCurSrchKey[l-1] = '\0';

			if ( (p - m_szCurSrchKey) > 0 ) 
				ISearchWordMake();
			else
				m_bCurSrchKeyMark = false;

		}else{
			WarningBeep();
		}
	}
	m_nISearchHistoryCount --;

	CLayoutRange sRange = m_sISearchHistory[m_nISearchHistoryCount];

	if(m_nISearchHistoryCount == 0){
		DisableSelectArea( true );
		sRange.m_ptTo.x = sRange.m_ptFrom.x;
	}

	MoveCursor( sRange.m_ptFrom.x, sRange.m_ptFrom.y, true, _CARETMARGINRATE / 3 );
	if(m_nISearchHistoryCount != 0){
		//	2005.06.24 Moca
		SetSelectArea( sRange );
	}

	Redraw();

	//�X�e�[�^�X�\��
	CMemory msg;
	ISearchSetStatusMsg(&msg);
	SendStatusMessage(msg.GetStringPtr());
	
}

//!	���͕�������A���������𐶐�����B
void CEditView::ISearchWordMake(void)
{
	int nFlag = 0x00;
	switch ( m_nISearchMode ) {
	case SEARCH_NORMAL: // �ʏ�C���N�������^���T�[�`
		break;
	case SEARCH_REGEXP: // ���K�\���C���N�������^���T�[�`
		if( !InitRegexp( m_hWnd, m_CurRegexp, true ) ){
			return ;
		}
		nFlag |= m_sCurSearchOption.bLoHiCase ? CBregexp::optCaseSensitive : 0;
		/* �����p�^�[���̃R���p�C�� */
		m_CurRegexp.Compile(m_szCurSrchKey , nFlag );
		break;
	case SEARCH_MIGEMO: // MIGEMO�C���N�������^���T�[�`
		if( !InitRegexp( m_hWnd, m_CurRegexp, true ) ){
			return ;
		}
		nFlag |= m_sCurSearchOption.bLoHiCase ? CBregexp::optCaseSensitive : 0;

		{
			//migemo�ő{��
			std::string strMigemoWord = m_pcmigemo->migemo_query_a((unsigned char *)m_szCurSrchKey);
			
			/* �����p�^�[���̃R���p�C�� */
			m_CurRegexp.Compile(strMigemoWord.c_str(), nFlag );

		}
		break;
	}
}

/*!	@brief ISearch���b�Z�[�W�\�z

	���݂̃T�[�`���[�h�y�ь����������񂩂�
	���b�Z�[�W�G���A�ɕ\�����镶������\�z����
	
	@param msg [out] ���b�Z�[�W�o�b�t�@
	
	@author isearch
	@date 2004/10/13
	@date 2005.01.13 genta ������C��
*/
void CEditView::ISearchSetStatusMsg(CMemory* msg) const
{

	switch ( m_nISearchMode){
	case SEARCH_NORMAL:
		msg->SetString(_T("I-Search") );
		break;
	case SEARCH_REGEXP:
		msg->SetString(_T("[RegExp] I-Search") );
		break;
	case SEARCH_MIGEMO:
		msg->SetString(_T("[Migemo] I-Search") );
		break;
	default:
		msg->SetString(_T(""));
		return;
	}
	if (m_nISearchDirection == SEARCH_BACKWARD){
		msg->AppendString(_T(" Backward: "));
	}
	else{
		msg->AppendString(_T(": "));
	}

	if(m_nISearchHistoryCount > 0)
		msg->AppendString(m_szCurSrchKey);
}

/*!
	ISearch��Ԃ��c�[���o�[�ɔ��f������D
	
	@sa CEditWnd::IsFuncChecked()

	@param nCommand [in] ���ׂ����R�}���h��ID
	@return true:�`�F�b�N�L�� / false: �`�F�b�N����
	
	@date 2005.01.10 genta �V�K�쐬
*/
bool CEditView::IsISearchEnabled(int nCommand) const
{
	switch( nCommand )
	{
	case F_ISEARCH_NEXT:
		return (m_nISearchMode == SEARCH_NORMAL) && (m_nISearchDirection == SEARCH_FORWARD);
	case F_ISEARCH_PREV:
		return (m_nISearchMode == SEARCH_NORMAL) && (m_nISearchDirection == SEARCH_BACKWARD);
	case F_ISEARCH_REGEXP_NEXT:
		return (m_nISearchMode == SEARCH_REGEXP) && (m_nISearchDirection == SEARCH_FORWARD);
	case F_ISEARCH_REGEXP_PREV:
		return (m_nISearchMode == SEARCH_REGEXP) && (m_nISearchDirection == SEARCH_BACKWARD);
	case F_ISEARCH_MIGEMO_NEXT:
		return (m_nISearchMode == SEARCH_MIGEMO) && (m_nISearchDirection == SEARCH_FORWARD);
	case F_ISEARCH_MIGEMO_PREV:
		return (m_nISearchMode == SEARCH_MIGEMO) && (m_nISearchDirection == SEARCH_BACKWARD);
	}
	return false;
}
/*[EOF]*/