
#include "MkCore_MkCheck.h"

//#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkRootWindowStyleNode.h"


//const MkHashStr MkRootWindowStyleNode::IconTagNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"IconTag";
//const MkHashStr MkRootWindowStyleNode::CaptionTagNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"CaptionTag";


//------------------------------------------------------------------------------------------------//

MkRootWindowStyleNode* MkRootWindowStyleNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkRootWindowStyleNode* node = __TSI_SceneNodeDerivedInstanceOp<MkRootWindowStyleNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkRootWindowStyleNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkRootWindowStyleNode::UpdateCursorState
(const MkInt2& position, const MkInt2& movement, bool cursorInside, eCursorState leftCS, eCursorState middleCS, eCursorState rightCS, int wheelDelta)
{
	
}

void MkRootWindowStyleNode::SendNodeCommandTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode& argument)
{
	
	
	MkWindowBaseNode::SendNodeCommandTypeEvent(eventType, argument);
}

//------------------------------------------------------------------------------------------------//

MkRootWindowStyleNode::MkRootWindowStyleNode(const MkHashStr& name) : MkWindowBaseNode(name)
{
	
}

//------------------------------------------------------------------------------------------------//