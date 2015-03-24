
#include "MkCore_MkCheck.h"
//#include "MkCore_MkAngleOp.h"
//#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkSceneNode.h"
//#include "MkS2D_MkSceneNodeFamilyDefinition.h"

// class hierarchy
MkTypeHierarchy<ePA_SceneNodeType> MkSceneNode::SceneNodeTypeHierarchy(ePA_SNT_SceneNode);



//const static MkHashStr CHILD_PANEL_NODE_NAME = MKDEF_S2D_SND_CHILD_PANEL_NODE_NAME;
//const static MkHashStr CHILD_SNODE_NODE_NAME = MKDEF_S2D_SND_CHILD_SNODE_NODE_NAME;

//------------------------------------------------------------------------------------------------//

bool MkSceneNode::IsDerivedFrom(ePA_SceneNodeType nodeType) const
{
	return SceneNodeTypeHierarchy.IsDerivedFrom(nodeType, GetNodeType());
}

bool MkSceneNode::IsDerivedFrom(const MkSceneNode* instance) const
{
	return (instance == NULL) ? false : SceneNodeTypeHierarchy.IsDerivedFrom(instance->GetNodeType(), GetNodeType());
}

MkSceneNode* MkSceneNode::FindNearestDerivedNode(ePA_SceneNodeType nodeType)
{
	MkSceneNode* node = this;
	while (node != NULL)
	{
		if (node->IsDerivedFrom(nodeType))
			return node;

		node = node->GetParentNode();
	}
	return NULL;
}
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
	m_Alpha.SetUp(alpha); // panel에서 바로 사용

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

			RemoveChildNode(currName); // 이미 존재하는 자식 노드면 삭제

			const MkDataNode* targetDataNode = childrenNode->GetChildNode(currName);

			// node type에 따라 생성이 다름
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

void MkSceneNode::Save(MkDataNode& node) // Load의 역
{
	// 기존 템플릿이 없으면 템플릿 적용
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

	// 이동 필요성이 있으면 갱신 진행
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
*/

MkPanel& MkSceneNode::CreatePanel(const MkHashStr& name)
{
	DeletePanel(name);
	MkPanel& panel = m_Panels.Create(name);
	panel.__SetParentNode(this);
	return panel;
}

MkPanel& MkSceneNode::CreatePanel(const MkHashStr& name, const MkSceneNode* targetNode, const MkInt2& panelSize)
{
	MkPanel& panel = CreatePanel(name);
	panel.__SetParentNode(this);
	panel.SetPanelSize(MkFloat2(static_cast<float>(panelSize.x), static_cast<float>(panelSize.y)));
	panel.SetMaskingNode(targetNode);
	return panel;
}

bool MkSceneNode::PickPanel
(MkArray<MkPanel*>& buffer, const MkFloat2& worldPoint, float startDepth, const MkBitField32& attrCondition) const
{
	if (GetVisible() && m_TotalAABR.CheckIntersection(worldPoint))
	{
		// attribute를 만족하면 직계 panel 상대로 검사
		if (m_Attribute.CheckInclusion(attrCondition) && m_PanelAABR.CheckIntersection(worldPoint))
		{
			MkConstHashMapLooper<MkHashStr, MkPanel> looper(m_Panels);
			MK_STL_LOOP(looper)
			{
				const MkPanel& currPanel = looper.GetCurrentField();
				if (currPanel.__CheckDrawable()) // 그려지는 panel만 대상으로 함
				{
					float panelDepth = currPanel.GetWorldDepth();
					float depthLimit = buffer.Empty() ? MKDEF_PA_MAX_WORLD_DEPTH : buffer[0]->GetWorldDepth();
					if ((panelDepth >= startDepth) && (panelDepth <= depthLimit) && currPanel.__CheckWorldIntersection(worldPoint))
					{
						// 새 최근거리 발견하면 새로 시작, 아니면(panelDepth == depthLimit) 기존 최근거리에 추가만 함
						if (panelDepth < depthLimit)
						{
							buffer.Flush();
						}
						
						buffer.PushBack(const_cast<MkPanel*>(&currPanel));
					}
				}
			}
		}

		// 하위 node 상대로 검사
		if (!m_ChildrenNode.Empty())
		{
			MkConstHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
			MK_STL_LOOP(looper)
			{
				looper.GetCurrentField()->PickPanel(buffer, worldPoint, startDepth, attrCondition);
			}
		}
	}

	return !buffer.Empty();
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
	m_Attribute.Clear();
	SetVisible(true);
}

//------------------------------------------------------------------------------------------------//

void MkSceneNode::__BuildSceneNodeTypeHierarchy(void)
{
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
}
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

void MkSceneNode::SendNodeCommandTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode* argument)
{
	if (!m_ChildrenNode.Empty())
	{
		MkHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
		MK_STL_LOOP(looper)
		{
			looper.GetCurrentField()->SendNodeCommandTypeEvent(eventType, argument);
		}
	}
}

void MkSceneNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	if (m_ParentNodePtr != NULL)
	{
		path.Insert(0, GetNodeName()); // MkDeque::PushFront()가 어울리지만 비용이 크지 않을거라 예상되므로 그냥 MkArray::Insert() 사용
		m_ParentNodePtr->SendNodeReportTypeEvent(eventType, path, argument);
	}
}

void MkSceneNode::StartNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode* argument)
{
	MkArray<MkHashStr> path;
	SendNodeReportTypeEvent(eventType, path, argument);
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
