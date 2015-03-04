
#include "MkCore_MkCheck.h"
#include "MkCore_MkTimeManager.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowThemedNode.h"


const MkHashStr MkWindowThemedNode::NodeNamePrefix(MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"Comp:");
const MkHashStr MkWindowThemedNode::ShadowNodeName(NodeNamePrefix.GetString() + L"Shadow");

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

void MkWindowThemedNode::SetThemeName(const MkHashStr& themeName)
{
	if (themeName != m_ThemeName)
	{
		m_ThemeName = themeName;
		m_UpdateCommand.Set(eUC_ThemeComponent);
	}
}

void MkWindowThemedNode::SetComponentType(MkWindowThemeData::eComponentType componentType)
{
	if (componentType != m_ComponentType)
	{
		m_ComponentType = componentType;
		m_UpdateCommand.Set(eUC_ThemeComponent);

		// component가 변경되면 form type도 변경이 일어날 수 있기 때문에 form state 초기화
		SetFormState(MkWindowThemeFormData::eS_Default);
	}
}

void MkWindowThemedNode::SetShadowUsage(bool enable)
{
	if (enable != m_UseShadow)
	{
		m_UseShadow = enable;
		m_UpdateCommand.Set(eUC_ThemeComponent);
	}
}

void MkWindowThemedNode::SetClientSize(const MkFloat2& clientSize)
{
	MkFloat2 cs = clientSize;
	cs.CompareAndKeepMax(MkFloat2::Zero); // ignore negative side

	if (cs != m_ClientRect.size)
	{
		m_ClientRect.size = cs;
		m_UpdateCommand.Set(eUC_Region);
	}
}

MkWindowThemeFormData::eFormType MkWindowThemedNode::GetFormType(void) const
{
	if (MK_STATIC_RES.GetWindowThemeSet().SetCurrentTheme(m_ThemeName))
	{
		const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(m_ComponentType);
		if (formData != NULL)
		{
			return formData->GetFormType();
		}
	}
	return MkWindowThemeFormData::eFT_None;
}

void MkWindowThemedNode::SetFormState(MkWindowThemeFormData::eState formState)
{
	if (formState != m_FormState)
	{
		m_FormState = formState;
		m_UpdateCommand.Set(eUC_FormState);
	}
}

MkWindowThemedNode::MkWindowThemedNode(const MkHashStr& name) : MkVisualPatternNode(name)
{
	m_ComponentType = MkWindowThemeData::eCT_None;
	m_UseShadow = false;
	m_FormState = MkWindowThemeFormData::eS_None;
}

bool MkWindowThemedNode::__UpdateThemeComponent(void)
{
	m_UpdateCommand.Set(eUC_Region); // theme, component, shadow가 변경되면 region도 갱신되야 함

	if (MK_STATIC_RES.GetWindowThemeSet().SetCurrentTheme(m_ThemeName))
	{
		const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(m_ComponentType);
		if (formData != NULL)
		{
			MkTimeState ts;
			MK_TIME_MGR.GetCurrentTimeState(ts);
			if (formData->AttachForm(this, ts.fullTime))
			{
				if (m_UseShadow)
				{
					if (ChildExist(ShadowNodeName))
					{
						MkWindowThemedNode* shadowNode = dynamic_cast<MkWindowThemedNode*>(GetChildNode(ShadowNodeName));
						if (shadowNode != NULL)
						{
							shadowNode->SetThemeName(m_ThemeName);
						}
					}
					else
					{
						MkWindowThemedNode* shadowNode = CreateChildNode(this, ShadowNodeName);
						if (shadowNode != NULL)
						{
							shadowNode->SetLocalDepth(0.1f); // form panel들과 겹치는 것을 피하기 위해 0.1f만큼 뒤에 위치
							shadowNode->SetThemeName(m_ThemeName);
							shadowNode->SetComponentType(MkWindowThemeData::eCT_ShadowBox);
							shadowNode->SetFormState(MkWindowThemeFormData::eS_Default);

							// shadow는 __UpdateRegion()에서 바로 영역 계산을 요구하기때문에 정상적인 Update()라인을 통한
							// 생성을 따르지 않고 바로 panel들을 만듬
							shadowNode->__UpdateThemeComponent();
						}
					}
				}
				else
				{
					RemoveChildNode(ShadowNodeName);
				}

				return true;
			}
		}
	}

	MkWindowThemeFormData::RemoveForm(this);
	RemoveChildNode(ShadowNodeName);
	return false;
}

bool MkWindowThemedNode::__UpdateFormState(void)
{
	if ((m_FormState != MkWindowThemeFormData::eS_None) && MK_STATIC_RES.GetWindowThemeSet().SetCurrentTheme(m_ThemeName))
	{
		const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(m_ComponentType);
		if (formData != NULL)
		{
			return formData->SetFormState(this, m_FormState);
		}
	}
	return false;
}

bool MkWindowThemedNode::__UpdateRegion(void)
{
	m_UpdateCommand.Set(eUC_Alignment); // region이 변경되면 alignment도 갱신되야 함

	if (MK_STATIC_RES.GetWindowThemeSet().SetCurrentTheme(m_ThemeName))
	{
		const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(m_ComponentType);
		if (formData != NULL)
		{
			formData->SetClientSizeToForm(this, m_ClientRect.size, m_ClientRect.position, m_WindowRect.size);

			// 자식 visual pattern node 재정렬
			if (!m_ChildrenNode.Empty())
			{
				MkHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
				MK_STL_LOOP(looper)
				{
					if (looper.GetCurrentField()->IsDerivedFrom(ePA_SNT_VisualPatternNode))
					{
						// shadow node
						if (looper.GetCurrentField()->GetNodeName() == ShadowNodeName)
						{
							MkWindowThemedNode* shadowNode = dynamic_cast<MkWindowThemedNode*>(looper.GetCurrentField());
							if (shadowNode != NULL)
							{
								shadowNode->SetClientSize(m_ClientRect.size);
								shadowNode->__UpdateRegion(); // 바로 계산

								// component와 shadow의 client rect를 동일하게 맞춤
								MkFloat2 offset = m_ClientRect.position - shadowNode->GetClientRect().position;
								shadowNode->SetLocalPosition(offset);
							}
						}
						// etc
						else
						{
							MkVisualPatternNode* etcNode = dynamic_cast<MkVisualPatternNode*>(looper.GetCurrentField());
							if (etcNode != NULL)
							{
								etcNode->__UpdateAlignment(this);
							}
						}
					}
				}
			}
			return true;
		}
	}
	return false;
}

void MkWindowThemedNode::_ExcuteUpdateCommand(void)
{
	if (m_UpdateCommand[eUC_ThemeComponent])
	{
		__UpdateThemeComponent();
	}
	if (m_UpdateCommand[eUC_Region])
	{
		__UpdateRegion();
	}
	if (m_UpdateCommand[eUC_FormState])
	{
		__UpdateFormState();
	}

	MkVisualPatternNode::_ExcuteUpdateCommand();
}

//------------------------------------------------------------------------------------------------//
