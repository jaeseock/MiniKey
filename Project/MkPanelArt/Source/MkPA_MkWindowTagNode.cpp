
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowTagNode.h"


const MkHashStr MkWindowTagNode::IconPanelName(MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"Icon");
const MkHashStr MkWindowTagNode::TextPanelName(MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"Text");

const MkHashStr MkWindowTagNode::ObjKey_IconPath(L"IconPath");
const MkHashStr MkWindowTagNode::ObjKey_IconSOSName(L"IconSOSName");
const MkHashStr MkWindowTagNode::ObjKey_TextName(L"TextName");
const MkHashStr MkWindowTagNode::ObjKey_TextMsg(L"TextMsg");
const MkHashStr MkWindowTagNode::ObjKey_LengthOfBetweenIT(L"LengthOfBetweenIT");

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

void MkWindowTagNode::SetTextName(const MkArray<MkHashStr>& textName)
{
	if (textName != m_TextName)
	{
		m_TextName = textName;
		m_UpdateCommand.Set(eUC_Text);
	}
}

void MkWindowTagNode::SetTextName(const MkArray<MkHashStr>& textName, const MkStr& msg)
{
	m_TextMsg = msg;

	bool txtNameChanged = false;
	if (textName != m_TextName)
	{
		m_TextName = textName;
		txtNameChanged = true;
	}

	MkPanel* textPanel = GetPanel(TextPanelName);

	// panel이 없거나 text name이 변경되었으면 바로 msg를 세팅해야되므로 즉각 반영
	if ((textPanel == NULL) || txtNameChanged)
	{
		if (_UpdateText())
		{
			textPanel = GetPanel(TextPanelName);
		}
	}

	if ((textPanel != NULL) && (!m_TextMsg.Empty()))
	{
		MkTextNode* textNode = textPanel->GetTextNodePtr();
		if (textNode != NULL)
		{
			textNode->SetText(m_TextMsg);
			textPanel->BuildAndUpdateTextCache();

			m_UpdateCommand.Set(eUC_Region);
		}
	}
}

MkTextNode* MkWindowTagNode::GetTagTextPtr(void)
{
	MkPanel* textPanel = GetPanel(TextPanelName);
	return (textPanel == NULL) ? NULL : textPanel->GetTextNodePtr();
}

const MkTextNode* MkWindowTagNode::GetTagTextPtr(void) const
{
	const MkPanel* textPanel = GetPanel(TextPanelName);
	return (textPanel == NULL) ? NULL : textPanel->GetTextNodePtr();
}

