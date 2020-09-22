
#include "MkCore_MkDataNode.h"

#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkBodyFrameControlNode.h"
#include "MkPA_MkListBoxControlNode.h"
#include "MkPA_MkDropDownListControlNode.h"
#include "MkPA_MkSliderControlNode.h"
#include "MkPA_MkWindowFactory.h"

#include "SettingRepository.h"
#include "DepthFogEditWindowNode.h"


//------------------------------------------------------------------------------------------------//

void DepthFogEditWindowNode::SetUp(MkSceneNode* rootNode)
{
	m_RootNode = rootNode;

	// title bar
	SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 300.f, true);
	SetIcon(MkWindowThemeData::eIT_Default);
	SetCaption(L"편집 : 깊이 안개", eRAP_LeftCenter);
	SetAlignmentPosition(eRAP_RightTop);
	SetAlignmentOffset(MkFloat2(-10.f, -10.f));
	
	// body frame
	const float HEIGHT = 240.f;

	MkBodyFrameControlNode* bodyFrame = MkBodyFrameControlNode::CreateChildNode(this, L"BodyFrame");
	bodyFrame->SetBodyFrame
		(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(224.f, HEIGHT));

	// graph
	{
		MkSceneNode* graphNode = new MkSceneNode(L"Graph");
		bodyFrame->AttachChildNode(graphNode);
		MkLineShape& lineShape = graphNode->CreateLine(L"LS_Axis");
		MkArray<MkFloat2> lineVertices(4);
		lineVertices.PushBack(MkFloat2(0.f, 25.f));
		lineVertices.PushBack(MkFloat2(0.f, 0.f));
		lineVertices.PushBack(MkFloat2(200.f, 0.f));
		lineVertices.PushBack(MkFloat2(200.f, 25.f));
		lineShape.AddLine(lineVertices);
		lineShape.AddLine(MkFloat2(40.f, 0.f), MkFloat2(40.f, 25.f));
		lineShape.SetColor(MkColor::LightGray);
		graphNode->SetLocalPosition(MkFloat2(15.f, HEIGHT - 50.f));
		graphNode->SetLocalDepth(-0.5f);
	}

	{
		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_BeginPos");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, -200, 1000, static_cast<int>(SREPO.GetDepthFogGraphBegin() * 800.f));
		sliderNode->SetLocalPosition(MkFloat2(12.f, HEIGHT - 80.f));
		sliderNode->SetLocalDepth(-1.f);
	}
	{
		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_EndPos");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, -200, 1000, static_cast<int>(SREPO.GetDepthFogGraphEnd() * 800.f));
		sliderNode->SetLocalPosition(MkFloat2(12.f, HEIGHT - 110.f));
		sliderNode->SetLocalDepth(-1.f);
	}

	// color
	{
		MkWindowFactory wndFactory;
		wndFactory.SetMinClientSizeForButton(MkFloat2(100.f, 12.f));

		MkWindowBaseNode* btnInst = wndFactory.CreateButtonTypeNode(L"Btn_Color", MkWindowThemeData::eCT_NormalBtn, MkStr::EMPTY);
		btnInst->SetLocalPosition(MkFloat2(110.f, HEIGHT - 160.f));
		bodyFrame->AttachChildNode(btnInst);
	}

	// method
	{
		MkDropDownListControlNode* ddList;
		ddList = MkDropDownListControlNode::CreateChildNode(bodyFrame, L"DD_Method");
		ddList->SetDropDownList(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 80.f, 12);
		ddList->SetLocalDepth(-3.f);
		ddList->SetLocalPosition(MkFloat2(10.f, HEIGHT - 160.f));
		ddList->AddItem(L"B", L"Blend");
		ddList->AddItem(L"M", L"Multiply");
		ddList->AddItem(L"A", L"Add");
	}

	// alpha
	{
		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_Alpha");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, 0, 100, static_cast<int>(SREPO.GetDepthFogColorA() * 100.f));
		sliderNode->SetLocalPosition(MkFloat2(12.f, HEIGHT - 190.f));
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

void DepthFogEditWindowNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	if (eventType == ePA_SNE_SliderMoved)
	{
		if (path.GetSize() == 2)
		{
			int factor;
			argument->GetData(MkSliderControlNode::ArgKey_SliderPos, factor, 0);

			MkPanel* mainPanel = (m_RootNode != NULL) ? m_RootNode->GetPanel(L"Main") : NULL;

			if (path[1].Equals(0, L"SL_BeginPos"))
			{
				SREPO.SetDepthFogGraphBegin(static_cast<float>(factor) / 800.f);

				_UpdateGraph();
				if (mainPanel != NULL)
				{
					mainPanel->SetUserDefinedProperty(L"g_DFogGraph", SREPO.GetDepthFogGraphBegin(), SREPO.GetDepthFogGraphEnd());
				}
			}
			else if (path[1].Equals(0, L"SL_EndPos"))
			{
				SREPO.SetDepthFogGraphEnd(static_cast<float>(factor) / 800.f);

				_UpdateGraph();
				if (mainPanel != NULL)
				{
					mainPanel->SetUserDefinedProperty(L"g_DFogGraph", SREPO.GetDepthFogGraphBegin(), SREPO.GetDepthFogGraphEnd());
				}
			}
			else if (path[1].Equals(0, L"SL_Alpha"))
			{
				SREPO.SetDepthFogColorA(static_cast<float>(factor) / 100.f);

				if (mainPanel != NULL)
				{
					if (SREPO.GetDepthFogMixerB() > 0.f)
					{
						mainPanel->SetUserDefinedProperty(L"g_DFogBlendColor", SREPO.GetDepthFogColorR(), SREPO.GetDepthFogColorG(), SREPO.GetDepthFogColorB(), SREPO.GetDepthFogColorA());
					}
					else if (SREPO.GetDepthFogMixerM() > 0.f)
					{
						mainPanel->SetUserDefinedProperty(L"g_DFogMultColor", SREPO.GetDepthFogColorR(), SREPO.GetDepthFogColorG(), SREPO.GetDepthFogColorB(), SREPO.GetDepthFogColorA());
					}
					else if (SREPO.GetDepthFogMixerA() > 0.f)
					{
						mainPanel->SetUserDefinedProperty(L"g_DFogAddColor", SREPO.GetDepthFogColorR(), SREPO.GetDepthFogColorG(), SREPO.GetDepthFogColorB(), SREPO.GetDepthFogColorA());
					}
				}
			}
			return;
		}
	}
	else if (eventType == ePA_SNE_DropDownItemSet)
	{
		if (path.GetSize() == 2)
		{
			MkHashStr buffer;
			if (argument->GetDataEx(MkListBoxControlNode::ArgKey_Item, buffer, 0) && (!buffer.Empty()))
			{
				if (buffer.Equals(0, L"B"))
				{
					SREPO.SetDepthFogMixerB(1.f);
					SREPO.SetDepthFogMixerM(0.f);
					SREPO.SetDepthFogMixerA(0.f);
				}
				else if (buffer.Equals(0, L"M"))
				{
					SREPO.SetDepthFogMixerB(0.f);
					SREPO.SetDepthFogMixerM(1.f);
					SREPO.SetDepthFogMixerA(0.f);
				}
				else if (buffer.Equals(0, L"A"))
				{
					SREPO.SetDepthFogMixerB(0.f);
					SREPO.SetDepthFogMixerM(0.f);
					SREPO.SetDepthFogMixerA(1.f);
				}

				if (m_RootNode != NULL)
				{
					MkPanel* mainPanel = m_RootNode->GetPanel(L"Main");
					if (mainPanel != NULL)
					{
						mainPanel->SetUserDefinedProperty(L"g_DFogMixer", SREPO.GetDepthFogMixerB(), SREPO.GetDepthFogMixerM(), SREPO.GetDepthFogMixerA());

						if (SREPO.GetDepthFogMixerB() > 0.f)
						{
							mainPanel->SetUserDefinedProperty(L"g_DFogBlendColor", SREPO.GetDepthFogColorR(), SREPO.GetDepthFogColorG(), SREPO.GetDepthFogColorB(), SREPO.GetDepthFogColorA());
						}
						else if (SREPO.GetDepthFogMixerM() > 0.f)
						{
							mainPanel->SetUserDefinedProperty(L"g_DFogMultColor", SREPO.GetDepthFogColorR(), SREPO.GetDepthFogColorG(), SREPO.GetDepthFogColorB(), SREPO.GetDepthFogColorA());
						}
						else if (SREPO.GetDepthFogMixerA() > 0.f)
						{
							mainPanel->SetUserDefinedProperty(L"g_DFogAddColor", SREPO.GetDepthFogColorR(), SREPO.GetDepthFogColorG(), SREPO.GetDepthFogColorB(), SREPO.GetDepthFogColorA());
						}
					}
				}
			}
		}
	}
	else if (eventType == ePA_SNE_CursorLBtnReleased)
	{
		if (path.GetSize() == 2)
		{
			if (path[1].Equals(0, L"Btn_Color"))
			{
				CHOOSECOLOR cc;
				::ZeroMemory(&cc, sizeof(CHOOSECOLOR));

				static COLORREF crTemp[16];
				cc.lStructSize = sizeof(CHOOSECOLOR);
				cc.lpCustColors = crTemp;
				cc.Flags = 0;

				if (::ChooseColor(&cc) != 0)
				{
					MkColor color;
					color.SetByCOLORREF(cc.rgbResult);

					_SetFogColor(color);
				}
			}
			else if (path[1].Equals(0, L"Btn_Reset"))
			{
				SREPO.ResetDepthFogValue();
				_ResetEffectValue();
			}
		}
	}

	MkTitleBarControlNode::SendNodeReportTypeEvent(eventType, path, argument);
}

