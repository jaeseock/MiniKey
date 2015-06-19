
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowTagNode.h"
#include "MkPA_MkTitleBarControlNode.h"


const MkHashStr MkTitleBarControlNode::CloseButtonNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"CloseBtn";
const MkHashStr MkTitleBarControlNode::IconNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Icon";
const MkHashStr MkTitleBarControlNode::CaptionNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Caption";

const MkHashStr MkTitleBarControlNode::ObjKey_UseCloseBtn(L"UseCloseBtn");
const MkHashStr MkTitleBarControlNode::ObjKey_IconType(L"IconType");
const MkHashStr MkTitleBarControlNode::ObjKey_IconPath(L"IconPath");
const MkHashStr MkTitleBarControlNode::ObjKey_IconSOSName(L"IconSOSName");
const MkHashStr MkTitleBarControlNode::ObjKey_CaptionTextName(L"CaptionTextName");
const MkHashStr MkTitleBarControlNode::ObjKey_CaptionString(L"CaptionString");
const MkHashStr MkTitleBarControlNode::ObjKey_CaptionAlignPos(L"CaptionAlignPos");

//------------------------------------------------------------------------------------------------//

MkTitleBarControlNode* MkTitleBarControlNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkTitleBarControlNode* node = __TSI_SceneNodeDerivedInstanceOp<MkTitleBarControlNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkTitleBarControlNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkTitleBarControlNode::SetTitleBar(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length, bool useCloseBtn)
{
	// title bar
	m_WindowFrameType = frameType;
	float frameSize = MK_STATIC_RES.GetWindowThemeSet().GetFrameSize(themeName, m_WindowFrameType);

	SetThemeName(themeName);
	SetComponentType(MkWindowThemeData::eCT_Title);
	SetClientSize(MkFloat2(GetMax<float>(length, frameSize), frameSize));
	SetFormState(MkWindowThemeFormData::eS_Back);

	UseCloseButton(useCloseBtn);
}

void MkTitleBarControlNode::UseCloseButton(bool enable)
{
	if (enable)
	{
		MkWindowBaseNode* node = ChildExist(CloseButtonNodeName) ?
			dynamic_cast<MkWindowBaseNode*>(GetChildNode(CloseButtonNodeName)) : MkWindowBaseNode::CreateChildNode(this, CloseButtonNodeName);

		if (node != NULL)
		{
			node->SetThemeName(GetThemeName());
			node->SetComponentType(MkWindowThemeData::GetLEDButtonComponent(m_WindowFrameType, MkWindowThemeData::eLED_Red));
			node->SetFormState(MkWindowThemeFormData::eS_Normal);
			node->SetLocalDepth(-0.1f); // title과 겹치는 것을 피하기 위해 0.1f만큼 앞에 위치
			node->SetAlignmentPosition(eRAP_RightCenter);
		}
	}
	else
	{
		RemoveChildNode(CloseButtonNodeName);
	}
}


