
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
		MK_CHECK(!parentNode->ChildExist(childNodeName), parentNode->GetNodeName().GetString() + L" node�� �̹� " + childNodeName.GetString() + L" �̸��� ���� �ڽ��� ����")
			return NULL;
	}

	MkWindowTagNode* node = new MkWindowTagNode(childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowTagNode alloc ����")
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
		panel->SetTextNode(tagInfo.textName, false); // �� text node�� �׷������� ������ ������ ���
		return true;
	}
	
	DeletePanel(TextPanelName);
	return false;
}

bool MkWindowTagNode::UpdateRegionInfo(const TagInfo& tagInfo)
{
	MkPanel* iconPanel = GetPanel(IconPanelName);
	MkPanel* textPanel = GetPanel(TextPanelName);
	if ((iconPanel == NULL) && (textPanel == NULL)) // icon, text ��� ����
		return false;

	if ((iconPanel != NULL) && (textPanel == NULL)) // icon�� ����
	{
		SetClientSize(iconPanel->GetTextureSize());
	}
	else if ((iconPanel == NULL) && (textPanel != NULL)) // text�� ����
	{
		SetClientSize(textPanel->GetTextureSize());
	}
	else if ((iconPanel != NULL) && (textPanel != NULL)) // icon, text ��� ����
	{
		MkFloatRect iconRect(MkFloat2::Zero, iconPanel->GetTextureSize());
		MkFloatRect textRect(MkFloat2(iconRect.size.x + tagInfo.lengthOfBetweenIconAndText, 0.f), textPanel->GetTextureSize());

		MkFloatRect totalRect;
		totalRect.UpdateToUnion(iconRect);
		totalRect.UpdateToUnion(textRect);
		SetClientSize(totalRect.size);

		// ������ ���� �������� ���߰� �� panel�� ���� ���� ���̸� ū ���� �߾ӿ� ������ ����
		MkFloat2 iconMovement, textMovement;
		if (totalRect.position.x < 0.f) // x�� 0.f ��ġ�� ����
		{
			iconMovement.x = textMovement.x = -totalRect.position.x;
		}

		float heightDiff = (iconRect.size.y - textRect.size.y) * 0.5f;
		if (heightDiff > 0.f)
		{
			textMovement.y = heightDiff; // icon ���̰� text���� ũ�� text ���̸� icon�� �߾ӿ� ������ �̵�
		}
		else if (heightDiff < 0.f)
		{
			iconMovement.y = -heightDiff; // text ���̰� icon���� ũ�� icon ���̸� text�� �߾ӿ� ������ �̵�
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
