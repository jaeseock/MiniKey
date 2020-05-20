
//#include "MkCore_MkCheck.h"
#include "MkCore_MkPathNAme.h"

#include "MkPA_MkBodyFrameControlNode.h"
#include "MkPA_MkCheckBoxControlNode.h"
#include "MkPA_MkWindowFactory.h"
#include "MkPA_MkWindowManagerNode.h"

#include "SettingRepository.h"
#include "SceneSelectionWindowNode.h"
#include "MainControlWindowNode.h"


//------------------------------------------------------------------------------------------------//

void MainControlWindowNode::SetUp(const MkArray<MkSceneNode*>& rootNode)
{
	m_RootNode = rootNode;

	// title bar
	SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 300.f, true);
	SetIcon(MkWindowThemeData::eIT_Default);
	SetCaption(L"이펙트 적용", eRAP_LeftCenter);
	SetAlignmentPosition(eRAP_LeftTop);
	SetAlignmentOffset(MkFloat2(10.f, -10.f));
	
	// body frame
	MkBodyFrameControlNode* bodyFrame = MkBodyFrameControlNode::CreateChildNode(this, L"BodyFrame");
	bodyFrame->SetBodyFrame
		(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(320.f, 250.f));

	// btn - load, save as
	MkWindowFactory wndFactory;
	wndFactory.SetMinClientSizeForButton(MkFloat2(140.f, 12.f));
	{
		MkWindowBaseNode* btnInst = wndFactory.CreateButtonTypeNode(L"BTN_Load", MkWindowThemeData::eCT_NormalBtn, L"이펙트 설정 열기");
		btnInst->SetAlignmentPosition(eRAP_LeftTop);
		btnInst->SetAlignmentOffset(MkFloat2(10.f, -30.f));
		bodyFrame->AttachChildNode(btnInst);
	}
	{
		MkWindowBaseNode* btnInst = wndFactory.CreateButtonTypeNode(L"BTN_SaveAs", MkWindowThemeData::eCT_NormalBtn, L"다른 이름으로 저장");
		btnInst->SetAlignmentPosition(eRAP_RightTop);
		btnInst->SetAlignmentOffset(MkFloat2(-10.f, -30.f));
		bodyFrame->AttachChildNode(btnInst);
	}

	// main scene
	_AddEffect(bodyFrame, L"Color", L"원본 컬러 수정", -70.f, false);
	_AddEffect(bodyFrame, L"DepthFog", L"깊이 안개", -100.f, false);
	_AddEffect(bodyFrame, L"EdgeDetection", L"외곽선 검출", -130.f, false);
	_AddEffect(bodyFrame, L"HDR", L"HDR", -160.f, false);
	_AddEffect(bodyFrame, L"DepthOfField", L"피사계 심도", -190.f, false);
	_AddEffect(bodyFrame, L"RadialBlur", L"원형 블러", -220.f, false);
}

void MainControlWindowNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	if (eventType == ePA_SNE_CheckOn)
	{
		if (_CheckAndUpdateCheckBox(path, L"Color", true))
		{
			SREPO.SetColorEditEnable(true);
			_SetShaderEffectEnableOfMainPanel(eDST_MainScene, true);
			return;
		}
		else if (_CheckAndUpdateCheckBox(path, L"DepthFog", true))
		{
			SREPO.SetDepthFogEnable(true);
			_SetShaderEffectEnableOfMainPanel(eDST_DepthFog, true);
			return;
		}
		else if (_CheckAndUpdateCheckBox(path, L"EdgeDetection", true))
		{
			SREPO.SetEdgeDetectionEnable(true);
			_SetShaderEffectEnableOfMainPanel(eDST_EdgeDetection, true);
			return;
		}
		else if (_CheckAndUpdateCheckBox(path, L"HDR", true))
		{
			SREPO.SetFakeHDREnable(true);
			_SetShaderEffectEnableOfMainPanel(eDST_HDR_Blend, true);
			return;
		}
		else if (_CheckAndUpdateCheckBox(path, L"DepthOfField", true))
		{
			SREPO.SetDepthOfFieldEnable(true);
			_SetShaderEffectEnableOfMainPanel(eDST_DepthOfField, true);
			return;
		}
		else if (_CheckAndUpdateCheckBox(path, L"RadialBlur", true))
		{
			SREPO.SetRadialBlurEnable(true);
			_SetShaderEffectEnableOfMainPanel(eDST_RadialBlur, true);
			return;
		}
	}
	else if (eventType == ePA_SNE_CheckOff)
	{
		if (_CheckAndUpdateCheckBox(path, L"Color", false))
		{
			SREPO.SetColorEditEnable(false);
			_SetShaderEffectEnableOfMainPanel(eDST_MainScene, false);
			return;
		}
		else if (_CheckAndUpdateCheckBox(path, L"DepthFog", false))
		{
			SREPO.SetDepthFogEnable(false);
			_SetShaderEffectEnableOfMainPanel(eDST_DepthFog, false);
			return;
		}
		else if (_CheckAndUpdateCheckBox(path, L"EdgeDetection", false))
		{
			SREPO.SetEdgeDetectionEnable(false);
			_SetShaderEffectEnableOfMainPanel(eDST_EdgeDetection, false);
			return;
		}
		else if (_CheckAndUpdateCheckBox(path, L"HDR", false))
		{
			SREPO.SetFakeHDREnable(false);
			_SetShaderEffectEnableOfMainPanel(eDST_HDR_Blend, false);
			return;
		}
		else if (_CheckAndUpdateCheckBox(path, L"DepthOfField", false))
		{
			SREPO.SetDepthOfFieldEnable(false);
			_SetShaderEffectEnableOfMainPanel(eDST_DepthOfField, false);
			return;
		}
		else if (_CheckAndUpdateCheckBox(path, L"RadialBlur", false))
		{
			SREPO.SetRadialBlurEnable(false);
			_SetShaderEffectEnableOfMainPanel(eDST_RadialBlur, false);
			return;
		}
	}
	else if (eventType == ePA_SNE_CursorLBtnReleased)
	{
		if (_CheckAndActivateEditWindow(path, L"Edit_Color"))
			return;
		else if (_CheckAndActivateEditWindow(path, L"Edit_DepthFog"))
			return;
		else if (_CheckAndActivateEditWindow(path, L"Edit_EdgeDetection"))
			return;
		else if (_CheckAndActivateEditWindow(path, L"Edit_HDR"))
			return;
		else if (_CheckAndActivateEditWindow(path, L"Edit_DepthOfField"))
			return;
		else if (_CheckAndActivateEditWindow(path, L"Edit_RadialBlur"))
			return;
		else if (path.GetSize() == 2)
		{
			if (path[1].Equals(0, L"BTN_Load"))
			{
				MkPathName fullPath;
				if (fullPath.GetSingleFilePathFromDialog(L"txt"))
				{
					if (SREPO.LoadEffectSetting(fullPath))
					{
						UpdateEffectSetting();
					}
				}
			}
			else if (path[1].Equals(0, L"BTN_SaveAs"))
			{
				MkPathName fullPath;
				if (fullPath.GetSaveFilePathFromDialog(L"txt"))
				{
					if (SREPO.SaveEffectSetting(fullPath) &&
						fullPath.ChangeFileExtension(L"png"))
					{
						m_RootNode[eDST_Final]->GetChildNode(L"<Scene>")->GetPanel(L"Main")->GetTexturePtr()->SaveToPNG(fullPath);
					}
				}
			}
		}
	}

	MkTitleBarControlNode::SendNodeReportTypeEvent(eventType, path, argument);
}

void MainControlWindowNode::UpdateEffectSetting(void)
{
	MkWindowManagerNode* mgrNode = GetWindowManagerNode();
	SceneSelectionWindowNode* selWin = dynamic_cast<SceneSelectionWindowNode*>(mgrNode->GetChildNode(L"SceneSelection"));
	selWin->SelectTargetScene();

	// check box 반영, edit window 끔
	_UpdateCheckBox(L"Color", SREPO.GetColorEditEnable(), mgrNode);
	_UpdateCheckBox(L"DepthFog", SREPO.GetDepthFogEnable(), mgrNode);
	_UpdateCheckBox(L"EdgeDetection", SREPO.GetEdgeDetectionEnable(), mgrNode);
	_UpdateCheckBox(L"HDR", SREPO.GetFakeHDREnable(), mgrNode);
	_UpdateCheckBox(L"DepthOfField", SREPO.GetDepthOfFieldEnable(), mgrNode);
	_UpdateCheckBox(L"RadialBlur", SREPO.GetRadialBlurEnable(), mgrNode);
}