void MkTitleBarControlNode::SetIcon(MkWindowThemeData::eIconType iconType, const MkHashStr& iconPath, const MkHashStr& iconSubsetOrSequenceName)
{
	if (iconType == MkWindowThemeData::eIT_None)
	{
		RemoveChildNode(IconNodeName);
	}
	// MkWindowTagNode 사용
	else if (iconType == MkWindowThemeData::eIT_CustomTag)
	{
		MkWindowTagNode* iconNode = NULL;
		if (ChildExist(IconNodeName))
		{
			MkSceneNode* sceneNode = GetChildNode(IconNodeName);
			if (sceneNode->GetNodeType() == ePA_SNT_WindowTagNode) // 동종이므로 재활용
			{
				iconNode = dynamic_cast<MkWindowTagNode*>(sceneNode);
			}
			else // 이종이므로 삭제
			{
				RemoveChildNode(IconNodeName);
			}
		}

		if (iconNode == NULL)
		{
			iconNode = MkWindowTagNode::CreateChildNode(this, IconNodeName);
		}
		if (iconNode != NULL)
		{
			iconNode->SetLocalDepth(-0.1f); // title과 겹치는 것을 피하기 위해 0.1f만큼 앞에 위치
			iconNode->SetIconPath(iconPath);
			iconNode->SetIconSubsetOrSequenceName(iconSubsetOrSequenceName);
			iconNode->SetAlignmentPosition(eRAP_LeftCenter);
		}
	}
	// system icon. MkWindowThemedNode 사용
	else
	{
		MkWindowThemeData::eComponentType componentType = MkWindowThemeData::GetSystemIconComponent(m_WindowFrameType, iconType);
		if (componentType == MkWindowThemeData::eCT_None)
		{
			RemoveChildNode(IconNodeName);
		}
		else
		{
			MkWindowThemedNode* iconNode = NULL;
			if (ChildExist(IconNodeName))
			{
				MkSceneNode* sceneNode = GetChildNode(IconNodeName);
				if (sceneNode->GetNodeType() == ePA_SNT_WindowThemedNode) // 동종이므로 재활용
				{
					iconNode = dynamic_cast<MkWindowThemedNode*>(sceneNode);
				}
				else // 이종이므로 삭제
				{
					RemoveChildNode(IconNodeName);
				}
			}

			if (iconNode == NULL)
			{
				iconNode = MkWindowThemedNode::CreateChildNode(this, IconNodeName);
			}
			if (iconNode != NULL)
			{
				iconNode->SetLocalDepth(-0.1f); // title과 겹치는 것을 피하기 위해 0.1f만큼 앞에 위치
				iconNode->SetThemeName(GetThemeName());
				iconNode->SetComponentType(componentType);
				iconNode->SetAlignmentPosition(eRAP_LeftCenter);
			}
		}
	}

	m_IconType = iconType;
}

void MkTitleBarControlNode::SetCaption(const MkArray<MkHashStr>& textNode, eRectAlignmentPosition position)
{
	_SetCaption(textNode, MkHashStr::EMPTY, position);
}

void MkTitleBarControlNode::SetCaption(const MkStr& caption, eRectAlignmentPosition position)
{
	_SetCaption(MK_STATIC_RES.GetWindowThemeSet().GetCaptionTextNode(GetThemeName(), m_WindowFrameType), caption, position);
}

void MkTitleBarControlNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	// left cursor click이고 해당 window가 close button이면 event를 ePA_SNE_CloseWindow로 바꾸어 보냄
	if ((eventType == ePA_SNE_CursorLBtnReleased) && (path.GetSize() == 1) && (path[0] == CloseButtonNodeName))
	{
		StartNodeReportTypeEvent(ePA_SNE_CloseWindow, NULL);
	}
	else
	{
		MkWindowBaseNode::SendNodeReportTypeEvent(eventType, path, argument);
	}
}

void MkTitleBarControlNode::Save(MkDataNode& node) const
{
	// closr btn, icon, caption node 제외
	static MkArray<MkHashStr> exceptions(3);
	if (exceptions.Empty())
	{
		exceptions.PushBack(CloseButtonNodeName);
		exceptions.PushBack(IconNodeName);
		exceptions.PushBack(CaptionNodeName);
	}
	_AddExceptionList(node, SystemKey_NodeExceptions, exceptions);

	// run
	MkWindowBaseNode::Save(node);
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkTitleBarControlNode);

void MkTitleBarControlNode::SetObjectTemplate(MkDataNode& node)
{
	MkWindowBaseNode::SetObjectTemplate(node);

	// close btn
	node.CreateUnit(ObjKey_UseCloseBtn, true);

	// icon
	node.CreateUnit(ObjKey_IconType, static_cast<int>(MkWindowThemeData::eIT_None));
	node.CreateUnit(ObjKey_IconPath, MkStr::EMPTY);
	node.CreateUnit(ObjKey_IconSOSName, MkStr::EMPTY);

	// caption
	// ObjKey_CaptionTextName
	node.CreateUnit(ObjKey_CaptionString, MkStr::EMPTY);
	node.CreateUnit(ObjKey_CaptionAlignPos, static_cast<int>(eRAP_NonePosition));
}

