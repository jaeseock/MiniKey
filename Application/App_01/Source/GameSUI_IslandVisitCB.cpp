
#include "MkCore_MkDataNode.h"
#include "MkCore_MkPageManager.h"

#include "GameGlobalDefinition.h"
#include "GameSUI_IslandVisitCB.h"


//------------------------------------------------------------------------------------------------//

void GameSUI_IslandVisitCB::UseWindowEvent(WindowEvent& evt)
{
	if (evt.type == MkSceneNodeFamilyDefinition::eSetTargetItem)
	{
		const static MkHashStr Agora(L"Agora");
		const static MkHashStr WizardOffice(L"WizardOffice");
		const static MkHashStr Troop(L"Troop");

		const MkHashStr& targetKey = GetTargetItemKey();
		if (targetKey == Agora)
		{
			MK_PAGE_MGR.ChangePageDirectly(GamePageName::IslandAgora);
		}
		else if (targetKey == WizardOffice)
		{
			MK_PAGE_MGR.ChangePageDirectly(GamePageName::WizardLab);
		}
		else if (targetKey == Troop)
		{
			MK_PAGE_MGR.ChangePageDirectly(GamePageName::Barrack);
		}

		m_TargetUniqueKey.Clear();
	}
}

GameSUI_IslandVisitCB::GameSUI_IslandVisitCB(void) : MkSpreadButtonNode(L"__#VisitCB")
{
	MkDataNode node;
	if (node.Load(L"Scene\\UI\\IslandMenuVisitCB.msd"))
	{
		Load(node);
	}
}

//------------------------------------------------------------------------------------------------//
