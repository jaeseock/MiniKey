
// KuntaraLauncher.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "KuntaraLauncher.h"
#include "KuntaraLauncherDlg.h"

#include "MkCore_MkLayeredWindow.h"

#include "MkAppManager.h"
#include "GDIPlusManager.h"
#include "CWebBrowser2.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//------------------------------------------------------------------------------------------------//

//static bool g_RunGame = false;
//static MkLayeredWindow g_LayeredWindow;

//------------------------------------------------------------------------------------------------//

// CKuntaraLauncherApp

BEGIN_MESSAGE_MAP(CKuntaraLauncherApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CKuntaraLauncherApp::CKuntaraLauncherApp()
{
}

CKuntaraLauncherApp theApp;


BOOL CKuntaraLauncherApp::InitInstance()
{
	if (!APP_MGR.GetApplication().Initialize(m_pszAppName, L"#DMK=_MkLauncher; #BME=_MkStarter; #BME=_MkLauncher; #BME=_MkGameApp; #HDP="))
		return FALSE;

	GP_MGR.SetUp();

	CWinAppEx::InitInstance();
	AfxEnableControlContainer();

	//SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

	CKuntaraLauncherDlg dlg;
	m_pMainWnd = &dlg;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ [���]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}

	return FALSE;
}

int CKuntaraLauncherApp::ExitInstance()
{
	GP_MGR.Clear();
	return CWinAppEx::ExitInstance();
}