void MkTitleBarControlNode::LoadObject(const MkDataNode& node)
{
	MkWindowBaseNode::LoadObject(node);

	// close btn
	bool useCloseBtn;
	if (node.GetData(ObjKey_UseCloseBtn, useCloseBtn, 0))
	{
		UseCloseButton(useCloseBtn);
	}

	// icon
	int iconType;
	if (node.GetData(ObjKey_IconType, iconType, 0))
	{
		MkHashStr path, sosName;
		node.GetDataEx(ObjKey_IconPath, path, 0);
		node.GetDataEx(ObjKey_IconSOSName, sosName, 0);

		SetIcon(static_cast<MkWindowThemeData::eIconType>(iconType), path, sosName);
	}

	// caption
	MkArray<MkHashStr> textName;
	if (node.GetDataEx(ObjKey_CaptionTextName, textName) && (!textName.Empty()))
	{
		MkStr caption;
		node.GetData(ObjKey_CaptionString, caption, 0);

		int alignPos = static_cast<int>(eRAP_NonePosition);
		node.GetData(ObjKey_CaptionAlignPos, alignPos, 0);

		_SetCaption(textName, caption, static_cast<eRectAlignmentPosition>(alignPos));
	}
}

void MkTitleBarControlNode::SaveObject(MkDataNode& node) const
{
	MkWindowBaseNode::SaveObject(node);

	// close btn
	node.SetData(ObjKey_UseCloseBtn, ChildExist(CloseButtonNodeName), 0);

	// icon
	node.SetData(ObjKey_IconType, static_cast<int>(m_IconType), 0);

	if ((m_IconType != MkWindowThemeData::eIT_None) && ChildExist(IconNodeName))
	{
		const MkSceneNode* sceneNode = GetChildNode(IconNodeName);
		if (sceneNode->GetNodeType() == ePA_SNT_WindowTagNode) // MkWindowThemeData::eIT_CustomTag
		{
			const MkWindowTagNode* iconNode = dynamic_cast<const MkWindowTagNode*>(sceneNode);
			if (iconNode != NULL)
			{
				node.SetDataEx(ObjKey_IconPath, iconNode->GetIconPath(), 0);
				node.SetDataEx(ObjKey_IconSOSName, iconNode->GetIconSubsetOrSequenceName(), 0);
			}
		}
	}

	// caption
	if (ChildExist(CaptionNodeName))
	{
		const MkWindowTagNode* captionNode = dynamic_cast<const MkWindowTagNode*>(GetChildNode(CaptionNodeName));
		if (captionNode != NULL)
		{
			node.CreateUnitEx(ObjKey_CaptionTextName, captionNode->GetTextName());
			node.SetData(ObjKey_CaptionString, m_CaptionString, 0);
			node.SetData(ObjKey_CaptionAlignPos, static_cast<int>(captionNode->GetAlignmentPosition()), 0);
		}
	}
}

void MkTitleBarControlNode::Clear(void)
{
	m_IconType = MkWindowThemeData::eIT_None;
	m_CaptionString.Clear();

	MkWindowBaseNode::Clear();
}

MkTitleBarControlNode::MkTitleBarControlNode(const MkHashStr& name) : MkWindowBaseNode(name)
{
	SetMovableByDragging(true);

	m_IconType = MkWindowThemeData::eIT_None;
}

void MkTitleBarControlNode::_SetCaption(const MkArray<MkHashStr>& textNode, const MkStr& caption, eRectAlignmentPosition position)
{
	if (textNode.Empty() || (!MK_STATIC_RES.TextNodeExist(textNode)))
	{
		RemoveChildNode(CaptionNodeName);
		m_CaptionString.Clear();
		return;
	}

	// MkWindowTagNode 사용
	MkWindowTagNode* node = ChildExist(CaptionNodeName) ?
		dynamic_cast<MkWindowTagNode*>(GetChildNode(CaptionNodeName)) : MkWindowTagNode::CreateChildNode(this, CaptionNodeName);

	if (node != NULL)
	{
		node->SetLocalDepth(-0.1f); // title과 겹치는 것을 피하기 위해 0.1f만큼 앞에 위치

		// caption
		m_CaptionString = caption;

		if (caption.Empty())
		{
			node->SetTextName(textNode);
		}
		else
		{
			node->SetTextName(textNode, caption);
		}

		// alignment
		node->SetAlignmentPosition(position);
		if (GetHorizontalRectAlignmentType(position) == eRAT_Left) // 좌측 계열 정렬이면 icon 위치 확보
		{
			float frameSize = MK_STATIC_RES.GetWindowThemeSet().GetFrameSize(GetThemeName(), m_WindowFrameType);
			node->SetAlignmentOffset(MkFloat2(frameSize + node->GetLengthOfBetweenIconAndText(), 0.f));
		}
	}
}

//------------------------------------------------------------------------------------------------//