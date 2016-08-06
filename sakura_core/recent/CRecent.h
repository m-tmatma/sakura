/*!	@file
	@brief �ŋߎg�������X�g

	���C�ɓ�����܂ލŋߎg�������X�g���Ǘ�����B

	@author MIK
	@date Apr. 05, 2003
	@date Apr. 03, 2005

	@date Oct. 19, 2007 kobake �^�`�F�b�N�������悤�ɁA�Đ݌v
*/
/*
	Copyright (C) 2003, MIK
	Copyright (C) 2005, MIK

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
#ifndef SAKURA_CRECENT_E523D9BB_0DBF_4642_846A_990A2B5122CF9_H_
#define SAKURA_CRECENT_E523D9BB_0DBF_4642_846A_990A2B5122CF9_H_

#include "_main/global.h"
#include "env/DLLSHAREDATA.h"

class CRecent{
public:
	virtual ~CRecent(){}

	//�C���X�^���X�Ǘ�
	virtual void	Terminate() = 0;

	//�A�C�e��
	virtual const TCHAR*	GetItemText( int nIndex ) const = 0;
	virtual int				GetArrayCount() const = 0;
	virtual int				GetItemCount() const = 0;
	virtual void			DeleteAllItem() = 0;
	virtual bool			DeleteItemsNoFavorite() = 0;
	virtual bool			DeleteItem( int nIndex ) = 0;	//!< �A�C�e�����N���A
	virtual bool			AppendItemText(const TCHAR* pszText) = 0;
	virtual bool			EditItemText( int nIndex, const TCHAR* pszText) = 0;

	int FindItemByText(const TCHAR* pszText) const
	{
		int n = GetItemCount();
		for(int i=0;i<n;i++){
			if(_tcscmp(GetItemText(i),pszText)==0)return i;
		}
		return -1;
	}

	//���C�ɓ���
	virtual bool	SetFavorite( int nIndex, bool bFavorite = true ) = 0;	//!< ���C�ɓ���ɐݒ�
	virtual bool	IsFavorite(int nIndex) const = 0;						//!< ���C�ɓ��肩���ׂ�

	//���̑�
	virtual int		GetViewCount() const = 0;
	virtual bool	UpdateView() = 0;

	// ���L�������A�N�Z�X
	DLLSHAREDATA*	GetShareData()
	{
		return &GetDllShareData();
	}
};

#include "CRecentImp.h"

#endif /* SAKURA_CRECENT_E523D9BB_0DBF_4642_846A_990A2B5122CF9_H_ */
/*[EOF]*/