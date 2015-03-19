
#include "MkCore_MkCheck.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowTagNode.h"
#include "MkPA_MkTitleBarControlNode.h"


const MkHashStr MkTitleBarControlNode::CloseButtonNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"CloseBtn";
const MkHashStr MkTitleBarControlNode::IconNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Icon";
const MkHashStr MkTitleBarControlNode::CaptionNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Caption";

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

	// close button
	if (useCloseBtn)
	{
		MkWindowBaseNode* node = ChildExist(CloseButtonNodeName) ?
			dynamic_cast<MkWindowBaseNode*>(GetChildNode(CloseButtonNodeName)) : MkWindowBaseNode::CreateChildNode(this, CloseButtonNodeName);

		if (node != NULL)
		{
			node->SetThemeName(themeName);
			node->SetComponentType(MkWindowThemeData::GetLEDButtonComponent(m_WindowFrameType, MkWindowThemeData::eLED_Red));
			node->SetFormState(MkWindowThemeFormData::eS_Normal);
			node->SetAcceptInput(true);
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
}

void MkTitleBarControlNode::SetCaption(const MkStr& caption, eRectAlignmentPosition position)
{
	if (caption.Empty())
	{
		RemoveChildNode(CaptionNodeName);
		return;
	}

	// MkWindowTagNode 사용
	MkWindowTagNode* node = ChildExist(CaptionNodeName) ?
		dynamic_cast<MkWindowTagNode*>(GetChildNode(CaptionNodeName)) : MkWindowTagNode::CreateChildNode(this, CaptionNodeName);

	if (node != NULL)
	{
		node->SetLocalDepth(-0.1f); // title과 겹치는 것을 피하기 위해 0.1f만큼 앞에 위치

		// caption
		node->SetTextName(MK_STATIC_RES.GetWindowThemeSet().GetCaptionTextNode(GetThemeName(), m_WindowFrameType), caption);

		// alignment
		node->SetAlignmentPosition(position);
		if (GetHorizontalRectAlignmentType(position) == eRAT_Left) // 좌측 계열 정렬이면 icon 위치 확보
		{
			float frameSize = MK_STATIC_RES.GetWindowThemeSet().GetFrameSize(GetThemeName(), m_WindowFrameType);
			node->SetAlignmentOffset(MkFloat2(frameSize + node->GetLengthOfBetweenIconAndText(), 0.f));
		}
	}
}

MkTitleBarControlNode::MkTitleBarControlNode(const MkHashStr& name) : MkWindowBaseNode(name)
{
	SetAcceptInput(true);
	SetMovableByDragging(true);
}

//------------------------------------------------------------------------------------------------//