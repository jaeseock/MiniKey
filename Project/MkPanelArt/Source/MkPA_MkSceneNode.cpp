
#include "MkCore_MkCheck.h"
//#include "MkCore_MkAngleOp.h"
//#include "MkCore_MkDataNode.h"

#include "MkPA_MkSceneNode.h"
//#include "MkS2D_MkSceneNodeFamilyDefinition.h"


//const static MkHashStr CHILD_PANEL_NODE_NAME = MKDEF_S2D_SND_CHILD_PANEL_NODE_NAME;
//const static MkHashStr CHILD_SNODE_NODE_NAME = MKDEF_S2D_SND_CHILD_SNODE_NODE_NAME;

//------------------------------------------------------------------------------------------------//
/*
void MkSceneNode::Load(const MkDataNode& node)
{
	// position
	MkVec3 position;
	node.GetData(MkSceneNodeFamilyDefinition::Scene::PositionKey, position, 0);
	SetLocalPosition(position);

	// rotation
	float rotation = 0.f;
	node.GetData(MkSceneNodeFamilyDefinition::Scene::RotationKey, rotation, 0);
	SetLocalRotation(MkAngleOp::DegreeToRadian(rotation)); // degree -> radian

	// scale
	float scale = 1.f;
	node.GetData(MkSceneNodeFamilyDefinition::Scene::ScaleKey, scale, 0);
	SetLocalScale(scale);

	// alpha
	float alpha = 1.f;
	node.GetData(MkSceneNodeFamilyDefinition::Scene::AlphaKey, alpha, 0);
	m_Alpha.SetUp(alpha); // panel���� �ٷ� ���

	// visible
	bool visible = true;
	node.GetData(MkSceneNodeFamilyDefinition::Scene::VisibleKey, visible, 0);
	SetVisible(visible);

	// Panels
	const MkDataNode* panelNode = node.GetChildNode(CHILD_PANEL_NODE_NAME);
	if (panelNode != NULL)
	{
		MkArray<MkHashStr> keys;
		panelNode->GetChildNodeList(keys);
		MK_INDEXING_LOOP(keys, i)
		{
			const MkHashStr& currName = keys[i];
			MkPanel* target = m_Panels.Exist(currName) ? &m_Panels[currName] : CreatePanel(currName);
			if (target != NULL)
			{
				target->Load(*panelNode->GetChildNode(currName));
			}
		}
	}

	// children
	const MkDataNode* childrenNode = node.GetChildNode(CHILD_SNODE_NODE_NAME);
	if (childrenNode != NULL)
	{
		MkArray<MkHashStr> keys;
		childrenNode->GetChildNodeList(keys);
		MK_INDEXING_LOOP(keys, i)
		{
			const MkHashStr& currName = keys[i];

			RemoveChildNode(currName); // �̹� �����ϴ� �ڽ� ���� ����

			const MkDataNode* targetDataNode = childrenNode->GetChildNode(currName);

			// node type�� ���� ������ �ٸ�
			MkHashStr templateName;
			if (targetDataNode->GetAppliedTemplateName(templateName))
			{
				MkSceneNode* targetInstance = MkSceneNodeFamilyDefinition::Alloc(templateName, currName);
				if (targetInstance != NULL)
				{
					if (AttachChildNode(targetInstance))
					{
						targetInstance->Load(*targetDataNode);
					}
					else
					{
						delete targetInstance;
					}
				}
			}
		}
	}
}

void MkSceneNode::Save(MkDataNode& node) // Load�� ��
{
	// ���� ���ø��� ������ ���ø� ����
	_ApplyBuildingTemplateToSave(node, MkSceneNodeFamilyDefinition::Scene::TemplateName);

	node.SetData(MkSceneNodeFamilyDefinition::Scene::PositionKey, m_LocalPosition.GetDecision(), 0);
	node.SetData(MkSceneNodeFamilyDefinition::Scene::RotationKey, MkAngleOp::RadianToDegree(m_LocalRotation.GetDecision()), 0); // radian -> degree
	node.SetData(MkSceneNodeFamilyDefinition::Scene::ScaleKey, m_LocalScale.GetDecision(), 0);
	node.SetData(MkSceneNodeFamilyDefinition::Scene::AlphaKey, m_Alpha.GetDecision(), 0);
	node.SetData(MkSceneNodeFamilyDefinition::Scene::VisibleKey, m_Visible, 0);

	if (!m_Panels.Empty())
	{
		MkDataNode* category = node.CreateChildNode(CHILD_PANEL_NODE_NAME);
		if (category != NULL)
		{
			MkHashMapLooper<MkHashStr, MkPanel> looper(m_Panels);
			MK_STL_LOOP(looper)
			{
				looper.GetCurrentField().Save(*category->CreateChildNode(looper.GetCurrentKey()));
			}
		}
	}

	if (!m_ChildrenNode.Empty())
	{
		MkDataNode* category = node.CreateChildNode(CHILD_SNODE_NODE_NAME);
		if (category != NULL)
		{
			MkHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
			MK_STL_LOOP(looper)
			{
				looper.GetCurrentField()->Save(*category->CreateChildNode(looper.GetCurrentKey()));
			}
		}
	}
}

void MkSceneNode::SetLocalAsWorldPosition(const MkFloat2& worldPosition, bool update)
{
	MkFloat2 newLocalPos = worldPosition;
	if (m_ParentNodePtr != NULL)
	{
		const MkVec3& pwp = m_ParentNodePtr->GetWorldPosition();
		newLocalPos.x -= pwp.x;
		newLocalPos.y -= pwp.y;
	}

	// �̵� �ʿ伺�� ������ ���� ����
	MkFloat2 oldLocalPos(m_LocalPosition.GetDecision().x, m_LocalPosition.GetDecision().y);
	if (newLocalPos != oldLocalPos)
	{
		SetLocalPosition(newLocalPos);

		if (update)
		{
			UpdateAll();
		}
	}
}

void MkSceneNode::RestoreDecoString(void)
{
	if (!m_Panels.Empty())
	{
		MkHashMapLooper<MkHashStr, MkPanel> looper(m_Panels);
		MK_STL_LOOP(looper)
		{
			looper.GetCurrentField().RestoreDecoString();
		}
	}

	if (!m_ChildrenNode.Empty())
	{
		MkHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
		MK_STL_LOOP(looper)
		{
			looper.GetCurrentField()->RestoreDecoString();
		}
	}
}

void MkSceneNode::AlignPosition(const MkFloatRect& anchorRect, eRectAlignmentPosition alignment, const MkInt2& border)
{
	if (anchorRect.SizeIsNotZero() && (alignment != eRAP_NonePosition))
	{
		MkFloat2 offsetToWorldPos = MkFloat2(m_WorldPosition.GetDecision().x, m_WorldPosition.GetDecision().y) - m_TotalAABR.position;
		MkFloat2 newPos = anchorRect.GetSnapPosition(m_TotalAABR, alignment, MkFloat2(static_cast<float>(border.x), static_cast<float>(border.y)));
		SetLocalAsWorldPosition(newPos + offsetToWorldPos, true);
	}
}

void MkSceneNode::AlignPosition(const MkSceneNode* anchorNode, eRectAlignmentPosition alignment, const MkInt2& border)
{
	if (anchorNode != NULL)
	{
		AlignPosition(anchorNode->GetTotalAABR(), alignment, border);
	}
}
*/

