
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

#include "MkCore_MkDataNode.h"

#include "MkS2D_MkRenderFramework.h"

#include "MkCore_MkUniformDice.h"

#include "MkS2D_MkTexturePool.h"
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkDisplayManager.h"
#include "MkS2D_MkRenderer.h"

#include "MkS2D_MkDecoStr.h"

#include "MkS2D_MkSceneNode.h"
#include "MkS2D_MkBaseWindowNode.h"
#include "MkS2D_MkSpreadButtonNode.h"
#include "MkS2D_MkCheckButtonNode.h"
#include "MkS2D_MkScrollBarNode.h"
#include "MkS2D_MkEditBoxNode.h"
#include "MkS2D_MkTabWindowNode.h"

#include "MkS2D_MkDrawStep.h"
#include "MkS2D_MkWindowEventManager.h"

//#include "MkCore_MkNameSelection.h"

#include "MkCore_MkDevPanel.h"

//------------------------------------------------------------------------------------------------//

// TestPage ¼±¾ð
class TestPage : public MkBasePage
{
public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		MK_TEXTURE_POOL.LoadBitmapTexture(L"Image\\s00.png");
		MK_TEXTURE_POOL.LoadBitmapTexture(L"Image\\s01.jpg");
		MK_TEXTURE_POOL.LoadBitmapTexture(L"Image\\s02.jpg");
		MK_TEXTURE_POOL.LoadBitmapTexture(L"Image\\s03.jpg");

		MK_SLANG_FILTER.RegisterKeyword(L"ºüÅ¥");
		MK_SLANG_FILTER.RegisterKeyword(L"»µÅ¥");

		MkDrawQueue& drawQueue = MK_RENDERER.GetDrawQueue();
		MkDrawStep* step00 = drawQueue.CreateStep(L"step_00", -1, MkRenderTarget::eTexture, 1, MkUInt2(800, 600), MkRenderToTexture::eRGBA);
		MkDrawStep* step01 = drawQueue.CreateStep(L"step_01", -1);
		
		m_RootNode = new MkSceneNode(L"Root");
		m_RootNode->SetLocalPosition(MkVec3(0.f, 0.f, 1000.f));

		m_Node01 = new MkBaseWindowNode(L"01");
		m_RootNode->AttachChildNode(m_Node01);

		MkBaseWindowNode* alignNode = new MkBaseWindowNode(L"align node");
		m_RootNode->AttachChildNode(alignNode);
		alignNode->SetLocalPosition(MkVec3(100.f, 50.f, -900.f));
		MkSRect* bgRect = alignNode->CreateSRect(L"bg");
		MkBaseTexturePtr bgTex;
		MK_TEXTURE_POOL.GetBitmapTexture(L"Image\\s03.jpg", bgTex);
		bgRect->SetTexture(bgTex);
		bgTex = NULL;

		MkBaseWindowNode* txtNode = new MkBaseWindowNode(L"txt node");
		alignNode->AttachChildNode(txtNode);
		MkSRect* textRect = txtNode->CreateSRect(L"text");
		textRect->SetLocalPosition(MkFloat2(50.f, 50.f));
		textRect->SetLocalDepth(-1.f);

		MkStr bufStr;
		bufStr.ReadTextFile(L"DecoString.txt");
		textRect->SetDecoString(bufStr);

		MkBaseWindowNode::BasicPresetWindowDesc winDesc;
		//winDesc.SetStandardDesc(L"Default", true);
		winDesc.SetStandardDesc(L"Default", true, MkFloat2(300.f, 200.f));
		//winDesc.SetStandardDesc(L"Default", true, L"Image\\s02.jpg", L"");
		//winDesc.SetStandardDesc(L"Default", false);
		//winDesc.SetStandardDesc(L"Default", false, MkFloat2(150.f, 100.f));
		//winDesc.SetStandardDesc(L"Default", false, L"Image\\s02.jpg", L"");
		MkBaseWindowNode* titleWin = MkBaseWindowNode::CreateBasicWindow(NULL, L"WinRoot", winDesc);
		m_Node01->AttachChildNode(titleWin);

