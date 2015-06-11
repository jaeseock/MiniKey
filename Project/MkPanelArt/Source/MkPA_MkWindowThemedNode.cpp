
#include "MkCore_MkCheck.h"
#include "MkCore_MkTimeManager.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowThemedNode.h"


const MkHashStr MkWindowThemedNode::NodeNamePrefix(MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"Comp:");
const MkHashStr MkWindowThemedNode::ShadowNodeName(NodeNamePrefix.GetString() + L"Shadow");

const MkHashStr MkWindowThemedNode::ArgKey_ChangeTheme(L"ChangeTheme");

//------------------------------------------------------------------------------------------------//

MkWindowThemedNode* MkWindowThemedNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkWindowThemedNode* node = __TSI_SceneNodeDerivedInstanceOp<MkWindowThemedNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowThemedNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkWindowThemedNode::SetThemeName(const MkHashStr& themeName)
{
	if (themeName != m_ThemeName)
	{
		bool ok = true;
		if (m_ComponentType == MkWindowThemeData::eCT_CustomForm)
		{
			const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(themeName, m_ComponentType, m_CustomFormName);
			ok = (formData != NULL);
		}

		if (ok)
		{
			m_ThemeName = themeName;
			m_UpdateCommand.Set(eUC_ThemeComponent);
		}
	}
}

void MkWindowThemedNode::SetComponentType(MkWindowThemeData::eComponentType componentType)
{
	if ((componentType != MkWindowThemeData::eCT_CustomForm) && (componentType != m_ComponentType))
	{
		m_ComponentType = componentType;
		m_UpdateCommand.Set(eUC_ThemeComponent);
		SetFormState(MkWindowThemeFormData::eS_Default); // component가 변경되면 form type도 변경이 일어날 수 있기 때문에 form state 초기화

		m_CustomFormName.Clear();
	}
}

bool MkWindowThemedNode::SetCustomForm(const MkHashStr& customFormName)
{
	const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(m_ThemeName, MkWindowThemeData::eCT_CustomForm, customFormName);
	bool ok = (formData != NULL);
	if (ok && (customFormName != m_CustomFormName))
	{
		m_ComponentType = MkWindowThemeData::eCT_CustomForm;
		m_CustomFormName = customFormName;
		m_UpdateCommand.Set(eUC_ThemeComponent);
		SetFormState(MkWindowThemeFormData::eS_Default); // component가 변경되면 form type도 변경이 일어날 수 있기 때문에 form state 초기화
	}
	return ok;
}

void MkWindowThemedNode::SetShadowUsage(bool enable)
{
	if (enable != m_UseShadow)
	{
		m_UseShadow = enable;
		m_UpdateCommand.Set(eUC_ThemeComponent);
	}
}

void MkWindowThemedNode::ChangeThemeName(const MkHashStr& srcThemeName, const MkHashStr& destThemeName)
{
	MkArray<MkHashStr> names(2);
	names.PushBack(srcThemeName);
	names.PushBack(destThemeName);
	MkDataNode argument;
	argument.CreateUnitEx(ArgKey_ChangeTheme, names);

	SendNodeCommandTypeEvent(ePA_SNE_ChangeTheme, &argument);
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
	const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(m_ThemeName, m_ComponentType, m_CustomFormName);
	return (formData == NULL) ? MkWindowThemeFormData::eFT_None : formData->GetFormType();
}

void MkWindowThemedNode::SetFormState(MkWindowThemeFormData::eState formState)
{
	if (formState != m_FormState)
	{
		m_FormState = formState;
		m_UpdateCommand.Set(eUC_FormState);
	}
}

void MkWindowThemedNode::SendNodeCommandTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode* argument)
{
	switch (eventType)
	{
	case ePA_SNE_ChangeTheme:
		{
			MkArray<MkHashStr> names;
			if (argument->GetDataEx(ArgKey_ChangeTheme, names) && (names.GetSize() == 2))
			{
				if (names[0].Empty() || (names[0] == m_ThemeName))
				{
					SetThemeName(names[1]);
				}
			}
		}
		break;
	}
	
	MkVisualPatternNode::SendNodeCommandTypeEvent(eventType, argument);
}

void MkWindowThemedNode::Clear(void)
{
	m_ThemeName.Clear();
	m_ComponentType = MkWindowThemeData::eCT_None;
	m_UseShadow = false;
	m_FormState = MkWindowThemeFormData::eS_None;
	m_CustomFormName.Clear();

	MkVisualPatternNode::Clear();
}

