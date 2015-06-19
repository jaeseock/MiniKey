
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
const MkHashStr MkWindowTagNode::ObjKey_LengthOfBetweenIT(L"LengthOfBetweenIT");

#define MKDEF_DEFAULT_LENGTH_OF_GAP 4.f

//------------------------------------------------------------------------------------------------//

MkWindowTagNode* MkWindowTagNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkWindowTagNode* node = __TSI_SceneNodeDerivedInstanceOp<MkWindowTagNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowTagNode ���� ����") {}
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
	if (textName != m_TextName)
	{
		m_TextName = textName;
	}

	MkPanel* textPanel = GetPanel(TextPanelName);

	if ((textPanel == NULL) && _UpdateText()) // panel�� ������ �ٷ� caption�� �����ؾߵǹǷ� �ﰢ �ݿ�
	{
		textPanel = GetPanel(TextPanelName);
	}

	if (textPanel != NULL)
	{
		MkTextNode* textNode = textPanel->GetTextNodePtr();
		if (textNode != NULL)
		{
			textNode->SetText(msg);
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

	MkVisualPatternNode::Clear();
}

void MkWindowTagNode::Save(MkDataNode& node) const
{
	// icon/text panel ����
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
	node.CreateUnit(ObjKey_LengthOfBetweenIT, MKDEF_DEFAULT_LENGTH_OF_GAP);
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
		SetTextName(textName);
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
	}

	node.SetDataEx(ObjKey_LengthOfBetweenIT, m_LengthOfBetweenIconAndText, 0);
}

MkWindowTagNode::MkWindowTagNode(const MkHashStr& name) : MkVisualPatternNode(name)
{
	m_LengthOfBetweenIconAndText = MKDEF_DEFAULT_LENGTH_OF_GAP;
}

bool MkWindowTagNode::_UpdateIcon(void)
{
	m_UpdateCommand.Set(eUC_Region); // icon�� ����Ǹ� region�� ���ŵǾ�� ��
	
	if (!m_IconPath.Empty())
	{
		MkPanel* panel = PanelExist(IconPanelName) ? GetPanel(IconPanelName) : &CreatePanel(IconPanelName);
		if (panel->SetTexture(m_IconPath, m_IconSubsetOrSequenceName, 0.))
		{
			if (panel->GetTextureSize().IsPositive()) // �� icon�� ������� ����
				return true;
		}
	}

	DeletePanel(IconPanelName);
	return false;
}

bool MkWindowTagNode::_UpdateText(void)
{
	m_UpdateCommand.Set(eUC_Region); // text�� ����Ǹ� region�� ���ŵǾ�� ��

	if ((!m_TextName.Empty()) && MK_STATIC_RES.TextNodeExist(m_TextName))
	{
		MkPanel* panel = PanelExist(TextPanelName) ? GetPanel(TextPanelName) : &CreatePanel(TextPanelName);
		panel->SetTextNode(m_TextName, false); // �� text node�� �׷������� ������ ������ ���
		return true;
	}
	
	DeletePanel(TextPanelName);
	return false;
}

bool MkWindowTagNode::_UpdateRegion(void)
{
	SetAlignmentCommand(); // region�� ����Ǹ� alignment�� ���ŵǾ� ��

	MkPanel* iconPanel = GetPanel(IconPanelName);
	MkPanel* textPanel = GetPanel(TextPanelName);
	if ((iconPanel == NULL) && (textPanel == NULL)) // icon, text ��� ����
		return false;

	if ((iconPanel != NULL) && (textPanel == NULL)) // icon�� ����
	{
		m_WindowRect.size = m_ClientRect.size = iconPanel->GetTextureSize();
		iconPanel->SetLocalPosition(MkFloat2::Zero);
	}
	else if ((iconPanel == NULL) && (textPanel != NULL)) // text�� ����
	{
		m_WindowRect.size = m_ClientRect.size = textPanel->GetTextureSize();
		textPanel->SetLocalPosition(MkFloat2::Zero);
	}
	else if ((iconPanel != NULL) && (textPanel != NULL)) // icon, text ��� ����
	{
		MkFloatRect iconRect(MkFloat2::Zero, iconPanel->GetTextureSize());
		MkFloatRect textRect(MkFloat2(iconRect.size.x + m_LengthOfBetweenIconAndText, 0.f), textPanel->GetTextureSize());

		MkFloatRect totalRect;
		totalRect.UpdateToUnion(iconRect);
		totalRect.UpdateToUnion(textRect);
		m_WindowRect.size = m_ClientRect.size = totalRect.size;

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
