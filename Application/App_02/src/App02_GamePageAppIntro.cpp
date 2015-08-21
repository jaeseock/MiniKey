
//#include "MkCore_MkCheck.h"
//#include "MkCore_MkDataNode.h"
//#include "MkCore_MkDevPanel.h"
//#include "MkCore_MkInputManager.h"
//
#include "MkCore_MkVersionTag.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkPageManager.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkTextNode.h"
#include "MkPA_MkWindowManagerNode.h"
#include "MkPA_MkWindowFactory.h"
#include "MkPA_MkWindowBaseNode.h"
#include "MkPA_MkWindowTagNode.h"
//#include "MkPA_MkStaticResourceContainer.h"

//#include "MkPA_MkDrawSceneNodeStep.h"

#include "App02_ProjectDefinition.h"
#include "App02_GamePageAppIntro.h"

const static MkHashStr NewGameBtnName(L"NewGame");
const static MkHashStr LoadBtnName(L"Load");

//------------------------------------------------------------------------------------------------//

class GPAppIntroWinMgr : public MkWindowManagerNode
{
public:

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
	{
		if ((eventType == ePA_SNE_CursorLBtnReleased) && (path.GetSize() == 1))
		{
			if (path[0] == NewGameBtnName)
			{
				MK_PAGE_MGR.SetMoveMessage(GamePage::AppIntro::Condition::Next);
				return;
			}
			else if (path[0] == LoadBtnName)
			{
				MkPathName loadFilePath;
				if (loadFilePath.GetSingleFilePathFromDialog(L"txt", MK_DEVICE_MGR.GetTargetWindow()->GetWindowHandle()))
				{
					
				}
				return;
			}
			else if (path[0] == MkWindowFactory::CancelButtonName)
			{
				MkBaseFramework::Close();
				return;
			}
		}
	
		MkWindowManagerNode::SendNodeReportTypeEvent(eventType, path, argument);
	}

	GPAppIntroWinMgr(const MkHashStr& name) : MkWindowManagerNode(name) {}
	virtual ~GPAppIntroWinMgr() {}
};

//------------------------------------------------------------------------------------------------//

bool GamePageAppIntro::SetUp(MkDataNode& sharingNode)
{
	if (!GamePageBase::SetUp(sharingNode))
		return false;

	m_RootSceneNode->SetLocalDepth(10.f);

	float screenWidth = static_cast<float>(MK_DEVICE_MGR.GetCurrentResolution().x);

	// window factory
	MkWindowFactory winFactory;
	winFactory.SetFrameType(MkWindowThemeData::eFT_Large);
	winFactory.SetMinClientSizeForButton(MkFloat2(200.f, 50.f));

	// title
	MkWindowTagNode* titleNode = MkWindowTagNode::CreateChildNode(m_RootSceneNode, L"Title");
	if (titleNode != NULL)
	{
		MkArray<MkHashStr> textName;
		textName.PushBack(L"AppIntroTitle");
		titleNode->SetTextName(textName);
		titleNode->Update(); // text panel 생성

		float width = static_cast<float>(titleNode->GetTagTextPtr()->GetWholePixelSize().x);
		titleNode->SetLocalPosition(MkFloat2((screenWidth - width) * 0.5f, 550.f));
	}

	// version
	MkVersionTag versionTag(ADEF_CORE_MAJOR_VERSION, ADEF_CORE_MINOR_VERSION, ADEF_CORE_BUILD_VERSION, ADEF_CORE_REVISION_VERSION);
	MkFloat2 versionSize;
	MkWindowTagNode* versionNode = winFactory.CreateTextTagNode(L"Version", L"Ver " + versionTag.ToString(), versionSize);
	if (versionNode != NULL)
	{
		m_RootSceneNode->AttachChildNode(versionNode);
		versionNode->SetLocalPosition(MkFloat2(screenWidth - versionSize.x - 10.f, 10.f));
	}

	// window mgr
	GPAppIntroWinMgr* winMgrNode = new GPAppIntroWinMgr(L"WinMgr");
	if (winMgrNode != NULL)
	{
		m_RootSceneNode->AttachChildNode(winMgrNode);

		// new game
		MkWindowBaseNode* newGameBtn = winFactory.CreateNormalButtonNode(NewGameBtnName, L"새 게임 시작");
		if (winMgrNode->AttachWindow(newGameBtn))
		{
			winMgrNode->ActivateWindow(newGameBtn->GetNodeName());
			newGameBtn->SetLocalPosition(MkFloat2((screenWidth - winFactory.GetMinClientSizeForButton().x) * 0.5f, 300.f));
		}

		// load
		MkWindowBaseNode* loadBtn = winFactory.CreateNormalButtonNode(LoadBtnName, L"이어하기");
		if (winMgrNode->AttachWindow(loadBtn))
		{
			winMgrNode->ActivateWindow(loadBtn->GetNodeName());
			loadBtn->SetLocalPosition(MkFloat2((screenWidth - winFactory.GetMinClientSizeForButton().x) * 0.5f, 200.f));
		}

		// quit
		MkWindowBaseNode* quitBtn = winFactory.CreateCancelButtonNode(L"종료");
		if (winMgrNode->AttachWindow(quitBtn))
		{
			winMgrNode->ActivateWindow(quitBtn->GetNodeName());
			quitBtn->SetLocalPosition(MkFloat2((screenWidth - winFactory.GetMinClientSizeForButton().x) * 0.5f, 100.f));
		}
	}
	
	return true;
}

GamePageAppIntro::GamePageAppIntro(void) : GamePageBase(GamePage::AppIntro::Name)
{
	// page flow
	SetPageFlowTable(GamePage::AppIntro::Condition::Next, GamePage::AppLobby::Name);
}

//------------------------------------------------------------------------------------------------//

