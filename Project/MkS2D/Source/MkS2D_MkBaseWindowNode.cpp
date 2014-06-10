
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkTexturePool.h"
#include "MkS2D_MkBaseWindowNode.h"


const static MkHashStr TEMPLATE_NAME = MKDEF_S2D_BT_BASEWINNODE_TEMPLATE_NAME;

// bool
const static MkHashStr ENABLE_KEY =  L"Enable";

// int
const static MkHashStr ALIGNMENT_TYPE_KEY = L"AlignType";

// int2
const static MkHashStr ALIGNMENT_BORDER_KEY = L"AlignBorder";

// MkStr
const static MkHashStr ALIGNMENT_TARGET_NAME_KEY = L"AlignTargetName";

// MkStr
const static MkHashStr PRESET_THEME_NAME_KEY = L"PresetThemeName";

// MkInt2
const static MkHashStr PRESET_BODY_SIZE_KEY = L"PresetBodySize";

//------------------------------------------------------------------------------------------------//

class __TSI_ImageSetToTargetNode
{
protected:
	static MkSRect* _SetSRect(const MkHashStr& name, const MkBaseTexturePtr& texture, const MkHashStr& subsetName, MkBaseWindowNode* targetNode)
	{
		MkSRect* targetRect = targetNode->ExistSRect(name) ? targetNode->GetSRect(name) : targetNode->CreateSRect(name);
		if (targetRect != NULL)
		{
			targetRect->SetTexture(texture, subsetName);
		}
		return targetRect;
	}

	static void _LineUpSRects(MkWindowTypeImageSet::eSetType setType, MkArray<MkSRect*>& srects, const MkFloat2& bodySize)
	{
		switch (setType) // bodySize를 반영한 위치 및 크기 재배치
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

	static bool _GetMatchingSRect
		(MkBaseWindowNode* targetNode, MkWindowTypeImageSet::eSetType setType, MkWindowTypeImageSet::eTypeIndex typeIndex, MkArray<MkSRect*>& srects)
	{
		const MkHashStr& key = MkWindowTypeImageSet::GetImageSetKeyword(setType, typeIndex);
		bool ok = targetNode->ExistSRect(key);
		if (ok)
		{
			srects.PushBack(targetNode->GetSRect(key));
		}
		return ok;
	}

public:
	static bool ApplyImageSetAndBodySize(const MkHashStr& imageSet, const MkFloat2& bodySize, MkBaseWindowNode* targetNode)
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
					// image set 형태(pack.type)에 맞게 처리
					const MkArray<MkHashStr>& subsetNames = pack.subsetNames;

					bool ok = false;
					switch (pack.type)
					{
					case MkWindowTypeImageSet::eSingleType:
						{
							const MkHashStr& rectName = MkWindowTypeImageSet::GetImageSetKeyword(pack.type, static_cast<MkWindowTypeImageSet::eTypeIndex>(0));
							ok = (_SetSRect(rectName, texture, subsetNames[0], targetNode) != NULL);
						}
						break;

					case MkWindowTypeImageSet::e3And1Type:
					case MkWindowTypeImageSet::e1And3Type:
					case MkWindowTypeImageSet::e3And3Type:
						{
							MkArray<MkSRect*> srects(subsetNames.GetSize());
							MK_INDEXING_LOOP(subsetNames, j)
							{
								const MkHashStr& rectName = MkWindowTypeImageSet::GetImageSetKeyword(pack.type, static_cast<MkWindowTypeImageSet::eTypeIndex>(j));
								MkSRect* srect = _SetSRect(rectName, texture, subsetNames[j], targetNode);
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
								_LineUpSRects(pack.type, srects, bodySize);
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

	static void ApplyBodySize(const MkFloat2& bodySize, MkBaseWindowNode* targetNode)
	{
		if (targetNode != NULL)
		{
			MkArray<MkSRect*> srects(9);

			if (_GetMatchingSRect(targetNode, MkWindowTypeImageSet::eSingleType, MkWindowTypeImageSet::eL, srects))
			{
				_LineUpSRects(MkWindowTypeImageSet::eSingleType, srects, bodySize);
				return;
			}
			srects.Flush();
			
			if (_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And1Type, MkWindowTypeImageSet::eL, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And1Type, MkWindowTypeImageSet::eM, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And1Type, MkWindowTypeImageSet::eR, srects))
			{
				_LineUpSRects(MkWindowTypeImageSet::e3And1Type, srects, bodySize);
				return;
			}
			srects.Flush();

			if (_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e1And3Type, MkWindowTypeImageSet::eT, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e1And3Type, MkWindowTypeImageSet::eC, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e1And3Type, MkWindowTypeImageSet::eB, srects))
			{
				_LineUpSRects(MkWindowTypeImageSet::e1And3Type, srects, bodySize);
				return;
			}
			srects.Flush();

			if (_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And3Type, MkWindowTypeImageSet::eLT, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And3Type, MkWindowTypeImageSet::eMT, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And3Type, MkWindowTypeImageSet::eRT, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And3Type, MkWindowTypeImageSet::eLC, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And3Type, MkWindowTypeImageSet::eMC, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And3Type, MkWindowTypeImageSet::eRC, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And3Type, MkWindowTypeImageSet::eLB, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And3Type, MkWindowTypeImageSet::eMB, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And3Type, MkWindowTypeImageSet::eRB, srects))
			{
				_LineUpSRects(MkWindowTypeImageSet::e3And3Type, srects, bodySize);
			}
		}
	}
};

template <class DataType>
class __TSI_ImageSetToStateNode
{
public:
	static bool ApplyImageSetAndBodySize(const MkArray<MkHashStr>& imageSets, MkBaseWindowNode* targetNode)
	{
		if (targetNode != NULL)
		{
			const MkFloat2& bodySize = targetNode->GetPresetBodySize();

			DataType beginState = static_cast<DataType>(MkWindowPresetStateInterface<DataType>::GetBegin());
			DataType endState = static_cast<DataType>(MkWindowPresetStateInterface<DataType>::GetEnd());
			DataType currState = beginState;
			while (currState < endState)
			{
				const MkHashStr& stateKeyword = MkWindowPresetStateInterface<DataType>::GetKeyword(currState);

				MkBaseWindowNode* childNode;
				if (targetNode->ChildExist(stateKeyword))
				{
					// 동일 component 노드는 네이밍 규칙에 의해 동일한 이름의 자식(state)들을 가지고 있음을 보장
					childNode = dynamic_cast<MkBaseWindowNode*>(targetNode->GetChildNode(stateKeyword));
				}
				else
				{
					childNode = new MkBaseWindowNode(stateKeyword);
					targetNode->AttachChildNode(childNode);
				}

				if (__TSI_ImageSetToTargetNode::ApplyImageSetAndBodySize(imageSets[currState], bodySize, childNode))
				{
					if (currState != beginState)
					{
						childNode->SetVisible(false);
					}
					currState = static_cast<DataType>(currState + 1);
				}
				else
					break;
			}
			return (currState == endState);
		}
		return false;
	}

