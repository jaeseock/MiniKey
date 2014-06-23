
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkTexturePool.h"
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkBaseWindowNode.h"

#include "MkS2D_MkWindowEventManager.h"


const static MkHashStr TEMPLATE_NAME = MKDEF_S2D_BT_BASEWINNODE_TEMPLATE_NAME;

// bool
const static MkHashStr ENABLE_KEY =  L"Enable";

// MkStr
const static MkHashStr PRESET_THEME_NAME_KEY = L"PresetThemeName";

// MkStr
const static MkHashStr PRESET_COMPONENT_KEY = L"PresetComponent";

// MkInt2
const static MkHashStr PRESET_BODY_SIZE_KEY = L"PresetBodySize";

// MkInt2
const static MkHashStr PRESET_FULL_SIZE_KEY = L"PresetFullSize";

// unsigned int
const static MkHashStr ATTRIBUTE_KEY = L"Attribute";

//------------------------------------------------------------------------------------------------//

const static MkHashStr COMPONENT_HIGHLIGHT_TAG_NAME = L"HighlightTag";
const static MkHashStr COMPONENT_NORMAL_TAG_NAME = L"NormalTag";

#define MKDEF_BASE_WINDOW_DEPTH_GRID 0.001f
#define MKDEF_TITLE_BAR_SAMPLE_STRING L"타 이 틀"
#define MKDEF_OK_BTN_SAMPLE_STRING L"확 인"
#define MKDEF_CANCEL_BTN_SAMPLE_STRING L"취 소"

//------------------------------------------------------------------------------------------------//

class __TSI_ImageSetToTargetNode
{
protected:
	static MkSRect* _SetSRect(const MkHashStr& name, const MkBaseTexturePtr& texture, const MkHashStr& subsetName, MkSceneNode* targetNode)
	{
		MkSRect* targetRect = targetNode->CreateSRect(name);
		if (targetRect != NULL)
		{
			targetRect->SetTexture(texture, subsetName);
		}
		return targetRect;
	}

	static void _LineUpSRects(MkWindowTypeImageSet::eSetType setType, MkArray<MkSRect*>& srects, const MkFloat2& bodySize, MkFloat2& fullSize)
	{
		switch (setType) // bodySize를 반영한 위치 및 크기 재배치
		{
		case MkWindowTypeImageSet::eSingleType:
			fullSize = srects[MkWindowTypeImageSet::eL]->GetLocalSize();
			break;
		case MkWindowTypeImageSet::e3And1Type:
			{
				MkSRect* lRect = srects[MkWindowTypeImageSet::eL];
				MkSRect* mRect = srects[MkWindowTypeImageSet::eM];
				MkSRect* rRect = srects[MkWindowTypeImageSet::eR];

				mRect->SetLocalPosition(MkFloat2(lRect->GetLocalSize().x, 0.f));
				mRect->SetLocalSize(MkFloat2(bodySize.x, mRect->GetLocalSize().y));
				rRect->SetLocalPosition(MkFloat2(lRect->GetLocalSize().x + bodySize.x, 0.f));

				fullSize.x = lRect->GetLocalSize().x + bodySize.x + rRect->GetLocalSize().x;
				fullSize.y = lRect->GetLocalSize().y;
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

				fullSize.x = tRect->GetLocalSize().y;
				fullSize.y = tRect->GetLocalSize().y + bodySize.y + bRect->GetLocalSize().y;
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

				fullSize.x = ltRect->GetLocalSize().x + bodySize.x + rtRect->GetLocalSize().x;
				fullSize.y = ltRect->GetLocalSize().y + bodySize.y + lbRect->GetLocalSize().y;
			}
			break;
		}
	}

	static bool _GetMatchingSRect
		(MkSceneNode* targetNode, MkWindowTypeImageSet::eSetType setType, MkWindowTypeImageSet::eTypeIndex typeIndex, MkArray<MkSRect*>& srects)
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
	static bool ApplyImageSetAndBodySize(const MkHashStr& imageSet, const MkFloat2& bodySize, MkSceneNode* targetNode, MkFloat2& fullSize)
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
					// 기존 SRect 삭제
					targetNode->DeleteAllSRects();

					// image set 형태(pack.type)에 맞게 처리
					const MkArray<MkHashStr>& subsetNames = pack.subsetNames;

					bool ok = false;
					MkArray<MkSRect*> srects(subsetNames.GetSize());
					switch (pack.type)
					{
					case MkWindowTypeImageSet::eSingleType:
						{
							const MkHashStr& rectName = MkWindowTypeImageSet::GetImageSetKeyword(pack.type, static_cast<MkWindowTypeImageSet::eTypeIndex>(0));
							MkSRect* srect = _SetSRect(rectName, texture, subsetNames[0], targetNode);
							if (srect != NULL)
							{
								srects.PushBack(srect);
								ok = true;
							}
						}
						break;

					case MkWindowTypeImageSet::e3And1Type:
					case MkWindowTypeImageSet::e1And3Type:
					case MkWindowTypeImageSet::e3And3Type:
						{
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
						}
						break;
					}

					if (ok)
					{
						_LineUpSRects(pack.type, srects, bodySize, fullSize);
					}

