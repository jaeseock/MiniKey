
#include "MkCore_MkCheck.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkTitleBarHooker.h"

#include "MkCore_MkLogManager.h"
#include "MkCore_MkSystemEnvironment.h"
#include "MkCore_MkSharedPointerManager.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkProfilingManager.h"
#include "MkCore_MkTimeManager.h"
#include "MkCore_MkInputManager.h"
#include "MkCore_MkKeywordFilter.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkBackgroundLoader.h"
#include "MkCore_MkPageManager.h"

#include "MkCore_MkLogicThreadUnit.h"
#include "MkCore_MkLoadingThreadUnit.h"

#include "MkCore_MkBaseFramework.h"

// �� ���ǵ� ������ �̸�
#define MKDEF_MAIN_THREAD_NAME L"Main"
#define MKDEF_LOGIC_THREAD_UNIT_NAME L"Logic"
#define MKDEF_LOADING_THREAD_UNIT_NAME L"BG Loading"

// �����ӿ�ũ�� WndProc�� ���� �������� ���� ������ �����̹Ƿ� �۷ι� ������ ������ �������� �ʾƵ� ��

// main window title bar hooker
static MkTitleBarHooker gMainWindowTitleBarHooker;

// dragged file path list
MkArray<MkPathName> gDraggedFilePathList;

// set cursor msg
bool gRecieveCursorMsg = false;

// set cursor msg
bool gDestroyMsg = false;

// main window
MkListeningWindow* gMainWindowPtr = NULL;

//------------------------------------------------------------------------------------------------//

MkBaseThreadUnit* MkBaseFramework::CreateLogicThreadUnit(const MkHashStr& threadName) const
{
	return NULL;
}

MkBaseThreadUnit* MkBaseFramework::CreateLoadingThreadUnit(const MkHashStr& threadName) const
{
	return new MkLoadingThreadUnit(threadName);
}

MkListeningWindow* MkBaseFramework::GetMainWindowPtr(void)
{
	return gMainWindowPtr;
}

void MkBaseFramework::Close(void)
{
	gDestroyMsg = true;
}

//------------------------------------------------------------------------------------------------//

