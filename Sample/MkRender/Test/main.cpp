
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
#include "MkCore_MkKeywordFilter.h"
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
#include "MkPA_MkWindowTagNode.h"
#include "MkPA_MkWindowThemedNode.h"
#include "MkPA_MkWindowBaseNode.h"
#include "MkPA_MkWindowManagerNode.h"
#include "MkPA_MkTitleBarControlNode.h"
#include "MkPA_MkBodyFrameControlNode.h"
#include "MkPA_MkCheckBoxControlNode.h"
#include "MkPA_MkScrollBarControlNode.h"
#include "MkPA_MkScenePortalNode.h"
#include "MkPA_MkEditBoxControlNode.h"
#include "MkPA_MkSliderControlNode.h"
#include "MkPA_MkListBoxControlNode.h"
#include "MkPA_MkDropDownListControlNode.h"
#include "MkPA_MkProgressBarNode.h"

#include "MkPA_MkDrawSceneNodeStep.h"
#include "MkPA_MkStaticResourceContainer.h"

#include "MkCore_MkDevPanel.h"

#include "MkPA_MkWindowFactory.h"
#include "MkCore_MkTimeCounter.h"

//------------------------------------------------------------------------------------------------//

// TestPage 선언
class TestPage : public MkBasePage
{
public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		m_PageNode = new MkSceneNode(L"<Page>");
		MkSceneNode* rootNode = m_PageNode->CreateChildNode(L"<Root>");
		MkSceneNode* mainNode = rootNode->CreateChildNode(L"Main");
		//m_TargetNode = mainNode;

		MkSceneNode* bgNode = mainNode->CreateChildNode(L"BG");
		bgNode->CreatePanel(L"P").SetTexture(L"Image\\rohan_screenshot.png");
		bgNode->SetLocalDepth(10000.f);

		MkPanel& ip = mainNode->CreatePanel(L"ImgTest");
		ip.SetSmallerSourceOp(MkPanel::eAttachToLeftTop);
		ip.SetBiggerSourceOp(MkPanel::eCutSource);
		ip.SetPanelSize(MkFloat2(450.f, 250.f));
		ip.SetLocalDepth(9999.f);
		ip.SetTexture(L"Image\\s01.jpg");

		MkPanel& tp = mainNode->CreatePanel(L"TextTest");
		tp.SetSmallerSourceOp(MkPanel::eAttachToLeftTop);
		tp.SetBiggerSourceOp(MkPanel::eCutSource);
		tp.SetPanelSize(MkFloat2(110.f, 250.f));
		tp.SetLocalDepth(9998.f);
		MkArray<MkHashStr> textName;
		textName.PushBack(L"_Sample");
		tp.SetTextNode(textName, true);

		//-----------------------------------------------------------//
		m_RectDummyNode = mainNode->CreateChildNode(L"RectDummy");
		m_RectDummyNode->SetLocalDepth(9997.f);
		m_DummyIndex = 0;
		//-----------------------------------------------------------//
		
		// window mgr
		MkWindowManagerNode* winMgrNode = MkWindowManagerNode::CreateChildNode(rootNode, L"WinMgr");
		winMgrNode->SetDepthBandwidth(1000.f);

