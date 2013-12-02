/*!	@file
	@brief �\���p�����񓙂̎擾

	�@�\���C�@�\���ށC�@�\�ԍ��Ȃǂ̕ϊ��D�ݒ��ʂł̕\���p�������p�ӂ���D

	@author genta
	@date Oct.  1, 2001 �}�N��
	@date Oct. 15, 2001 �J�X�^�����j���[
*/
/*
	Copyright (C) 2001, genta
	Copyright (C) 2007, ryoji

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose, 
	including commercial applications, and to alter it and redistribute it 
	freely, subject to the following restrictions:

		1. The origin of this software must not be misrepresented;
		   you must not claim that you wrote the original software.
		   If you use this software in a product, an acknowledgment
		   in the product documentation would be appreciated but is
		   not required.

		2. Altered source versions must be plainly marked as such, 
		   and must not be misrepresented as being the original software.

		3. This notice may not be removed or altered from any source
		   distribution.
*/

#include "StdAfx.h"
#include "func/CFuncLookup.h"
#include "plugin/CJackManager.h"

//	�I�t�Z�b�g�l
const int LUOFFSET_MACRO = 0;
const int LUOFFSET_CUSTMENU = 1;
const int LUOFFSET_PLUGIN = 2;

/*!	@brief ���ޒ��̈ʒu�ɑΉ�����@�\�ԍ���Ԃ��D

	@param category [in] ���ޔԍ� (0-)
	@param position [in] ���ޒ���index (0-)
	@param bGetUnavailable [in] ���o�^�}�N���ł��@�\�ԍ���Ԃ�

	@retval �@�\�ԍ�

	@date 2007.11.02 ryoji bGetUnavailable�p�����[�^�ǉ�
*/
EFunctionCode CFuncLookup::Pos2FuncCode( int category, int position, bool bGetUnavailable ) const
{
	if( category < 0 || position < 0 )
		return F_DISABLE;

	if( category < nsFuncCode::nFuncKindNum ){
		if( position < nsFuncCode::pnFuncListNumArr[category] )
			return nsFuncCode::ppnFuncListArr[category][position];
	}
	else if( category == nsFuncCode::nFuncKindNum + LUOFFSET_MACRO ){
		//	�L�[���蓖�ă}�N��
		if( position < MAX_CUSTMACRO ){
			if( bGetUnavailable || m_pMacroRec[position].IsEnabled() )
				return (EFunctionCode)(F_USERMACRO_0 + position);
		}
	}
	else if( category == nsFuncCode::nFuncKindNum + LUOFFSET_CUSTMENU ){
		//	�J�X�^�����j���[
		if( position == 0 )
			return F_MENU_RBUTTON;
		else if( position < MAX_CUSTOM_MENU )
			return (EFunctionCode)(F_CUSTMENU_BASE + position);
	}
	else if( category == nsFuncCode::nFuncKindNum + LUOFFSET_PLUGIN ){
		//	�v���O�C��
		return CJackManager::getInstance()->GetCommandCode( position );
	}
	return F_DISABLE;
}

/*!	@brief ���ޒ��̈ʒu�ɑΉ�����@�\���̂�Ԃ��D

	@retval true �w�肳�ꂽ�@�\�ԍ��͒�`����Ă���
	@retval false �w�肳�ꂽ�@�\�ԍ��͖���`

	@date 2007.11.02 ryoji �������ȑf��
*/
bool CFuncLookup::Pos2FuncName(
	int		category,	//!< [in]  ���ޔԍ� (0-)
	int		position,	//!< [in]  ���ޒ���index (0-)
	WCHAR*	ptr,		//!< [out] ��������i�[����o�b�t�@�̐擪
	int		bufsize		//!< [in]  ��������i�[����o�b�t�@�̃T�C�Y
) const
{
	int funccode = Pos2FuncCode( category, position );
	return Funccode2Name( funccode, ptr, bufsize );
}

