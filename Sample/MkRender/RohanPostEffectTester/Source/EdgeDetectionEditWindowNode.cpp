
#include "MkCore_MkDataNode.h"

#include "MkPA_MkBodyFrameControlNode.h"
#include "MkPA_MkSliderControlNode.h"
#include "MkPA_MkEditBoxControlNode.h"
#include "MkPA_MkWindowFactory.h"

#include "SettingRepository.h"
#include "EdgeDetectionEditWindowNode.h"


//------------------------------------------------------------------------------------------------//

void EdgeDetectionEditWindowNode::SetUp(MkSceneNode* rootNode)
{
	m_RootNode = rootNode;

	// title bar
	SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 300.f, true);
	SetIcon(MkWindowThemeData::eIT_Default);
	SetCaption(L"편집 : 외곽선 검출", eRAP_LeftCenter);
	SetAlignmentPosition(eRAP_RightTop);
	SetAlignmentOffset(MkFloat2(-10.f, -10.f));
	
	// body frame
	const float HEIGHT = 310.f;

	MkBodyFrameControlNode* bodyFrame = MkBodyFrameControlNode::CreateChildNode(this, L"BodyFrame");
	bodyFrame->SetBodyFrame
		(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(224.f, HEIGHT));

	// amplifier
	{
		MkPanel& panel = bodyFrame->CreatePanel(L"ST_Amplifier");
		panel.SetLocalPosition(MkFloat2(10.f, HEIGHT - 50.f));
		panel.SetLocalDepth(-1.f);
		panel.SetTextMsg(L"증폭 팩터");

		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_Amplifier");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, 1, 14, static_cast<int>(SREPO.GetEdgeDetectionAmplifier()));
		sliderNode->SetAlignmentPosition(eRAP_LeftTop);
		sliderNode->SetAlignmentOffset(MkFloat2(10.f, -80.f));
		sliderNode->SetLocalDepth(-1.f);
	}

	// blend
	{
		MkPanel& panel = bodyFrame->CreatePanel(L"ST_Blend");
		panel.SetLocalPosition(MkFloat2(10.f, HEIGHT - 140.f));
		panel.SetLocalDepth(-1.f);
		panel.SetTextMsg(L"[디버깅] 원본 색상 유지");

		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_Blend");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, 0, 100, static_cast<int>(SREPO.GetEdgeDetectionDebugBlend() * 100.f));
		sliderNode->SetAlignmentPosition(eRAP_LeftTop);
		sliderNode->SetAlignmentOffset(MkFloat2(10.f, -170.f));
		sliderNode->SetLocalDepth(-1.f);
	}

	// color
	{
		MkPanel& panel = bodyFrame->CreatePanel(L"ST_EdgeColor");
		panel.SetLocalPosition(MkFloat2(10.f, HEIGHT - 230.f));
		panel.SetLocalDepth(-1.f);
		panel.SetTextMsg(L"외곽선 색상 (R,G,B:0~255)");
		
		MkEditBoxControlNode* ebNode = MkEditBoxControlNode::CreateChildNode(bodyFrame, L"EB_EdgeColor");
		ebNode->SetSingleLineEditBox(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 190.f, L"0, 0, 0", true);
		ebNode->SetAlignmentPosition(eRAP_LeftTop);
		ebNode->SetAlignmentOffset(MkFloat2(10.f, -246.f));
		ebNode->SetLocalDepth(-1.f);
	}

	{
		MkWindowFactory wndFactory;
		wndFactory.SetMinClientSizeForButton(MkFloat2(205.f, 12.f));
		MkWindowBaseNode* btnInst = wndFactory.CreateButtonTypeNode(L"Btn_Reset", MkWindowThemeData::eCT_CancelBtn, L"설정 초기화");
		btnInst->SetLocalPosition(MkFloat2(10.f, 10.f));
		bodyFrame->AttachChildNode(btnInst);
	}
}

void EdgeDetectionEditWindowNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	if (eventType == ePA_SNE_SliderMoved)
	{
		if (path.GetSize() == 2)
		{
			int factor;
			argument->GetData(MkSliderControlNode::ArgKey_SliderPos, factor, 0);

			bool resetValue = false;
			if (path[1].Equals(0, L"SL_Amplifier"))
			{
				SREPO.SetEdgeDetectionAmplifier(static_cast<float>(factor));
				resetValue = true;
			}
			else if (path[1].Equals(0, L"SL_Blend"))
			{
				SREPO.SetEdgeDetectionDebugBlend(static_cast<float>(factor) / 100.f);
				resetValue = true;
			}

			if (resetValue && (m_RootNode != NULL))
			{
				MkPanel* mainPanel = m_RootNode->GetPanel(L"Main");
				if (mainPanel != NULL)
				{
					mainPanel->SetUserDefinedProperty(L"g_EdgeFactor", SREPO.GetEdgeDetectionAmplifier(), SREPO.GetEdgeDetectionDebugBlend());
				}
				return;
			}
		}
	}
	else if (eventType == ePA_SNE_TextCommitted)
	{
		MkStr buffer;
		if (argument->GetData(MkEditBoxControlNode::ArgKey_Text, buffer, 0))
		{
			MkArray<MkStr> tokens;
			if (buffer.Tokenize(tokens, L",") == 3)
			{
				int ir = tokens[0].ToInteger();
				int ig = tokens[1].ToInteger();
				int ib = tokens[2].ToInteger();

				MkVec3 edgeColor;
				edgeColor.x = Clamp<float>(static_cast<float>(ir) / 255.f, 0.f, 1.f);
				edgeColor.y = Clamp<float>(static_cast<float>(ig) / 255.f, 0.f, 1.f);
				edgeColor.z = Clamp<float>(static_cast<float>(ib) / 255.f, 0.f, 1.f);

				SREPO.SetEdgeDetectionEdgeColor(edgeColor);

				MkPanel* mainPanel = m_RootNode->GetPanel(L"Main");
				if (mainPanel != NULL)
				{
					mainPanel->SetUserDefinedProperty(L"g_EdgeColor", SREPO.GetEdgeDetectionEdgeColor().x, SREPO.GetEdgeDetectionEdgeColor().y, SREPO.GetEdgeDetectionEdgeColor().z);
				}
			}
			return;
		}
	}
	else if (eventType == ePA_SNE_CursorLBtnReleased)
	{
		if (path.GetSize() == 2)
		{
			if (path[1].Equals(0, L"Btn_Reset"))
			{
				SREPO.ResetEdgeDetectionValue();
				_ResetEffectValue();
			}
		}
	}

	MkTitleBarControlNode::SendNodeReportTypeEvent(eventType, path, argument);
}

void EdgeDetectionEditWindowNode::Activate(void)
{
	_ResetEffectValue();

	MkTitleBarControlNode::Activate();
}

//------------------------------------------------------------------------------------------------//

void EdgeDetectionEditWindowNode::_ResetEffectValue(void)
{
	MkSceneNode* bodyFrame = GetChildNode(L"BodyFrame");

	MkSliderControlNode* sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_Amplifier"));
	sliderNode->SetSliderValue(static_cast<int>(SREPO.GetEdgeDetectionAmplifier()));
	
	MkEditBoxControlNode* ebNode = dynamic_cast<MkEditBoxControlNode*>(bodyFrame->GetChildNode(L"EB_EdgeColor"));
	MkStr colorMsg;
	colorMsg.Reserve(20);
	colorMsg += static_cast<int>(SREPO.GetEdgeDetectionEdgeColor().x * 255.f);
	colorMsg += L", ";
	colorMsg += static_cast<int>(SREPO.GetEdgeDetectionEdgeColor().y * 255.f);
	colorMsg += L", ";
	colorMsg += static_cast<int>(SREPO.GetEdgeDetectionEdgeColor().z * 255.f);
	ebNode->SetText(colorMsg);

	MkPanel* mainPanel = m_RootNode->GetPanel(L"Main");
	if (mainPanel != NULL)
	{
		mainPanel->SetUserDefinedProperty(L"g_EdgeColor", SREPO.GetEdgeDetectionEdgeColor().x, SREPO.GetEdgeDetectionEdgeColor().y, SREPO.GetEdgeDetectionEdgeColor().z);
	}
}