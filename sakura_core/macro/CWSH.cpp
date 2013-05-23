/*!	@file
	@brief WSH Handler

	@author �S
	@date 2002�N4��28��
*/
/*
	Copyright (C) 2002, �S, genta
	Copyright (C) 2003, FILE
	Copyright (C) 2004, genta
	Copyright (C) 2005, FILE, zenryaku
	Copyright (C) 2009, syat

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
#include <process.h> // _beginthreadex
#ifdef __MINGW32__
#define INITGUID 1
#endif
#include <ObjBase.h>
#include <InitGuid.h>
#include <ShlDisp.h>
#include "macro/CWSH.h"
#include "macro/CIfObj.h"
#include "window/CEditWnd.h"
#include "util/os.h"
#include "util/module.h"
#include "util/window.h"	// BlockingHook
#include "dlg/CDlgCancel.h"
#include "sakura_rc.h"
#ifndef SCRIPT_E_REPORTED
#define	SCRIPT_E_REPORTED	0x80020101L	// ActivScp.h(VS2012)�Ɠ����l�Ȍ`�ɕύX
#endif

/* 2009.10.29 syat �C���^�t�F�[�X�I�u�W�F�N�g������CWSHIfObj.h�ɕ���
class CInterfaceObjectTypeInfo: public ImplementsIUnknown<ITypeInfo>
 */

//IActiveScriptSite, IActiveScriptSiteWindow
/*!
	@date Sep. 15, 2005 FILE IActiveScriptSiteWindow�����D
		�}�N����MsgBox���g�p�\�ɂ���D
*/
class CWSHSite: public IActiveScriptSite, public IActiveScriptSiteWindow
{
private:
	CWSHClient *m_Client;
	ITypeInfo *m_TypeInfo;
	ULONG m_RefCount;
public:
	CWSHSite(CWSHClient *AClient): m_RefCount(0), m_Client(AClient)
	{
	}

	virtual ULONG _stdcall AddRef() {
		return ++m_RefCount;
	}

