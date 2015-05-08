
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowTagNode.h"
#include "MkPA_MkSliderControlNode.h"


const MkHashStr MkSliderControlNode::BtnNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Btn";
const MkHashStr MkSliderControlNode::ValueTagName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"VTag";

const MkHashStr MkSliderControlNode::ArgKey_SliderPos = L"SliderPos";


// button과 value tag간 거리
#define MKDEF_VALUE_TAG_OFFSET 1.f

// value tag가 조건을 만족하지 못할 때 사라지는 시간 길이(sec)
#define MKDEF_VALUE_TAG_HIDING_TERM 0.3

//------------------------------------------------------------------------------------------------//

MkSliderControlNode* MkSliderControlNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkSliderControlNode* node = __TSI_SceneNodeDerivedInstanceOp<MkSliderControlNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkSliderControlNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkSliderControlNode::SetHorizontalSlider
(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length, int beginValue, int valueSize, int initValue, bool showValue)
{
	_SetSlider(themeName, frameType, length, beginValue, valueSize, initValue, showValue, true);
}

void MkSliderControlNode::SetVerticalSlider
(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length, int beginValue, int valueSize, int initValue, bool showValue)
{
	_SetSlider(themeName, frameType, length, beginValue, valueSize, initValue, showValue, false);
}

void MkSliderControlNode::SetSliderRange(int beginValue, int valueSize)
{
	valueSize = GetMax<int>(1, valueSize);
	if ((beginValue != m_BeginValue) || (valueSize != m_ValueSize))
	{
		float fraction = _CalculateFraction();

		m_BeginValue = beginValue;
		m_ValueSize = valueSize;

		// 기존 비율대로 재조정
		SetSliderValue(_CalculateValue(fraction));
	}
}

void MkSliderControlNode::SetSliderValue(int value)
{
	value = Clamp<int>(value, m_BeginValue, m_BeginValue + m_ValueSize);
	if (value != m_CurrentValue)
	{
		m_CurrentValue = value;

		_ApplyValueToSliderButton();
		_SendSliderMovementEvent();
	}
}

void MkSliderControlNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	// drag movement이고 해당 window가 slider button이면 value 산출 후 event 소멸
	// 만약 value가 변경되었다면 ePA_SNE_SliderMoved event 발생시킴
	if ((eventType == ePA_SNE_DragMovement) && (path.GetSize() == 1) && (path[0] == BtnNodeName))
	{
		MkSceneNode* node = GetChildNode(BtnNodeName);
		float currProgress = (m_Horizontal) ?
			(node->GetLocalPosition().x - m_DraggingMovementLock.position.x) : (node->GetLocalPosition().y - m_DraggingMovementLock.position.y);

		currProgress += 0.001f; // floating 계산의 오차 감안

		float fraction = currProgress / m_LineLength; // 0.f ~ 1.f
		int newValue = _CalculateValue(fraction);

		if (newValue != m_CurrentValue)
		{
			m_CurrentValue = newValue;

			MkDataNode arg;
			arg.CreateUnit(ArgKey_SliderPos, m_CurrentValue);
			StartNodeReportTypeEvent(ePA_SNE_SliderMoved, &arg);
		}
		return;
	}

	// drag begin이고 해당 window가 slider button이면 button drag flag 킴
	if ((eventType == ePA_SNE_BeginDrag) && (path.GetSize() == 1) && (path[0] == BtnNodeName))
	{
		m_ButtonDragging = true;
	}

	// drag end고 해당 window가 slider button이면 button drag flag 끄고 정렬
	if ((eventType == ePA_SNE_EndDrag) && (path.GetSize() == 1) && (path[0] == BtnNodeName))
	{
		m_ButtonDragging = false;
		_ApplyValueToSliderButton();
	}

	// 수직 slider인데 wheel이 움직였으면 value 변경, event 소멸
	if ((eventType == ePA_SNE_WheelMoved) && (!m_Horizontal) && (argument != NULL))
	{
		int delta;
		if (argument->GetData(MkWindowBaseNode::ArgKey_WheelDelta, delta, 0))
		{
			SetSliderValue(m_CurrentValue + delta);
			return;
		}
	}

	// cursor가 slide bar, button에 들어왔으면 cursor focusing flag 킴
	if (eventType == ePA_SNE_CursorEntered)
	{
		m_CursorFocusing = true;
	}

	// cursor가 slide bar, button에서 나갔으면 cursor focusing flag 끔
	if (eventType == ePA_SNE_CursorLeft)
	{
		m_CursorFocusing = false;
	}

	// value 변경이 발생했고 value tag가 존재하면 반영
	if ((eventType == ePA_SNE_SliderMoved) && ChildExist(BtnNodeName))
	{
		MkSceneNode* btnNode = GetChildNode(BtnNodeName);
		if ((btnNode != NULL) && btnNode->ChildExist(ValueTagName))
		{
			MkWindowTagNode* tagNode = dynamic_cast<MkWindowTagNode*>(btnNode->GetChildNode(ValueTagName));
			if (tagNode != NULL)
			{
				MkTextNode* textNode = tagNode->GetTagTextPtr();
				if (textNode != NULL)
				{
					textNode->SetText(MkStr(m_CurrentValue));
					tagNode->CommitTagText();
				}
			}
		}
	}

	MkWindowBaseNode::SendNodeReportTypeEvent(eventType, path, argument);
}

