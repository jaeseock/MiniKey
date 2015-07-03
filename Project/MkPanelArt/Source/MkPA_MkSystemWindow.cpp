
#include "MkCore_MkCheck.h"
//#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowTagNode.h"
#include "MkPA_MkBodyFrameControlNode.h"
#include "MkPA_MkSystemWindow.h"

//------------------------------------------------------------------------------------------------//

const MkHashStr MkSystemWindow::OneAndOnlyTagName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"OAOTag";

MkWindowBaseNode* MkSystemWindow::CreateNormalButton(const MkHashStr& name)
{
	return NULL;
}

MkWindowBaseNode* MkSystemWindow::CreateOkButton(const MkHashStr& name)
{
	return NULL;
}

MkWindowBaseNode* MkSystemWindow::CreateCancelButton(const MkHashStr& name)
{
	return NULL;
}

const MkHashStr MkSystemWindow::MkMessageBox::TitleNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Title";
const MkHashStr MkSystemWindow::MkMessageBox::BodyNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Body";
const MkHashStr MkSystemWindow::MkMessageBox::TextPanelName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Text";
const MkHashStr MkSystemWindow::MkMessageBox::OkButtonNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Ok";
const MkHashStr MkSystemWindow::MkMessageBox::CancelButtonNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Cancel";
const MkHashStr MkSystemWindow::MkMessageBox::ButtonPanelName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Msg";

//------------------------------------------------------------------------------------------------//

MkSystemWindow::MkMessageBox* MkSystemWindow::MkMessageBox::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkSystemWindow::MkMessageBox* node = __TSI_SceneNodeDerivedInstanceOp<MkSystemWindow::MkMessageBox>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkSystemWindow::MkMessageBox 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkSystemWindow::MkMessageBox::SetMessageBox(const MkHashStr& themeName, eUsageType usageType, eFrameType frameType, const MkStr& message)
{
}

void MkSystemWindow::MkMessageBox::SetMessageBox(const MkHashStr& themeName, eUsageType usageType, eFrameType frameType, const MkTextNode& textNode)
{
}

void MkSystemWindow::MkMessageBox::SetMessageBox(const MkHashStr& themeName, eUsageType usageType, eFrameType frameType, const MkArray<MkHashStr>& textName)
{
}

void MkSystemWindow::MkMessageBox::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	/*
	// left cursor click이고 해당 window가 자신이면 toggle
	if ((eventType == ePA_SNE_CursorLBtnReleased) && path.Empty())
	{
		_SetCheck(!GetCheck(), true);
	}
	*/
	MkTitleBarControlNode::SendNodeReportTypeEvent(eventType, path, argument);
}

void MkSystemWindow::MkMessageBox::Save(MkDataNode& node) const
{/*
	// caption, on icon 제외
	static MkArray<MkHashStr> exceptions(2);
	if (exceptions.Empty())
	{
		exceptions.PushBack(CaptionNodeName);
		exceptions.PushBack(CheckIconName);
	}
	_AddExceptionList(node, SystemKey_NodeExceptions, exceptions);
*/
	// run
	MkTitleBarControlNode::Save(node);
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkSystemWindow::MkMessageBox);

void MkSystemWindow::MkMessageBox::SetObjectTemplate(MkDataNode& node)
{
	MkTitleBarControlNode::SetObjectTemplate(node);

	// ObjKey_CaptionTextName
	//node.CreateUnit(ObjKey_CaptionString, MkStr::EMPTY);
	//node.CreateUnit(ObjKey_OnCheck, false);
}

void MkSystemWindow::MkMessageBox::LoadObject(const MkDataNode& node)
{
	MkTitleBarControlNode::LoadObject(node);
/*
	// caption
	MkArray<MkHashStr> textName;
	if (node.GetDataEx(ObjKey_CaptionTextName, textName) && (!textName.Empty()))
	{
		MkStr caption;
		node.GetData(ObjKey_CaptionString, caption, 0);

		_SetCaption(textName, caption);
	}

	// check
	bool onCheck;
	if (node.GetData(ObjKey_OnCheck, onCheck, 0))
	{
		SetCheck(onCheck);
	}
	*/
}

