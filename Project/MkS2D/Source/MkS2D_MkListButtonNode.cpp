
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkListButtonNode.h"


const static MkHashStr TEMPLATE_NAME = MKDEF_S2D_BT_LISTBUTTON_TEMPLATE_NAME;

// MkStr
const static MkHashStr TARGET_NODE_NAME = L"TargetNodeName";


//------------------------------------------------------------------------------------------------//

void MkListButtonNode::Load(const MkDataNode& node)
{
	MkStr targetNodeName;
	node.GetData(TARGET_NODE_NAME, targetNodeName, 0);
	m_TargetNodeName = targetNodeName;

	// MkBaseWindowNode
	MkBaseWindowNode::Load(node);
}

void MkListButtonNode::Save(MkDataNode& node)
{
	// 기존 템플릿이 없으면 템플릿 적용
	_ApplyBuildingTemplateToSave(node, TEMPLATE_NAME);

	node.SetData(TARGET_NODE_NAME, m_TargetNodeName.GetString(), 0);

	// MkBaseWindowNode
	MkBaseWindowNode::Save(node);
}

bool MkListButtonNode::InputEventMousePress(unsigned int button, const MkFloat2& position, bool managedRoot)
{
	return MkBaseWindowNode::InputEventMousePress(button, position, managedRoot);
}

bool MkListButtonNode::InputEventMouseRelease(unsigned int button, const MkFloat2& position, bool managedRoot)
{
	return MkBaseWindowNode::InputEventMouseRelease(button, position, managedRoot);
}

//------------------------------------------------------------------------------------------------//

void MkListButtonNode::__GenerateBuildingTemplate(void)
{
	MkDataNode node;
	MkDataNode* tNode = node.CreateChildNode(TEMPLATE_NAME);
	MK_CHECK(tNode != NULL, TEMPLATE_NAME.GetString() + L" template node alloc 실패")
		return;

	tNode->ApplyTemplate(MKDEF_S2D_BT_BASEWINNODE_TEMPLATE_NAME); // MkBaseWindowNode의 template 적용

	tNode->CreateUnit(TARGET_NODE_NAME, MkStr::Null);

	tNode->DeclareToTemplate(true);
}

//------------------------------------------------------------------------------------------------//