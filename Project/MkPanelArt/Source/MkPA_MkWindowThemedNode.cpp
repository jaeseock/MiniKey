
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowThemedNode.h"


const MkHashStr MkWindowThemedNode::ShadowNodeName(MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"Shadow");

const MkHashStr MkWindowThemedNode::ArgKey_ChangeTheme(L"ChangeTheme");

const MkHashStr MkWindowThemedNode::ObjKey_ThemeName(L"ThemeName");
const MkHashStr MkWindowThemedNode::ObjKey_ComponentType(L"ComponentType");
const MkHashStr MkWindowThemedNode::ObjKey_CustomFormName(L"CustomFormName");
const MkHashStr MkWindowThemedNode::ObjKey_UseShadow(L"UseShadow");
const MkHashStr MkWindowThemedNode::ObjKey_ClientSize(L"ClientSize");
const MkHashStr MkWindowThemedNode::ObjKey_FrameType(L"FrameType");

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
	m_FrameType = MkWindowThemeData::eFT_None;

	MkVisualPatternNode::Clear();
}

void MkWindowThemedNode::Save(MkDataNode& node) const
{
	// component panel 제외
	static MkArray<MkHashStr> panelExceptions(10);
	if (panelExceptions.Empty())
	{
		panelExceptions.PushBack(MkWindowThemeUnitData::ImagePositionName);
		panelExceptions.PushBack(MkWindowThemeUnitData::EdgeAndTablePositionName[MkWindowThemeUnitData::eP_LT]);
		panelExceptions.PushBack(MkWindowThemeUnitData::EdgeAndTablePositionName[MkWindowThemeUnitData::eP_MT]);
		panelExceptions.PushBack(MkWindowThemeUnitData::EdgeAndTablePositionName[MkWindowThemeUnitData::eP_RT]);
		panelExceptions.PushBack(MkWindowThemeUnitData::EdgeAndTablePositionName[MkWindowThemeUnitData::eP_LC]);
		panelExceptions.PushBack(MkWindowThemeUnitData::EdgeAndTablePositionName[MkWindowThemeUnitData::eP_MC]);
		panelExceptions.PushBack(MkWindowThemeUnitData::EdgeAndTablePositionName[MkWindowThemeUnitData::eP_RC]);
		panelExceptions.PushBack(MkWindowThemeUnitData::EdgeAndTablePositionName[MkWindowThemeUnitData::eP_LB]);
		panelExceptions.PushBack(MkWindowThemeUnitData::EdgeAndTablePositionName[MkWindowThemeUnitData::eP_MB]);
		panelExceptions.PushBack(MkWindowThemeUnitData::EdgeAndTablePositionName[MkWindowThemeUnitData::eP_RB]);
	}
	_AddExceptionList(node, SystemKey_PanelExceptions, panelExceptions);

	// shadow node 제외
	static MkArray<MkHashStr> nodeExceptions;
	if (nodeExceptions.Empty())
	{
		nodeExceptions.PushBack(ShadowNodeName);
	}
	_AddExceptionList(node, SystemKey_NodeExceptions, nodeExceptions);

	// run
	MkVisualPatternNode::Save(node);
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkWindowThemedNode);

void MkWindowThemedNode::SetObjectTemplate(MkDataNode& node)
{
	MkVisualPatternNode::SetObjectTemplate(node);

	node.CreateUnit(ObjKey_ThemeName, MkStr::EMPTY);
	node.CreateUnit(ObjKey_ComponentType, MkStr::EMPTY);
	node.CreateUnit(ObjKey_CustomFormName, MkStr::EMPTY);
	node.CreateUnit(ObjKey_UseShadow, false);
	node.CreateUnitEx(ObjKey_ClientSize, MkFloat2::Zero);
	node.CreateUnit(ObjKey_FrameType, MkStr::EMPTY);
	
}

