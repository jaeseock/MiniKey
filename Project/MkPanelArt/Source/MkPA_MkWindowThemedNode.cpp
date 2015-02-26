
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
		MK_CHECK(!parentNode->ChildExist(childNodeName), parentNode->GetNodeName().GetString() + L" node에 이미 " + childNodeName.GetString() + L" 이름을 가진 자식이 존재")
			return NULL;
	}

	MkWindowThemedNode* node = new MkWindowThemedNode(childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowThemedNode alloc 실패")
		return NULL;

	if (parentNode != NULL)
	{
		parentNode->AttachChildNode(node);
	}
	return node;
}

//------------------------------------------------------------------------------------------------//

bool MkWindowThemedNode::SetTheme(const MkHashStr& themeName)
{
	bool ok = _SetTheme(themeName);
	if (ok)
	{
		MkVisualPatternNode::__SendNodeEvent(_NodeEvent(static_cast<int>(eET_SetTheme), themeName));
	}
	return ok;
	
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
			// component의 변경은 form type도 변경시킬 수 있음
			// form type 변경이 발생하면 position 초기화, 동일하면 현 form position을 그대로 반영
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

	// 초기화
	tagData.tagInfo.lengthOfBetweenIconAndText = 10.f;
	tagData.tagInfo.alignmentPosition = eRAP_LeftTop;

	// node name 생성
	tagData.nodeName = MkWindowTagNode::NodeNamePrefix.GetString() + name.GetString();
}

bool MkWindowThemedNode::SetTagInfo(const MkHashStr& name, const MkWindowTagNode::TagInfo& tagInfo)
{
	MK_CHECK(m_Tags.Exist(name), L"존재하지 않는 " + name.GetString() + L" window tag 설정 시도")
		return false;

	_TagData& tagData = m_Tags[name];

	MkWindowTagNode* tagNode = ChildExist(tagData.nodeName) ?
		dynamic_cast<MkWindowTagNode*>(GetChildNode(tagData.nodeName)) : MkWindowTagNode::CreateChildNode(this, tagData.nodeName);

	MK_CHECK(tagNode != NULL, name.GetString() + L" 이름의 window tag node가 MkWindowTagNode 객체가 아님")
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
	MK_CHECK(tagNode->UpdateRegionInfo(tagInfo), GetNodeName().GetString() + L" node에서 icon/text가 모두 없는 tag 설정 시도")
	{
		DeleteTag(name); // 의미 없는 tag이므로 삭제
		return false;
	}
	currInfo.lengthOfBetweenIconAndText = tagInfo.lengthOfBetweenIconAndText;

	// position
	tagNode->UpdatePositionInfo(tagInfo, m_ClientRect);
	currInfo.alignmentPosition = tagInfo.alignmentPosition;
	currInfo.offset = tagInfo.offset;

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

				// text node의 변화는 tag 크기 변경 가능성이 있으므로 region과 position을 재계산 해야 함
				tagNode->UpdateRegionInfo(tagData.tagInfo); // UpdateRegionInfo()는 항상 성공(text node가 존재)
				tagNode->UpdatePositionInfo(tagData.tagInfo, m_ClientRect);
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
		textNode->Build(); // font stype 변화는 크기 변경을 불러오지 않으므로 cashe만 재생성하면 됨
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

			// theme가 변경되었다고 form type도 변경되지는 않음(MkWindowThemeData::ComponentFormType에 의해 강제됨)
			// 따라서 현 form position을 그대로 반영
			m_ThemeName = themeName;
			_ApplyThemeForm(_GetValidForm());
			_ApplyShadowNode();
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------//

const MkWindowThemeFormData* MkWindowThemedNode::_GetValidForm(void) const
{
	// 현재 지정된 theme이고 component가 유효하면
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

		// reposition tags
		MkConstHashMapLooper<MkHashStr, _TagData> looper(m_Tags);
		MK_STL_LOOP(looper)
		{
			const _TagData& tagData = looper.GetCurrentField();
			MkWindowTagNode* tagNode = _GetTagNode(tagData.nodeName);
			if (tagNode != NULL)
			{
				tagNode->UpdatePositionInfo(tagData.tagInfo, m_ClientRect);
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
	// 그림자 사용 설정중이고 현재 지정된 theme이고 component가 유효하면 진행
	if ((!GetShadowEnable()) || (m_ThemeName != MK_STATIC_RES.GetWindowThemeSet().GetCurrentTheme()) || (m_ComponentType == MkWindowThemeData::eCT_None))
		return;

	MkWindowThemedNode* shadowNode = NULL;
	if (ChildExist(ShadowNodeName))
	{
		shadowNode = dynamic_cast<MkWindowThemedNode*>(GetChildNode(ShadowNodeName));
		MK_CHECK(shadowNode != NULL, GetNodeName().GetString() + L" MkWindowThemedNode에서 " + ShadowNodeName.GetString() + L" node가 MkWindowThemedNode 객체가 아님")
			return;
	}
	else
	{
		shadowNode = CreateChildNode(this, ShadowNodeName);
		if (shadowNode == NULL)
			return;

		shadowNode->SetLocalDepth(0.1f); // form panel들과 겹치는 것을 피하기 위해 0.1f만큼 뒤에 위치
	}

	shadowNode->SetTheme(m_ThemeName);
	shadowNode->SetComponent(MkWindowThemeData::eCT_ShadowBox); // 고정

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

		// component와 shadow의 client rect를 동일하게 맞춤
		MkFloat2 offset = m_ClientRect.position - shadowNode->GetClientRect().position;
		shadowNode->SetLocalPosition(offset);
	}
}

MkWindowTagNode* MkWindowThemedNode::_GetTagNode(const MkHashStr& nodeName)
{
	return ChildExist(nodeName) ? dynamic_cast<MkWindowTagNode*>(GetChildNode(nodeName)) : NULL;
}

//------------------------------------------------------------------------------------------------//