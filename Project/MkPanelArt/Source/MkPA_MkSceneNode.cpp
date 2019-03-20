
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkSceneNode.h"


const MkHashStr MkSceneNode::ArgKey_DragMovement(L"DragMovement");

const MkHashStr MkSceneNode::SystemKey_PanelExceptions(L"__#WS:PanelEx");
const MkHashStr MkSceneNode::SystemKey_NodeExceptions(L"__#WS:NodeEx");

const MkHashStr MkSceneNode::RootKey_Panels(L"[PANELS]");
const MkHashStr MkSceneNode::RootKey_Lines(L"[LINES]");
const MkHashStr MkSceneNode::RootKey_SubNodes(L"[SUBNODES]");

const MkHashStr MkSceneNode::ObjKey_Attribute(L"Attribute");


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

MkPanel* MkSceneNode::PickPanel(const MkFloat2& worldPoint, const MkBitField32& attrCondition) const
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
				if (currPanel.__CheckDrawable() && currPanel.__CheckWorldIntersection(worldPoint)) // 그려지는 panel만 대상으로 함
					return const_cast<MkPanel*>(&currPanel);
			}
		}

		// 하위 node 상대로 검사
		if (!m_ChildrenNode.Empty())
		{
			MkConstHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
			MK_STL_LOOP(looper)
			{
				MkPanel* hitPanel = looper.GetCurrentField()->PickPanel(worldPoint, attrCondition);
				if (hitPanel != NULL)
					return hitPanel;
			}
		}
	}
	return NULL;
}

MkLineShape& MkSceneNode::CreateLine(const MkHashStr& name)
{
	DeleteLine(name);
	MkLineShape& line = m_Lines.Create(name);
	line.__SetParentNode(this);
	return line;
}

void MkSceneNode::Update(double currTime)
{
	if (GetSkipUpdateWhileInvisible() && (!GetVisible()))
		return;

	// transform
	m_Transform.Update((m_ParentNodePtr == NULL) ? NULL : m_ParentNodePtr->__GetTransformPtr());

	// panels
	m_PanelAABR.Clear();
	if (!m_Panels.Empty())
	{
		MkHashMapLooper<MkHashStr, MkPanel> looper(m_Panels);
		MK_STL_LOOP(looper)
		{
			MkPanel& currObject = looper.GetCurrentField();
			currObject.__Update(__GetTransformPtr(), currTime);
			if (currObject.__CheckDrawable())
			{
				m_PanelAABR.UpdateToUnion(currObject.GetWorldAABR());
			}
		}
	}

	m_TotalAABR = m_PanelAABR;

	// lines
	m_LineAABR.Clear();
	if (!m_Lines.Empty())
	{
		MkHashMapLooper<MkHashStr, MkLineShape> looper(m_Lines);
		MK_STL_LOOP(looper)
		{
			MkLineShape& currObject = looper.GetCurrentField();
			currObject.__Update(__GetTransformPtr());
			if (currObject.__CheckDrawable())
			{
				m_LineAABR.UpdateToUnion(currObject.GetWorldAABR());
			}
		}
	}

	m_TotalAABR.UpdateToUnion(m_LineAABR);

	// children
	if (!m_ChildrenNode.Empty())
	{
		MkHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
		MK_STL_LOOP(looper)
		{
			MkSceneNode* node = looper.GetCurrentField();
			if (node->GetVisible() || (!node->GetSkipUpdateWhileInvisible()))
			{
				node->Update(currTime);

				if (node->GetVisible())
				{
					m_TotalAABR.UpdateToUnion(node->GetTotalAABR());
				}
			}
		}
	}
}

void MkSceneNode::Clear(void)
{
	m_Panels.Clear();
	m_Lines.Clear();

	m_PanelAABR.Clear();
	m_LineAABR.Clear();
	m_TotalAABR.Clear();

	MkSingleTypeTreePattern<MkSceneNode>::Clear();
}

