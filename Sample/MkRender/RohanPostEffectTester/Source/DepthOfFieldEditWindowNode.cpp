
#include "MkCore_MkDataNode.h"

#include "MkPA_MkBodyFrameControlNode.h"
#include "MkPA_MkSliderControlNode.h"
#include "MkPA_MkWindowFactory.h"

#include "SettingRepository.h"
#include "DepthOfFieldEditWindowNode.h"


//------------------------------------------------------------------------------------------------//

void DepthOfFieldEditWindowNode::SetUp(MkSceneNode* rootNode)
{
	m_RootNode = rootNode;

	// title bar
	SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 300.f, true);
	SetIcon(MkWindowThemeData::eIT_Default);
	SetCaption(L"편집 : 피사계 심도", eRAP_LeftCenter);
	SetAlignmentPosition(eRAP_RightTop);
	SetAlignmentOffset(MkFloat2(-10.f, -10.f));
	
	// body frame
	const float HEIGHT = 330.f;

	MkBodyFrameControlNode* bodyFrame = MkBodyFrameControlNode::CreateChildNode(this, L"BodyFrame");
	bodyFrame->SetBodyFrame
		(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(224.f, HEIGHT));

	// position
	{
		MkPanel& panel = bodyFrame->CreatePanel(L"ST_Position");
		panel.SetLocalPosition(MkFloat2(10.f, HEIGHT - 50.f));
		panel.SetLocalDepth(-1.f);
		panel.SetTextMsg(L"위치(meter)");

		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_Position");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, 0, 800, static_cast<int>(SREPO.GetDepthOfFieldPosition() * 800.f));
		sliderNode->SetAlignmentPosition(eRAP_LeftTop);
		sliderNode->SetAlignmentOffset(MkFloat2(10.f, -80.f));
		sliderNode->SetLocalDepth(-1.f);
	}

	// range
	{
		MkPanel& panel = bodyFrame->CreatePanel(L"ST_Range");
		panel.SetLocalPosition(MkFloat2(10.f, HEIGHT - 140.f));
		panel.SetLocalDepth(-1.f);
		panel.SetTextMsg(L"범위");

		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_Range");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, 0, 60, static_cast<int>(SREPO.GetDepthOfFieldRange()));
		sliderNode->SetAlignmentPosition(eRAP_LeftTop);
		sliderNode->SetAlignmentOffset(MkFloat2(10.f, -170.f));
		sliderNode->SetLocalDepth(-1.f);
	}

	// debug color
	{
		MkPanel& panel = bodyFrame->CreatePanel(L"ST_Debug");
		panel.SetLocalPosition(MkFloat2(10.f, HEIGHT - 230.f));
		panel.SetLocalDepth(-1.f);
		panel.SetTextMsg(L"[디버깅] 심도만 표현");
		
		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_Debug");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, 0, 100, static_cast<int>(SREPO.GetDepthOfFieldDebugBlend() * 100.f));
		sliderNode->SetAlignmentPosition(eRAP_LeftTop);
		sliderNode->SetAlignmentOffset(MkFloat2(10.f, -260.f));
		sliderNode->SetLocalDepth(-1.f);
	}

	{
		MkWindowFactory wndFactory;
		wndFactory.SetMinClientSizeForButton(MkFloat2(205.f, 12.f));
		MkWindowBaseNode* btnInst = wndFactory.CreateButtonTypeNode(L"Btn_Reset", MkWindowThemeData::eCT_CancelBtn, L"설정 초기화");
		btnInst->SetLocalPosition(MkFloat2(10.f, 10.f));
		bodyFrame->AttachChildNode(btnInst);
	}
}

void DepthOfFieldEditWindowNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	if (eventType == ePA_SNE_SliderMoved)
	{
		if (path.GetSize() == 2)
		{
			int factor;
			argument->GetData(MkSliderControlNode::ArgKey_SliderPos, factor, 0);

			bool resetValue = false;
			if (path[1].Equals(0, L"SL_Position"))
			{
				SREPO.SetDepthOfFieldPosition(static_cast<float>(factor) / 800.f);
				resetValue = true;
			}
			else if (path[1].Equals(0, L"SL_Range"))
			{
				SREPO.SetDepthOfFieldRange(static_cast<float>(factor));
				resetValue = true;
			}
			else if (path[1].Equals(0, L"SL_Debug"))
			{
				SREPO.SetDepthOfFieldDebugBlend(static_cast<float>(factor) / 100.f);
				resetValue = true;
			}

			if (resetValue && (m_RootNode != NULL))
			{
				MkPanel* mainPanel = m_RootNode->GetPanel(L"Main");
				if (mainPanel != NULL)
				{
					mainPanel->SetUserDefinedProperty(L"g_DOF_Factor", SREPO.GetDepthOfFieldPosition(), SREPO.GetDepthOfFieldRange(), SREPO.GetDepthOfFieldDebugBlend());
				}
				return;
			}
		}
	}
	else if (eventType == ePA_SNE_CursorLBtnReleased)
	{
		if (path.GetSize() == 2)
		{
			if (path[1].Equals(0, L"Btn_Reset"))
			{
				SREPO.ResetDepthOfFieldValue();
				_ResetEffectValue();
			}
		}
	}

	MkTitleBarControlNode::SendNodeReportTypeEvent(eventType, path, argument);
}

void DepthOfFieldEditWindowNode::Activate(void)
{
	_ResetEffectValue();

	MkTitleBarControlNode::Activate();
}

//------------------------------------------------------------------------------------------------//

void DepthOfFieldEditWindowNode::_ResetEffectValue(void)
{
	MkSceneNode* bodyFrame = GetChildNode(L"BodyFrame");

	MkSliderControlNode* sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_Position"));
	sliderNode->SetSliderValue(static_cast<int>(SREPO.GetDepthOfFieldPosition() * 800.f));
	
	sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_Range"));
	sliderNode->SetSliderValue(static_cast<int>(SREPO.GetDepthOfFieldRange()));
}