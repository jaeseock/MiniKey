
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
	// ���� draw step ����(RTT)
	MkInt2 currResolution = MK_DISPLAY_MGR.GetCurrentResolution();
	MkDrawStep* sceneStep = MK_RENDERER.GetDrawQueue().CreateStep(L"MainDS", 0, MkRenderTarget::eTexture, 1, MkUInt2(currResolution.x, currResolution.y));

	// window manager �ʱ�ȭ
	MkBaseTexturePtr sceneTexture;
	sceneStep->GetTargetTexture(0, sceneTexture);
	MK_WIN_EVENT_MGR.SetUp(sceneTexture);

	// dice seed �ʱ�ȭ
	GAME_SYSTEM.SetDiceSeed();

	// ���� ���� ���ҽ� �ε�
	GameDataNode::SetUp();
	GameSharedUI::SetUp();

	return true;
}

void GamePageGameRoot::Update(const MkTimeState& timeState)
{
	// window �̺�Ʈ ó��
	MK_WIN_EVENT_MGR.Update();
}

void GamePageGameRoot::Clear(MkDataNode* sharingNode)
{
	MK_CHECK(GAME_SYSTEM.SaveMasterUserData(GDEF_LAST_SAVE_DATA_PATH), L"������ ���� ���� ��� ����") {}

	GameDataNode::Clear();
	GameSharedUI::Clear();

	// window manager ������
	MK_WIN_EVENT_MGR.Clear();

	// ���� draw step�� ����
	MK_RENDERER.GetDrawQueue().RemoveStep(L"MainDS");
}

GamePageGameRoot::GamePageGameRoot(const MkHashStr& name) : MkBasePage(name)
{
	
}

//------------------------------------------------------------------------------------------------//
