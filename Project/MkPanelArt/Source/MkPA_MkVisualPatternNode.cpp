
#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkVisualPatternNode.h"


const MkStr MkVisualPatternNode::NamePrefix(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX);

//------------------------------------------------------------------------------------------------//

void MkVisualPatternNode::SetLocalPosition(const MkFloat2& position)
{
	if (GetRestrictedWithinParentClient() && (m_ParentNodePtr != NULL) && (m_ParentNodePtr->GetNodeType() >= ePA_SNT_VisualPatternNode))
	{
		const MkVisualPatternNode* parentVisualPatternNode = dynamic_cast<const MkVisualPatternNode*>(m_ParentNodePtr);
		MkFloat2 restrictedPos = parentVisualPatternNode->GetClientRect().Confine(MkFloatRect(position, m_WindowSize));
		MkSceneNode::SetLocalPosition(restrictedPos);
	}
	else
	{
		MkSceneNode::SetLocalPosition(position);
	}
}

void MkVisualPatternNode::SetAlignmentPosition(eRectAlignmentPosition position, const MkFloat2& offset)
{
	m_AlignmentPosition = position;
	m_AlignmentOffset = offset;
}

void MkVisualPatternNode::UpdateAlignmentPosition(const MkFloatRect& targetRect)
{
	if (targetRect.SizeIsNotZero() && m_WindowSize.IsPositive())
	{
		MkFloat2 snapPos = targetRect.GetSnapPosition(m_WindowSize, m_AlignmentPosition, MkFloat2::Zero, false); // border is zero
		SetLocalPosition(snapPos + m_AlignmentOffset);
	}
}

void MkVisualPatternNode::UpdateAlignmentPosition(void)
{
	if ((m_ParentNodePtr != NULL) && (m_ParentNodePtr->GetNodeType() >= ePA_SNT_VisualPatternNode))
	{
		const MkVisualPatternNode* parentVisualPatternNode = dynamic_cast<const MkVisualPatternNode*>(m_ParentNodePtr);
		UpdateAlignmentPosition(parentVisualPatternNode->GetClientRect());
	}
}

void MkVisualPatternNode::__SendNodeEvent(const _NodeEvent& evt)
{
	switch (evt.eventType)
	{
	case static_cast<int>(eET_UpdateAlignmentPosition):
		UpdateAlignmentPosition(MkFloatRect(evt.arg2, evt.arg3));
		return;
	}
	
	MkSceneNode::__SendNodeEvent(evt);
}

MkVisualPatternNode::MkVisualPatternNode(const MkHashStr& name) : MkSceneNode(name)
{
	m_AlignmentPosition = eRAP_NonePosition;
}

//------------------------------------------------------------------------------------------------//
