
#include "MkCore_MkPageManager.h"
#include "MkCore_MkInputManager.h"

#include "MkS2D_MkDrawStep.h"
#include "MkS2D_MkRenderer.h"
#include "MkS2D_MkBaseWindowNode.h"
#include "MkS2D_MkWindowEventManager.h"


#include "MkS2D_MkWindowOpHelper.h"

#include "GamePageMainTitle.h"

const MkHashStr GamePageMainTitle::Name(L"MainTitle");

//------------------------------------------------------------------------------------------------//

class GP_MainTitleWindow : public MkBaseWindowNode
{
protected:

	MkHashStr m_NewBtnName;
	MkHashStr m_LoadBtnName;
	MkHashStr m_ExitBtnName;

public:

	virtual void UseWindowEvent(WindowEvent& evt)
	{
		if (evt.type == MkSceneNodeFamilyDefinition::eCursorLeftRelease)
		{
			if (evt.node->GetNodeName() == m_NewBtnName)
			{
				//MK_WIN_EVENT_MGR.DeactivateWindow(GetNodeName());
			}
			else if (evt.node->GetNodeName() == m_LoadBtnName)
			{
				//MK_WIN_EVENT_MGR.DeactivateWindow(GetNodeName());
			}
			else if (evt.node->GetNodeName() == m_ExitBtnName)
			{
				DestroyWindow(MK_INPUT_MGR.__GetTargetWindowHandle());
			}
		}
	}

	GP_MainTitleWindow(void) : MkBaseWindowNode(GamePageMainTitle::Name)
	{
		const float btnWidth = 200.f;
		MkDrawStep* drawStep = MK_RENDERER.GetDrawQueue().GetStep(L"MainDS");
		float xPos = (drawStep->GetRegionRect().size.x - btnWidth) / 2.f;
		SetLocalPosition(MkFloat2(xPos, 150.f));

		const MkFloat2 btnSize(btnWidth, 40.f);

		m_NewBtnName = L"NewBtn";
		m_LoadBtnName = L"LoadBtn";
		m_ExitBtnName = L"ExitBtn";
		
		MkWindowOpHelper::CreateWindowPreset(this, m_NewBtnName, MkHashStr::NullHash, eS2D_WPC_OKButton, btnSize, L"�� ���� ����", MkFloat2(0.f, 160.f));
		MkWindowOpHelper::CreateWindowPreset(this, m_LoadBtnName, MkHashStr::NullHash, eS2D_WPC_OKButton, btnSize, L"���� ���� �̾��ϱ�", MkFloat2(0.f, 80.f));
		MkWindowOpHelper::CreateWindowPreset(this, m_ExitBtnName, MkHashStr::NullHash, eS2D_WPC_CancelButton, btnSize, L"����", MkFloat2(0.f, 0.f));
	}

	virtual ~GP_MainTitleWindow() {}
};

//------------------------------------------------------------------------------------------------//

bool GamePageMainTitle::SetUp(MkDataNode& sharingNode)
{
	// draw step
	MkDrawStep* drawStep = MK_RENDERER.GetDrawQueue().GetStep(L"MainDS");

	// �⺻ scene ����
	m_SceneNode = new MkSceneNode(L"Scene");

	m_SceneNode->SetLocalDepth(1000.f);
	drawStep->AddSceneNode(m_SceneNode);

	MkSRect* bgRect = m_SceneNode->CreateSRect(L"MainBG");
	bgRect->SetTexture(L"Image\\main_bg.dds", MkHashStr::NullHash);

	MkSRect* textRect = m_SceneNode->CreateSRect(L"Title");

	MkStr msg;
	MkDecoStr::Convert(L"���40", L"RedFS", 0, L"������ ���������� �Ͽ��� ���� ����", msg);
	textRect->SetDecoString(msg);
	textRect->AlignRect(drawStep->GetRegionRect().size, eRAP_MiddleTop, MkFloat2(0.f, 200.f), 0.f);
	textRect->SetLocalDepth(-1.f);

	m_SceneNode->UpdateAll();

	// window
	MK_WIN_EVENT_MGR.RegisterWindow(new GP_MainTitleWindow(), true);

	return true;
}

void GamePageMainTitle::Update(const MkTimeState& timeState)
{
	// �� �� ��¸��� �����̹Ƿ� �ƹ��͵� �����ʰ� �ٷ� ���� �������� �̵�
	//MK_PAGE_MGR.SetMoveMessage(L"ToNextPage");
}

void GamePageMainTitle::Clear(void)
{
	MK_DELETE(m_SceneNode);

	MkDrawStep* drawStep = MK_RENDERER.GetDrawQueue().GetStep(L"MainDS");
	if (drawStep != NULL)
	{
		drawStep->ClearAllSceneNodes();
	}

	MK_WIN_EVENT_MGR.RemoveWindow(GamePageMainTitle::Name);
}

GamePageMainTitle::GamePageMainTitle() : MkBasePage(Name)
{
	m_SceneNode = NULL;
}

//------------------------------------------------------------------------------------------------//
