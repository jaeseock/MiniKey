
#include "MkCore_MkDataNode.h"

#include "MkPA_MkBodyFrameControlNode.h"
#include "MkPA_MkSliderControlNode.h"
#include "MkPA_MkWindowFactory.h"

#include "SettingRepository.h"
#include "ColorEditWindowNode.h"


//------------------------------------------------------------------------------------------------//

void ColorEditWindowNode::SetUp(MkSceneNode* rootNode)
{
	m_RootNode = rootNode;

	// title bar
	SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 300.f, true);
	SetIcon(MkWindowThemeData::eIT_Default);
	SetCaption(L"편집 : 컬러", eRAP_LeftCenter);
	SetAlignmentPosition(eRAP_RightTop);
	SetAlignmentOffset(MkFloat2(-10.f, -10.f));
	
	// body frame
	const float HEIGHT = 490.f;

	MkBodyFrameControlNode* bodyFrame = MkBodyFrameControlNode::CreateChildNode(this, L"BodyFrame");
	bodyFrame->SetBodyFrame
		(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(224.f, HEIGHT));

	// brightness
	{
		MkPanel& panel = bodyFrame->CreatePanel(L"ST_Brightness");
		panel.SetLocalPosition(MkFloat2(10.f, HEIGHT - 50.f));
		panel.SetLocalDepth(-1.f);
		panel.SetTextMsg(L"Brightness");

		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_Brightness");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, -100, 200, _ValueToFactor(SREPO.GetColorEditBrightness()));
		sliderNode->SetAlignmentPosition(eRAP_LeftTop);
		sliderNode->SetAlignmentOffset(MkFloat2(10.f, -80.f));
		sliderNode->SetLocalDepth(-1.f);
	}

	// saturation
	{
		MkPanel& panel = bodyFrame->CreatePanel(L"ST_Saturation");
		panel.SetLocalPosition(MkFloat2(10.f, HEIGHT - 140.f));
		panel.SetLocalDepth(-1.f);
		panel.SetTextMsg(L"Saturation");

		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_Saturation");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, -100, 200, _ValueToFactor(SREPO.GetColorEditSaturation()));
		sliderNode->SetAlignmentPosition(eRAP_LeftTop);
		sliderNode->SetAlignmentOffset(MkFloat2(10.f, -170.f));
		sliderNode->SetLocalDepth(-1.f);
	}

	// contrast
	{
		MkPanel& panel = bodyFrame->CreatePanel(L"ST_Contrast");
		panel.SetLocalPosition(MkFloat2(10.f, HEIGHT - 230.f));
		panel.SetLocalDepth(-1.f);
		panel.SetTextMsg(L"Contrast");
		
		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_Contrast");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, -100, 200, _ValueToFactor(SREPO.GetColorEditContrast()));
		sliderNode->SetAlignmentPosition(eRAP_LeftTop);
		sliderNode->SetAlignmentOffset(MkFloat2(10.f, -260.f));
		sliderNode->SetLocalDepth(-1.f);
	}

	// color balance
	{
		MkPanel& panel = bodyFrame->CreatePanel(L"ST_ColorBalance");
		panel.SetLocalPosition(MkFloat2(10.f, HEIGHT - 320.f));
		panel.SetLocalDepth(-1.f);
		panel.SetTextMsg(L"Color balance(R/G/B)");
		
		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_ColorBalanceR");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, -100, 200, static_cast<int>(SREPO.GetColorEditColorBalanceR() * 100.f));
		sliderNode->SetAlignmentPosition(eRAP_LeftTop);
		sliderNode->SetAlignmentOffset(MkFloat2(10.f, -350.f));
		sliderNode->SetLocalDepth(-1.f);

		sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_ColorBalanceG");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, -100, 200, static_cast<int>(SREPO.GetColorEditColorBalanceG() * 100.f));
		sliderNode->SetAlignmentPosition(eRAP_LeftTop);
		sliderNode->SetAlignmentOffset(MkFloat2(10.f, -390.f));
		sliderNode->SetLocalDepth(-1.f);

		sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_ColorBalanceB");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, -100, 200, static_cast<int>(SREPO.GetColorEditColorBalanceB() * 100.f));
		sliderNode->SetAlignmentPosition(eRAP_LeftTop);
		sliderNode->SetAlignmentOffset(MkFloat2(10.f, -430.f));
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

void ColorEditWindowNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	if (eventType == ePA_SNE_SliderMoved)
	{
		if (path.GetSize() == 2)
		{
			int factor;
			argument->GetData(MkSliderControlNode::ArgKey_SliderPos, factor, 0);

			bool resetValue = false;
			if (path[1].Equals(0, L"SL_Brightness"))
			{
				SREPO.SetColorEditBrightness(_FactorToValue(factor));
				resetValue = true;
			}
			else if (path[1].Equals(0, L"SL_Saturation"))
			{
				SREPO.SetColorEditSaturation(_FactorToValue(factor));
				resetValue = true;
			}
			else if (path[1].Equals(0, L"SL_Contrast"))
			{
				SREPO.SetColorEditContrast(_FactorToValue(factor));
				resetValue = true;
			}
			else if (path[1].Equals(0, L"SL_ColorBalanceR"))
			{
				SREPO.SetColorEditColorBalanceR(factor / 100.f);
				resetValue = true;
			}
			else if (path[1].Equals(0, L"SL_ColorBalanceG"))
			{
				SREPO.SetColorEditColorBalanceG(factor / 100.f);
				resetValue = true;
			}
			else if (path[1].Equals(0, L"SL_ColorBalanceB"))
			{
				SREPO.SetColorEditColorBalanceB(factor / 100.f);
				resetValue = true;
			}

			if (resetValue && (m_RootNode != NULL))
			{
				MkPanel* mainPanel = m_RootNode->GetPanel(L"Main");
				if (mainPanel != NULL)
				{
					mainPanel->SetUserDefinedProperty(L"g_ColorFactor", SREPO.GetColorEditBrightness(), SREPO.GetColorEditSaturation(), SREPO.GetColorEditContrast());
					mainPanel->SetUserDefinedProperty(L"g_ColorShift", SREPO.GetColorEditColorBalanceR(), SREPO.GetColorEditColorBalanceG(), SREPO.GetColorEditColorBalanceB());
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
				SREPO.ResetColorEditValue();
				_ResetEffectValue();
			}
		}
	}

	MkTitleBarControlNode::SendNodeReportTypeEvent(eventType, path, argument);
}

void ColorEditWindowNode::Activate(void)
{
	_ResetEffectValue();

	MkTitleBarControlNode::Activate();
}

//------------------------------------------------------------------------------------------------//

int ColorEditWindowNode::_ValueToFactor(float value)
{
	return static_cast<int>((value - 1.f) * 100.f); // (0.f <= value <= 2.f) >> (-100 <= factor <= 100)
}

float ColorEditWindowNode::_FactorToValue(int factor)
{
	return (static_cast<float>(factor) / 100.f + 1.f); // (-100 <= factor <= 100) >> (0.f <= value <= 2.f)
}

void ColorEditWindowNode::_ResetEffectValue(void)
{
	MkSceneNode* bodyFrame = GetChildNode(L"BodyFrame");

	MkSliderControlNode* sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_Brightness"));
	sliderNode->SetSliderValue(_ValueToFactor(SREPO.GetColorEditBrightness()));
	
	sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_Saturation"));
	sliderNode->SetSliderValue(_ValueToFactor(SREPO.GetColorEditSaturation()));

	sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_Contrast"));
	sliderNode->SetSliderValue(_ValueToFactor(SREPO.GetColorEditContrast()));

	sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_ColorBalanceR"));
	sliderNode->SetSliderValue(static_cast<int>(SREPO.GetColorEditColorBalanceR() * 100.f));
	sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_ColorBalanceG"));
	sliderNode->SetSliderValue(static_cast<int>(SREPO.GetColorEditColorBalanceG() * 100.f));
	sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_ColorBalanceB"));
	sliderNode->SetSliderValue(static_cast<int>(SREPO.GetColorEditColorBalanceB() * 100.f));
}

//------------------------------------------------------------------------------------------------//