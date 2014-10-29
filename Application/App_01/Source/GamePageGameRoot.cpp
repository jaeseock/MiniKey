
#include "MkCore_MkCheck.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkDisplayManager.h"
#include "MkS2D_MkRenderer.h"
#include "MkS2D_MkWindowEventManager.h"

#include "GameDataNode.h"
#include "GameSharedUI.h"
#include "GameSystemManager.h"
#include "GamePageGameRoot.h"

//------------------------------------------------------------------------------------------------//

bool GamePageGameRoot::SetUp(MkDataNode& sharingNode)
{
	// 공용 draw step 생성(RTT)
	MkInt2 currResolution = MK_DISPLAY_MGR.GetCurrentResolution();
	MkDrawStep* sceneStep = MK_RENDERER.GetDrawQueue().CreateStep(L"MainDS", 0, MkRenderTarget::eTexture, 1, MkUInt2(currResolution.x, currResolution.y));

	// window manager 초기화
	MkBaseTexturePtr sceneTexture;
	sceneStep->GetTargetTexture(0, sceneTexture);
	MK_WIN_EVENT_MGR.SetUp(sceneTexture);

	// dice seed 초기화
	GAME_SYSTEM.SetDiceSeed();

	// 게임 공용 리소스 로딩
	GameDataNode::SetUp();
	GameSharedUI::SetUp();

	return true;
}

void GamePageGameRoot::Update(const MkTimeState& timeState)
{
	// window 이벤트 처리
	MK_WIN_EVENT_MGR.Update();
}

void GamePageGameRoot::Clear(MkDataNode* sharingNode)
{
	MK_CHECK(GAME_SYSTEM.SaveMasterUserData(GDEF_LAST_SAVE_DATA_PATH), L"마지막 저장 파일 기록 오류") {}

	GameDataNode::Clear();
	GameSharedUI::Clear();

	// window manager 날리고
	MK_WIN_EVENT_MGR.Clear();

	// 공용 draw step도 날림
	MK_RENDERER.GetDrawQueue().RemoveStep(L"MainDS");
}

GamePageGameRoot::GamePageGameRoot(const MkHashStr& name) : MkBasePage(name)
{
	
}

//------------------------------------------------------------------------------------------------//