		MkBaseWindowNode* formNode = new MkBaseWindowNode(L"Form");
		formNode->CreateWindowPreset(L"Default", eS2D_WPC_GuideBox, MkFloat2(280.f, 160.f));
		formNode->SetLocalPosition(MkFloatRect(0.f, 0.f, 300.f, 180.f).GetSnapPosition(MkFloatRect(formNode->GetPresetComponentSize()), eRAP_MiddleCenter, MkFloat2(0.f, 0.f)));
		formNode->SetLocalDepth(-0.001f);

		MkBaseWindowNode* bgNode = dynamic_cast<MkBaseWindowNode*>((winDesc.hasTitle) ? titleWin->GetChildNode(L"Background") : titleWin);
		bgNode->AttachChildNode(formNode);

		MkSpreadButtonNode* lbNode = new MkSpreadButtonNode(L"SB");
		lbNode->CreateSelectionRootTypeButton(L"Default", MkFloat2(100.f, 20.f), MkSpreadButtonNode::eDownward);
		MkBaseWindowNode::ItemTagInfo tagInfo;
		tagInfo.iconPath = L"Default\\window_mgr.png";
		tagInfo.iconSubset = L"SampleIcon";
		lbNode->SetPresetComponentItemTag(tagInfo);
		lbNode->SetLocalPosition(MkVec3(10.f, 130.f, -0.1f));

		lbNode->AddItem(L"0", tagInfo);

		MkSpreadButtonNode* g1 = lbNode->AddItem(L"1", tagInfo);
		g1->AddItem(L"1-0", tagInfo);
		g1->AddItem(L"1-1", tagInfo);
		g1->AddItem(L"1-2", tagInfo);

		lbNode->AddItem(L"2", tagInfo);

		MkSpreadButtonNode* g3 = lbNode->AddItem(L"3", tagInfo);
		g3->AddItem(L"3-0", tagInfo);
		MkSpreadButtonNode* g31 = g3->AddItem(L"3-1", tagInfo);
		g31->AddItem(L"3-1-0", tagInfo);
		lbNode->SetTargetItem(L"1-1");

		formNode->AttachChildNode(lbNode);

		MkCheckButtonNode* cbNode = new MkCheckButtonNode(L"CB");
		MkBaseWindowNode::CaptionDesc captionDesc;
		captionDesc = L"Å×½ºÆ®Ä¸¼õ";
		cbNode->CreateCheckButton(L"Default", captionDesc, false);
		cbNode->SetCheck(true);
		cbNode->SetLocalPosition(MkVec3(10.f, 100.f, -0.001f));
		formNode->AttachChildNode(cbNode);

		MkScrollBarNode* vsbNode = new MkScrollBarNode(L"VSB");
		vsbNode->CreateScrollBar(L"Default", MkScrollBarNode::eVertical, true, 120.f);
		vsbNode->SetLocalPosition(MkVec3(260.f, 20.f + 14.f, -0.001f));
		vsbNode->SetPageInfo(200, 30, 20);
		formNode->AttachChildNode(vsbNode);

		MkScrollBarNode* hsbNode = new MkScrollBarNode(L"HSB");
		hsbNode->CreateScrollBar(L"Default", MkScrollBarNode::eHorizontal, true, 120.f);
		hsbNode->SetLocalPosition(MkVec3(140.f, 20.f, -0.001f));
		hsbNode->SetPageInfo(200, 30, 20);
		formNode->AttachChildNode(hsbNode);

		MkEditBoxNode* ebNode = new MkEditBoxNode(L"EB");
		ebNode->CreateEditBox(L"Default", MkFloat2(150.f, 20.f), L"", L"", L"", L"", L"¿ÀÀÌ!!!", true);
		ebNode->SetLocalPosition(MkVec3(10.f, 50.f, -0.001f));
		formNode->AttachChildNode(ebNode);