void MkSliderControlNode::Update(double currTime)
{
	// value tag 상태 갱신
	MkSceneNode* btnNode = GetChildNode(BtnNodeName);
	if ((btnNode != NULL) && btnNode->ChildExist(ValueTagName))
	{
		MkSceneNode* tagNode = btnNode->GetChildNode(ValueTagName);
		if (tagNode != NULL)
		{
			if (m_CursorFocusing || m_ButtonDragging)
			{
				m_ValueTagTimeStamp = currTime;

				tagNode->SetVisible(true);
				tagNode->SetLocalAlpha(1.f);
			}
			else if (tagNode->GetVisible())
			{
				float alpha = 1.f - static_cast<float>(Clamp<double>(currTime - m_ValueTagTimeStamp, 0., MKDEF_VALUE_TAG_HIDING_TERM) / MKDEF_VALUE_TAG_HIDING_TERM);
				tagNode->SetVisible(alpha > 0.f);
				tagNode->SetLocalAlpha(alpha);
			}
		}
	}

	MkWindowBaseNode::Update(currTime);
}

void MkSliderControlNode::Clear(void)
{
	m_LineLength = 0.f;
	m_CursorFocusing = false;
	m_ButtonDragging = false;
	m_ValueTagTimeStamp = 0.;

	MkWindowBaseNode::Clear();
}

MkSliderControlNode::MkSliderControlNode(const MkHashStr& name) : MkWindowBaseNode(name)
{
	m_LineLength = 0.f;
	m_BeginValue = 0;
	m_ValueSize = 100;
	m_CurrentValue = 0;
	m_Horizontal = false;
	m_CursorFocusing = false;
	m_ButtonDragging = false;
	m_ValueTagTimeStamp = 0.;
}

