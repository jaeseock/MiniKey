
#include "MkCore_MkCheck.h"
#include "MkCore_MkAngleOp.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkTexturePool.h"
#include "MkS2D_MkSceneNode.h"
#include "MkS2D_MkSceneNodeAllocator.h"


const static MkHashStr TEMPLATE_NAME = MKDEF_S2D_BT_SCENENODE_TEMPLATE_NAME;

// MkVec3
const static MkHashStr POSITION_KEY = MKDEF_S2D_BT_SCENENODE_POSITION_KEY;

// float
const static MkHashStr ROTATION_KEY = MKDEF_S2D_BT_SCENENODE_ROTATION_KEY;

// float
const static MkHashStr SCALE_KEY = MKDEF_S2D_BT_SCENENODE_SCALE_KEY;

// float
const static MkHashStr ALPHA_KEY = MKDEF_S2D_BT_SCENENODE_ALPHA_KEY;

// bool
const static MkHashStr VISIBLE_KEY = MKDEF_S2D_BT_SCENENODE_VISIBLE_KEY;

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

MkSceneNode* MkSceneNode::CreateWindowPreset(const MkHashStr& theme, eS2D_WindowPresetComponent component, const MkFloat2& bodySize)
{
	const MkHashStr& componentKeyword = MkWindowPreset::GetWindowPresetComponentKeyword(component);
	if (!ChildExist(componentKeyword))
	{
		const MkArray<MkHashStr>& imageSets = MK_WR_PRESET.GetWindowTypeImageSet(theme, component);
		if (!imageSets.Empty())
		{
			// data가 모두 준비된 상태. 노드 생성
			MkSceneNode* targetNode = MkSceneNodeAllocator::Alloc(TEMPLATE_NAME, componentKeyword);
			if (targetNode != NULL)
			{
				bool ok = false;
				switch (component)
				{
				case eS2D_WPC_BaseWindow: ok = _AddWindowTypeImageSet(imageSets[0], bodySize, targetNode); break;
				case eS2D_WPC_WindowTitle: ok = _AddWindowTypeTitleImageSet(imageSets, bodySize, targetNode); break;
				case eS2D_WPC_NegativeButton:
				case eS2D_WPC_PossitiveButton:
				case eS2D_WPC_CancelIcon: ok = _AddWindowTypeStateImageSet(imageSets, bodySize, targetNode); break;
				}

				if (ok)
				{
					ok = AttachChildNode(targetNode);
				}
				else
				{
					delete targetNode;
					targetNode = NULL;
				}
			}
			return targetNode;
		}
	}
	
	return NULL;
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

MkSRect* MkSceneNode::CreateSRect(const MkHashStr& name, const MkBaseTexturePtr& texture, const MkHashStr& subsetName, const MkFloat2& position)
{
	MkSRect* srect = CreateSRect(name);
	if (srect != NULL)
	{
		srect->SetLocalPosition(position);
		srect->SetTexture(texture);
		if (!subsetName.Empty())
		{
			srect->SetSubset(subsetName);
		}
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
	m_Presets.Clear();

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

bool MkSceneNode::_AddWindowTypeImageSet(const MkHashStr& imageSet, const MkFloat2& bodySize, MkSceneNode* targetNode)
{
	if (targetNode != NULL)
	{
		const MkWindowTypeImageSet::Pack& pack = MK_WR_IMAGE_SET.GetPack(imageSet);
		if (pack.type != MkWindowTypeImageSet::eNull)
		{
			// bitmap texture
			MkBaseTexturePtr texture;
			MK_TEXTURE_POOL.GetBitmapTexture(pack.filePath, texture, 0);
			if (texture != NULL)
			{
				const MkArray<MkHashStr>& subsetNames = pack.subsetNames;

				bool ok = false;
				switch (pack.type)
				{
				case MkWindowTypeImageSet::eSingleType: // 하나만 생성
					{
						const MkHashStr& rectName = MkWindowTypeImageSet::GetImageSetKeyword(pack.type, static_cast<MkWindowTypeImageSet::eTypeIndex>(0));
						ok = (targetNode->CreateSRect(rectName, texture, subsetNames[0], MkFloat2(0.f, 0.f)) != NULL);
					}
					break;

				case MkWindowTypeImageSet::e3And1Type: // subset 갯수만큼 생성
				case MkWindowTypeImageSet::e1And3Type:
				case MkWindowTypeImageSet::e3And3Type:
					{
						MkArray<MkSRect*> srects(subsetNames.GetSize());
						MK_INDEXING_LOOP(subsetNames, j)
						{
							const MkHashStr& rectName = MkWindowTypeImageSet::GetImageSetKeyword(pack.type, static_cast<MkWindowTypeImageSet::eTypeIndex>(j));
							MkSRect* srect = targetNode->CreateSRect(rectName, texture, subsetNames[j], MkFloat2(0.f, 0.f));
							if (srect != NULL)
							{
								srects.PushBack(srect);
							}
							else
								break;
						}

						switch (pack.type)
						{
						case MkWindowTypeImageSet::e3And1Type:
						case MkWindowTypeImageSet::e1And3Type:
							ok = (srects.GetSize() == 3);
							break;
						case MkWindowTypeImageSet::e3And3Type:
							ok = (srects.GetSize() == 9);
							break;
						}

						if (ok)
						{
							switch (pack.type) // bodySize를 반영한 위치 재배치
							{
							case MkWindowTypeImageSet::e3And1Type:
								{
									MkSRect* lRect = srects[MkWindowTypeImageSet::eL];
									MkSRect* mRect = srects[MkWindowTypeImageSet::eM];
									MkSRect* rRect = srects[MkWindowTypeImageSet::eR];

									mRect->SetLocalPosition(MkFloat2(lRect->GetLocalSize().x, 0.f));
									mRect->SetLocalSize(MkFloat2(bodySize.x, mRect->GetLocalSize().y));
									rRect->SetLocalPosition(MkFloat2(lRect->GetLocalSize().x + bodySize.x, 0.f));
								}
								break;
							case MkWindowTypeImageSet::e1And3Type:
								{
									MkSRect* tRect = srects[MkWindowTypeImageSet::eT];
									MkSRect* cRect = srects[MkWindowTypeImageSet::eC];
									MkSRect* bRect = srects[MkWindowTypeImageSet::eB];

									cRect->SetLocalPosition(MkFloat2(0.f, bRect->GetLocalSize().y));
									cRect->SetLocalSize(MkFloat2(cRect->GetLocalSize().x, bodySize.y));
									tRect->SetLocalPosition(MkFloat2(0.f, bRect->GetLocalSize().y + bodySize.y));
								}
								break;
							case MkWindowTypeImageSet::e3And3Type:
								{
									MkSRect* ltRect = srects[MkWindowTypeImageSet::eLT];
									MkSRect* mtRect = srects[MkWindowTypeImageSet::eMT];
									MkSRect* rtRect = srects[MkWindowTypeImageSet::eRT];
									MkSRect* lcRect = srects[MkWindowTypeImageSet::eLC];
									MkSRect* mcRect = srects[MkWindowTypeImageSet::eMC];
									MkSRect* rcRect = srects[MkWindowTypeImageSet::eRC];
									MkSRect* lbRect = srects[MkWindowTypeImageSet::eLB];
									MkSRect* mbRect = srects[MkWindowTypeImageSet::eMB];
									MkSRect* rbRect = srects[MkWindowTypeImageSet::eRB];

									mbRect->SetLocalPosition(MkFloat2(lbRect->GetLocalSize().x, 0.f));
									mbRect->SetLocalSize(MkFloat2(bodySize.x, mbRect->GetLocalSize().y));
									rbRect->SetLocalPosition(MkFloat2(lbRect->GetLocalSize().x + bodySize.x, 0.f));

									lcRect->SetLocalPosition(MkFloat2(0.f, lbRect->GetLocalSize().y));
									lcRect->SetLocalSize(MkFloat2(lcRect->GetLocalSize().x, bodySize.y));
									mcRect->SetLocalPosition(MkFloat2(lcRect->GetLocalSize().x, mbRect->GetLocalSize().y));
									mcRect->SetLocalSize(bodySize);
									rcRect->SetLocalPosition(MkFloat2(lcRect->GetLocalSize().x + bodySize.x, rbRect->GetLocalSize().y));
									rcRect->SetLocalSize(MkFloat2(rcRect->GetLocalSize().x, bodySize.y));

									ltRect->SetLocalPosition(MkFloat2(0.f, lbRect->GetLocalSize().y + bodySize.y));
									mtRect->SetLocalPosition(MkFloat2(ltRect->GetLocalSize().x, mbRect->GetLocalSize().y + bodySize.y));
									mtRect->SetLocalSize(MkFloat2(bodySize.x, mtRect->GetLocalSize().y));
									rtRect->SetLocalPosition(MkFloat2(ltRect->GetLocalSize().x + bodySize.x, rbRect->GetLocalSize().y + bodySize.y));
								}
								break;
							}
						}
					}
					break;
				}
				return true;
			}
		}
	}
	return false;
}

bool MkSceneNode::_AddWindowTypeTitleImageSet(const MkArray<MkHashStr>& imageSet, const MkFloat2& bodySize, MkSceneNode* targetNode)
{
	if (targetNode != NULL)
	{
		MkSceneNode* onFocusNode = targetNode->CreateChildNode(MkWindowPreset::GetTitleStateKeyword(eS2D_TS_OnFocusState));
		MkSceneNode* lostFocusNode = targetNode->CreateChildNode(MkWindowPreset::GetTitleStateKeyword(eS2D_TS_LostFocusState));

		if (_AddWindowTypeImageSet(imageSet[eS2D_TS_OnFocusState], bodySize, onFocusNode) &&
			_AddWindowTypeImageSet(imageSet[eS2D_TS_LostFocusState], bodySize, lostFocusNode))
		{
			lostFocusNode->SetVisible(false);
			return true;
		}
	}
	return false;
}

bool MkSceneNode::_AddWindowTypeStateImageSet(const MkArray<MkHashStr>& imageSet, const MkFloat2& bodySize, MkSceneNode* targetNode)
{
	if (targetNode != NULL)
	{
		eS2D_WindowState state = eS2D_WS_DefaultState;
		while (state < eS2D_WS_MaxWindowState)
		{
			MkSceneNode* childNode = targetNode->CreateChildNode(MkWindowPreset::GetWindowStateKeyword(state));
			if (_AddWindowTypeImageSet(imageSet[state], bodySize, childNode))
			{
				if (state != eS2D_WS_DefaultState)
				{
					childNode->SetVisible(false);
				}
				state = static_cast<eS2D_WindowState>(state + 1);
			}
			else
				break;
		}
		return (state == eS2D_WS_MaxWindowState);
	}
	return false;
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
