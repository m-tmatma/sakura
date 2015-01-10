/*!	@file
	@brief �R�}���h���C���p�[�T

	@author aroka
	@date	2002/01/08 �쐬
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2000-2001, genta
	Copyright (C) 2002, aroka CEditApp��蕪��
	Copyright (C) 2002, genta, Moca
	Copyright (C) 2005, D.S.Koba, genta, susu
	Copyright (C) 2006, ryoji, rastiv
	Copyright (C) 2007, ryoji, genta
	Copyright (C) 2009, syat

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/


#include "StdAfx.h"
#include "CCommandLine.h"
#include "CMemory.h"
#include <tchar.h>
#include <io.h>
#include <string.h>
#include "CRunningTimer.h"
#include "etc_uty.h"
#include "file.h"
#include "my_icmp.h"
#include "charcode.h"  // 2006.06.28 rastiv
#include "Debug.h"

/* �R�}���h���C���I�v�V�����p�萔 */
#define CMDLINEOPT_R			1002
#define CMDLINEOPT_NOWIN		1003
#define CMDLINEOPT_WRITEQUIT	1004
#define CMDLINEOPT_GREPMODE		1100
#define CMDLINEOPT_GREPDLG		1101
#define CMDLINEOPT_DEBUGMODE	1999
#define CMDLINEOPT_NOMOREOPT	1998
#define CMDLINEOPT_X			1
#define CMDLINEOPT_Y			2
#define CMDLINEOPT_VX			3
#define CMDLINEOPT_VY			4
#define CMDLINEOPT_TYPE			5
#define CMDLINEOPT_CODE			6
#define CMDLINEOPT_SX			7
#define CMDLINEOPT_SY			8
#define CMDLINEOPT_WX			9
#define CMDLINEOPT_WY			10
#define CMDLINEOPT_GKEY			101
#define CMDLINEOPT_GFILE		102
#define CMDLINEOPT_GFOLDER		103
#define CMDLINEOPT_GOPT			104
#define CMDLINEOPT_GCODE		105
#define CMDLINEOPT_M			106
#define CMDLINEOPT_MTYPE		107
#define CMDLINEOPT_GROUP		500

