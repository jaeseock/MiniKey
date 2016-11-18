
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkTimeManager.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkWindowManagerNode.h"
#include "MkPA_MkWindowBaseNode.h"


const MkHashStr MkWindowBaseNode::ToolTipName = L"#ToolTip";

const MkHashStr MkWindowBaseNode::ArgKey_CursorLocalPosition(L"CursorLocalPosition");
const MkHashStr MkWindowBaseNode::ArgKey_CursorWorldPosition(L"CursorWorldPosition");
const MkHashStr MkWindowBaseNode::ArgKey_WheelDelta(L"WheelDelta");
const MkHashStr MkWindowBaseNode::ArgKey_ExclusiveWindow(L"ExclusiveWindow");
const MkHashStr MkWindowBaseNode::ArgKey_ExclusiveException(L"ExclusiveException");


//------------------------------------------------------------------------------------------------//

MkWindowBaseNode* MkWindowBaseNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkWindowBaseNode* node = __TSI_SceneNodeDerivedInstanceOp<MkWindowBaseNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowBaseNode ���� ����") {}
	return node;
}

MkWindowManagerNode* MkWindowBaseNode::GetWindowManagerNode(void)
{
	MkSceneNode* mgrNode = FindNearestDerivedNode(ePA_SNT_WindowManagerNode);
	return (mgrNode == NULL) ? NULL : dynamic_cast<MkWindowManagerNode*>(mgrNode);
}

//------------------------------------------------------------------------------------------------//

void MkWindowBaseNode::UpdateCursorInput
(const MkInt2& position, const MkInt2& movement, bool cursorInside, eButtonState leftBS, eButtonState middleBS, eButtonState rightBS, int wheelDelta)
{
	if (cursorInside)
	{
		if (IsQuadForm())
		{
			switch (leftBS)
			{
			case eBS_None:
			case eBS_Released:
				SetFormState(MkWindowThemeFormData::eS_Focus);
				break;

			case eBS_Pushing:
			case eBS_Pressed:
			case eBS_DoubleClicked:
				SetFormState(MkWindowThemeFormData::eS_Pushing);
				break;
			}
		}

		switch (leftBS)
		{
		case eBS_Released:
			_StartCursorReport(ePA_SNE_CursorLBtnReleased, position);
			m_HoldingEventType = ePA_SNE_None;
			break;
		case eBS_Pressed:
			_StartCursorReport(ePA_SNE_CursorLBtnPressed, position);
			_StartHoldingCheck(ePA_SNE_CursorLBtnHold);
			break;
		case eBS_DoubleClicked:
			_StartCursorReport(ePA_SNE_CursorLBtnDBClicked, position);
			_StartHoldingCheck(ePA_SNE_CursorLBtnHold);
			break;
		}

		switch (middleBS)
		{
		case eBS_Released:
			_StartCursorReport(ePA_SNE_CursorMBtnReleased, position);
			m_HoldingEventType = ePA_SNE_None;
			break;
		case eBS_Pressed:
			_StartCursorReport(ePA_SNE_CursorMBtnPressed, position);
			_StartHoldingCheck(ePA_SNE_CursorMBtnHold);
			break;
		case eBS_DoubleClicked:
			_StartCursorReport(ePA_SNE_CursorMBtnDBClicked, position);
			_StartHoldingCheck(ePA_SNE_CursorMBtnHold);
			break;
		}

		switch (rightBS)
		{
		case eBS_Released:
			_StartCursorReport(ePA_SNE_CursorRBtnReleased, position);
			m_HoldingEventType = ePA_SNE_None;
			break;
		case eBS_Pressed:
			_StartCursorReport(ePA_SNE_CursorRBtnPressed, position);
			_StartHoldingCheck(ePA_SNE_CursorRBtnHold);
			break;
		case eBS_DoubleClicked:
			_StartCursorReport(ePA_SNE_CursorRBtnDBClicked, position);
			_StartHoldingCheck(ePA_SNE_CursorRBtnHold);
			break;
		}

		if (wheelDelta != 0)
		{
			MkDataNode arg;
			arg.CreateUnit(MkWindowBaseNode::ArgKey_WheelDelta, wheelDelta);
			StartNodeReportTypeEvent(ePA_SNE_WheelMoved, &arg);
		}

		if (!m_CursorInside)
		{
			StartNodeReportTypeEvent(ePA_SNE_CursorEntered, NULL);
		}

		// holding check
		if ((m_HoldingEventType == ePA_SNE_CursorLBtnHold) || (m_HoldingEventType == ePA_SNE_CursorMBtnHold) || (m_HoldingEventType == ePA_SNE_CursorRBtnHold))
		{
			MkTimeState ts;
			MK_TIME_MGR.GetCurrentTimeState(ts);

			if (m_HoldingCounter.OnTick(ts))
			{
				_StartCursorReport(m_HoldingEventType, position);

				m_HoldingEventType = ePA_SNE_None;
			}
		}
	}
	else
	{
		if (IsQuadForm())
		{
			SetFormState(MkWindowThemeFormData::eS_Normal);
		}

		StartNodeReportTypeEvent(ePA_SNE_CursorLeft, NULL);

		m_HoldingEventType = ePA_SNE_None;
	}

	m_CursorInside = cursorInside;
}

void MkWindowBaseNode::Activate(void)
{
	SetFormState(MkWindowThemeFormData::eS_Default);
}

void MkWindowBaseNode::Deactivate(void)
{
	SetFormState(MkWindowThemeFormData::eS_Default);
}

void MkWindowBaseNode::OnFocus(void)
{
	if (IsDualForm())
	{
		SetFormState(MkWindowThemeFormData::eS_Front);
	}
}