void MkSystemWindow::MkMessageBox::SaveObject(MkDataNode& node) const
{
	MkTitleBarControlNode::SaveObject(node);
/*
	// caption
	if (ChildExist(CaptionNodeName))
	{
		const MkWindowTagNode* captionNode = dynamic_cast<const MkWindowTagNode*>(GetChildNode(CaptionNodeName));
		if (captionNode != NULL)
		{
			node.CreateUnitEx(ObjKey_CaptionTextName, captionNode->GetTextName());
			node.SetData(ObjKey_CaptionString, m_CaptionString, 0);
		}
	}

	// check
	node.SetData(ObjKey_OnCheck, m_OnCheck, 0);
	*/
}

MkSystemWindow::MkMessageBox::MkMessageBox(const MkHashStr& name) : MkTitleBarControlNode(name)
{
	
}
/*
void MkSystemWindow::MkMessageBox::_SetCheckBox
(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, const MkArray<MkHashStr>& textNode, const MkStr& caption, bool onCheck)
{
	m_WindowFrameType = frameType;
	float frameSize = MK_STATIC_RES.GetWindowThemeSet().GetFrameSize(themeName, m_WindowFrameType);

	// check box
	SetThemeName(themeName);
	SetComponentType(MkWindowThemeData::eCT_CheckBoxBtn);
	SetClientSize(MkFloat2(frameSize, frameSize));
	SetFormState(MkWindowThemeFormData::eS_Normal);

	// caption
	_SetCaption(textNode, caption);

	// on check
	_SetCheck(onCheck, false);
}

void MkSystemWindow::MkMessageBox::_SetCaption(const MkArray<MkHashStr>& textNode, const MkStr& caption)
{
	if (textNode.Empty() || (!MK_STATIC_RES.TextNodeExist(textNode)))
	{
		RemoveChildNode(CaptionNodeName);
		m_CaptionString.Clear();
		return;
	}

	// MkWindowTagNode 사용
	MkWindowTagNode* node = NULL;
	if (ChildExist(CaptionNodeName))
	{
		node = dynamic_cast<MkWindowTagNode*>(GetChildNode(CaptionNodeName));
	}
	else
	{
		node = MkWindowTagNode::CreateChildNode(this, CaptionNodeName);
		if (node != NULL)
		{
			node->SetAlignmentPivotIsWindowRect(true);
			node->SetAlignmentPosition(eRAP_RMostCenter);
			node->SetAlignmentOffset(MkFloat2(node->GetLengthOfBetweenIconAndText(), 0.f));
		}
	}

	if (node != NULL)
	{
		m_CaptionString = caption;
		if (caption.Empty())
		{
			node->SetTextName(textNode);
		}
		else
		{
			node->SetTextName(textNode, caption);
		}
	}
}
*/
//------------------------------------------------------------------------------------------------//

MkWindowBaseNode* MkSystemWindow::_CreateDefaultComponent(const MkHashStr& name, MkWindowThemeData::eComponentType componentType)
{
	MkWindowBaseNode* node = MkWindowBaseNode::CreateChildNode(NULL, name);
	if (node != NULL)
	{
		node->SetThemeName(MkWindowThemeData::DefaultThemeName);
		node->SetComponentType(componentType);
		node->SetFormState(MkWindowThemeFormData::eS_Default);
		node->SetLocalDepth(-1.f); // 자동으로 1.f만큼 앞에 위치시킴
	}
	return node;
}

MkWindowBaseNode* MkSystemWindow::_CreateDefaultButton(const MkHashStr& name, MkWindowThemeData::eComponentType componentType, const MkStr& message)
{
	/*
	const MkHashStr& themeName = MkWindowThemeData::DefaultThemeName;
	const MkWindowThemeData::eFrameType frameType = MkWindowThemeData::eFT_Small;

	const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(themeName, componentType, MkHashStr::EMPTY);
	if ((formData != NULL) && formData->IsButtonFormType())
	{
		// btn node
		MkWindowBaseNode* btnNode = _CreateDefaultComponent(name, componentType);
		if (btnNode != NULL)
		{
			// client size
			float frameSize = MK_STATIC_RES.GetWindowThemeSet().GetFrameSize(themeName, frameType);
			btnNode->SetClientSize(MkFloat2(100.f, frameSize));

			// tag
			if (!message.Empty())
			{
				const MkArray<MkHashStr>& textNode = MK_STATIC_RES.GetWindowThemeSet().GetCaptionTextNode(themeName, frameType);
				OneAndOnlyTagName;
			}
		}
	}
	*/
	return NULL;
}

//------------------------------------------------------------------------------------------------//