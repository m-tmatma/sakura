/*!	@file
	@brief DLL�̃��[�h�A�A�����[�h

	@author genta
	@date Jun. 10, 2001
*/
/*
	Copyright (C) 2001, genta
	Copyright (C) 2002, YAZAKI, genta
	Copyright (C) 2007, genta

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

#ifndef _LOAD_LIBRARY_H_
#define _LOAD_LIBRARY_H_

#include <windows.h>
#include "global.h"

//! DLL�̓��I��Load/Unload���s�����߂̃N���X
/*!
	@author genta
	@date Jun. 10, 2001 genta
	@date Apr. 15, 2002 genta RegisterEntries�̒ǉ��B
*/
class CDllHandler {
public:
	/*!
		�A�h���X�ƃG���g�����̑Ή��\�BRegisterEntries�Ŏg����B
		@author YAZAKI
		@date 2002.01.26
	*/
	struct ImportTable{
		void*		proc;
		const char*	name;
	};

	CDllHandler();
	virtual ~CDllHandler();

	//! ���p��Ԃ̃`�F�b�N
	/*!
		DLL�̊֐����Ăяo���邩��Ԃǂ���

		@retval true ���p�\
		@retval false ���p�s�\
	*/
	virtual bool IsAvailable(void) const { return m_hInstance != NULL; }
	//! DLL�̃��[�h
	/*!
		@retval 0 ����I���BDLL�����[�h���ꂽ�B
		@retval other �ُ�I���BDLL�̓��[�h����Ȃ������B

		@date 2001.07.05 genta �����ǉ��B�p�X�̎w��ȂǂɎg����
		@date 2007.06.25 genta GetDllNameInOrder���g���悤�Ɏ�����ύX�D
	*/
	int LoadLibrary(const char* str = NULL);
	//! DLL�̃A�����[�h
	/*!
		@param force [in] �I�������Ɏ��s���Ă�DLL��������邩�ǂ���
	*/
	int FreeLibrary(bool force = false);

	//! ���p��Ԃ̃`�F�b�N�ioperator�Łj
	bool operator!(void) const { return IsAvailable(); }

	//!	DLL�̃��[�h
	/*!
		�ڍׂȖ߂�l��Ԃ��Ȃ����ƈȊO��LoadLibrary()�Ɠ���
	*/
	bool Init(const char* str = NULL){ return LoadLibrary(str) == 0; }

	//! �C���X�^���X�n���h���̎擾
	HINSTANCE GetInstance() const { return m_hInstance; }
protected:
	//!	DLL�̏�����
	/*!
		DLL�̃��[�h�ɐ�����������ɌĂяo�����D�G���g���|�C���g��
		�m�F�Ȃǂ��s���D

		@retval true ����I��
		@retval false �ُ�I��

		@note false��Ԃ����ꍇ�́A�ǂݍ���DLL���������D
	*/
	virtual bool InitDll(void) = 0;

	//!	�֐��̏�����
	/*!
		DLL�̃A�����[�h���s�����O�ɌĂяo�����D�������̉���Ȃǂ�
		�s���D

		@retval true ����I��
		@retval false �ُ�I��

		@note false��Ԃ����Ƃ���DLL��Unload�͍s���Ȃ��D
		@par ����
		�f�X�g���N�^����FreeLibrary�y��DeinitDll���Ăяo���ꂽ�Ƃ���
		�|�����[�t�B�Y�����s���Ȃ����߂ɃT�u�N���X��DeinitDll���Ăяo����Ȃ��B
		���̂��߁A�T�u�N���X�̃f�X�g���N�^�ł�DeinitDll�𖾎��I�ɌĂяo���K�v������B
		
		FreeLibrary���f�X�g���N�^�ȊO����Ăяo�����ꍇ��DeinitDll�͉��z�֐��Ƃ���
		�T�u�N���X�̂��̂��Ăяo����A�f�X�g���N�^�͓��R�Ăяo����Ȃ��̂�
		DeinitDll���̂��͕̂K�v�ł���B
		
		�f�X�g���N�^����DeinitDll���ĂԂƂ��́A����������Ă���Ƃ����ۏ؂��Ȃ��̂�
		�Ăяo���O��IsAvailable�ɂ��m�F��K���s�����ƁB
		
		@date 2002.04.15 genta ���ӏ����ǉ�
	*/
	virtual bool DeinitDll(void);

	//! DLL�t�@�C�����̎擾
	/*!
		@date 2001.07.05 genta �����ǉ��B�p�X�̎w��ȂǂɎg����
		@date 2007.06.25 genta GetDllNameInOrder���g�p����ꍇ�͕K�{�ł͂Ȃ��̂ŁC
			�������z�֐��͂�߂ăv���[�X�z���_�[��p�ӂ���D
	*/
	virtual LPCTSTR GetDllName(LPCTSTR);

	//! DLL�t�@�C�����̎擾(����������)
	/*!
		DLL�t�@�C�����Ƃ��ĕ����̉\��������C���̂����̈�ł�
		�����������̂��g�p����ꍇ�ɑΉ�����D
		
		�ԍ��ɉ����Ă��ꂼ��قȂ�t�@�C������Ԃ����Ƃ��ł���D
		LoadLibrary()�����counter��0����1�����������ď��ɌĂт������D
		�����DLL�̃��[�h�ɐ�������(����)���C�߂�l�Ƃ���NULL��Ԃ�(���s)
		�܂ő�������D

		�f�t�H���g�̎�����counter��0�̏ꍇ����GetDllName()���Ăт����C
		����ȊO�ł�NULL��Ԃ��D
		
		@param[in] str LoadLibrary()�ɓn���ꂽ�����񂪂��̂܂ܓn�����
		@param[in] index �C���f�b�N�X�D(0�`)
		
		@return �����ɉ�����DLL��(LoadLibrary�ɓn��������)�C�܂���NULL�D
	*/
	virtual LPCTSTR GetDllNameInOrder(LPCTSTR str, int index);

	bool RegisterEntries(const ImportTable table[]);

private:
	HINSTANCE		m_hInstance;
};

#endif


/*[EOF]*/