/*!
	�R�}���h���C���̃`�F�b�N���s���āA�I�v�V�����ԍ���
	����������ꍇ�͂��̐擪�A�h���X��Ԃ��B
	CCommandLine::ParseCommandLine()�Ŏg����B

	@return �I�v�V�����̔ԍ��B�ǂ�ɂ��Y�����Ȃ��Ƃ���0��Ԃ��B

	@author genta
	@date Apr. 6, 2001
	@date 2006.10.25 ryoji �I�v�V����������̑啶������������ʂ��Ȃ�
*/
int CCommandLine::CheckCommandLine(
	LPTSTR	str,		//!< [in] ���؂��镶����i�擪��-�͊܂܂Ȃ��j
	int quotelen,		//!< [in] �I�v�V���������̈��p���̒����D�I�v�V�����S�̂����p���ň͂܂�Ă���ꍇ�̍l���D
	TCHAR** arg,		//!< [out] ����������ꍇ�͂��̐擪�ւ̃|�C���^
	int*	arglen		//!< [out] �����̒���
)
{
	/*!
		�R�}���h���C���I�v�V������͗p�\���̔z��
	*/
	struct _CmdLineOpt {
		LPCTSTR opt;	//!< �I�v�V����������
		int len;		//!< �I�v�V�����̕����񒷁i�v�Z���Ȃ����߁j
		int value;		//!< �ϊ���̒l
	};

	/*!
		�R�}���h���C���I�v�V����
		���Ɉ��������Ȃ�����
	*/
	static const _CmdLineOpt _COptWoA[] = {
		{_T("R"),			1,	CMDLINEOPT_R},
		{_T("-"),			1,	CMDLINEOPT_NOMOREOPT},
		{_T("NOWIN"),		5,	CMDLINEOPT_NOWIN},
		{_T("WQ"),			2,	CMDLINEOPT_WRITEQUIT},	// 2007.05.19 ryoji sakuext�p�ɒǉ�
		{_T("GREPMODE"),	8,	CMDLINEOPT_GREPMODE},
		{_T("GREPDLG"),		7,	CMDLINEOPT_GREPDLG},
		{_T("DEBUGMODE"),	9,	CMDLINEOPT_DEBUGMODE},
		{NULL, 0, 0}
	};

	/*!
		�R�}���h���C���I�v�V����
		���Ɉ�����������
	*/
	static const _CmdLineOpt _COptWithA[] = {
		{_T("X"),		1,			CMDLINEOPT_X},
		{_T("Y"),		1,			CMDLINEOPT_Y},
		{_T("VX"),		2,			CMDLINEOPT_VX},
		{_T("VY"),		2,			CMDLINEOPT_VY},
		{_T("SX"),		2,			CMDLINEOPT_SX},
		{_T("SY"),		2,			CMDLINEOPT_SY},
		{_T("WX"),		2,			CMDLINEOPT_WX},
		{_T("WY"),		2,			CMDLINEOPT_WY},
		{_T("CODE"),	4,			CMDLINEOPT_CODE},	// 2002/09/20 Moca _COptWoA����ړ�
		{_T("TYPE"),	4,			CMDLINEOPT_TYPE},	//!< �^�C�v�ʐݒ� Mar. 7, 2002 genta
		{_T("GKEY"),	4,			CMDLINEOPT_GKEY},
		{_T("GFILE"),	5,			CMDLINEOPT_GFILE},
		{_T("GFOLDER"),	7,			CMDLINEOPT_GFOLDER},
		{_T("GOPT"),	4,			CMDLINEOPT_GOPT},
		{_T("GCODE"),	5,			CMDLINEOPT_GCODE},	// 2002/09/21 Moca �ǉ�
		{_T("GROUP"),	5,			CMDLINEOPT_GROUP},	// 2007.06.26 ryoji
		{_T("M"),		1,			CMDLINEOPT_M},		// 2009.07.19 syat
		{_T("MTYPE"),	5,			CMDLINEOPT_MTYPE},	// 2009.07.19 syat
		{NULL, 0, 0}
	};

	const _CmdLineOpt *ptr;
	int len = lstrlen( str ) - quotelen;

	//	����������ꍇ���Ɋm�F
	for( ptr = _COptWithA; ptr->opt != NULL; ptr++ )
	{
		if( len >= ptr->len &&	//	����������Ă��邩
			//	�I�v�V���������̒����`�F�b�N
			( str[ptr->len] == '=' || str[ptr->len] == ':' ) &&
			//	������̔�r
			my_memicmp( str, ptr->opt, ptr->len ) == 0 ){		// 2006.10.25 ryoji memcmp() -> _memicmp()
			*arg = str + ptr->len + 1;
			*arglen = len - ptr->len;
			return ptr->value;
		}
	}

	//	�������Ȃ��ꍇ
	for( ptr = _COptWoA; ptr->opt != NULL; ptr++ )
	{
		if( len == ptr->len &&	//	�����`�F�b�N
			my_memicmp( str, ptr->opt, ptr->len ) == 0 )	//	������̔�r	// 2006.10.25 ryoji memcmp() -> _memicmp()
		{
			*arglen = 0;
			return ptr->value;
		}
	}
	return 0;	//	�Y������
}

void strncpyWithCheckOverflow(char *dest, int destCount, char *src, int srcCount)
{
	if( destCount < srcCount + 1 ){
		dest[0] = '\0';
		TopErrorMessage( NULL,
					  _T("�t�@�C���p�X���������܂��B ANSI �łł� %d �o�C�g�ȏ�̐�΃p�X�������܂���B"),
					  destCount );
	}else{
		strcpy(dest, src);
	}
}