void DepthFogEditWindowNode::Activate(void)
{
	_ResetEffectValue();

	MkTitleBarControlNode::Activate();
}

//------------------------------------------------------------------------------------------------//

void DepthFogEditWindowNode::_ResetEffectValue(void)
{
	MkSceneNode* bodyFrame = GetChildNode(L"BodyFrame");
	{
		MkSliderControlNode* sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_BeginPos"));
		sliderNode->SetSliderValue(static_cast<int>(SREPO.GetDepthFogGraphBegin() * 800.f));
	}
	{
		MkSliderControlNode* sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_EndPos"));
		sliderNode->SetSliderValue(static_cast<int>(SREPO.GetDepthFogGraphEnd() * 800.f));
	}

	{
		MkDropDownListControlNode* ddList = dynamic_cast<MkDropDownListControlNode*>(bodyFrame->GetChildNode(L"DD_Method"));
		if (SREPO.GetDepthFogMixerB() > 0.f)
		{
			ddList->SetTargetItemKey(L"B");
		}
		else if (SREPO.GetDepthFogMixerM() > 0.f)
		{
			ddList->SetTargetItemKey(L"M");
		}
		else if (SREPO.GetDepthFogMixerA() > 0.f)
		{
			ddList->SetTargetItemKey(L"A");
		}
	}

	{
		MkSliderControlNode* sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_Alpha"));
		sliderNode->SetSliderValue(static_cast<int>(SREPO.GetDepthFogColorA() * 100.f));
	}

	_SetFogColor(MkColor(SREPO.GetDepthFogColorR(), SREPO.GetDepthFogColorG(), SREPO.GetDepthFogColorB()));
}

