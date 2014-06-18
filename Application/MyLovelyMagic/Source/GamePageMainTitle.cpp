
#include "MkCore_MkPathName.h"
#include "MkS2D_MkSceneNode.h"
#include "MkS2D_MkDrawStep.h"
#include "MkS2D_MkRenderer.h"

#include "GamePageMainTitle.h"

#include "MkCore_MkDataNode.h"

//------------------------------------------------------------------------------------------------//

const MkHashStr GamePageMainTitle::Name(L"MainTitle");


bool GamePageMainTitle::SetUp(MkDataNode& sharingNode)
{
	// 백버퍼에 바로 출력하는 render step 구성
	MkDrawQueue& drawQueue = MK_RENDERER.GetDrawQueue();
	MkDrawStep* drawStep = drawQueue.CreateStep(L"CS", -1);
	drawStep->SetBackgroundColor(MkColor::Black);

	// 기본 scene 구성
	// test node
	// test
	MkDataNode loadDN;
	MkDataNode saveDN;
	m_SceneNode = new MkSceneNode(L"Scene");
	if (loadDN.Load(L"Scene\\test_scene.txt"))
	{
		m_SceneNode->Load(loadDN);

		m_SceneNode->Update();

		m_SceneNode->Save(saveDN);
		saveDN.SaveToText(L"Scene\\test_scene_.txt");
	}

	drawStep->AddSceneNode(m_SceneNode);
	
	m_State = eDrawWelcomeMsg;

	return true;
}

void GamePageMainTitle::Update(const MkTimeState& timeState)
{
	//m_SceneNode->Update(MK_RENDERER.GetDrawQueue().GetStep(L"CS")->GetRegionRect());
	m_SceneNode->Update();

	switch (m_State)
	{
	case eDrawWelcomeMsg:
		// do nothing
		break;

	case eLoadAssets:
		_LoadAssets();
		m_State = eMoveToNextGamePage;
		break;

	case eMoveToNextGamePage:
		break;
	}
}

void GamePageMainTitle::Clear(void)
{
	MK_DELETE(m_SceneNode);

	//MK_TEXTURE_POOL.UnloadGroup(0);

	MK_RENDERER.GetDrawQueue().Clear();

	m_State = eUnset;
}

GamePageMainTitle::GamePageMainTitle() : MkBasePage(Name)
{
	m_SceneNode = NULL;
	m_State = eUnset;
}

void GamePageMainTitle::_LoadAssets(void)
{
}

//------------------------------------------------------------------------------------------------//
