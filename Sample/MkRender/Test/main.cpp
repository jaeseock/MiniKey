
//------------------------------------------------------------------------------------------------//
// single/multi-thread rendering application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkCheck.h"
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
#include "MkPA_MkWindowThemedNode.h"
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
		m_RootNode = new MkSceneNode(L"Root");

		MkSceneNode* mainNode = m_RootNode->CreateChildNode(L"Main");
		m_TargetNode = mainNode;

		MkSceneNode* subNode = m_RootNode->CreateChildNode(L"Sub");
		subNode->CreatePanel(L"P").SetTexture(L"Image\\s03.jpg");

		MkSceneNode* bgNode = mainNode->CreateChildNode(L"BG");
		bgNode->CreatePanel(L"P").SetTexture(L"Image\\rohan_screenshot.png");
		bgNode->SetLocalDepth(10000.f);
		
		MkPanel& ip = mainNode->CreatePanel(L"ImgTest");
		ip.SetSmallerSourceOp(MkPanel::eAttachToLeftTop);
		ip.SetBiggerSourceOp(MkPanel::eCutSource);
		ip.SetPanelSize(MkFloat2(450.f, 250.f));
		ip.SetLocalDepth(2.f);
		ip.SetTexture(L"Image\\s01.jpg");

		MkPanel& tp = mainNode->CreatePanel(L"TextTest");
		tp.SetSmallerSourceOp(MkPanel::eAttachToLeftTop);
		tp.SetBiggerSourceOp(MkPanel::eCutSource);
		tp.SetPanelSize(MkFloat2(110.f, 250.f));
		tp.SetLocalDepth(1.f);
		tp.SetTextNode(L"_Sample", true);

		MkPanel& mp = mainNode->CreatePanel(L"MaskingTest", subNode, MkInt2(200, 150));
		mp.SetLocalPosition(MkFloat2(120.f, 50.f));

		MkWindowThemedNode* trNode = MkWindowThemedNode::CreateChildNode(mainNode, L"ThemeRoot");
		trNode->SetLocalPosition(MkFloat2(500.f, 100.f));
		trNode->SetLocalDepth(2.f);
		trNode->SetTheme(L"Default");
		trNode->SetComponent(MkWindowThemeData::eCT_DefaultBox);
		trNode->SetClientSize(MkFloat2(300.f, 200.f));
		trNode->SetShadowEnable(true);
		trNode->SetAcceptInput(true);

		MkWindowThemedNode* tsNode = MkWindowThemedNode::CreateChildNode(trNode, L"ThemeSub");
		tsNode->SetLocalDepth(-1.f);
		tsNode->SetTheme(L"Default");
		tsNode->SetComponent(MkWindowThemeData::eCT_GuideBtn);
		tsNode->SetClientSize(MkFloat2(80.f, 50.f));
		tsNode->SetAcceptInput(true);
		tsNode->SetRestrictedWithinParentClient(true);

		tsNode->CreateTag(L"Tag");
		MkWindowTagNode::TagInfo tagInfo;
		tagInfo.iconPath = L"Default\\theme_default.png";
		tagInfo.iconSubsetOrSequenceName = L"IcnWinDef";
		tagInfo.textName = L"WindowTitle";
		tagInfo.lengthOfBetweenIconAndText = 4.f;
		tagInfo.alignmentPosition = eRAP_LeftTop;
		tagInfo.alignmentOffset = MkFloat2::Zero;
		tsNode->SetTagInfo(L"Tag", tagInfo);
		
		MkDrawSceneNodeStep* ds = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep(L"Final");
		ds->SetSceneNode(mainNode);

		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		MkInt2 mp = MK_INPUT_MGR.GetAbsoluteMousePosition(true);
		MK_DEV_PANEL.MsgToFreeboard(0, L"Cursor : " + MkStr(mp));

		if (m_RootNode != NULL)
		{
			MkSceneNode* mainNode = m_RootNode->GetChildNode(L"Main");
			if (mainNode != NULL)
			{
				if (MK_INPUT_MGR.GetMouseLeftButtonReleased())
				{
					MkBitField32 attr;
					attr.Set(MkVisualPatternNode::eAT_AcceptInput);
					MkArray<MkPanel*> buffer;
					if (mainNode->PickPanel(buffer, MkFloat2(static_cast<float>(mp.x), static_cast<float>(mp.y)), 0., attr))
					{
						m_TargetNode = buffer[0]->GetParentNode();
						MK_DEV_PANEL.MsgToLog(L"Pick : " + m_TargetNode->GetNodeName().GetString());
					}
				}
			}

			if (m_TargetNode != NULL)
			{
				const float movement = static_cast<float>(timeState.elapsed) * 300.f;

				MkFloat2 localPos = m_TargetNode->GetLocalPosition();
				if (MK_INPUT_MGR.GetKeyPushing(L'A'))
				{
					localPos.x -= movement;
					m_TargetNode->SetLocalPosition(localPos);
				}
				if (MK_INPUT_MGR.GetKeyPushing(L'D'))
				{
					localPos.x += movement;
					m_TargetNode->SetLocalPosition(localPos);
				}
				if (MK_INPUT_MGR.GetKeyPushing(L'W'))
				{
					localPos.y += movement;
					m_TargetNode->SetLocalPosition(localPos);
				}
				if (MK_INPUT_MGR.GetKeyPushing(L'S'))
				{
					localPos.y -= movement;
					m_TargetNode->SetLocalPosition(localPos);
				}

				float localRot = m_TargetNode->GetLocalRotation();
				const float rotVel = static_cast<float>(timeState.elapsed) * MKDEF_PI * 0.5f;
				if (MK_INPUT_MGR.GetKeyPushing(L'Q'))
				{
					localRot -= rotVel;
					m_TargetNode->SetLocalRotation(localRot);
				}
				if (MK_INPUT_MGR.GetKeyPushing(L'E'))
				{
					localRot += rotVel;
					m_TargetNode->SetLocalRotation(localRot);
				}
				
				float localScale = m_TargetNode->GetLocalScale();
				const float scaleVel = static_cast<float>(timeState.elapsed);
				if (MK_INPUT_MGR.GetKeyPushing(L'Z'))
				{
					localScale -= scaleVel;
					m_TargetNode->SetLocalScale(localScale);
				}
				if (MK_INPUT_MGR.GetKeyPushing(L'X'))
				{
					localScale += scaleVel;
					m_TargetNode->SetLocalScale(localScale);
				}
				if (MK_INPUT_MGR.GetKeyPushing(L'C'))
				{
					localScale = 1.f;
					m_TargetNode->SetLocalScale(localScale);
				}
				
				if (m_TargetNode->GetNodeName().GetString() == L"Main")
				{
					MkPanel* tp = m_TargetNode->GetPanel(L"TextTest");
					MkFloat2 psp = tp->GetPixelScrollPosition();
					if (MK_INPUT_MGR.GetKeyPushing(VK_LEFT))
					{
						psp.x -= movement;
						tp->SetPixelScrollPosition(psp);
					}
					if (MK_INPUT_MGR.GetKeyPushing(VK_RIGHT))
					{
						psp.x += movement;
						tp->SetPixelScrollPosition(psp);
					}
					if (MK_INPUT_MGR.GetKeyPushing(VK_UP))
					{
						psp.y -= movement;
						tp->SetPixelScrollPosition(psp);
					}
					if (MK_INPUT_MGR.GetKeyPushing(VK_DOWN))
					{
						psp.y += movement;
						tp->SetPixelScrollPosition(psp);
					}

					if (MK_INPUT_MGR.GetKeyReleased(L'1'))
					{
						MkPanel* ip = m_TargetNode->GetPanel(L"ImgTest");
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

					if (MK_INPUT_MGR.GetKeyReleased(L'2'))
					{
						MkPanel* tp = m_TargetNode->GetPanel(L"TextTest");
						MkTextNode* textNode = tp->GetTextNodePtr();
						MkTextNode* targetNode = textNode->GetChildNode(L"1st")->GetChildNode(L"Sub list")->GetChildNode(L"이번이구나");
						targetNode->SetFontStyle(L"Desc:Notice");
						targetNode->SetText(L"- 이걸로 바꿨음당 ( ㅡ_-)r");
						tp->BuildAndUpdateTextCache();
					}
				}
				else if (m_TargetNode->GetNodeType() >= ePA_SNT_WindowThemedNode)
				{
					MkWindowThemedNode* thNode = dynamic_cast<MkWindowThemedNode*>(m_TargetNode);
					MkFloat2 cs = thNode->GetClientRect().size;
					if (MK_INPUT_MGR.GetKeyPushing(VK_LEFT))
					{
						cs.x -= movement;
					}
					if (MK_INPUT_MGR.GetKeyPushing(VK_RIGHT))
					{
						cs.x += movement;
					}
					if (MK_INPUT_MGR.GetKeyPushing(VK_UP))
					{
						cs.y += movement;
					}
					if (MK_INPUT_MGR.GetKeyPushing(VK_DOWN))
					{
						cs.y -= movement;
					}
					thNode->SetClientSize(cs);

					if (MK_INPUT_MGR.GetKeyReleased(L'1'))
					{
						thNode->SetShadowEnable(!thNode->GetShadowEnable());
					}

					if (MK_INPUT_MGR.GetKeyReleased(L'2'))
					{
						int comp = static_cast<int>(thNode->GetComponent()) + 1;
						if (comp >= static_cast<int>(MkWindowThemeData::eCT_Max))
						{
							comp = 1;
						}
						thNode->SetComponent(static_cast<MkWindowThemeData::eComponentType>(comp));

						MK_DEV_PANEL.MsgToLog(L"component : " + MkWindowThemeData::ComponentTypeName[comp].GetString());
					}

					if (MK_INPUT_MGR.GetKeyReleased(L'3'))
					{
						MkWindowThemeFormData::eFormType ft = thNode->GetFormType();
						int maxPos = 0;
						if (ft == MkWindowThemeFormData::eFT_DualUnit)
						{
							maxPos = 2;
						}
						else if (ft == MkWindowThemeFormData::eFT_QuadUnit)
						{
							maxPos = 4;
						}
						if (maxPos > 0)
						{
							int fp = static_cast<int>(thNode->GetFormPosition()) + 1;
							if (fp >= maxPos)
							{
								fp = 0;
							}
							thNode->SetFormPosition(static_cast<MkWindowThemeFormData::ePosition>(fp));
							MK_DEV_PANEL.MsgToLog(L"form pos : " + MkStr(fp));
						}
					}

					if (MK_INPUT_MGR.GetKeyReleased(L'4'))
					{
						_IncAP();
						thNode->SetAlignmentPosition(ap, MkFloat2::Zero);
						thNode->UpdateAlignmentPosition();
					}
				}

				MK_DEV_PANEL.MsgToFreeboard(1, L"Target node : " + m_TargetNode->GetNodeName().GetString());
			}

			if (m_RootNode != NULL)
			{
				m_RootNode->Update(timeState.fullTime);
			}
			
			//MK_DEV_PANEL.MsgToFreeboard(0, L"HorizontalChange : " + hcn.GetString());
		}
	}

	virtual void Clear(MkDataNode* sharingNode = NULL)
	{
		MK_DELETE(m_RootNode);
	}

	TestPage(const MkHashStr& name) : MkBasePage(name)
	{
		m_RootNode = NULL;
		m_TargetNode = NULL;
		si = 0;
		ap = eRAP_NonePosition;
	}

	virtual ~TestPage() { Clear(); }