		MkBaseWindowNode* fiNode = new MkBaseWindowNode(L"FreeImageBtn");
		MkArray<MkHashStr> fiSN(4);
		fiSN.PushBack(L"DirUDef");
		fiSN.PushBack(L"DirUOCu");
		fiSN.PushBack(L"DirUOCl");
		fiSN.PushBack(L"DirUDis");
		fiNode->CreateFreeImageBaseButtonWindow(L"Default\\default_theme.png", fiSN);
		fiNode->SetLocalPosition(MkVec3(100.f, 100.f, -0.001f));
		formNode->AttachChildNode(fiNode);

		twNode = new MkTabWindowNode(L"TW");
		twNode->CreateTabRoot(L"Default", MkTabWindowNode::eRightside, MkFloat2(35.f, 20.f), MkFloat2(130.f, 40.f));
		tagInfo.iconPath.Clear();
		tagInfo.iconSubset.Clear();
		tagInfo.captionDesc.SetString(L"1ÅÇ");
		MkSRect* tmpr1 = twNode->AddTab(L"1ÅÇ", tagInfo)->CreateSRect(L"1");
		tmpr1->SetDecoString(L"1ÅÇ");
		tmpr1->SetLocalDepth(-0.001f);
		tagInfo.captionDesc.SetString(L"2ÅÇ");
		MkSRect* tmpr2 = twNode->AddTab(L"2ÅÇ", tagInfo)->CreateSRect(L"1");
		tmpr2->SetDecoString(L"2ÅÇ");
		tmpr2->SetLocalDepth(-0.001f);
		tagInfo.captionDesc.SetString(L"3ÅÇ");
		MkSRect* tmpr3 = twNode->AddTab(L"3ÅÇ", tagInfo)->CreateSRect(L"1");
		tmpr3->SetDecoString(L"3ÅÇ");
		tmpr3->SetLocalDepth(-0.001f);
		twNode->SetLocalPosition(MkVec3(125.f, 85.f, -0.001f));
		formNode->AttachChildNode(twNode);
		twNode->SetTabEnable(L"2ÅÇ", false);

		MkSRect* testRect = formNode->CreateSRect(L"_Sample");
		testRect->SetDecoString(L"¿ï¶ö¶ó" + MkStr::CR + L"¼¼¼õ");
		testRect->SetLocalPosition(MkFloat2(160.f, 60.f));
		testRect->SetLocalDepth(-0.001f);
		

		m_Node01->SetLocalPosition(MkVec3(400.f, 300.f, -910.f));

		m_Node02 = m_Node01->CreateChildNode(L"02");
		
		step00->AddSceneNode(m_RootNode);

		m_ScreenNode = new MkSceneNode(L"Root");
		MkSRect* screen = m_ScreenNode->CreateSRect(L"Screen");
		MkBaseTexturePtr screenTex;
		step00->GetTargetTexture(0, screenTex);
		screen->SetTexture(screenTex);
		step01->AddSceneNode(m_ScreenNode);

		_SetTargetNode(0);

		m_RectCount = 0;
		m_DiceX.SetMinMax(0, 700);
		m_DiceX.SetSeed(1234);
		m_DiceY.SetMinMax(0, 500);
		m_DiceY.SetSeed(5678);
		m_DiceT.SetMinMax(0, 44);
		m_DiceT.SetSeed(90);
		m_DiceA.SetMinMax(2, 4);
		m_DiceA.SetSeed(1000);

		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		if (MK_INPUT_MGR.GetKeyReleased(VK_SPACE))
		{
			MkSRect* srect;
			MkBaseTexturePtr diffTex;
			MK_TEXTURE_POOL.GetBitmapTexture(L"Image\\s01.jpg", diffTex);
			for (unsigned int i=0; i<10; ++i)
			{
				srect = m_Node02->CreateSRect(MkStr(m_RectCount));
				srect->SetLocalPosition(MkFloat2(static_cast<float>(m_DiceX.GenerateRandomNumber()), static_cast<float>(m_DiceY.GenerateRandomNumber())));
				srect->SetLocalDepth(-static_cast<float>(m_RectCount));
				
				srect->SetObjectAlpha(static_cast<float>(m_DiceA.GenerateRandomNumber()) * 0.2f);

				MkStr subsetName = L"Face_";
				subsetName += m_DiceT.GenerateRandomNumber();
				srect->SetTexture(diffTex, subsetName);
				
				++m_RectCount;
			}
		}