	virtual ULONG _stdcall Release() {
		if(--m_RefCount == 0)
		{
			delete this;
			return 0;
		}
		return m_RefCount;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(
	    /* [in] */ REFIID iid,
	    /* [out] */ void ** ppvObject)
	{
		*ppvObject = NULL;

		if(iid == IID_IActiveScriptSiteWindow){
			*ppvObject = static_cast<IActiveScriptSiteWindow*>(this);
			++m_RefCount;
			return S_OK;
		}

		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetLCID( 
	    /* [out] */ LCID *plcid) 
	{ 
#ifdef TEST
		cout << "GetLCID" << endl;
#endif
		return E_NOTIMPL; //�V�X�e���f�t�H���g���g�p
	}

	virtual HRESULT STDMETHODCALLTYPE GetItemInfo( 
	    /* [in] */ LPCOLESTR pstrName,
	    /* [in] */ DWORD dwReturnMask,
	    /* [out] */ IUnknown **ppiunkItem,
	    /* [out] */ ITypeInfo **ppti) 
	{
#ifdef TEST
		wcout << L"GetItemInfo:" << pstrName << endl;
#endif
		//�w�肳�ꂽ���O�̃C���^�t�F�[�X�I�u�W�F�N�g������
		const CWSHClient::List& objects = m_Client->GetInterfaceObjects();
		for( CWSHClient::ListIter it = objects.begin(); it != objects.end(); it++ )
		{
			//	Nov. 10, 2003 FILE Win9X�ł́A[lstrcmpiW]�������̂��߁A[_wcsicmp]�ɏC��
			if( _wcsicmp( pstrName, (*it)->m_sName.c_str() ) == 0 )
			{
				if(dwReturnMask & SCRIPTINFO_IUNKNOWN)
				{
					(*ppiunkItem) = *it;
					(*ppiunkItem)->AddRef();
				}
				if(dwReturnMask & SCRIPTINFO_ITYPEINFO)
				{
					(*it)->GetTypeInfo(0, 0, ppti);
				}
				return S_OK;
			}
		}
		return TYPE_E_ELEMENTNOTFOUND;
	}

	virtual HRESULT STDMETHODCALLTYPE GetDocVersionString( 
	    /* [out] */ BSTR *pbstrVersion) 
	{ 
#ifdef TEST
		cout << "GetDocVersionString" << endl;
#endif
		return E_NOTIMPL; 
	}

	virtual HRESULT STDMETHODCALLTYPE OnScriptTerminate( 
	    /* [in] */ const VARIANT *pvarResult,
	    /* [in] */ const EXCEPINFO *pexcepinfo) 
	{ 
#ifdef TEST
		cout << "OnScriptTerminate" << endl;
#endif
		return S_OK; 
	}

	virtual HRESULT STDMETHODCALLTYPE OnStateChange( 
	    /* [in] */ SCRIPTSTATE ssScriptState) 
	{ 
#ifdef TEST
		cout << "OnStateChange" << endl;
#endif
		return S_OK; 
	}

	//	Nov. 3, 2002 �S
	//	�G���[�s�ԍ��\���Ή�
	virtual HRESULT STDMETHODCALLTYPE OnScriptError(
	  /* [in] */ IActiveScriptError *pscripterror)
	{ 
		EXCEPINFO Info;
		if(pscripterror->GetExceptionInfo(&Info) == S_OK)
		{
			DWORD Context;
			ULONG Line;
			LONG Pos;
			if(Info.bstrDescription == NULL) {
				Info.bstrDescription = SysAllocString(L"�}�N���̎��s�𒆒f���܂����B");
			}
			if(pscripterror->GetSourcePosition(&Context, &Line, &Pos) == S_OK)
			{
				wchar_t *Message = new wchar_t[SysStringLen(Info.bstrDescription) + 128];
				//	Nov. 10, 2003 FILE Win9X�ł́A[wsprintfW]�������̂��߁A[auto_sprintf]�ɏC��
				const wchar_t* szDesc=Info.bstrDescription;
				auto_sprintf(Message, L"[Line %d] %ls", Line + 1, szDesc);
				SysReAllocString(&Info.bstrDescription, Message);
				delete[] Message;
			}
			m_Client->Error(Info.bstrDescription, Info.bstrSource);
			SysFreeString(Info.bstrSource);
			SysFreeString(Info.bstrDescription);
			SysFreeString(Info.bstrHelpFile);
		}
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE OnEnterScript() {
#ifdef TEST
		cout << "OnEnterScript" << endl;
#endif
		return S_OK; 
	}

	virtual HRESULT STDMETHODCALLTYPE OnLeaveScript() {
#ifdef TEST
		cout << "OnLeaveScript" << endl;
#endif
		return S_OK; 
	}

	//	Sep. 15, 2005 FILE IActiveScriptSiteWindow����
	virtual HRESULT __stdcall GetWindow(
	    /* [out] */ HWND *phwnd)
	{
		*phwnd = CEditWnd::getInstance()->m_cSplitterWnd.GetHwnd();
		return S_OK;
	}

	//	Sep. 15, 2005 FILE IActiveScriptSiteWindow����
	virtual HRESULT __stdcall EnableModeless(
	    /* [in] */ BOOL fEnable)
	{
		return S_OK;
	}
};

//implementation

CWSHClient::CWSHClient(wchar_t const *AEngine, ScriptErrorHandler AErrorHandler, void *AData): 
				m_Engine(NULL), m_Data(AData), m_OnError(AErrorHandler), m_Valid(false)
{ 
	// 2010.08.28 DLL �C���W�F�N�V�����΍�Ƃ���EXE�̃t�H���_�Ɉړ�����
	CCurrentDirectoryBackupPoint dirBack;
	ChangeCurrentDirectoryToExeDir();
	
	CLSID ClassID;
	if(CLSIDFromProgID(AEngine, &ClassID) != S_OK)
		Error(L"�w���̃X�N���v�g�G���W����������܂���");
	else
	{
		if(CoCreateInstance(ClassID, 0, CLSCTX_INPROC_SERVER, IID_IActiveScript, reinterpret_cast<void **>(&m_Engine)) != S_OK)
			Error(L"�w���̃X�N���v�g�G���W�����쐬�ł��܂���");
		else
		{
			IActiveScriptSite *Site = new CWSHSite(this);
			if(m_Engine->SetScriptSite(Site) != S_OK)
			{
				delete Site;
				Error(L"�T�C�g��o�^�ł��܂���");
			}
			else
			{
				m_Valid = true;
			}
		}
	}
}

CWSHClient::~CWSHClient()
{
	//�C���^�t�F�[�X�I�u�W�F�N�g�����
	for( ListIter it = m_IfObjArr.begin(); it != m_IfObjArr.end(); it++ ){
		(*it)->Release();
	}
	
	if(m_Engine != NULL) 
		m_Engine->Release();
}

// AbortMacroProc�̃p�����[�^�\����
typedef struct {
	HANDLE hEvent;
	IActiveScript *pEngine;				//ActiveScript
	int nCancelTimer;
	CEditView *view;
} SAbortMacroParam;

// WSH�}�N�����s�𒆎~����X���b�h
static unsigned __stdcall AbortMacroProc( LPVOID lpParameter )
{
	SAbortMacroParam* pParam = (SAbortMacroParam*) lpParameter;

	//��~�_�C�A���O�\���O�ɐ��b�҂�
	if(::WaitForSingleObject(pParam->hEvent, pParam->nCancelTimer * 1000) == WAIT_TIMEOUT){
		//��~�_�C�A���O�\��
		DEBUG_TRACE(_T("AbortMacro: Show Dialog\n"));

		MSG msg;
		CDlgCancel cDlgCancel;
		HWND hwndDlg = cDlgCancel.DoModeless(G_AppInstance(), NULL, IDD_MACRORUNNING);	// �G�f�B�^�r�W�[�ł��\���ł���悤�A�e���w�肵�Ȃ�
		// �_�C�A���O�^�C�g���ƃt�@�C������ݒ�
		::SendMessage(hwndDlg, WM_SETTEXT, 0, (LPARAM)GSTR_APPNAME);
		::SendMessage(GetDlgItem(hwndDlg, IDC_STATIC_CMD),
			WM_SETTEXT, 0, (LPARAM)pParam->view->GetDocument()->m_cDocFile.GetFilePath());
		
		bool bCanceled = false;
		for(;;){
			DWORD dwResult = MsgWaitForMultipleObjects( 1, &pParam->hEvent, FALSE, INFINITE, QS_ALLINPUT );
			if(dwResult == WAIT_OBJECT_0){
				::SendMessage( cDlgCancel.GetHwnd(), WM_CLOSE, 0, 0 );
			}else if(dwResult == WAIT_OBJECT_0+1){
				while(::PeekMessage(&msg , NULL , 0 , 0, PM_REMOVE )){
					if(cDlgCancel.GetHwnd() != NULL && ::IsDialogMessage(cDlgCancel.GetHwnd(), &msg)){
					}else{
						::TranslateMessage(&msg);
						::DispatchMessage(&msg);
					}
				}
			}else{
				//MsgWaitForMultipleObjects�ɗ^�����n���h���̃G���[
				break;
			}
			if(!bCanceled && cDlgCancel.IsCanceled()){
				DEBUG_TRACE(_T("Canceld\n"));
				bCanceled = true;
				cDlgCancel.CloseDialog( 0 );
			}
			if(cDlgCancel.GetHwnd() == NULL){
				DEBUG_TRACE(_T("Close\n"));
				break;
			}
		}

		DEBUG_TRACE(_T("AbortMacro: Try Interrupt\n"));
		pParam->pEngine->InterruptScriptThread(SCRIPTTHREADID_BASE, NULL, 0);
		DEBUG_TRACE(_T("AbortMacro: Done\n"));
	}

	DEBUG_TRACE(_T("AbortMacro: Exit\n"));
	return 0;
}


void CWSHClient::Execute(wchar_t const *AScript)
{
	IActiveScriptParse *Parser;
	if(m_Engine->QueryInterface(IID_IActiveScriptParse, reinterpret_cast<void **>(&Parser)) != S_OK)
		Error(L"�p�[�T���擾�ł��܂���");
	else 
	{
		if(Parser->InitNew() != S_OK)
			Error(L"�������ł��܂���");
		else
		{
			bool bAddNamedItemError = false;

			for( ListIter it = m_IfObjArr.begin(); it != m_IfObjArr.end(); it++ )
			{
				DWORD dwFlag = SCRIPTITEM_ISVISIBLE;

				if( (*it)->IsGlobal() ){ dwFlag |= SCRIPTITEM_GLOBALMEMBERS; }

				if(m_Engine->AddNamedItem( (*it)->Name(), dwFlag ) != S_OK)
				{
					bAddNamedItemError = true;
					Error(L"�I�u�W�F�N�g��n���Ȃ�����");
					break;
				}
			}
			if( !bAddNamedItemError )
			{
				//�}�N����~�X���b�h�̋N��
				SAbortMacroParam sThreadParam;
				sThreadParam.pEngine = m_Engine;
				sThreadParam.nCancelTimer = GetDllShareData().m_Common.m_sMacro.m_nMacroCancelTimer;
				sThreadParam.view = (CEditView*)m_Data;

				HANDLE hThread = NULL;
				unsigned int nThreadId = 0;
				if( 0 < sThreadParam.nCancelTimer ){
					sThreadParam.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
					hThread = (HANDLE)_beginthreadex( NULL, 0, AbortMacroProc, (LPVOID)&sThreadParam, 0, &nThreadId );
					DEBUG_TRACE(_T("Start AbortMacroProc 0x%08x\n"), nThreadId);
				}

				//�}�N�����s
				if(m_Engine->SetScriptState(SCRIPTSTATE_STARTED) != S_OK)
					Error(L"��ԕύX�G���[");
				else
				{
					HRESULT hr = Parser->ParseScriptText(AScript, 0, 0, 0, 0, 0, SCRIPTTEXT_ISVISIBLE, 0, 0);
					if (hr == SCRIPT_E_REPORTED) {
					/*
						IActiveScriptSite->OnScriptError�ɒʒm�ς݁B
						���f���b�Z�[�W�����ɕ\������Ă�͂��B
					*/
					} else if(hr != S_OK) {
						Error(L"���s�Ɏ��s���܂���");
					}
				}

				if( 0 < sThreadParam.nCancelTimer ){
					::SetEvent(sThreadParam.hEvent);

					//�}�N����~�X���b�h�̏I���҂�
					DEBUG_TRACE(_T("Waiting for AbortMacroProc to finish\n"));
					::WaitForSingleObject(hThread, INFINITE); 
					::CloseHandle(hThread);
					::CloseHandle(sThreadParam.hEvent);
				}
			}
		}
		Parser->Release();
	}
	m_Engine->Close();
}

void CWSHClient::Error(BSTR Description, BSTR Source)
{
	if(m_OnError != NULL)
		m_OnError(Description, Source, m_Data);
}

void CWSHClient::Error(wchar_t* Description)
{
	BSTR S = SysAllocString(L"WSH");
	BSTR D = SysAllocString(Description);
	Error(D, S);
	SysFreeString(S);
	SysFreeString(D);
}

//�C���^�t�F�[�X�I�u�W�F�N�g�̒ǉ�
void CWSHClient::AddInterfaceObject( CIfObj* obj )
{
	if( !obj ) return;
	m_IfObjArr.push_back( obj );
	obj->m_Owner = this;
	obj->AddRef();
}


/////////////////////////////////////////////
/*!
	MacroCommand��CWSHIfObj.cpp�ֈړ�
	CWSHMacroManager ���@CWSHManager.cpp�ֈړ�

*/