protected:

	void _IncAP(void)
	{
		switch (ap)
		{
		case eRAP_NonePosition: ap = eRAP_LMostOver; break;

		case eRAP_LMostOver: ap = eRAP_LMostTop; break;
		case eRAP_LMostTop: ap = eRAP_LMostCenter; break;
		case eRAP_LMostCenter: ap = eRAP_LMostBottom; break;
		case eRAP_LMostBottom: ap = eRAP_LMostUnder; break;
		case eRAP_LMostUnder: ap = eRAP_LeftOver; break;

		case eRAP_LeftOver: ap = eRAP_LeftTop; break;
		case eRAP_LeftTop: ap = eRAP_LeftCenter; break;
		case eRAP_LeftCenter: ap = eRAP_LeftBottom; break;
		case eRAP_LeftBottom: ap = eRAP_LeftUnder; break;
		case eRAP_LeftUnder: ap = eRAP_MiddleOver; break;

		case eRAP_MiddleOver: ap = eRAP_MiddleTop; break;
		case eRAP_MiddleTop: ap = eRAP_MiddleCenter; break;
		case eRAP_MiddleCenter: ap = eRAP_MiddleBottom; break;
		case eRAP_MiddleBottom: ap = eRAP_MiddleUnder; break;
		case eRAP_MiddleUnder: ap = eRAP_RightOver; break;

		case eRAP_RightOver: ap = eRAP_RightTop; break;
		case eRAP_RightTop: ap = eRAP_RightCenter; break;
		case eRAP_RightCenter: ap = eRAP_RightBottom; break;
		case eRAP_RightBottom: ap = eRAP_RightUnder; break;
		case eRAP_RightUnder: ap = eRAP_RMostOver; break;

		case eRAP_RMostOver: ap = eRAP_RMostTop; break;
		case eRAP_RMostTop: ap = eRAP_RMostCenter; break;
		case eRAP_RMostCenter: ap = eRAP_RMostBottom; break;
		case eRAP_RMostBottom: ap = eRAP_RMostUnder; break;
		case eRAP_RMostUnder: ap = eRAP_LMostOver; break;
		}
	}

protected:

	MkSceneNode* m_RootNode;
	MkSceneNode* m_TargetNode;
	unsigned int si;
	eRectAlignmentPosition ap;
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