//------------------------------------------------------------------------------------------------//

void MainControlWindowNode::_AddEffect(MkSceneNode* bodyFrame, const MkStr& name, const MkStr& msg, float yOffset, bool check)
{
	MkCheckBoxControlNode* checkBox = MkCheckBoxControlNode::CreateChildNode(bodyFrame, name);
	checkBox->SetCheckBox(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, msg, check);
	checkBox->SetLocalDepth(-1.f);
	checkBox->SetAlignmentPosition(eRAP_LeftTop);
	checkBox->SetAlignmentOffset(MkFloat2(10.f, yOffset));

	MkStr btnName = L"Edit_" + name;
	MkWindowFactory wndFactory;
	MkWindowBaseNode* btnInst = wndFactory.CreateButtonTypeNode(btnName, MkWindowThemeData::eCT_OKBtn, L"에디터 열기");
	btnInst->SetAlignmentPosition(eRAP_RightTop);
	btnInst->SetAlignmentOffset(MkFloat2(-10.f, yOffset));
	btnInst->SetEnable(check);
	bodyFrame->AttachChildNode(btnInst);
}

bool MainControlWindowNode::_CheckAndUpdateCheckBox(const MkArray<MkHashStr>& path, const MkHashStr& key, bool onOff)
{
	if (path.GetSize() == 2)
	{
		if (path[1].Equals(0, key))
		{
			_UpdateEditBtnEnable(path[1].GetString(), onOff);
			return true;
		}
	}
	return false;
}

void MainControlWindowNode::_UpdateCheckBox(const MkHashStr& key, bool onOff, MkWindowManagerNode* mgrNode)
{
	MkCheckBoxControlNode* cbNode = dynamic_cast<MkCheckBoxControlNode*>(GetChildNode(L"BodyFrame")->GetChildNode(key));
	if (cbNode != NULL)
	{
		cbNode->SetCheck(onOff);
	}

	MkHashStr editWindowName = L"Edit_" + key.GetString();
	if (mgrNode->IsActivating(editWindowName))
	{
		mgrNode->DeactivateWindow(editWindowName);
	}
}

void MainControlWindowNode::_UpdateEditBtnEnable(const MkStr& key, bool onOff)
{
	MkHashStr btnAndEditWindowName = L"Edit_" + key;
	MkWindowBaseNode* btnNode = dynamic_cast<MkWindowBaseNode*>(GetChildNode(L"BodyFrame")->GetChildNode(btnAndEditWindowName));
	if (btnNode != NULL)
	{
		btnNode->SetEnable(onOff);
	}

	if (!onOff)
	{
		MkWindowManagerNode* mgrNode = GetWindowManagerNode();
		if (mgrNode->IsActivating(btnAndEditWindowName))
		{
			mgrNode->DeactivateWindow(btnAndEditWindowName);
		}
	}
}

bool MainControlWindowNode::_CheckAndActivateEditWindow(const MkArray<MkHashStr>& path, const MkHashStr& key)
{
	if (path.GetSize() == 2)
	{
		if (path[1].Equals(0, key))
		{
			MkWindowManagerNode* mgrNode = GetWindowManagerNode();
			if (!mgrNode->IsActivating(path[1]))
			{
				mgrNode->ActivateWindow(path[1]);
			}
			return true;
		}
	}
	return false;
}

MkPanel* MainControlWindowNode::_GetMainPanel(eDrawStepType drawStep)
{
	return m_RootNode.IsValidIndex(drawStep) ? m_RootNode[drawStep]->GetPanel(L"Main") : NULL;
}

void MainControlWindowNode::_SetShaderEffectEnableOfMainPanel(eDrawStepType drawStep, bool enable)
{
	MkPanel* mainPanel = _GetMainPanel(drawStep);
	if (mainPanel != NULL)
	{
		mainPanel->SetShaderEffectEnable(enable);
	}
}

//------------------------------------------------------------------------------------------------//
