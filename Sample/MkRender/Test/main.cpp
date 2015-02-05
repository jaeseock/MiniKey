
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
		MkPanel* p = sn->GetPanel(L"Test");
		p->SetSmallerSourceOp(MkPanel::eAttachToLeftTop);
		p->SetBiggerSourceOp(MkPanel::eCutSource);
		p->SetPanelSize(MkFloat2(110.f, 250.f));
		
		//p->SetTexture(L"Image\\s01.jpg");
		
		MkTextNode tn;
		tn.SetUp(L"TextNodeSample.txt");
		p->SetTextNode(tn, true);

		//sn->SetLocalPosition(MkFloat2(0.f, 505.f));
		//sn->SetLocalDepth(1.f);

		MkDrawSceneNodeStep& ds = MK_RENDERER.GetDS();
		ds.SetSceneNode(sn);

		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		if (sn != NULL)
		{
			MkPanel* p = sn->GetPanel(L"Test");

			MkFloat2 localPos = p->GetLocalPosition();
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
			p->SetLocalPosition(localPos);

			float localRot = p->GetLocalRotation();
			const float rotVel = static_cast<float>(timeState.elapsed) * MKDEF_PI * 0.5f;
			if (MK_INPUT_MGR.GetKeyPushing(L'Q'))
			{
				localRot -= rotVel;
			}
			if (MK_INPUT_MGR.GetKeyPushing(L'E'))
			{
				localRot += rotVel;
			}
			p->SetLocalRotation(localRot);

			float localScale = p->GetLocalScale();
			const float scaleVel = static_cast<float>(timeState.elapsed);
			if (MK_INPUT_MGR.GetKeyPushing(L'Z'))
			{
				localScale -= scaleVel;
			}
			if (MK_INPUT_MGR.GetKeyPushing(L'X'))
			{
				localScale += scaleVel;
			}
			p->SetLocalScale(localScale);

			MkFloat2 psp = p->GetPixelScrollPosition();
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
			p->SetPixelScrollPosition(psp);
		}

		if (MK_INPUT_MGR.GetKeyReleased(L' '))
		{
			if (sn != NULL)
			{
				MkPanel* p = sn->GetPanel(L"Test");
				MkArray<MkHashStr> keys;
				p->GetAllSequences(keys);
				++si;
				if (si >= keys.GetSize())
				{
					si = 0;
				}
				p->SetSubsetOrSequenceName(keys[si], timeState.fullTime);
				MK_DEV_PANEL.MsgToLog(keys[si].GetString());
			}
		}

		if (MK_INPUT_MGR.GetKeyReleased(L'1'))
		{
			if (sn != NULL)
			{
				MkPanel* panel = sn->GetPanel(L"Test");
				MkTextNode* textNode = panel->GetTextNodePtr();
				MkTextNode* targetNode = textNode->GetChildNode(L"1st")->GetChildNode(L"Sub list")->GetChildNode(L"이번이구나");
				targetNode->SetFontStyle(L"Desc:Notice");
				targetNode->SetText(L"- 이걸로 바꿨음당 ( ㅡ_-)r");
				panel->BuildAndUpdateTextCache();
			}
		}

		if (sn != NULL)
		{
			sn->Update(timeState.fullTime);
		}
		
		//MK_DEV_PANEL.MsgToFreeboard(0, L"HorizontalChange : " + hcn.GetString());
	}

	virtual void Clear(MkDataNode* sharingNode = NULL)
	{
		MK_DELETE(sn);
	}

	TestPage(const MkHashStr& name) : MkBasePage(name)
	{
		sn = NULL;
		si = 0;
	}

	virtual ~TestPage() { Clear(); }

protected:

	MkSceneNode* sn;
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

