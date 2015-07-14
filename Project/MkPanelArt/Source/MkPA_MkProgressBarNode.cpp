
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowTagNode.h"
#include "MkPA_MkProgressBarNode.h"


const MkHashStr MkProgressBarNode::ProgressBarName(MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"PBar");
const MkHashStr MkProgressBarNode::ProgressTagName(MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"PTag");

const MkHashStr MkProgressBarNode::ObjKey_CurrentValue(L"CurrValue");
const MkHashStr MkProgressBarNode::ObjKey_MaximumValue(L"MaxValue");
const MkHashStr MkProgressBarNode::ObjKey_ShowProgressMode(L"ShowProgMode");

//------------------------------------------------------------------------------------------------//

MkProgressBarNode* MkProgressBarNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkProgressBarNode* node = __TSI_SceneNodeDerivedInstanceOp<MkProgressBarNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkProgressBarNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkProgressBarNode::SetProgressBar
(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length, int initValue, int maxValue, eShowProgressMode mode)
{
	m_FrameType = frameType;
	float frameSize = MK_STATIC_RES.GetWindowThemeSet().GetFrameSize(themeName, m_FrameType);

	// background bar
	SetThemeName(themeName);
	SetComponentType(MkWindowThemeData::eCT_DefaultBox);
	SetClientSize(MkFloat2(GetMax<float>(length, frameSize), frameSize));
	SetFormState(MkWindowThemeFormData::eS_Default);

	// progress bar
	SetCurrentValue(initValue);
	SetMaximumValue(maxValue);

	// progress tag
	SetShowProgressMode(mode);
}

void MkProgressBarNode::SetCurrentValue(int value)
{
	value = GetMax<int>(0, value);
	if (value != m_CurrentValue)
	{
		m_CurrentValue = value;
		m_UpdateCommand.Set(eUC_ProgressBar);
	}
}

void MkProgressBarNode::SetMaximumValue(int value)
{
	value = GetMax<int>(1, value);
	if (value != m_MaximumValue)
	{
		m_MaximumValue = value;
		m_UpdateCommand.Set(eUC_ProgressBar);
	}
}

int MkProgressBarNode::GetProgressPercentage(void) const
{
	return ((m_CurrentValue > 0) && (m_MaximumValue > 0)) ? ConvertToPercentage<int, int>(m_CurrentValue, m_MaximumValue) : 0;
}

void MkProgressBarNode::SetShowProgressMode(eShowProgressMode mode)
{
	if (mode != m_ShowProgressMode)
	{
		m_ShowProgressMode = mode;
		m_UpdateCommand.Set(eUC_ProgressTag);
	}
}

void MkProgressBarNode::Save(MkDataNode& node) const
{
	// bar, tag node 제외
	static MkArray<MkHashStr> exceptions(2);
	if (exceptions.Empty())
	{
		exceptions.PushBack(ProgressBarName);
		exceptions.PushBack(ProgressTagName);
	}
	_AddExceptionList(node, SystemKey_NodeExceptions, exceptions);

	// run
	MkWindowThemedNode::Save(node);
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkProgressBarNode);

void MkProgressBarNode::SetObjectTemplate(MkDataNode& node)
{
	MkWindowThemedNode::SetObjectTemplate(node);

	node.CreateUnit(ObjKey_CurrentValue, static_cast<int>(0));
	node.CreateUnit(ObjKey_MaximumValue, static_cast<int>(0));
	node.CreateUnit(ObjKey_ShowProgressMode, static_cast<int>(eSPM_None));
}

void MkProgressBarNode::LoadObject(const MkDataNode& node)
{
	MkWindowThemedNode::LoadObject(node);

	// curr value
	int currVal = 0;
	if (node.GetData(ObjKey_CurrentValue, currVal, 0))
	{
		SetCurrentValue(currVal);
	}

	// max value
	int maxVal = 0;
	if (node.GetData(ObjKey_MaximumValue, maxVal, 0))
	{
		SetMaximumValue(maxVal);
	}

	// show mode
	int showMode = 0;
	if (node.GetData(ObjKey_ShowProgressMode, showMode, 0))
	{
		SetShowProgressMode(static_cast<eShowProgressMode>(showMode));
	}
}

