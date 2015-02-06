
//------------------------------------------------------------------------------------------------//
// single/multi-thread rendering application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkPageManager.h"
#include "MkCore_MkInputManager.h"
#include "MkCore_MkTimeState.h"
#include "MkCore_MkWin32Application.h"
#include "MkCore_MkProfilingManager.h"
#include "MkCore_MkSlangFilter.h"
#include "MkCore_MkFloatOp.h"

#include "MkCore_MkDataNode.h"

#include "MkPA_MkRenderFramework.h"

#include "MkCore_MkUniformDice.h"

//#include "MkS2D_MkTexturePool.h"
#include "MkPA_MkFontManager.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkRenderer.h"

#include "MkPA_MkTextNode.h"

#include "MkPA_MkSceneNode.h"
//#include "MkS2D_MkBaseWindowNode.h"
//#include "MkS2D_MkSpreadButtonNode.h"
//#include "MkS2D_MkCheckButtonNode.h"
//#include "MkS2D_MkScrollBarNode.h"
//#include "MkS2D_MkEditBoxNode.h"
//#include "MkS2D_MkTabWindowNode.h"

#include "MkPA_MkDrawSceneNodeStep.h"
#include "MkPA_MkStaticResourceContainer.h"
//#include "MkS2D_MkWindowEventManager.h"

//#include "MkCore_MkNameSelection.h"

#include "MkCore_MkDevPanel.h"

//------------------------------------------------------------------------------------------------//

// TestPage 선언
class TestPage : public MkBasePage
{
public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		sn = new MkSceneNode(L"Root");
		sn->SetLocalPosition(MkFloat2(100.f, 100.f));

		sub = new MkSceneNode(L"Sub");
		sub->CreatePanel(L"P").SetTexture(L"Image\\s03.jpg");
		
		MkPanel& ip = sn->CreatePanel(L"ImgTest");
		ip.SetSmallerSourceOp(MkPanel::eAttachToLeftTop);
		ip.SetBiggerSourceOp(MkPanel::eCutSource);
		ip.SetPanelSize(MkFloat2(450.f, 250.f));
		ip.SetLocalDepth(2.f);
		ip.SetTexture(L"Image\\s01.jpg");

		MkPanel& tp = sn->CreatePanel(L"TextTest");
		tp.SetSmallerSourceOp(MkPanel::eAttachToLeftTop);
		tp.SetBiggerSourceOp(MkPanel::eCutSource);
		tp.SetPanelSize(MkFloat2(110.f, 250.f));
		tp.SetLocalDepth(1.f);
		tp.SetTextNode(L"_Sample", true);

		MkPanel& mp = sn->CreatePanel(L"MaskingTest", sub, MkInt2(200, 150));
		mp.SetLocalPosition(MkFloat2(120.f, 50.f));