		if (MK_INPUT_MGR.GetKeyReleased(0x31))
		{
			_SetTargetNode(0);
		}
		if (MK_INPUT_MGR.GetKeyReleased(0x32))
		{
			_SetTargetNode(1);
		}
		if (MK_INPUT_MGR.GetKeyReleased(0x33))
		{
			_SetTargetNode(2);
		}

		if (MK_INPUT_MGR.GetKeyReleased(VK_LEFT))
		{
			MkFloat2 ss = twNode->GetTabButtonSize();
			twNode->SetTabButtonSize(MkFloat2(ss.x - 1, ss.y - 1));
			//m_Offset.x -= timeState.elapsed * 300.f;
		}
		if (MK_INPUT_MGR.GetKeyReleased(VK_RIGHT))
		{
			MkFloat2 ss = twNode->GetTabButtonSize();
			twNode->SetTabButtonSize(MkFloat2(ss.x + 1, ss.y + 1));
			//m_Offset.x += timeState.elapsed * 300.f;
		}
		if (MK_INPUT_MGR.GetKeyReleased(VK_UP))
		{
			MkFloat2 ss = twNode->GetTabBodySize();
			twNode->SetTabBodySize(MkFloat2(ss.x + 1, ss.y + 1));
		}
		if (MK_INPUT_MGR.GetKeyReleased(VK_DOWN))
		{
			MkFloat2 ss = twNode->GetTabBodySize();
			twNode->SetTabBodySize(MkFloat2(ss.x - 1, ss.y - 1));
		}
		if (MK_INPUT_MGR.GetKeyPushing('U'))
		{
			m_Offset.z += timeState.elapsed * 100.f;
		}
		if (MK_INPUT_MGR.GetKeyPushing('J'))
		{
			m_Offset.z -= timeState.elapsed * 100.f;
		}
		if (MK_INPUT_MGR.GetKeyPushing('I'))
		{
			m_Rotation += timeState.elapsed * MKDEF_PI;
		}
		if (MK_INPUT_MGR.GetKeyPushing('K'))
		{
			m_Rotation -= timeState.elapsed * MKDEF_PI;
		}
		if (MK_INPUT_MGR.GetKeyPushing('O'))
		{
			m_Scale += timeState.elapsed * 1.f;
		}
		if (MK_INPUT_MGR.GetKeyPushing('L'))
		{
			m_Scale -= timeState.elapsed * 1.f;
		}

		MkSceneNode* node = _GetTargetNode();
		if (node != NULL)
		{
			node->SetLocalPosition(m_Offset);
			node->SetLocalRotation(m_Rotation);
			node->SetLocalScale(m_Scale);
		}

		m_RootNode->UpdateAll();
		m_ScreenNode->UpdateAll();

		if (MK_INPUT_MGR.GetKeyReleased(VK_RETURN))
		{
			MkDataNode node;
			
			MkHashStr pkey = L"save";
			MK_PROF_MGR.Begin(pkey);

			m_Node01->GetChildNode(L"WinRoot")->Save(node);

			MK_PROF_MGR.End(pkey, true);
			MkStr sbuf;
			MK_PROF_MGR.GetEverageTimeStr(pkey, sbuf);
			MK_DEV_PANEL.MsgToLog(sbuf);

			node.SaveToText(L"test_scene.txt");
		}

		if (MK_INPUT_MGR.GetKeyReleased('N'))
		{
			MK_PAGE_MGR.ChangePageDirectly(L"RestorePage");
		}

