
#include "MkCore_MkCheck.h"
#include "MkCore_MkAngleOp.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkSceneNode.h"
#include "MkS2D_MkSceneNodeAllocator.h"


const static MkHashStr TEMPLATE_NAME = MKDEF_S2D_BT_SCENENODE_TEMPLATE_NAME;

// MkVec3
const static MkHashStr POSITION_KEY = L"Position";

// float
const static MkHashStr ROTATION_KEY = L"Rotation";

// float
const static MkHashStr SCALE_KEY = L"Scale";

// float
const static MkHashStr ALPHA_KEY = L"Alpha";

// bool
const static MkHashStr VISIBLE_KEY = L"Visible";

//------------------------------------------------------------------------------------------------//

const static MkHashStr CHILD_SRECT_NODE_NAME = MKDEF_S2D_SND_CHILD_SRECT_NODE_NAME;
const static MkHashStr CHILD_SNODE_NODE_NAME = MKDEF_S2D_SND_CHILD_SNODE_NODE_NAME;

//------------------------------------------------------------------------------------------------//

void MkSceneNode::Load(const MkDataNode& node)
{
	// position
	MkVec3 position;
	node.GetData(POSITION_KEY, position, 0);
	SetLocalPosition(position);

	// rotation
	float rotation = 0.f;
	node.GetData(ROTATION_KEY, rotation, 0);
	SetLocalRotation(MkAngleOp::DegreeToRadian(rotation)); // degree -> radian

	// scale
	float scale = 1.f;
	node.GetData(SCALE_KEY, scale, 0);
	SetLocalScale(scale);

	// alpha
	float alpha = 1.f;
	node.GetData(ALPHA_KEY, alpha, 0);
	m_Alpha.SetUp(alpha); // SRect에서 바로 사용

	// visible
	bool visible = true;
	node.GetData(VISIBLE_KEY, visible, 0);
	SetVisible(visible);

	// SRects
	const MkDataNode* srectNode = node.GetChildNode(CHILD_SRECT_NODE_NAME);
	if (srectNode != NULL)
	{
		MkArray<MkHashStr> keys;
		srectNode->GetChildNodeList(keys);
		MK_INDEXING_LOOP(keys, i)
		{
			const MkHashStr& currName = keys[i];
			MkSRect* target = m_SRects.Exist(currName) ? &m_SRects[currName] : CreateSRect(currName);
			if (target != NULL)
			{
				target->Load(*srectNode->GetChildNode(currName));
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

			// 이미 존재하는 자식 노드면 삭제
			if (m_ChildrenNode.Exist(currName))
			{
				MkSceneNode* removeTarget = m_ChildrenNode[currName];
				DetachChildNode(currName);
				delete removeTarget;
			}

			const MkDataNode* targetDataNode = childrenNode->GetChildNode(currName);

			// node type에 따라 생성이 다름
			MkHashStr templateName;
			if (targetDataNode->GetAppliedTemplateName(templateName))
			{
				MkSceneNode* targetInstance = MkSceneNodeAllocator::Alloc(templateName, currName);
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
	_ApplyBuildingTemplateToSave(node, TEMPLATE_NAME);

	node.SetData(POSITION_KEY, m_LocalPosition.GetDecision(), 0);
	node.SetData(ROTATION_KEY, MkAngleOp::RadianToDegree(m_LocalRotation.GetDecision()), 0); // radian -> degree
	node.SetData(SCALE_KEY, m_LocalScale.GetDecision(), 0);
	node.SetData(ALPHA_KEY, m_Alpha.GetDecision(), 0);
	node.SetData(VISIBLE_KEY, m_Visible, 0);

	if (!m_SRects.Empty())
	{
		MkDataNode* category = node.CreateChildNode(CHILD_SRECT_NODE_NAME);
		if (category != NULL)
		{
			MkHashMapLooper<MkHashStr, MkSRect> looper(m_SRects);
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

void MkSceneNode::SetLocalRotation(float rotation)
{
	m_LocalRotation = MkAngleOp::Unitize2PI(rotation);
}

void MkSceneNode::SetAlpha(float alpha)
{
	alpha = Clamp<float>(alpha, 0.f, 1.f);
	if (m_Alpha.Commit(alpha))
	{
		if (!m_SRects.Empty())
		{
			MkHashMapLooper<MkHashStr, MkSRect> looper(m_SRects);
			MK_STL_LOOP(looper)
			{
				looper.GetCurrentField().SetObjectAlpha(alpha);
			}
		}

		if (!m_ChildrenNode.Empty())
		{
			MkHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
			MK_STL_LOOP(looper)
			{
				looper.GetCurrentField()->SetAlpha(alpha);
			}
		}
	}
}

MkSRect* MkSceneNode::CreateSRect(const MkHashStr& name)
{
	MK_CHECK(!m_SRects.Exist(name), m_NodeName.GetString() + L" SceneNode에 이미 " + name.GetString() + L" 이름을 가진 SRect가 존재")
		return NULL;

	MkSRect* srect = &m_SRects.Create(name);
	if (srect != NULL)
	{
		srect->SetObjectAlpha(m_Alpha.GetDecision());
	}
	return srect;
}

MkSRect* MkSceneNode::GetSRect(const MkHashStr& name)
{
	MK_CHECK(m_SRects.Exist(name), m_NodeName.GetString() + L" SceneNode에 존재하지 않는 " + name.GetString() + L" SRect 참조 시도")
		return NULL;

	return &m_SRects[name];
}

bool MkSceneNode::DeleteSRect(const MkHashStr& name)
{
	MK_CHECK(m_SRects.Exist(name), m_NodeName.GetString() + L" SceneNode에 존재하지 않는 " + name.GetString() + L" SRect 삭제 시도")
		return false;

	m_SRects.Erase(name);
	return true;
}

void MkSceneNode::DeleteAllSRects(void)
{
	m_SRects.Clear();
}

void MkSceneNode::Update(void)
{
	__UpdateTransform();
	__UpdateWorldAABR();
}

void MkSceneNode::Update(const MkFloatRect& rootRegion)
{
	MkSceneNode::Update();

	__UpdateWindow(rootRegion);
}

void MkSceneNode::Clear(void)
{
	m_SRects.Clear();

	MkSingleTypeTreePattern<MkSceneNode>::Clear();
}

MkSceneNode::MkSceneNode(const MkHashStr& name) : MkSingleTypeTreePattern<MkSceneNode>(name)
{
	m_LocalPosition.SetUp();
	m_LocalRotation.SetUp(0.f);
	m_LocalScale.SetUp(1.f);

	m_WorldPosition.SetUp();
	m_WorldRotation.SetUp(0.f);
	m_WorldScale.SetUp(1.f);
	m_WorldPosition.Commit();
	m_WorldRotation.Commit();
	m_WorldScale.Commit();

	m_Alpha.SetUp(1.f);

	m_Visible = true;
}

//------------------------------------------------------------------------------------------------//

void MkSceneNode::__GenerateBuildingTemplate(void)
{
	MkDataNode node;
	MkDataNode* tNode = node.CreateChildNode(TEMPLATE_NAME);
	MK_CHECK(tNode != NULL, TEMPLATE_NAME.GetString() + L" template node alloc 실패")
		return;

	tNode->CreateUnit(POSITION_KEY, MkVec3::Zero);
	tNode->CreateUnit(ROTATION_KEY, 0.f);
	tNode->CreateUnit(SCALE_KEY, 1.f);
	tNode->CreateUnit(ALPHA_KEY, 1.f);
	tNode->CreateUnit(VISIBLE_KEY, true);

	tNode->DeclareToTemplate(true);
}

void MkSceneNode::__UpdateTransform(void)
{
	// node transform
	if (m_ParentNodePtr == NULL)
	{
		_UpdateNodeTransform();
	}
	else
	{
		_UpdateNodeTransform(m_ParentNodePtr->__GetWorldPosition(), m_ParentNodePtr->__GetWorldRotation(), m_ParentNodePtr->__GetWorldScale());
	}

	// child rect transform
	if (m_SRects.Empty())
	{
		m_NodeOnlyAABR.Clear();
	}
	else
	{
		// update rect
		float worldRotation = m_WorldRotation.GetDecision();
		float cosR = (worldRotation == 0.f) ? 1.f : MkAngleOp::Cos(worldRotation);
		float sinR = (worldRotation == 0.f) ? 0.f : MkAngleOp::Sin(worldRotation);

		MkHashMapLooper<MkHashStr, MkSRect> looper(m_SRects);
		MK_STL_LOOP(looper)
		{
			looper.GetCurrentField().__UpdateTransform(m_WorldPosition.GetDecision(), worldRotation, cosR, sinR, m_WorldScale.GetDecision());
		}

		// update local AABR
		looper.SetUp(m_SRects);

		m_NodeOnlyAABR = looper.GetCurrentField().GetAABR(); // init

		looper.__MoveToNext();
		MK_STL_LOOP(looper)
		{
			m_NodeOnlyAABR.UpdateToUnion(looper.GetCurrentField().GetAABR());
		}
	}

	// all child nodes
	if (!m_ChildrenNode.Empty())
	{
		MkHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
		MK_STL_LOOP(looper)
		{
			looper.GetCurrentField()->__UpdateTransform();
		}
	}
}

void MkSceneNode::__UpdateWorldAABR(void)
{
	m_WorldAABR = m_NodeOnlyAABR;

	if (!m_ChildrenNode.Empty())
	{
		MkHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
		MK_STL_LOOP(looper)
		{
			looper.GetCurrentField()->__UpdateWorldAABR();

			m_WorldAABR.UpdateToUnion(looper.GetCurrentField()->GetWorldAABR());
		}
	}
}

void MkSceneNode::__GetAllValidSRects(const MkFloatRect& cameraAABR, MkPairArray<float, const MkSRect*>& buffer) const
{
	if (GetVisible())
	{
		if ((!m_SRects.Empty()) && m_NodeOnlyAABR.SizeIsNotZero() && m_NodeOnlyAABR.CheckIntersection(cameraAABR))
		{
			MkConstHashMapLooper<MkHashStr, MkSRect> looper(m_SRects);
			MK_STL_LOOP(looper)
			{
				const MkSRect& rect = looper.GetCurrentField();
				if (rect.CheckValidation(cameraAABR))
				{
					buffer.PushBack(rect.GetWorldDepth(), &rect);
				}
			}
		}

		if ((!m_ChildrenNode.Empty()) && m_WorldAABR.SizeIsNotZero() && m_WorldAABR.CheckIntersection(cameraAABR))
		{
			MkConstHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
			MK_STL_LOOP(looper)
			{
				looper.GetCurrentField()->__GetAllValidSRects(cameraAABR, buffer);
			}
		}
	}
}

void MkSceneNode::__UpdateWindow(const MkFloatRect& rootRegion)
{
	// world AABR의 크기가 zero라는 의미는 모든 자식 노드 포함해 의미 있는 rect가 없다는 의미
	if (m_WorldAABR.SizeIsNotZero() && (!m_ChildrenNode.Empty()))
	{
		MkHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
		MK_STL_LOOP(looper)
		{
			looper.GetCurrentField()->__UpdateWindow(rootRegion);
		}
	}
}

void MkSceneNode::_ApplyBuildingTemplateToSave(MkDataNode& node, const MkHashStr& templateName)
{
	MkHashStr appliedTemplateName;
	if (!node.GetAppliedTemplateName(appliedTemplateName))
	{
		node.Clear();
		node.ApplyTemplate(templateName);
	}
}

void MkSceneNode::_UpdateNodeTransform
(const MkValueDecision<MkVec3>& worldPosition, const MkValueDecision<float>& worldRotation, const MkValueDecision<float>& worldScale)
{
	m_LocalPosition.Commit();
	m_LocalRotation.Commit();
	m_LocalScale.Commit();

	// position : LP * PR * PS + PP
	// depth : LP + PP
	if (worldPosition.Changed() || worldRotation.Changed() || worldScale.Changed() || m_LocalPosition.Changed())
	{
		const MkVec3& wp = worldPosition.GetDecision(); // PP
		const float& wr = worldRotation.GetDecision(); // PR
		const float& ws = worldScale.GetDecision(); // PS
		const MkVec3& lp = m_LocalPosition.GetDecision(); // LP

		float cosR = MkAngleOp::Cos(wr);
		float sinR = MkAngleOp::Sin(wr);

		float nx = (cosR * lp.x + sinR * lp.y) * ws + wp.x; // CW rotation
		float ny = (-sinR * lp.x + cosR * lp.y) * ws + wp.y;
		float nz = lp.z + wp.z;

		m_WorldPosition.Commit(MkVec3(nx, ny, nz));
	}

	// rotation : LR + PR
	if (worldRotation.Changed() || m_LocalRotation.Changed())
	{
		m_WorldRotation.Commit(MkAngleOp::Unitize2PI(worldRotation.GetDecision() + m_LocalRotation.GetDecision()));
	}

	// scale :LS * PS
	if (worldScale.Changed() || m_LocalScale.Changed())
	{
		m_WorldScale.Commit(worldScale.GetDecision() * m_LocalScale.GetDecision());
	}
}

void MkSceneNode::_UpdateNodeTransform(void)
{
	m_LocalPosition.Commit();
	m_LocalRotation.Commit();
	m_LocalScale.Commit();

	if (m_LocalPosition.Changed())
	{
		m_WorldPosition.Commit(m_LocalPosition.GetDecision());
	}

	if (m_LocalRotation.Changed())
	{
		m_WorldRotation.Commit(m_LocalRotation.GetDecision());
	}

	if (m_LocalScale.Changed())
	{
		m_WorldScale.Commit(m_LocalScale.GetDecision());
	}
}

//------------------------------------------------------------------------------------------------//
