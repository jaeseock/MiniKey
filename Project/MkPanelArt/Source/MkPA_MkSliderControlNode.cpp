
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowTagNode.h"
#include "MkPA_MkSliderControlNode.h"


const MkHashStr MkSliderControlNode::BtnNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Btn";
const MkHashStr MkSliderControlNode::ValueTagName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"VTag";

const MkHashStr MkSliderControlNode::ArgKey_SliderPos = L"SliderPos";

const MkHashStr MkSliderControlNode::ObjKey_LineLength = L"LineLength";
const MkHashStr MkSliderControlNode::ObjKey_Horizontal = L"Horizontal";
const MkHashStr MkSliderControlNode::ObjKey_BeginValue = L"BeginValue";
const MkHashStr MkSliderControlNode::ObjKey_ValueSize = L"ValueSize";
const MkHashStr MkSliderControlNode::ObjKey_CurrentValue = L"CurrentValue";
const MkHashStr MkSliderControlNode::ObjKey_ShowValue = L"ShowValue";

// button�� value tag�� �Ÿ�
#define MKDEF_VALUE_TAG_OFFSET 1.f

// value tag�� ������ �������� ���� �� ������� �ð� ����(sec)
#define MKDEF_VALUE_TAG_HIDING_TERM 0.3

//------------------------------------------------------------------------------------------------//

MkSliderControlNode* MkSliderControlNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkSliderControlNode* node = __TSI_SceneNodeDerivedInstanceOp<MkSliderControlNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkSliderControlNode ���� ����") {}
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

		// ���� ������� ������
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
	// drag movement�̰� �ش� window�� slider button�̸� value ���� �� event �Ҹ�
	// ���� value�� ����Ǿ��ٸ� ePA_SNE_SliderMoved event �߻���Ŵ
	if ((eventType == ePA_SNE_DragMovement) && (path.GetSize() == 1) && (path[0] == BtnNodeName))
	{
		MkSceneNode* node = GetChildNode(BtnNodeName);
		float currProgress = (m_Horizontal) ?
			(node->GetLocalPosition().x - m_DraggingMovementLock.position.x) : (node->GetLocalPosition().y - m_DraggingMovementLock.position.y);

		currProgress += 0.001f; // floating ����� ���� ����

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

	// drag begin�̰� �ش� window�� slider button�̸� button drag flag Ŵ
	if ((eventType == ePA_SNE_BeginDrag) && (path.GetSize() == 1) && (path[0] == BtnNodeName))
	{
		m_ButtonDragging = true;
	}

	// drag end�� �ش� window�� slider button�̸� button drag flag ���� ����
	if ((eventType == ePA_SNE_EndDrag) && (path.GetSize() == 1) && (path[0] == BtnNodeName))
	{
		m_ButtonDragging = false;
		_ApplyValueToSliderButton();
	}

	// wheel�� ���������� value ����, event �Ҹ�
	if ((eventType == ePA_SNE_WheelMoved) && (argument != NULL))
	{
		int delta;
		if (argument->GetData(MkWindowBaseNode::ArgKey_WheelDelta, delta, 0))
		{
			SetSliderValue(m_CurrentValue + delta);
			return;
		}
	}

	// cursor�� slide bar, button�� �������� cursor focusing flag Ŵ
	if (eventType == ePA_SNE_CursorEntered)
	{
		m_CursorFocusing = true;
	}

	// cursor�� slide bar, button���� �������� cursor focusing flag ��
	if (eventType == ePA_SNE_CursorLeft)
	{
		m_CursorFocusing = false;
	}

	// value ������ �߻��߰� value tag�� �����ϸ� �ݿ�
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
	// value tag ���� ����
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

void MkSliderControlNode::Save(MkDataNode& node) const
{
	// btn/value ����
	static MkArray<MkHashStr> exceptions;
	if (exceptions.Empty())
	{
		exceptions.PushBack(BtnNodeName); // ValueTagName�� ����
	}
	_AddExceptionList(node, SystemKey_NodeExceptions, exceptions);

	// run
	MkWindowBaseNode::Save(node);
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkSliderControlNode);

void MkSliderControlNode::SetObjectTemplate(MkDataNode& node)
{
	MkWindowBaseNode::SetObjectTemplate(node);

	node.CreateUnit(ObjKey_LineLength, 0.f);
	node.CreateUnit(ObjKey_Horizontal, false);
	node.CreateUnit(ObjKey_BeginValue, static_cast<int>(0));
	node.CreateUnit(ObjKey_ValueSize, static_cast<int>(0));
	node.CreateUnit(ObjKey_CurrentValue, static_cast<int>(0));
	node.CreateUnit(ObjKey_ShowValue, false);
}

void MkSliderControlNode::LoadObject(const MkDataNode& node)
{
	MkWindowBaseNode::LoadObject(node);

	float lineLength;
	bool horizontal;
	int beginValue;
	int valueSize;
	int currentValue;
	bool showValue;
	if (node.GetData(ObjKey_LineLength, lineLength, 0) &&
		node.GetData(ObjKey_Horizontal, horizontal, 0) &&
		node.GetData(ObjKey_BeginValue, beginValue, 0) &&
		node.GetData(ObjKey_ValueSize, valueSize, 0) &&
		node.GetData(ObjKey_CurrentValue, currentValue, 0) &&
		node.GetData(ObjKey_ShowValue, showValue, 0))
	{
		_SetSlider(GetThemeName(), m_FrameType, lineLength, beginValue, valueSize, currentValue, showValue, horizontal);
	}
}

void MkSliderControlNode::SaveObject(MkDataNode& node) const
{
	MkWindowBaseNode::SaveObject(node);

	node.SetData(ObjKey_LineLength, m_LineLength, 0);
	node.SetData(ObjKey_Horizontal, m_Horizontal, 0);
	node.SetData(ObjKey_BeginValue, m_BeginValue, 0);
	node.SetData(ObjKey_ValueSize, m_ValueSize, 0);
	node.SetData(ObjKey_CurrentValue, m_CurrentValue, 0);
	node.SetData(ObjKey_ShowValue, ChildExist(BtnNodeName) && GetChildNode(BtnNodeName)->ChildExist(ValueTagName), 0);
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

	m_FrameType = frameType;
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
			btnNode->SetLocalDepth(-0.1f); // slider bar�� ��ġ�� ���� ���ϱ� ���� 0.1f��ŭ �տ� ��ġ
			btnNode->SetFormState(MkWindowThemeFormData::eS_Normal);
			btnNode->SetMovableByDragging(true);
		}
	}

	MkWindowThemeData::eComponentType btnComponentType = MkWindowThemeData::GetLEDButtonComponent(m_FrameType, MkWindowThemeData::eLED_Blue);
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
					const MkArray<MkHashStr>& textName = MK_STATIC_RES.GetWindowThemeSet().GetEditTextNode(GetThemeName(), m_FrameType);
					tagNode->SetTextName(textName, MkStr(initValue)); // �ﰢ���� panel ����
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