
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
 int x, int y, int clientWidth, int clientHeight, bool fullScreen, bool dragAccept, WNDPROC wndProc, const wchar_t* argAddition)
{
	__try
	{
		if (!Initialize(title, argAddition))
			return;

		// 프레임워크 객체 생성
		m_Framework = CreateFramework();
		if (m_Framework != NULL)
		{
			// 프레임워크 실행
			m_Framework->__Start(hInstance, title, rootPath, useLog, sysWinProp, x, y, clientWidth, clientHeight, fullScreen, dragAccept, wndProc, g_CmdLine);

			// 프레임워크 삭제
			MK_DELETE(m_Framework);
		}
	}
	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))
	{
	}
}

bool MkWin32Application::Initialize(const wchar_t* title, const wchar_t* argAddition)
{
	assert(title != NULL);

	// 문자열 초기화
	MkStr::SetUp();

	// argument
	g_CmdLine.SetUp();
	g_CmdLine += argAddition;

	// mutex를 이용한 중복 실행 방지 및 등록여부 점검
	return _CheckExcution(g_CmdLine, m_MutexHandle, title);
}

bool MkWin32Application::SetUpFramework(HWND hWnd, const wchar_t* rootPath, bool useLog, bool dragAccept)
{
	// 프레임워크 객체 생성
	m_Framework = CreateFramework();

	// 프레임워크 초기화
	if ((m_Framework != NULL) && m_Framework->__Start(hWnd, rootPath, useLog, dragAccept, g_CmdLine))
		return true;

	MK_DELETE(m_Framework);
	return false;
}

void MkWin32Application::Update(void)
{
	if (m_Framework != NULL)
	{
		m_Framework->__Update();
	}
}

void MkWin32Application::Close(void)
{
	if (m_Framework != NULL)
	{
		m_Framework->__Close();
	}
}

void MkWin32Application::Destroy(void)
{
	if (m_Framework != NULL)
	{
		m_Framework->__Destroy();
	}
}

bool MkWin32Application::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return MkBaseFramework::MsgProc(hWnd, msg, wParam, lParam);
}

const MkCmdLine& MkWin32Application::GetCmdLine(void)
{
	return g_CmdLine;
}

MkWin32Application::MkWin32Application()
{
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc(1633); // 블럭 넘버 넣어주면 브레이크
#endif // _DEBUG

	m_MutexHandle = NULL;
	m_Framework = NULL;
}

MkWin32Application::~MkWin32Application()
{
	MK_DELETE(m_Framework);

	if (m_MutexHandle != NULL)
	{
		::CloseHandle(m_MutexHandle);
	}
}

bool MkWin32Application::_CheckExcution(MkCmdLine& cmdLine, HANDLE& myMutexHandle, const wchar_t* myTitle)
{
	// 자기 자신의 mutex 선언 여부 조사
	MkStr myMutexKey;
	if (cmdLine.HasPair(L"#DMK")) // declare mutex key
	{
		myMutexKey = cmdLine.GetValue(L"#DMK", 0);
	}

	// 중복 실행 금지 여부 검사
	if (cmdLine.HasPair(L"#BME")) // block multiple excution
	{
		MkArray<MkStr> checkList;

		if (cmdLine.HasPair(L"#AME")) // allow multiple excution
		{
			// 검사 대상이라도 중복실행 허용 리스트는 제외
			const MkArray<MkStr>& blockList = cmdLine.GetValues(L"#BME");
			const MkArray<MkStr>& allowList = cmdLine.GetValues(L"#AME");
			blockList.GetDifferenceOfSets(allowList, checkList); // checkList = blockList - allowList
		}
		else
		{
			checkList = cmdLine.GetValues(L"#BME");
		}

		MK_INDEXING_LOOP(checkList, i)
		{
			const MkStr& currMutexKey = checkList[i];
			if (!currMutexKey.Empty())
			{
				HANDLE handle = ::CreateMutex(NULL, FALSE, currMutexKey.GetPtr()); // mutex 생성 시도
				if (handle != NULL)
				{
					// 해당 mutex가 이미 존재하면 탈출
					if (::GetLastError() == ERROR_ALREADY_EXISTS)
					{
						// 동일 어플을 실행중이면 front로 올림
						if ((!myMutexKey.Empty()) && (myMutexKey == currMutexKey))
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
	if (!myMutexKey.Empty())
	{
		myMutexHandle = ::CreateMutex(NULL, FALSE, myMutexKey.GetPtr()); // mutex 생성 시도
	}

	// 사용 종료된 cmd 삭제
	cmdLine.RemovePair(L"#DMK");
	cmdLine.RemovePair(L"#AME");
	cmdLine.RemovePair(L"#BME");

	return true;
}

//------------------------------------------------------------------------------------------------//
