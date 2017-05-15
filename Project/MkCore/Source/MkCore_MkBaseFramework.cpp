
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

// 선 정의된 스레드 이름
#define MKDEF_MAIN_THREAD_NAME L"Main"
#define MKDEF_LOGIC_THREAD_UNIT_NAME L"Logic"
#define MKDEF_LOADING_THREAD_UNIT_NAME L"BG Loading"

// 프레임워크의 WndProc는 메인 스레드의 메인 윈도우 전용이므로 글로벌 영역의 공유를 걱정하지 않아도 됨

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

	// 고정키, 필터키 사용 정지
	m_ASKSetter.TurnOffStickyAndFilterKeys();

	// 실수 정밀도 고정
	m_FPUSetter.FixPrecision();

	// 문자열 설정
	MkStr::SetUp();
	MkPathName::SetUp(rootPath);

	// 싱글톤 생성
	m_InstanceDeallocator.Reserve(64);

	// 0.
	m_InstanceDeallocator.RegisterInstance(new MkSystemEnvironment());
	MkSystemEnvironment::eBuildMode buildMode = MK_SYS_ENV.GetBuildMode();
	bool notShipping = (buildMode == MkSystemEnvironment::eDebug) || (buildMode == MkSystemEnvironment::eRelease);

	// 디버그빌드가 아니고 멀티코어이면 LFH 사용
	if ((buildMode != MkSystemEnvironment::eDebug) && (MK_SYS_ENV.GetNumberOfProcessors() > 1))
	{
		_TurnOnLowFragmentationHeap();
	}

	// 1.
	m_InstanceDeallocator.RegisterInstance(new MkLogManager(useLog));
	
	// 2.
	m_InstanceDeallocator.RegisterInstance(new MkSharedPointerManager());

	// 3. root directory로 file system root 설정
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
	MK_CHECK(gMainWindowPtr == NULL, L"main window가 이미 존재")
		return false;

	// 메인 윈도우 생성
	if (!m_MainWindow.SetUpByWindowCreation(hInstance, (wndProc == NULL) ? WndProc : wndProc, NULL, title, sysWinProp, MkInt2(x, y), MkInt2(clientWidth, clientHeight), fullScreen))
		return false;

	gMainWindowPtr = &m_MainWindow;

	// 후커에 등록
	gMainWindowTitleBarHooker.SetUp(&m_MainWindow, true);

	// 싱글톤에 윈도우 등록
	MK_INPUT_MGR.SetUp(m_MainWindow.GetWindowHandle());
	if (MK_DEV_PANEL.SetUp(m_MainWindow, eRAP_RMostTop, CreateCheatMessage()))
	{
		m_MainWindow.SetOnTop();

		MK_SYS_ENV.__PrintSystemInformationToLog();
		MkFileManager::Instance().__PrintSystemInformationToLog();
	}

	// 확장 초기화. app이 update를 필요로 하지 않을 경우 여기서 false를 받아 종료
	if (!SetUp(clientWidth, clientHeight, fullScreen, cmdLine))
		return false;

	// logic thread procedure
	MkBaseThreadUnit* logicThreadUnit = CreateLogicThreadUnit(MKDEF_LOGIC_THREAD_UNIT_NAME);
	m_UseLogicThreadUnit = (logicThreadUnit != NULL);
	if (m_UseLogicThreadUnit)
	{
		MK_CHECK(m_ThreadManager.RegisterThreadUnit(logicThreadUnit), L"logic thread unit 등록 실패")
			return false;
	}
	
	// loading thread unit
	MkBaseThreadUnit* loadingThreadUnit = CreateLoadingThreadUnit(MKDEF_LOADING_THREAD_UNIT_NAME);
	MK_CHECK(m_ThreadManager.RegisterThreadUnit(loadingThreadUnit), L"loading thread unit 등록 실패")
		return false;

	// 시간 관리자 초기화
	MK_TIME_MGR.SetUp();

	// 드래그&드롭 허용
	if (dragAccept)
	{
		DragAcceptFiles(m_MainWindow.GetWindowHandle(), TRUE);

		gDraggedFilePathList.Reserve(32);

#if MKDEF_VISTA_OR_HIGHER_ONLY
#if (WINVER >= 0x0600)
		// UAC 사용시에도 파일 드롭 사용 가능하도록 설정
		// http://www.tipssoft.com/bulletin/board.php?bo_table=FAQ&wr_id=1571
		ChangeWindowMessageFilter(0x0049, 0x0001); // WM_COPYGLOBALDATA, MSGFLT_ADD
		ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
#endif
#endif
		MK_DEV_PANEL.MsgToLog(L"Drag accept files", false);
	}

	MK_DEV_PANEL.MsgToLog(L"MkBaseFramework 초기화 성공", false);
	return true;
}

