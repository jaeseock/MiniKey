
#include "MkCore_MkTimeManager.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkThemedNode.h"


const MkHashStr MkThemedNode::ShadowNodeName(L"__#Shadow");

//------------------------------------------------------------------------------------------------//

bool MkThemedNode::SetTheme(const MkHashStr& themeName)
{
	if (themeName != m_ThemeName)
	{
		if (themeName.Empty())
		{
			_DeleteThemeForm(_GetValidForm());
			m_ThemeName.Clear();
		}
		else
		{
			if (!MK_STATIC_RES.GetWindowThemeSet().SetCurrentTheme(themeName))
				return false;

			m_ThemeName = themeName;
			_ApplyThemeForm(_GetValidForm());
		}
	}
	return true;
}

void MkThemedNode::SetComponent(MkWindowThemeData::eComponentType componentType)
{
	if (componentType != m_ComponentType)
	{
		if (componentType == MkWindowThemeData::eCT_None)
		{
			_DeleteThemeForm(_GetValidForm());
			m_ComponentType = MkWindowThemeData::eCT_None;
		}
		else
		{
			m_ComponentType = componentType;
			_ApplyThemeForm(_GetValidForm());
		}
	}
}

void MkThemedNode::SetClientSize(const MkFloat2& clientSize)
{
	if (clientSize != m_ClientRect.size)
	{
		m_ClientRect.size = clientSize;
		_ApplyThemeSize(_GetValidForm());
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

void MkThemedNode::UseShadow(bool enable)
{
	/*
	if (enable && (!m_UseShadow)) // turn on
	{
		MK_CHECK(!ChildExist(ShadowNodeName), ShadowNodeName.GetString() + L" node가 이미 존재하는데 shadow를 부착 시도")
			return;
		
	}
	else if ((!enable) && m_UseShadow) // turn off
	{
		MK_CHECK(ChildExist(ShadowNodeName), ShadowNodeName.GetString() + L" node가 없어 shadow를 끌 수 없음")
			return;
	}
	*/

	m_UseShadow = enable;
}

void MkThemedNode::Clear(void)
{
	m_ClientRect.Clear();
	m_WindowSize.Clear();

	m_ThemeName.Clear();
	m_ComponentType = MkWindowThemeData::eCT_None;
	m_UseShadow = false;
	m_FormPosition = MkWindowThemeFormData::eP_None;

	MkSceneNode::Clear();
}

MkThemedNode::MkThemedNode(const MkHashStr& name) : MkSceneNode(name)
{
	m_ComponentType = MkWindowThemeData::eCT_None;
	m_UseShadow = false;
	m_FormPosition = MkWindowThemeFormData::eP_None;
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

		if (m_UseShadow)
		{
			MkSceneNode* shadowNode = GetChildNode(ShadowNodeName);
			if (shadowNode != NULL)
			{
				shadowNode->DetachFromParentNode();
				delete shadowNode;
			}
		}
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

		if (static_cast<int>(m_FormPosition) != 0)
		{
			_ApplyThemePosition(form); // position
		}

		if (m_UseShadow)
		{
			MkThemedNode* shadowNode = ChildExist(ShadowNodeName) ? dynamic_cast<MkThemedNode*>(GetChildNode(ShadowNodeName)) : new MkThemedNode(ShadowNodeName);
			if (shadowNode != NULL)
			{
				//shadowNode->SetLocalPosition(MkFloat2(500.f, 100.f));
				shadowNode->SetTheme(m_ThemeName);
				shadowNode->SetComponent(MkWindowThemeData::eCT_Shadow); // 고정
				shadowNode->SetClientSize(m_ClientRect.size);

				const MkFloatRect& shadowClientRect = shadowNode->GetClientRect();

			}
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

//------------------------------------------------------------------------------------------------//