void MkProgressBarNode::SaveObject(MkDataNode& node) const
{
	MkWindowThemedNode::SaveObject(node);

	node.SetData(ObjKey_CurrentValue, m_CurrentValue, 0);
	node.SetData(ObjKey_MaximumValue, m_MaximumValue, 0);
	node.SetData(ObjKey_ShowProgressMode, static_cast<int>(m_ShowProgressMode), 0);
}

void MkProgressBarNode::Clear(void)
{
	m_CurrentValue = -1;
	m_MaximumValue = -1;

	MkWindowThemedNode::Clear();
}

MkProgressBarNode::MkProgressBarNode(const MkHashStr& name) : MkWindowThemedNode(name)
{
	m_CurrentValue = -1;
	m_MaximumValue = -1;
	m_ShowProgressMode = eSPM_None;
}

void MkProgressBarNode::_UpdateProgressBar(void)
{
	m_UpdateCommand.Set(eUC_ProgressTag);

	if ((m_MaximumValue > 0) && (m_CurrentValue > 0))
	{
		MkWindowThemedNode* barNode = NULL;
		if (ChildExist(ProgressBarName))
		{
			barNode = dynamic_cast<MkWindowThemedNode*>(GetChildNode(ProgressBarName));
		}
		else
		{
			barNode = MkWindowThemedNode::CreateChildNode(this, ProgressBarName);
			if (barNode != NULL)
			{
				barNode->SetThemeName(GetThemeName());
				barNode->SetComponentType(MkWindowThemeData::eCT_BlueZone);
				barNode->SetFormState(MkWindowThemeFormData::eS_Default);
				barNode->SetAlignmentPosition(eRAP_LeftCenter);
				barNode->SetLocalDepth(-0.1f); // bg bar와 겹치지 않도록 0.1f만큼 앞에 위치
			}
		}

		if (barNode != NULL)
		{
			MkFloat2 barClientSize = ConvertWindowToClientSize(GetThemeName(), GetComponentType(), GetCustomForm(), GetClientSize());

			int currValue = GetMin<int>(m_CurrentValue, m_MaximumValue); // m_CurrentValue가 m_MaximumValue를 초과 할 수도 있음
			float progress = static_cast<float>(currValue) / static_cast<float>(m_MaximumValue);
			barClientSize.x *= progress;

			barNode->SetClientSize(barClientSize);
		}
	}
	else
	{
		RemoveChildNode(ProgressBarName);
	}
}

void MkProgressBarNode::_UpdateProgressTag(void)
{
	if ((m_ShowProgressMode != eSPM_None) && (m_MaximumValue > 0) && (m_CurrentValue >= 0))
	{
		MkWindowTagNode* tagNode = NULL;
		if (ChildExist(ProgressTagName))
		{
			tagNode = dynamic_cast<MkWindowTagNode*>(GetChildNode(ProgressTagName));
		}
		else
		{
			tagNode = MkWindowTagNode::CreateChildNode(this, ProgressTagName);
			if (tagNode != NULL)
			{
				tagNode->SetTextName(MK_STATIC_RES.GetWindowThemeSet().GetCaptionTextNode(GetThemeName(), m_FrameType), MkStr::EMPTY);
				tagNode->SetAlignmentPosition(eRAP_MiddleCenter);
				tagNode->SetLocalDepth(-0.2f); // bar node와 겹치지 않도록 0.2f만큼 앞에 위치
			}
		}

		if (tagNode != NULL)
		{
			MkStr msg;
			msg.Reserve(30);

			switch (m_ShowProgressMode)
			{
			case eSPM_CurrentAndMax:
				msg += m_CurrentValue;
				msg += L" / ";
				msg += m_MaximumValue;
				break;

			case eSPM_Percentage:
				msg += GetProgressPercentage();
				msg += L" %";
				break;
			}

			if (msg.Empty())
			{
				RemoveChildNode(ProgressTagName);
			}
			else
			{
				MkTextNode* textNode = tagNode->GetTagTextPtr();
				textNode->SetText(msg);
				tagNode->CommitTagText();
			}
		}
	}
	else
	{
		RemoveChildNode(ProgressTagName);
	}
}

void MkProgressBarNode::_ExcuteUpdateCommand(void)
{
	if (m_UpdateCommand[eUC_Region] || m_UpdateCommand[eUC_ProgressBar])
	{
		_UpdateProgressBar();
	}

	if (m_UpdateCommand[eUC_ProgressTag])
	{
		_UpdateProgressTag();
	}

	MkWindowThemedNode::_ExcuteUpdateCommand();
}

//------------------------------------------------------------------------------------------------//