bool MkBaseFramework::__Start
(HINSTANCE hInstance, const wchar_t* title, const wchar_t* rootPath, bool useLog, eSystemWindowProperty sysWinProp,
 int x, int y, int clientWidth, int clientHeight, bool fullScreen, bool dragAccept, WNDPROC wndProc, const MkCmdLine& cmdLine)
{
	assert(title != NULL);
	assert(rootPath != NULL);

	// ����Ű, ����Ű ��� ����
	m_ASKSetter.TurnOffStickyAndFilterKeys();

	// �Ǽ� ���е� ����
	m_FPUSetter.FixPrecision();

	// ���ڿ� ����
	MkStr::SetUp();
	MkPathName::SetUp(rootPath);

	// �̱��� ����
	m_InstanceDeallocator.Reserve(64);

	// 0.
	m_InstanceDeallocator.RegisterInstance(new MkSystemEnvironment());
	MkSystemEnvironment::eBuildMode buildMode = MK_SYS_ENV.GetBuildMode();
	bool notShipping = (buildMode == MkSystemEnvironment::eDebug) || (buildMode == MkSystemEnvironment::eRelease);

	// ����׺��尡 �ƴϰ� ��Ƽ�ھ��̸� LFH ���
	if ((buildMode != MkSystemEnvironment::eDebug) && (MK_SYS_ENV.GetNumberOfProcessors() > 1))
	{
		_TurnOnLowFragmentationHeap();
	}

	// 1.
	m_InstanceDeallocator.RegisterInstance(new MkLogManager(useLog));
	
	// 2.
	m_InstanceDeallocator.RegisterInstance(new MkSharedPointerManager());

	// 3. root directory�� file system root ����
	m_InstanceDeallocator.RegisterInstance(new MkFileManager(L""));

	// 4.
	m_InstanceDeallocator.RegisterInstance(new MkTimeManager());

	// 5.
	m_InstanceDeallocator.RegisterInstance(new MkProfilingManager(notShipping));

	// 6.
	m_InstanceDeallocator.RegisterInstance(new MkInputManager());

	// 7.
	m_InstanceDeallocator.RegisterInstance(new MkKeywordFilter());

	// 8.
	m_InstanceDeallocator.RegisterInstance(new MkDevPanel(notShipping));

	// 9.
	m_InstanceDeallocator.RegisterInstance(new MkBackgroundLoader());

	// 10.
	m_InstanceDeallocator.RegisterInstance(new MkPageManager());

	// start!!!
	MK_DEV_PANEL.MsgToLog(L"Application start" + MkStr::CRLF, true);

	// main window
	MK_CHECK(gMainWindowPtr == NULL, L"main window�� �̹� ����")
		return false;

	// ���� ������ ����
	if (!m_MainWindow.SetUpByWindowCreation(hInstance, (wndProc == NULL) ? WndProc : wndProc, NULL, title, sysWinProp, MkInt2(x, y), MkInt2(clientWidth, clientHeight), fullScreen))
		return false;

	gMainWindowPtr = &m_MainWindow;

	// ��Ŀ�� ���
	gMainWindowTitleBarHooker.SetUp(&m_MainWindow, true);

	// �̱��濡 ������ ���
	MK_INPUT_MGR.SetUp(m_MainWindow.GetWindowHandle());
	if (MK_DEV_PANEL.SetUp(m_MainWindow, eRAP_RMostTop, CreateCheatMessage()))
	{
		m_MainWindow.SetOnTop();

		MK_SYS_ENV.__PrintSystemInformationToLog();
		MkFileManager::Instance().__PrintSystemInformationToLog();
	}

	// Ȯ�� �ʱ�ȭ. app�� update�� �ʿ�� ���� ���� ��� ���⼭ false�� �޾� ����
	if (!SetUp(clientWidth, clientHeight, fullScreen, cmdLine))
		return false;

	// logic thread procedure
	MkBaseThreadUnit* logicThreadUnit = CreateLogicThreadUnit(MKDEF_LOGIC_THREAD_UNIT_NAME);
	m_UseLogicThreadUnit = (logicThreadUnit != NULL);
	if (m_UseLogicThreadUnit)
	{
		MK_CHECK(m_ThreadManager.RegisterThreadUnit(logicThreadUnit), L"logic thread unit ��� ����")
			return false;
	}
	
	// loading thread unit
	MkBaseThreadUnit* loadingThreadUnit = CreateLoadingThreadUnit(MKDEF_LOADING_THREAD_UNIT_NAME);
	MK_CHECK(m_ThreadManager.RegisterThreadUnit(loadingThreadUnit), L"loading thread unit ��� ����")
		return false;

	// �ð� ������ �ʱ�ȭ
	MK_TIME_MGR.SetUp();

	// �巡��&��� ���
	if (dragAccept)
	{
		DragAcceptFiles(m_MainWindow.GetWindowHandle(), TRUE);

		gDraggedFilePathList.Reserve(32);

#if MKDEF_VISTA_OR_HIGHER_ONLY
#if (WINVER >= 0x0600)
		// UAC ���ÿ��� ���� ��� ��� �����ϵ��� ����
		// http://www.tipssoft.com/bulletin/board.php?bo_table=FAQ&wr_id=1571
		ChangeWindowMessageFilter(0x0049, 0x0001); // WM_COPYGLOBALDATA, MSGFLT_ADD
		ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
#endif
#endif
		MK_DEV_PANEL.MsgToLog(L"Drag accept files", false);
	}

	MK_DEV_PANEL.MsgToLog(L"MkBaseFramework �ʱ�ȭ ����", false);
	return true;
}