					return true;
				}
			}
		}
		return false;
	}

	static bool ApplyFreeImage(const MkPathName& imagePath, const MkHashStr& subsetName, MkSceneNode* targetNode, MkFloat2& fullSize)
	{
		if (targetNode != NULL)
		{
			MkBaseTexturePtr texture;
			MK_TEXTURE_POOL.GetBitmapTexture(imagePath, texture, 0);
			if (texture != NULL)
			{
				// 기존 SRect 삭제
				targetNode->DeleteAllSRects();

				// MkWindowTypeImageSet::eSingleType으로 처리
				const MkHashStr& rectName = MkWindowTypeImageSet::GetImageSetKeyword(MkWindowTypeImageSet::eSingleType, static_cast<MkWindowTypeImageSet::eTypeIndex>(0));
				MkSRect* srect = _SetSRect(rectName, texture, subsetName, targetNode);
				if (srect != NULL)
				{
					fullSize = srect->GetLocalSize();
					return true;
				}
			}
		}
		return false;
	}

	static void ApplyBodySize(const MkFloat2& bodySize, MkSceneNode* targetNode, MkFloat2& fullSize)
	{
		if (targetNode != NULL)
		{
			MkArray<MkSRect*> srects(9);

			if (_GetMatchingSRect(targetNode, MkWindowTypeImageSet::eSingleType, MkWindowTypeImageSet::eL, srects))
			{
				_LineUpSRects(MkWindowTypeImageSet::eSingleType, srects, bodySize, fullSize);
				return;
			}
			srects.Flush();
			
			if (_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And1Type, MkWindowTypeImageSet::eL, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And1Type, MkWindowTypeImageSet::eM, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And1Type, MkWindowTypeImageSet::eR, srects))
			{
				_LineUpSRects(MkWindowTypeImageSet::e3And1Type, srects, bodySize, fullSize);
				return;
			}
			srects.Flush();

			if (_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e1And3Type, MkWindowTypeImageSet::eT, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e1And3Type, MkWindowTypeImageSet::eC, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e1And3Type, MkWindowTypeImageSet::eB, srects))
			{
				_LineUpSRects(MkWindowTypeImageSet::e1And3Type, srects, bodySize, fullSize);
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
				_LineUpSRects(MkWindowTypeImageSet::e3And3Type, srects, bodySize, fullSize);
			}
		}
	}
};

template <class DataType>
class __TSI_ImageSetToStateNode
{
protected:
	static void _AlignTagRect(MkBaseWindowNode* targetNode, MkSRect* stateRect, eRectAlignmentPosition alignment, const MkFloat2& border)
	{
		MkFloatRect anchorRect(MkFloat2(0.f, 0.f), targetNode->GetPresetFullSize());
		MkFloat2 position = anchorRect.GetSnapPosition(stateRect->GetLocalRect(), alignment, border);
		stateRect->SetLocalPosition(position);
		stateRect->SetLocalDepth(-0.001f); // 살짝 앞으로 나오게
	}

	static void _SetState(DataType state, MkSceneNode* targetNode)
	{
		if (targetNode != NULL)
		{
			const MkHashStr& destKey = MkWindowPresetStateInterface<DataType>::GetKeyword(state);
			MkSceneNode* destNode = targetNode->GetChildNode(destKey);
			if (destNode != NULL)
			{
				if (!destNode->GetVisible()) // 이미 활성화된 state node가 아니면 노드를 순회하며 visible/invisible 지정
				{
					const MkArray<MkHashStr>& keyList = MkWindowPresetStateInterface<DataType>::GetKeywordList();
					MK_INDEXING_LOOP(keyList, i)
					{
						const MkHashStr& currKey = keyList[i];
						MkSceneNode* stateNode = targetNode->GetChildNode(currKey);
						if (stateNode != NULL)
						{
							stateNode->SetVisible(currKey == destKey);
						}
					}
				}
			}
		}
	}

	static void _SetHighlight(DataType state, MkSceneNode* targetNode)
	{
		if (targetNode != NULL)
		{
			bool onHighlight = (state ==  static_cast<DataType>(MkWindowPresetStateInterface<DataType>::GetHighlight()));
			if (targetNode->ExistSRect(COMPONENT_HIGHLIGHT_TAG_NAME))
			{
				targetNode->GetSRect(COMPONENT_HIGHLIGHT_TAG_NAME)->SetVisible(onHighlight);
			}
			if (targetNode->ExistSRect(COMPONENT_NORMAL_TAG_NAME))
			{
				targetNode->GetSRect(COMPONENT_NORMAL_TAG_NAME)->SetVisible(!onHighlight);
			}
		}
	}

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

				// 동일 component 노드는 네이밍 규칙에 의해 동일한 이름의 자식(state)들을 가지고 있음을 보장
				MkSceneNode* stateNode = targetNode->ChildExist(stateKeyword) ? targetNode->GetChildNode(stateKeyword) : targetNode->CreateChildNode(stateKeyword);

				MkFloat2 rectSize;
				if (__TSI_ImageSetToTargetNode::ApplyImageSetAndBodySize(imageSets[currState], bodySize, stateNode, rectSize))
				{
					bool defaultState = (currState == beginState);
					stateNode->SetVisible(defaultState);
					if (defaultState)
					{
						targetNode->__SetFullSize(rectSize);
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

	static bool ApplyFreeImageToState(DataType state, const MkPathName& imagePath, const MkHashStr& subsetName, MkBaseWindowNode* targetNode)
	{
		if (targetNode != NULL)
		{
			const MkHashStr& stateKeyword = MkWindowPresetStateInterface<DataType>::GetKeyword(state);
			MkSceneNode* stateNode = targetNode->ChildExist(stateKeyword) ? targetNode->GetChildNode(stateKeyword) : targetNode->CreateChildNode(stateKeyword);
			MkFloat2 rectSize;
			if (__TSI_ImageSetToTargetNode::ApplyFreeImage(imagePath, subsetName, stateNode, rectSize))
			{
				stateNode->SetVisible(true);
				targetNode->__SetFullSize(rectSize);
				return true;
			}
		}
		return false;
	}

	static void ApplyBodySize(MkBaseWindowNode* targetNode)
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

				MkFloat2 rectSize;
				__TSI_ImageSetToTargetNode::ApplyBodySize(bodySize, targetNode->GetChildNode(stateKeyword), rectSize);
				if (currState == beginState)
				{
					targetNode->__SetFullSize(rectSize);
				}

				currState = static_cast<DataType>(currState + 1);
			}
		}
	}

	static void SetState(DataType state, MkSceneNode* targetNode)
	{
		_SetState(state, targetNode);
		_SetHighlight(state, targetNode);
	}

	static const MkFloatRect* GetWorldAABR(const MkBaseWindowNode* targetNode)
	{
		if (targetNode != NULL)
		{
			DataType destState = static_cast<DataType>(MkWindowPresetStateInterface<DataType>::GetBegin());
			const MkHashStr& destKeyword = MkWindowPresetStateInterface<DataType>::GetKeyword(destState);
			const MkSceneNode* destNode = targetNode->GetChildNode(destKeyword);
			if (destNode != NULL)
			{
				return &destNode->GetWorldAABR();
			}
		}
		return NULL;
	}
};