		// window A : title bar
		MkTitleBarControlNode* titleBar = MkTitleBarControlNode::CreateChildNode(NULL, L"Win(1)");
		titleBar->SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 300.f, true);
		titleBar->SetIcon(MkWindowThemeData::eIT_Notice);
		titleBar->SetCaption(L"(1) 윈도우 안의 윈도우 + 에디트박스", eRAP_LeftCenter);
		winMgrNode->AttachWindow(titleBar);
		winMgrNode->ActivateWindow(L"Win(1)");

		titleBar->SetLocalPosition(MkFloat2(400.f, 600.f));

		// window A : body frame
		MkBodyFrameControlNode* bodyFrame = MkBodyFrameControlNode::CreateChildNode(titleBar, L"BodyFrame");
		bodyFrame->SetBodyFrame
			(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(350.f, 250.f));

		MkWindowThemedNode* bodyClient = MkWindowThemedNode::CreateChildNode(bodyFrame, L"BodyClient");
		bodyClient->SetThemeName(MkWindowThemeData::DefaultThemeName);
		bodyClient->SetComponentType(MkWindowThemeData::eCT_StaticBox);
		bodyClient->SetClientSize(MkFloat2(330.f, 205.f));
		bodyClient->SetFormState(MkWindowThemeFormData::eS_Default);
		bodyClient->SetAlignmentPosition(eRAP_MiddleBottom);
		bodyClient->SetAlignmentOffset(MkFloat2(0.f, 10.f));
		bodyClient->SetLocalDepth(-1.f);

		// window A : scene portal
		MkScenePortalNode* scenePortal = MkScenePortalNode::CreateChildNode(bodyClient, L"ScenePortal");
		MkWindowManagerNode* swinMgrNode = scenePortal->CreateScenePortal(MkWindowThemeData::DefaultThemeName, MkFloat2(250.f, 180.f));
		scenePortal->SetAlignmentPosition(eRAP_LeftBottom);
		scenePortal->SetAlignmentOffset(MkFloat2(50.f, 10.f));
		scenePortal->SetLocalDepth(-1.f);

		// window A : MkSliderControlNode
		MkSliderControlNode* vSliderNode = MkSliderControlNode::CreateChildNode(bodyClient, L"Slider");
		vSliderNode->SetVerticalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 150.f, 5, 15, 12);
		vSliderNode->SetAlignmentPosition(eRAP_LeftTop);
		vSliderNode->SetAlignmentOffset(MkFloat2(10.f, -30.f));
		vSliderNode->SetLocalDepth(-1.f);

		// window B : title bar
		titleBar = MkTitleBarControlNode::CreateChildNode(NULL, L"Win(2)");
		titleBar->SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 300.f, true);
		titleBar->SetIcon(MkWindowThemeData::eIT_Notice);
		titleBar->SetCaption(L"(2) 코레가 타이틀데스B", eRAP_LeftCenter);
		winMgrNode->AttachWindow(titleBar);
		winMgrNode->ActivateWindow(L"Win(2)");

		titleBar->SetLocalPosition(MkFloat2(200.f, 500.f));

		// window B : body frame
		bodyFrame = MkBodyFrameControlNode::CreateChildNode(titleBar, L"BodyFrameB");
		bodyFrame->SetBodyFrame
			(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(350.f, 250.f));

		MkListBoxControlNode* lbNode = MkListBoxControlNode::CreateChildNode(bodyFrame, L"ListBox");
		lbNode->SetListBox(MkWindowThemeData::DefaultThemeName, 6, 200.f, MkWindowThemeData::eFT_Small);
		lbNode->SetAlignmentPosition(eRAP_LeftTop);
		lbNode->SetAlignmentOffset(MkFloat2(10.f, -30.f));
		lbNode->SetLocalDepth(-1.f);
		lbNode->AddItem(L"가나다", L"가나다");
		lbNode->AddItem(L"가고파", L"가고파");
		lbNode->AddItem(L"도나도나", L"도나도나");
		lbNode->AddItem(L"웁스", L"웁스");
		lbNode->AddItem(L"도나", L"도나");
		lbNode->AddItem(L"aaa", L"aaa");
		lbNode->AddItem(L"bbb", L"bbb");
		lbNode->AddItem(L"ccc", L"ccc");
		lbNode->SortItemSequenceInAscendingOrder();

		MkProgressBarNode* pbNode = MkProgressBarNode::CreateChildNode(bodyFrame, L"ProgBar");
		pbNode->SetProgressBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Medium, 250.f, 50, 200, MkProgressBarNode::eSPM_Percentage);
		pbNode->SetAlignmentPosition(eRAP_LeftTop);
		pbNode->SetAlignmentOffset(MkFloat2(10.f, -160.f));
		pbNode->SetLocalDepth(-1.f);

		// window C : title bar
		titleBar = MkTitleBarControlNode::CreateChildNode(NULL, L"Win(3)");
		titleBar->SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 300.f, true);
		titleBar->SetIcon(MkWindowThemeData::eIT_Notice);
		titleBar->SetCaption(L"(3) 다국어 및 깐츄롤 + 2D 라인 그리기", eRAP_LeftCenter);
		winMgrNode->AttachWindow(titleBar);
		winMgrNode->ActivateWindow(L"Win(3)");

		titleBar->SetLocalPosition(MkFloat2(100.f, 400.f));

		// window C : body frame
		bodyFrame = MkBodyFrameControlNode::CreateChildNode(titleBar, L"BodyFrameC");
		bodyFrame->SetBodyFrame
			(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(350.f, 250.f));

		MkCheckBoxControlNode* checkBox = MkCheckBoxControlNode::CreateChildNode(bodyFrame, L"CheckBox");
		checkBox->SetCheckBox(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, L"체크박스 샘플", true);
		checkBox->SetLocalDepth(-1.f);
		checkBox->SetAlignmentPosition(eRAP_LeftTop);
		checkBox->SetAlignmentOffset(MkFloat2(10.f, -30.f));

		MkDropDownListControlNode* ddList = MkDropDownListControlNode::CreateChildNode(bodyFrame, L"DropDown");
		ddList->SetDropDownList(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 300.f, 6);
		ddList->SetLocalDepth(-2.f);
		ddList->SetAlignmentPosition(eRAP_LeftTop);
		ddList->SetAlignmentOffset(MkFloat2(10.f, -55.f));

		MkStr itemText;
		itemText.ReadTextFile(L"다국어.txt");
		MkArray<MkStr> itemTokens;
		itemText.Tokenize(itemTokens, MkStr::LF);
		MK_INDEXING_LOOP(itemTokens, i)
		{
			MkStr& currToken = itemTokens[i];
			currToken.RemoveFrontSideBlank();
			currToken.RemoveRearSideBlank();

			ddList->AddItem(MkStr(i), currToken);
		}
		ddList->SetTargetItemKey(L"0");

		MkWindowFactory wndFactory;
		MkWindowBaseNode* btnInst = wndFactory.CreateNormalButtonNode(L"BtnNormal", L"개행하는 텍스트?\n버튼 사이즈 자동 조정");
		btnInst->SetAlignmentPosition(eRAP_LeftTop);
		btnInst->SetAlignmentOffset(MkFloat2(10.f, -90.f));
		bodyFrame->AttachChildNode(btnInst);
		MkWindowBaseNode* btnOk = wndFactory.CreateButtonTypeNode(L"BtnSel Y", MkWindowThemeData::eCT_YellowSelBtn, L"노랑 선택 버튼");
		btnOk->SetAlignmentPosition(eRAP_LeftTop);
		btnOk->SetAlignmentOffset(MkFloat2(10.f, -90.f - 60.f));
		bodyFrame->AttachChildNode(btnOk);
		MkWindowBaseNode* btnCancel = wndFactory.CreateButtonTypeNode(L"BtnSel B", MkWindowThemeData::eCT_BlueSelBtn, L"파랑 선택 버튼");
		btnCancel->SetAlignmentPosition(eRAP_LeftTop);
		btnCancel->SetAlignmentOffset(MkFloat2(10.f, -90.f - 90.f));
		bodyFrame->AttachChildNode(btnCancel);
		MkWindowBaseNode* btnExtra = wndFactory.CreateButtonTypeNode(L"BtnSel R", MkWindowThemeData::eCT_RedOutlineSelBtn, L"가이드라인 선택 버튼");
		btnExtra->SetAlignmentPosition(eRAP_LeftTop);
		btnExtra->SetAlignmentOffset(MkFloat2(10.f, -90.f - 120.f));
		bodyFrame->AttachChildNode(btnExtra);

		/*
		// control box : dummy
		MkWindowBaseNode* controlWin = __TSI_SceneNodeDerivedInstanceOp<_ControlBoxNode>::Alloc(NULL, L"ControlDummy");
		winMgrNode->AttachChildNode(controlWin);
		winMgrNode->ActivateWindow(L"ControlDummy", false);
		controlWin->SetLocalPosition(MkFloat2(850.f, 720.f));
		controlWin->SetLocalDepth(10.f); // tmp

		// control box : buttons
		btnInst = wndFactory.CreateButtonTypeNode(L"Btn1", MkWindowThemeData::eCT_NormalBtn, L"(1)번 윈도우 토글");
		btnInst->SetClientSize(MkFloat2(160.f, 20.f));
		controlWin->AttachChildNode(btnInst);

		btnInst = wndFactory.CreateButtonTypeNode(L"Btn2", MkWindowThemeData::eCT_NormalBtn, L"(2)번 윈도우 토글");
		btnInst->SetClientSize(MkFloat2(160.f, 20.f));
		btnInst->SetLocalPosition(MkFloat2(0.f, -40.f));
		controlWin->AttachChildNode(btnInst);

		btnInst = wndFactory.CreateButtonTypeNode(L"Btn3", MkWindowThemeData::eCT_NormalBtn, L"(3)번 윈도우 토글");
		btnInst->SetClientSize(MkFloat2(160.f, 20.f));
		btnInst->SetLocalPosition(MkFloat2(0.f, -80.f));
		controlWin->AttachChildNode(btnInst);

		btnInst = wndFactory.CreateButtonTypeNode(L"Btn4", MkWindowThemeData::eCT_OKBtn, L"메세지 박스(모달) 토글");
		btnInst->SetClientSize(MkFloat2(160.f, 20.f));
		btnInst->SetLocalPosition(MkFloat2(0.f, -120.f));
		controlWin->AttachChildNode(btnInst);

		btnInst = wndFactory.CreateButtonTypeNode(L"Btn5", MkWindowThemeData::eCT_CheckBoxBtn, L"내부 브라우저 토글");
		btnInst->SetClientSize(MkFloat2(160.f, 20.f));
		btnInst->SetLocalPosition(MkFloat2(0.f, -160.f));
		controlWin->AttachChildNode(btnInst);
		*/

		// message box
		MkWindowBaseNode* msgBoxNode = wndFactory.CreateMessageBox
			(L"MsgBox", L"캡션입니당", L"너무너무 잘생겨서\n어케 할지를 모르겠으용\n~(-_-)~", NULL, MkWindowFactory::eMBT_Warning, MkWindowFactory::eMBB_OkCancel);
		msgBoxNode->SetLocalPosition(MkFloat2(500.f, 300.f));
		winMgrNode->AttachWindow(msgBoxNode);

		//--------------------------------------------------//
		// sub window mgr
		
		swinMgrNode->SetDepthBandwidth(1000.f);
		
		// window Sub : title bar
		MkTitleBarControlNode* stitleBar = MkTitleBarControlNode::CreateChildNode(NULL, L"STitleBar");
		stitleBar->SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 0.f, true);
		stitleBar->SetIcon(MkWindowThemeData::eIT_Notice);
		stitleBar->SetCaption(L"서브 윈도우", eRAP_LeftCenter);
		swinMgrNode->AttachWindow(stitleBar);
		swinMgrNode->ActivateWindow(L"STitleBar");
		stitleBar->SetLocalPosition(MkFloat2(0.f, 180.f - 18.f));
		stitleBar->SetLocalDepth(10.f); // tmp

		// window Sub : body frame
		MkBodyFrameControlNode* sbodyFrame = MkBodyFrameControlNode::CreateChildNode(stitleBar, L"SBodyFrame");
		sbodyFrame->SetBodyFrame
			(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_NoticeBox, false, MkBodyFrameControlNode::eHT_UnderParentWindow, MkFloat2(244.f, 300.f));

		// window Sub : ok btn
		MkWindowBaseNode* okbtnNode = MkWindowBaseNode::CreateChildNode(sbodyFrame, L"SubOKBtn");
		okbtnNode->SetThemeName(MkWindowThemeData::DefaultThemeName);
		okbtnNode->SetComponentType(MkWindowThemeData::eCT_OKBtn);
		okbtnNode->SetClientSize(MkFloat2(100.f, 20.f));
		okbtnNode->SetFormState(MkWindowThemeFormData::eS_Default);
		okbtnNode->SetAlignmentPosition(eRAP_LeftTop);
		okbtnNode->SetAlignmentOffset(MkFloat2(10.f, -10.f));
		okbtnNode->SetLocalDepth(-1.f);

		// window Sub : cancel btn
		MkWindowBaseNode* ccbtnNode = MkWindowBaseNode::CreateChildNode(sbodyFrame, L"SubCCBtn");
		ccbtnNode->SetThemeName(MkWindowThemeData::DefaultThemeName);
		ccbtnNode->SetComponentType(MkWindowThemeData::eCT_CancelBtn);
		ccbtnNode->SetClientSize(MkFloat2(100.f, 20.f));
		ccbtnNode->SetFormState(MkWindowThemeFormData::eS_Default);
		ccbtnNode->SetAlignmentPosition(eRAP_LeftTop);
		ccbtnNode->SetAlignmentOffset(MkFloat2(120.f, -10.f));
		ccbtnNode->SetLocalDepth(-1.f);

		// window Sub : MkEditBoxControlNode
		MkEditBoxControlNode* ebNode = MkEditBoxControlNode::CreateChildNode(sbodyFrame, L"EB");
		ebNode->SetSingleLineEditBox(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 210.f, L"아기 다리", true);
		ebNode->SetAlignmentPosition(eRAP_LeftTop);
		ebNode->SetAlignmentOffset(MkFloat2(10.f, -50.f));
		ebNode->SetLocalDepth(-1.f);

		// window Sub : MkSliderControlNode
		MkSliderControlNode* hSliderNode = MkSliderControlNode::CreateChildNode(sbodyFrame, L"Slider");
		hSliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 210.f, 10, 90, 20);
		hSliderNode->SetAlignmentPosition(eRAP_LeftTop);
		hSliderNode->SetAlignmentOffset(MkFloat2(10.f, -100.f));
		hSliderNode->SetLocalDepth(-1.f);

		//--------------------------------------------------//
		// line

		MkLineShape& lineShape0 = bodyFrame->CreateLine(L"test red line");
		lineShape0.AddLine(MkFloat2(0.f, 0.f), MkFloat2(200.f, 100.f));
		lineShape0.SetColor(MkColor::Red);
		lineShape0.SetLocalDepth(-1.f);
		lineShape0.SetRecordable(true);
		
		MkLineShape& lineShape1 = bodyFrame->CreateLine(L"test white line");
		MkArray<MkFloat2> lineVertices(4);
		lineVertices.PushBack(MkFloat2(0.f, 0.f));
		lineVertices.PushBack(MkFloat2(-100.f, 200.f));
		lineVertices.PushBack(MkFloat2(-100.f, 300.f));
		lineVertices.PushBack(MkFloat2(50.f, 350.f));
		lineShape1.AddLine(lineVertices);
		lineShape1.SetColor(MkColor::White);
		lineShape1.SetLocalDepth(-1.f);
		lineShape1.SetRecordable(true);
		
		//--------------------------------------------------//
		MkDrawSceneNodeStep* ds = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep(L"Final");
		ds->SetSceneNode(m_PageNode);

		MK_DEV_PANEL.MsgToFreeboard(2, L"1 키 : (1)번 윈도우 토글");
		MK_DEV_PANEL.MsgToFreeboard(3, L"2 키 : (2)번 윈도우 토글");
		MK_DEV_PANEL.MsgToFreeboard(4, L"3 키 : (3)번 윈도우 토글");
		MK_DEV_PANEL.MsgToFreeboard(5, L"4 키 : 모달 메세지 박스 토글");
		MK_DEV_PANEL.MsgToFreeboard(6, L"5 키 : 웹 브라우져 삽입/삭제");

		MK_DEV_PANEL.MsgToFreeboard(8, L"S 키 : 현재 scene을 [PackRoot.msd] 파일로 저장");
		MK_DEV_PANEL.MsgToFreeboard(9, L"L 키 : [PackRoot.msd] 파일을 읽어 현재 scene을 구성");

		m_PageNode->Update();

		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		MkInt2 mp = MK_INPUT_MGR.GetAbsoluteMousePosition(true);
		MK_DEV_PANEL.MsgToFreeboard(0, L"Cursor : " + MkStr(mp));

		if (m_PageNode != NULL)
		{
			/*
			MkSceneNode* mainNode = rootNode->GetChildNode(L"<Root>")->GetChildNode(L"Main");
			if (mainNode != NULL)
			{
				if (MK_INPUT_MGR.GetMouseLeftButtonReleased())
				{
					MkBitField32 attr;
					attr.Set(ePA_SNA_AcceptInput);
					MkArray<MkPanel*> buffer;
					if (mainNode->PickPanel(buffer, MkFloat2(static_cast<float>(mp.x), static_cast<float>(mp.y)), 0., attr))
					{
						m_TargetNode = buffer[0]->GetParentNode();
						MK_DEV_PANEL.MsgToLog(L"Pick : " + m_TargetNode->GetNodeName().GetString());
					}
				}

				if (MK_INPUT_MGR.GetMouseRightButtonPushing())
				{
					const int CHECK_COUNT = 4;
					const float MARGIN_RATIO = 0.2f;

					MkFloat2 rectSize(150.f, 30.f);
					MkFloat2 clientSize(rectSize.x - 6.f, rectSize.y - 6.f);
					MkFloat2 rectPos;
					rectPos.x = static_cast<float>(mp.x) - rectSize.x * 0.5f;
					rectPos.y = static_cast<float>(mp.y) - rectSize.y * 0.5f;

					// 상하좌우 MARGIN_RATIO 만큼 겹쳐도 됨
					MkFloat2 margin = rectSize * MARGIN_RATIO;
					MkFloatRect myRect(rectPos + margin, rectSize - margin * 2.f);

					//
					unsigned int childCnt = m_RectDummyNode->GetChildNodeCount();

					// 이미 존재하는 rect 리스트를 구함
					MkArray<MkFloatRect> rectList(childCnt);
					MkArray<MkHashStr> childNameList;
					m_RectDummyNode->GetChildNodeList(childNameList);
					MK_INDEXING_LOOP(childNameList, i)
					{
						MkWindowThemedNode* node = dynamic_cast<MkWindowThemedNode*>(m_RectDummyNode->GetChildNode(childNameList[i]));
						rectList.PushBack(MkFloatRect(node->GetWorldPosition(), node->GetWindowRect().size));
					}
					
					for (int k=0; k<CHECK_COUNT; ++k)
					{
						MkFloatRect collisionUnion;
						MK_INDEXING_LOOP(rectList, i)
						{
							const MkFloatRect& currRect = rectList[i];
							if (currRect.CheckIntersection(myRect))
							{
								collisionUnion.UpdateToUnion(currRect);
							}
						}

						if (collisionUnion.SizeIsZero() || (!collisionUnion.SqueezeOut(myRect)))
							break;
					}

					rectPos = myRect.position - margin;

					//
					MkWindowThemedNode* tbgNode = MkWindowThemedNode::CreateChildNode(m_RectDummyNode, MkStr(m_DummyIndex));
					tbgNode->SetLocalPosition(rectPos);
					tbgNode->SetLocalDepth(-0.001f * static_cast<float>(m_DummyIndex));
					tbgNode->SetThemeName(MkWindowThemeData::DefaultThemeName);
					tbgNode->SetComponentType(MkWindowThemeData::eCT_NoticeBox);
					tbgNode->SetShadowUsage(false);
					tbgNode->SetClientSize(clientSize);
					tbgNode->SetFormState(MkWindowThemeFormData::eS_Default);
					tbgNode->SetAcceptInput(false);

					DummyTimer& dummyTimer = m_DummyTimer.PushBack();
					dummyTimer.id = m_DummyIndex;
					dummyTimer.state = 0;
					dummyTimer.counter.SetUp(timeState, 1.f);

					++m_DummyIndex;
				}

				unsigned int killCount = 0;
				MK_INDEXING_LOOP(m_DummyTimer, i)
				{
					DummyTimer& dummyTimer = m_DummyTimer[i];
					if (dummyTimer.counter.OnTick(timeState))
					{
						if (dummyTimer.state == 0)
						{
							dummyTimer.counter.SetUp(timeState, 0.2f);
							dummyTimer.state = 1;
						}
						else if (dummyTimer.state == 1)
						{
							++killCount;
						}
					}
					else
					{
						MkSceneNode* node = m_RectDummyNode->GetChildNode(MkStr(dummyTimer.id));

						MkFloat2 pos = node->GetLocalPosition();
						pos.y += static_cast<float>(timeState.elapsed) * 40.f;
						node->SetLocalPosition(pos);

						if (dummyTimer.state == 1)
						{
							node->SetLocalAlpha(1.f - dummyTimer.counter.GetTickRatio(timeState));
						}
					}
				}

				for (unsigned int i=0; i<killCount; ++i)
				{
					m_RectDummyNode->RemoveChildNode(MkStr(m_DummyTimer[0].id));
					m_DummyTimer.PopFront();
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
						ip->SetSubsetOrSequenceName(keys[si], 0.);
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
				else if (m_TargetNode->IsDerivedFrom(ePA_SNT_VisualPatternNode))
				{
					MkVisualPatternNode* vpNode = dynamic_cast<MkVisualPatternNode*>(m_TargetNode);

					if (MK_INPUT_MGR.GetKeyReleased(L'1'))
					{
						_IncAP();
						vpNode->SetAlignmentPosition(ap);
					}

					if (vpNode->GetNodeType() == ePA_SNT_WindowTagNode)
					{
						MkWindowTagNode* targetNode = dynamic_cast<MkWindowTagNode*>(vpNode);

						if (MK_INPUT_MGR.GetKeyReleased(L'2'))
						{
							targetNode->SetIconPath(targetNode->GetIconPath().Empty() ? L"Default\\theme_default.png" : MkHashStr::EMPTY);
						}

						if (MK_INPUT_MGR.GetKeyReleased(L'3'))
						{
							MkArray<MkHashStr> textName;
							if (targetNode->GetTextName().Empty())
							{
								textName.PushBack(L"WindowTitle");
							}
							targetNode->SetTextName(textName);
						}
					}
					else if (vpNode->GetNodeType() == ePA_SNT_WindowThemedNode)
					{
						MkWindowThemedNode* targetNode = dynamic_cast<MkWindowThemedNode*>(vpNode);

						MkFloat2 cs = targetNode->GetClientRect().size;
						if (MK_INPUT_MGR.GetKeyPushing(VK_LEFT))
						{
							cs.x -= movement;
							targetNode->SetClientSize(cs);
						}
						if (MK_INPUT_MGR.GetKeyPushing(VK_RIGHT))
						{
							cs.x += movement;
							targetNode->SetClientSize(cs);
						}
						if (MK_INPUT_MGR.GetKeyPushing(VK_UP))
						{
							cs.y += movement;
							targetNode->SetClientSize(cs);
						}
						if (MK_INPUT_MGR.GetKeyPushing(VK_DOWN))
						{
							cs.y -= movement;
							targetNode->SetClientSize(cs);
						}

						if (MK_INPUT_MGR.GetKeyReleased(L'2'))
						{
							targetNode->SetShadowUsage(!targetNode->GetShadowUsage());
						}

						if (MK_INPUT_MGR.GetKeyReleased(L'3'))
						{
							int comp = static_cast<int>(targetNode->GetComponentType()) + 1;
							if (comp >= static_cast<int>(MkWindowThemeData::eCT_RegularMax))
							{
								comp = 1;
							}
							targetNode->SetComponentType(static_cast<MkWindowThemeData::eComponentType>(comp));

							MK_DEV_PANEL.MsgToLog(L"component : " + MkWindowThemeData::ComponentTypeName[comp].GetString());
						}

						if (MK_INPUT_MGR.GetKeyReleased(L'4'))
						{
							MkWindowThemeFormData::eFormType ft = targetNode->GetFormType();
							int maxPos = 0, fp = 0;
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
								fp = static_cast<int>(targetNode->GetFormState()) + 1;
								if (fp >= maxPos)
								{
									fp = 0;
								}
							}
							targetNode->SetFormState(static_cast<MkWindowThemeFormData::eState>(fp));
							MK_DEV_PANEL.MsgToLog(L"form pos : " + MkStr(fp));
						}
					}
				}

				MK_DEV_PANEL.MsgToFreeboard(1, L"Target node : " + m_TargetNode->GetNodeName().GetString());
			}
			*/

			if (m_PageNode != NULL)
			{
				m_PageNode->Update(timeState.fullTime);
			}

			if (MK_INPUT_MGR.GetKeyReleased(L'1'))
			{
				MkWindowManagerNode* winMgr = dynamic_cast<MkWindowManagerNode*>(m_PageNode->GetChildNode(L"<Root>")->GetChildNode(L"WinMgr"));
				MkHashStr name(L"Win(1)");
				if (winMgr->IsActivating(name))
				{
					winMgr->DeactivateWindow(name);
				}
				else
				{
					winMgr->ActivateWindow(name, false);
				}
			}
			if (MK_INPUT_MGR.GetKeyReleased(L'2'))
			{
				MkWindowManagerNode* winMgr = dynamic_cast<MkWindowManagerNode*>(m_PageNode->GetChildNode(L"<Root>")->GetChildNode(L"WinMgr"));
				MkHashStr name(L"Win(2)");
				if (winMgr->IsActivating(name))
				{
					winMgr->DeactivateWindow(name);
				}
				else
				{
					winMgr->ActivateWindow(name, false);
				}
			}
			if (MK_INPUT_MGR.GetKeyReleased(L'3'))
			{
				MkWindowManagerNode* winMgr = dynamic_cast<MkWindowManagerNode*>(m_PageNode->GetChildNode(L"<Root>")->GetChildNode(L"WinMgr"));
				MkHashStr name(L"Win(3)");
				if (winMgr->IsActivating(name))
				{
					winMgr->DeactivateWindow(name);
				}
				else
				{
					winMgr->ActivateWindow(name, false);
				}
			}
			if (MK_INPUT_MGR.GetKeyReleased(L'4'))
			{
				MkWindowManagerNode* winMgr = dynamic_cast<MkWindowManagerNode*>(m_PageNode->GetChildNode(L"<Root>")->GetChildNode(L"WinMgr"));
				MkHashStr name(L"MsgBox");
				if (winMgr->IsActivating(name))
				{
					winMgr->DeactivateWindow(name);
				}
				else
				{
					winMgr->ActivateWindow(name, true);
				}
			}

			if (MK_INPUT_MGR.GetKeyReleased(L'5'))
			{
				if (MK_RENDERER.GetWebDialog() == NULL)
				{
					MK_RENDERER.OpenWebDialog(L"http://www.daum.net/", MkIntRect(30, 100, 800, 600));
				}
				else
				{
					MK_RENDERER.CloseWebDialog();
				}
			}

			if (MK_INPUT_MGR.GetKeyReleased(L'S'))
			{
				MkDataNode tmpRootSaveTarget;
				m_PageNode->GetChildNode(L"<Root>")->Save(tmpRootSaveTarget);
				tmpRootSaveTarget.SaveToText(L"PackRoot.msd");

				MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
				MK_DEV_PANEL.MsgToLog(L"> 현재 scene 저장 -> PackRoot.msd");
				MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
			}

			if (MK_INPUT_MGR.GetKeyReleased(L'L'))
			{
				MkDataNode tmpRootLoadTarget;
				if (tmpRootLoadTarget.Load(L"PackRoot.msd"))
				{
					m_PageNode->Clear();
					MkSceneNode* rootNode = MkSceneObject::Build(tmpRootLoadTarget);
					rootNode->ChangeNodeName(L"<Root>");
					m_PageNode->AttachChildNode(rootNode);

					MK_DEV_PANEL.ClearLogWindow();
					MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
					MK_DEV_PANEL.MsgToLog(L"> PackRoot.msd 로드 -> 현재 scene");
					MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
				}
			}
			
			//MK_DEV_PANEL.MsgToFreeboard(0, L"HorizontalChange : " + hcn.GetString());
		}
	}

	virtual void Clear(MkDataNode* sharingNode = NULL)
	{
		MK_DELETE(m_PageNode);
		
		MK_RENDERER.GetDrawQueue().RemoveStep(L"Final");
	}

	TestPage(const MkHashStr& name) : MkBasePage(name)
	{
		m_PageNode = NULL;
		//m_TargetNode = NULL;
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

	MkSceneNode* m_PageNode;
	//MkSceneNode* m_TargetNode;
	unsigned int si;
	eRectAlignmentPosition ap;

	MkSceneNode* m_RectDummyNode;
	unsigned int m_DummyIndex;

	typedef struct _DummyTimer
	{
		unsigned int id;
		int state;
		MkTimeCounter counter;
	}
	DummyTimer;

	MkDeque<DummyTimer> m_DummyTimer;
};

class TestFramework : public MkRenderFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		MK_PAGE_MGR.SetUp(new MkBasePage(L"Root"));
		MK_PAGE_MGR.RegisterChildPage(L"Root", new TestPage(L"TestPage"));
		MK_PAGE_MGR.ChangePageDirectly(L"TestPage");
		
		return MkRenderFramework::SetUp(clientWidth, clientHeight, fullScreen, cmdLine);
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
	application.Run(hI, L"떼스또", L"..\\FileRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768);

	return 0;
}

//------------------------------------------------------------------------------------------------//

