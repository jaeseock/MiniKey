
//#include "MkCore_MkCheck.h"
//#include "MkCore_MkDataNode.h"
//#include "MkCore_MkDevPanel.h"
//#include "MkCore_MkInputManager.h"
//#include "MkCore_MkTimeState.h"
#include "MkCore_MkPageManager.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkWindowManagerNode.h"
#include "MkPA_MkWindowFactory.h"
#include "MkPA_MkWindowBaseNode.h"
//#include "MkPA_MkStaticResourceContainer.h"

//#include "MkPA_MkDrawSceneNodeStep.h"

#include "App02_ProjectDefinition.h"
#include "App02_GamePageAppBattleZone.h"

//const static MkHashStr BattleBtnName(L"Battle");

//------------------------------------------------------------------------------------------------//

class GPAppBattleZoneWinMgr : public MkWindowManagerNode
{
public:

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
	{
		if ((eventType == ePA_SNE_CursorLBtnReleased) && (path.GetSize() == 1))
		{
			//if (path[0] == BattleBtnName)
			//{
				//MK_PAGE_MGR.SetMoveMessage(GamePage::AppIntro::Condition::Next);
				//return;
			//}
		}
	
		MkWindowManagerNode::SendNodeReportTypeEvent(eventType, path, argument);
	}

	GPAppBattleZoneWinMgr(const MkHashStr& name) : MkWindowManagerNode(name) {}
	virtual ~GPAppBattleZoneWinMgr() {}
};

//------------------------------------------------------------------------------------------------//

bool GamePageAppBattleZone::SetUp(MkDataNode& sharingNode)
{
	if (!GamePageBase::SetUp(sharingNode))
		return false;

	m_RootSceneNode->SetLocalDepth(10.f);

	MkFloat2 screenSize = MkFloat2(static_cast<float>(MK_DEVICE_MGR.GetCurrentResolution().x), static_cast<float>(MK_DEVICE_MGR.GetCurrentResolution().y));

	// window factory
	MkWindowFactory winFactory;
	winFactory.SetFrameType(MkWindowThemeData::eFT_Large);
	winFactory.SetMinClientSizeForButton(MkFloat2(200.f, 50.f));

	// window mgr
	GPAppBattleZoneWinMgr* winMgrNode = new GPAppBattleZoneWinMgr(L"WinMgr");
	if (winMgrNode != NULL)
	{
		m_RootSceneNode->AttachChildNode(winMgrNode);

		/*
		// battle
		MkWindowBaseNode* battleBtn = winFactory.CreateNormalButtonNode(BattleBtnName, L"전투 시작");
		if (winMgrNode->AttachWindow(battleBtn))
		{
			winMgrNode->ActivateWindow(battleBtn->GetNodeName());
			MkFloat2 pos;
			pos.x = screenSize.x - winFactory.GetMinClientSizeForButton().x - 30.f;
			pos.y = 20.f;
			battleBtn->SetLocalPosition(pos);
		}
		*/
	}
	
	return true;
}

GamePageAppBattleZone::GamePageAppBattleZone(void) : GamePageBase(GamePage::AppBattleZone::Name)
{
	// page flow
	//SetPageFlowTable(GamePage::AppBattleZone::Condition::Next, GamePage::Intro::Name);
}

//------------------------------------------------------------------------------------------------//

