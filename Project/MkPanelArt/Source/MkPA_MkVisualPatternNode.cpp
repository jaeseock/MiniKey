
#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkVisualPatternNode.h"


const MkStr MkVisualPatternNode::NamePrefix(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX);

//------------------------------------------------------------------------------------------------//

void MkVisualPatternNode::SnapTo(const MkFloatRect& targetRect, eRectAlignmentPosition alignmentPosition)
{
	if (targetRect.SizeIsNotZero() && m_WindowSize.IsPositive())
	{
		MkFloat2 snapPos = targetRect.GetSnapPosition(m_WindowSize, alignmentPosition, MkFloat2::Zero, false); // border is zero
		SetLocalPosition(snapPos);
	}
}

void MkVisualPatternNode::SnapToParentClientRect(eRectAlignmentPosition alignmentPosition)
{
	if ((m_ParentNodePtr != NULL) && (m_ParentNodePtr->GetNodeType() >= ePA_SNT_VisualPatternNode))
	{
		const MkVisualPatternNode* parentVisualPatternNode = dynamic_cast<const MkVisualPatternNode*>(m_ParentNodePtr);
		SnapTo(parentVisualPatternNode->GetClientRect(), alignmentPosition);
	}
}

//------------------------------------------------------------------------------------------------//
