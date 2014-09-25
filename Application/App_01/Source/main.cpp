
//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkPageManager.h"
//#include "MkCore_MkInputManager.h"
//#include "MkCore_MkTimeState.h"
#include "MkCore_MkWin32Application.h"

#include "MkS2D_MkRenderFramework.h"

//#include "MkCore_MkUniformDice.h"

//#include "MkS2D_MkTexturePool.h"
//#include "MkS2D_MkFontManager.h"
//#include "MkS2D_MkRenderer.h"

//#include "MkS2D_MkDecoStr.h"

//#include "MkS2D_MkSceneNode.h"
//#include "MkS2D_MkDrawStep.h"
//#include "MkS2D_MkDrawQueue.h"

//#include "MkCore_MkNameSelection.h"

//#include "MkCore_MkDevPanel.h"

#include "GameGlobalDefinition.h"
#include "GamePageAppRoot.h"
#include "GamePageClientStart.h"
#include "GamePageGameRoot.h"
#include "GamePageMainTitle.h"
#include "GamePageIslandAgora.h"
#include "GamePageWizardLab.h"
#include "GamePageBarrack.h"

//------------------------------------------------------------------------------------------------//

class GameFramework : public MkRenderFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg)
	{
		//const MkHashStr GamePageName::WizardLab(L"WizardLab");
		//const MkHashStr GamePageName::Barrack(L"Barrack");
		// AppRoot
		//	- ClientStart
		//	- GameRoot
		//		- MainTitle
		//		- IslandAgora
		MK_PAGE_MGR.SetUp(new GamePageAppRoot(GamePageName::AppRoot));

		MK_PAGE_MGR.RegisterChildPage(GamePageName::AppRoot, new GamePageClientStart(GamePageName::ClientStart));
		MK_PAGE_MGR.RegisterChildPage(GamePageName::AppRoot, new GamePageGameRoot(GamePageName::GameRoot));

		MK_PAGE_MGR.RegisterChildPage(GamePageName::GameRoot, new GamePageMainTitle(GamePageName::MainTitle));
		MK_PAGE_MGR.RegisterChildPage(GamePageName::GameRoot, new GamePageIslandAgora(GamePageName::IslandAgora));
		MK_PAGE_MGR.RegisterChildPage(GamePageName::GameRoot, new GamePageWizardLab(GamePageName::WizardLab));
		MK_PAGE_MGR.RegisterChildPage(GamePageName::GameRoot, new GamePageBarrack(GamePageName::Barrack));

		// start page
		MK_PAGE_MGR.ChangePageDirectly(GamePageName::ClientStart);

		return MkRenderFramework::SetUp(clientWidth, clientHeight, fullScreen, arg);
	}

	GameFramework() : MkRenderFramework() {}
	virtual ~GameFramework() {}
};

//------------------------------------------------------------------------------------------------//

class GameApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new GameFramework; }

	GameApplication() : MkWin32Application() {}
	virtual ~GameApplication() {}
};

//------------------------------------------------------------------------------------------------//

int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	GameApplication application;
	application.Run(hI, L"App 01", L"..\\FileRoot", true, eSWP_FixedSize, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

