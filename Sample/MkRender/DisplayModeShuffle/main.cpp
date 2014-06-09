
//------------------------------------------------------------------------------------------------//
// shuffle display mode
// multi-thread
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkTimeCounter.h"
#include "MkCore_MkTimeManager.h"
#include "MkCore_MkColor.h"
#include "MkCore_MkUniformDice.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkInputManager.h"
#include "MkRender_MkDisplayManager.h"
#include "MkRender_MkRenderer.h"

#include "MkCore_MkPageManager.h"
#include "MkCore_MkLogicThreadUnit.h"
#include "MkCore_MkWin32Application.h"
#include "MkRender_MkRenderFramework.h"

//------------------------------------------------------------------------------------------------//

#define SAMPLEDEF_COUNTER_TIME 5.f

class TestPage : public MkBasePage
{
public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		MkTimeState timeState;
		MK_TIME_MGR.GetCurrentTimeState(timeState);
		m_Counter.SetUp(timeState, SAMPLEDEF_COUNTER_TIME);

		MK_DISPLAY_MGR.GetAvailableResolutionList(m_ResolutionList);
		m_ResolutionList.PushBack(MkInt2(0, 0));
		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		// ESC 키를 누르면 종료
		if (MK_INPUT_MGR.GetKeyPressed(VK_ESCAPE))
		{
			MK_DEV_PANEL.__QuitApplication();
		}
		else
		{
			if (m_Counter.OnTick(timeState))
			{
				// random으로 백퍼버 컬러 변경
				MkUniformDice cDice(0, 255);
				float r = static_cast<float>(cDice.GenerateRandomNumber()) / 255.f;
				float g = static_cast<float>(cDice.GenerateRandomNumber()) / 255.f;
				float b = static_cast<float>(cDice.GenerateRandomNumber()) / 255.f;
				MK_RENDERER.SetDefaultBackbufferColor(MkColor(r, g, b));

				// random으로 해상도와 풀스크린 여부를 변경
				MkUniformDice rDice(0, m_ResolutionList.GetSize() - 1);
				unsigned int index = rDice.GenerateRandomNumber();
				bool fullScreen = ((rDice.GenerateRandomNumber() % 2) == 0);
				MK_RENDERER.ChangeDisplayMode(m_ResolutionList[index], fullScreen);
			}

			MK_DEV_PANEL.MsgToFreeboard(1, L"Time to change : " + MkStr(SAMPLEDEF_COUNTER_TIME - m_Counter.GetTickTime(timeState)));
		}
	}

	TestPage(const MkHashStr& name) : MkBasePage(name) {}
	virtual ~TestPage() {}

protected:

	MkTimeCounter m_Counter;

	MkArray<MkInt2> m_ResolutionList;
};

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

class TestFramework : public MkRenderFramework
{
public:
	virtual MkBaseThreadUnit* CreateLogicThreadUnit(const MkHashStr& threadName) const { return new TestThreadUnit(threadName); }
	virtual ~TestFramework() {}
};

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
	application.Run(hI, L"Display mode shuffle", L"..\\FileRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