class __TSI_ImageSetToComponentNode
{
public:
	static bool ApplyImageSetAndBodySize(eS2D_WindowPresetComponent component, const MkArray<MkHashStr>& imageSets, MkBaseWindowNode* targetNode)
	{
		if (component == eS2D_WPC_BackgroundWindow)
		{
			return __TSI_ImageSetToStateNode<eS2D_BackgroundState>::ApplyImageSetAndBodySize(imageSets, targetNode);
		}
		else if (component == eS2D_WPC_TitleWindow)
		{
			return __TSI_ImageSetToStateNode<eS2D_TitleState>::ApplyImageSetAndBodySize(imageSets, targetNode);
		}
		else if ((component >= eS2D_WPC_WindowStateTypeBegin) && (component < eS2D_WPC_WindowStateTypeEnd))
		{
			return __TSI_ImageSetToStateNode<eS2D_WindowState>::ApplyImageSetAndBodySize(imageSets, targetNode);
		}
		return false;
	}

	static void ApplyBodySize(eS2D_WindowPresetComponent component, MkBaseWindowNode* targetNode)
	{
		if (component == eS2D_WPC_BackgroundWindow)
		{
			return __TSI_ImageSetToStateNode<eS2D_BackgroundState>::ApplyBodySize(targetNode);
		}
		else if (component == eS2D_WPC_TitleWindow)
		{
			return __TSI_ImageSetToStateNode<eS2D_TitleState>::ApplyBodySize(targetNode);
		}
		else if ((component >= eS2D_WPC_WindowStateTypeBegin) && (component < eS2D_WPC_WindowStateTypeEnd))
		{
			return __TSI_ImageSetToStateNode<eS2D_WindowState>::ApplyBodySize(targetNode);
		}
	}

	static const MkFloatRect* GetWorldAABR(eS2D_WindowPresetComponent component, const MkBaseWindowNode* targetNode)
	{
		if (component == eS2D_WPC_BackgroundWindow)
		{
			return __TSI_ImageSetToStateNode<eS2D_BackgroundState>::GetWorldAABR(targetNode);
		}
		else if (component == eS2D_WPC_TitleWindow)
		{
			return __TSI_ImageSetToStateNode<eS2D_TitleState>::GetWorldAABR(targetNode);
		}
		else if ((component >= eS2D_WPC_WindowStateTypeBegin) && (component < eS2D_WPC_WindowStateTypeEnd))
		{
			return __TSI_ImageSetToStateNode<eS2D_WindowState>::GetWorldAABR(targetNode);
		}
		return NULL;
	}
};

//------------------------------------------------------------------------------------------------//

void MkBaseWindowNode::BasicPresetWindowDesc::SetStandardDesc(const MkHashStr& themeName, bool hasTitle)
{
	this->themeName = themeName;
	dragMovement = true;

	this->hasTitle = hasTitle;
	if (hasTitle)
	{
		titleHeight = 20.f;
		iconImageFilePath = MK_WR_PRESET.GetSystemImageFilePath();
		iconImageSubsetName = MK_WR_PRESET.GetWindowIconSampleSubsetName();
		iconImageHeightOffset = 0.f;
	}
	hasCancelIcon = hasTitle;
	
	hasFreeImageBG = true;
	bgImageFilePath = MK_WR_PRESET.GetSystemImageFilePath();
	bgImageSubsetName = MK_WR_PRESET.GetWindowBackgroundSampleSubsetName();

	hasOKButton = true;
	hasCancelButton = true;
}

void MkBaseWindowNode::BasicPresetWindowDesc::SetStandardDesc(const MkHashStr& themeName, bool hasTitle, const MkFloat2& windowSize)
{
	SetStandardDesc(themeName, hasTitle);

	hasFreeImageBG = false;
	bgImageFilePath.Clear();
	bgImageSubsetName.Clear();
	this->windowSize = windowSize;
}

void MkBaseWindowNode::BasicPresetWindowDesc::SetStandardDesc(const MkHashStr& themeName, bool hasTitle, const MkPathName& bgFilePath, const MkHashStr& subsetName)
{
	SetStandardDesc(themeName, hasTitle);

	bgImageFilePath = bgFilePath;
	bgImageSubsetName = subsetName;
}

MkBaseWindowNode::BasicPresetWindowDesc::BasicPresetWindowDesc()
{
	SetStandardDesc(MK_WR_PRESET.GetDefaultThemeName(), true, MkFloat2(150.f, 100.f));
}

//------------------------------------------------------------------------------------------------//

MkBaseWindowNode* MkBaseWindowNode::GetAncestorWindowNode(void) const
{
	MkSceneNode* parentNode = m_ParentNodePtr;
	while (true)
	{
		if (parentNode == NULL)
			return NULL;

		if (parentNode->GetNodeType() >= eS2D_SNT_BaseWindowNode)
			return dynamic_cast<MkBaseWindowNode*>(parentNode);

		parentNode = parentNode->GetParentNode();
	}
}