		MkDrawSceneNodeStep* ds = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep(L"Final");
		ds->SetSceneNode(sn);

		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		if (sn != NULL)
		{
			MkFloat2 localPos = sn->GetLocalPosition();
			const float movement = static_cast<float>(timeState.elapsed) * 300.f;
			if (MK_INPUT_MGR.GetKeyPushing(L'A'))
			{
				localPos.x -= movement;
			}
			if (MK_INPUT_MGR.GetKeyPushing(L'D'))
			{
				localPos.x += movement;
			}
			if (MK_INPUT_MGR.GetKeyPushing(L'W'))
			{
				localPos.y += movement;
			}
			if (MK_INPUT_MGR.GetKeyPushing(L'S'))
			{
				localPos.y -= movement;
			}
			sn->SetLocalPosition(localPos);

			float localRot = sn->GetLocalRotation();
			const float rotVel = static_cast<float>(timeState.elapsed) * MKDEF_PI * 0.5f;
			if (MK_INPUT_MGR.GetKeyPushing(L'Q'))
			{
				localRot -= rotVel;
			}
			if (MK_INPUT_MGR.GetKeyPushing(L'E'))
			{
				localRot += rotVel;
			}
			sn->SetLocalRotation(localRot);

			float localScale = sn->GetLocalScale();
			const float scaleVel = static_cast<float>(timeState.elapsed);
			if (MK_INPUT_MGR.GetKeyPushing(L'Z'))
			{
				localScale -= scaleVel;
			}
			if (MK_INPUT_MGR.GetKeyPushing(L'X'))
			{
				localScale += scaleVel;
			}
			sn->SetLocalScale(localScale);

			MkPanel* tp = sn->GetPanel(L"TextTest");
			MkFloat2 psp = tp->GetPixelScrollPosition();
			if (MK_INPUT_MGR.GetKeyPushing(VK_LEFT))
			{
				psp.x -= movement;
			}
			if (MK_INPUT_MGR.GetKeyPushing(VK_RIGHT))
			{
				psp.x += movement;
			}
			if (MK_INPUT_MGR.GetKeyPushing(VK_UP))
			{
				psp.y -= movement;
			}
			if (MK_INPUT_MGR.GetKeyPushing(VK_DOWN))
			{
				psp.y += movement;
			}
			tp->SetPixelScrollPosition(psp);
		}

		if (MK_INPUT_MGR.GetKeyReleased(L' '))
		{
			if (sn != NULL)
			{
				MkPanel* ip = sn->GetPanel(L"ImgTest");
				MkArray<MkHashStr> keys;
				ip->GetAllSequences(keys);
				++si;
				if (si >= keys.GetSize())
				{
					si = 0;
				}
				ip->SetSubsetOrSequenceName(keys[si], timeState.fullTime);
				MK_DEV_PANEL.MsgToLog(keys[si].GetString());
			}
		}

		if (MK_INPUT_MGR.GetKeyReleased(L'1'))
		{
			if (sn != NULL)
			{
				MkPanel* tp = sn->GetPanel(L"TextTest");
				MkTextNode* textNode = tp->GetTextNodePtr();
				MkTextNode* targetNode = textNode->GetChildNode(L"1st")->GetChildNode(L"Sub list")->GetChildNode(L"이번이구나");
				targetNode->SetFontStyle(L"Desc:Notice");
				targetNode->SetText(L"- 이걸로 바꿨음당 ( ㅡ_-)r");
				tp->BuildAndUpdateTextCache();
			}
		}

		if (sn != NULL)
		{
			sn->Update(timeState.fullTime);
		}
		if (sub != NULL)
		{
			sub->Update(timeState.fullTime);
		}
		
		//MK_DEV_PANEL.MsgToFreeboard(0, L"HorizontalChange : " + hcn.GetString());
	}

	virtual void Clear(MkDataNode* sharingNode = NULL)
	{
		MK_DELETE(sn);
		MK_DELETE(sub);
	}

	TestPage(const MkHashStr& name) : MkBasePage(name)
	{
		sn = NULL;
		sub = NULL;
		si = 0;
	}

	virtual ~TestPage() { Clear(); }

protected:

	MkSceneNode* sn;
	MkSceneNode* sub;
	unsigned int si;
};

class TestFramework : public MkRenderFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg)
	{
		MK_PAGE_MGR.SetUp(new MkBasePage(L"Root"));
		MK_PAGE_MGR.RegisterChildPage(L"Root", new TestPage(L"TestPage"));
		
		MK_PAGE_MGR.ChangePageDirectly(L"TestPage");
		
		return MkRenderFramework::SetUp(clientWidth, clientHeight, fullScreen, arg);
	}

	virtual ~TestFramework() {}
};

// TestApplication 선언
class TestApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new TestFramework; }

public:
	TestApplication() : MkWin32Application() {}
	virtual ~TestApplication() {}
};

//------------------------------------------------------------------------------------------------//

// 엔트리 포인트에서의 TestApplication 실행
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"떼스또", L"..\\FileRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

