
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
#include "GamePageIslandRoot.h"
#include "GamePageMainTitle.h"
#include "GamePageIslandAgora.h"
#include "GamePageWizardLab.h"
#include "GamePageBarrack.h"
#include "GamePageTrainRoom.h"
#include "GamePageSupplyDepot.h"
#include "GamePageBattleRoot.h"
#include "GamePageScout.h"

#include "GameSystemManager.h"

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
		//		- IslandRoot
		//			- IslandAgora
		//			- WizardLab
		//			- Barrack
		//			- TrainRoom
		//			- SupplyDepot
		//		- BattleRoot
		//			- Scout
		MK_PAGE_MGR.SetUp(new GamePageAppRoot(GamePageName::AppRoot));

		MK_PAGE_MGR.RegisterChildPage(GamePageName::AppRoot, new GamePageClientStart(GamePageName::ClientStart));
		MK_PAGE_MGR.RegisterChildPage(GamePageName::AppRoot, new GamePageGameRoot(GamePageName::GameRoot));

		MK_PAGE_MGR.RegisterChildPage(GamePageName::GameRoot, new GamePageMainTitle(GamePageName::MainTitle));
		MK_PAGE_MGR.RegisterChildPage(GamePageName::GameRoot, new GamePageIslandRoot(GamePageName::IslandRoot));
		MK_PAGE_MGR.RegisterChildPage(GamePageName::IslandRoot, new GamePageIslandAgora(GamePageName::IslandAgora));
		MK_PAGE_MGR.RegisterChildPage(GamePageName::IslandRoot, new GamePageWizardLab(GamePageName::WizardLab));
		MK_PAGE_MGR.RegisterChildPage(GamePageName::IslandRoot, new GamePageBarrack(GamePageName::Barrack));
		MK_PAGE_MGR.RegisterChildPage(GamePageName::IslandRoot, new GamePageTrainRoom(GamePageName::TrainRoom));
		MK_PAGE_MGR.RegisterChildPage(GamePageName::IslandRoot, new GamePageSupplyDepot(GamePageName::SupplyDepot));
		MK_PAGE_MGR.RegisterChildPage(GamePageName::GameRoot, new GamePageBattleRoot(GamePageName::BattleRoot));
		MK_PAGE_MGR.RegisterChildPage(GamePageName::BattleRoot, new GamePageScout(GamePageName::Scout));
		

		// start page
		MK_PAGE_MGR.ChangePageDirectly(GamePageName::ClientStart);

		// singleton
		m_InstanceDeallocator.Expand(1);
		m_InstanceDeallocator.RegisterInstance(new GameSystemManager());

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

