
#include "MkCore_MkCheck.h"
#include "MkCore_MkTimeManager.h"

//#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkTextNode.h"
#include "MkPA_MkWindowTagNode.h"
#include "MkPA_MkWindowThemedNode.h"
#include "MkPA_MkWindowBaseNode.h"


//------------------------------------------------------------------------------------------------//

MkWindowBaseNode* MkWindowBaseNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	if (parentNode != NULL)
	{
		MK_CHECK(!parentNode->ChildExist(childNodeName), parentNode->GetNodeName().GetString() + L" node에 이미 " + childNodeName.GetString() + L" 이름을 가진 자식이 존재")
			return NULL;
	}

	MkWindowBaseNode* node = new MkWindowBaseNode(childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowBaseNode alloc 실패")
		return NULL;

	if (parentNode != NULL)
	{
		parentNode->AttachChildNode(node);
	}
	return node;
}

//------------------------------------------------------------------------------------------------//

MkWindowBaseNode::_PositionInfo::_PositionInfo()
{
	align = eRAP_NonePosition;
	depth = 0.f;
}

//------------------------------------------------------------------------------------------------//

MkWindowBaseNode::_TagInfo::_TagInfo()
{
	lengthOfBetweenIconAndText = 4.f;
}

void MkWindowBaseNode::CreateTag(const MkHashStr& name)
{
	DeleteTag(name);

	_TagData& data = m_Tags.Create(name);
	data.node = NULL;
	data.nodeName = MkWindowTagNode::NodeNamePrefix.GetString() + name.GetString();
}

bool MkWindowBaseNode::SetTagInfo(const MkHashStr& name, const TagInfo& tagInfo)
{
	MK_CHECK(m_Tags.Exist(name), L"존재하지 않는 " + name.GetString() + L" window tag 설정 시도")
		return false;

	_TagData& tagData = m_Tags[name];
	if (tagData.node == NULL)
	{
		tagData.node = MkWindowTagNode::CreateChildNode(this, tagData.nodeName);
	}

	if (tagData.node == NULL)
		return false; // alloc 실패

	TagInfo& currInfo = tagData.tagInfo;

	// icon
	if ((tagInfo.iconPath != currInfo.iconPath) || (tagInfo.iconSubsetOrSequenceName != currInfo.iconSubsetOrSequenceName))
	{
		tagData.node->SetIconPath(tagInfo.iconPath);
		tagData.node->SetIconSubsetOrSequenceName(tagInfo.iconSubsetOrSequenceName);
		if (tagData.node->__UpdateIcon())
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
		tagData.node->SetTextName(tagInfo.textName);
		if (tagData.node->__UpdateText())
		{
			currInfo.textName = tagInfo.textName;
		}
		else
		{
			currInfo.textName.Clear();
		}
	}

	// region
	tagData.node->SetLengthOfBetweenIconAndText(tagInfo.lengthOfBetweenIconAndText);
	MK_CHECK(tagData.node->__UpdateRegion(), GetNodeName().GetString() + L" node에서 icon/text가 모두 없는 tag 설정 시도")
	{
		DeleteTag(name); // 의미 없는 tag이므로 삭제
		return false;
	}
	currInfo.lengthOfBetweenIconAndText = tagInfo.lengthOfBetweenIconAndText;

	// position
	_ApplyPosition(tagData.node, tagData.posInfo);

	return true;
}

bool MkWindowBaseNode::GetTagInfo(const MkHashStr& name, TagInfo& buffer) const
{
	bool ok = m_Tags.Exist(name);
	if (ok)
	{
		buffer = m_Tags[name].tagInfo;
	}
	return ok;
}

bool MkWindowBaseNode::SetTagPositionInfo(const MkHashStr& name, const PositionInfo& posInfo)
{
	MK_CHECK(m_Tags.Exist(name), L"존재하지 않는 " + name.GetString() + L" window tag 참조 시도")
		return false;

	_TagData& data = m_Tags[name];
	data.posInfo = posInfo;

	_ApplyPosition(data.node, data.posInfo);
	return true;
}

