
#include "MkCore_MkCheck.h"
#include "MkCore_MkTimeManager.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowThemedNode.h"


const MkHashStr MkWindowThemedNode::ShadowNodeName(MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"Shadow");

//------------------------------------------------------------------------------------------------//

MkWindowThemedNode* MkWindowThemedNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	if (parentNode != NULL)
	{
		MK_CHECK(!parentNode->ChildExist(childNodeName), parentNode->GetNodeName().GetString() + L" node�� �̹� " + childNodeName.GetString() + L" �̸��� ���� �ڽ��� ����")
			return NULL;
	}

	MkWindowThemedNode* node = new MkWindowThemedNode(childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowThemedNode alloc ����")
		return NULL;

	if (parentNode != NULL)
	{
		parentNode->AttachChildNode(node);
	}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkWindowThemedNode::SetTheme(const MkHashStr& themeName)
{
	__SendNodeEvent(_NodeEvent(static_cast<int>(eET_SetTheme), themeName, MkHashStr::EMPTY, MkFloat2::Zero, MkFloat2::Zero));
}

void MkWindowThemedNode::ChangeTheme(const MkHashStr& srcThemeName, const MkHashStr& destThemeName)
{
	__SendNodeEvent(_NodeEvent(static_cast<int>(eET_ChangeTheme), srcThemeName, destThemeName, MkFloat2::Zero, MkFloat2::Zero));
}

void MkWindowThemedNode::SetComponent(MkWindowThemeData::eComponentType componentType)
{
	if (componentType != m_ComponentType)
	{
		if (componentType == MkWindowThemeData::eCT_None)
		{
			_DeleteThemeForm(_GetValidForm());
			_DeleteShadowNode();
			m_ComponentType = MkWindowThemeData::eCT_None;
		}
		else
		{
			// component�� ������ form type�� �����ų �� ����
			// form type ������ �߻��ϸ� position �ʱ�ȭ, �����ϸ� �� form position�� �״�� �ݿ�
			if (m_ThemeName == MK_STATIC_RES.GetWindowThemeSet().GetCurrentTheme())
			{
				MkWindowThemeFormData::eFormType oldFT = MkWindowThemeData::ComponentFormType[m_ComponentType];
				MkWindowThemeFormData::eFormType newFT = MkWindowThemeData::ComponentFormType[componentType];
				if (oldFT != newFT)
				{
					m_FormPosition = static_cast<MkWindowThemeFormData::ePosition>(0);
				}
			}

			m_ComponentType = componentType;
			_ApplyThemeForm(_GetValidForm());
			_ApplyShadowNode();
		}
	}
}

void MkWindowThemedNode::SetFormPosition(MkWindowThemeFormData::ePosition position)
{
	if ((position != m_FormPosition) && (position != MkWindowThemeFormData::eP_None))
	{
		m_FormPosition = position;
		_ApplyThemePosition(_GetValidForm());
	}
}

MkWindowThemeFormData::eFormType MkWindowThemedNode::GetFormType(void) const
{
	const MkWindowThemeFormData* fd = _GetValidForm();
	return (fd == NULL) ? MkWindowThemeFormData::eFT_None : fd->GetFormType();
}

//------------------------------------------------------------------------------------------------//

void MkWindowThemedNode::CreateTag(const MkHashStr& name)
{
	DeleteTag(name);

	_TagData& tagData = m_Tags.Create(name);

	// node name ����
	tagData.nodeName = MkWindowTagNode::NodeNamePrefix.GetString() + name.GetString();
}

bool MkWindowThemedNode::SetTagInfo(const MkHashStr& name, const MkWindowTagNode::TagInfo& tagInfo)
{
	MK_CHECK(m_Tags.Exist(name), L"�������� �ʴ� " + name.GetString() + L" window tag ���� �õ�")
		return false;

	_TagData& tagData = m_Tags[name];

	MkWindowTagNode* tagNode = ChildExist(tagData.nodeName) ?
		dynamic_cast<MkWindowTagNode*>(GetChildNode(tagData.nodeName)) : MkWindowTagNode::CreateChildNode(this, tagData.nodeName);

	MK_CHECK(tagNode != NULL, name.GetString() + L" �̸��� window tag node�� MkWindowTagNode ��ü�� �ƴ�")
		return false;

	MkWindowTagNode::TagInfo& currInfo = tagData.tagInfo;

	// icon
	if ((tagInfo.iconPath != currInfo.iconPath) || (tagInfo.iconSubsetOrSequenceName != currInfo.iconSubsetOrSequenceName))
	{
		if (tagNode->UpdateIconInfo(tagInfo))
		{
			currInfo.iconPath = tagInfo.iconPath;
			currInfo.iconSubsetOrSequenceName = tagInfo.iconSubsetOrSequenceName;
		}
		else
		{
			currInfo.iconPath.Clear();
			currInfo.iconSubsetOrSequenceName.Clear();
		}
	}

	// text
	if (tagInfo.textName != currInfo.textName)
	{
		if (tagNode->UpdateTextInfo(tagInfo))
		{
			currInfo.textName = tagInfo.textName;
		}
		else
		{
			currInfo.textName.Clear();
		}
	}

	// region
	MK_CHECK(tagNode->UpdateRegionInfo(tagInfo), GetNodeName().GetString() + L" node���� icon/text�� ��� ���� tag ���� �õ�")
	{
		DeleteTag(name); // �ǹ� ���� tag�̹Ƿ� ����
		return false;
	}
	currInfo.lengthOfBetweenIconAndText = tagInfo.lengthOfBetweenIconAndText;

	// alignment
	tagNode->SetAlignmentPosition(tagInfo.alignmentPosition, tagInfo.alignmentOffset);
	tagNode->UpdateAlignmentPosition(m_ClientRect);

	currInfo.alignmentPosition = tagInfo.alignmentPosition;
	currInfo.alignmentOffset = tagInfo.alignmentOffset;

	return true;
}

bool MkWindowThemedNode::GetTagInfo(const MkHashStr& name, MkWindowTagNode::TagInfo& buffer) const
{
	bool ok = m_Tags.Exist(name);
	if (ok)
	{
		buffer = m_Tags[name].tagInfo;
	}
	return ok;
}

void MkWindowThemedNode::DeleteTag(const MkHashStr& name)
{
	if (m_Tags.Exist(name))
	{
		MkSceneNode* tagNode = GetChildNode(m_Tags[name].nodeName);
		if (tagNode != NULL)
		{
			tagNode->DetachFromParentNode();
			delete tagNode;
		}

		m_Tags.Erase(name);
	}
}

void MkWindowThemedNode::DeleteAllTags(void)
{
	MkConstHashMapLooper<MkHashStr, _TagData> looper(m_Tags);
	MK_STL_LOOP(looper)
	{
		MkSceneNode* tagNode = GetChildNode(looper.GetCurrentField().nodeName);
		if (tagNode != NULL)
		{
			tagNode->DetachFromParentNode();
			delete tagNode;
		}
	}

	m_Tags.Clear();
}

MkTextNode* MkWindowThemedNode::GetTagTextPtr(const MkHashStr& name)
{
	if (m_Tags.Exist(name))
	{
		MkWindowTagNode* tagNode = _GetTagNode(m_Tags[name].nodeName);
		return (tagNode == NULL) ? NULL : tagNode->GetTagTextPtr();
	}
	return NULL;
}

void MkWindowThemedNode::CommitTagText(const MkHashStr& name)
{
	if (m_Tags.Exist(name))
	{
		const _TagData& tagData = m_Tags[name];

		MkWindowTagNode* tagNode = _GetTagNode(tagData.nodeName);
		if (tagNode != NULL)
		{
			MkTextNode* textNode = tagNode->GetTagTextPtr();
			if (textNode != NULL)
			{
				textNode->Build();

				// text node�� ��ȭ�� tag ũ�� ���� ���ɼ��� �����Ƿ� region�� position�� ���� �ؾ� ��
				tagNode->UpdateRegionInfo(tagData.tagInfo); // UpdateRegionInfo()�� �׻� ����(text node�� ����)
				tagNode->UpdateAlignmentPosition(m_ClientRect);
			}
		}
	}
}

bool MkWindowThemedNode::SetTagTextFontStyle(const MkHashStr& name, const MkHashStr& fontStyle)
{
	MkTextNode* textNode = GetTagTextPtr(name);
	bool ok = (textNode != NULL) && (fontStyle != textNode->GetFontStyle()) && textNode->SetFontStyle(fontStyle);
	if (ok)
	{
		textNode->Build(); // font stype ��ȭ�� ũ�� ������ �ҷ����� �����Ƿ� cashe�� ������ϸ� ��
	}
	return ok;
}

//------------------------------------------------------------------------------------------------//

void MkWindowThemedNode::SetClientSize(const MkFloat2& clientSize)
{
	MkFloat2 sizeCopy = clientSize;
	sizeCopy.CompareAndKeepMax(MkFloat2::Zero); // ignore negative side

	if (sizeCopy != m_ClientRect.size)
	{
		m_ClientRect.size = sizeCopy;

		_ApplyThemeSize(_GetValidForm());
		_ApplyShadowSize(NULL);
	}
}

//------------------------------------------------------------------------------------------------//

void MkWindowThemedNode::SetShadowEnable(bool enable)
{
	m_Attribute.Assign(eAT_UseShadow, enable);

	if (enable)
	{
		_ApplyShadowNode();
	}
	else
	{
		_DeleteShadowNode();
	}
}

void MkWindowThemedNode::Clear(void)
{
	m_ClientRect.Clear();
	m_WindowSize.Clear();

	m_ThemeName.Clear();
	m_ComponentType = MkWindowThemeData::eCT_None;
	m_FormPosition = MkWindowThemeFormData::eP_None;

	SetShadowEnable(false);

	DeleteAllTags();

	MkVisualPatternNode::Clear();
}

MkWindowThemedNode::MkWindowThemedNode(const MkHashStr& name) : MkVisualPatternNode(name)
{
	m_ComponentType = MkWindowThemeData::eCT_None;
	m_FormPosition = MkWindowThemeFormData::eP_None;
}

//------------------------------------------------------------------------------------------------//

void MkWindowThemedNode::__SendNodeEvent(const _NodeEvent& evt)
{
	switch (evt.eventType)
	{
	case static_cast<int>(eET_SetTheme):
		{
			if (!_SetTheme(evt.arg0))
				return;
		}
		break;

	case static_cast<int>(eET_ChangeTheme):
		{
			if (!_ChangeTheme(evt.arg0, evt.arg1))
				return;
		}
		break;
	}
	
	MkVisualPatternNode::__SendNodeEvent(evt);
}

bool MkWindowThemedNode::_SetTheme(const MkHashStr& themeName)
{
	if (themeName != m_ThemeName)
	{
		if (themeName.Empty())
		{
			_DeleteThemeForm(_GetValidForm());
			_DeleteShadowNode();
			m_ThemeName.Clear();
		}
		else
		{
			if (!MK_STATIC_RES.GetWindowThemeSet().SetCurrentTheme(themeName))
				return false;

			// theme�� ����Ǿ��ٰ� form type�� ��������� ����(MkWindowThemeData::ComponentFormType�� ���� ������)
			// ���� �� form position�� �״�� �ݿ�
			m_ThemeName = themeName;
			_ApplyThemeForm(_GetValidForm());
			_ApplyShadowNode();
		}
	}
	return true;
}

bool MkWindowThemedNode::_ChangeTheme(const MkHashStr& srcThemeName, const MkHashStr& destThemeName)
{
	return (srcThemeName == m_ThemeName) ? _SetTheme(destThemeName) : true;
}

//------------------------------------------------------------------------------------------------//

const MkWindowThemeFormData* MkWindowThemedNode::_GetValidForm(void) const
{
	// ���� ������ theme�̰� component�� ��ȿ�ϸ�
	return ((m_ThemeName == MK_STATIC_RES.GetWindowThemeSet().GetCurrentTheme()) && (m_ComponentType != MkWindowThemeData::eCT_None)) ?
		MK_STATIC_RES.GetWindowThemeSet().GetFormData(m_ComponentType) : NULL;
}

void MkWindowThemedNode::_DeleteThemeForm(const MkWindowThemeFormData* form)
{
	if (form != NULL)
	{
		form->RemoveForm(this);
		m_FormPosition = MkWindowThemeFormData::eP_None;
	}
}

void MkWindowThemedNode::_ApplyThemeForm(const MkWindowThemeFormData* form)
{
	if (form != NULL)
	{
		MkTimeState ts;
		MK_TIME_MGR.GetCurrentTimeState(ts);

		form->AttachForm(this, ts.fullTime); // form

		_ApplyThemeSize(form); // size

		if (m_FormPosition == MkWindowThemeFormData::eP_None) // position
		{
			m_FormPosition = static_cast<MkWindowThemeFormData::ePosition>(0);
		}
		else if (static_cast<int>(m_FormPosition) > 0)
		{
			_ApplyThemePosition(form);
		}
	}
}

void MkWindowThemedNode::_ApplyThemeSize(const MkWindowThemeFormData* form)
{
	if (form != NULL)
	{
		form->SetClientSizeToForm(this, m_ClientRect.size, m_ClientRect.position, m_WindowSize);

		// ���� visual pattern node���� snap position �ݿ�
		if (!m_ChildrenNode.Empty())
		{
			_NodeEvent evt(static_cast<int>(MkVisualPatternNode::eET_UpdateAlignmentPosition), MkHashStr::EMPTY, MkHashStr::EMPTY, m_ClientRect.position, m_ClientRect.size);

			MkHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
			MK_STL_LOOP(looper)
			{
				looper.GetCurrentField()->__SendNodeEvent(evt);
			}
		}
	}
}

void MkWindowThemedNode::_ApplyThemePosition(const MkWindowThemeFormData* form)
{
	if ((form != NULL) && (m_FormPosition != MkWindowThemeFormData::eP_None))
	{
		form->SetFormPosition(this, m_FormPosition);
	}
}

void MkWindowThemedNode::_DeleteShadowNode(void)
{
	MkSceneNode* shadowNode = GetChildNode(ShadowNodeName);
	if (shadowNode != NULL)
	{
		shadowNode->DetachFromParentNode();
		delete shadowNode;
	}
}

void MkWindowThemedNode::_ApplyShadowNode(void)
{
	// �׸��� ��� �������̰� ���� ������ theme�̰� component�� ��ȿ�ϸ� ����
	if ((!GetShadowEnable()) || (m_ThemeName != MK_STATIC_RES.GetWindowThemeSet().GetCurrentTheme()) || (m_ComponentType == MkWindowThemeData::eCT_None))
		return;

	MkWindowThemedNode* shadowNode = NULL;
	if (ChildExist(ShadowNodeName))
	{
		shadowNode = dynamic_cast<MkWindowThemedNode*>(GetChildNode(ShadowNodeName));
		MK_CHECK(shadowNode != NULL, GetNodeName().GetString() + L" MkWindowThemedNode���� " + ShadowNodeName.GetString() + L" node�� MkWindowThemedNode ��ü�� �ƴ�")
			return;
	}
	else
	{
		shadowNode = CreateChildNode(this, ShadowNodeName);
		if (shadowNode == NULL)
			return;

		shadowNode->SetLocalDepth(0.1f); // form panel��� ��ġ�� ���� ���ϱ� ���� 0.1f��ŭ �ڿ� ��ġ
	}

	shadowNode->SetTheme(m_ThemeName);
	shadowNode->SetComponent(MkWindowThemeData::eCT_ShadowBox); // ����

	_ApplyShadowSize(shadowNode);
}

void MkWindowThemedNode::_ApplyShadowSize(MkWindowThemedNode* shadowNode)
{
	if ((shadowNode == NULL) && ChildExist(ShadowNodeName))
	{
		shadowNode = dynamic_cast<MkWindowThemedNode*>(GetChildNode(ShadowNodeName));
	}

	if (shadowNode != NULL)
	{
		shadowNode->SetClientSize(m_ClientRect.size);

		// component�� shadow�� client rect�� �����ϰ� ����
		MkFloat2 offset = m_ClientRect.position - shadowNode->GetClientRect().position;
		shadowNode->SetLocalPosition(offset);
	}
}

MkWindowTagNode* MkWindowThemedNode::_GetTagNode(const MkHashStr& nodeName)
{
	return ChildExist(nodeName) ? dynamic_cast<MkWindowTagNode*>(GetChildNode(nodeName)) : NULL;
}

//------------------------------------------------------------------------------------------------//