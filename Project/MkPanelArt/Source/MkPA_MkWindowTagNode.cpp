
#include "MkCore_MkCheck.h"
#include "MkCore_MkTimeManager.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowTagNode.h"


const MkHashStr MkWindowTagNode::NodeNamePrefix(MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"Tag:");

const MkHashStr MkWindowTagNode::IconPanelName(MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"Icon");
const MkHashStr MkWindowTagNode::TextPanelName(MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"Text");

//------------------------------------------------------------------------------------------------//

MkWindowTagNode* MkWindowTagNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	if (parentNode != NULL)
	{
		MK_CHECK(!parentNode->ChildExist(childNodeName), parentNode->GetNodeName().GetString() + L" node에 이미 " + childNodeName.GetString() + L" 이름을 가진 자식이 존재")
			return NULL;
	}

	MkWindowTagNode* node = new MkWindowTagNode(childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowTagNode alloc 실패")
		return NULL;

	if (parentNode != NULL)
	{
		parentNode->AttachChildNode(node);
	}
	return node;
}

//------------------------------------------------------------------------------------------------//

bool MkWindowTagNode::UpdateIconInfo(const TagInfo& tagInfo)
{
	if (!tagInfo.iconPath.Empty())
	{
		MkPanel* panel = PanelExist(IconPanelName) ? GetPanel(IconPanelName) : &CreatePanel(IconPanelName);

		MkTimeState ts;
		MK_TIME_MGR.GetCurrentTimeState(ts);

		if (panel->SetTexture(tagInfo.iconPath, tagInfo.iconSubsetOrSequenceName, ts.fullTime, 0.))
		{
			return panel->GetTextureSize().IsPositive();
		}
	}

	DeletePanel(IconPanelName);
	return false;
}

bool MkWindowTagNode::UpdateTextInfo(const TagInfo& tagInfo)
{
	if ((!tagInfo.textName.Empty()) && MK_STATIC_RES.TextNodeExist(tagInfo.textName))
	{
		MkPanel* panel = PanelExist(TextPanelName) ? GetPanel(TextPanelName) : &CreatePanel(TextPanelName);
		panel->SetTextNode(tagInfo.textName, false); // 빈 text node도 그려지지는 않지만 설정은 허용
		return true;
	}
	
	DeletePanel(TextPanelName);
	return false;
}

bool MkWindowTagNode::UpdateRegionInfo(const TagInfo& tagInfo)
{
	MkPanel* iconPanel = GetPanel(IconPanelName);
	MkPanel* textPanel = GetPanel(TextPanelName);
	if ((iconPanel == NULL) && (textPanel == NULL)) // icon, text 모두 없음
		return false;

	if ((iconPanel != NULL) && (textPanel == NULL)) // icon만 존재
	{
		SetClientSize(iconPanel->GetTextureSize());
	}
	else if ((iconPanel == NULL) && (textPanel != NULL)) // text만 존재
	{
		SetClientSize(textPanel->GetTextureSize());
	}
	else if ((iconPanel != NULL) && (textPanel != NULL)) // icon, text 모두 존재
	{
		MkFloatRect iconRect(MkFloat2::Zero, iconPanel->GetTextureSize());
		MkFloatRect textRect(MkFloat2(iconRect.size.x + tagInfo.lengthOfBetweenIconAndText, 0.f), textPanel->GetTextureSize());

		MkFloatRect totalRect;
		totalRect.UpdateToUnion(iconRect);
		totalRect.UpdateToUnion(textRect);
		SetClientSize(totalRect.size);

		// 영역을 원점 기준으로 맞추고 두 panel중 작은 쪽의 높이를 큰 쪽의 중앙에 오도록 정렬
		MkFloat2 iconMovement, textMovement;
		if (totalRect.position.x < 0.f) // x를 0.f 위치에 정렬
		{
			iconMovement.x = textMovement.x = -totalRect.position.x;
		}

		float heightDiff = (iconRect.size.y - textRect.size.y) * 0.5f;
		if (heightDiff > 0.f)
		{
			textMovement.y = heightDiff; // icon 높이가 text보다 크면 text 높이를 icon의 중앙에 오도록 이동
		}
		else if (heightDiff < 0.f)
		{
			iconMovement.y = -heightDiff; // text 높이가 icon보다 크면 icon 높이를 text의 중앙에 오도록 이동
		}

		iconPanel->SetLocalPosition(iconMovement);
		textPanel->SetLocalPosition(textRect.position + textMovement);
	}
	
	return true;
}

MkTextNode* MkWindowTagNode::GetTagTextPtr(void)
{
	MkPanel* textPanel = GetPanel(TextPanelName);
	return (textPanel == NULL) ? NULL : textPanel->GetTextNodePtr();
}

//------------------------------------------------------------------------------------------------//