const MkHashStr& MkBaseWindowNode::GetManagedRootName(void) const
{
	MkSceneNode* targetNode = const_cast<MkBaseWindowNode*>(this);
	while (true)
	{
		if (targetNode == NULL)
			return MkHashStr::NullHash; // error

		MkSceneNode* parentNode = targetNode->GetParentNode();
		if (parentNode == NULL)
			return MkHashStr::NullHash; // root

		if ((targetNode->GetNodeType() >= eS2D_SNT_BaseWindowNode) && (parentNode->GetParentNode() == NULL))
		{
			return targetNode->GetNodeName(); // 부모는 존재하지만 조부모가 없는 경우
		}

		targetNode = parentNode;
	}
}

void MkBaseWindowNode::Load(const MkDataNode& node)
{
	// enable
	bool enable = true;
	node.GetData(ENABLE_KEY, enable, 0);

	// preset
	MkStr presetThemeName;
	node.GetData(PRESET_THEME_NAME_KEY, presetThemeName, 0);
	m_PresetThemeName = presetThemeName;

	MkStr componentName;
	node.GetData(PRESET_COMPONENT_KEY, componentName, 0);
	m_PresetComponentName = componentName;

	MkInt2 presetBodySize;
	node.GetData(PRESET_BODY_SIZE_KEY, presetBodySize, 0);
	m_PresetBodySize = MkFloat2(static_cast<float>(presetBodySize.x), static_cast<float>(presetBodySize.y));

	MkInt2 presetFullSize;
	node.GetData(PRESET_FULL_SIZE_KEY, presetFullSize, 0);
	m_PresetFullSize = MkFloat2(static_cast<float>(presetFullSize.x), static_cast<float>(presetFullSize.y));

	// attribute
	m_Attribute.Clear();
	node.GetData(ATTRIBUTE_KEY, m_Attribute.m_Field, 0);

	// MkSceneNode
	MkSceneNode::Load(node);

	// 위의 MkSceneNode::Load(node)에서 eS2D_WindowState에 영향받는 component가 있다면 enable(true)인 상태로 로딩됨
	// 따라서 그 뒤에 읽어들인 enable을 적용 해 주어야 올바른 eS2D_WindowState가 적용 됨
	m_Enable = true;
	SetEnable(enable);
}

void MkBaseWindowNode::Save(MkDataNode& node)
{
	// 기존 템플릿이 없으면 템플릿 적용
	_ApplyBuildingTemplateToSave(node, TEMPLATE_NAME);

	node.SetData(ENABLE_KEY, m_Enable, 0);
	node.SetData(PRESET_THEME_NAME_KEY, m_PresetThemeName.GetString(), 0);
	node.SetData(PRESET_COMPONENT_KEY, m_PresetComponentName.GetString(), 0);
	node.SetData(PRESET_BODY_SIZE_KEY, MkInt2(static_cast<int>(m_PresetBodySize.x), static_cast<int>(m_PresetBodySize.y)), 0);
	node.SetData(PRESET_FULL_SIZE_KEY, MkInt2(static_cast<int>(m_PresetFullSize.x), static_cast<int>(m_PresetFullSize.y)), 0);
	node.SetData(ATTRIBUTE_KEY, m_Attribute.m_Field, 0);

	// MkSceneNode
	MkSceneNode::Save(node);
}

void MkBaseWindowNode::SetEnable(bool enable)
{
	eS2D_WindowState windowState = eS2D_WS_MaxWindowState;
	if (m_Enable && (!enable)) // on -> off
	{
		windowState = eS2D_WS_DisableState;
	}
	else if ((!m_Enable) && enable) // off -> on
	{
		windowState = eS2D_WS_DefaultState;
	}

	if (windowState != eS2D_WS_MaxWindowState)
	{
		m_Enable = enable;

		eS2D_WindowPresetComponent component = MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName);
		if ((component >= eS2D_WPC_WindowStateTypeBegin) && (component < eS2D_WPC_WindowStateTypeEnd))
		{
			__TSI_ImageSetToStateNode<eS2D_WindowState>::SetState(windowState, this);
		}

		MkArray<MkBaseWindowNode*> buffer;
		if (_CollectUpdatableWindowNodes(buffer))
		{
			MK_INDEXING_LOOP(buffer, i)
			{
				buffer[i]->SetEnable(enable);
			}
		}
	}
}

const MkFloatRect& MkBaseWindowNode::GetWindowRect(void) const
{
	if (!m_PresetComponentName.Empty())
	{
		eS2D_WindowPresetComponent component = MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName);
		const MkFloatRect* targetRect = __TSI_ImageSetToComponentNode::GetWorldAABR(component, this);
		if (targetRect != NULL)
		{
			return *targetRect;
		}
	}
	return m_WorldAABR;
}

