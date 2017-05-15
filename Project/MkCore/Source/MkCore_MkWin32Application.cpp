
#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#endif // _DEBUG

#include "MkCore_StackWalker.h"
#include "MkCore_MkStringOp.h"
#include "MkCore_MkCmdLine.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"


static MkCmdLine g_CmdLine;

//------------------------------------------------------------------------------------------------//

MkBaseFramework* MkWin32Application::CreateFramework(void) const
{
	return new MkBaseFramework;
}

void MkWin32Application::Run
(HINSTANCE hInstance, const wchar_t* title, const wchar_t* rootPath, bool useLog, eSystemWindowProperty sysWinProp,
 int x, int y, int clientWidth, int clientHeight, bool fullScreen, bool dragAccept, WNDPROC wndProc, const char* arg1, const char* arg2)
{
#ifdef _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc(1633); // 블럭 넘버 넣어주면 브레이크
#endif // _DEBUG

	HANDLE mutexHandle = NULL;
	
	__try
	{
		// argument
		g_CmdLine = arg1;
		g_CmdLine += arg2;

		// mutex를 이용한 중복 실행 방지 및 등록여부 점검
		if (!_CheckExcution(g_CmdLine, mutexHandle, title))
			return;

		// 프레임워크 객체 생성
		MkBaseFramework* frameworkPtr = CreateFramework();
		assert(frameworkPtr != NULL);

		// 프레임워크 초기화
		if (frameworkPtr->__Start(hInstance, title, rootPath, useLog, sysWinProp, x, y, clientWidth, clientHeight, fullScreen, dragAccept, wndProc, g_CmdLine))
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

bool MkWin32Application::_CheckExcution(MkCmdLine& cmdLine, HANDLE& myMutexHandle, const wchar_t* myTitle)
{
	// 자기 자신의 mutex 선언 여부 조사
	std::string myMutexKey;
	if (g_CmdLine.HasPair("#DMK")) // declare mutex key
	{
		myMutexKey = g_CmdLine.GetValue("#DMK", 0);
	}

	// 중복 실행 금지 여부 검사
	if (g_CmdLine.HasPair("#BME")) // block multiple excution
	{
		MkArray<std::string> checkList;

		if (g_CmdLine.HasPair("#AME")) // allow multiple excution
		{
			// 검사 대상이라도 중복실행 허용 리스트는 제외
			const MkArray<std::string>& blockList = g_CmdLine.GetValues("#BME");
			const MkArray<std::string>& allowList = g_CmdLine.GetValues("#AME");
			blockList.GetDifferenceOfSets(allowList, checkList); // checkList = blockList - allowList
		}
		else
		{
			checkList = g_CmdLine.GetValues("#BME");
		}

		MK_INDEXING_LOOP(checkList, i)
		{
			const std::string& currMutexKey = checkList[i];
			if (!currMutexKey.empty())
			{
				std::wstring keyBuffer;
				MkStringOp::ConvertString(currMutexKey, keyBuffer);

				HANDLE handle = ::CreateMutex(NULL, FALSE, keyBuffer.c_str()); // mutex 생성 시도
				if (handle != NULL)
				{
					// 해당 mutex가 이미 존재하면 탈출
					if (::GetLastError() == ERROR_ALREADY_EXISTS)
					{
						// 동일 어플을 실행중이면 front로 올림
						if ((!myMutexKey.empty()) && (myMutexKey == currMutexKey))
						{
							HWND runWnd = ::FindWindow(myTitle, myTitle);
							if (runWnd == NULL)
							{
								::MessageBox(NULL, L"Already running", myTitle, MB_OK);
							}
							else
							{
								::ShowWindow(runWnd, SW_SHOWNORMAL);
								::SetFocus(runWnd);
								::SetForegroundWindow(runWnd);
								::SetActiveWindow(runWnd);
							}
						}
						// 다른 어플일 경우 메세지만 출력
						else
						{
							::MessageBox(NULL, L"Already running", myTitle, MB_OK);
						}

						::CloseHandle(handle);
						return false; // 탈출
					}

					::CloseHandle(handle);
				}
			}
		}
	}

	// key가 존재하면 자신의 mutex 선언
	if (!myMutexKey.empty())
	{
		std::wstring keyBuffer;
		MkStringOp::ConvertString(myMutexKey, keyBuffer);

		myMutexHandle = ::CreateMutex(NULL, FALSE, keyBuffer.c_str()); // mutex 생성 시도
	}

	// 사용 종료된 cmd 삭제
	bool updateCmd = cmdLine.RemovePair("#DMK");
	updateCmd |= cmdLine.RemovePair("#AME");
	updateCmd |= cmdLine.RemovePair("#BME");
	if (updateCmd)
	{
		cmdLine.UpdateFullStr();
	}

	return true;
}

//------------------------------------------------------------------------------------------------//
