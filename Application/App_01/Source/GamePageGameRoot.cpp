
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

	// 게임 공용 리소스 로딩
	GameDataNode::SetUp();
	GameSharedUI::SetUp();

	// 기본 유저 설정
	MkDataNode defUserSetting;
	MkPathName defUserFilePath = L"DataNode\\DefaultUserData.txt";
	MK_CHECK(defUserSetting.Load(defUserFilePath) && GAME_SYSTEM.GetMasterPlayer().Load(defUserSetting), L"기본 유저 설정파일 로딩 오류 : " + defUserFilePath)
		return false;

	return true;
}

void GamePageGameRoot::Update(const MkTimeState& timeState)
{
	// window 이벤트 처리
	MK_WIN_EVENT_MGR.Update();
}

void GamePageGameRoot::Clear(void)
{
	MkDataNode lastSaveData;
	if (GAME_SYSTEM.GetMasterPlayer().Save(lastSaveData))
	{
		lastSaveData.SaveToText(L"DataNode\\LastSaveFile.txt");
	}

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