void MkWindowTagNode::CommitTagText(void)
{
	MkPanel* textPanel = GetPanel(TextPanelName);
	if (textPanel != NULL)
	{
		textPanel->BuildAndUpdateTextCache();
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
	m_TextMsg.Clear();

	MkVisualPatternNode::Clear();
}

void MkWindowTagNode::Save(MkDataNode& node) const
{
	// icon/text panel 제외
	static MkArray<MkHashStr> exceptions(2);
	if (exceptions.Empty())
	{
		exceptions.PushBack(IconPanelName);
		exceptions.PushBack(TextPanelName);
	}
	_AddExceptionList(node, SystemKey_PanelExceptions, exceptions);

	// run
	MkVisualPatternNode::Save(node);
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkWindowTagNode);

void MkWindowTagNode::SetObjectTemplate(MkDataNode& node)
{
	MkVisualPatternNode::SetObjectTemplate(node);

	node.CreateUnit(ObjKey_IconPath, MkStr::EMPTY);
	node.CreateUnit(ObjKey_IconSOSName, MkStr::EMPTY);
	// ObjKey_TextName
	// ObjKey_TextMsg
	node.CreateUnit(ObjKey_LengthOfBetweenIT, MKDEF_PA_DEFAULT_LENGTH_BETWEEN_WIN_COMPONENT_X);
}

void MkWindowTagNode::LoadObject(const MkDataNode& node)
{
	MkVisualPatternNode::LoadObject(node);

	// icon
	MkHashStr path;
	if (node.GetDataEx(ObjKey_IconPath, path, 0) && (!path.Empty()))
	{
		SetIconPath(path);

		MkHashStr sosName;
		node.GetDataEx(ObjKey_IconSOSName, sosName, 0);
		SetIconSubsetOrSequenceName(sosName);
	}

	// text
	MkArray<MkHashStr> textName;
	if (node.GetDataEx(ObjKey_TextName, textName))
	{
		MkStr msg;
		if (node.GetData(ObjKey_TextMsg, msg, 0))
		{
			if (msg.Empty())
			{
				SetTextName(textName);
			}
			else
			{
				SetTextName(textName, msg);
			}
		}
	}

	// gap
	float lengthOfBetweenIconAndText;
	if (node.GetData(ObjKey_LengthOfBetweenIT, lengthOfBetweenIconAndText, 0))
	{
		SetLengthOfBetweenIconAndText(lengthOfBetweenIconAndText);
	}
}

void MkWindowTagNode::SaveObject(MkDataNode& node) const
{
	MkVisualPatternNode::SaveObject(node);

	node.SetDataEx(ObjKey_IconPath, m_IconPath, 0);
	node.SetDataEx(ObjKey_IconSOSName, m_IconSubsetOrSequenceName, 0);

	if (!m_TextName.Empty())
	{
		node.CreateUnitEx(ObjKey_TextName, m_TextName);

		if (!m_TextMsg.Empty())
		{
			node.CreateUnit(ObjKey_TextMsg, m_TextMsg);
		}
	}
	
	node.SetDataEx(ObjKey_LengthOfBetweenIT, m_LengthOfBetweenIconAndText, 0);
}

MkWindowTagNode::MkWindowTagNode(const MkHashStr& name) : MkVisualPatternNode(name)
{
	m_LengthOfBetweenIconAndText = MKDEF_PA_DEFAULT_LENGTH_BETWEEN_WIN_COMPONENT_X;
}

MkFloat2 MkWindowTagNode::_ConvertToEvenValue(const MkFloat2& value) const
{
	return MkFloat2(MkFloatOp::SnapToUpperBound(value.x, 2.f), value.y);
}

bool MkWindowTagNode::_UpdateIcon(void)
{
	m_UpdateCommand.Set(eUC_Region); // icon이 변경되면 region도 갱신되어야 함
	
	if (!m_IconPath.Empty())
	{
		MkPanel* panel = PanelExist(IconPanelName) ? GetPanel(IconPanelName) : &CreatePanel(IconPanelName);
		if (panel->SetTexture(m_IconPath, m_IconSubsetOrSequenceName, 0.))
		{
			if (panel->GetTextureSize().IsPositive()) // 빈 icon은 허용하지 않음
				return true;
		}
	}

	DeletePanel(IconPanelName);
	return false;
}

bool MkWindowTagNode::_UpdateText(void)
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

bool MkWindowTagNode::_UpdateRegion(void)
{
	SetAlignmentCommand(); // region이 변경되면 alignment도 갱신되야 함

	MkPanel* iconPanel = GetPanel(IconPanelName);
	MkPanel* textPanel = GetPanel(TextPanelName);
	if ((iconPanel == NULL) && (textPanel == NULL)) // icon, text 모두 없음
		return false;

	if ((iconPanel != NULL) && (textPanel == NULL)) // icon만 존재
	{
		m_WindowRect.size = m_ClientRect.size = _ConvertToEvenValue(iconPanel->GetTextureSize());
		iconPanel->SetLocalPosition(MkFloat2::Zero);
	}
	else if ((iconPanel == NULL) && (textPanel != NULL)) // text만 존재
	{
		m_WindowRect.size = m_ClientRect.size = _ConvertToEvenValue(textPanel->GetTextureSize());
		textPanel->SetLocalPosition(MkFloat2::Zero);
	}
	else if ((iconPanel != NULL) && (textPanel != NULL)) // icon, text 모두 존재
	{
		MkFloatRect iconRect(MkFloat2::Zero, _ConvertToEvenValue(iconPanel->GetTextureSize()));
		MkFloatRect textRect(MkFloat2(iconRect.size.x + m_LengthOfBetweenIconAndText, 0.f), _ConvertToEvenValue(textPanel->GetTextureSize()));

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
		_UpdateIcon();
	}
	if (m_UpdateCommand[eUC_Text])
	{
		_UpdateText();
	}
	if (m_UpdateCommand[eUC_Region])
	{
		_UpdateRegion();
	}

	MkVisualPatternNode::_ExcuteUpdateCommand();
}

//------------------------------------------------------------------------------------------------//
