
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

	// ���� ���� ���ҽ� �ε�
	GameDataNode::SetUp();
	GameSharedUI::SetUp();

	// �⺻ ���� ����
	MkDataNode defUserSetting;
	MkPathName defUserFilePath = L"DataNode\\DefaultUserData.txt";
	MK_CHECK(defUserSetting.Load(defUserFilePath) && GAME_SYSTEM.GetMasterPlayer().Load(defUserSetting), L"�⺻ ���� �������� �ε� ���� : " + defUserFilePath)
		return false;

	return true;
}

void GamePageGameRoot::Update(const MkTimeState& timeState)
{
	// window �̺�Ʈ ó��
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

	// window manager ������
	MK_WIN_EVENT_MGR.Clear();

	// ���� draw step�� ����
	MK_RENDERER.GetDrawQueue().RemoveStep(L"MainDS");
}

GamePageGameRoot::GamePageGameRoot(const MkHashStr& name) : MkBasePage(name)
{
	
}

//------------------------------------------------------------------------------------------------//
