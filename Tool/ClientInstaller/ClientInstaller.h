
// ClientInstaller.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CClientInstallerApp:
// �� Ŭ������ ������ ���ؼ��� ClientInstaller.cpp�� �����Ͻʽÿ�.
//

class CClientInstallerApp : public CWinAppEx
{
public:
	CClientInstallerApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CClientInstallerApp theApp;