void MkSceneNode::Load(const MkDataNode& node)
{
	// object
	LoadObject(node);

	// panels
	if (node.ChildExist(RootKey_Panels))
	{
		const MkDataNode* rootNode = node.GetChildNode(RootKey_Panels);

		MkArray<MkHashStr> keyList;
		rootNode->GetChildNodeList(keyList);
		MK_INDEXING_LOOP(keyList, i)
		{
			const MkHashStr& name = keyList[i];
			MkPanel& object = CreatePanel(name);
			object.LoadObject(*rootNode->GetChildNode(name));
		}
	}

	// lines
	if (node.ChildExist(RootKey_Lines))
	{
		const MkDataNode* rootNode = node.GetChildNode(RootKey_Lines);

		MkArray<MkHashStr> keyList;
		rootNode->GetChildNodeList(keyList);
		MK_INDEXING_LOOP(keyList, i)
		{
			const MkHashStr& name = keyList[i];
			MkLineShape& object = CreateLine(name);
			object.LoadObject(*rootNode->GetChildNode(name));
		}
	}

	// children(sub nodes)
	if (node.ChildExist(RootKey_SubNodes))
	{
		const MkDataNode* rootNode = node.GetChildNode(RootKey_SubNodes);

		MkArray<MkHashStr> keyList;
		rootNode->GetChildNodeList(keyList);
		MK_INDEXING_LOOP(keyList, i)
		{
			const MkHashStr& name = keyList[i];
			const MkDataNode* dataNode = rootNode->GetChildNode(name);

			// scene node 계열 class template을 가졌으면 ok
			MkHashStr classKey;
			if (dataNode->GetAppliedTemplateName(classKey) && IsNodeClassKey(classKey))
			{
				// 생성 및 구성
				MkSceneNode* sceneNode = MkSceneObject::CreateNodeInstance(classKey, name);
				if (sceneNode != NULL)
				{
					AttachChildNode(sceneNode);
					sceneNode->Load(*dataNode);
				}
			}
		}
	}

	// complete
	LoadComplete(node);
}

void MkSceneNode::Save(MkDataNode& node) const
{
	// class key
	const MkHashStr& nodeClassKey = GetSceneClassKey();
	MK_CHECK(!nodeClassKey.Empty(), GetNodeName().GetString() + L" node는 MkSceneObject 기능이 정의되지 않았음")
		return;

	// template 반영 전 exception list backup
	MkArray<MkHashStr> panelExceptions, nodeExceptions;
	node.GetDataEx(SystemKey_PanelExceptions, panelExceptions);
	node.GetDataEx(SystemKey_NodeExceptions, nodeExceptions);

	node.ApplyTemplate(nodeClassKey);

	// object
	SaveObject(node);

	// panels
	if (!m_Panels.Empty())
	{
		// 대상
		MkArray<MkHashStr> targets;
		m_Panels.GetKeyList(targets);

		// 예외가 존재하면 대상에서 지움
		if (!panelExceptions.Empty())
		{
			MkArray<MkHashStr> diffs;
			targets.GetDifferenceOfSets(panelExceptions, diffs); // diffs = target - exceptions
			targets = diffs;
		}

		if (!targets.Empty())
		{
			MkDataNode* rootNode = node.CreateChildNode(RootKey_Panels);
			if (rootNode != NULL)
			{
				MK_INDEXING_LOOP(targets, i)
				{
					const MkHashStr& key = targets[i];
					MkDataNode* targetDataNode = rootNode->CreateChildNode(key);
					if (targetDataNode != NULL)
					{
						const MkSceneRenderObject* targetObject = GetPanel(key);
						const MkHashStr& objClassKey = targetObject->GetSceneClassKey();
						MK_CHECK(!objClassKey.Empty(), key.GetString() + L" object node는 MkSceneObject 기능이 정의되지 않았음")
							return;

						targetDataNode->ApplyTemplate(objClassKey);
						targetObject->SaveObject(*targetDataNode);
					}
				}
			}
		}
	}

	// lines
	if (!m_Lines.Empty())
	{
		// 저장 대상일 경우만 지정
		MkArray<MkHashStr> targets(m_Lines.GetSize());
		MkConstHashMapLooper<MkHashStr, MkLineShape> looper(m_Lines);
		MK_STL_LOOP(looper)
		{
			if (looper.GetCurrentField().GetRecordable())
			{
				targets.PushBack(looper.GetCurrentKey());
			}
		}

		if (!targets.Empty())
		{
			MkDataNode* rootNode = node.CreateChildNode(RootKey_Lines);
			if (rootNode != NULL)
			{
				MK_INDEXING_LOOP(targets, i)
				{
					const MkHashStr& key = targets[i];
					MkDataNode* targetDataNode = rootNode->CreateChildNode(key);
					if (targetDataNode != NULL)
					{
						const MkSceneRenderObject* targetObject = GetLine(key);
						const MkHashStr& objClassKey = targetObject->GetSceneClassKey();
						MK_CHECK(!objClassKey.Empty(), key.GetString() + L" object node는 MkSceneObject 기능이 정의되지 않았음")
							return;

						targetDataNode->ApplyTemplate(objClassKey);
						targetObject->SaveObject(*targetDataNode);
					}
				}
			}
		}
	}

	// children(sub nodes)
	if (!m_ChildrenNode.Empty())
	{
		// 대상
		MkArray<MkHashStr> targets;
		m_ChildrenNode.GetKeyList(targets);

		// 예외가 존재하면 대상에서 지움
		if (!nodeExceptions.Empty())
		{
			MkArray<MkHashStr> diffs;
			targets.GetDifferenceOfSets(nodeExceptions, diffs); // diffs = target - exceptions
			targets = diffs;
		}

		if (!targets.Empty())
		{
			MkDataNode* rootNode = node.CreateChildNode(RootKey_SubNodes);
			if (rootNode != NULL)
			{
				MK_INDEXING_LOOP(targets, i)
				{
					const MkHashStr& key = targets[i];
					MkDataNode* targetDataNode = rootNode->CreateChildNode(key);
					if (targetDataNode != NULL)
					{
						GetChildNode(key)->Save(*targetDataNode);
					}
				}
			}
		}
	}
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkSceneNode);

