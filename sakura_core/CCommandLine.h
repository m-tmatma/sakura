/*!	@file
	@brief �R�}���h���C���p�[�T �w�b�_�t�@�C��

	@author aroka
	@date	2002/01/08 �쐬
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2000-2001, genta
	Copyright (C) 2002, aroka CEditApp��蕪��
	Copyright (C) 2002, genta
	Copyright (C) 2005, D.S.Koba
	Copyright (C) 2007, ryoji

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/

#ifndef _CCOMMANDLINE_H_
#define _CCOMMANDLINE_H_

#include "global.h"
#include "CShareData.h"	// EditInfo, GrepInfo

class CMemory;

/*!	�����I�v�V����
	20020118 aroka
*/
struct GrepInfo {
	CMemory			cmGrepKey;			//!< �����L�[
	CMemory			cmGrepFile;			//!< �����Ώۃt�@�C��
	CMemory			cmGrepFolder;		//!< �����Ώۃt�H���_
	SSearchOption	sGrepSearchOption;	//!< �����I�v�V����
	bool			bGrepSubFolder;		//!< �T�u�t�H���_����������
	bool			bGrepOutputLine;	//!< ���ʏo�͂ŊY���s���o�͂���
	int				nGrepOutputStyle;	//!< ���ʏo�͌`��
	ECodeType		nGrepCharSet;		//!< �����R�[�h�Z�b�g
};


/*-----------------------------------------------------------------------
�N���X�̐錾
-----------------------------------------------------------------------*/

/*!
	@brief �R�}���h���C���p�[�T �N���X
*/
class CCommandLine {
public:
	static CCommandLine* getInstance(){
		static CCommandLine instance;

		return &instance;
	}

private:
	static int CheckCommandLine(
		LPTSTR	str,		//!< [in] ���؂��镶����i�擪��-�͊܂܂Ȃ��j
		int quotelen, 		//!< [in] �I�v�V���������̈��p���̒����D�I�v�V�����S�̂����p���ň͂܂�Ă���ꍇ�̍l���D
		TCHAR**	arg,		//!< [out] ����������ꍇ�͂��̐擪�ւ̃|�C���^
		int*	arglen		//!< [out] �����̒���
	);
	
	// �O�����点�Ȃ��B
	CCommandLine();
	CCommandLine(CCommandLine const&);
	void operator=(CCommandLine const&);

	/*!
		���p���ň͂܂�Ă��鐔�l��F������悤�ɂ���
		@date 2002.12.05 genta
	*/
	static int AtoiOptionInt(const TCHAR* arg){
		return ( arg[0] == _T('"') || arg[0] == _T('\'') ) ?
			_ttoi( arg + 1 ) : _ttoi( arg );
	}

// member accessor method
public:
	bool IsNoWindow() const {return m_bNoWindow;}
	bool IsWriteQuit() const {return m_bWriteQuit;}	// 2007.05.19 ryoji sakuext�p�ɒǉ�
	bool IsGrepMode() const {return m_bGrepMode;}
	bool IsGrepDlg() const {return m_bGrepDlg;}
	bool IsDebugMode() const {return m_bDebugMode;}
	bool IsReadOnly() const {return m_bReadOnly;}
	bool GetEditInfo(EditInfo* fi) const { *fi = m_fi; return true; }
	bool GetGrepInfo(GrepInfo* gi) const { *gi = m_gi; return true; }
	int GetGroupId() const {return m_nGroup;}	// 2007.06.26 ryoji
	LPCSTR GetMacro() const{ return m_cmMacro.GetStringPtr(); }
	LPCSTR GetMacroType() const{ return m_cmMacroType.GetStringPtr(); }
	int GetFileNum(void) { return m_vFiles.size(); }
	const TCHAR* GetFileName(int i) { return i < GetFileNum() ? m_vFiles[i].c_str() : NULL; }
	void ClearFile(void) { m_vFiles.clear(); }
	void ParseCommandLine( LPCTSTR pszCmdLineSrc = NULL );

// member valiables
private:
	bool		m_bGrepMode;		//! [out] TRUE: Grep Mode
	bool		m_bGrepDlg;			//  Grep�_�C�A���O
	bool		m_bDebugMode;		
	bool		m_bNoWindow;		//! [out] TRUE: �ҏWWindow���J���Ȃ�
	bool		m_bWriteQuit;		//! [out] TRUE: �ݒ��ۑ����ďI��	// 2007.05.19 ryoji sakuext�p�ɒǉ�
	EditInfo	m_fi;				//!
	GrepInfo	m_gi;				//!
	bool		m_bReadOnly;		//! [out] TRUE: Read Only
	int			m_nGroup;			//! �O���[�vID	// 2007.06.26 ryoji
	CMemory		m_cmMacro;			//! [out] �}�N���t�@�C�����^�}�N����
	CMemory		m_cmMacroType;		//! [out] �}�N�����
	std::vector<std::string> m_vFiles;	//!< �t�@�C����(����)
};

///////////////////////////////////////////////////////////////////////
#endif /* _CCOMMANDLINE_H_ */


/*[EOF]*/