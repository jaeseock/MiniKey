
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkPanel.h"
#include "MkPA_MkSceneNode.h"
#include "MkPA_MkVisualPatternNode.h"
#include "MkPA_MkWindowManagerNode.h"
#include "MkPA_MkWindowTagNode.h"
#include "MkPA_MkWindowThemedNode.h"
#include "MkPA_MkWindowBaseNode.h"
#include "MkPA_MkBodyFrameControlNode.h"
#include "MkPA_MkCheckBoxControlNode.h"
#include "MkPA_MkEditBoxControlNode.h"
#include "MkPA_MkListBoxControlNode.h"
#include "MkPA_MkScrollBarControlNode.h"
#include "MkPA_MkSliderControlNode.h"
#include "MkPA_MkTitleBarControlNode.h"
#include "MkPA_MkDropDownListControlNode.h"
#include "MkPA_MkScenePortalNode.h"

//------------------------------------------------------------------------------------------------//

template <class DataType>
class __TSI_ObjectOp
{
public:
	static void SetTemplate(void)
	{
		MkDataNode templateNode(DataType::SceneClassKey());
		DataType::SetObjectTemplate(templateNode);
		templateNode.DeclareToTemplate(true);
	}
};

// class hierarchy
MkTypeHierarchy<ePA_SceneNodeType> MkSceneObject::SceneNodeTypeHierarchy(ePA_SNT_SceneNode);

//------------------------------------------------------------------------------------------------//

MkSceneNode* MkSceneObject::Build(const MkDataNode& node)
{
	MkSceneNode* rootNode = NULL;
	MkHashStr classKey;
	if (node.GetAppliedTemplateName(classKey) && IsNodeClassKey(classKey))
	{
		rootNode = CreateNodeInstance(classKey, node.GetNodeName());
		if (rootNode != NULL)
		{
			rootNode->Load(node);
		}
	}
	return rootNode;
}

void MkSceneObject::InitSceneObject(void)
{
	// hierarchy
	SceneNodeTypeHierarchy.SetHierarchy(ePA_SNT_SceneNode, ePA_SNT_WindowManagerNode);
	SceneNodeTypeHierarchy.SetHierarchy(ePA_SNT_SceneNode, ePA_SNT_VisualPatternNode);
	SceneNodeTypeHierarchy.SetHierarchy(ePA_SNT_VisualPatternNode, ePA_SNT_WindowTagNode);
	SceneNodeTypeHierarchy.SetHierarchy(ePA_SNT_VisualPatternNode, ePA_SNT_WindowThemedNode);
	SceneNodeTypeHierarchy.SetHierarchy(ePA_SNT_WindowThemedNode, ePA_SNT_WindowBaseNode);
	SceneNodeTypeHierarchy.SetHierarchy(ePA_SNT_WindowBaseNode, ePA_SNT_TitleBarControlNode);
	SceneNodeTypeHierarchy.SetHierarchy(ePA_SNT_WindowBaseNode, ePA_SNT_BodyFrameControlNode);
	SceneNodeTypeHierarchy.SetHierarchy(ePA_SNT_WindowBaseNode, ePA_SNT_CheckBoxControlNode);
	SceneNodeTypeHierarchy.SetHierarchy(ePA_SNT_WindowBaseNode, ePA_SNT_ScrollBarControlNode);
	SceneNodeTypeHierarchy.SetHierarchy(ePA_SNT_WindowBaseNode, ePA_SNT_SliderControlNode);
	SceneNodeTypeHierarchy.SetHierarchy(ePA_SNT_WindowBaseNode, ePA_SNT_ScenePortalNode);
	SceneNodeTypeHierarchy.SetHierarchy(ePA_SNT_WindowBaseNode, ePA_SNT_EditBoxControlNode);
	SceneNodeTypeHierarchy.SetHierarchy(ePA_SNT_WindowBaseNode, ePA_SNT_ListBoxControlNode);
	SceneNodeTypeHierarchy.SetHierarchy(ePA_SNT_WindowBaseNode, ePA_SNT_DropDownListControlNode);

	// template
	__TSI_ObjectOp<MkPanel>::SetTemplate();
	__TSI_ObjectOp<MkSceneNode>::SetTemplate();
	__TSI_ObjectOp<MkVisualPatternNode>::SetTemplate();
	__TSI_ObjectOp<MkWindowManagerNode>::SetTemplate();
	__TSI_ObjectOp<MkWindowTagNode>::SetTemplate();
	__TSI_ObjectOp<MkWindowThemedNode>::SetTemplate();
	__TSI_ObjectOp<MkWindowBaseNode>::SetTemplate();
	__TSI_ObjectOp<MkBodyFrameControlNode>::SetTemplate();
	__TSI_ObjectOp<MkCheckBoxControlNode>::SetTemplate();
	__TSI_ObjectOp<MkEditBoxControlNode>::SetTemplate();
	__TSI_ObjectOp<MkListBoxControlNode>::SetTemplate();
	__TSI_ObjectOp<MkScrollBarControlNode>::SetTemplate();
	__TSI_ObjectOp<MkSliderControlNode>::SetTemplate();
	__TSI_ObjectOp<MkTitleBarControlNode>::SetTemplate();
	__TSI_ObjectOp<MkDropDownListControlNode>::SetTemplate();
	__TSI_ObjectOp<MkScenePortalNode>::SetTemplate();
}

