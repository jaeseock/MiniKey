
//#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkBodyFrameControlNode.h"
#include "MkPA_MkCheckBoxControlNode.h"
#include "MkPA_MkListBoxControlNode.h"
#include "MkPA_MkDropDownListControlNode.h"
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
		(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(320.f, 280.f + 256.f));

	// btn - load, save as
	MkWindowFactory wndFactory;
	wndFactory.SetMinClientSizeForButton(MkFloat2(100.f, 12.f));
	{
		MkWindowBaseNode* btnInst = wndFactory.CreateButtonTypeNode(L"BTN_Load", MkWindowThemeData::eCT_NormalBtn, L"이펙트 설정 열기");
		btnInst->SetAlignmentPosition(eRAP_LeftTop);
		btnInst->SetAlignmentOffset(MkFloat2(10.f, -30.f));
		bodyFrame->AttachChildNode(btnInst);
	}
	{
		MkWindowBaseNode* btnInst = wndFactory.CreateButtonTypeNode(L"BTN_SaveAs", MkWindowThemeData::eCT_NormalBtn, L"다른 이름으로 저장");
		btnInst->SetAlignmentPosition(eRAP_LeftTop);
		btnInst->SetAlignmentOffset(MkFloat2(122.f, -30.f));
		bodyFrame->AttachChildNode(btnInst);
	}

	// view option
	{
		MkCheckBoxControlNode* checkBox = MkCheckBoxControlNode::CreateChildNode(bodyFrame, L"CB_ViewOpt");
		checkBox->SetCheckBox(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, L"보기 옵션", true);
		checkBox->SetLocalDepth(-1.f);
		checkBox->SetAlignmentPosition(eRAP_LeftTop);
		checkBox->SetAlignmentOffset(MkFloat2(240.f, -30.f));
	}

	// world
	{
		MkDropDownListControlNode* ddList;
		ddList = MkDropDownListControlNode::CreateChildNode(bodyFrame, L"DD_World");
		ddList->SetDropDownList(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 294.f, 14);
		ddList->SetLocalDepth(-3.f);
		ddList->SetAlignmentPosition(eRAP_LeftTop);
		ddList->SetAlignmentOffset(MkFloat2(10.f, -60.f));

		MkDataNode node;
		MkPathName filePath;
		filePath.ConvertToRootBasisAbsolutePath(L"WorldSetting.txt");
		if (filePath.CheckAvailable() && node.Load(filePath))
		{
			MkArray<MkHashStr> keys;
			node.GetChildNodeList(keys);
			keys.SortInAscendingOrder();

			MK_INDEXING_LOOP(keys, i)
			{
				MkDataNode& childNode = *node.GetChildNode(keys[i]);
				MkStr name;
				childNode.GetData(L"Name", name, 0);
				ddList->AddItem(keys[i], name);

				MkInt2 xy;
				if (childNode.GetData(L"XY", xy, 0))
				{
					m_MinimapNameTable.Create(keys[i], MkStr(xy.x) + L"-" + MkStr(xy.y));
				}
			}

			unsigned int defPos = keys.FindFirstInclusion(MkArraySection(0), L"#Default");
			if (defPos == MKDEF_ARRAY_ERROR)
			{
				ddList->AddItem(L"#Default", L"[전체 기본]");
			}
		}

		ddList->SetTargetItemKey(L"#Default");

		m_MinimapNameTable.Create(L"#Default", L"Def");
	}

	// minimap
	_SetMinimapPanel(m_MinimapNameTable[L"#Default"]);

	// main scene
	_AddEffect(bodyFrame, L"Color", L"원본 컬러 수정", -100.f - 256.f, false);
	_AddEffect(bodyFrame, L"DepthFog", L"깊이 안개", -130.f - 256.f, false);
	_AddEffect(bodyFrame, L"EdgeDetection", L"외곽선 검출", -160.f - 256.f, false);
	_AddEffect(bodyFrame, L"HDR", L"HDR", -190.f - 256.f, false);
	_AddEffect(bodyFrame, L"DepthOfField", L"피사계 심도", -220.f - 256.f, false);
	_AddEffect(bodyFrame, L"RadialBlur", L"원형 블러", -250.f - 256.f, false);
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
		if (path.GetSize() == 2)
		{
			if (path[1].Equals(0, L"CB_ViewOpt"))
			{
				SREPO.SetColorEditEnable(true);
				SREPO.SetDepthFogEnable(true);
				SREPO.SetFakeHDREnable(true);
				SREPO.SetDepthOfFieldEnable(true);

				UpdateEffectSetting();
				return;
			}
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
	else if (eventType == ePA_SNE_DropDownItemSet)
	{
		if (path.GetSize() == 2)
		{
			MkHashStr buffer;
			if (argument->GetDataEx(MkListBoxControlNode::ArgKey_Item, buffer, 0) && (!buffer.Empty()))
			{
				if (path[1].Equals(0, L"DD_World"))
				{
					SREPO.SetTargetRegion(buffer);

					if (m_MinimapNameTable.Exist(buffer))
					{
						_SetMinimapPanel(m_MinimapNameTable[buffer]);
					}

					MkCheckBoxControlNode* cbNode = dynamic_cast<MkCheckBoxControlNode*>(GetChildNode(L"BodyFrame")->GetChildNode(L"CB_ViewOpt"));
					if ((cbNode != NULL) && cbNode->GetCheck())
					{
						SREPO.SetColorEditEnable(true);
						SREPO.SetDepthFogEnable(true);
						SREPO.SetFakeHDREnable(true);
						SREPO.SetDepthOfFieldEnable(true);
					}

					UpdateEffectSetting();
				}
			}
		}
	}

	MkTitleBarControlNode::SendNodeReportTypeEvent(eventType, path, argument);
}

void MainControlWindowNode::UpdateEffectSetting(void)
{
	MkWindowManagerNode* mgrNode = GetWindowManagerNode();
	if (mgrNode != NULL)
	{
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

void MainControlWindowNode::_SetMinimapPanel(const MkHashStr& key)
{
	MkPanel& panel = GetChildNode(L"BodyFrame")->CreatePanel(L"P_Map");
	panel.SetTexture(L"Default\\rohan_map.png", key);
	panel.SetLocalPosition(MkFloat2(32.f, 195.f));
	panel.SetLocalDepth(-1.f);
}

//------------------------------------------------------------------------------------------------//
