
//------------------------------------------------------------------------------------------------//
// single/multi-thread rendering application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkCheck.h"
#include "MkCore_MkPageManager.h"
#include "MkCore_MkInputManager.h"
#include "MkCore_MkTimeManager.h"
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
		
		MkSceneNode* bgNode = mainNode->CreateChildNode(L"BG");
		bgNode->CreatePanel(L"P").SetTexture(L"Image\\rohan_screenshot.png");
		bgNode->SetLocalDepth(10000.f);

		MkPanel& ip = mainNode->CreatePanel(L"ImgTest");
		ip.SetSmallerSourceOp(MkPanel::eAttachToLeftTop);
		ip.SetLocalDepth(9999.f);
		ip.SetTexture(L"Image\\s01.jpg");

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
		titleBar->SetCaption(L"(2) 리스트 + 프로그레스 + client size 변경", eRAP_LeftCenter);
		winMgrNode->AttachWindow(titleBar);
		winMgrNode->ActivateWindow(L"Win(2)");

		titleBar->SetLocalPosition(MkFloat2(200.f, 500.f));

		// window B : body frame
		bodyFrame = MkBodyFrameControlNode::CreateChildNode(titleBar, L"BodyFrameB");
		bodyFrame->SetBodyFrame
			(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(360.f, 250.f));

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
		pbNode->SetProgressBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Medium, 215.f, 50, 200, MkProgressBarNode::eSPM_Percentage);
		pbNode->SetAlignmentPosition(eRAP_LeftTop);
		pbNode->SetAlignmentOffset(MkFloat2(10.f, -160.f));
		pbNode->SetLocalDepth(-1.f);

		MkWindowThemedNode* tgNode = MkWindowThemedNode::CreateChildNode(bodyFrame, L"TextGuide");
		tgNode->SetThemeName(MkWindowThemeData::DefaultThemeName);
		tgNode->SetComponentType(MkWindowThemeData::eCT_GuideBox);
		MkFloat2 textPanelSize(110.f, 220.f);
		tgNode->SetClientSize(textPanelSize);
		tgNode->SetAlignmentPosition(eRAP_LeftTop);
		tgNode->SetAlignmentOffset(MkFloat2(240.f, -20.f));
		tgNode->SetLocalDepth(-1.f);

		MkPanel& tp = tgNode->CreatePanel(L"TextTest");
		tp.SetSmallerSourceOp(MkPanel::eAttachToLeftTop);
		tp.SetBiggerSourceOp(MkPanel::eCutSource);
		tp.SetPanelSize(textPanelSize);
		tp.SetLocalDepth(-1.f);
		MkArray<MkHashStr> textName;
		textName.PushBack(L"_Sample");
		tp.SetTextNode(textName, true);

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

		// custom btn
		MkWindowBaseNode* customBtnNode = MkWindowBaseNode::CreateChildNode(bodyFrame, L"Custom_01");
		customBtnNode->SetThemeName(MkWindowThemeData::DefaultThemeName);
		customBtnNode->SetCustomForm(L"TestBtn01");
		customBtnNode->SetFormState(MkWindowThemeFormData::eS_Default);
		customBtnNode->SetLocalPosition(MkFloat2(160.f, 60.f));
		customBtnNode->SetLocalDepth(-1.f);

		customBtnNode = MkWindowBaseNode::CreateChildNode(bodyFrame, L"Custom_02");
		customBtnNode->SetThemeName(MkWindowThemeData::DefaultThemeName);
		customBtnNode->SetCustomForm(L"TestBtn02");
		customBtnNode->SetFormState(MkWindowThemeFormData::eS_Default);
		customBtnNode->SetLocalPosition(MkFloat2(240.f, 5.f));
		customBtnNode->SetLocalDepth(-2.f);

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

		MK_DEV_PANEL.MsgToFreeboard(2, L"1/2/3 키 : 해당 번호 윈도우 토글");
		MK_DEV_PANEL.MsgToFreeboard(3, L"4 키 : 메세지박스(모달) 토글");
		MK_DEV_PANEL.MsgToFreeboard(4, L"5 키 : 웹 브라우져 삽입/삭제");
		
		MK_DEV_PANEL.MsgToFreeboard(6, L"O 키 : 현재 scene을 [PackRoot.msd] 파일로 저장");
		MK_DEV_PANEL.MsgToFreeboard(7, L"P 키 : [PackRoot.msd] 파일을 읽어 현재 scene을 구성");

		m_PageNode->Update();

		MkTimeState timeState;
		MK_TIME_MGR.GetCurrentTimeState(timeState);
		m_TickCounter.SetUp(timeState, 3.f);

		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		// 1st msg(header)
		{
			MkStr msg;
			msg.Reserve(512);

			msg += L"커서 : ";
			msg += MK_INPUT_MGR.GetAbsoluteMousePosition(true);
			msg += L", 틱 : ";
			msg += m_TickCounter.GetTickCount() - m_TickCounter.GetTickTime(timeState);

			MK_DEV_PANEL.MsgToFreeboard(0, msg);
		}

		if (m_PageNode != NULL)
		{
			if (m_TickCounter.OnTick(timeState))
			{
				MkPanel* imagePanel = m_PageNode->GetChildNode(L"<Root>")->GetChildNode(L"Main")->GetPanel(L"ImgTest");
				MkHashStr currSN = imagePanel->GetSubsetOrSequenceName();
				MkArray<MkHashStr> seqs;
				imagePanel->GetTexturePtr()->GetImageInfo().GetSequences().GetKeyList(seqs);

				if (currSN.Empty())
				{
					currSN = seqs[0];
				}
				else
				{
					unsigned int index = seqs.FindFirstInclusion(MkArraySection(0), currSN) + 1;
					currSN = seqs.IsValidIndex(index) ? seqs[index] : MkHashStr::EMPTY;
				}
				
				imagePanel->SetSubsetOrSequenceName(currSN, 0.);
				MkStr msg = L"s01.jpg의 애니메이션";
				msg += currSN.Empty() ? L" 없음. 원본 그대로 출력" : MkStr(L"을 [" + currSN.GetString() + L"]로 교체");
				MK_DEV_PANEL.MsgToLog(msg);
			}

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
			}
			
			if (m_TargetNode != NULL)
			{
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
			
			MkWindowManagerNode* winMgr = dynamic_cast<MkWindowManagerNode*>(m_PageNode->GetChildNode(L"<Root>")->GetChildNode(L"WinMgr"));
			if (winMgr != NULL)
			{
				// 포커스 윈도우 transform
				MkHashStr winName = winMgr->GetFocusWindowName();
				if (winName != m_FocusWindowName)
				{
					m_FocusWindowName = winName;
					if (m_FocusWindowName.Empty())
					{
						MK_DEV_PANEL.MsgToFreeboard(9, L"포커스 윈도우 없음");
						MK_DEV_PANEL.MsgToFreeboard(10, MkStr::EMPTY);
						MK_DEV_PANEL.MsgToFreeboard(11, MkStr::EMPTY);
						MK_DEV_PANEL.MsgToFreeboard(12, MkStr::EMPTY);
					}
					else
					{
						MK_DEV_PANEL.MsgToFreeboard(9, L"포커스 윈도우 : " + m_FocusWindowName.GetString());
						MK_DEV_PANEL.MsgToFreeboard(10, L"   방향키 : 이동");
						MK_DEV_PANEL.MsgToFreeboard(11, L"   Q/E 키 : 좌회전/우회전, W 키 : 회전 초기화");
						MK_DEV_PANEL.MsgToFreeboard(12, L"   A/D 키 : 축소/확대, S 키 : 크기 초기화");
					}
				}
				
				MkSceneNode* frontNode = winMgr->GetChildNode(m_FocusWindowName);
				if (frontNode != NULL)
				{
					const float movement = static_cast<float>(timeState.elapsed) * 300.f;

					bool focusIsWin2 = m_FocusWindowName.Equals(0, L"Win(2)");
					MkFloat2 win2SizeOffset;

					// 이동
					MkFloat2 localPos = frontNode->GetLocalPosition();
					if (MK_INPUT_MGR.GetKeyPushing(VK_LEFT))
					{
						localPos.x -= movement;
						frontNode->SetLocalPosition(localPos);

						if (focusIsWin2) win2SizeOffset.x -= movement;
					}
					if (MK_INPUT_MGR.GetKeyPushing(VK_RIGHT))
					{
						localPos.x += movement;
						frontNode->SetLocalPosition(localPos);

						if (focusIsWin2) win2SizeOffset.x += movement;
					}
					if (MK_INPUT_MGR.GetKeyPushing(VK_UP))
					{
						localPos.y += movement;
						frontNode->SetLocalPosition(localPos);

						if (focusIsWin2) win2SizeOffset.y -= movement;
					}
					if (MK_INPUT_MGR.GetKeyPushing(VK_DOWN))
					{
						localPos.y -= movement;
						frontNode->SetLocalPosition(localPos);

						if (focusIsWin2) win2SizeOffset.y += movement;
					}

					if (focusIsWin2)
					{
						MkWindowThemedNode* win2BGNode = dynamic_cast<MkWindowThemedNode*>(frontNode->GetChildNode(L"BodyFrameB"));

						MkProgressBarNode* progBar = dynamic_cast<MkProgressBarNode*>(win2BGNode->GetChildNode(L"ProgBar"));
						float currVal = static_cast<float>(progBar->GetMaximumValue()) * m_TickCounter.GetTickRatio(timeState);
						progBar->SetCurrentValue(static_cast<int>(currVal));

						if (!win2SizeOffset.IsZero())
						{
							MkFloat2 bgSize = win2BGNode->GetClientSize() + win2SizeOffset;
							if ((bgSize.x >= 360.f) && (bgSize.y >= 250.f) && (bgSize.x <= 500.f) && (bgSize.y <= 350.f)) // 크기 제한
							{
								win2BGNode->SetClientSize(bgSize);

								MkWindowThemedNode* tgNode = dynamic_cast<MkWindowThemedNode*>(win2BGNode->GetChildNode(L"TextGuide"));
								MkFloat2 textCS = tgNode->GetClientSize() + win2SizeOffset;
								tgNode->SetClientSize(textCS);

								MkPanel* textPanel = tgNode->GetPanel(L"TextTest");
								textPanel->SetPanelSize(textCS);
								textPanel->BuildAndUpdateTextCache();
							}
						}
					}

					// 회전
					float localRot = frontNode->GetLocalRotation();
					const float rotVel = static_cast<float>(timeState.elapsed) * MKDEF_PI * 0.5f;
					if (MK_INPUT_MGR.GetKeyPushing(L'Q'))
					{
						localRot -= rotVel;
						frontNode->SetLocalRotation(localRot);
					}
					if (MK_INPUT_MGR.GetKeyPushing(L'E'))
					{
						localRot += rotVel;
						frontNode->SetLocalRotation(localRot);
					}
					if (MK_INPUT_MGR.GetKeyPressed(L'W'))
					{
						frontNode->SetLocalRotation(0.f);
					}
					
					float localScale = frontNode->GetLocalScale();
					const float scaleVel = static_cast<float>(timeState.elapsed);
					if (MK_INPUT_MGR.GetKeyPushing(L'A'))
					{
						localScale -= scaleVel;
						frontNode->SetLocalScale(localScale);
					}
					if (MK_INPUT_MGR.GetKeyPushing(L'D'))
					{
						localScale += scaleVel;
						frontNode->SetLocalScale(localScale);
					}
					if (MK_INPUT_MGR.GetKeyPressed(L'S'))
					{
						frontNode->SetLocalScale(1.f);
					}
				}
			}

			m_PageNode->Update(timeState.fullTime);

			if (winMgr != NULL)
			{
				MkHashStr targetWinName;
				bool modal = false;

				if (MK_INPUT_MGR.GetKeyReleased(L'1'))
				{
					targetWinName = L"Win(1)";
				}
				if (MK_INPUT_MGR.GetKeyReleased(L'2'))
				{
					targetWinName = L"Win(2)";
				}
				if (MK_INPUT_MGR.GetKeyReleased(L'3'))
				{
					targetWinName = L"Win(3)";
				}
				if (MK_INPUT_MGR.GetKeyReleased(L'4'))
				{
					targetWinName = L"MsgBox";
					modal = true;
				}

				if (!targetWinName.Empty())
				{
					if (winMgr->IsActivating(targetWinName))
					{
						winMgr->DeactivateWindow(targetWinName);
						MK_DEV_PANEL.MsgToLog(targetWinName.GetString() + L" 윈도우 닫음");
					}
					else
					{
						winMgr->ActivateWindow(targetWinName, modal);
						MK_DEV_PANEL.MsgToLog(targetWinName.GetString() + L" 윈도우 오픈");
					}
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

			if (MK_INPUT_MGR.GetKeyReleased(L'O'))
			{
				MkDataNode tmpRootSaveTarget;
				m_PageNode->GetChildNode(L"<Root>")->Save(tmpRootSaveTarget);
				tmpRootSaveTarget.SaveToText(L"PackRoot.msd");

				MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
				MK_DEV_PANEL.MsgToLog(L"> 현재 scene 저장 -> PackRoot.msd");
				MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
			}

			if (MK_INPUT_MGR.GetKeyReleased(L'P'))
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
	}

	virtual ~TestPage() { Clear(); }

protected:

	MkSceneNode* m_PageNode;

	MkHashStr m_FocusWindowName;
	MkTimeCounter m_TickCounter;
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