void MkWindowThemedNode::LoadObject(const MkDataNode& node)
{
	MkVisualPatternNode::LoadObject(node);

	// theme name
	MkHashStr themeName;
	if (node.GetDataEx(ObjKey_ThemeName, themeName, 0) && (!themeName.Empty()))
	{
		SetThemeName(themeName);
	}

	// component
	MkHashStr componentBuffer;
	if (node.GetDataEx(ObjKey_ComponentType, componentBuffer, 0) && (!componentBuffer.Empty()))
	{
		SetComponentType(MkWindowThemeData::ConvertComponentNameToType(componentBuffer));
	}
	else if (node.GetDataEx(ObjKey_CustomFormName, componentBuffer, 0) && (!componentBuffer.Empty()))
	{
		SetComponentType(MkWindowThemeData::eCT_CustomForm);
		SetCustomForm(componentBuffer);
	}

	// shadow
	bool useShadow;
	if (node.GetData(ObjKey_UseShadow, useShadow, 0))
	{
		SetShadowUsage(useShadow);
	}

	// client size
	MkFloat2 clientSize;
	if (node.GetDataEx(ObjKey_ClientSize, clientSize, 0))
	{
		SetClientSize(clientSize);
	}

	// frame type
	MkHashStr frameTypeName;
	if (node.GetDataEx(ObjKey_FrameType, frameTypeName, 0) && (!frameTypeName.Empty()))
	{
		m_FrameType = MkWindowThemeData::ConvertFrameNameToType(frameTypeName);
	}

	// form state -> default
	SetFormState(MkWindowThemeFormData::eS_Default);
}

void MkWindowThemedNode::SaveObject(MkDataNode& node) const
{
	MkVisualPatternNode::SaveObject(node);

	// theme name
	node.SetDataEx(ObjKey_ThemeName, m_ThemeName, 0);
	
	// component
	if ((m_ComponentType > MkWindowThemeData::eCT_None) && (m_ComponentType < MkWindowThemeData::eCT_RegularMax))
	{
		node.SetDataEx(ObjKey_ComponentType, MkWindowThemeData::ComponentTypeName[m_ComponentType], 0);
	}
	else if (m_ComponentType == MkWindowThemeData::eCT_CustomForm)
	{
		node.SetDataEx(ObjKey_CustomFormName, m_CustomFormName, 0);
	}

	// use shadow
	node.SetData(ObjKey_UseShadow, m_UseShadow, 0);

	// client size
	node.SetDataEx(ObjKey_ClientSize, m_ClientRect.size, 0);

	// frame type
	if ((m_FrameType > MkWindowThemeData::eFT_None) && (m_FrameType < MkWindowThemeData::eFT_Max))
	{
		node.SetDataEx(ObjKey_FrameType, MkWindowThemeData::FrameTypeName[m_FrameType], 0);
	}
}

MkWindowThemedNode::MkWindowThemedNode(const MkHashStr& name) : MkVisualPatternNode(name)
{
	m_ComponentType = MkWindowThemeData::eCT_None;
	m_UseShadow = false;
	m_FormState = MkWindowThemeFormData::eS_None;
	m_FrameType = MkWindowThemeData::eFT_None;
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

MkFloat2 MkWindowThemedNode::ConvertClientToWindowSize
(const MkHashStr& themeName, MkWindowThemeData::eComponentType componentType, const MkHashStr& customFormName, const MkFloat2& clientSize)
{
	MkFloat2 windowSize = clientSize;
	const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(themeName, componentType, customFormName);
	if (formData != NULL)
	{
		windowSize.x += formData->GetLeftMargin();
		windowSize.x += formData->GetRightMargin();
		windowSize.y += formData->GetTopMargin();
		windowSize.y += formData->GetBottomMargin();
	}
	return windowSize;
}

MkFloat2 MkWindowThemedNode::CalculateWindowSize(void) const
{
	return ConvertClientToWindowSize(GetThemeName(), GetComponentType(), GetCustomForm(), GetClientSize());
}

bool MkWindowThemedNode::_UpdateThemeComponent(void)
{
	m_UpdateCommand.Set(eUC_Region); // theme, component, shadow가 변경되면 region도 갱신되야 함

	const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(m_ThemeName, m_ComponentType, m_CustomFormName);
	if ((formData != NULL) && formData->AttachForm(this))
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
			// 자신의 alignment 갱신 요청
			SetAlignmentCommand();

			// 자식 visual pattern 파생 객체들에게 재정렬 명령
			if (!m_ChildrenNode.Empty())
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
