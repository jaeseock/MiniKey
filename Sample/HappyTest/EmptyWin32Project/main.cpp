
//------------------------------------------------------------------------------------------------//
// single/multi-thread application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkPageManager.h"
#include "MkCore_MkLogicThreadUnit.h"

#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

//------------------------------------------------------------------------------------------------//
// multi-thread의 경우 주석 처리
//------------------------------------------------------------------------------------------------//

#define SAMPLEDEF_SINGLE_THREAD

//------------------------------------------------------------------------------------------------//

// TestPage 선언(공통)
class TestPage : public MkBasePage
{
public:
	virtual bool SetUp(MkDataNode& sharingNode) { return true; }
	virtual void Update(const MkTimeState& timeState) {}
	virtual void Clear(MkDataNode* sharingNode = NULL) {}

	TestPage(const MkHashStr& name) : MkBasePage(name) {}
	virtual ~TestPage() {}
};

// single-thread에서의 TestFramework 선언
#ifdef SAMPLEDEF_SINGLE_THREAD

class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		MK_PAGE_MGR.SetUp(new TestPage(L"TestPage"));
		MK_PAGE_MGR.ChangePageDirectly(L"TestPage");

		return true;
	}

	virtual ~TestFramework() {}
};

#else

// multi-thread에서의 TestThreadUnit 선언
class TestThreadUnit : public MkLogicThreadUnit
{
public:

	virtual bool SetUp(void)
	{
		MK_PAGE_MGR.SetUp(new TestPage(L"TestPage"));
		MK_PAGE_MGR.ChangePageDirectly(L"TestPage");

		return MkLogicThreadUnit::SetUp();
	}

	TestThreadUnit(const MkHashStr& threadName) : MkLogicThreadUnit(threadName) {}
	virtual ~TestThreadUnit() {}
};

// multi-thread에서의 TestFramework 선언
class TestFramework : public MkBaseFramework
{
public:
	virtual MkBaseThreadUnit* CreateLogicThreadUnit(const MkHashStr& threadName) const { return new TestThreadUnit(threadName); }
	virtual ~TestFramework() {}
};

#endif

// TestApplication 선언(공통)
class TestApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new TestFramework; }

public:
	TestApplication() : MkWin32Application() {}
	virtual ~TestApplication() {}
};

//------------------------------------------------------------------------------------------------//

// 엔트리 포인트에서의 TestApplication 실행
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"win32 empty project", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, false, false, NULL);

	return 0;
}

//------------------------------------------------------------------------------------------------//

