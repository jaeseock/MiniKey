
// KuntaraLauncher.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CKuntaraLauncherApp:
// �� Ŭ������ ������ ���ؼ��� KuntaraLauncher.cpp�� �����Ͻʽÿ�.
//

class CKuntaraLauncherApp : public CWinAppEx
{
public:
	CKuntaraLauncherApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CKuntaraLauncherApp theApp;