MkBaseWindowNode* MkBaseWindowNode::CreateBasicWindow(const MkHashStr& nodeName, const BasicPresetWindowDesc& desc)
{
	BasicPresetWindowDesc d = desc;

	// free image BG일 경우 먼저 windowSize 설정
	if (d.hasFreeImageBG)
	{
		MkBaseTexturePtr texture;
		MK_TEXTURE_POOL.GetBitmapTexture(d.bgImageFilePath, texture, 0);
		if (texture != NULL)
		{
			d.windowSize = texture->GetSubsetSize(d.bgImageSubsetName);
		}
	}

	const float MARGIN = MK_WR_PRESET.GetMargin();
	
	// sindow size 유효성 검사
	if (d.windowSize.x < MARGIN * 4.f)
	{
		d.windowSize.x = MARGIN * 4.f;
	}
	if (d.windowSize.y < MARGIN * 4.f)
	{
		d.windowSize.x = MARGIN * 4.f;
	}

	// 반환 할 노드
	MkBaseWindowNode* rootWindow = NULL;

	// title bar
	MkBaseWindowNode* titleWindow = NULL;
	if (d.hasTitle)
	{
		// title body size 검사
		if (d.titleHeight < MARGIN * 2.f)
		{
			d.titleHeight = MARGIN * 2.f;
		}
		MkFloat2 titleBodySize(d.windowSize.x - MARGIN * 4.f, d.titleHeight - MARGIN * 2.f);

		titleWindow = __CreateWindowPreset(nodeName, d.themeName, eS2D_WPC_TitleWindow, titleBodySize);
		if (titleWindow != NULL)
		{
			rootWindow = titleWindow;

			titleWindow->SetLocalPosition(MkFloat2(0.f, -titleWindow->GetPresetFullSize().y));

			// window icon
			if (d.hasIcon)
			{
				SetPresetComponentIcon(L"WindowIcon", eRAP_LeftCenter, MkFloat2(MARGIN, 0.f), d.iconImageHeightOffset, d.iconImageFilePath, d.iconImageSubsetName);
			}

			// cancel icon
			if (d.hasCancelIcon)
			{
				MkBaseWindowNode* cancelWindow = titleWindow->__CreateWindowPreset(L"CancelButton", d.themeName, eS2D_WPC_CancelIcon, MkFloat2(0.f, 0.f));
				if (cancelWindow != NULL)
				{
					MkFloat2 localPos =	MkFloatRect(MkFloat2(0.f, 0.f), titleWindow->GetPresetFullSize()).GetSnapPosition
						(MkFloatRect(MkFloat2(0.f, 0.f), cancelWindow->GetPresetFullSize()), eRAP_RightCenter, MkFloat2(MARGIN, 0.f));
					cancelWindow->SetLocalPosition(MkVec3(localPos.x, localPos.y, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				}
			}

			// title icon
			_SetSampleComponentIcon(d.themeName, titleWindow, MKDEF_TITLE_BAR_SAMPLE_STRING);
		}
	}

	// background
	MkBaseWindowNode* backgroundWindow = NULL;
	MkBaseWindowNode* parentWindow = (titleWindow == NULL) ? this : titleWindow;
	MkHashStr bgNodeName = (titleWindow == NULL) ? nodeName : L"Background";
	if (d.hasFreeImageBG)
	{
		backgroundWindow = new MkBaseWindowNode(nodeName);
		if (backgroundWindow != NULL)
		{
			if (backgroundWindow->CreateFreeImageBaseBackgroundWindow(d.bgImageFilePath, d.bgImageSubsetName))
			{
				parentWindow->AttachChildNode(backgroundWindow);
			}
			else
			{
				MK_DELETE(backgroundWindow);
			}
		}
	}
	else
	{
		// background body size 검사
		MkFloat2 bgBodySize(d.windowSize.x - MARGIN * 2.f, d.windowSize.y - MARGIN * 2.f);
		backgroundWindow = parentWindow->__CreateWindowPreset(bgNodeName, d.themeName, eS2D_WPC_BackgroundWindow, bgBodySize);
	}

	if (backgroundWindow != NULL)
	{
		float heightOffset = 0.f;
		if (titleWindow == NULL)
		{
			rootWindow = backgroundWindow;
		}
		else
		{
			heightOffset = titleWindow->GetPresetFullSize().y;
		}
		backgroundWindow->SetLocalPosition(MkVec3(-MARGIN, heightOffset - backgroundWindow->GetPresetFullSize().y + MARGIN, MKDEF_BASE_WINDOW_DEPTH_GRID));
		backgroundWindow->SetAttribute(MkBaseWindowNode::eConfinedToParent, true);

		const MkFloat2 buttonBodySize(50.f, 12.f);
		// ok button
		MkBaseWindowNode* okWindow = NULL;
		if (d.hasOKButton)
		{
			okWindow = backgroundWindow->__CreateWindowPreset(L"PButton00", d.themeName, eS2D_WPC_PossitiveButton, buttonBodySize);
			okWindow->SetAttribute(MkBaseWindowNode::eConfinedToParent, true);
			_SetSampleComponentIcon(d.themeName, okWindow, MKDEF_OK_BTN_SAMPLE_STRING);
		}

		// cancel button
		MkBaseWindowNode* cancelWindow = NULL;
		if (d.hasCancelButton)
		{
			cancelWindow = backgroundWindow->__CreateWindowPreset(L"NButton00", d.themeName, eS2D_WPC_NegativeButton, buttonBodySize);
			cancelWindow->SetAttribute(MkBaseWindowNode::eConfinedToParent, true);
			_SetSampleComponentIcon(d.themeName, cancelWindow, MKDEF_CANCEL_BTN_SAMPLE_STRING);
		}

		if ((okWindow != NULL) && (cancelWindow == NULL))
		{
			float posX = (backgroundWindow->GetPresetFullSize().x - okWindow->GetPresetFullSize().x) / 2.f;
			okWindow->SetLocalPosition(MkVec3(posX, MARGIN, -MKDEF_BASE_WINDOW_DEPTH_GRID)); // 하단 중앙 정렬
		}
		else if ((okWindow == NULL) && (cancelWindow != NULL))
		{
			float posX = (backgroundWindow->GetPresetFullSize().x - cancelWindow->GetPresetFullSize().x) / 2.f;
			cancelWindow->SetLocalPosition(MkVec3(posX, MARGIN, -MKDEF_BASE_WINDOW_DEPTH_GRID)); // 하단 중앙 정렬
		}
		else if ((okWindow != NULL) && (cancelWindow != NULL))
		{
			float blank = (backgroundWindow->GetPresetFullSize().x - okWindow->GetPresetFullSize().x * 2.f) / 3.f;
			okWindow->SetLocalPosition(MkVec3(blank, MARGIN, -MKDEF_BASE_WINDOW_DEPTH_GRID));
			cancelWindow->SetLocalPosition(MkVec3(blank * 2.f + okWindow->GetPresetFullSize().x, MARGIN, -MKDEF_BASE_WINDOW_DEPTH_GRID));
		}
	}

	if (rootWindow != NULL)
	{
		// drag movement
		rootWindow->SetAttribute(MkBaseWindowNode::eDragMovement, d.dragMovement);
		rootWindow->SetAttribute(MkBaseWindowNode::eConfinedToScreen, true);
	}

	return rootWindow;
}

bool MkBaseWindowNode::CreateWindowPreset(const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& bodySize)
{
	if (m_PresetComponentName.Empty())
	{
		const MkArray<MkHashStr>& imageSets = MK_WR_PRESET.GetWindowTypeImageSet(themeName, component);
		if (!imageSets.Empty())
		{
			m_PresetThemeName = themeName;
			m_PresetBodySize = bodySize;
			m_PresetComponentName = MkWindowPreset::GetWindowPresetComponentKeyword(component);

			return __TSI_ImageSetToComponentNode::ApplyImageSetAndBodySize(component, imageSets, this);
		}
	}
	return false;
}

bool MkBaseWindowNode::CreateFreeImageBaseBackgroundWindow(const MkPathName& imagePath, const MkHashStr& subsetName)
{
	if (m_PresetComponentName.Empty())
	{
		m_PresetThemeName.Clear();
		m_PresetBodySize.Clear();
		m_PresetComponentName = MkWindowPreset::GetWindowPresetComponentKeyword(eS2D_WPC_BackgroundWindow);

		return __TSI_ImageSetToStateNode<eS2D_BackgroundState>::ApplyFreeImageToState(eS2D_BS_DefaultState, imagePath, subsetName, this);
	}
	return false;
}

void MkBaseWindowNode::SetPresetThemeName(const MkHashStr& themeName)
{
	if ((!m_PresetThemeName.Empty()) && (themeName != m_PresetThemeName) && (!m_PresetComponentName.Empty())) // 테마가 다르면
	{
		eS2D_WindowPresetComponent component = MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName);
		const MkArray<MkHashStr>& newImageSets = MK_WR_PRESET.GetWindowTypeImageSet(themeName, component);
		const MkArray<MkHashStr>& oldImageSets = MK_WR_PRESET.GetWindowTypeImageSet(m_PresetThemeName, component);

		if ((!newImageSets.Empty()) && (newImageSets != oldImageSets) && // 테마명은 다르더라도 image set은 같을 수 있으므로 비교 필요
			__TSI_ImageSetToComponentNode::ApplyImageSetAndBodySize(component, newImageSets, this))
		{
			m_PresetThemeName = themeName;

			// 위의 ApplyImageSetAndBodySize()을 거치면 visible 상태가 초기화 되버리기 때문에 disable일 경우 반영해 주어야 함
			if ((!m_Enable) && (component >= eS2D_WPC_WindowStateTypeBegin) && (component < eS2D_WPC_WindowStateTypeEnd))
			{
				__TSI_ImageSetToStateNode<eS2D_WindowState>::SetState(eS2D_WS_DisableState, this);
			}
		}
	}

	MkArray<MkBaseWindowNode*> buffer;
	if (_CollectUpdatableWindowNodes(buffer))
	{
		MK_INDEXING_LOOP(buffer, i)
		{
			buffer[i]->SetPresetThemeName(themeName);
		}
	}
}

void MkBaseWindowNode::SetPresetComponentBodySize(const MkFloat2& bodySize)
{
	if ((!m_PresetComponentName.Empty()) && (bodySize != m_PresetBodySize))
	{
		m_PresetBodySize = bodySize;
		__TSI_ImageSetToComponentNode::ApplyBodySize(MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName), this);
	}
}

