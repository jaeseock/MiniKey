
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
	// 백버퍼에 바로 출력하는 render step 구성
	MkDrawStep* drawStep = MK_RENDERER.GetDrawQueue().CreateStep(L"TmpDS", 0);

	// 기본 scene 구성
	m_SceneNode = new MkSceneNode(L"Scene");

	m_SceneNode->SetLocalDepth(1000.f);
	drawStep->AddSceneNode(m_SceneNode);

	MkSRect* bgRect = m_SceneNode->CreateSRect(L"MainBG");
	bgRect->SetTexture(L"Scene\\main_bg.dds", MkHashStr::NullHash);

	MkSRect* textRect = m_SceneNode->CreateSRect(L"Msg");
	
	MkStr msg;
	MkDecoStr::Convert(L"고딕40", L"YellowFS", 0, L"앱을 시작합니다." + MkStr::CR + L"잠시만 기다려 주세요.", msg);
	textRect->SetDecoString(msg);
	textRect->AlignRect(drawStep->GetRegionRect().size, eRAP_LeftTop, MkFloat2(100.f, 100.f), 0.f);
	textRect->SetLocalDepth(-1.f);

	m_SceneNode->UpdateAll();

	return true;
}

void GamePageClientStart::Update(const MkTimeState& timeState)
{
	// 한 번 출력만이 목적이므로 아무것도 하지않고 바로 다음 페이지로 이동
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
