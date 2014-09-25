
#include "MkCore_MkPathName.h"
#include "MkCore_MkPageManager.h"

#include "MkS2D_MkSceneNode.h"
#include "MkS2D_MkDrawStep.h"
#include "MkS2D_MkRenderer.h"

#include "GameGlobalDefinition.h"
#include "GamePageClientStart.h"

//------------------------------------------------------------------------------------------------//

bool GamePageClientStart::SetUp(MkDataNode& sharingNode)
{
	// ����ۿ� �ٷ� ����ϴ� render step ����
	MkDrawStep* drawStep = MK_RENDERER.GetDrawQueue().CreateStep(L"TmpDS", 0);

	// �⺻ scene ����
	m_SceneNode = new MkSceneNode(L"Scene");

	m_SceneNode->SetLocalDepth(1000.f);
	drawStep->AddSceneNode(m_SceneNode);

	MkSRect* bgRect = m_SceneNode->CreateSRect(L"MainBG");
	bgRect->SetTexture(L"Scene\\main_bg.dds", MkHashStr::NullHash);

	MkSRect* textRect = m_SceneNode->CreateSRect(L"Msg");
	
	MkStr msg;
	MkDecoStr::Convert(L"���40", L"YellowFS", 0, L"���� �����մϴ�." + MkStr::CR + L"��ø� ��ٷ� �ּ���.", msg);
	textRect->SetDecoString(msg);
	textRect->AlignRect(drawStep->GetRegionRect().size, eRAP_LeftTop, MkFloat2(100.f, 100.f), 0.f);
	textRect->SetLocalDepth(-1.f);

	m_SceneNode->UpdateAll();

	return true;
}

void GamePageClientStart::Update(const MkTimeState& timeState)
{
	// �� �� ��¸��� �����̹Ƿ� �ƹ��͵� �����ʰ� �ٷ� ���� �������� �̵�
	MK_PAGE_MGR.ChangePageDirectly(GamePageName::MainTitle);
}

void GamePageClientStart::Clear(void)
{
	MK_DELETE(m_SceneNode);

	MK_RENDERER.GetDrawQueue().RemoveStep(L"TmpDS");
}

GamePageClientStart::GamePageClientStart(const MkHashStr& name) : MkBasePage(name)
{
	m_SceneNode = NULL;
}

//------------------------------------------------------------------------------------------------//