void DepthFogEditWindowNode::_UpdateGraph(void)
{
	MkSceneNode* bodyFrame = GetChildNode(L"BodyFrame");
	MkSceneNode* graphNode = bodyFrame->GetChildNode(L"Graph");

	float beginPos = SREPO.GetDepthFogGraphBegin() * 160.f + 40.f;
	float endPos = max(beginPos, SREPO.GetDepthFogGraphEnd() * 160.f + 40.f);

	MkLineShape& lineShape = graphNode->CreateLine(L"LS_Value");
	MkArray<MkFloat2> lineVertices(4);

	if (beginPos > 0.f)
	{
		lineVertices.PushBack(MkFloat2(0.f, 0.f));
	}
	lineVertices.PushBack(MkFloat2(beginPos, 0.f));
	lineVertices.PushBack(MkFloat2(endPos, 25.f));
	if (endPos < 200.f)
	{
		lineVertices.PushBack(MkFloat2(200.f, 25.f));
	}

	lineShape.AddLine(lineVertices);
	lineShape.SetColor(MkColor::Red);
	lineShape.SetLocalDepth(-0.5f);
}

void DepthFogEditWindowNode::_SetFogColor(const MkColor& color)
{
	SREPO.SetDepthFogColorR(color.r);
	SREPO.SetDepthFogColorG(color.g);
	SREPO.SetDepthFogColorB(color.b);

	MkPanel* mainPanel = (m_RootNode != NULL) ? m_RootNode->GetPanel(L"Main") : NULL;
	if (mainPanel != NULL)
	{
		if (SREPO.GetDepthFogMixerB() > 0.f)
		{
			mainPanel->SetUserDefinedProperty(L"g_DFogBlendColor", SREPO.GetDepthFogColorR(), SREPO.GetDepthFogColorG(), SREPO.GetDepthFogColorB(), SREPO.GetDepthFogColorA());
		}
		else if (SREPO.GetDepthFogMixerM() > 0.f)
		{
			mainPanel->SetUserDefinedProperty(L"g_DFogMultColor", SREPO.GetDepthFogColorR(), SREPO.GetDepthFogColorG(), SREPO.GetDepthFogColorB(), SREPO.GetDepthFogColorA());
		}
		else if (SREPO.GetDepthFogMixerA() > 0.f)
		{
			mainPanel->SetUserDefinedProperty(L"g_DFogAddColor", SREPO.GetDepthFogColorR(), SREPO.GetDepthFogColorG(), SREPO.GetDepthFogColorB(), SREPO.GetDepthFogColorA());
		}
	}

	_UpdateColorTag();
}

void DepthFogEditWindowNode::_UpdateColorTag(void)
{
	MkSceneNode* bodyFrame = GetChildNode(L"BodyFrame");
	MkSceneNode* btnNode = bodyFrame->GetChildNode(L"Btn_Color");
	if (btnNode != NULL)
	{
		MkLineShape& lineShape = btnNode->CreateLine(L"LS_Color");
		lineShape.ReserveSegments(18);

		for (int i=0; i<12; ++i)
		{
			float height = static_cast<float>(i);
			lineShape.AddLine(MkFloat2(3.f, 3.f + height), MkFloat2(103.f, 3.f + height));
		}
		
		lineShape.SetColor(MkColor(SREPO.GetDepthFogColorR(), SREPO.GetDepthFogColorG(), SREPO.GetDepthFogColorB()));
		lineShape.SetLocalDepth(-1.f);
	}
}

//------------------------------------------------------------------------------------------------//