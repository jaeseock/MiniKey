
#include "MkCore_MkCheck.h"
#include "MkCore_MkPageManager.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkInputManager.h"

#include "MkS2D_MkDrawStep.h"
#include "MkS2D_MkRenderer.h"
#include "MkS2D_MkBaseWindowNode.h"
#include "MkS2D_MkWindowEventManager.h"


#include "MkS2D_MkWindowOpHelper.h"

#include "GameGlobalDefinition.h"
#include "GameSystemManager.h"
#include "GamePageMainTitle.h"

//------------------------------------------------------------------------------------------------//

class GP_MainTitleWindow : public MkBaseWindowNode
{
protected:

	MkHashStr m_NewBtnName;
	MkHashStr m_LoadBtnName;
	MkHashStr m_ContinueBtnName;
	MkHashStr m_ExitBtnName;

public:

	virtual void UseWindowEvent(WindowEvent& evt)
	{
		if (evt.type == MkSceneNodeFamilyDefinition::eCursorLeftRelease)
		{
			if (evt.node->GetNodeName() == m_NewBtnName)
			{
				MK_CHECK(GAME_SYSTEM.LoadMasterUserData(GDEF_DEF_USER_DATA_PATH), L"기본 유저 설정파일 로딩 오류")
					return;

				MK_PAGE_MGR.ChangePageDirectly(GamePageName::IslandAgora);
			}
			else if (evt.node->GetNodeName() == m_LoadBtnName)
			{
				MkPathName filePath;
				if (filePath.GetSingleFilePathFromDialog(GDEF_USER_SAVE_DATA_EXT))
				{
					MK_CHECK(GAME_SYSTEM.LoadMasterUserData(filePath), L"유저 설정파일 로딩 오류")
						return;

					MK_PAGE_MGR.ChangePageDirectly(GamePageName::IslandAgora);
				}
			}
			else if (evt.node->GetNodeName() == m_ContinueBtnName)
			{
				if (MkFileManager::CheckAvailable(GDEF_LAST_SAVE_DATA_PATH))
				{
					MK_CHECK(GAME_SYSTEM.LoadMasterUserData(GDEF_LAST_SAVE_DATA_PATH), L"마지막 저장 파일 로딩 오류")
						return;

					MK_PAGE_MGR.ChangePageDirectly(GamePageName::IslandAgora);
				}
			}
			else if (evt.node->GetNodeName() == m_ExitBtnName)
			{
				DestroyWindow(MK_INPUT_MGR.__GetTargetWindowHandle());
			}
		}
	}

	GP_MainTitleWindow(const MkHashStr& name) : MkBaseWindowNode(name)
	{
		const float btnWidth = 200.f;
		MkDrawStep* drawStep = MK_RENDERER.GetDrawQueue().GetStep(L"MainDS");
		float xPos = (drawStep->GetRegionRect().size.x - btnWidth) / 2.f;
		SetLocalPosition(MkFloat2(xPos, 150.f));

		const MkFloat2 btnSize(btnWidth, 40.f);

		m_NewBtnName = L"NewBtn";
		m_LoadBtnName = L"LoadBtn";
		m_ContinueBtnName = L"ContinueBtn";
		m_ExitBtnName = L"ExitBtn";
		
		MkWindowOpHelper::CreateWindowPreset(this, m_NewBtnName, MkHashStr::NullHash, eS2D_WPC_OKButton, btnSize, L"새 게임 시작", MkFloat2(0.f, 180.f));
		MkWindowOpHelper::CreateWindowPreset(this, m_LoadBtnName, MkHashStr::NullHash, eS2D_WPC_OKButton, btnSize, L"이전 게임 이어하기", MkFloat2(0.f, 120.f));
		MkBaseWindowNode* cBtn = MkWindowOpHelper::CreateWindowPreset(this, m_ContinueBtnName, MkHashStr::NullHash, eS2D_WPC_OKButton, btnSize, L"마지막 게임 이어하기", MkFloat2(0.f, 60.f));
		MkWindowOpHelper::CreateWindowPreset(this, m_ExitBtnName, MkHashStr::NullHash, eS2D_WPC_CancelButton, btnSize, L"종료", MkFloat2(0.f, 0.f));

		if (cBtn != NULL)
		{
			cBtn->SetEnable(MkFileManager::CheckAvailable(GDEF_LAST_SAVE_DATA_PATH));
		}
	}

	virtual ~GP_MainTitleWindow() {}
};

//------------------------------------------------------------------------------------------------//

bool GamePageMainTitle::SetUp(MkDataNode& sharingNode)
{
	// draw step
	MkDrawStep* drawStep = MK_RENDERER.GetDrawQueue().GetStep(L"MainDS");

	// 기본 scene 구성
	m_SceneNode = new MkSceneNode(L"Scene");

	m_SceneNode->SetLocalDepth(1000.f);
	drawStep->AddSceneNode(m_SceneNode);

	MkSRect* bgRect = m_SceneNode->CreateSRect(L"MainBG");
	bgRect->SetTexture(L"Scene\\main_bg.dds", MkHashStr::NullHash);

	MkSRect* textRect = m_SceneNode->CreateSRect(L"Title");

	MkStr msg;
	MkDecoStr::Convert(L"고딕40", L"RedFS", 0, L"제목은 미정이지만 하여간 게임 어플", msg);
	textRect->SetDecoString(msg);
	textRect->AlignRect(drawStep->GetRegionRect().size, eRAP_MiddleTop, MkFloat2(0.f, 200.f), 0.f);
	textRect->SetLocalDepth(-1.f);

	m_SceneNode->UpdateAll();

	// window
	MK_WIN_EVENT_MGR.RegisterWindow(new GP_MainTitleWindow(GetPageName()), true);

	return true;
}

void GamePageMainTitle::Update(const MkTimeState& timeState)
{
	
}

void GamePageMainTitle::Clear(MkDataNode* sharingNode)
{
	MK_DELETE(m_SceneNode);

	MkDrawStep* drawStep = MK_RENDERER.GetDrawQueue().GetStep(L"MainDS");
	if (drawStep != NULL)
	{
		drawStep->ClearAllSceneNodes();
	}

	MK_WIN_EVENT_MGR.RemoveWindow(GetPageName());
}

GamePageMainTitle::GamePageMainTitle(const MkHashStr& name) : MkBasePage(name)
{
	m_SceneNode = NULL;
}

//------------------------------------------------------------------------------------------------//