void MkSceneNode::SetObjectTemplate(MkDataNode& node)
{
	// attribute
	MkBitField32 attr;
	attr.Assign(ePA_SNA_Visible, true);
	node.CreateUnit(ObjKey_Attribute, attr.m_Field);

	// transform
	MkSceneTransform::SetObjectTemplate(node);
}

void MkSceneNode::LoadObject(const MkDataNode& node)
{
	// attribute
	node.GetData(ObjKey_Attribute, m_Attribute.m_Field, 0);

	// transform
	m_Transform.Load(node);
}

void MkSceneNode::SaveObject(MkDataNode& node) const
{
	// attribute
	node.SetData(ObjKey_Attribute, m_Attribute.m_Field, 0);

	// transform
	m_Transform.Save(node);
}

MkSceneNode::MkSceneNode(const MkHashStr& name) : MkSingleTypeTreePattern<MkSceneNode>(name), MkSceneObject()
{
	m_Attribute.Clear();
	SetVisible(true);
}

//------------------------------------------------------------------------------------------------//

void MkSceneNode::__GetAllValidObjects(const MkFloatRect& cameraAABR, MkPairArray<float, const MkSceneRenderObject*>& buffer) const
{
	if (GetVisible())
	{
		if ((!m_Panels.Empty()) && m_PanelAABR.SizeIsNotZero() && m_PanelAABR.CheckIntersection(cameraAABR))
		{
			MkConstHashMapLooper<MkHashStr, MkPanel> looper(m_Panels);
			MK_STL_LOOP(looper)
			{
				const MkPanel& panel = looper.GetCurrentField();
				if (panel.__CheckDrawableAndInside(cameraAABR))
				{
					buffer.PushBack(panel.GetWorldDepth(), &panel);
				}
			}
		}

		if ((!m_Lines.Empty()) && m_LineAABR.SizeIsNotZero() && m_LineAABR.CheckIntersection(cameraAABR))
		{
			MkConstHashMapLooper<MkHashStr, MkLineShape> looper(m_Lines);
			MK_STL_LOOP(looper)
			{
				const MkLineShape& lineShape = looper.GetCurrentField();
				if (lineShape.__CheckDrawableAndInside(cameraAABR))
				{
					buffer.PushBack(lineShape.GetWorldDepth(), &lineShape);
				}
			}
		}

		if ((!m_ChildrenNode.Empty()) && m_TotalAABR.SizeIsNotZero() && m_TotalAABR.CheckIntersection(cameraAABR))
		{
			MkConstHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
			MK_STL_LOOP(looper)
			{
				looper.GetCurrentField()->__GetAllValidObjects(cameraAABR, buffer);
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

void MkSceneNode::_AddExceptionList(MkDataNode& node, const MkHashStr& expKey, const MkArray<MkHashStr>& expList)
{
	MkArray<MkHashStr> buffer;
	if (node.GetDataEx(expKey, buffer))
	{
		buffer.PushBack(expList);
		node.SetDataEx(expKey, buffer);
	}
	else
	{
		node.CreateUnitEx(expKey, expList);
	}
}

//------------------------------------------------------------------------------------------------//