		MK_DEV_PANEL.MsgToFreeboard(0, m_RootNode->GetNodeName().GetString());
		MK_DEV_PANEL.MsgToFreeboard(1, L"   (LP)" + MkStr(m_RootNode->GetLocalPosition()) + L" (LR)" + MkStr(m_RootNode->GetLocalRotation()) + L" (LS)" + MkStr(m_RootNode->GetLocalScale()));
		MK_DEV_PANEL.MsgToFreeboard(2, L"   (WP)" + MkStr(m_RootNode->GetWorldPosition()) + L" (WR)" + MkStr(m_RootNode->GetWorldRotation()) + L" (WS)" + MkStr(m_RootNode->GetWorldScale()));

		MK_DEV_PANEL.MsgToFreeboard(3, m_Node01->GetNodeName().GetString());
		MK_DEV_PANEL.MsgToFreeboard(4, L"   (LP)" + MkStr(m_Node01->GetLocalPosition()) + L" (LR)" + MkStr(m_Node01->GetLocalRotation()) + L" (LS)" + MkStr(m_Node01->GetLocalScale()));
		MK_DEV_PANEL.MsgToFreeboard(5, L"   (WP)" + MkStr(m_Node01->GetWorldPosition()) + L" (WR)" + MkStr(m_Node01->GetWorldRotation()) + L" (WS)" + MkStr(m_Node01->GetWorldScale()));

		MkSceneNode* alignNode = m_RootNode->GetChildNode(L"align node");
		MkSceneNode* txtNode = alignNode->GetChildNode(L"txt node");
		//MkSRect* textRect = txtNode->GetSRect(L"text");
		//const MkFloatRect& rrr = textRect->GetAABR();
		MK_DEV_PANEL.MsgToFreeboard(6, txtNode->GetNodeName().GetString());
		MK_DEV_PANEL.MsgToFreeboard(7, L"   (LP)" + MkStr(txtNode->GetLocalPosition()) + L" (LR)" + MkStr(txtNode->GetLocalRotation()) + L" (LS)" + MkStr(txtNode->GetLocalScale()));
		MK_DEV_PANEL.MsgToFreeboard(8, L"   (WP)" + MkStr(txtNode->GetWorldPosition()) + L" (WR)" + MkStr(txtNode->GetWorldRotation()) + L" (WS)" + MkStr(txtNode->GetWorldScale()));
		//MK_DEV_PANEL.MsgToFreeboard(6, m_Node02->GetNodeName().GetString());
		//MK_DEV_PANEL.MsgToFreeboard(7, L"   (LP)" + MkStr(m_Node02->GetLocalPosition()) + L" (LR)" + MkStr(m_Node02->GetLocalRotation()) + L" (LS)" + MkStr(m_Node02->GetLocalScale()));
		//MK_DEV_PANEL.MsgToFreeboard(8, L"   (WP)" + MkStr(m_Node02->GetWorldPosition()) + L" (WR)" + MkStr(m_Node02->GetWorldRotation()) + L" (WS)" + MkStr(m_Node02->GetWorldScale()));

		MK_DEV_PANEL.MsgToFreeboard(10, L"Target -> " + node->GetNodeName().GetString());
		MK_DEV_PANEL.MsgToFreeboard(11, L"  (P) " + MkStr(m_Offset));
		MK_DEV_PANEL.MsgToFreeboard(12, L"  (R) " + MkStr(m_Rotation));
		MK_DEV_PANEL.MsgToFreeboard(13, L"  (S) " + MkStr(m_Scale));

		MK_DEV_PANEL.MsgToFreeboard(15, L"Rect count : " + MkStr(m_RectCount));
	}

	virtual void Clear(MkDataNode* sharingNode = NULL)
	{
		MK_DELETE(m_RootNode);
		MK_DELETE(m_ScreenNode);
		
		MK_RENDERER.GetDrawQueue().Clear();
		//MK_TEXTURE_POOL.UnloadGroup(0);
	}

	TestPage(const MkHashStr& name) : MkBasePage(name)
	{
		m_RootNode = NULL;
		m_ScreenNode = NULL;
	}

	virtual ~TestPage() { Clear(); }