bool MkWindowBaseNode::GetTagPositionInfo(const MkHashStr& name, PositionInfo& buffer) const
{
	bool ok = m_Tags.Exist(name);
	if (ok)
	{
		buffer = m_Tags[name].posInfo;
	}
	return ok;
}

void MkWindowBaseNode::DeleteTag(const MkHashStr& name)
{
	if (m_Tags.Exist(name))
	{
		RemoveChildNode(m_Tags[name].nodeName);
		m_Tags.Erase(name);
	}
}

void MkWindowBaseNode::DeleteAllTags(void)
{
	MkHashMapLooper<MkHashStr, _TagData> looper(m_Tags);
	MK_STL_LOOP(looper)
	{
		_TagData& data = looper.GetCurrentField();
		RemoveChildNode(data.nodeName);
	}
	m_Tags.Clear();
}

MkTextNode* MkWindowBaseNode::GetTagTextPtr(const MkHashStr& name)
{
	if (m_Tags.Exist(name))
	{
		_TagData& data = m_Tags[name];
		return (data.node == NULL) ? NULL : data.node->GetTagTextPtr();
	}
	return NULL;
}

void MkWindowBaseNode::CommitTagText(const MkHashStr& name)
{
	if (m_Tags.Exist(name))
	{
		_TagData& data = m_Tags[name];
		if (data.node != NULL)
		{
			data.node->CommitTagText();
		}
	}
}

bool MkWindowBaseNode::SetTagTextFontStyle(const MkHashStr& name, const MkHashStr& fontStyle)
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

MkWindowBaseNode::_ComponentInfo::_ComponentInfo()
{
	componentType = MkWindowThemeData::eCT_None;
	useShadow = false;
	formState = MkWindowThemeFormData::eS_Default;
}

void MkWindowBaseNode::CreateComponent(const MkHashStr& name)
{
	DeleteComponent(name);

	_CompData& data = m_Components.Create(name);
	data.node = NULL;
	data.nodeName = MkWindowThemedNode::NodeNamePrefix.GetString() + name.GetString();
}

bool MkWindowBaseNode::SetComponentInfo(const MkHashStr& name, const ComponentInfo& compInfo)
{
	MK_CHECK(m_Components.Exist(name), L"존재하지 않는 " + name.GetString() + L" window component 설정 시도")
		return false;

	_CompData& compData = m_Components[name];
	if (compData.node == NULL)
	{
		compData.node = MkWindowThemedNode::CreateChildNode(this, compData.nodeName);
	}

	if (compData.node == NULL)
		return false; // alloc 실패

	ComponentInfo& currInfo = compData.compInfo;

	do
	{
		// theme, component
		if ((compInfo.themeName != currInfo.themeName) || (compInfo.componentType != currInfo.componentType))
		{
			compData.node->SetThemeName(compInfo.themeName);
			compData.node->SetComponentType(compInfo.componentType);
			compData.node->SetShadowUsage(compInfo.useShadow);
			if (compData.node->__UpdateThemeComponent())
			{
				currInfo.themeName = compInfo.themeName;
				currInfo.componentType = compInfo.componentType;
				currInfo.useShadow = compInfo.useShadow;
			}
			else
				break;
		}

		// region
		if (compInfo.clientSize != currInfo.clientSize)
		{
			compData.node->SetClientSize(compInfo.clientSize);
			if (!compData.node->__UpdateRegion())
				break;

			currInfo.clientSize = compData.node->GetClientRect().size;
		}

		// form state
		if (compInfo.formState != currInfo.formState)
		{
			compData.node->SetFormState(compInfo.formState);
			if (compData.node->__UpdateFormState())
			{
				currInfo.formState = compInfo.formState;
			}
			else
				break;
		}

		// position
		_ApplyPosition(compData.node, compData.posInfo);
		return true;
	}
	while (false);

	DeleteComponent(name); // 의미 없는 component이므로 삭제
	return false;
}

