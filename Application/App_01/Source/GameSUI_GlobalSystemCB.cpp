
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkPageManager.h"

#include "GameGlobalDefinition.h"
#include "GameSystemManager.h"
#include "GameSUI_GlobalSystemCB.h"


//------------------------------------------------------------------------------------------------//

void GameSUI_GlobalSystemCB::UseWindowEvent(WindowEvent& evt)
{
	if (evt.type == MkSceneNodeFamilyDefinition::eSetTargetItem)
	{
		const static MkHashStr Save(L"Save");
		const static MkHashStr ToTitle(L"ToTitle");

		const MkHashStr& targetKey = GetTargetItemKey();
		if (targetKey == Save)
		{
			MkPathName filePath;
			if (filePath.GetSaveFilePathFromDialog(GDEF_USER_SAVE_DATA_EXT))
			{
				MK_CHECK(GAME_SYSTEM.SaveMasterUserData(filePath), L"저장 파일 기록 오류") {}
			}
		}
		else if (targetKey == ToTitle)
		{
			MK_PAGE_MGR.ChangePageDirectly(GamePageName::MainTitle);
		}

		m_TargetUniqueKey.Clear();
	}
}

GameSUI_GlobalSystemCB::GameSUI_GlobalSystemCB(void) : MkSpreadButtonNode(L"__#SystemCB")
{
	MkDataNode node;
	if (node.Load(L"Scene\\UI\\GlobalMenuSystemCB.msd"))
	{
		Load(node);
	}
}

//------------------------------------------------------------------------------------------------//