void MkBaseFramework::__Run(void)
{
	// 등록된 모든 스레드 유닛 시작
	m_ThreadManager.WakeUpAll();

	// SetUp이 완료될 때 까지 대기
	// 실패하면 종료
	if (!m_ThreadManager.WaitTillAllThreadsAreRunning())
		return;

	// 루핑 알람
	StartLooping();

	MK_DEV_PANEL.MsgToLog(L"MkBaseFramework 루핑 진입", false);
	MK_DEV_PANEL.MsgToLog(L"----------------------------------------------------------", false);

	// 루핑 돌입
	const MkHashStr profKey = MkStr(MKDEF_PROFILING_PREFIX_FOR_THREAD) + MkStr(MKDEF_MAIN_THREAD_NAME);
	HWND hWnd = m_MainWindow.GetWindowHandle();

	MkTimeState lastTimeState;
	lastTimeState.frameCount = 0xffffffff;

	while (true)
	{
		if (gDestroyMsg)
		{
			// background loader 삭제(아직 loading thread에는 남아 있을 수 있음)
			MK_BG_LOADER.__Clear();

			if (!m_UseLogicThreadUnit)
			{
				// 모든 페이지 삭제
				MK_PAGE_MGR.__Clear();
			}

			// 스레드 유닛 모두 종료
			m_ThreadManager.StopAll();

			// window 종료 전 해제(renderer가 존재하면 여기서 해제)
			Free();

			// window 종료
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
			// logic thread unit 사용
			if (m_UseLogicThreadUnit)
			{
				MkTimeState currTimeState;
				MK_TIME_MGR.GetCurrentTimeState(currTimeState);

				// 새로운 프레임이면 수행
				if (currTimeState.frameCount != lastTimeState.frameCount)
				{
					// 현재 프레임 프로파일링 시작
					MK_PROF_MGR.Begin(profKey);

					// 입력 갱신
					MK_INPUT_MGR.__Update();

					// page 갱신은 logic thread에서 처리

					// cursor 처리
					if (gRecieveCursorMsg)
					{
						ConsumeSetCursorMsg();
						gRecieveCursorMsg = false;
					}

					// 확장 갱신
					Update();

					// 드래그 파일 처리
					ConsumeDraggedFiles(gDraggedFilePathList);

					// 현재 프레임 프로파일링 종료
					MK_PROF_MGR.End(profKey);

					lastTimeState = currTimeState;
				}
				else
				{
					Sleep(1); // 동일 시간이면 대기
				}
			}
			// single thread
			else
			{
				// 프레임 시작
				MK_TIME_MGR.Update();

				// 현재 프레임 프로파일링 시작
				MK_PROF_MGR.Begin(profKey);

				// 입력 갱신
				MK_INPUT_MGR.__Update();

				// cursor 처리
				if (gRecieveCursorMsg)
				{
					ConsumeSetCursorMsg();
					gRecieveCursorMsg = false;
				}

				// page 갱신
				MK_PAGE_MGR.__Update();

				// 확장 갱신
				Update();

				// 드래그 파일 처리
				ConsumeDraggedFiles(gDraggedFilePathList);

				// 현재 프레임 프로파일링 종료
				MK_PROF_MGR.End(profKey);
			}

			// dev panel 위치 조정
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

			// dev panel 갱신
			MK_DEV_PANEL.__Update();

			// 후커 갱신
			gMainWindowTitleBarHooker.Update();
		}
	}
}

void MkBaseFramework::__End(void)
{
	// 윈도우 등록 해제
	m_MainWindow.Clear();

	// window 종료 후 해제
	Clear();

	// 원 설정 복원
	m_FPUSetter.Restore();
	m_ASKSetter.Restore();

	// end msg
	MK_LOG_MGR.Msg(L"MkBaseFramework end. Bye~", true);

	// 싱글톤 종료
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