MkWindowThemedNode::MkWindowThemedNode(const MkHashStr& name) : MkVisualPatternNode(name)
{
	m_ComponentType = MkWindowThemeData::eCT_None;
	m_UseShadow = false;
	m_FormState = MkWindowThemeFormData::eS_None;
}

MkFloat2 MkWindowThemedNode::ConvertWindowToClientSize
(const MkHashStr& themeName, MkWindowThemeData::eComponentType componentType, const MkHashStr& customFormName, const MkFloat2& windowSize)
{
	MkFloat2 clientSize = windowSize;
	const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(themeName, componentType, customFormName);
	if (formData != NULL)
	{
		clientSize.x -= formData->GetLeftMargin();
		clientSize.x -= formData->GetRightMargin();
		clientSize.y -= formData->GetTopMargin();
		clientSize.y -= formData->GetBottomMargin();
	}
	return clientSize;
}

bool MkWindowThemedNode::_UpdateThemeComponent(void)
{
	m_UpdateCommand.Set(eUC_Region); // theme, component, shadow가 변경되면 region도 갱신되야 함

	const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(m_ThemeName, m_ComponentType, m_CustomFormName);
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
						shadowNode->SetAlignmentTargetIsWindowRect(false); // 자신과 shadow node의 client rect끼리 정렬
						shadowNode->SetAlignmentPosition(eRAP_LeftBottom); // client size를 동일하게 맞출 것이므로 아무거나 상관 없음
					}
				}
			}
			return true;
		}
	}

	MkWindowThemeFormData::RemoveForm(this);
	RemoveChildNode(ShadowNodeName);
	return false;
}

bool MkWindowThemedNode::_UpdateFormState(void)
{
	if (m_FormState != MkWindowThemeFormData::eS_None)
	{
		const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(m_ThemeName, m_ComponentType, m_CustomFormName);
		return (formData == NULL) ? false : formData->SetFormState(this, m_FormState);
	}
	return false;
}

bool MkWindowThemedNode::_UpdateRegion(void)
{
	const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(m_ThemeName, m_ComponentType, m_CustomFormName);
	bool ok = (formData != NULL);
	if (ok)
	{
		MkFloatRect oldWinRect = m_WindowRect;
		MkFloatRect oldCliRect = m_ClientRect;
		
		formData->SetClientSizeToForm(this, m_ClientRect.size, m_ClientRect.position, m_WindowRect.size);

		if (m_WindowRect != oldWinRect)
		{
			m_UpdateCommand.Set(eUC_Alignment); // window rect가 변경되면 자신의 alignment도 갱신되야 함
		}

		// client rect에 변경이 발생하면 자식 visual pattern 파생 객체들에게 재정렬 명령
		if ((!m_ChildrenNode.Empty()) && (m_ClientRect != oldCliRect))
		{
			MkHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
			MK_STL_LOOP(looper)
			{
				if (looper.GetCurrentField()->IsDerivedFrom(ePA_SNT_VisualPatternNode))
				{
					// shadow node. client size를 동기화
					if (looper.GetCurrentField()->GetNodeName() == ShadowNodeName)
					{
						MkWindowThemedNode* shadowNode = dynamic_cast<MkWindowThemedNode*>(looper.GetCurrentField());
						if (shadowNode != NULL)
						{
							shadowNode->SetClientSize(m_ClientRect.size);
						}
					}
					// etc
					else
					{
						MkVisualPatternNode* etcNode = dynamic_cast<MkVisualPatternNode*>(looper.GetCurrentField());
						if (etcNode != NULL)
						{
							etcNode->SetAlignmentCommand();
						}
					}
				}
			}
		}
	}
	return ok;
}

void MkWindowThemedNode::_ExcuteUpdateCommand(void)
{
	if (m_UpdateCommand[eUC_ThemeComponent])
	{
		_UpdateThemeComponent();
	}
	if (m_UpdateCommand[eUC_Region])
	{
		_UpdateRegion();
	}
	if (m_UpdateCommand[eUC_FormState])
	{
		_UpdateFormState();
	}

	MkVisualPatternNode::_ExcuteUpdateCommand();
}

//------------------------------------------------------------------------------------------------//
