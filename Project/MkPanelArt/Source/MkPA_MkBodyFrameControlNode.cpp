
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkTitleBarControlNode.h"
#include "MkPA_MkBodyFrameControlNode.h"


const MkHashStr MkBodyFrameControlNode::ObjKey_HangingType(L"HangingType");

//------------------------------------------------------------------------------------------------//

MkBodyFrameControlNode* MkBodyFrameControlNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkBodyFrameControlNode* node = __TSI_SceneNodeDerivedInstanceOp<MkBodyFrameControlNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkBodyFrameControlNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkBodyFrameControlNode::SetBodyFrame
(const MkHashStr& themeName, MkWindowThemeData::eComponentType componentType, bool useShadow, eHangingType hangingType, const MkFloat2& clientSize)
{
	_SetBodyFrame(themeName, componentType, MkHashStr::EMPTY, useShadow, hangingType, clientSize);
}

void MkBodyFrameControlNode::SetBodyFrame
(const MkHashStr& themeName, const MkHashStr& customFormName, bool useShadow, eHangingType hangingType, const MkFloat2& clientSize)
{
	_SetBodyFrame(themeName, MkWindowThemeData::eCT_CustomForm, customFormName, useShadow, hangingType, clientSize);
}

void MkBodyFrameControlNode::SetClientSize(const MkFloat2& clientSize)
{
	MkWindowBaseNode::SetClientSize(clientSize);

	if ((m_ParentNodePtr != NULL) && (m_ParentNodePtr->IsDerivedFrom(ePA_SNT_TitleBarControlNode)))
	{
		MkTitleBarControlNode* parentNode = dynamic_cast<MkTitleBarControlNode*>(m_ParentNodePtr);
		if (parentNode != NULL)
		{
			// 부모의 크기(length)를 변경
			float sizeOffset = 0.f;
			if ((m_HangingType == eHT_IncludeParentAtTop) || (m_HangingType == eHT_IncludeParentAtBottom))
			{
				const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(GetThemeName(), GetComponentType(), GetCustomForm());
				if (formData != NULL)
				{
					sizeOffset += formData->GetLeftMargin();
					sizeOffset += formData->GetRightMargin();
				}
			}

			float frameSize = MK_STATIC_RES.GetWindowThemeSet().GetFrameSize(parentNode->GetThemeName(), parentNode->GetWindowFrameType());
			float titleLength = GetMax<float>(m_ClientRect.size.x - sizeOffset, 0.f);
			parentNode->SetClientSize(MkFloat2(titleLength, frameSize));
		}
	}
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkBodyFrameControlNode);

void MkBodyFrameControlNode::SetObjectTemplate(MkDataNode& node)
{
	MkWindowBaseNode::SetObjectTemplate(node);

	node.CreateUnit(ObjKey_HangingType, static_cast<int>(eHT_None));
}

void MkBodyFrameControlNode::LoadObject(const MkDataNode& node)
{
	// SetClientSize() 호출 전 hanging type이 세팅되어 있어야 함
	int hangingType = static_cast<int>(eHT_None);
	node.GetData(ObjKey_HangingType, hangingType, 0);
	m_HangingType = static_cast<eHangingType>(hangingType);

	MkWindowBaseNode::LoadObject(node);
}

void MkBodyFrameControlNode::SaveObject(MkDataNode& node) const
{
	MkWindowBaseNode::SaveObject(node);

	node.SetData(ObjKey_HangingType, static_cast<int>(m_HangingType), 0);
}

MkBodyFrameControlNode::MkBodyFrameControlNode(const MkHashStr& name) : MkWindowBaseNode(name)
{
	m_HangingType = eHT_None;
}

void MkBodyFrameControlNode::_SetBodyFrame
(const MkHashStr& themeName, MkWindowThemeData::eComponentType componentType, const MkHashStr& customFormName,
 bool useShadow, eHangingType hangingType, const MkFloat2& clientSize)
{
	SetThemeName(themeName);

	if (componentType == MkWindowThemeData::eCT_CustomForm)
	{
		SetCustomForm(customFormName);
	}
	else
	{
		SetComponentType(componentType);
	}

	SetFormState(MkWindowThemeFormData::eS_Default);
	SetShadowUsage(useShadow);

	m_HangingType = hangingType;
	SetClientSize(clientSize);

	SetLocalDepth(0.1f); // title과 겹치는 것을 피하기 위해 0.1f만큼 뒤에 위치

	_ApplyHangingType();
}

void MkBodyFrameControlNode::_ApplyHangingType(void)
{
	bool pivotIsWindowRect = false;
	bool targetIsWindowRect = true;
	eRectAlignmentPosition alignPos = eRAP_NonePosition;

	if (m_HangingType != eHT_None)
	{
		switch (m_HangingType)
		{
		case eHT_OverParentWindow:
			pivotIsWindowRect = true;
			alignPos = eRAP_LeftOver;
			break;

		case eHT_UnderParentWindow:
			pivotIsWindowRect = true;
			alignPos = eRAP_LeftUnder;
			break;

		case eHT_IncludeParentAtTop:
		case eHT_IncludeParentAtBottom:
			{
				const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(GetThemeName(), GetComponentType(), GetCustomForm());
				if (formData != NULL)
				{
					switch (m_HangingType)
					{
					case eHT_IncludeParentAtTop:
						pivotIsWindowRect = true;
						targetIsWindowRect = false;
						alignPos = eRAP_LeftTop;
						break;

					case eHT_IncludeParentAtBottom:
						pivotIsWindowRect = true;
						targetIsWindowRect = false;
						alignPos = eRAP_LeftBottom;
						break;
					}
				}
			}
			break;
		}
	}

	SetAlignmentPivotIsWindowRect(pivotIsWindowRect);
	SetAlignmentTargetIsWindowRect(targetIsWindowRect);
	SetAlignmentPosition(alignPos);
}

//------------------------------------------------------------------------------------------------//