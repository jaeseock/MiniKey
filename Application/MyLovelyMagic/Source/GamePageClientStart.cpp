
#include "MkCore_MkPathName.h"
#include "MkCore_MkPageManager.h"
#include "MkCore_MkTimeManager.h"
#include "MkCore_MkInputManager.h"

#include "MkS2D_MkSceneNode.h"
#include "MkS2D_MkDrawStep.h"
#include "MkS2D_MkTexturePool.h"
#include "MkS2D_MkRenderer.h"

#include "GamePageClientStart.h"

#define GDEF_GP_SP_DECO_ALPHA_BEGIN 0.3f
#define GDEF_GP_SP_TXT_TOGGLE_TIME 1.f

//------------------------------------------------------------------------------------------------//

const MkHashStr GamePageClientStart::Name(L"ClientStart");


bool GamePageClientStart::SetUp(MkDataNode& sharingNode)
{
	// 백버퍼에 바로 출력하는 render step 구성
	MkDrawQueue& drawQueue = MK_RENDERER.GetDrawQueue();
	MkDrawStep* drawStep = drawQueue.CreateStep(L"CS", -1);
	drawStep->SetBackgroundColor(MkColor::Black);

	// 기본 scene 구성
	m_SceneNode = new MkSceneNode(L"Scene");

	m_SceneNode->SetLocalPosition(MkVec3(0.f, 0.f, 1000.f));
	drawStep->AddSceneNode(m_SceneNode);

	// loading BG. 남자라면 아스키 아트!!!
	MkStr decoStr;
	decoStr.ReadTextFile(L"Text\\ClientStartMsg.txt");

	MkSRect* textRect = m_SceneNode->CreateSRect(L"AA");
	textRect->SetLocalPosition(MkFloat2(0.f, 0.f));
	textRect->SetLocalDepth(-1.f);
	textRect->SetDecoString(decoStr);
	textRect->SetObjectAlpha(GDEF_GP_SP_DECO_ALPHA_BEGIN);

	// start msg
	MkStr tmpDeco;
	MkDecoStr::Convert(L"나고40", L"노랑그림자", 0, L"Press any mouse button", tmpDeco);
	textRect = m_SceneNode->CreateSRect(L"Hit");
	textRect->SetDecoString(tmpDeco);
	textRect->SetVisible(false);
	MkFloat2 alignedPos = drawStep->GetRegionRect().GetSnapPosition(textRect->GetLocalRect(), eRAP_MiddleCenter, MkFloat2(0.f, 0.f));
	textRect->SetLocalPosition(alignedPos);
	textRect->SetLocalDepth(-1.f);

	/*
	MK_TEXTURE_POOL.LoadBitmapTexture(L"Image\\MainTitle_Deco.png", 0);

	MkSRect* decoRect = m_SceneNode->CreateSRect(L"deco");
	decoRect->SetLocalPosition(MkFloat2(100.f, 0.f));
	decoRect->SetLocalDepth(-2.f);

	MkBaseTexturePtr decoTexture;
	MK_TEXTURE_POOL.GetBitmapTexture(L"Image\\MainTitle_Deco.png", decoTexture);
	decoRect->SetTexture(decoTexture);
	decoRect->SetObjectAlpha(0.5f);
	decoTexture = NULL;
	*/

	m_State = eDrawStartMsg;

	return true;
}

void GamePageClientStart::Update(const MkTimeState& timeState)
{
	m_SceneNode->Update();

	switch (m_State)
	{
	case eDrawStartMsg:
		m_State = eLoadAssets;
		break;

	case eLoadAssets:
		_LoadAssets();

		MK_TIME_MGR.SetUp(); // 로딩 끝났으니 시간관리자 초기화(밀렸던 프레임 제거)
		m_State = eShowEffect;
		break;

	case eShowEffect:
		m_TimeCounter.SetUp(timeState, GDEF_GP_SP_TXT_TOGGLE_TIME);
		m_State = eOnEffect;
		break;

	case eOnEffect:
		{
			float tickRatio;
			if (m_TimeCounter.OnTick(timeState))
			{
				tickRatio = 1.f;

				m_TimeCounter.SetUp(timeState, GDEF_GP_SP_TXT_TOGGLE_TIME);

				m_SceneNode->GetSRect(L"Hit")->SetVisible(true); // hit msg on

				m_State = eMoveToNextGamePage;
			}
			else
			{
				tickRatio = m_TimeCounter.GetTickRatio(timeState);
			}

			MkSRect* textRect = m_SceneNode->GetSRect(L"AA");

			float objAlpha = GDEF_GP_SP_DECO_ALPHA_BEGIN + (1.f - GDEF_GP_SP_DECO_ALPHA_BEGIN) * tickRatio;
			textRect->SetObjectAlpha(objAlpha);

			textRect->SetLocalPosition(MkFloat2(150.f * tickRatio, 0.f));
		}
		break;

	case eMoveToNextGamePage:
		{
			if (m_TimeCounter.OnTick(timeState))
			{
				MkSRect* textRect = m_SceneNode->GetSRect(L"Hit"); // toggle
				textRect->SetVisible(!textRect->GetVisible());

				m_TimeCounter.SetUp(timeState, GDEF_GP_SP_TXT_TOGGLE_TIME);
			}

			if (MK_INPUT_MGR.GetMouseLeftButtonReleased() ||
				MK_INPUT_MGR.GetMouseRightButtonReleased() ||
				MK_INPUT_MGR.GetMouseMiddleButtonReleased())
			{
				MK_PAGE_MGR.SetMoveMessage(L"ToNext");
			}
		}
		break;
	}
}

void GamePageClientStart::Clear(void)
{
	MK_DELETE(m_SceneNode);

	MK_TEXTURE_POOL.UnloadGroup(0);

	MK_RENDERER.GetDrawQueue().Clear();

	m_State = eUnset;
}

GamePageClientStart::GamePageClientStart() : MkBasePage(Name)
{
	m_SceneNode = NULL;
	m_State = eUnset;
}

void GamePageClientStart::_LoadAssets(void)
{
}

//------------------------------------------------------------------------------------------------//
