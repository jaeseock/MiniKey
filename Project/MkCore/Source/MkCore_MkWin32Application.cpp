
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
	//_CrtSetBreakAlloc(1633); // 블럭 넘버 넣어주면 브레이크
#endif // _DEBUG

	HANDLE mutexHandle = NULL;
	__try
	{
		// mutex를 이용한 중복 실행 방지
#ifndef _DEBUG
		if (wcslen(MKDEF_BLOCK_DUP_EXCUTE_NAME) > 0)
		{
			mutexHandle = ::CreateMutex(NULL, FALSE, MKDEF_BLOCK_DUP_EXCUTE_NAME);
			if ((mutexHandle != NULL) && (::GetLastError() == ERROR_ALREADY_EXISTS))
			{
				HWND runWnd = ::FindWindow(title, title);
				if (runWnd == NULL)
				{
					::MessageBox(NULL, L"Already running", title, MB_OK);
				}
				else
				{
					::ShowWindow(runWnd, SW_SHOWNORMAL);
					::SetFocus(runWnd);
					::SetForegroundWindow(runWnd);
					::SetActiveWindow(runWnd);
				}

				::CloseHandle(mutexHandle);
				return;
			}
		}
#endif

		// 프레임워크 객체 생성
		MkBaseFramework* frameworkPtr = CreateFramework();
		assert(frameworkPtr != NULL);

		// 프레임워크 초기화
		if (frameworkPtr->__Start(hInstance, title, rootPath, useLog, sysWinProp, x, y, clientWidth, clientHeight, fullScreen, dragAccept, wndProc, arg))
		{
			// 루핑
			frameworkPtr->__Run();
		}

		// 종료
		frameworkPtr->__End();
		delete frameworkPtr;
	}
	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))
	{
	}

	if (mutexHandle != NULL)
	{
		::CloseHandle(mutexHandle);
	}
}

//------------------------------------------------------------------------------------------------//
