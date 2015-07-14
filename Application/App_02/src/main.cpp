
//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkPageManager.h"
#include "MkCore_MkWin32Application.h"
#include "MkPA_MkRenderFramework.h"

#include "App02_ProjectDefinition.h"

#include "App02_GamePageRoot.h"
#include "App02_GamePageAppStart.h"
#include "App02_GamePageAppIntro.h"

//------------------------------------------------------------------------------------------------//
// framework
//------------------------------------------------------------------------------------------------//

class AppFramework : public MkRenderFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg)
	{
		MK_PAGE_MGR.SetUp(new GamePageRoot());
		MK_PAGE_MGR.RegisterChildPage(GamePage::Root::Name, new GamePageAppStart());
		MK_PAGE_MGR.RegisterChildPage(GamePage::Root::Name, new GamePageAppIntro());
		
		MK_PAGE_MGR.ChangePageDirectly(GamePage::AppStart::Name);
		
		return MkRenderFramework::SetUp(clientWidth, clientHeight, fullScreen, arg);
	}

	virtual ~AppFramework() {}
};

//------------------------------------------------------------------------------------------------//
// application
//------------------------------------------------------------------------------------------------//

class ThisApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new AppFramework; }

public:
	ThisApplication() : MkWin32Application() {}
	virtual ~ThisApplication() {}
};

//------------------------------------------------------------------------------------------------//
// entry point
//------------------------------------------------------------------------------------------------//

int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	ThisApplication application;
	application.Run(hI, L"App 02", L"..\\FileRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT,
		static_cast<int>(GlobalDef::AppResolution.x), static_cast<int>(GlobalDef::AppResolution.y), false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