MkPanel& MkSceneNode::CreatePanel(const MkHashStr& name)
{
	DeletePanel(name);
	return m_Panels.Create(name);
}

MkPanel& MkSceneNode::CreatePanel(const MkHashStr& name, const MkSceneNode* targetNode, const MkInt2& panelSize)
{
	MkPanel& panel = CreatePanel(name);
	panel.SetPanelSize(MkFloat2(static_cast<float>(panelSize.x), static_cast<float>(panelSize.y)));
	panel.SetMaskingNode(targetNode);
	return panel;
}

void MkSceneNode::Update(double currTime)
{
	// transform
	m_Transform.Update((m_ParentNodePtr == NULL) ? NULL : m_ParentNodePtr->__GetTransformPtr());

	// panels
	m_PanelAABR.Clear();
	if (!m_Panels.Empty())
	{
		MkHashMapLooper<MkHashStr, MkPanel> looper(m_Panels);
		MK_STL_LOOP(looper)
		{
			MkPanel& currPanel = looper.GetCurrentField();
			currPanel.__Update(__GetTransformPtr(), currTime);
			if (currPanel.__CheckDrawable())
			{
				m_PanelAABR.UpdateToUnion(currPanel.GetWorldAABR());
			}
		}
	}

	m_TotalAABR = m_PanelAABR;

	// children
	if (!m_ChildrenNode.Empty())
	{
		MkHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
		MK_STL_LOOP(looper)
		{
			MkSceneNode* node = looper.GetCurrentField();
			node->Update(currTime);

			if (node->GetVisible())
			{
				m_TotalAABR.UpdateToUnion(node->GetTotalAABR());
			}
		}
	}
}

