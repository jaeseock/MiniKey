
#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkVisualPatternNode.h"


const MkStr MkVisualPatternNode::NamePrefix(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX);

//------------------------------------------------------------------------------------------------//

void MkVisualPatternNode::SetAlignmentPosition(eRectAlignmentPosition position)
{
	if (position != m_AlignmentPosition)
	{
		m_AlignmentPosition = position;
		m_UpdateCommand.Set(eUC_Alignment);
	}
}

void MkVisualPatternNode::SetAlignmentOffset(const MkFloat2& offset)
{
	if (offset != m_AlignmentOffset)
	{
		m_AlignmentOffset = offset;
		m_UpdateCommand.Set(eUC_Alignment);
	}
}

void MkVisualPatternNode::Update(double currTime)
{
	// update command
	if (m_UpdateCommand.Any())
	{
		_ExcuteUpdateCommand();
		m_UpdateCommand.Clear();
	}

	MkSceneNode::Update(currTime);
}

MkVisualPatternNode::MkVisualPatternNode(const MkHashStr& name) : MkSceneNode(name)
{
	m_AlignmentPosition = eRAP_NonePosition;
}


void MkVisualPatternNode::__UpdateAlignment(const MkVisualPatternNode* parentNode)
{
	if ((parentNode != NULL) && parentNode->GetClientRect().SizeIsNotZero() && m_WindowRect.SizeIsNotZero())
	{
		MkFloat2 snapPos = parentNode->GetClientRect().GetSnapPosition
			(MkFloatRect(GetLocalPosition() + m_WindowRect.position, m_WindowRect.size), m_AlignmentPosition, MkFloat2::Zero, false); // border is zero

		SetLocalPosition(snapPos + m_AlignmentOffset - m_WindowRect.position);
	}
}

void MkVisualPatternNode::__UpdateAlignment(void)
{
	if ((m_ParentNodePtr != NULL) && m_ParentNodePtr->IsDerivedFrom(ePA_SNT_VisualPatternNode))
	{
		const MkVisualPatternNode* parentNode = dynamic_cast<const MkVisualPatternNode*>(m_ParentNodePtr);
		__UpdateAlignment(parentNode);
	}
}

void MkVisualPatternNode::_ExcuteUpdateCommand(void)
{
	if (m_UpdateCommand[eUC_Alignment])
	{
		__UpdateAlignment();
	}
}

//------------------------------------------------------------------------------------------------//