bool MkWindowBaseNode::GetComponentInfo(const MkHashStr& name, ComponentInfo& buffer) const
{
	bool ok = m_Components.Exist(name);
	if (ok)
	{
		buffer = m_Components[name].compInfo;
	}
	return ok;
}

bool MkWindowBaseNode::SetComponentPositionInfo(const MkHashStr& name, const PositionInfo& posInfo)
{
	MK_CHECK(m_Components.Exist(name), L"존재하지 않는 " + name.GetString() + L" window component 참조 시도")
		return false;

	_CompData& data = m_Components[name];
	data.posInfo = posInfo;

	_ApplyPosition(data.node, data.posInfo);
	return true;
}

bool MkWindowBaseNode::GetComponentPositionInfo(const MkHashStr& name, PositionInfo& buffer) const
{
	bool ok = m_Components.Exist(name);
	if (ok)
	{
		buffer = m_Components[name].posInfo;
	}
	return ok;
}

void MkWindowBaseNode::DeleteComponent(const MkHashStr& name)
{
	if (m_Components.Exist(name))
	{
		RemoveChildNode(m_Components[name].nodeName);
		m_Components.Erase(name);
	}
}

void MkWindowBaseNode::DeleteAllComponents(void)
{
	MkHashMapLooper<MkHashStr, _CompData> looper(m_Components);
	MK_STL_LOOP(looper)
	{
		_CompData& data = looper.GetCurrentField();
		RemoveChildNode(data.nodeName);
	}
	m_Components.Clear();
}

//------------------------------------------------------------------------------------------------//

void MkWindowBaseNode::Clear(void)
{
	DeleteAllTags();
	DeleteAllComponents();

	MkVisualPatternNode::Clear();
}

//------------------------------------------------------------------------------------------------//

void MkWindowBaseNode::_ApplyPosition(MkVisualPatternNode* node, const PositionInfo& info)
{
	if (node != NULL)
	{
		if (m_Components.Exist(info.pivotComponent))
		{
			MkWindowThemedNode* pivotNode = m_Components[info.pivotComponent].node;
			if (pivotNode != NULL)
			{
				node->SetAlignmentPosition(info.align);
				node->SetAlignmentOffset(info.offset);
				node->__UpdateAlignment(pivotNode);

				// node가 pivotNode의 자식이면 여기서 끝나지만 그렇지 않으므로 pivotNode의 local position을 반영
				node->SetLocalPosition(node->GetLocalPosition() + pivotNode->GetLocalPosition());
			}
		}

		node->SetLocalDepth(info.depth);
	}
}

void MkWindowBaseNode::_UpdateRegion(void)
{
	m_ClientRect.Clear();
	m_WindowRect.Clear();

	// 모든 component들 대상(tag는 해당되지 않음)
	MkHashMapLooper<MkHashStr, _CompData> looper(m_Components);
	MK_STL_LOOP(looper)
	{
		_CompData& data = looper.GetCurrentField();
		if (data.node != NULL)
		{
			// 모든 client 영역의 합이 client rect
			MkFloatRect clientRegion(data.node->GetLocalPosition() + data.node->GetClientRect().position, data.node->GetClientRect().size);
			m_ClientRect.UpdateToUnion(clientRegion);

			// 모든 window 영역의 합이 window rect
			MkFloatRect windowRegion(data.node->GetLocalPosition() + data.node->GetWindowRect().position, data.node->GetWindowRect().size);
			m_WindowRect.UpdateToUnion(windowRegion);
		}
	}
}
/*
void MkWindowBaseNode::__SendNodeEvent(const _NodeEvent& evt)
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
*/
//------------------------------------------------------------------------------------------------//