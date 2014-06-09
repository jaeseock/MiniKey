#ifndef __MINIKEY_CORE_MKWIN32APPLICATION_H__
#define __MINIKEY_CORE_MKWIN32APPLICATION_H__

//------------------------------------------------------------------------------------------------//
// win32 application
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include "MkCore_MkGlobalDefinition.h"


class MkBaseFramework;

class MkWin32Application
{
public:

	// �����ӿ�ũ ���� ��ȯ
	virtual MkBaseFramework* CreateFramework(void) const;

	// application ����
	// ������ ���� ��Ģ
	// - fullScreen�� false�� ���
	//   * clientWidth, clientHeight�� 0���� Ŭ ��� �ش� ũ�⸸ŭ ������ ����
	//   * clientWidth, clientHeight�� 0�� ��� �۾����� ũ�⸸ŭ ������ ����
	// - normal window application�� �� fullScreen�� true�� ���
	//   * �ǹ� ����. ��
	// - rendering window application�� �� fullScreen�� true�� ���
	//   * clientWidth, clientHeight�� 0���� Ŭ ��� ��ġ���� ����ϴ� ũ��� ������ ����
	//   * clientWidth, clientHeight�� 0�� ��� ����ȭ�� ũ��� ������ ����
	void Run(
		HINSTANCE hInstance,
		const wchar_t* title = L"MiniKey",
		const wchar_t* rootPath = L"..\\FileRoot",
		bool useLog = true,
		eSystemWindowProperty sysWinProp = eSWP_All,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int clientWidth = 640,
		int clientHeight = 480,
		bool fullScreen = false,
		bool dragAccept = false,
		WNDPROC wndProc = NULL,
		const char* arg = NULL
		);

	virtual ~MkWin32Application() {};
};

//------------------------------------------------------------------------------------------------//

#endif
