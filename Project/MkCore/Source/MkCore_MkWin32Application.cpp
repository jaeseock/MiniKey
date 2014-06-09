
#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#endif // _DEBUG

#include "MkCore_StackWalker.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

//------------------------------------------------------------------------------------------------//

MkBaseFramework* MkWin32Application::CreateFramework(void) const
{
	return new MkBaseFramework;
}

void MkWin32Application::Run
(HINSTANCE hInstance, const wchar_t* title, const wchar_t* rootPath, bool useLog, eSystemWindowProperty sysWinProp,
 int x, int y, int clientWidth, int clientHeight, bool fullScreen, bool dragAccept, WNDPROC wndProc, const char* arg)
{
#ifdef _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc(1633); // �� �ѹ� �־��ָ� �극��ũ
#endif // _DEBUG

	__try
	{
		// �����ӿ�ũ ��ü ����
		MkBaseFramework* frameworkPtr = CreateFramework();
		assert(frameworkPtr != NULL);

		// �����ӿ�ũ �ʱ�ȭ
		if (frameworkPtr->__Start(hInstance, title, rootPath, useLog, sysWinProp, x, y, clientWidth, clientHeight, fullScreen, dragAccept, wndProc, arg))
		{
			// ����
			frameworkPtr->__Run();
		}

		// ����
		frameworkPtr->__End();
		delete frameworkPtr;
	}
	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))
	{
	}
}

//------------------------------------------------------------------------------------------------//
