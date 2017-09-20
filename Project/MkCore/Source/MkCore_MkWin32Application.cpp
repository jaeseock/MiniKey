
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

		// �����ӿ�ũ ��ü ����
		m_Framework = CreateFramework();
		if (m_Framework != NULL)
		{
			// �����ӿ�ũ ����
			m_Framework->__Start(hInstance, title, rootPath, useLog, sysWinProp, x, y, clientWidth, clientHeight, fullScreen, dragAccept, wndProc, g_CmdLine);

			// �����ӿ�ũ ����
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

	// ���ڿ� �ʱ�ȭ
	MkStr::SetUp();

	// argument
	g_CmdLine.SetUp();
	g_CmdLine += argAddition;

	// mutex�� �̿��� �ߺ� ���� ���� �� ��Ͽ��� ����
	return _CheckExcution(g_CmdLine, m_MutexHandle, title);
}

bool MkWin32Application::SetUpFramework(HWND hWnd, const wchar_t* rootPath, bool useLog, bool dragAccept)
{
	// �����ӿ�ũ ��ü ����
	m_Framework = CreateFramework();

	// �����ӿ�ũ �ʱ�ȭ
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
	//_CrtSetBreakAlloc(1633); // �� �ѹ� �־��ָ� �극��ũ
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
	// �ڱ� �ڽ��� mutex ���� ���� ����
	MkStr myMutexKey;
	if (cmdLine.HasPair(L"#DMK")) // declare mutex key
	{
		myMutexKey = cmdLine.GetValue(L"#DMK", 0);
	}

	// �ߺ� ���� ���� ���� �˻�
	if (cmdLine.HasPair(L"#BME")) // block multiple excution
	{
		MkArray<MkStr> checkList;

		if (cmdLine.HasPair(L"#AME")) // allow multiple excution
		{
			// �˻� ����̶� �ߺ����� ��� ����Ʈ�� ����
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
				HANDLE handle = ::CreateMutex(NULL, FALSE, currMutexKey.GetPtr()); // mutex ���� �õ�
				if (handle != NULL)
				{
					// �ش� mutex�� �̹� �����ϸ� Ż��
					if (::GetLastError() == ERROR_ALREADY_EXISTS)
					{
						// ���� ������ �������̸� front�� �ø�
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
	if (!myMutexKey.Empty())
	{
		myMutexHandle = ::CreateMutex(NULL, FALSE, myMutexKey.GetPtr()); // mutex ���� �õ�
	}

	// ��� ����� cmd ����
	cmdLine.RemovePair(L"#DMK");
	cmdLine.RemovePair(L"#AME");
	cmdLine.RemovePair(L"#BME");

	return true;
}

//------------------------------------------------------------------------------------------------//