	static void ApplyBodySize(MkBaseWindowNode* targetNode)
	{
		if (targetNode != NULL)
		{
			const MkFloat2& bodySize = targetNode->GetPresetBodySize();

			DataType endState = static_cast<DataType>(MkWindowPresetStateInterface<DataType>::GetEnd());
			DataType currState = static_cast<DataType>(MkWindowPresetStateInterface<DataType>::GetBegin());
			while (currState < endState)
			{
				const MkHashStr& stateKeyword = MkWindowPresetStateInterface<DataType>::GetKeyword(currState);
				if (targetNode->ChildExist(stateKeyword))
				{
					// 동일 component 노드는 네이밍 규칙에 의해 동일한 이름의 자식(state)들을 가지고 있음을 보장
					MkBaseWindowNode* childNode = dynamic_cast<MkBaseWindowNode*>(targetNode->GetChildNode(stateKeyword));
					__TSI_ImageSetToTargetNode::ApplyBodySize(bodySize, childNode);
				}
				currState = static_cast<DataType>(currState + 1);
			}
		}
	}
};

class __TSI_ImageSetToComponentNode
{
public:
	static bool ApplyImageSetAndBodySize(eS2D_WindowPresetComponent component, const MkArray<MkHashStr>& imageSets, MkBaseWindowNode* targetNode)
	{
		switch (component)
		{
		case eS2D_WPC_BackgroundWindow: return __TSI_ImageSetToStateNode<eS2D_BackgroundState>::ApplyImageSetAndBodySize(imageSets, targetNode);
		case eS2D_WPC_TitleWindow: return __TSI_ImageSetToStateNode<eS2D_TitleState>::ApplyImageSetAndBodySize(imageSets, targetNode);
		case eS2D_WPC_NegativeButton:
		case eS2D_WPC_PossitiveButton:
		case eS2D_WPC_CancelIcon: return __TSI_ImageSetToStateNode<eS2D_WindowState>::ApplyImageSetAndBodySize(imageSets, targetNode);
		}
		return false;
	}

