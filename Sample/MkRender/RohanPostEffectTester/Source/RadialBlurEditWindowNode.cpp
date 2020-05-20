
#include "MkCore_MkDataNode.h"

#include "MkPA_MkBodyFrameControlNode.h"
#include "MkPA_MkSliderControlNode.h"
#include "MkPA_MkWindowFactory.h"

#include "SettingRepository.h"
#include "RadialBlurEditWindowNode.h"


//------------------------------------------------------------------------------------------------//

void RadialBlurEditWindowNode::SetUp(MkSceneNode* rootNode)
{
	m_RootNode = rootNode;

	// title bar
	SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 300.f, true);
	SetIcon(MkWindowThemeData::eIT_Default);
	SetCaption(L"편집 : 원형 블러", eRAP_LeftCenter);
	SetAlignmentPosition(eRAP_RightTop);
	SetAlignmentOffset(MkFloat2(-10.f, -10.f));
	
	// body frame
	const float HEIGHT = 280.f;

	MkBodyFrameControlNode* bodyFrame = MkBodyFrameControlNode::CreateChildNode(this, L"BodyFrame");
	bodyFrame->SetBodyFrame
		(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(224.f, HEIGHT));

	{
		MkPanel& panel = bodyFrame->CreatePanel(L"ST_XY");
		panel.SetLocalPosition(MkFloat2(10.f, HEIGHT - 50.f));
		panel.SetLocalDepth(-1.f);
		panel.SetTextMsg(L"초점 위치 변경 : 우클릭 드래그");
	}

	// start range
	{
		MkPanel& panel = bodyFrame->CreatePanel(L"ST_StartRange");
		panel.SetLocalPosition(MkFloat2(10.f, HEIGHT - 100.f));
		panel.SetLocalDepth(-1.f);
		panel.SetTextMsg(L"블러 시작 반경");

		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_StartRange");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, 1, 100, static_cast<int>(SREPO.GetRadialBlurStartRange() * 100.f));
		sliderNode->SetAlignmentPosition(eRAP_LeftTop);
		sliderNode->SetAlignmentOffset(MkFloat2(10.f, -130.f));
		sliderNode->SetLocalDepth(-1.f);
	}

	// power
	{
		MkPanel& panel = bodyFrame->CreatePanel(L"ST_Power");
		panel.SetLocalPosition(MkFloat2(10.f, HEIGHT - 190.f));
		panel.SetLocalDepth(-1.f);
		panel.SetTextMsg(L"블러 강도");

		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_Power");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, 1, 20, static_cast<int>(SREPO.GetRadialBlurPower() * 1000.f));
		sliderNode->SetAlignmentPosition(eRAP_LeftTop);
		sliderNode->SetAlignmentOffset(MkFloat2(10.f, -220.f));
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

void RadialBlurEditWindowNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	if (eventType == ePA_SNE_SliderMoved)
	{
		if (path.GetSize() == 2)
		{
			int factor;
			argument->GetData(MkSliderControlNode::ArgKey_SliderPos, factor, 0);

			bool resetValue = false;
			if (path[1].Equals(0, L"SL_StartRange"))
			{
				SREPO.SetRadialBlurStartRange(static_cast<float>(factor) / 100.f);
				resetValue = true;
			}
			else if (path[1].Equals(0, L"SL_Power"))
			{
				SREPO.SetRadialBlurPower(static_cast<float>(factor) / 1000.f);
				resetValue = true;
			}

			if (resetValue && (m_RootNode != NULL))
			{
				MkPanel* mainPanel = m_RootNode->GetPanel(L"Main");
				if (mainPanel != NULL)
				{
					mainPanel->SetUserDefinedProperty(L"g_RadialBlurFactor", SREPO.GetRadialBlurPosition().x, SREPO.GetRadialBlurPosition().y, SREPO.GetRadialBlurStartRange(), SREPO.GetRadialBlurPower());
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
				SREPO.ResetRadialBlurValue();
				_ResetEffectValue();
			}
		}
	}

	MkTitleBarControlNode::SendNodeReportTypeEvent(eventType, path, argument);
}

void RadialBlurEditWindowNode::Activate(void)
{
	_ResetEffectValue();

	MkTitleBarControlNode::Activate();
}

//------------------------------------------------------------------------------------------------//

void RadialBlurEditWindowNode::_ResetEffectValue(void)
{
	MkSceneNode* bodyFrame = GetChildNode(L"BodyFrame");

	MkSliderControlNode* sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_StartRange"));
	sliderNode->SetSliderValue(static_cast<int>(SREPO.GetRadialBlurStartRange() * 100.f));
	
	sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_Power"));
	sliderNode->SetSliderValue(static_cast<int>(SREPO.GetRadialBlurPower() * 1000.f));
}