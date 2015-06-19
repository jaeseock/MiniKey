
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowTagNode.h"
#include "MkPA_MkCheckBoxControlNode.h"


const MkHashStr MkCheckBoxControlNode::CaptionNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Caption";
const MkHashStr MkCheckBoxControlNode::CheckIconName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"CheckIcon";

const MkHashStr MkCheckBoxControlNode::ObjKey_CaptionTextName(L"CaptionTextName");
const MkHashStr MkCheckBoxControlNode::ObjKey_CaptionString(L"CaptionString");
const MkHashStr MkCheckBoxControlNode::ObjKey_OnCheck(L"OnCheck");

//------------------------------------------------------------------------------------------------//

MkCheckBoxControlNode* MkCheckBoxControlNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkCheckBoxControlNode* node = __TSI_SceneNodeDerivedInstanceOp<MkCheckBoxControlNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkCheckBoxControlNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkCheckBoxControlNode::SetCheckBox(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, const MkArray<MkHashStr>& textNode, bool onCheck)
{
	_SetCheckBox(themeName, frameType, textNode, MkStr::EMPTY, onCheck);
}

void MkCheckBoxControlNode::SetCheckBox(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, const MkStr& caption, bool onCheck)
{
	_SetCheckBox(themeName, frameType, MK_STATIC_RES.GetWindowThemeSet().GetCaptionTextNode(themeName, frameType), caption, onCheck);
}

void MkCheckBoxControlNode::SetCheck(bool onCheck)
{
	_SetCheck(onCheck, true);
}

void MkCheckBoxControlNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	// left cursor click이고 해당 window가 자신이면 toggle
	if ((eventType == ePA_SNE_CursorLBtnReleased) && path.Empty())
	{
		_SetCheck(!GetCheck(), true);
	}
	else
	{
		MkWindowBaseNode::SendNodeReportTypeEvent(eventType, path, argument);
	}
}

void MkCheckBoxControlNode::Save(MkDataNode& node) const
{
	// caption, on icon 제외
	static MkArray<MkHashStr> exceptions(2);
	if (exceptions.Empty())
	{
		exceptions.PushBack(CaptionNodeName);
		exceptions.PushBack(CheckIconName);
	}
	_AddExceptionList(node, SystemKey_NodeExceptions, exceptions);

	// run
	MkWindowBaseNode::Save(node);
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkCheckBoxControlNode);

void MkCheckBoxControlNode::SetObjectTemplate(MkDataNode& node)
{
	MkWindowBaseNode::SetObjectTemplate(node);

	// ObjKey_CaptionTextName
	node.CreateUnit(ObjKey_CaptionString, MkStr::EMPTY);
	node.CreateUnit(ObjKey_OnCheck, false);
}

void MkCheckBoxControlNode::LoadObject(const MkDataNode& node)
{
	MkWindowBaseNode::LoadObject(node);

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
}

void MkCheckBoxControlNode::SaveObject(MkDataNode& node) const
{
	MkWindowBaseNode::SaveObject(node);

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
}

MkCheckBoxControlNode::MkCheckBoxControlNode(const MkHashStr& name) : MkWindowBaseNode(name)
{
	m_OnCheck = false;
}

void MkCheckBoxControlNode::_SetCheckBox
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

void MkCheckBoxControlNode::_SetCaption(const MkArray<MkHashStr>& textNode, const MkStr& caption)
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

void MkCheckBoxControlNode::_SetCheck(bool onCheck, bool makeEvent)
{
	if (onCheck != m_OnCheck)
	{
		m_OnCheck = onCheck;

		MkWindowThemedNode* node = NULL;
		if (ChildExist(CheckIconName))
		{
			node = dynamic_cast<MkWindowThemedNode*>(GetChildNode(CheckIconName));
		}
		else
		{
			MkWindowThemeData::eComponentType iconCT = MkWindowThemeData::GetSystemIconComponent(m_WindowFrameType, MkWindowThemeData::eIT_CheckMark);
			if (iconCT != MkWindowThemeData::eCT_None)
			{
				node = MkWindowThemedNode::CreateChildNode(this, CheckIconName);
				if (node != NULL)
				{
					node->SetLocalDepth(-0.1f); // check box와 겹치는 것을 피하기 위해 0.1f만큼 앞에 위치
					node->SetThemeName(GetThemeName());
					node->SetComponentType(iconCT);
					node->SetAlignmentPosition(eRAP_MiddleCenter);
				}
			}
		}

		if (node != NULL)
		{
			node->SetVisible(m_OnCheck);
		}

		if (makeEvent)
		{
			StartNodeReportTypeEvent((m_OnCheck) ? ePA_SNE_CheckOn : ePA_SNE_CheckOff, NULL);
		}
	}
}

//------------------------------------------------------------------------------------------------//