void MkBaseFramework::__Run(void)
{
	// ��ϵ� ��� ������ ���� ����
	m_ThreadManager.WakeUpAll();

	// SetUp�� �Ϸ�� �� ���� ���
	// �����ϸ� ����
	if (!m_ThreadManager.WaitTillAllThreadsAreRunning())
		return;

	// ���� �˶�
	StartLooping();

	MK_DEV_PANEL.MsgToLog(L"MkBaseFramework ���� ����", false);
	MK_DEV_PANEL.MsgToLog(L"----------------------------------------------------------", false);

	// ���� ����
	const MkHashStr profKey = MkStr(MKDEF_PROFILING_PREFIX_FOR_THREAD) + MkStr(MKDEF_MAIN_THREAD_NAME);
	HWND hWnd = m_MainWindow.GetWindowHandle();

	MkTimeState lastTimeState;
	lastTimeState.frameCount = 0xffffffff;

	while (true)
	{
		if (gDestroyMsg)
		{
			// background loader ����(���� loading thread���� ���� ���� �� ����)
			MK_BG_LOADER.__Clear();

			if (!m_UseLogicThreadUnit)
			{
				// ��� ������ ����
				MK_PAGE_MGR.__Clear();
			}

			// ������ ���� ��� ����
			m_ThreadManager.StopAll();

			// window ���� �� ����(renderer�� �����ϸ� ���⼭ ����)
			Free();

			// window ����
			DestroyWindow(hWnd);
		}

		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			if (!TranslateAccelerator(hWnd, NULL, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			// logic thread unit ���
			if (m_UseLogicThreadUnit)
			{
				MkTimeState currTimeState;
				MK_TIME_MGR.GetCurrentTimeState(currTimeState);

				// ���ο� �������̸� ����
				if (currTimeState.frameCount != lastTimeState.frameCount)
				{
					// ���� ������ �������ϸ� ����
					MK_PROF_MGR.Begin(profKey);

					// �Է� ����
					MK_INPUT_MGR.__Update();

					// page ������ logic thread���� ó��

					// cursor ó��
					if (gRecieveCursorMsg)
					{
						ConsumeSetCursorMsg();
						gRecieveCursorMsg = false;
					}

					// Ȯ�� ����
					Update();

					// �巡�� ���� ó��
					ConsumeDraggedFiles(gDraggedFilePathList);

					// ���� ������ �������ϸ� ����
					MK_PROF_MGR.End(profKey);

					lastTimeState = currTimeState;
				}
				else
				{
					Sleep(1); // ���� �ð��̸� ���
				}
			}
			// single thread
			else
			{
				// ������ ����
				MK_TIME_MGR.Update();

				// ���� ������ �������ϸ� ����
				MK_PROF_MGR.Begin(profKey);

				// �Է� ����
				MK_INPUT_MGR.__Update();

				// cursor ó��
				if (gRecieveCursorMsg)
				{
					ConsumeSetCursorMsg();
					gRecieveCursorMsg = false;
				}

				// page ����
				MK_PAGE_MGR.__Update();

				// Ȯ�� ����
				Update();

				// �巡�� ���� ó��
				ConsumeDraggedFiles(gDraggedFilePathList);

				// ���� ������ �������ϸ� ����
				MK_PROF_MGR.End(profKey);
			}

			// dev panel ��ġ ����
			if (MK_INPUT_MGR.GetKeyPushing(VK_CONTROL) && MK_INPUT_MGR.GetKeyPushing(VK_SHIFT))
			{
				if (MK_INPUT_MGR.GetKeyPressed(VK_LEFT))
				{
					MK_DEV_PANEL.__SetAlignmentPosition(eRAP_LMostTop);
				}
				else if (MK_INPUT_MGR.GetKeyPressed(VK_RIGHT))
				{
					MK_DEV_PANEL.__SetAlignmentPosition(eRAP_RMostTop);
				}
				else if (MK_INPUT_MGR.GetKeyPressed(VK_UP))
				{
					MK_DEV_PANEL.__SetAlignmentPosition(eRAP_RightTop);
				}
				else if (MK_INPUT_MGR.GetKeyPressed(VK_DOWN))
				{
					MK_DEV_PANEL.__SetAlignmentPosition(eRAP_LeftBottom);
				}
			}

			// dev panel ����
			MK_DEV_PANEL.__Update();

			// ��Ŀ ����
			gMainWindowTitleBarHooker.Update();
		}
	}
}

void MkBaseFramework::__End(void)
{
	// ������ ��� ����
	m_MainWindow.Clear();

	// window ���� �� ����
	Clear();

	// �� ���� ����
	m_FPUSetter.Restore();
	m_ASKSetter.Restore();

	// end msg
	MK_LOG_MGR.Msg(L"MkBaseFramework end. Bye~", true);

	// �̱��� ����
	m_InstanceDeallocator.ClearRearToFront();
}

LRESULT CALLBACK MkBaseFramework::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MK_INPUT_MGR.__CheckWndProc(hWnd, msg, wParam, lParam);

	if (gMainWindowTitleBarHooker.CheckWndProc(hWnd, msg, wParam, lParam))
		return 0;

	switch (msg)
	{
	case WM_SETCURSOR:
		{
			if (LOWORD(lParam) == HTCLIENT)
			{
				gRecieveCursorMsg = true;
				return TRUE;
			}
		}
		break;

	case WM_DROPFILES:
		{
			HDROP hDrop = reinterpret_cast<HDROP>(wParam);
			wchar_t pathBuf[MAX_PATH];
			int fileCount = DragQueryFile(hDrop, 0xffffffff, pathBuf, MAX_PATH);

			for (int i=0; i<fileCount; ++i)
			{
				ZeroMemory(pathBuf, sizeof(pathBuf));
				DragQueryFile(hDrop, i, pathBuf, MAX_PATH);

				MkPathName filePath = pathBuf;
				gDraggedFilePathList.PushBack(filePath);
			}

			DragFinish(hDrop);
		}
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------------------------//

// http://jacking.tistory.com/363
void MkBaseFramework::_TurnOnLowFragmentationHeap(void)
{
	HANDLE heaps[1025] = {0, };
	DWORD heapCount = GetProcessHeaps(1024, heaps);
	for (DWORD i=0; i<heapCount; ++i)
	{
		ULONG heapFragValue = 2;
		bool success = (HeapSetInformation(heaps[i], HeapCompatibilityInformation, &heapFragValue, sizeof(heapFragValue)) == TRUE);
	}
}

//------------------------------------------------------------------------------------------------//
