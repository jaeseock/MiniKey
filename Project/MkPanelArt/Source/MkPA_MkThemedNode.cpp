
#include "MkCore_MkCheck.h"
#include "MkCore_MkTimeManager.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkThemedNode.h"

const MkHashStr MkThemedNode::ShadowNodeName(MkStr(MKDEF_PA_WINDOW_THEME_TAG_PREFIX) + L"Shadow");

//------------------------------------------------------------------------------------------------//

MkThemedNode* MkThemedNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	if (parentNode != NULL)
	{
		MK_CHECK(!parentNode->ChildExist(childNodeName), parentNode->GetNodeName().GetString() + L" node에 이미 " + childNodeName.GetString() + L" 이름을 가진 자식이 존재")
			return NULL;
	}

	MkThemedNode* node = new MkThemedNode(childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkThemedNode alloc 실패")
		return NULL;

	if (parentNode != NULL)
	{
		parentNode->AttachChildNode(node);
	}
	return node;
}

bool MkThemedNode::SetTheme(const MkHashStr& themeName)
{
	bool ok = _SetTheme(themeName);
	if (ok)
	{
		MkSceneNode::__SendNodeEvent(_NodeEvent(static_cast<int>(eSetTheme), themeName));
	}
	return ok;
	
}

void MkThemedNode::SetComponent(MkWindowThemeData::eComponentType componentType)
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

void MkThemedNode::SetClientSize(const MkFloat2& clientSize)
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

void MkThemedNode::SetFormPosition(MkWindowThemeFormData::ePosition position)
{
	if ((position != m_FormPosition) && (position != MkWindowThemeFormData::eP_None))
	{
		m_FormPosition = position;
		_ApplyThemePosition(_GetValidForm());
	}
}

MkWindowThemeFormData::eFormType MkThemedNode::GetFormType(void) const
{
	const MkWindowThemeFormData* fd = _GetValidForm();
	return (fd == NULL) ? MkWindowThemeFormData::eFT_None : fd->GetFormType();
}

void MkThemedNode::SnapTo(const MkFloatRect& targetRect, eRectAlignmentPosition alignmentPosition)
{
	if (targetRect.size.IsPositive() && m_WindowSize.IsPositive())
	{
		MkFloat2 snapPos = targetRect.GetSnapPosition(m_WindowSize, alignmentPosition, MkFloat2::Zero, false); // border is zero
		SetLocalPosition(snapPos);
	}
}

void MkThemedNode::SnapToParentClientRect(eRectAlignmentPosition alignmentPosition)
{
	if ((m_ParentNodePtr != NULL) && (m_ParentNodePtr->GetNodeType() >= ePA_SNT_ThemedNode))
	{
		const MkThemedNode* parentThemedNode = dynamic_cast<const MkThemedNode*>(m_ParentNodePtr);
		SnapTo(parentThemedNode->GetClientRect(), alignmentPosition);
	}
}

void MkThemedNode::SetShadowEnable(bool enable)
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

void MkThemedNode::Clear(void)
{
	m_ClientRect.Clear();
	m_WindowSize.Clear();

	m_ThemeName.Clear();
	m_ComponentType = MkWindowThemeData::eCT_None;
	m_FormPosition = MkWindowThemeFormData::eP_None;

	SetShadowEnable(false);

	MkSceneNode::Clear();
}

MkThemedNode::MkThemedNode(const MkHashStr& name) : MkSceneNode(name)
{
	m_ComponentType = MkWindowThemeData::eCT_None;
	m_FormPosition = MkWindowThemeFormData::eP_None;
}

void MkThemedNode::__SendNodeEvent(const _NodeEvent& evt)
{
	switch (evt.eventType)
	{
	case static_cast<int>(eSetTheme):
		{
			if (!_SetTheme(evt.arg0))
				return;
		}
		break;
	}
	
	MkSceneNode::__SendNodeEvent(evt);
}

bool MkThemedNode::_SetTheme(const MkHashStr& themeName)
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

const MkWindowThemeFormData* MkThemedNode::_GetValidForm(void) const
{
	// 현재 지정된 theme이고 component가 유효하면
	return ((m_ThemeName == MK_STATIC_RES.GetWindowThemeSet().GetCurrentTheme()) && (m_ComponentType != MkWindowThemeData::eCT_None)) ?
		MK_STATIC_RES.GetWindowThemeSet().GetFormData(m_ComponentType) : NULL;
}

void MkThemedNode::_DeleteThemeForm(const MkWindowThemeFormData* form)
{
	if (form != NULL)
	{
		form->RemoveForm(this);
		m_FormPosition = MkWindowThemeFormData::eP_None;
	}
}

void MkThemedNode::_ApplyThemeForm(const MkWindowThemeFormData* form)
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

void MkThemedNode::_ApplyThemeSize(const MkWindowThemeFormData* form)
{
	if (form != NULL)
	{
		form->SetClientSizeToForm(this, m_ClientRect.size, m_ClientRect.position, m_WindowSize);
	}
}

void MkThemedNode::_ApplyThemePosition(const MkWindowThemeFormData* form)
{
	if ((form != NULL) && (m_FormPosition != MkWindowThemeFormData::eP_None))
	{
		form->SetFormPosition(this, m_FormPosition);
	}
}

void MkThemedNode::_DeleteShadowNode(void)
{
	MkSceneNode* shadowNode = GetChildNode(ShadowNodeName);
	if (shadowNode != NULL)
	{
		shadowNode->DetachFromParentNode();
		delete shadowNode;
	}
}

void MkThemedNode::_ApplyShadowNode(void)
{
	// 그림자 사용 설정중이고 현재 지정된 theme이고 component가 유효하면 진행
	if ((!GetShadowEnable()) || (m_ThemeName != MK_STATIC_RES.GetWindowThemeSet().GetCurrentTheme()) || (m_ComponentType == MkWindowThemeData::eCT_None))
		return;

	MkThemedNode* shadowNode = NULL;
	if (ChildExist(ShadowNodeName))
	{
		shadowNode = dynamic_cast<MkThemedNode*>(GetChildNode(ShadowNodeName));
		MK_CHECK(shadowNode != NULL, GetNodeName().GetString() + L" MkThemedNode에서 " + ShadowNodeName.GetString() + L" node가 MkThemedNode 객체가 아님")
			return;
	}
	else
	{
		shadowNode = CreateChildNode(this, ShadowNodeName);
		if (shadowNode == NULL)
			return;

		shadowNode->SetLocalDepth(0.1f); // 0.1f만큼 뒤에 위치
	}

	shadowNode->SetTheme(m_ThemeName);
	shadowNode->SetComponent(MkWindowThemeData::eCT_ShadowBox); // 고정

	_ApplyShadowSize(shadowNode);
}

void MkThemedNode::_ApplyShadowSize(MkThemedNode* shadowNode)
{
	if ((shadowNode == NULL) && ChildExist(ShadowNodeName))
	{
		shadowNode = dynamic_cast<MkThemedNode*>(GetChildNode(ShadowNodeName));
	}

	if (shadowNode != NULL)
	{
		shadowNode->SetClientSize(m_ClientRect.size);

		// component와 shadow의 client rect를 동일하게 맞춤
		MkFloat2 offset = m_ClientRect.position - shadowNode->GetClientRect().position;
		shadowNode->SetLocalPosition(offset);
	}
}

//------------------------------------------------------------------------------------------------//