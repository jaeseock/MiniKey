
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkStateControlNode.h"


// bool
const static MkHashStr ENABLE_KEY = MKDEF_S2D_BT_BASEWINNODE_ENABLE_KEY;

// int
const static MkHashStr ALIGNMENT_TYPE_KEY = MKDEF_S2D_BT_BASEWINNODE_ALIGNMENT_TYPE_KEY;

// int2
const static MkHashStr ALIGNMENT_BORDER_KEY = MKDEF_S2D_BT_BASEWINNODE_ALIGNMENT_BORDER_KEY;

// MkStr
const static MkHashStr ALIGNMENT_TARGET_NAME_KEY = MKDEF_S2D_BT_BASEWINNODE_ALIGNMENT_TARGET_NAME_KEY;


//------------------------------------------------------------------------------------------------//

void MkStateControlNode::Load(const MkDataNode& node)
{
	
}

void MkStateControlNode::Save(MkDataNode& node)
{
	
}

void MkStateControlNode::SetEnable(bool enable)
{
	if (m_Enable && (!enable)) // on -> off
	{
		SetWindowState(eS2D_WS_DisableState);
	}
	else if ((!m_Enable) && enable) // off -> on
	{
		SetWindowState(eS2D_WS_DefaultState);
	}

	m_Enable = enable;
}

void MkStateControlNode::SetWindowState(eS2D_WindowState windowState)
{
	m_WindowState = windowState;
}

MkStateControlNode::MkStateControlNode(const MkHashStr& name) : MkBaseWindowNode(name)
{
	m_WindowState = eS2D_WS_DefaultState;
}

//------------------------------------------------------------------------------------------------//

void MkStateControlNode::__GenerateBuildingTemplate(void)
{
	/*
	MkDataNode node;
	MkDataNode* tNode = node.CreateChildNode(MKDEF_S2D_BT_BASEWINNODE_TEMPLATE_NAME);
	MK_CHECK(tNode != NULL, MkStr(MKDEF_S2D_BT_BASEWINNODE_TEMPLATE_NAME) + L" template node alloc 실패")
		return;

	tNode->ApplyTemplate(MKDEF_S2D_BT_SCENENODE_TEMPLATE_NAME); // MkSceneNode의 template 적용

	tNode->CreateUnit(ENABLE_KEY, true);
	tNode->CreateUnit(ALIGNMENT_TYPE_KEY, static_cast<int>(eRAP_NonePosition));
	tNode->CreateUnit(ALIGNMENT_BORDER_KEY, MkInt2(0, 0));
	tNode->CreateUnit(ALIGNMENT_TARGET_NAME_KEY, MkStr::Null);

	tNode->DeclareToTemplate(true);
	*/
}

//------------------------------------------------------------------------------------------------//