	static void ApplyBodySize(eS2D_WindowPresetComponent component, MkBaseWindowNode* targetNode)
	{
		switch (component)
		{
		case eS2D_WPC_BackgroundWindow: __TSI_ImageSetToStateNode<eS2D_BackgroundState>::ApplyBodySize(targetNode); break;
		case eS2D_WPC_TitleWindow: __TSI_ImageSetToStateNode<eS2D_TitleState>::ApplyBodySize(targetNode); break;
		case eS2D_WPC_NegativeButton:
		case eS2D_WPC_PossitiveButton:
		case eS2D_WPC_CancelIcon: __TSI_ImageSetToStateNode<eS2D_WindowState>::ApplyBodySize(targetNode); break;
		}
	}
};

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

	// preset
	MkStr presetThemeName;
	node.GetData(PRESET_THEME_NAME_KEY, presetThemeName, 0);
	m_PresetThemeName = presetThemeName;

	MkInt2 presetBodySize;
	node.GetData(PRESET_BODY_SIZE_KEY, presetBodySize, 0);
	m_PresetBodySize = MkFloat2(static_cast<float>(presetBodySize.x), static_cast<float>(presetBodySize.y));

	// MkSceneNode
	MkSceneNode::Load(node);
}

void MkBaseWindowNode::Save(MkDataNode& node)
{
	// 기존 템플릿이 없으면 템플릿 적용
	_ApplyBuildingTemplateToSave(node, TEMPLATE_NAME);

	node.SetData(ENABLE_KEY, m_Enable, 0);
	node.SetData(ALIGNMENT_TYPE_KEY, static_cast<int>(m_AlignmentType), 0);
	node.SetData(ALIGNMENT_BORDER_KEY, MkInt2(static_cast<int>(m_AlignmentBorder.x), static_cast<int>(m_AlignmentBorder.y)), 0);
	node.SetData(ALIGNMENT_TARGET_NAME_KEY, m_TargetAlignmentWindowName.GetString(), 0);
	node.SetData(PRESET_THEME_NAME_KEY, m_PresetThemeName.GetString(), 0);
	node.SetData(PRESET_BODY_SIZE_KEY, MkInt2(static_cast<int>(m_PresetBodySize.x), static_cast<int>(m_PresetBodySize.y)), 0);

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

MkBaseWindowNode* MkBaseWindowNode::CreateWindowPreset(const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& bodySize)
{
	const MkHashStr& componentKeyword = MkWindowPreset::GetWindowPresetComponentKeyword(component);
	if (!ChildExist(componentKeyword))
	{
		const MkArray<MkHashStr>& imageSets = MK_WR_PRESET.GetWindowTypeImageSet(themeName, component);
		if (!imageSets.Empty())
		{
			// data가 모두 준비된 상태. 노드 생성
			MkBaseWindowNode* targetNode = new MkBaseWindowNode(componentKeyword);
			targetNode->__SetThemeName(themeName);
			targetNode->__SetBodySize(bodySize);

			if (targetNode != NULL)
			{
				if (__TSI_ImageSetToComponentNode::ApplyImageSetAndBodySize(component, imageSets, targetNode))
				{
					AttachChildNode(targetNode);
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

void MkBaseWindowNode::SetPresetThemeName(const MkHashStr& themeName)
{
	for (int i=eS2D_WPC_BackgroundWindow; i<eS2D_WPC_MaxWindowPresetComponent; ++i)
	{
		const MkHashStr& componentKeyword = MkWindowPreset::GetWindowPresetComponentKeyword(static_cast<eS2D_WindowPresetComponent>(i));
		if (ChildExist(componentKeyword)) // component에 해당하는 노드가 존재하면
		{
			MkBaseWindowNode* targetNode = dynamic_cast<MkBaseWindowNode*>(GetChildNode(componentKeyword));
			const MkHashStr& oldThemeName = targetNode->GetPresetThemeName();
			if ((targetNode != NULL) && (!oldThemeName.Empty()) && (themeName != oldThemeName)) // 노드가 다른 테마를 가진 MkBaseWindowNode면
			{
				eS2D_WindowPresetComponent component = MkWindowPreset::GetWindowPresetComponentEnum(componentKeyword);
				const MkArray<MkHashStr>& newImageSets = MK_WR_PRESET.GetWindowTypeImageSet(themeName, component);
				const MkArray<MkHashStr>& oldImageSets = MK_WR_PRESET.GetWindowTypeImageSet(oldThemeName, component);
				if ((!newImageSets.Empty()) && (newImageSets != oldImageSets) && // 테마명은 다르더라고 image set은 같을 수 있으므로 비교 필요
					__TSI_ImageSetToComponentNode::ApplyImageSetAndBodySize(component, newImageSets, targetNode))
				{
					targetNode->__SetThemeName(themeName);
				}
			}
		}
	}
}

void MkBaseWindowNode::SetPresetComponentBodySize(eS2D_WindowPresetComponent component, const MkFloat2& bodySize)
{
	const MkHashStr& componentKeyword = MkWindowPreset::GetWindowPresetComponentKeyword(component);
	if (ChildExist(componentKeyword)) // component에 해당하는 노드가 존재하면
	{
		MkBaseWindowNode* targetNode = dynamic_cast<MkBaseWindowNode*>(GetChildNode(componentKeyword));
		const MkHashStr& oldThemeName = targetNode->GetPresetThemeName();
		const MkFloat2& oldBodySize = targetNode->GetPresetBodySize();
		if ((targetNode != NULL) && (!oldThemeName.Empty()) && (bodySize != oldBodySize)) // 노드가 테마를 가지고 있고 크기가 다른 MkBaseWindowNode면
		{
			targetNode->__SetBodySize(bodySize);
			__TSI_ImageSetToComponentNode::ApplyBodySize(component, targetNode);
		}
	}
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
	MkDataNode* tNode = node.CreateChildNode(TEMPLATE_NAME);
	MK_CHECK(tNode != NULL, TEMPLATE_NAME.GetString() + L" template node alloc 실패")
		return;

	tNode->ApplyTemplate(MKDEF_S2D_BT_SCENENODE_TEMPLATE_NAME); // MkSceneNode의 template 적용

	tNode->CreateUnit(ENABLE_KEY, true);
	tNode->CreateUnit(ALIGNMENT_TYPE_KEY, static_cast<int>(eRAP_NonePosition));
	tNode->CreateUnit(ALIGNMENT_BORDER_KEY, MkInt2(0, 0));
	tNode->CreateUnit(ALIGNMENT_TARGET_NAME_KEY, MkStr::Null);
	tNode->CreateUnit(PRESET_THEME_NAME_KEY, MkStr::Null);
	tNode->CreateUnit(PRESET_BODY_SIZE_KEY, MkInt2(0, 0));

	tNode->DeclareToTemplate(true);
}

void MkBaseWindowNode::__UpdateWindow(const MkFloatRect& rootRegion)
{
	// world AABR의 크기가 zero라는 의미는 모든 자식 노드 포함해 의미 있는 rect가 없다는 의미
	if (m_WorldAABR.SizeIsNotZero())
	{
		// 정렬
		if (m_AlignmentType != eRAP_NonePosition)
		{
			const MkFloatRect& pivotRegion = (m_TargetAlignmentWindowNode == NULL) ? rootRegion :
				// eRAP_NonePosition이 아니므로 1(outside)이나 -1(inside)만 리턴됨
				((GetRectAlignmentPositionType(m_AlignmentType) > 0) ? m_TargetAlignmentWindowNode->GetWindowRect() : m_TargetAlignmentWindowNode->GetClientRect());

			// 배경 영역이 의미 있으면 정렬 계산
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

				// 이동 필요성이 있으면 갱신 진행
				if (newLocalPos != oldLocalPos)
				{
					SetLocalPosition(newLocalPos);

					Update(); // local position이 갱신되었으므로 자신과 하위 모든 노드들 갱신
				}
			}
		}
	}

	MkSceneNode::__UpdateWindow(rootRegion);
}

//------------------------------------------------------------------------------------------------//