bool MkBaseWindowNode::SetFreeImageToBackgroundWindow(const MkPathName& imagePath, const MkHashStr& subsetName)
{
	// 테마가 없고 component가 eS2D_WPC_BackgroundWindow 타입이면
	if (m_PresetThemeName.Empty() && (m_PresetComponentName == MkWindowPreset::GetWindowPresetComponentKeyword(eS2D_WPC_BackgroundWindow)))
	{
		return __TSI_ImageSetToStateNode<eS2D_BackgroundState>::ApplyFreeImageToState(eS2D_BS_DefaultState, imagePath, subsetName, this);
	}
	return false;
}

bool MkBaseWindowNode::SetPresetComponentIcon
(const MkHashStr& iconName, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset, const MkPathName& imagePath, const MkHashStr& subsetName)
{
	if (!m_PresetComponentName.Empty())
	{
		MkSRect* iconRect = ExistSRect(iconName) ? GetSRect(iconName) : CreateSRect(iconName);
		if (iconRect != NULL)
		{
			iconRect->SetTexture(imagePath, subsetName);
			iconRect->AlignRect(GetPresetFullSize(), alignment, border, heightOffset, -MKDEF_BASE_WINDOW_DEPTH_GRID);
			return true;
		}
	}
	return false;
}

bool MkBaseWindowNode::SetPresetComponentIcon
(const MkHashStr& iconName, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset, const MkStr& decoStr)
{
	if (!m_PresetComponentName.Empty())
	{
		MkSRect* iconRect = ExistSRect(iconName) ? GetSRect(iconName) : CreateSRect(iconName);
		if (iconRect != NULL)
		{
			iconRect->SetDecoString(decoStr);
			iconRect->AlignRect(GetPresetFullSize(), alignment, border, heightOffset, -MKDEF_BASE_WINDOW_DEPTH_GRID);
			return true;
		}
	}
	return false;
}

bool MkBaseWindowNode::SetPresetComponentIcon
(const MkHashStr& iconName, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset, const MkArray<MkHashStr>& nodeNameAndKey)
{
	if (!m_PresetComponentName.Empty())
	{
		MkSRect* iconRect = ExistSRect(iconName) ? GetSRect(iconName) : CreateSRect(iconName);
		if (iconRect != NULL)
		{
			iconRect->SetDecoString(nodeNameAndKey);
			iconRect->AlignRect(GetPresetFullSize(), alignment, border, heightOffset, -MKDEF_BASE_WINDOW_DEPTH_GRID);
			return true;
		}
	}
	return false;
}

