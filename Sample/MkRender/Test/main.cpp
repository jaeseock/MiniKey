
//------------------------------------------------------------------------------------------------//
// single/multi-thread rendering application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkPageManager.h"
#include "MkCore_MkInputManager.h"
#include "MkCore_MkTimeState.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkDataNode.h"

#include "MkS2D_MkRenderFramework.h"

#include "MkCore_MkUniformDice.h"

#include "MkS2D_MkTexturePool.h"
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkRenderer.h"

#include "MkS2D_MkDecoStr.h"

#include "MkS2D_MkSceneNode.h"
#include "MkS2D_MkBaseWindowNode.h"
#include "MkS2D_MkListButtonNode.h"
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
		MK_TEXTURE_POOL.LoadBitmapTexture(L"Image\\s00.png", 0);
		MK_TEXTURE_POOL.LoadBitmapTexture(L"Image\\s01.jpg", 0);
		MK_TEXTURE_POOL.LoadBitmapTexture(L"Image\\s02.jpg", 0);
		MK_TEXTURE_POOL.LoadBitmapTexture(L"Image\\s03.jpg", 0);

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
		winDesc.SetStandardDesc(L"Default", true, MkFloat2(150.f, 100.f));
		//winDesc.SetStandardDesc(L"Default", true, L"Image\\s02.jpg", L"");
		//winDesc.SetStandardDesc(L"Default", false);
		//winDesc.SetStandardDesc(L"Default", false, MkFloat2(150.f, 100.f));
		//winDesc.SetStandardDesc(L"Default", false, L"Image\\s02.jpg", L"");
		MkBaseWindowNode* titleWin = m_Node01->CreateBasicWindow(L"WinRoot", winDesc);
		
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

		if (MK_INPUT_MGR.GetKeyPushing(VK_LEFT))
		{
			m_Offset.x -= timeState.elapsed * 300.f;
		}
		if (MK_INPUT_MGR.GetKeyPushing(VK_RIGHT))
		{
			m_Offset.x += timeState.elapsed * 300.f;
		}
		if (MK_INPUT_MGR.GetKeyPushing(VK_UP))
		{
			m_Offset.y += timeState.elapsed * 300.f;
		}
		if (MK_INPUT_MGR.GetKeyPushing(VK_DOWN))
		{
			m_Offset.y -= timeState.elapsed * 300.f;
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

		m_RootNode->Update();
		m_ScreenNode->Update();

		if (MK_INPUT_MGR.GetKeyReleased(VK_RETURN))
		{
			MkDataNode node;
			m_Node01->GetChildNode(L"WinRoot")->Save(node);
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

	virtual void Clear(void)
	{
		MK_DELETE(m_RootNode);
		MK_DELETE(m_ScreenNode);
		
		MK_RENDERER.GetDrawQueue().Clear();
		MK_TEXTURE_POOL.UnloadGroup(0);
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
		m_RootNode = new MkBaseWindowNode(L"Root");
		MK_RENDERER.GetDrawQueue().CreateStep(L"scene step", 0)->AddSceneNode(m_RootNode);

		MK_WIN_EVENT_MGR.SetUp(MK_RENDERER.GetDrawQueue().CreateStep(L"window step", 1));

		MkDataNode node;
		if (node.Load(L"test_scene.txt"))
		{
			MkUniformDice diceX, diceY;
			diceX.SetMinMax(0, 800);
			diceX.SetSeed(1234);
			diceY.SetMinMax(200, 700);
			diceY.SetSeed(5678);

			for (unsigned int i=0; i<15; ++i)
			{
				MkBaseWindowNode* winNode = new MkBaseWindowNode(MkStr(i));
				winNode->Load(node);
				winNode->SetLocalPosition(MkFloat2(static_cast<float>(diceX.GenerateRandomNumber()), static_cast<float>(diceY.GenerateRandomNumber())));
				
				MkSRect* nameTag = winNode->CreateSRect(L"Name");
				nameTag->SetDecoString(winNode->GetNodeName().GetString());
				nameTag->SetLocalPosition(MkFloat2(6.f, 2.f));
				nameTag->SetLocalDepth(-0.002f);

				//winNode->SetPresetThemeName(L"Default");

				MK_WIN_EVENT_MGR.RegisterWindow(winNode, true);
			}
		}

		MkBaseWindowNode::BasicPresetWindowDesc winDesc;
		winDesc.SetStandardDesc(L"Default", true, MkFloat2(250.f, 200.f));
		winDesc.hasCancelButton = false;
		winDesc.hasOKButton = false;
		MkBaseWindowNode* testWin = m_RootNode->CreateBasicWindow(L"test", winDesc);
		m_RootNode->DetachChildNode(L"test");
		testWin->SetLocalPosition(MkFloat2(200.f, 450.f));

		MkListButtonNode* lbNode = new MkListButtonNode(L"LB");
		lbNode->CreateListTypeButton(L"Default", MkFloat2(150.f, 20.f), MkListButtonNode::eSeletionRoot, MkListButtonNode::eRightside);
		MkListButtonNode::ItemTagInfo tagInfo;
		tagInfo.iconPath = L"Default\\system_default.png";
		tagInfo.iconSubset = L"WinIconSP";
		tagInfo.caption = L"Å×½ºÆ® ÇÕ´Ï´ç!!!";
		lbNode->SetItemTag(tagInfo);
		lbNode->SetLocalPosition(MkVec3(30.f, 50.f, -0.001f));

		tagInfo.caption = L"²¿ºØ 0";
		lbNode->AddItem(L"0", tagInfo, false);
		tagInfo.caption = L"²¿ºØ 1";
		lbNode->AddItem(L"1", tagInfo, false);
		tagInfo.caption = L"²¿ºØ 2";
		lbNode->AddItem(L"2", tagInfo, true);
		tagInfo.caption = L"²¿ºØ 3";
		lbNode->AddItem(L"3", tagInfo, false);

		testWin->GetChildNode(L"Background")->AttachChildNode(lbNode);
		
		MK_WIN_EVENT_MGR.RegisterWindow(testWin, true);
		
		//testWin->SetPresetThemeName(L"SolidLB");
		//m_WindowNode1->SetPresetComponentBodySize(MkFloat2(100.f, 100.f));

		m_RootNode->Update();
		
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

		m_RootNode->Update();
	}

	virtual void Clear(void)
	{
		MK_WIN_EVENT_MGR.Clear();
		MK_DELETE(m_RootNode);
		
		MK_RENDERER.GetDrawQueue().Clear();
		MK_TEXTURE_POOL.UnloadGroup(0);
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