void MkSceneNode::Clear(void)
{
	m_Panels.Clear();
	m_PanelAABR.Clear();
	m_TotalAABR.Clear();

	MkSingleTypeTreePattern<MkSceneNode>::Clear();
}

MkSceneNode::MkSceneNode(const MkHashStr& name) : MkSingleTypeTreePattern<MkSceneNode>(name)
{
	m_Visible = true;
}

//------------------------------------------------------------------------------------------------//
/*
void MkSceneNode::__GenerateBuildingTemplate(void)
{
	MkDataNode node;
	MkDataNode* tNode = node.CreateChildNode(MkSceneNodeFamilyDefinition::Scene::TemplateName);
	if (tNode != NULL)
	{
		tNode->CreateUnit(MkSceneNodeFamilyDefinition::Scene::PositionKey, MkVec3::Zero);
		tNode->CreateUnit(MkSceneNodeFamilyDefinition::Scene::RotationKey, 0.f);
		tNode->CreateUnit(MkSceneNodeFamilyDefinition::Scene::ScaleKey, 1.f);
		tNode->CreateUnit(MkSceneNodeFamilyDefinition::Scene::AlphaKey, 1.f);
		tNode->CreateUnit(MkSceneNodeFamilyDefinition::Scene::VisibleKey, true);

		tNode->DeclareToTemplate(true);
	}
}
*/

void MkSceneNode::__GetAllValidPanels(const MkFloatRect& cameraAABR, MkPairArray<float, const MkPanel*>& buffer) const
{
	if (GetVisible())
	{
		if ((!m_Panels.Empty()) && m_PanelAABR.SizeIsNotZero() && m_PanelAABR.CheckIntersection(cameraAABR))
		{
			MkConstHashMapLooper<MkHashStr, MkPanel> looper(m_Panels);
			MK_STL_LOOP(looper)
			{
				const MkPanel& panel = looper.GetCurrentField();
				if (panel.__CheckDrawable(cameraAABR))
				{
					buffer.PushBack(panel.GetWorldDepth(), &panel);
				}
			}
		}

		if ((!m_ChildrenNode.Empty()) && m_TotalAABR.SizeIsNotZero() && m_TotalAABR.CheckIntersection(cameraAABR))
		{
			MkConstHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
			MK_STL_LOOP(looper)
			{
				looper.GetCurrentField()->__GetAllValidPanels(cameraAABR, buffer);
			}
		}
	}
}
/*
void MkSceneNode::_ApplyBuildingTemplateToSave(MkDataNode& node, const MkHashStr& templateName)
{
	MkHashStr appliedTemplateName;
	if (!node.GetAppliedTemplateName(appliedTemplateName))
	{
		node.Clear();
		node.ApplyTemplate(templateName);
	}
}
*/
//------------------------------------------------------------------------------------------------//