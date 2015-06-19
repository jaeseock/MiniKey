
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkWindowBaseNode.h"


const MkHashStr MkWindowBaseNode::ArgKey_CursorLocalPosition(L"CursorLocalPosition");
const MkHashStr MkWindowBaseNode::ArgKey_WheelDelta(L"WheelDelta");

const MkHashStr MkWindowBaseNode::ObjKey_WindowFrameType(L"WinFrameType");


//------------------------------------------------------------------------------------------------//

MkWindowBaseNode* MkWindowBaseNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkWindowBaseNode* node = __TSI_SceneNodeDerivedInstanceOp<MkWindowBaseNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowBaseNode 생성 실패") {}
	return node;
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
		case eBS_Released: _StartCursorReport(ePA_SNE_CursorLBtnReleased, position); break;
		case eBS_Pressed: _StartCursorReport(ePA_SNE_CursorLBtnPressed, position); break;
		case eBS_DoubleClicked: _StartCursorReport(ePA_SNE_CursorLBtnDBClicked, position); break;
		}

		switch (middleBS)
		{
		case eBS_Released: _StartCursorReport(ePA_SNE_CursorMBtnReleased, position); break;
		case eBS_Pressed: _StartCursorReport(ePA_SNE_CursorMBtnPressed, position); break;
		case eBS_DoubleClicked: _StartCursorReport(ePA_SNE_CursorMBtnDBClicked, position); break;
		}

		switch (rightBS)
		{
		case eBS_Released: _StartCursorReport(ePA_SNE_CursorRBtnReleased, position); break;
		case eBS_Pressed: _StartCursorReport(ePA_SNE_CursorRBtnPressed, position); break;
		case eBS_DoubleClicked: _StartCursorReport(ePA_SNE_CursorRBtnDBClicked, position); break;
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
	}
	else
	{
		if (IsQuadForm())
		{
			SetFormState(MkWindowThemeFormData::eS_Normal);
		}

		if (m_CursorInside)
		{
			StartNodeReportTypeEvent(ePA_SNE_CursorLeft, NULL);
		}
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
		if (node == NULL) // MkWindowManagerNode 객체를 만나지 못한 상태로 root 도달. error
			return;

		if (node->IsDerivedFrom(ePA_SNT_WindowManagerNode)) // 정상 종료
			break;

		buffer.PushBack(node->GetNodeName());
		node = node->GetParentNode();
	}

	buffer.ReverseOrder(); // root -> leaf 순서가 되도록 뒤집음
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

void MkWindowBaseNode::Clear(void)
{
	m_WindowFrameType = MkWindowThemeData::eFT_None;
	m_CursorInside = false;

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

	// frame type
	node.CreateUnit(ObjKey_WindowFrameType, MkStr::EMPTY);
}

void MkWindowBaseNode::LoadObject(const MkDataNode& node)
{
	MkWindowThemedNode::LoadObject(node);

	// frame type
	MkHashStr frameTypeName;
	if (node.GetDataEx(ObjKey_WindowFrameType, frameTypeName, 0) && (!frameTypeName.Empty()))
	{
		m_WindowFrameType = MkWindowThemeData::ConvertFrameNameToType(frameTypeName);
	}
}

void MkWindowBaseNode::SaveObject(MkDataNode& node) const
{
	MkWindowThemedNode::SaveObject(node);

	// frame type
	if ((m_WindowFrameType > MkWindowThemeData::eFT_None) && (m_WindowFrameType < MkWindowThemeData::eFT_Max))
	{
		node.SetDataEx(ObjKey_WindowFrameType, MkWindowThemeData::FrameTypeName[m_WindowFrameType], 0);
	}
}

MkWindowBaseNode::MkWindowBaseNode(const MkHashStr& name) : MkWindowThemedNode(name)
{
	SetAcceptInput(true);
	SetEnable(true);

	m_WindowFrameType = MkWindowThemeData::eFT_None;
	m_CursorInside = false;
}

void MkWindowBaseNode::_StartCursorReport(ePA_SceneNodeEvent evt, const MkInt2& position)
{
	MkFloat2 worldPos(static_cast<float>(position.x), static_cast<float>(position.y));
	MkFloat2 localPos = worldPos - GetWorldPosition();

	MkDataNode arg;
	arg.CreateUnit(MkWindowBaseNode::ArgKey_CursorLocalPosition, MkVec2(localPos.x, localPos.y));
	StartNodeReportTypeEvent(evt, &arg);
}

//------------------------------------------------------------------------------------------------//