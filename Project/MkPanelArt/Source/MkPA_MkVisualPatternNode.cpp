
#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkVisualPatternNode.h"


const MkStr MkVisualPatternNode::NamePrefix(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX);

//------------------------------------------------------------------------------------------------//

void MkVisualPatternNode::SetAlignmentPivotIsWindowRect(bool enable)
{
	if (enable != m_AlignmentPivotIsWindowRect)
	{
		m_AlignmentPivotIsWindowRect = enable;
		SetAlignmentCommand();
	}
}

void MkVisualPatternNode::SetAlignmentTargetIsWindowRect(bool enable)
{
	if (enable != m_AlignmentTargetIsWindowRect)
	{
		m_AlignmentTargetIsWindowRect = enable;
		SetAlignmentCommand();
	}
}

void MkVisualPatternNode::SetAlignmentPosition(eRectAlignmentPosition position)
{
	if (position != m_AlignmentPosition)
	{
		m_AlignmentPosition = position;
		SetAlignmentCommand();
	}
}

void MkVisualPatternNode::SetAlignmentOffset(const MkFloat2& offset)
{
	if (offset != m_AlignmentOffset)
	{
		m_AlignmentOffset = offset;
		SetAlignmentCommand();
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
	m_AlignmentPivotIsWindowRect = false; // client rect default
	m_AlignmentTargetIsWindowRect = true; // window rect default
	m_AlignmentPosition = eRAP_NonePosition;
}

void MkVisualPatternNode::_UpdateAlignment(void)
{
	const MkFloatRect& targetRect = (m_AlignmentTargetIsWindowRect) ? this->GetWindowRect() : this->GetClientRect();
	if (targetRect.SizeIsNotZero() && (m_ParentNodePtr != NULL) && m_ParentNodePtr->IsDerivedFrom(ePA_SNT_VisualPatternNode))
	{
		const MkVisualPatternNode* parentVPNode = dynamic_cast<const MkVisualPatternNode*>(m_ParentNodePtr);
		if (parentVPNode != NULL)
		{
			const MkFloatRect& pivotRect = (m_AlignmentPivotIsWindowRect) ? parentVPNode->GetWindowRect() : parentVPNode->GetClientRect();
			if (pivotRect.SizeIsNotZero())
			{
				MkFloat2 snapPos = pivotRect.GetSnapPosition
					(MkFloatRect(GetLocalPosition() + targetRect.position, targetRect.size), m_AlignmentPosition, MkFloat2::Zero, false); // border is zero

				SetLocalPosition(snapPos + m_AlignmentOffset - targetRect.position);
			}
		}
	}
}

void MkVisualPatternNode::_ExcuteUpdateCommand(void)
{
	if (m_UpdateCommand[eUC_Alignment])
	{
		_UpdateAlignment();
	}
}

//------------------------------------------------------------------------------------------------//