/*!	@brief �@�\�ԍ��ɑΉ�����@�\���̂�Ԃ��D

	@param funccode [in] �@�\�ԍ�
	@param ptr [out] ��������i�[����o�b�t�@�̐擪
	@param bufsize [in] ��������i�[����o�b�t�@�̃T�C�Y
	
	@retval true �w�肳�ꂽ�@�\�ԍ��͒�`����Ă���
	@retval false �w�肳�ꂽ�@�\�ԍ��͖���`

	@date 2007.11.02 ryoji ���o�^�}�N������������i�[�D�߂�l�̈Ӗ���ύX�i������͕K���i�[�j�D
*/
bool CFuncLookup::Funccode2Name( int funccode, WCHAR* ptr, int bufsize ) const
{
	LPCWSTR pszStr = NULL;

	if( F_USERMACRO_0 <= funccode && funccode < F_USERMACRO_0 + MAX_CUSTMACRO ){
		int position = funccode - F_USERMACRO_0;
		if( m_pMacroRec[position].IsEnabled() ){
			const TCHAR *p = m_pMacroRec[position].GetTitle();
			_tcstowcs( ptr, p, bufsize - 1 );
			ptr[ bufsize - 1 ] = LTEXT('\0');
		}else{
			_snwprintf( ptr, bufsize, LTEXT("�}�N�� %d (���o�^)"), position );
			ptr[ bufsize - 1 ] = LTEXT('\0');
		}
		return true;
	}
	else if( funccode == F_MENU_RBUTTON ){
		Custmenu2Name( 0, ptr, bufsize );
		ptr[bufsize-1] = LTEXT('\0');
		return true;
	}
	else if( F_CUSTMENU_1 <= funccode && funccode < F_CUSTMENU_BASE + MAX_CUSTOM_MENU ){	// MAX_CUSTMACRO->MAX_CUSTOM_MENU	2010/3/14 Uchi
		Custmenu2Name( funccode - F_CUSTMENU_BASE, ptr, bufsize );
		ptr[bufsize-1] = LTEXT('\0');
		return true;
	}
	else if( F_MENU_FIRST <= funccode && funccode < F_MENU_NOT_USED_FIRST ){
		if( ( pszStr = LSW( funccode ) )[0] != L'\0' ){
			wcsncpy( ptr, pszStr, bufsize );
			ptr[bufsize-1] = LTEXT('\0');
			return true;	// ��`���ꂽ�R�}���h
		}
	}
	else if( F_PLUGCOMMAND_FIRST <= funccode && funccode < F_PLUGCOMMAND_LAST ){
		if( CJackManager::getInstance()->GetCommandName( funccode, ptr, bufsize ) > 0 ){
			return true;	// �v���O�C���R�}���h
		}
	}

	// ����`�R�}���h
	if( ( pszStr = LSW( funccode ) )[0] != L'\0' ){
		wcsncpy( ptr, pszStr, bufsize );
		ptr[bufsize-1] = LTEXT('\0');
		return false;
	}
	return false;
}

/*!	@brief �@�\���ޔԍ��ɑΉ�����@�\���̂�Ԃ��D

	@param category [in] �@�\���ޔԍ�
	
	@return NULL ���ޖ��́D�擾�Ɏ��s������NULL�D
*/
const TCHAR* CFuncLookup::Category2Name( int category ) const
{
	if( category < 0 )
		return NULL;

	if( category < nsFuncCode::nFuncKindNum ){
		return LS( nsFuncCode::ppszFuncKind[category] );
	}
	else if( category == nsFuncCode::nFuncKindNum + LUOFFSET_MACRO ){
		return LS( STR_ERR_DLGFUNCLKUP1 );
	}
	else if( category == nsFuncCode::nFuncKindNum + LUOFFSET_CUSTMENU ){
		return LS( STR_ERR_DLGFUNCLKUP2 );
	}
	else if( category == nsFuncCode::nFuncKindNum + LUOFFSET_PLUGIN ){
		return LS( STR_ERR_DLGFUNCLKUP19 );
	}
	return NULL;
}

