
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

#include "GamePageAppRoot.h"
#include "GamePageClientStart.h"
#include "GamePageGameRoot.h"
#include "GamePageMainTitle.h"

//------------------------------------------------------------------------------------------------//

class GameFramework : public MkRenderFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg)
	{
		// AppRoot
		//	- ClientStart
		//	- GameRoot
		//		- MainTitle
		MK_PAGE_MGR.SetUp(new GamePageAppRoot);

		MK_PAGE_MGR.RegisterChildPage(GamePageAppRoot::Name, new GamePageClientStart);
		MK_PAGE_MGR.RegisterChildPage(GamePageAppRoot::Name, new GamePageGameRoot);

		MK_PAGE_MGR.RegisterChildPage(GamePageGameRoot::Name, new GamePageMainTitle);

		MK_PAGE_MGR.SetPageFlowTable(GamePageClientStart::Name, L"ToNextPage", GamePageMainTitle::Name);

		// start page
		MK_PAGE_MGR.ChangePageDirectly(GamePageClientStart::Name);

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

