
#include "MkCore_MkCheck.h"

#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkTitleBarControlNode.h"
#include "MkPA_MkBodyFrameControlNode.h"


//------------------------------------------------------------------------------------------------//

MkBodyFrameControlNode* MkBodyFrameControlNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkBodyFrameControlNode* node = __TSI_SceneNodeDerivedInstanceOp<MkBodyFrameControlNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkBodyFrameControlNode ���� ����") {}
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
			// �θ��� ũ��(length)�� ����
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

	SetLocalDepth(0.1f); // title�� ��ġ�� ���� ���ϱ� ���� 0.1f��ŭ �ڿ� ��ġ

	_ApplyHangingType();
}

void MkBodyFrameControlNode::_ApplyHangingType(void)
{
	bool pivotIsWindowRect = false;
	eRectAlignmentPosition alignPos = eRAP_NonePosition;
	MkFloat2 alignOffset;

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
						alignPos = eRAP_LeftTop;
						alignOffset = MkFloat2(-formData->GetLeftMargin(), formData->GetTopMargin());
						break;

					case eHT_IncludeParentAtBottom:
						pivotIsWindowRect = true;
						alignPos = eRAP_LeftBottom;
						alignOffset = MkFloat2(-formData->GetLeftMargin(), -formData->GetBottomMargin());
						break;
					}
				}
			}
			break;
		}
	}

	SetAlignmentPivotIsWindowRect(pivotIsWindowRect);
	SetAlignmentPosition(alignPos);
	SetAlignmentOffset(alignOffset);
}

//------------------------------------------------------------------------------------------------//