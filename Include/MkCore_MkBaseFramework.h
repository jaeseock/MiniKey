#ifndef __MINIKEY_CORE_MKBASEFRAMEWORK_H__
#define __MINIKEY_CORE_MKBASEFRAMEWORK_H__

//------------------------------------------------------------------------------------------------//
// base framework
//
// 초기화, 루핑, 종료까지의 순차 관리
// 기본 싱글톤 및 객체 관리, 메인 스레드외의 스레드 유닛(logic, file loading) 소유
//
// 프로젝트 성격에 따라 logic-render를 동일 루프에서 소화하는 single-thread와 별도의 thread에서 처리되는
// multi-thread중 어느 방식으로 진행할지 여부를 선택 가능(디폴트는 single-thread)
// - CreateLogicThreadUnit()의 반환값이 NULL일 경우는 single-thread로, logic thread unit 객체일 경우 multi-thread로 인식
// - 차이점은 MK_TIME_MGR.Update()를 통한 시간 관리와 MK_PAGE_MGR.__Update()를 통한 페이지 관리
// - single-thread의 경우 모두 한 루프에서, multi-thread의 경우 logic thread unit의 루프에서 처리
//
// 프레임 관리
// - single-thread의 경우 순차적인 프레임 진행 보장
// - multi-thread인 경우(logic thread unit 존재) logic thread unit이 프레임 진행의 기준이 됨
//   다른 thread들은 logic thread의 프레임이 진행 되었을 때 갱신 실행
//   * 기준 thread의 Update()는 순차적인 프레임 진행 보장 (ex> 0, 1, 2, 3, 4, 5, ...)
//   * 타 thread의 Update()는 프레임 스킵 가능성 존재 (ex> 0, 1, 3, 5, 6, 8, ...)
//
// ex> single/multi-thread 환경에 따른 application 초기화 예시
//
//	// TestPage 선언(공통)
//	class TestPage : public MkBasePage
//	{
//		...
//	};
//
//	// single-thread에서의 TestFramework 선언
//	class TestFramework : public MkBaseFramework
//	{
//	public:
//		virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg)
//		{
//			MK_PAGE_MGR.SetUp(new TestPage(L"TestPage"));
//			MK_PAGE_MGR.ChangePageDirectly(L"TestPage");
//			return true;
//		}
//		virtual ~TestFramework() {}
//	};
//
//	// multi-thread에서의 TestThreadUnit 선언
//	class TestThreadUnit : public MkLogicThreadUnit
//	{
//	public:
//		virtual bool SetUp(void)
//		{
//			MK_PAGE_MGR.SetUp(new TestPage(L"TestPage"));
//			MK_PAGE_MGR.ChangePageDirectly(L"TestPage");
//			return MkLogicThreadUnit::SetUp();
//		}
//
//		TestThreadUnit(const MkHashStr& threadName) : MkLogicThreadUnit(threadName) {}
//		virtual ~TestThreadUnit() {}
//	};
//
//	// multi-thread에서의 TestFramework 선언
//	class TestFramework : public MkBaseFramework
//	{
//	public:
//		virtual MkBaseThreadUnit* CreateLogicThreadUnit(const MkHashStr& threadName) const { return new TestThreadUnit(threadName); }
//		virtual ~TestFramework() {}
//	};
//
//	// TestApplication 선언(공통)
//	class TestApplication : public MkWin32Application
//	{
//	public:
//		virtual MkBaseFramework* CreateFramework(void) const { return new TestFramework; }
//
//		TestApplication() : MkWin32Application() {}
//		virtual ~TestApplication() {}
//	};
//
//	// 엔트리 포인트에서의 TestApplication 실행
//	int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
//	{
//		TestApplication application;
//		application.Run(hI, L"MK Test app", L"..\\FileRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, false, false, NULL, cmdline);
//		return 0;
//	}
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkInstanceDeallocator.h"
#include "MkCore_MkAccessibilityShortcutKeySetter.h"
#include "MkCore_MkFixedFPUSetter.h"
#include "MkCore_MkThreadManager.h"
#include "MkCore_MkListeningWindow.h"
#include "MkCore_MkPathName.h"


class MkBaseThreadUnit;
class MkCheatMessage;

class MkBaseFramework
{
public:

	// logic thread unit 생성해 반환
	// 확장 thread unit을 사용하고 싶을 경우 여기서 대신 생성해 반환
	// NULL을 반환하면 single thread처럼 사용
	virtual MkBaseThreadUnit* CreateLogicThreadUnit(const MkHashStr& threadName) const;

	// loading thread unit 생성해 반환
	// 확장 thread unit을 사용하고 싶을 경우 여기서 대신 생성해 반환
	virtual MkBaseThreadUnit* CreateLoadingThreadUnit(const MkHashStr& threadName) const;

	// cheat message 생성해 반환
	virtual MkCheatMessage* CreateCheatMessage(void) const { return NULL; }

	// 확장 초기화
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg) { return true; }

	// 모든 스레드의 초기화 완료 후 루핑 시작 알람
	virtual void StartLooping(void) {}

	// 확장 갱신
	virtual void Update(void) {}

	// 드래그 파일 처리 확장 갱신
	virtual void ConsumeDraggedFiles(MkArray<MkPathName>& draggedFilePathList) { draggedFilePathList.Clear(); }

	// WM_SETCURSOR message 처리
	virtual void ConsumeSetCursorMsg(void) {}

	// 확장 해제
	virtual void Clear(void) {}

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	bool __Start
		(HINSTANCE hInstance, const wchar_t* title, const wchar_t* rootPath, bool useLog, eSystemWindowProperty sysWinProp,
		int x, int y, int clientWidth, int clientHeight, bool fullScreen, bool dragAccept, WNDPROC wndProc, const char* arg);

	void __Run(void);

	void __End(void);

	// 윈도우 프로시져
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	MkBaseFramework() {}
	virtual ~MkBaseFramework() {}

protected:

	void _TurnOnLowFragmentationHeap(void);

protected:

	MkAccessibilityShortcutKeySetter m_ASKSetter;
	MkFixedFPUSetter m_FPUSetter;

	MkInstanceDeallocator m_InstanceDeallocator;
	MkThreadManager m_ThreadManager;
	MkListeningWindow m_MainWindow;

	bool m_UseLogicThreadUnit;
};

//------------------------------------------------------------------------------------------------//

#endif