bool MkBaseWindowNode::SetPresetComponentIcon
(bool highlight, eRectAlignmentPosition alignment, const MkFloat2& border, const MkPathName& imagePath, const MkHashStr& subsetName)
{
	if (!m_PresetComponentName.Empty())
	{
		eS2D_WindowPresetComponent component = MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName);
		if ((component == eS2D_WPC_TitleWindow) || ((component >= eS2D_WPC_WindowStateTypeBegin) && (component < eS2D_WPC_WindowStateTypeEnd)))
		{
			return SetPresetComponentIcon((highlight) ? COMPONENT_HIGHLIGHT_TAG_NAME : COMPONENT_NORMAL_TAG_NAME, alignment, border, 0.f, imagePath, subsetName);
		}
	}
	return false;
}

bool MkBaseWindowNode::SetPresetComponentIcon(bool highlight, eRectAlignmentPosition alignment, const MkFloat2& border, const MkStr& decoStr)
{
	if (!m_PresetComponentName.Empty())
	{
		eS2D_WindowPresetComponent component = MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName);
		if ((component == eS2D_WPC_TitleWindow) || ((component >= eS2D_WPC_WindowStateTypeBegin) && (component < eS2D_WPC_WindowStateTypeEnd)))
		{
			return SetPresetComponentIcon((highlight) ? COMPONENT_HIGHLIGHT_TAG_NAME : COMPONENT_NORMAL_TAG_NAME, alignment, border, 0.f, decoStr);
		}
	}
	return false;
}

bool MkBaseWindowNode::SetPresetComponentIcon(bool highlight, eRectAlignmentPosition alignment, const MkFloat2& border, const MkArray<MkHashStr>& nodeNameAndKey)
{
	if (!m_PresetComponentName.Empty())
	{
		eS2D_WindowPresetComponent component = MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName);
		if ((component == eS2D_WPC_TitleWindow) || ((component >= eS2D_WPC_WindowStateTypeBegin) && (component < eS2D_WPC_WindowStateTypeEnd)))
		{
			return SetPresetComponentIcon((highlight) ? COMPONENT_HIGHLIGHT_TAG_NAME : COMPONENT_NORMAL_TAG_NAME, alignment, border, 0.f, nodeNameAndKey);
		}
	}
	return false;
}

bool MkBaseWindowNode::InputEventMousePress(unsigned int button, const MkFloat2& position, bool managedRoot)
{
	MkArray<MkBaseWindowNode*> buffer;
	if (_CollectUpdatableWindowNodes(position, buffer))
	{
		MK_INDEXING_LOOP(buffer, i)
		{
			if (buffer[i]->InputEventMousePress(button, position, false))
				return true;
		}
	}
	return false;
}

bool MkBaseWindowNode::InputEventMouseRelease(unsigned int button, const MkFloat2& position, bool managedRoot)
{
	if ((button == 0) && GetWindowRect().CheckIntersection(position)) // left release
	{
		if (MkWindowPreset::GetWindowPresetComponentEnum(GetPresetComponentName()) == eS2D_WPC_CancelIcon)
		{
			MK_WIN_EVENT_MGR.DeactivateWindow(GetManagedRootName());
			return true;
		}
	}
	
	MkArray<MkBaseWindowNode*> buffer;
	if (_CollectUpdatableWindowNodes(position, buffer))
	{
		MK_INDEXING_LOOP(buffer, i)
		{
			if (buffer[i]->InputEventMouseRelease(button, position, false))
				return true;
		}
	}
	return false;
}

bool MkBaseWindowNode::InputEventMouseDoubleClick(unsigned int button, const MkFloat2& position, bool managedRoot)
{
	MkArray<MkBaseWindowNode*> buffer;
	if (_CollectUpdatableWindowNodes(position, buffer))
	{
		MK_INDEXING_LOOP(buffer, i)
		{
			if (buffer[i]->InputEventMouseDoubleClick(button, position, false))
				return true;
		}
	}
	return false;
}

bool MkBaseWindowNode::InputEventMouseWheelMove(int delta, const MkFloat2& position, bool managedRoot)
{
	MkArray<MkBaseWindowNode*> buffer;
	if (_CollectUpdatableWindowNodes(position, buffer))
	{
		MK_INDEXING_LOOP(buffer, i)
		{
			if (buffer[i]->InputEventMouseWheelMove(delta, position, false))
				return true;
		}
	}
	return false;
}

void MkBaseWindowNode::InputEventMouseMove(bool inside, bool (&btnPushing)[3], const MkFloat2& position, bool managedRoot)
{
	// eS2D_WindowState가 적용된 노드면 eS2D_WS_DefaultState, eS2D_WS_OnClickState, eS2D_WS_OnCursorState 중 하나를 지정
	eS2D_WindowPresetComponent component = MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName);
	if ((component >= eS2D_WPC_WindowStateTypeBegin) && (component < eS2D_WPC_WindowStateTypeEnd))
	{
		eS2D_WindowState windowState = eS2D_WS_DefaultState;
		if (GetWorldAABR().CheckIntersection(position))
		{
			windowState = btnPushing[0] ? eS2D_WS_OnClickState : eS2D_WS_OnCursorState;
		}

		__TSI_ImageSetToStateNode<eS2D_WindowState>::SetState(windowState, this);
	}

	MkArray<MkBaseWindowNode*> buffer;
	if (_CollectUpdatableWindowNodes(buffer))
	{
		MK_INDEXING_LOOP(buffer, i)
		{
			if (buffer[i]->GetEnable())
			{
				buffer[i]->InputEventMouseMove(inside, btnPushing, position, false);
			}
		}
	}
}

