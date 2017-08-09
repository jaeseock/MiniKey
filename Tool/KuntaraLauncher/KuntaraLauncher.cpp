
// KuntaraLauncher.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
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

	//SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	CKuntaraLauncherDlg dlg;
	m_pMainWnd = &dlg;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}

	return FALSE;
}

int CKuntaraLauncherApp::ExitInstance()
{
	GP_MGR.Clear();
	return CWinAppEx::ExitInstance();
}