void MkSliderControlNode::_SetSlider
(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length, int beginValue, int valueSize, int initValue, bool showValue, bool horizontal)
{
	m_LineLength = GetMax<float>(0.f, length);

	m_WindowFrameType = frameType;
	m_Horizontal = horizontal;

	// slider bar
	SetThemeName(themeName);
	SetComponentType(MkWindowThemeData::eCT_GuideLine);
	SetFormState(MkWindowThemeFormData::eS_Default);

	MkFloat2 barClientSize((m_Horizontal) ? m_LineLength : 0.f, (m_Horizontal) ? 0.f : m_LineLength);
	SetClientSize(barClientSize);

	// button
	MkWindowBaseNode* btnNode = NULL;
	if (ChildExist(BtnNodeName))
	{
		btnNode = dynamic_cast<MkWindowBaseNode*>(GetChildNode(BtnNodeName));
	}
	else
	{
		btnNode = MkWindowBaseNode::CreateChildNode(this, BtnNodeName);
		if (btnNode != NULL)
		{
			btnNode->SetLocalDepth(-0.1f); // slider bar와 겹치는 것을 피하기 위해 0.1f만큼 앞에 위치
			btnNode->SetFormState(MkWindowThemeFormData::eS_Normal);
			btnNode->SetMovableByDragging(true);
		}
	}

	MkWindowThemeData::eComponentType btnComponentType = MkWindowThemeData::GetLEDButtonComponent(m_WindowFrameType, MkWindowThemeData::eLED_Blue);
	if (btnNode != NULL)
	{
		btnNode->SetThemeName(GetThemeName());
		btnNode->SetComponentType(btnComponentType);
	}

	// value tag
	if (btnNode != NULL)
	{
		if (showValue)
		{
			MkWindowTagNode* tagNode = NULL;
			if (ChildExist(ValueTagName))
			{
				tagNode = dynamic_cast<MkWindowTagNode*>(GetChildNode(ValueTagName));
			}
			else
			{
				tagNode = MkWindowTagNode::CreateChildNode(btnNode, ValueTagName);
				if (tagNode != NULL)
				{
					const MkArray<MkHashStr>& textName = MK_STATIC_RES.GetWindowThemeSet().GetEditTextNode(GetThemeName(), m_WindowFrameType);
					tagNode->SetTextName(textName, MkStr(initValue)); // 즉각적인 panel 생성
					tagNode->SetVisible(false);
				}
			}

			if (tagNode != NULL)
			{
				tagNode->SetAlignmentPosition((m_Horizontal) ? eRAP_MiddleOver : eRAP_RMostCenter);
				tagNode->SetAlignmentOffset((m_Horizontal) ? MkFloat2(0.f, MKDEF_VALUE_TAG_OFFSET) : MkFloat2(MKDEF_VALUE_TAG_OFFSET, 0.f));
			}
		}
		else
		{
			btnNode->RemoveChildNode(ValueTagName);
		}
	}

	// movement lock
	MkFloat2 leftBottomPos;
	MkFloat2 rightTopPos = barClientSize;

	const MkWindowThemeFormData* barFormData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(GetThemeName(), GetComponentType(), MkHashStr::EMPTY);
	if (barFormData != NULL)
	{
		MkFloat2 offset(barFormData->GetLeftMargin(), barFormData->GetBottomMargin());
		leftBottomPos += offset;
		rightTopPos += offset;
	}

	const MkWindowThemeFormData* btnFormData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(GetThemeName(), btnComponentType, MkHashStr::EMPTY);
	if (btnFormData != NULL)
	{
		MkFloat2 offset = btnFormData->CalculateWindowSize(MkFloat2::Zero) * 0.5f;
		leftBottomPos -= offset;
		rightTopPos += offset;
	}

	m_DraggingMovementLock.position = leftBottomPos;
	m_DraggingMovementLock.size = rightTopPos - leftBottomPos;

	// value
	m_BeginValue = beginValue;
	m_ValueSize = GetMax<int>(1, valueSize);
	m_CurrentValue = Clamp<int>(initValue, m_BeginValue, m_BeginValue + m_ValueSize);

	_ApplyValueToSliderButton();
	_SendSliderMovementEvent();
}

void MkSliderControlNode::_ApplyValueToSliderButton(void)
{
	if (ChildExist(BtnNodeName))
	{
		float progress = m_LineLength * _CalculateFraction();

		MkFloat2 pos = m_DraggingMovementLock.position;
		pos.x += (m_Horizontal) ? progress : 0.f;
		pos.y += (m_Horizontal) ? 0.f : progress;
		GetChildNode(BtnNodeName)->SetLocalPosition(pos);
	}
}

void MkSliderControlNode::_SendSliderMovementEvent(void)
{
	MkDataNode arg;
	arg.CreateUnit(ArgKey_SliderPos, m_CurrentValue);
	StartNodeReportTypeEvent(ePA_SNE_SliderMoved, &arg);
}

//------------------------------------------------------------------------------------------------//