/*! �R�}���h���C���̉��

	WinMain()����Ăяo�����B
	
	@date 2005-08-24 D.S.Koba �֐���static����߁C�����o�ϐ��������œn���̂���߂�
	@date 2007.09.09 genta Visual Studio���e�X�̈��������߉�ɂ�""�ň͂ޖ��ɑΉ��D
		�I�v�V������""�ň͂܂ꂽ�ꍇ�ɑΉ�����D
		���������-�Ŏn�܂�t�@�C�������w��ł��Ȃ��Ȃ�̂ŁC
		����ȍ~�I�v�V������͂����Ȃ��Ƃ��� "--" �I�v�V������V�݂���D
	@date 2012.02.25 novice �����t�@�C���ǂݍ���

	@note
	���ꂪ�Ăяo���ꂽ���_�ł͋��L�������̏��������������Ă��Ȃ����߁C
	���L�������ɃA�N�Z�X���Ă͂Ȃ�Ȃ��D
*/
void CCommandLine::ParseCommandLine( LPCTSTR pszCmdLineSrc )
{
	MY_RUNNINGTIMER( cRunningTimer, "CCommandLine::Parse" );

	//	May 30, 2000 genta
	//	���s�t�@�C���������ƂɊ����R�[�h���Œ肷��D
	{
		TCHAR	exename[512];
		::GetModuleFileName( NULL, exename, _countof(exename) );

		int		len = _tcslen( exename );

		for( TCHAR *p = exename + len - 1; p > exename; p-- ){
			if( *p == _T('.') ){
				ECodeType n = (ECodeType)(p[-1] - _T('0'));
				if(IsValidCodeType(n))
					m_fi.m_nCharCode = n;
				break;
			}
		}
	}


	TCHAR	szPath[_MAX_PATH];
	bool	bFind = false;				// �t�@�C���������t���O
	bool	bParseOptDisabled = false;	// 2007.09.09 genta �I�v�V������͂��s��Ȃ��C�t�@�C�����Ƃ��Ĉ���
	int		nPos;
	int		i;
	if( pszCmdLineSrc[0] != _T('-') ){
		for( i = 0; i < _countof( szPath ); ++i ){
			if( pszCmdLineSrc[i] == _T(' ') || pszCmdLineSrc[i] == _T('\0') ){
				/* �t�@�C���̑��݂��`�F�b�N */
				szPath[i] = _T('\0');	// �I�[����
				if( fexist(szPath) ){
					bFind = true;
					break;
				}
				if( pszCmdLineSrc[i] == _T('\0') ){
					break;
				}
			}
			szPath[i] = pszCmdLineSrc[i];
		}
	}
	if( bFind ){
		ResolvePath(szPath);
		_tcscpy( m_fi.m_szPath, szPath );	/* �t�@�C���� */
		nPos = i + 1;
	}else{
		m_fi.m_szPath[0] = _T('\0');
		nPos = 0;
	}

	LPTSTR pszCmdLineWork = new TCHAR[lstrlen( pszCmdLineSrc ) + 1];
	_tcscpy( pszCmdLineWork, pszCmdLineSrc );
	int nCmdLineWorkLen = lstrlen( pszCmdLineWork );
	LPTSTR pszToken = my_strtok( pszCmdLineWork, nCmdLineWorkLen, &nPos, _T(" ") );
	while( pszToken != NULL )
	{
		DEBUG_TRACE( _T("OPT=[%ts]\n"), pszToken );

		//	2007.09.09 genta �I�v�V�������胋�[���ύX�D�I�v�V������͒�~��""�ň͂܂ꂽ�I�v�V�������l��
		if( ( bParseOptDisabled ||
			! (pszToken[0] == '-' || pszToken[0] == '"' && pszToken[1] == '-' ) )){

			if( pszToken[0] == _T('\"') ){
				CMemory cmWork;
				//	Nov. 3, 2005 genta
				//	�����̃N�H�[�e�[�V�����������ꍇ���l�����āC
				//	�Ōオ�_�u���N�H�[�g�̏ꍇ�̂ݎ�菜��
				//	�t�@�C�����ɂ͎g���Ȃ������Ȃ̂Ńt�@�C�����Ɋ܂܂�Ă���ꍇ�͍l���s�v
				//	�܂�SHIFT-JIS��2�o�C�g�ڂ̍l�����s�v
				//	Nov. 27, 2005 genta
				//	�������_�u���N�H�[�g1�̏ꍇ�ɁC����1���ŏ��ƍŌ��1����
				//	���ԈႦ�āC�C���f�b�N�X-1�ɃA�N�Z�X���Ă��܂��̂�h�����߂ɒ������`�F�b�N����
				//	�t�@�C�����̌��ɂ���Option����͂��邽�߁C���[�v�͌p��
				int len = lstrlen( pszToken + 1 );
				if( len > 0 ){
					cmWork.SetString( &pszToken[1], len - ( pszToken[len] == _T('"') ? 1 : 0 ));
					cmWork.Replace( _T("\"\""), _T("\"") );
					strncpyWithCheckOverflow(szPath, _countof(szPath), cmWork.GetStringPtr(), cmWork.GetStringLength());
				}
				else {
					szPath[0] = _T('\0');
				}
			}
			else{
				strncpyWithCheckOverflow(szPath, _countof(szPath), pszToken, strlen(pszToken));
			}

			// Nov. 11, 2005 susu
			// �s���ȃt�@�C�����̂܂܂��ƃt�@�C���ۑ����_�C�A���O���o�Ȃ��Ȃ�̂�
			// �ȒP�ȃt�@�C���`�F�b�N���s���悤�ɏC��
			if (!memcmp(szPath, _T("file:///"), 8)) {
				char tmp_str[_MAX_PATH + 1];
				_tcscpy(tmp_str, &(szPath[8]));
				_tcscpy(szPath, tmp_str);
			}
			int len = _tcslen(szPath);
			for (int i = 0; i < len ; i ++) {
				if ( (szPath[i] == '<' ||	//	0x3C
					  szPath[i] == '>' ||	//	0x3E
					  szPath[i] == '?' ||	//	0x3F
					  szPath[i] == '"' ||	//	0x22
					  szPath[i] == '|' ||	//	0x7C
					  szPath[i] == '*' ||	//	0x2A
					  0
					 ) &&
					( i ==0 || (i > 0 && ! _IS_SJIS_1( (unsigned char)(szPath[i - 1] )) ))){
						TCHAR msg_str[_MAX_PATH + 1];
						_stprintf(
							msg_str,
							_T("%s\r\n")
							_T("��L�̃t�@�C�����͕s���ł��B�t�@�C������ \\ / : * ? \" < > | �̕����͎g���܂���B "),
							szPath
						);
						MessageBox( NULL, msg_str, _T("FileNameError"), MB_OK);
						szPath[0] = _T('\0');
						break;
				}
			}

			if (szPath[0] != _T('\0')) {
				ResolvePath(szPath);
				if (m_fi.m_szPath[0] == _T('\0')) {
					_tcscpy(m_fi.m_szPath, szPath );
				}
				else {
					m_vFiles.push_back( szPath );
				}
			}
		}
		else{
			int nQuoteLen = 0;
			if( *pszToken == '"' ){
				++pszToken;	// 2007.09.09 genta �擪��"�̓X�L�b�v
				nQuoteLen = 1;
				int tokenlen = _tcslen( pszToken );
				if( pszToken[ tokenlen-1 ] == '"' ){	// 2009.07.19 syat ������"����菜��
					pszToken[ tokenlen-1 ] = '\0';
				}
			}
			++pszToken;	//	�擪��'-'��skip
			TCHAR *arg = NULL;
			int nArgLen;
			switch( CheckCommandLine( pszToken, nQuoteLen, &arg, &nArgLen ) ){
			case CMDLINEOPT_X: //	X
				/* �s���w���1�J�n�ɂ��� */
				m_fi.m_ptCursor.x = AtoiOptionInt( arg ) - 1;
				break;
			case CMDLINEOPT_Y:	//	Y
				m_fi.m_ptCursor.y = AtoiOptionInt( arg ) - 1;
				break;
			case CMDLINEOPT_VX:	// VX
				/* �s���w���1�J�n�ɂ��� */
				m_fi.m_nViewLeftCol = AtoiOptionInt( arg ) - 1;
				break;
			case CMDLINEOPT_VY:	//	VY
				/* �s���w���1�J�n�ɂ��� */
				m_fi.m_nViewTopLine = AtoiOptionInt( arg ) - 1;
				break;
			case CMDLINEOPT_SX: //	SX
				m_fi.m_nWindowSizeX = AtoiOptionInt( arg ) - 1;
				break;
			case CMDLINEOPT_SY:	//	SY
				m_fi.m_nWindowSizeY = AtoiOptionInt( arg ) - 1;
				break;
			case CMDLINEOPT_WX: //	WX
				m_fi.m_nWindowOriginX = AtoiOptionInt( arg );
				break;
			case CMDLINEOPT_WY:	//	WY
				m_fi.m_nWindowOriginY = AtoiOptionInt( arg );
				break;
			case CMDLINEOPT_TYPE:	//	TYPE
				//	Mar. 7, 2002 genta
				//	�t�@�C���^�C�v�̋����w��
				{
					_tcsncpy( m_fi.m_szDocType, arg, MAX_DOCTYPE_LEN );
					m_fi.m_szDocType[ nArgLen < MAX_DOCTYPE_LEN ? nArgLen : MAX_DOCTYPE_LEN ]= '\0';
				}
				break;
			case CMDLINEOPT_CODE:	//	CODE
				m_fi.m_nCharCode = (ECodeType)AtoiOptionInt( arg );
				break;
			case CMDLINEOPT_R:	//	R
				m_bReadOnly = true;
				break;
			case CMDLINEOPT_NOWIN:	//	NOWIN
				m_bNoWindow = true;
				break;
			case CMDLINEOPT_WRITEQUIT:	//	WRITEQUIT	// 2007.05.19 ryoji sakuext�p�ɒǉ�
				m_bWriteQuit = true;
				m_bNoWindow = true;	// 2007.09.05 ryoji -WQ���w�肳�ꂽ��-NOWIN���w�肳�ꂽ�Ƃ��Ĉ���
				break;
			case CMDLINEOPT_GREPMODE:	//	GREPMODE
				m_bGrepMode = true;
				if( _T('\0') == m_fi.m_szDocType[0] ){
					_tcscpy( m_fi.m_szDocType , _T("grepout") );
				}
				break;
			case CMDLINEOPT_GREPDLG:	//	GREPDLG
				m_bGrepDlg = true;
				break;
			case CMDLINEOPT_GKEY:	//	GKEY
				//	�O���""����菜��
				m_gi.cmGrepKey.SetString( arg + 1,  lstrlen( arg ) - 2 );
				m_gi.cmGrepKey.Replace( "\"\"", "\"" );
				break;
			case CMDLINEOPT_GFILE:	//	GFILE
				//	�O���""����菜��
				m_gi.cmGrepFile.SetString( arg + 1,  lstrlen( arg ) - 2 );
				m_gi.cmGrepFile.Replace( _T("\"\""), _T("\"") );
				break;
			case CMDLINEOPT_GFOLDER:	//	GFOLDER
				m_gi.cmGrepFolder.SetString( arg + 1,  lstrlen( arg ) - 2 );
				m_gi.cmGrepFolder.Replace( _T("\"\""), _T("\"") );
				break;
			case CMDLINEOPT_GOPT:	//	GOPT
				for( ; *arg != '\0' ; ++arg ){
					switch( *arg ){
					case 'S':
						// �T�u�t�H���_�������������
						m_gi.bGrepSubFolder = true;	break;
					case 'L':
						// �p�啶���Ɖp����������ʂ���
						m_gi.sGrepSearchOption.bLoHiCase = true;	break;
					case 'R':
						// ���K�\��
						m_gi.sGrepSearchOption.bRegularExp = true;	break;
					case 'K':
						// �����R�[�h��������
						// 2002/09/21 Moca �݊����ێ��̂��߂̏���
						m_gi.nGrepCharSet = CODE_AUTODETECT;	break;
					case 'P':
						// �s���o�͂��邩�Y�����������o�͂��邩
						m_gi.bGrepOutputLine = true;	break;
					case 'W':
						// �P��P�ʂŒT��
						m_gi.sGrepSearchOption.bWordOnly = true;	break;
					case '1':
						// Grep: �o�͌`��
						m_gi.nGrepOutputStyle = 1;	break;
					case '2':
						// Grep: �o�͌`��
						m_gi.nGrepOutputStyle = 2;	break;
					}
				}
				break;
			// 2002/09/21 Moca Grep�ł̕����R�[�h�Z�b�g �ǉ�
			case CMDLINEOPT_GCODE:
				m_gi.nGrepCharSet = (ECodeType)AtoiOptionInt( arg );	break;
			case CMDLINEOPT_GROUP:	// GROUP	// 2007.06.26 ryoji
				m_nGroup = AtoiOptionInt( arg );
				break;
			case CMDLINEOPT_DEBUGMODE:
				m_bDebugMode = true;
				// 2010.06.16 Moca -TYPE=output �����Ƃ���
				if( _T('\0') == m_fi.m_szDocType[0] ){
					_tcscpy( m_fi.m_szDocType , _T("output") );
				}
				break;
			case CMDLINEOPT_NOMOREOPT:	// 2007.09.09 genta ����ȍ~��������
				bParseOptDisabled = true;
				break;
			case CMDLINEOPT_M:			// 2009.07.19 syat �ǉ�
				m_cmMacro.SetString( arg );
				m_cmMacro.Replace( "\"\"", "\"" );
				break;
			case CMDLINEOPT_MTYPE:		// 2009.07.19 syat �ǉ�
				m_cmMacroType.SetString( arg );
				break;
			}
		}
		pszToken = my_strtok( pszCmdLineWork, nCmdLineWorkLen, &nPos, _T(" ") );
	}
	delete [] pszCmdLineWork;

	return;
}

/*! 
	�R���X�g���N�^
	
	@date 2005-08-24 D.S.Koba ParseCommandLine()�ύX�ɂ�胁���o�ϐ��ɏ����l���
*/
CCommandLine::CCommandLine()
{
	m_bGrepMode				= false;
	m_bGrepDlg				= false;
	m_bDebugMode			= false;
	m_bNoWindow				= false;
	m_bWriteQuit			= false;
	m_gi.bGrepSubFolder		= false;
	m_gi.sGrepSearchOption.Reset();
	/*
	m_gi.bGrepNoIgnoreCase	= false;
	m_gi.bGrepRegularExp	= false;
	m_gi.bGrepWordOnly		= false;
	*/
	m_gi.nGrepCharSet		= CODE_SJIS;
	m_gi.bGrepOutputLine	= false;
	m_gi.nGrepOutputStyle	= 1;
	m_bReadOnly				= false;
	m_nGroup				= -1;		// 2007.06.26 ryoji
}
/*[EOF]*/