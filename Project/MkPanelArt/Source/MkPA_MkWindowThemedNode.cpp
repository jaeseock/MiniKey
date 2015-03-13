
#include "MkCore_MkCheck.h"
#include "MkCore_MkTimeManager.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowThemedNode.h"


const MkHashStr MkWindowThemedNode::NodeNamePrefix(MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"Comp:");

const MkHashStr MkWindowThemedNode::CustomImagePanelName(MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"CImg");
const MkHashStr MkWindowThemedNode::ShadowNodeName(NodeNamePrefix.GetString() + L"Shadow");

const static MkHashStr CHANGE_THEME_ARG_KEY = L"ThemeName";

//------------------------------------------------------------------------------------------------//

MkWindowThemedNode* MkWindowThemedNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkWindowThemedNode* node = __TSI_SceneNodeDerivedInstanceOp<MkWindowThemedNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowThemedNode ���� ����") {}
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
		SetFormState(MkWindowThemeFormData::eS_Default); // component�� ����Ǹ� form type�� ������ �Ͼ �� �ֱ� ������ form state �ʱ�ȭ

		m_CustomImagePath.Clear();
		m_CustomSubsetOrSequenceName.Clear();
	}
}

void MkWindowThemedNode::SetCustomImagePath(const MkHashStr& imagePath)
{
	if (imagePath != m_CustomImagePath)
	{
		m_CustomImagePath = imagePath;
		m_UpdateCommand.Set(eUC_ThemeComponent);
		
		m_ComponentType = MkWindowThemeData::eCT_None;
	}
}

void MkWindowThemedNode::SetCustomImageSubsetOrSequenceName(const MkHashStr& subsetOrSequenceName)
{
	if (subsetOrSequenceName != m_CustomSubsetOrSequenceName)
	{
		m_CustomSubsetOrSequenceName = subsetOrSequenceName;
		m_UpdateCommand.Set(eUC_ThemeComponent);
		
		m_ComponentType = MkWindowThemeData::eCT_None;
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

void MkWindowThemedNode::ChangeThemeName(const MkHashStr& srcThemeName, const MkHashStr& destThemeName)
{
	MkArray<MkHashStr> names(2);
	names.PushBack(srcThemeName);
	names.PushBack(destThemeName);
	MkDataNode argument;
	argument.CreateUnitEx(CHANGE_THEME_ARG_KEY, names);

	SendNodeCommandTypeEvent(ePA_SNE_ChangeTheme, argument);
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

void MkWindowThemedNode::SendNodeCommandTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode& argument)
{
	switch (eventType)
	{
	case ePA_SNE_ChangeTheme:
		{
			MkArray<MkHashStr> names;
			if (argument.GetDataEx(CHANGE_THEME_ARG_KEY, names) && (names.GetSize() == 2))
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
	m_CustomImagePath.Clear();
	m_CustomSubsetOrSequenceName.Clear();

	MkVisualPatternNode::Clear();
}

MkWindowThemedNode::MkWindowThemedNode(const MkHashStr& name) : MkVisualPatternNode(name)
{
	m_ComponentType = MkWindowThemeData::eCT_None;
	m_UseShadow = false;
	m_FormState = MkWindowThemeFormData::eS_None;
}

bool MkWindowThemedNode::__UpdateThemeComponent(void)
{
	m_UpdateCommand.Set(eUC_Region); // theme, component, custom image, shadow�� ����Ǹ� region�� ���ŵǾ� ��

	if (MK_STATIC_RES.GetWindowThemeSet().SetCurrentTheme(m_ThemeName))
	{
		bool ok = false;
		MkTimeState ts;
		MK_TIME_MGR.GetCurrentTimeState(ts);

		// custom image
		if (m_ComponentType == MkWindowThemeData::eCT_None)
		{
			MkPanel* panel = PanelExist(CustomImagePanelName) ? GetPanel(CustomImagePanelName) : &CreatePanel(CustomImagePanelName);
			if (panel != NULL)
			{
				if (panel->SetTexture(m_CustomImagePath, m_CustomSubsetOrSequenceName, ts.fullTime, 0.) && panel->GetTextureSize().IsPositive())
				{
					m_FormState = MkWindowThemeFormData::eS_None; // custom image�� form type�� �������� ����
					m_UpdateCommand.Clear(eUC_FormState);

					ok = true;
				}
			}
		}
		// component
		else
		{
			const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(m_ComponentType);
			if (formData != NULL)
			{
				ok = formData->AttachForm(this, ts.fullTime);
			}
		}

		if (ok)
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
						shadowNode->SetLocalDepth(0.1f); // form panel��� ��ġ�� ���� ���ϱ� ���� 0.1f��ŭ �ڿ� ��ġ
						shadowNode->SetThemeName(m_ThemeName);
						shadowNode->SetComponentType(MkWindowThemeData::eCT_ShadowBox);
						shadowNode->SetFormState(MkWindowThemeFormData::eS_Default);

						// shadow�� __UpdateRegion()���� �ٷ� ���� ����� �䱸�ϱ⶧���� �������� Update()������ ����
						// ������ ������ �ʰ� �ٷ� panel���� ����
						shadowNode->__UpdateThemeComponent();
					}
				}
			}
			return true;
		}
	}

	MkWindowThemeFormData::RemoveForm(this);
	DeletePanel(CustomImagePanelName);
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
	m_UpdateCommand.Set(eUC_Alignment); // region�� ����Ǹ� alignment�� ���ŵǾ� ��

	if (MK_STATIC_RES.GetWindowThemeSet().SetCurrentTheme(m_ThemeName))
	{
		bool ok = false;
		MkFloatRect oldClient = m_ClientRect;

		// custom image
		if (m_ComponentType == MkWindowThemeData::eCT_None)
		{
			MkPanel* panel = GetPanel(CustomImagePanelName);
			if (panel != NULL)
			{
				m_ClientRect.position.Clear();
				m_ClientRect.size = panel->GetTextureSize();
				m_WindowRect = m_ClientRect;
				ok = true;
			}
		}
		// component
		else
		{
			const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(m_ComponentType);
			if (formData != NULL)
			{
				formData->SetClientSizeToForm(this, m_ClientRect.size, m_ClientRect.position, m_WindowRect.size);
				ok = true;
			}
		}

		// client rect�� ������ �߻��ϸ� �ڽ� visual pattern node ������
		// local transform�� �����ϴ� ���̹Ƿ� node�� ���Ĵ� ���� ����(�ڽĵ� ���� �����ϰ� �θ�� ���߿� �ص� ��)
		if (ok)
		{
			if ((!m_ChildrenNode.Empty()) && (m_ClientRect != oldClient))
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
								shadowNode->__UpdateRegion(); // �ٷ� ���

								// component�� shadow�� client rect�� �����ϰ� ����
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
