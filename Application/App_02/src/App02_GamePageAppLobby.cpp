
//#include "MkCore_MkCheck.h"
//#include "MkCore_MkDataNode.h"
//#include "MkCore_MkDevPanel.h"
//#include "MkCore_MkInputManager.h"
//#include "MkCore_MkTimeState.h"
#include "MkCore_MkPageManager.h"
#include "MkPA_MkWindowManagerNode.h"
#include "MkPA_MkWindowFactory.h"
#include "MkPA_MkWindowBaseNode.h"
//#include "MkPA_MkStaticResourceContainer.h"

//#include "MkPA_MkDrawSceneNodeStep.h"

#include "App02_ProjectDefinition.h"
#include "App02_GamePageAppLobby.h"

const static MkHashStr BattleBtnName(L"Battle");

//------------------------------------------------------------------------------------------------//

class GPAppLobbyWinMgr : public MkWindowManagerNode
{
public:

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
	{
		if ((eventType == ePA_SNE_CursorLBtnReleased) && (path.GetSize() == 1))
		{
			if (path[0] == BattleBtnName)
			{
				MK_PAGE_MGR.SetMoveMessage(GamePage::AppLobby::Condition::Battle);
				return;
			}
		}
	
		MkWindowManagerNode::SendNodeReportTypeEvent(eventType, path, argument);
	}

	GPAppLobbyWinMgr(const MkHashStr& name) : MkWindowManagerNode(name) {}
	virtual ~GPAppLobbyWinMgr() {}
};

//------------------------------------------------------------------------------------------------//

bool GamePageAppLobby::SetUp(MkDataNode& sharingNode)
{
	if (!GamePageBase::SetUp(sharingNode))
		return false;

	m_RootSceneNode->SetLocalDepth(10.f);

	// window factory
	MkWindowFactory winFactory;
	winFactory.SetFrameType(MkWindowThemeData::eFT_Large);
	winFactory.SetMinClientSizeForButton(MkFloat2(200.f, 50.f));

	// window mgr
	GPAppLobbyWinMgr* winMgrNode = new GPAppLobbyWinMgr(L"WinMgr");
	if (winMgrNode != NULL)
	{
		m_RootSceneNode->AttachChildNode(winMgrNode);

		// battle
		MkWindowBaseNode* battleBtn = winFactory.CreateNormalButtonNode(BattleBtnName, L"ÀüÅõ »ùÇÃ");
		if (winMgrNode->AttachWindow(battleBtn))
		{
			winMgrNode->ActivateWindow(battleBtn->GetNodeName());
			battleBtn->SetLocalPosition(MkFloat2(600.f, 300.f));
		}
	}
	
	return true;
}

GamePageAppLobby::GamePageAppLobby(void) : GamePageBase(GamePage::AppLobby::Name)
{
	// page flow
	SetPageFlowTable(GamePage::AppLobby::Condition::Battle, GamePage::AppBriefingRoom::Name);
}

//------------------------------------------------------------------------------------------------//

