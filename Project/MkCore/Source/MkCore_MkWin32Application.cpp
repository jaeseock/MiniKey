
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
	//_CrtSetBreakAlloc(1633); // �� �ѹ� �־��ָ� �극��ũ
#endif // _DEBUG

	HANDLE mutexHandle = NULL;
	
	__try
	{
		// argument
		g_CmdLine = arg1;
		g_CmdLine += arg2;

		// mutex�� �̿��� �ߺ� ���� ���� �� ��Ͽ��� ����
		if (!_CheckExcution(g_CmdLine, mutexHandle, title))
			return;

		// �����ӿ�ũ ��ü ����
		MkBaseFramework* frameworkPtr = CreateFramework();
		assert(frameworkPtr != NULL);

		// �����ӿ�ũ �ʱ�ȭ
		if (frameworkPtr->__Start(hInstance, title, rootPath, useLog, sysWinProp, x, y, clientWidth, clientHeight, fullScreen, dragAccept, wndProc, g_CmdLine))
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

	if (mutexHandle != NULL)
	{
		::CloseHandle(mutexHandle);
	}
}

bool MkWin32Application::_CheckExcution(MkCmdLine& cmdLine, HANDLE& myMutexHandle, const wchar_t* myTitle)
{
	// �ڱ� �ڽ��� mutex ���� ���� ����
	std::string myMutexKey;
	if (g_CmdLine.HasPair("#DMK")) // declare mutex key
	{
		myMutexKey = g_CmdLine.GetValue("#DMK", 0);
	}

	// �ߺ� ���� ���� ���� �˻�
	if (g_CmdLine.HasPair("#BME")) // block multiple excution
	{
		MkArray<std::string> checkList;

		if (g_CmdLine.HasPair("#AME")) // allow multiple excution
		{
			// �˻� ����̶� �ߺ����� ��� ����Ʈ�� ����
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

				HANDLE handle = ::CreateMutex(NULL, FALSE, keyBuffer.c_str()); // mutex ���� �õ�
				if (handle != NULL)
				{
					// �ش� mutex�� �̹� �����ϸ� Ż��
					if (::GetLastError() == ERROR_ALREADY_EXISTS)
					{
						// ���� ������ �������̸� front�� �ø�
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
						// �ٸ� ������ ��� �޼����� ���
						else
						{
							::MessageBox(NULL, L"Already running", myTitle, MB_OK);
						}

						::CloseHandle(handle);
						return false; // Ż��
					}

					::CloseHandle(handle);
				}
			}
		}
	}

	// key�� �����ϸ� �ڽ��� mutex ����
	if (!myMutexKey.empty())
	{
		std::wstring keyBuffer;
		MkStringOp::ConvertString(myMutexKey, keyBuffer);

		myMutexHandle = ::CreateMutex(NULL, FALSE, keyBuffer.c_str()); // mutex ���� �õ�
	}

	// ��� ����� cmd ����
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
