
#include "MkCore_MkPathName.h"
#include "MkCore_MkPageManager.h"

#include "MkS2D_MkSceneNode.h"
#include "MkS2D_MkDrawStep.h"
#include "MkS2D_MkRenderer.h"

#include "GamePageClientStart.h"

//------------------------------------------------------------------------------------------------//

const MkHashStr GamePageClientStart::Name(L"ClientStart");


bool GamePageClientStart::SetUp(MkDataNode& sharingNode)
{
	// ����ۿ� �ٷ� ����ϴ� render step ����
	MkDrawQueue& drawQueue = MK_RENDERER.GetDrawQueue();
	MkDrawStep* drawStep = drawQueue.CreateStep(L"CS", -1);
	drawStep->SetBackgroundColor(MkColor::Black);

	// �⺻ scene ����
	m_SceneNode = new MkSceneNode(L"Scene");

	m_SceneNode->SetLocalDepth(1000.f);
	drawStep->AddSceneNode(m_SceneNode);

	MkSRect* bgRect = m_SceneNode->CreateSRect(L"MainBG");
	bgRect->SetTexture(L"Image\\main_bg.dds", MkHashStr::NullHash);

	MkSRect* textRect = m_SceneNode->CreateSRect(L"Msg");
	
	MkStr msg;
	MkDecoStr::Convert(L"���40", L"YellowFS", 0, L"���� �����մϴ�." + MkStr::CR + L"��ø� ��ٷ� �ּ���.", msg);
	textRect->SetDecoString(msg);
	textRect->AlignRect(drawStep->GetRegionRect().size, eRAP_LeftTop, MkFloat2(100.f, 100.f), 0.f);
	textRect->SetLocalDepth(-1.f);

	return true;
}

void GamePageClientStart::Update(const MkTimeState& timeState)
{
	m_SceneNode->UpdateAll();

	// �� �� ��¸��� �����̹Ƿ� �ƹ��͵� �����ʰ� �ٷ� ���� �������� �̵�
	MK_PAGE_MGR.SetMoveMessage(L"ToNextPage");
}

void GamePageClientStart::Clear(void)
{
	MK_DELETE(m_SceneNode);

	MK_RENDERER.GetDrawQueue().Clear();
}

GamePageClientStart::GamePageClientStart() : MkBasePage(Name)
{
	m_SceneNode = NULL;
}

//------------------------------------------------------------------------------------------------//
