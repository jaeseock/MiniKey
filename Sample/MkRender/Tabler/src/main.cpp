
//------------------------------------------------------------------------------------------------//
// single/multi-thread rendering application sample
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkPageManager.h"
//#include "MkCore_MkInputManager.h"
//#include "MkCore_MkTimeManager.h"
#include "MkCore_MkWin32Application.h"
//#include "MkCore_MkProfilingManager.h"
//#include "MkCore_MkDevPanel.h"
//#include "MkCore_MkDataNode.h"
//#include "MkCore_MkUniformDice.h"

#include "MkPA_MkRenderFramework.h"
//#include "MkPA_MkRenderer.h"
//#include "MkPA_MkWindowManagerNode.h"
//#include "MkPA_MkTitleBarControlNode.h"
//#include "MkPA_MkBodyFrameControlNode.h"
//#include "MkPA_MkSliderControlNode.h"
//#include "MkPA_MkDrawSceneNodeStep.h"
//#include "MkPA_MkWindowFactory.h"

#include "ApStaticDefinition.h"
#include "ApStageSetupPage.h"
#include "ApStagePlayPage.h"

// framework 선언
class MainFramework : public MkRenderFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		ApStaticDefinition::Instance().SetUp(L"StaticDefinition.txt", MkInt2(clientWidth, clientHeight));

		MK_PAGE_MGR.SetUp(new MkBasePage(L"Root"));

		MK_PAGE_MGR.RegisterChildPage(L"Root", new ApStageSetupPage(L"ApStageSetup"));
		MK_PAGE_MGR.RegisterChildPage(L"Root", new ApStagePlayPage(L"ApStagePlay"));

		MK_PAGE_MGR.ChangePageDirectly(L"ApStageSetup");

		return MkRenderFramework::SetUp(clientWidth, clientHeight, fullScreen, cmdLine);
	}

	virtual ~MainFramework() {}
};

// application 선언
class GameApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new MainFramework; }

public:
	GameApplication() : MkWin32Application() {}
	virtual ~GameApplication() {}
};

// 엔트리 포인트에서의 TestApplication 실행
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	GameApplication application;
	application.Run(hI, L"Tabler", L"..\\FileRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 480, 800);

	return 0;
}