bool MkSceneObject::IsNodeClassKey(const MkHashStr& classKey)
{
	return ((classKey == MkSceneNode::SceneClassKey()) ||
		(classKey == MkVisualPatternNode::SceneClassKey()) ||
		(classKey == MkWindowManagerNode::SceneClassKey()) ||
		(classKey == MkWindowTagNode::SceneClassKey()) ||
		(classKey == MkWindowThemedNode::SceneClassKey()) ||
		(classKey == MkWindowBaseNode::SceneClassKey()) ||
		(classKey == MkBodyFrameControlNode::SceneClassKey()) ||
		(classKey == MkCheckBoxControlNode::SceneClassKey()) ||
		(classKey == MkEditBoxControlNode::SceneClassKey()) ||
		(classKey == MkListBoxControlNode::SceneClassKey()) ||
		(classKey == MkScrollBarControlNode::SceneClassKey()) ||
		(classKey == MkSliderControlNode::SceneClassKey()) ||
		(classKey == MkTitleBarControlNode::SceneClassKey()) ||
		(classKey == MkDropDownListControlNode::SceneClassKey()) ||
		(classKey == MkScenePortalNode::SceneClassKey()));
}

MkSceneNode* MkSceneObject::CreateNodeInstance(const MkHashStr& classKey, const MkHashStr& name)
{
	MkSceneNode* node = NULL;

	if (classKey == MkSceneNode::SceneClassKey()) { node = new MkSceneNode(name); }
	else if (classKey == MkVisualPatternNode::SceneClassKey()) { node = new MkVisualPatternNode(name); }
	else if (classKey == MkWindowManagerNode::SceneClassKey()) { node = new MkWindowManagerNode(name); }
	else if (classKey == MkWindowTagNode::SceneClassKey()) { node = new MkWindowTagNode(name); }
	else if (classKey == MkWindowThemedNode::SceneClassKey()) { node = new MkWindowThemedNode(name); }
	else if (classKey == MkWindowBaseNode::SceneClassKey()) { node = new MkWindowBaseNode(name); }
	else if (classKey == MkBodyFrameControlNode::SceneClassKey()) { node = new MkBodyFrameControlNode(name); }
	else if (classKey == MkCheckBoxControlNode::SceneClassKey()) { node = new MkCheckBoxControlNode(name); }
	else if (classKey == MkEditBoxControlNode::SceneClassKey()) { node = new MkEditBoxControlNode(name); }
	else if (classKey == MkListBoxControlNode::SceneClassKey()) { node = new MkListBoxControlNode(name); }
	else if (classKey == MkScrollBarControlNode::SceneClassKey()) { node = new MkScrollBarControlNode(name); }
	else if (classKey == MkSliderControlNode::SceneClassKey()) { node = new MkSliderControlNode(name); }
	else if (classKey == MkTitleBarControlNode::SceneClassKey()) { node = new MkTitleBarControlNode(name); }
	else if (classKey == MkDropDownListControlNode::SceneClassKey()) { node = new MkDropDownListControlNode(name); }
	else if (classKey == MkScenePortalNode::SceneClassKey()) { node = new MkScenePortalNode(name); }

	return node;
}

//------------------------------------------------------------------------------------------------//
