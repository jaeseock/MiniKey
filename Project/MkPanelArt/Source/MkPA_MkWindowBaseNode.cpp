
#include "MkCore_MkCheck.h"

//#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkWindowBaseNode.h"


//const MkHashStr MkWindowBaseNode::IconTagNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"IconTag";
//const MkHashStr MkWindowBaseNode::CaptionTagNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"CaptionTag";


//------------------------------------------------------------------------------------------------//

MkWindowBaseNode* MkWindowBaseNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkWindowBaseNode* node = __TSI_SceneNodeDerivedInstanceOp<MkWindowBaseNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowBaseNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkWindowBaseNode::UpdateCursorState
(const MkInt2& position, const MkInt2& movement, bool cursorInside, eCursorState leftCS, eCursorState middleCS, eCursorState rightCS, int wheelDelta)
{
	if (_IsQuadForm())
	{
		if (cursorInside)
		{
			switch (leftCS)
			{
			case eCS_None:
			case eCS_Released:
				SetFormState(MkWindowThemeFormData::eS_Focus);
				break;

			case eCS_Pushing:
			case eCS_Pressed:
			case eCS_DoubleClicked:
				SetFormState(MkWindowThemeFormData::eS_Pushing);
				break;
			}
		}
		else
		{
			SetFormState(MkWindowThemeFormData::eS_Normal);
		}
	}
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
	if (_IsDualForm())
	{
		SetFormState(MkWindowThemeFormData::eS_Front);
	}
}

void MkWindowBaseNode::LostFocus(void)
{
	if (_IsDualForm())
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

		buffer.PushBack(GetNodeName());
		node = node->GetParentNode();
	}

	buffer.ReverseOrder(); // root-> leaf 순서가 되도록 뒤집음
	path = buffer;
}

void MkWindowBaseNode::SetEnable(bool enable)
{
	if (_IsQuadForm())
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

void MkWindowBaseNode::SendNodeCommandTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode& argument)
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

MkWindowBaseNode::MkWindowBaseNode(const MkHashStr& name) : MkWindowThemedNode(name)
{
	SetEnable(true);
}

//------------------------------------------------------------------------------------------------//