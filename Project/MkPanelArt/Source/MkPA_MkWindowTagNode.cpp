
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
	MkWindowTagNode* node = __TSI_SceneNodeDerivedInstanceOp<MkWindowTagNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowTagNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkWindowTagNode::SetIconPath(const MkHashStr& iconPath)
{
	if (iconPath != m_IconPath)
	{
		m_IconPath = iconPath;
		m_UpdateCommand.Set(eUC_Icon);
	}
}

void MkWindowTagNode::SetIconSubsetOrSequenceName(const MkHashStr& subsetOrSequenceName)
{
	if (subsetOrSequenceName != m_IconSubsetOrSequenceName)
	{
		m_IconSubsetOrSequenceName = subsetOrSequenceName;
		m_UpdateCommand.Set(eUC_Icon);
	}
}

void MkWindowTagNode::SetTextName(const MkHashStr& textName)
{
	if (textName != m_TextName)
	{
		m_TextName = textName;
		m_UpdateCommand.Set(eUC_Text);
	}
}

MkTextNode* MkWindowTagNode::GetTagTextPtr(void)
{
	MkPanel* textPanel = GetPanel(TextPanelName);
	return (textPanel == NULL) ? NULL : textPanel->GetTextNodePtr();
}

void MkWindowTagNode::CommitTagText(void)
{
	MkTextNode* textNode = GetTagTextPtr();
	if (textNode != NULL)
	{
		textNode->Build();
		m_UpdateCommand.Set(eUC_Region);
	}
}

void MkWindowTagNode::SetLengthOfBetweenIconAndText(float length)
{
	if (length != m_LengthOfBetweenIconAndText)
	{
		m_LengthOfBetweenIconAndText = length;
		m_UpdateCommand.Set(eUC_Region);
	}
}

void MkWindowTagNode::Clear(void)
{
	m_IconPath.Clear();
	m_IconSubsetOrSequenceName.Clear();
	m_TextName.Clear();

	MkVisualPatternNode::Clear();
}

MkWindowTagNode::MkWindowTagNode(const MkHashStr& name) : MkVisualPatternNode(name)
{
	m_LengthOfBetweenIconAndText = 4.f;
}

bool MkWindowTagNode::__UpdateIcon(void)
{
	m_UpdateCommand.Set(eUC_Region); // icon이 변경되면 region도 갱신되어야 함

	if (!m_IconPath.Empty())
	{
		MkPanel* panel = PanelExist(IconPanelName) ? GetPanel(IconPanelName) : &CreatePanel(IconPanelName);

		MkTimeState ts;
		MK_TIME_MGR.GetCurrentTimeState(ts);

		if (panel->SetTexture(m_IconPath, m_IconSubsetOrSequenceName, ts.fullTime, 0.))
		{
			if (panel->GetTextureSize().IsPositive()) // 빈 icon은 허용하지 않음
				return true;
		}
	}

	DeletePanel(IconPanelName);
	return false;
}

bool MkWindowTagNode::__UpdateText(void)
{
	m_UpdateCommand.Set(eUC_Region); // text가 변경되면 region도 갱신되어야 함

	if ((!m_TextName.Empty()) && MK_STATIC_RES.TextNodeExist(m_TextName))
	{
		MkPanel* panel = PanelExist(TextPanelName) ? GetPanel(TextPanelName) : &CreatePanel(TextPanelName);
		panel->SetTextNode(m_TextName, false); // 빈 text node도 그려지지는 않지만 설정은 허용
		return true;
	}
	
	DeletePanel(TextPanelName);
	return false;
}

bool MkWindowTagNode::__UpdateRegion(void)
{
	m_UpdateCommand.Set(eUC_Alignment); // region이 변경되면 alignment도 갱신되야 함

	MkPanel* iconPanel = GetPanel(IconPanelName);
	MkPanel* textPanel = GetPanel(TextPanelName);
	if ((iconPanel == NULL) && (textPanel == NULL)) // icon, text 모두 없음
		return false;

	if ((iconPanel != NULL) && (textPanel == NULL)) // icon만 존재
	{
		m_WindowRect.size = m_ClientRect.size = iconPanel->GetTextureSize();
		iconPanel->SetLocalPosition(MkFloat2::Zero);
	}
	else if ((iconPanel == NULL) && (textPanel != NULL)) // text만 존재
	{
		m_WindowRect.size = m_ClientRect.size = textPanel->GetTextureSize();
		textPanel->SetLocalPosition(MkFloat2::Zero);
	}
	else if ((iconPanel != NULL) && (textPanel != NULL)) // icon, text 모두 존재
	{
		MkFloatRect iconRect(MkFloat2::Zero, iconPanel->GetTextureSize());
		MkFloatRect textRect(MkFloat2(iconRect.size.x + m_LengthOfBetweenIconAndText, 0.f), textPanel->GetTextureSize());

		MkFloatRect totalRect;
		totalRect.UpdateToUnion(iconRect);
		totalRect.UpdateToUnion(textRect);
		m_WindowRect.size = m_ClientRect.size = totalRect.size;

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

void MkWindowTagNode::_ExcuteUpdateCommand(void)
{
	if (m_UpdateCommand[eUC_Icon])
	{
		__UpdateIcon();
	}
	if (m_UpdateCommand[eUC_Text])
	{
		__UpdateText();
	}
	if (m_UpdateCommand[eUC_Region])
	{
		__UpdateRegion();
	}

	MkVisualPatternNode::_ExcuteUpdateCommand();
}

//------------------------------------------------------------------------------------------------//