protected:

	MkSceneNode* _GetTargetNode(void)
	{
		MkSceneNode* node = NULL;
		switch (m_TargetNode)
		{
		case 0: node = m_RootNode; break;
		case 1: node = m_Node01; break;
		case 2: node = m_Node02; break;
		}
		return node;
	}

	void _SetTargetNode(int nodeIndex)
	{
		m_TargetNode = nodeIndex;
		MkSceneNode* node = _GetTargetNode();
		if (node != NULL)
		{
			m_Offset = node->GetLocalPosition();
			m_Rotation = node->GetLocalRotation();
			m_Scale = node->GetLocalScale();
		}
	}

protected:

	MkSceneNode* m_RootNode;
	MkBaseWindowNode* m_Node01;
	MkSceneNode* m_Node02;

	MkSceneNode* m_ScreenNode;

	MkTabWindowNode* twNode;

	int m_TargetNode;

	MkVec3 m_Offset;
	float m_Rotation;
	float m_Scale;

	unsigned int m_RectCount;
	MkUniformDice m_DiceX;
	MkUniformDice m_DiceY;
	MkUniformDice m_DiceT;
	MkUniformDice m_DiceA;
};

// RestorePage ¼±¾ð
class RestorePage : public MkBasePage
{
public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		MkInt2 currResolution = MK_DISPLAY_MGR.GetCurrentResolution();
		MkDrawStep* sceneStep = MK_RENDERER.GetDrawQueue().CreateStep(L"scene step", 0, MkRenderTarget::eTexture, 1, MkUInt2(currResolution.x, currResolution.y));

		m_RootNode = new MkBaseWindowNode(L"Root");
		sceneStep->AddSceneNode(m_RootNode);
		m_RootNode->CreateSRect(L"BG")->SetTexture(L"Image\\rohan_screenshot.png", L"");

		MkBaseTexturePtr sceneTexture;
		sceneStep->GetTargetTexture(0, sceneTexture);
		MK_WIN_EVENT_MGR.SetUp(sceneTexture);

		m_RootNode->UpdateAll();
		
		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		if (MK_INPUT_MGR.GetKeyReleased('R'))
		{
			MK_WIN_EVENT_MGR.ActivateWindow(L"10", true);
		}
		if (MK_INPUT_MGR.GetKeyReleased('T'))
		{
			MK_WIN_EVENT_MGR.DeactivateWindow(L"10");
		}

		// window ÀÌº¥Æ® Ã³¸®
		MK_WIN_EVENT_MGR.Update();

		m_RootNode->UpdateAll();
	}

	virtual void Clear(MkDataNode* sharingNode = NULL)
	{
		MK_WIN_EVENT_MGR.Clear();
		MK_DELETE(m_RootNode);
		
		MK_RENDERER.GetDrawQueue().Clear();
		//MK_TEXTURE_POOL.UnloadGroup(0);
	}

	RestorePage(const MkHashStr& name) : MkBasePage(name)
	{
		m_RootNode = NULL;
	}

	virtual ~RestorePage() { Clear(); }

protected:

	MkBaseWindowNode* m_RootNode;
};

class TestFramework : public MkRenderFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg)
	{
		MK_PAGE_MGR.SetUp(new MkBasePage(L"Root"));
		MK_PAGE_MGR.RegisterChildPage(L"Root", new TestPage(L"TestPage"));
		MK_PAGE_MGR.RegisterChildPage(L"Root", new RestorePage(L"RestorePage"));
		
		MK_PAGE_MGR.ChangePageDirectly(L"TestPage");
		
		return MkRenderFramework::SetUp(clientWidth, clientHeight, fullScreen, arg);
	}

	virtual ~TestFramework() {}
};

// TestApplication ¼±¾ð
class TestApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new TestFramework; }

public:
	TestApplication() : MkWin32Application() {}
	virtual ~TestApplication() {}
};

//------------------------------------------------------------------------------------------------//

// ¿£Æ®¸® Æ÷ÀÎÆ®¿¡¼­ÀÇ TestApplication ½ÇÇà
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"¶¼½º¶Ç", L"..\\FileRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