/*!	@brief ComboBox�ɗ��p�\�ȋ@�\���ވꗗ��o�^����

	@param hComboBox [in(out)] �f�[�^��ݒ肷��R���{�{�b�N�X
*/
void CFuncLookup::SetCategory2Combo( HWND hComboBox ) const
{
	int i;

	//	�R���{�{�b�N�X������������
	Combo_ResetContent( hComboBox );

	//	�Œ�@�\���X�g
	for( i = 0; i < nsFuncCode::nFuncKindNum; ++i ){
		Combo_AddString( hComboBox, LS( nsFuncCode::ppszFuncKind[i] ) );
	}

	//	���[�U�}�N��
	Combo_AddString( hComboBox, LS( STR_ERR_DLGFUNCLKUP1 ) );
	//	�J�X�^�����j���[
	Combo_AddString( hComboBox, LS( STR_ERR_DLGFUNCLKUP2 ) );
	//	�v���O�C��
	Combo_AddString( hComboBox, LS( STR_ERR_DLGFUNCLKUP19 ) );
}

/*!	@brief �w�肳�ꂽ���ނɑ�����@�\���X�g��ListBox�ɓo�^����D
	
	@param hListBox [in(out)] �l��ݒ肷�郊�X�g�{�b�N�X
	@param category [in] �@�\���ޔԍ�

	@date 2007.11.02 ryoji ����`�R�}���h�͏��O�D�������ȑf���D
*/
void CFuncLookup::SetListItem( HWND hListBox, int category ) const
{
	WCHAR pszLabel[256];
	int n;
	int i;

	//	���X�g������������
	List_ResetContent( hListBox );

	n = GetItemCount( category );
	for( i = 0; i < n; i++ ){
		if( Pos2FuncCode( category, i ) == F_DISABLE )
			continue;
		Pos2FuncName( category, i, pszLabel, _countof(pszLabel) );
		List_AddString( hListBox, pszLabel );
	}
}

/*!
	�w�蕪�ޒ��̋@�\�����擾����D
	
	@param category [in] �@�\���ޔԍ�
*/
int CFuncLookup::GetItemCount(int category) const
{
	if( category < 0 )
		return 0;

	if( category < nsFuncCode::nFuncKindNum ){
		return nsFuncCode::pnFuncListNumArr[category];
	}
	else if( category == nsFuncCode::nFuncKindNum + LUOFFSET_MACRO ){
		//	�}�N��
		return MAX_CUSTMACRO;
	}
	else if( category == nsFuncCode::nFuncKindNum + LUOFFSET_CUSTMENU ){
		//	�J�X�^�����j���[
		return MAX_CUSTOM_MENU;
	}
	else if( category == nsFuncCode::nFuncKindNum + LUOFFSET_PLUGIN ){
		//	�v���O�C���R�}���h
		return CJackManager::getInstance()->GetCommandCount();
	}
	return 0;
}

/*!	@brief �ԍ��ɑΉ�����J�X�^�����j���[���̂�Ԃ��D

	@param index [in] �J�X�^�����j���[�ԍ�
	
	@return NULL ���ޖ��́D�擾�Ɏ��s������NULL�D
*/
const WCHAR* CFuncLookup::Custmenu2Name( int index, WCHAR buf[], int bufSize ) const
{
	if( index < 0 || CUSTMENU_INDEX_FOR_TABWND < index )
		return NULL;

	// ���ʐݒ�Ŗ��̂�ݒ肵�Ă���΂����Ԃ�
	if ( m_pCommon->m_sCustomMenu.m_szCustMenuNameArr[ index ][0] != '\0' ) {
		return m_pCommon->m_sCustomMenu.m_szCustMenuNameArr[ index ];
	}

	// ���ʐݒ�Ŗ��ݒ�̏ꍇ�A���\�[�X�̃f�t�H���g����Ԃ�
	if( index == 0 ){
		wcscpyn( buf, LSW( STR_CUSTMENU_RIGHT_CLICK ), bufSize );
		return buf;
	}
	else if( index == CUSTMENU_INDEX_FOR_TABWND ){
		wcscpyn( buf, LSW( STR_CUSTMENU_TAB ), bufSize );
		return buf;
	}
	else {
		swprintf( buf, LSW( STR_CUSTMENU_CUSTOM ), index );
		return buf;
	}

	return NULL;
}