void MkBaseWindowNode::OnFocus(bool managedRoot)
{
	if (m_PresetComponentName == MkWindowPreset::GetWindowPresetComponentKeyword(eS2D_WPC_TitleWindow))
	{
		__TSI_ImageSetToStateNode<eS2D_TitleState>::SetState(eS2D_TS_OnFocusState, this);
	}

	MkArray<MkBaseWindowNode*> buffer;
	if (_CollectUpdatableWindowNodes(buffer))
	{
		MK_INDEXING_LOOP(buffer, i)
		{
			buffer[i]->OnFocus(false);
		}
	}
}

void MkBaseWindowNode::LostFocus(bool managedRoot)
{
	if (m_PresetComponentName == MkWindowPreset::GetWindowPresetComponentKeyword(eS2D_WPC_TitleWindow))
	{
		__TSI_ImageSetToStateNode<eS2D_TitleState>::SetState(eS2D_TS_LostFocusState, this);
	}

	MkArray<MkBaseWindowNode*> buffer;
	if (_CollectUpdatableWindowNodes(buffer))
	{
		MK_INDEXING_LOOP(buffer, i)
		{
			buffer[i]->LostFocus(false);
		}
	}
}

MkBaseWindowNode::MkBaseWindowNode(const MkHashStr& name) : MkSceneNode(name)
{
	m_Enable = true;
}

MkBaseWindowNode::MkBaseWindowNode(const MkHashStr& name, const MkHashStr& themeName, const MkFloat2& bodySize, const MkHashStr& componentName) : MkSceneNode(name)
{
	m_Enable = true;

	m_PresetThemeName = themeName;
	m_PresetBodySize = bodySize;
	m_PresetComponentName = componentName;
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
	tNode->CreateUnit(PRESET_THEME_NAME_KEY, MkStr::Null);
	tNode->CreateUnit(PRESET_COMPONENT_KEY, MkStr::Null);
	tNode->CreateUnit(PRESET_BODY_SIZE_KEY, MkInt2(0, 0));
	tNode->CreateUnit(PRESET_FULL_SIZE_KEY, MkInt2(0, 0));
	tNode->CreateUnit(ATTRIBUTE_KEY, static_cast<unsigned int>(0));

	tNode->DeclareToTemplate(true);
}

MkBaseWindowNode* MkBaseWindowNode::__GetHitWindows(const MkFloat2& position)
{
	MkPairArray<float, MkBaseWindowNode*> hitWindows(8);
	__GetHitWindows(position, hitWindows);
	if (hitWindows.Empty())
	{
		return NULL;
	}
	else if (hitWindows.GetSize() > 1)
	{
		hitWindows.SortInAscendingOrder();
	}
	return hitWindows.GetFieldAt(0);
}

MkBaseWindowNode* MkBaseWindowNode::__CreateWindowPreset
(const MkHashStr& nodeName, const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& bodySize)
{
	if (!ChildExist(nodeName))
	{
		MkBaseWindowNode* targetNode = new MkBaseWindowNode(nodeName);
		if (targetNode != NULL)
		{
			if (targetNode->CreateWindowPreset(themeName, component, bodySize))
			{
				AttachChildNode(targetNode);
			}
			else
			{
				MK_DELETE(targetNode);
			}
		}
		return targetNode;
	}
	return NULL;
}

void MkBaseWindowNode::__GetHitWindows(const MkFloat2& position, MkPairArray<float, MkBaseWindowNode*>& hitWindows)
{
	if (GetWindowRect().CheckIntersection(position))
	{
		hitWindows.PushBack(GetWorldPosition().z, this);
	}

	MkArray<MkBaseWindowNode*> buffer;
	if (_CollectUpdatableWindowNodes(position, buffer))
	{
		MK_INDEXING_LOOP(buffer, i)
		{
			buffer[i]->__GetHitWindows(position, hitWindows);
		}
	}
}

bool MkBaseWindowNode::_CollectUpdatableWindowNodes(MkArray<MkBaseWindowNode*>& buffer)
{
	if (!m_ChildrenNode.Empty())
	{
		buffer.Reserve(m_ChildrenNode.GetSize());

		MkHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
		MK_STL_LOOP(looper)
		{
			MkSceneNode* node = looper.GetCurrentField();
			if (node->GetNodeType() >= eS2D_SNT_BaseWindowNode)
			{
				MkBaseWindowNode* winNode = dynamic_cast<MkBaseWindowNode*>(node);
				if (winNode != NULL)
				{
					buffer.PushBack(winNode);
				}
			}
		}
	}
	return (!buffer.Empty());
}

bool MkBaseWindowNode::_CollectUpdatableWindowNodes(const MkFloat2& position, MkArray<MkBaseWindowNode*>& buffer)
{
	if (!m_ChildrenNode.Empty())
	{
		buffer.Reserve(m_ChildrenNode.GetSize());

		MkHashMapLooper<MkHashStr, MkSceneNode*> looper(m_ChildrenNode);
		MK_STL_LOOP(looper)
		{
			MkSceneNode* node = looper.GetCurrentField();
			if ((node->GetNodeType() >= eS2D_SNT_BaseWindowNode) && (node->GetWorldAABR().CheckIntersection(position)))
			{
				MkBaseWindowNode* winNode = dynamic_cast<MkBaseWindowNode*>(node);
				if ((winNode != NULL) && (winNode->GetEnable()))
				{
					buffer.PushBack(winNode);
				}
			}
		}
	}
	return (!buffer.Empty());
}

void MkBaseWindowNode::_SetSampleComponentIcon(const MkHashStr& themeName, MkBaseWindowNode* targetNode, const MkStr& tag)
{
	if (targetNode != NULL)
	{
		MkStr hBuf, nBuf;
		if (MK_WR_PRESET.ConvertToDecoStr(themeName, tag, hBuf, nBuf))
		{
			targetNode->SetPresetComponentIcon(true, eRAP_MiddleCenter, MkFloat2(0.f, 0.f), hBuf);
			targetNode->SetPresetComponentIcon(false, eRAP_MiddleCenter, MkFloat2(0.f, 0.f), nBuf);
		}
	}
}

//------------------------------------------------------------------------------------------------//