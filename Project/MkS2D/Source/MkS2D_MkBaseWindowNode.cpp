
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkBaseWindowNode.h"


// bool
const static MkHashStr ENABLE_KEY = MKDEF_S2D_BT_BASEWINNODE_ENABLE_KEY;

// int
const static MkHashStr ALIGNMENT_TYPE_KEY = MKDEF_S2D_BT_BASEWINNODE_ALIGNMENT_TYPE_KEY;

// int2
const static MkHashStr ALIGNMENT_BORDER_KEY = MKDEF_S2D_BT_BASEWINNODE_ALIGNMENT_BORDER_KEY;

// MkStr
const static MkHashStr ALIGNMENT_TARGET_NAME_KEY = MKDEF_S2D_BT_BASEWINNODE_ALIGNMENT_TARGET_NAME_KEY;


//------------------------------------------------------------------------------------------------//

void MkBaseWindowNode::Load(const MkDataNode& node)
{
	// enable
	bool enable = true;
	node.GetData(ENABLE_KEY, enable, 0);
	m_Enable = enable; // not SetEnable()

	// alignment
	int alignmentType = 0; // eRAP_NonePosition
	node.GetData(ALIGNMENT_TYPE_KEY, alignmentType, 0);

	MkInt2 alignmentBorder;
	node.GetData(ALIGNMENT_BORDER_KEY, alignmentBorder, 0);

	MkStr alignTargetName;
	node.GetData(ALIGNMENT_TARGET_NAME_KEY, alignTargetName, 0);

	SetAlignment(alignTargetName, static_cast<eRectAlignmentPosition>(alignmentType), alignmentBorder);

	// MkSceneNode
	MkSceneNode::Load(node);
}

void MkBaseWindowNode::Save(MkDataNode& node)
{
	// ���� ���ø��� ������ ���ø� ����
	_ApplyBuildingTemplateToSave(node, MKDEF_S2D_BT_BASEWINNODE_TEMPLATE_NAME);

	node.SetData(ENABLE_KEY, m_Enable, 0);
	node.SetData(ALIGNMENT_TYPE_KEY, static_cast<int>(m_AlignmentType), 0);
	node.SetData(ALIGNMENT_BORDER_KEY, MkInt2(static_cast<int>(m_AlignmentBorder.x), static_cast<int>(m_AlignmentBorder.y)), 0);
	node.SetData(ALIGNMENT_TARGET_NAME_KEY, m_TargetAlignmentWindowName.GetString(), 0);

	// MkSceneNode
	MkSceneNode::Save(node);
}

bool MkBaseWindowNode::SetAlignment(const MkHashStr& pivotWinNodeName, eRectAlignmentPosition alignment, const MkInt2& border)
{
	m_AlignmentType = alignment;
	m_TargetAlignmentWindowName.Clear();
	m_AlignmentBorder.Clear();
	m_TargetAlignmentWindowNode = NULL;
	
	if (!pivotWinNodeName.Empty())
	{
		MkBaseWindowNode* target = dynamic_cast<MkBaseWindowNode*>(GetAncestorNode(pivotWinNodeName));
		if (target != NULL)
		{
			m_TargetAlignmentWindowName = pivotWinNodeName;
			m_AlignmentBorder = MkFloat2(static_cast<float>(border.x), static_cast<float>(border.y));
			m_TargetAlignmentWindowNode = target;
			return true;
		}
	}
	return false;
}

MkBaseWindowNode::MkBaseWindowNode(const MkHashStr& name) : MkSceneNode(name)
{
	m_Enable = true;
	m_AlignmentType = eRAP_NonePosition;
	m_TargetAlignmentWindowNode = NULL;
}

//------------------------------------------------------------------------------------------------//

void MkBaseWindowNode::__GenerateBuildingTemplate(void)
{
	MkDataNode node;
	MkDataNode* tNode = node.CreateChildNode(MKDEF_S2D_BT_BASEWINNODE_TEMPLATE_NAME);
	MK_CHECK(tNode != NULL, MkStr(MKDEF_S2D_BT_BASEWINNODE_TEMPLATE_NAME) + L" template node alloc ����")
		return;

	tNode->ApplyTemplate(MKDEF_S2D_BT_SCENENODE_TEMPLATE_NAME); // MkSceneNode�� template ����

	tNode->CreateUnit(ENABLE_KEY, true);
	tNode->CreateUnit(ALIGNMENT_TYPE_KEY, static_cast<int>(eRAP_NonePosition));
	tNode->CreateUnit(ALIGNMENT_BORDER_KEY, MkInt2(0, 0));
	tNode->CreateUnit(ALIGNMENT_TARGET_NAME_KEY, MkStr::Null);

	tNode->DeclareToTemplate(true);
}

void MkBaseWindowNode::__UpdateWindow(const MkFloatRect& rootRegion)
{
	// world AABR�� ũ�Ⱑ zero��� �ǹ̴� ��� �ڽ� ��� ������ �ǹ� �ִ� rect�� ���ٴ� �ǹ�
	if (m_WorldAABR.SizeIsNotZero())
	{
		// ����
		if (m_AlignmentType != eRAP_NonePosition)
		{
			const MkFloatRect& pivotRegion = (m_TargetAlignmentWindowNode == NULL) ? rootRegion :
				// eRAP_NonePosition�� �ƴϹǷ� 1(outside)�̳� -1(inside)�� ���ϵ�
				((GetRectAlignmentPositionType(m_AlignmentType) > 0) ? m_TargetAlignmentWindowNode->GetWindowRect() : m_TargetAlignmentWindowNode->GetClientRect());

			// ��� ������ �ǹ� ������ ���� ���
			if (pivotRegion.SizeIsNotZero())
			{
				MkFloat2 oldLocalPos(m_LocalPosition.GetDecision().x, m_LocalPosition.GetDecision().y);

				MkFloat2 validToWorld = MkFloat2(m_WorldPosition.GetDecision().x, m_WorldPosition.GetDecision().y) - m_WorldAABR.position;
				MkFloat2 alignedPos = pivotRegion.GetSnapPosition(m_WorldAABR, m_AlignmentType, m_AlignmentBorder);
				MkFloat2 newLocalPos = alignedPos + validToWorld;

				if (m_ParentNodePtr != NULL)
				{
					const MkVec3& pwp = m_ParentNodePtr->GetWorldPosition();
					newLocalPos -= MkFloat2(pwp.x, pwp.y);
				}

				// �̵� �ʿ伺�� ������ ���� ����
				if (newLocalPos != oldLocalPos)
				{
					SetLocalPosition(newLocalPos);

					Update(); // local position�� ���ŵǾ����Ƿ� �ڽŰ� ���� ��� ���� ����
				}
			}
		}
	}

	MkSceneNode::__UpdateWindow(rootRegion);
}

//------------------------------------------------------------------------------------------------//