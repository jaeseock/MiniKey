
#include "MkCore_MkDataNode.h"

#include "MkPA_MkBodyFrameControlNode.h"
#include "MkPA_MkSliderControlNode.h"
#include "MkPA_MkWindowFactory.h"

#include "SettingRepository.h"
#include "HDREditWindowNode.h"


//------------------------------------------------------------------------------------------------//

void HDREditWindowNode::SetUp(MkSceneNode* rootNode)
{
	m_RootNode = rootNode;

	// title bar
	SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 300.f, true);
	SetIcon(MkWindowThemeData::eIT_Default);
	SetCaption(L"편집 : HDR", eRAP_LeftCenter);
	SetAlignmentPosition(eRAP_RightTop);
	SetAlignmentOffset(MkFloat2(-10.f, -10.f));
	
	// body frame
	const float HEIGHT = 230.f;

	MkBodyFrameControlNode* bodyFrame = MkBodyFrameControlNode::CreateChildNode(this, L"BodyFrame");
	bodyFrame->SetBodyFrame
		(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(224.f, HEIGHT));

	// amplifier
	{
		MkPanel& panel = bodyFrame->CreatePanel(L"ST_PowerHigh");
		panel.SetLocalPosition(MkFloat2(10.f, HEIGHT - 50.f));
		panel.SetLocalDepth(-1.f);
		panel.SetTextMsg(L"빛 강조 비율");

		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_PowerHigh");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, 0, 100, static_cast<int>(SREPO.GetFakeHDRPowerHigh() * 100.f));
		sliderNode->SetAlignmentPosition(eRAP_LeftTop);
		sliderNode->SetAlignmentOffset(MkFloat2(10.f, -80.f));
		sliderNode->SetLocalDepth(-1.f);
	}

	// blend
	{
		MkPanel& panel = bodyFrame->CreatePanel(L"ST_PowerLow");
		panel.SetLocalPosition(MkFloat2(10.f, HEIGHT - 140.f));
		panel.SetLocalDepth(-1.f);
		panel.SetTextMsg(L"기저 이미지 비율");

		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_PowerLow");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, 0, 100, static_cast<int>(SREPO.GetFakeHDRPowerLow() * 100.f));
		sliderNode->SetAlignmentPosition(eRAP_LeftTop);
		sliderNode->SetAlignmentOffset(MkFloat2(10.f, -170.f));
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

void HDREditWindowNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	if (eventType == ePA_SNE_SliderMoved)
	{
		if (path.GetSize() == 2)
		{
			int factor;
			argument->GetData(MkSliderControlNode::ArgKey_SliderPos, factor, 0);

			bool resetValue = false;
			if (path[1].Equals(0, L"SL_PowerHigh"))
			{
				SREPO.SetFakeHDRPowerHigh(static_cast<float>(factor) / 100.f);
				resetValue = true;
			}
			else if (path[1].Equals(0, L"SL_PowerLow"))
			{
				SREPO.SetFakeHDRPowerLow(static_cast<float>(factor) / 100.f);
				resetValue = true;
			}

			if (resetValue && (m_RootNode != NULL))
			{
				MkPanel* mainPanel = m_RootNode->GetPanel(L"Main");
				if (mainPanel != NULL)
				{
					mainPanel->SetUserDefinedProperty(L"g_HDRPower", SREPO.GetFakeHDRPowerHigh(), SREPO.GetFakeHDRPowerLow());
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
				SREPO.ResetFakeHDRValue();
				_ResetEffectValue();
			}
		}
	}
	
	MkTitleBarControlNode::SendNodeReportTypeEvent(eventType, path, argument);
}

void HDREditWindowNode::Activate(void)
{
	_ResetEffectValue();

	MkTitleBarControlNode::Activate();
}

//------------------------------------------------------------------------------------------------//

void HDREditWindowNode::_ResetEffectValue(void)
{
	MkSceneNode* bodyFrame = GetChildNode(L"BodyFrame");

	MkSliderControlNode* sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_PowerHigh"));
	sliderNode->SetSliderValue(static_cast<int>(SREPO.GetFakeHDRPowerHigh() * 100.f));
	
	sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_PowerLow"));
	sliderNode->SetSliderValue(static_cast<int>(SREPO.GetFakeHDRPowerLow() * 100.f));
}