void MkWindowBaseNode::LostFocus(void)
{
	if (IsDualForm())
	{
		SetFormState(MkWindowThemeFormData::eS_Back);
	}
}

void MkWindowBaseNode::GetWindowPath(MkArray<MkHashStr>& path) const
{
	MkArray<MkHashStr> buffer(GetDepthFromRootNode() + 1);
	const MkSceneNode* node = this;

	while (true)
	{
		if (node == NULL) // MkWindowManagerNode ��ü�� ������ ���� ���·� root ����. error
			return;

		if (node->IsDerivedFrom(ePA_SNT_WindowManagerNode)) // ���� ����
			break;

		buffer.PushBack(node->GetNodeName());
		node = node->GetParentNode();
	}

	buffer.ReverseOrder(); // root -> leaf ������ �ǵ��� ������
	path = buffer;
}

void MkWindowBaseNode::SetEnable(bool enable)
{
	if (IsQuadForm())
	{
		bool oldEnable = GetEnable();
		if (oldEnable && (!enable)) // turn off
		{
			SetFormState(MkWindowThemeFormData::eS_Disable);
		}
		else if ((!oldEnable) && enable) // turn on
		{
			SetFormState(MkWindowThemeFormData::eS_Normal);
		}
	}

	m_Attribute.Assign(ePA_SNA_Enable, enable);
}

void MkWindowBaseNode::SendNodeCommandTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode* argument)
{
	switch (eventType)
	{
	case ePA_SNE_Activate: Activate(); break;
	case ePA_SNE_Deactivate: Deactivate(); break;
	case ePA_SNE_OnFocus: OnFocus(); break;
	case ePA_SNE_LostFocus: LostFocus(); break;
	}
	
	MkWindowThemedNode::SendNodeCommandTypeEvent(eventType, argument);
}

//------------------------------------------------------------------------------------------------//

void MkWindowBaseNode::Update(double currTime)
{
	// tooltip ���� ����
	if (ChildExist(ToolTipName))
	{
		MkSceneNode* tooltipNode = GetChildNode(ToolTipName);
		if (tooltipNode != NULL)
		{
			tooltipNode->SetVisible(m_CursorInside);
		}
	}

	MkWindowThemedNode::Update(currTime);
}

void MkWindowBaseNode::Clear(void)
{
	m_CursorInside = false;
	m_HoldingEventType = ePA_SNE_None;
	m_CallBackTargetWindowPath.Clear();

	MkWindowThemedNode::Clear();
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkWindowBaseNode);

void MkWindowBaseNode::SetObjectTemplate(MkDataNode& node)
{
	MkWindowThemedNode::SetObjectTemplate(node);

	// update attribute
	MkBitField32 attr;
	node.GetData(ObjKey_Attribute, attr.m_Field, 0);
	attr.Assign(ePA_SNA_AcceptInput, true);
	attr.Assign(ePA_SNA_Enable, true);
	node.SetData(ObjKey_Attribute, attr.m_Field, 0);
}

void MkWindowBaseNode::LoadObject(const MkDataNode& node)
{
	MkWindowThemedNode::LoadObject(node);
}

void MkWindowBaseNode::SaveObject(MkDataNode& node) const
{
	MkWindowThemedNode::SaveObject(node);
}

MkWindowBaseNode::MkWindowBaseNode(const MkHashStr& name) : MkWindowThemedNode(name)
{
	SetAcceptInput(true);
	SetEnable(true);

	m_CursorInside = false;
	m_HoldingEventType = ePA_SNE_None;
}

void MkWindowBaseNode::_StartCursorReport(ePA_SceneNodeEvent evt, const MkInt2& position)
{
	// report
	MkFloat2 worldPos(static_cast<float>(position.x), static_cast<float>(position.y));
	MkFloat2 localPos = worldPos - GetWorldPosition();

	MkDataNode arg;
	arg.CreateUnit(MkWindowBaseNode::ArgKey_CursorLocalPosition, MkVec2(localPos.x, localPos.y));
	arg.CreateUnit(MkWindowBaseNode::ArgKey_CursorWorldPosition, MkVec2(worldPos.x, worldPos.y));
	StartNodeReportTypeEvent(evt, &arg);

	// call back
	if (!m_CallBackTargetWindowPath.Empty())
	{
		do
		{
			MkSceneNode* mgrNode = FindNearestDerivedNode(ePA_SNT_WindowManagerNode); // �Ҽ� window mananger
			if (mgrNode == NULL)
				break;

			MkSceneNode* targetNode = mgrNode->GetChildNode(m_CallBackTargetWindowPath); // call back�� ȣ�� �� node
			if ((targetNode == NULL) || (!targetNode->IsDerivedFrom(ePA_SNT_WindowBaseNode)))
				break;

			MkWindowBaseNode* winBase = dynamic_cast<MkWindowBaseNode*>(targetNode);
			if (winBase == NULL)
				break;

			MkArray<MkHashStr> myPath;
			GetWindowPath(myPath);
			winBase->CallBackOperation(evt, &arg, myPath);
			return;
		}
		while (false);

		m_CallBackTargetWindowPath.Clear(); // ���� ���. �ʱ�ȭ
	}
}

void MkWindowBaseNode::_StartHoldingCheck(ePA_SceneNodeEvent evt)
{
	m_HoldingEventType = evt; // ePA_SNE_Cursor(L/M/R)BtnHold

	MkTimeState ts;
	MK_TIME_MGR.GetCurrentTimeState(ts);
	m_HoldingCounter.SetUp(ts, MKDEF_PA_HOLD_EVENT_TIME_CONDITION);
}

//------------------------------------------------------------------------------------------------//