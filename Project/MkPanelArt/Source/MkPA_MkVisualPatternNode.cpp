
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkVisualPatternNode.h"


const MkHashStr MkVisualPatternNode::ObjKey_AlignPosition(L"AlignPosition");
const MkHashStr MkVisualPatternNode::ObjKey_AlignOffset(L"AlignOffset");

//------------------------------------------------------------------------------------------------//

MkVisualPatternNode* MkVisualPatternNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkVisualPatternNode* node = __TSI_SceneNodeDerivedInstanceOp<MkVisualPatternNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkVisualPatternNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkVisualPatternNode::SetAlignmentPivotIsWindowRect(bool enable)
{
	if (enable != GetAlignmentPivotIsWindowRect())
	{
		m_Attribute.Assign(ePA_SNA_AlignmentPivotIsWindowRect, enable);
		SetAlignmentCommand();
	}
}

void MkVisualPatternNode::SetAlignmentTargetIsWindowRect(bool enable)
{
	if (enable != GetAlignmentTargetIsWindowRect())
	{
		m_Attribute.Assign(ePA_SNA_AlignmentTargetIsWindowRect, enable);
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

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkVisualPatternNode);

void MkVisualPatternNode::SetObjectTemplate(MkDataNode& node)
{
	MkSceneNode::SetObjectTemplate(node);

	// update attribute
	MkBitField32 attr;
	node.GetData(ObjKey_Attribute, attr.m_Field, 0);
	attr.Assign(ePA_SNA_AlignmentTargetIsWindowRect, true);
	node.SetData(ObjKey_Attribute, attr.m_Field, 0);

	// alignment
	node.CreateUnit(ObjKey_AlignPosition, static_cast<int>(eRAP_NonePosition));
	node.CreateUnitEx(ObjKey_AlignOffset, MkFloat2::Zero);
}

void MkVisualPatternNode::LoadObject(const MkDataNode& node)
{
	MkSceneNode::LoadObject(node);

	// alignment
	int alignPos;
	node.GetData(ObjKey_AlignPosition, alignPos, 0);
	SetAlignmentPosition(static_cast<eRectAlignmentPosition>(alignPos));

	MkFloat2 alignOffset;
	node.GetDataEx(ObjKey_AlignOffset, alignOffset, 0);
	SetAlignmentOffset(alignOffset);
}

void MkVisualPatternNode::SaveObject(MkDataNode& node) const
{
	MkSceneNode::SaveObject(node);

	// alignment
	node.SetData(ObjKey_AlignPosition, static_cast<int>(m_AlignmentPosition), 0);
	node.SetDataEx(ObjKey_AlignOffset, m_AlignmentOffset, 0);
}

MkVisualPatternNode::MkVisualPatternNode(const MkHashStr& name) : MkSceneNode(name)
{
	m_Attribute.Assign(ePA_SNA_AlignmentPivotIsWindowRect, false); // client rect default
	m_Attribute.Assign(ePA_SNA_AlignmentTargetIsWindowRect, true); // window rect default
	m_AlignmentPosition = eRAP_NonePosition;
}

void MkVisualPatternNode::_UpdateAlignment(void)
{
	const MkFloatRect& targetRect = GetAlignmentTargetIsWindowRect() ? this->GetWindowRect() : this->GetClientRect();
	if (targetRect.SizeIsNotZero() && (m_ParentNodePtr != NULL) && m_ParentNodePtr->IsDerivedFrom(ePA_SNT_VisualPatternNode))
	{
		const MkVisualPatternNode* parentVPNode = dynamic_cast<const MkVisualPatternNode*>(m_ParentNodePtr);
		if (parentVPNode != NULL)
		{
			const MkFloatRect& pivotRect = GetAlignmentPivotIsWindowRect() ? parentVPNode->GetWindowRect() : parentVPNode->GetClientRect();
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
