
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkTexturePool.h"
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkBaseWindowNode.h"

#include "MkS2D_MkWindowEventManager.h"


const static MkHashStr COMPONENT_HIGHLIGHT_TAG_NAME = L"HighlightTag";
const static MkHashStr COMPONENT_NORMAL_TAG_NAME = L"NormalTag";

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

	static void _LineUpSRects(MkWindowTypeImageSet::eSetType setType, MkArray<MkSRect*>& srects, const MkFloat2& sizeIn, MkFloat2& sizeOut)
	{
		if (setType == MkWindowTypeImageSet::eSingleType)
		{
			sizeOut = srects[MkWindowTypeImageSet::eL]->GetLocalSize();
		}
		else
		{
			float gap = MK_WR_PRESET.GetMargin() * 2.f;
			MkFloat2 bodySize = sizeIn - MkFloat2(gap, gap);
			if (bodySize.x < 0.f) { bodySize.x = 0.f; }
			if (bodySize.y < 0.f) { bodySize.y = 0.f; }

			switch (setType) // size를 반영한 위치 및 크기 재배치
			{
			case MkWindowTypeImageSet::e3And1Type:
				{
					MkSRect* lRect = srects[MkWindowTypeImageSet::eL];
					MkSRect* mRect = srects[MkWindowTypeImageSet::eM];
					MkSRect* rRect = srects[MkWindowTypeImageSet::eR];

					mRect->SetLocalPosition(MkFloat2(lRect->GetLocalSize().x, 0.f));
					mRect->SetLocalSize(MkFloat2(bodySize.x, mRect->GetLocalSize().y));
					rRect->SetLocalPosition(MkFloat2(lRect->GetLocalSize().x + bodySize.x, 0.f));

					sizeOut.x = lRect->GetLocalSize().x + bodySize.x + rRect->GetLocalSize().x;
					sizeOut.y = lRect->GetLocalSize().y;
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

					sizeOut.x = tRect->GetLocalSize().y;
					sizeOut.y = tRect->GetLocalSize().y + bodySize.y + bRect->GetLocalSize().y;
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

					sizeOut.x = ltRect->GetLocalSize().x + bodySize.x + rtRect->GetLocalSize().x;
					sizeOut.y = ltRect->GetLocalSize().y + bodySize.y + lbRect->GetLocalSize().y;
				}
				break;
			}
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
	static bool ApplyImageSetAndSize(const MkHashStr& imageSet, const MkFloat2& sizeIn, MkSceneNode* targetNode, MkFloat2& sizeOut)
	{
		if (targetNode != NULL)
		{
			const MkWindowTypeImageSet::Pack& pack = MK_WR_IMAGE_SET.GetPack(imageSet);
			if (pack.type != MkWindowTypeImageSet::eNull)
			{
				// bitmap texture
				MkBaseTexturePtr texture;
				MK_TEXTURE_POOL.GetBitmapTexture(pack.filePath, texture);
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
						_LineUpSRects(pack.type, srects, sizeIn, sizeOut);
					}

					return true;
				}
			}
		}
		return false;
	}

	static bool ApplyFreeImage(const MkPathName& imagePath, const MkHashStr& subsetName, MkSceneNode* targetNode, MkFloat2& sizeOut)
	{
		if (targetNode != NULL)
		{
			MkBaseTexturePtr texture;
			MK_TEXTURE_POOL.GetBitmapTexture(imagePath, texture);
			if (texture != NULL)
			{
				// 기존 SRect 삭제
				targetNode->DeleteAllSRects();

				// MkWindowTypeImageSet::eSingleType으로 처리
				const MkHashStr& rectName = MkWindowTypeImageSet::GetImageSetKeyword(MkWindowTypeImageSet::eSingleType, static_cast<MkWindowTypeImageSet::eTypeIndex>(0));
				MkSRect* srect = _SetSRect(rectName, texture, subsetName, targetNode);
				if (srect != NULL)
				{
					sizeOut = srect->GetLocalSize();
					return true;
				}
			}
		}
		return false;
	}

	static void ApplySize(const MkFloat2& sizeIn, MkSceneNode* targetNode, MkFloat2& sizeOut)
	{
		if (targetNode != NULL)
		{
			MkArray<MkSRect*> srects(9);

			if (_GetMatchingSRect(targetNode, MkWindowTypeImageSet::eSingleType, MkWindowTypeImageSet::eL, srects))
			{
				_LineUpSRects(MkWindowTypeImageSet::eSingleType, srects, sizeIn, sizeOut);
				return;
			}
			srects.Flush();
			
			if (_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And1Type, MkWindowTypeImageSet::eL, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And1Type, MkWindowTypeImageSet::eM, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e3And1Type, MkWindowTypeImageSet::eR, srects))
			{
				_LineUpSRects(MkWindowTypeImageSet::e3And1Type, srects, sizeIn, sizeOut);
				return;
			}
			srects.Flush();

			if (_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e1And3Type, MkWindowTypeImageSet::eT, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e1And3Type, MkWindowTypeImageSet::eC, srects) &&
				_GetMatchingSRect(targetNode, MkWindowTypeImageSet::e1And3Type, MkWindowTypeImageSet::eB, srects))
			{
				_LineUpSRects(MkWindowTypeImageSet::e1And3Type, srects, sizeIn, sizeOut);
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
				_LineUpSRects(MkWindowTypeImageSet::e3And3Type, srects, sizeIn, sizeOut);
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
		MkFloatRect anchorRect(MkFloat2(0.f, 0.f), targetNode->GetPresetComponentSize());
		MkFloat2 position = anchorRect.GetSnapPosition(stateRect->GetLocalRect(), alignment, border);
		stateRect->SetLocalPosition(position);
		stateRect->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID); // 살짝 앞으로 나오게
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
	static bool ApplyImageSetAndSize(const MkArray<MkHashStr>& imageSets, MkBaseWindowNode* targetNode)
	{
		if (targetNode != NULL)
		{
			const MkFloat2& sizeIn = targetNode->GetPresetComponentSize();

			DataType beginState = static_cast<DataType>(MkWindowPresetStateInterface<DataType>::GetBegin());
			DataType endState = static_cast<DataType>(MkWindowPresetStateInterface<DataType>::GetEnd());
			DataType currState = beginState;
			while (currState < endState)
			{
				const MkHashStr& stateKeyword = MkWindowPresetStateInterface<DataType>::GetKeyword(currState);

				// 동일 component 노드는 네이밍 규칙에 의해 동일한 이름의 자식(state)들을 가지고 있음을 보장
				MkSceneNode* stateNode = targetNode->ChildExist(stateKeyword) ? targetNode->GetChildNode(stateKeyword) : targetNode->CreateChildNode(stateKeyword);

				MkFloat2 sizeOut;
				if (__TSI_ImageSetToTargetNode::ApplyImageSetAndSize(imageSets[currState], sizeIn, stateNode, sizeOut))
				{
					bool defaultState = (currState == beginState);
					stateNode->SetVisible(defaultState);
					if (defaultState)
					{
						targetNode->__SetPresetComponentSize(sizeOut);
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
			MkFloat2 sizeOut;
			bool defaultState = (state == static_cast<DataType>(MkWindowPresetStateInterface<DataType>::GetBegin()));
			if (__TSI_ImageSetToTargetNode::ApplyFreeImage(imagePath, subsetName, stateNode, sizeOut))
			{
				stateNode->SetVisible(defaultState);
				if (defaultState)
				{
					targetNode->__SetPresetComponentSize(sizeOut);
				}
				return true;
			}
		}
		return false;
	}

	static void ApplySize(MkBaseWindowNode* targetNode)
	{
		if (targetNode != NULL)
		{
			const MkFloat2& sizeIn = targetNode->GetPresetComponentSize();

			DataType beginState = static_cast<DataType>(MkWindowPresetStateInterface<DataType>::GetBegin());
			DataType endState = static_cast<DataType>(MkWindowPresetStateInterface<DataType>::GetEnd());
			DataType currState = beginState;
			while (currState < endState)
			{
				const MkHashStr& stateKeyword = MkWindowPresetStateInterface<DataType>::GetKeyword(currState);

				MkFloat2 sizeOut;
				__TSI_ImageSetToTargetNode::ApplySize(sizeIn, targetNode->GetChildNode(stateKeyword), sizeOut);
				if (currState == beginState)
				{
					targetNode->__SetPresetComponentSize(sizeOut);
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
	static bool ApplyImageSetAndSize(eS2D_WindowPresetComponent component, const MkArray<MkHashStr>& imageSets, MkBaseWindowNode* targetNode)
	{
		if (IsBackgroundStateType(component))
		{
			return __TSI_ImageSetToStateNode<eS2D_BackgroundState>::ApplyImageSetAndSize(imageSets, targetNode);
		}
		else if (IsTitleStateType(component))
		{
			return __TSI_ImageSetToStateNode<eS2D_TitleState>::ApplyImageSetAndSize(imageSets, targetNode);
		}
		else if (IsWindowStateType(component))
		{
			return __TSI_ImageSetToStateNode<eS2D_WindowState>::ApplyImageSetAndSize(imageSets, targetNode);
		}
		return false;
	}

	static void ApplySize(eS2D_WindowPresetComponent component, MkBaseWindowNode* targetNode)
	{
		if (IsBackgroundStateType(component))
		{
			return __TSI_ImageSetToStateNode<eS2D_BackgroundState>::ApplySize(targetNode);
		}
		else if (IsTitleStateType(component))
		{
			return __TSI_ImageSetToStateNode<eS2D_TitleState>::ApplySize(targetNode);
		}
		else if (IsWindowStateType(component))
		{
			return __TSI_ImageSetToStateNode<eS2D_WindowState>::ApplySize(targetNode);
		}
	}

	static const MkFloatRect* GetWorldAABR(eS2D_WindowPresetComponent component, const MkBaseWindowNode* targetNode)
	{
		if (IsBackgroundStateType(component))
		{
			return __TSI_ImageSetToStateNode<eS2D_BackgroundState>::GetWorldAABR(targetNode);
		}
		else if (IsTitleStateType(component))
		{
			return __TSI_ImageSetToStateNode<eS2D_TitleState>::GetWorldAABR(targetNode);
		}
		else if (IsWindowStateType(component))
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
		titleCaption = MKDEF_TITLE_BAR_SAMPLE_STRING;
	}
	hasCloseIcon = hasTitle;
	
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

void MkBaseWindowNode::CaptionDesc::SetString(const MkStr& msg)
{
	if (msg != m_Str)
	{
		m_Str = msg;
		m_NameList.Clear();
	}
}

void MkBaseWindowNode::CaptionDesc::SetNameList(const MkArray<MkHashStr>& nameList)
{
	if (nameList != m_NameList)
	{
		m_NameList = nameList;
		m_Str.Clear();
	}
}

void MkBaseWindowNode::CaptionDesc::__Load(const MkStr& defaultMsg, const MkArray<MkStr>& buffer)
{
	unsigned int count = buffer.GetSize();
	if (count == 0)
	{
		m_Str = defaultMsg;
	}
	else if (count == 1)
	{
		m_Str = buffer[0].Empty() ? defaultMsg : buffer[0];
	}
	else // count > 1
	{
		m_NameList.Reserve(count - 1);
		for (unsigned int i=1; i<count; ++i)
		{
			m_NameList.PushBack(buffer[i]);
		}
	}
}

void MkBaseWindowNode::CaptionDesc::__Save(MkArray<MkStr>& buffer) const
{
	buffer.Reserve(1 + m_NameList.GetSize());
	buffer.PushBack(m_Str);
	MK_INDEXING_LOOP(m_NameList, i)
	{
		buffer.PushBack(m_NameList[i]);
	}
}

void MkBaseWindowNode::CaptionDesc::__Check(const MkStr& defaultMsg)
{
	if (m_Str.Empty() && m_NameList.Empty())
	{
		m_Str = defaultMsg;
	}
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

MkBaseWindowNode* MkBaseWindowNode::GetRootWindow(void) const
{
	if (CheckRootWindow())
		return const_cast<MkBaseWindowNode*>(this);

	MkSceneNode* parentNode = m_ParentNodePtr;
	while (true)
	{
		if (parentNode == NULL)
			return NULL;

		if (parentNode->GetNodeType() >= eS2D_SNT_BaseWindowNode)
		{
			MkBaseWindowNode* targetNode = dynamic_cast<MkBaseWindowNode*>(parentNode);
			if (targetNode != NULL)
			{
				if (targetNode->CheckRootWindow())
					return targetNode;
			}
		}

		parentNode = parentNode->GetParentNode();
	}
}

void MkBaseWindowNode::Load(const MkDataNode& node)
{
	// enable
	bool enable = true;
	node.GetData(MkSceneNodeFamilyDefinition::BaseWindow::EnableKey, enable, 0);

	// preset
	MkStr presetThemeName;
	node.GetData(MkSceneNodeFamilyDefinition::BaseWindow::PresetThemeNameKey, presetThemeName, 0);
	m_PresetThemeName = presetThemeName;

	MkStr componentName;
	node.GetData(MkSceneNodeFamilyDefinition::BaseWindow::PresetComponentKey, componentName, 0);
	m_PresetComponentName = componentName;

	MkInt2 presetCompSize;
	node.GetData(MkSceneNodeFamilyDefinition::BaseWindow::PresetComponentSizeKey, presetCompSize, 0);
	m_PresetComponentSize = MkFloat2(static_cast<float>(presetCompSize.x), static_cast<float>(presetCompSize.y));

	// attribute
	m_Attribute.Clear();
	node.GetData(MkSceneNodeFamilyDefinition::BaseWindow::AttributeKey, m_Attribute.m_Field, 0);

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
	_ApplyBuildingTemplateToSave(node, MkSceneNodeFamilyDefinition::BaseWindow::TemplateName);

	node.SetData(MkSceneNodeFamilyDefinition::BaseWindow::EnableKey, m_Enable, 0);
	node.SetData(MkSceneNodeFamilyDefinition::BaseWindow::PresetThemeNameKey, m_PresetThemeName.GetString(), 0);
	node.SetData(MkSceneNodeFamilyDefinition::BaseWindow::PresetComponentKey, m_PresetComponentName.GetString(), 0);
	node.SetData(MkSceneNodeFamilyDefinition::BaseWindow::PresetComponentSizeKey, MkInt2(static_cast<int>(m_PresetComponentSize.x), static_cast<int>(m_PresetComponentSize.y)), 0);
	node.SetData(MkSceneNodeFamilyDefinition::BaseWindow::AttributeKey, m_Attribute.m_Field, 0);

	// MkSceneNode
	MkSceneNode::Save(node);
}

void MkBaseWindowNode::SetEnable(bool enable)
{
	eS2D_WindowState windowState = eS2D_WS_MaxWindowState;
	if (m_Enable && (!enable)) // on -> off
	{
		windowState = eS2D_WS_DisableState;
		_PushWindowEvent(MkSceneNodeFamilyDefinition::eDisable);
	}
	else if ((!m_Enable) && enable) // off -> on
	{
		windowState = eS2D_WS_DefaultState;
		_PushWindowEvent(MkSceneNodeFamilyDefinition::eEnable);
	}

	if (windowState != eS2D_WS_MaxWindowState)
	{
		m_Enable = enable;

		eS2D_WindowPresetComponent component = MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName);
		if (IsWindowStateType(component))
		{
			__TSI_ImageSetToStateNode<eS2D_WindowState>::SetState(windowState, this);
		}

		MK_INDEXING_LOOP(m_ChildWindows, i)
		{
			m_ChildWindows[i]->SetEnable(enable);
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

MkBaseWindowNode* MkBaseWindowNode::CreateBasicWindow(MkBaseWindowNode* targetWindow, const MkHashStr& nodeName, const BasicPresetWindowDesc& desc)
{
	BasicPresetWindowDesc d = desc;

	// free image BG일 경우 먼저 windowSize 설정
	if (d.hasFreeImageBG)
	{
		MkBaseTexturePtr texture;
		MK_TEXTURE_POOL.GetBitmapTexture(d.bgImageFilePath, texture);
		if (texture != NULL)
		{
			d.windowSize = texture->GetSubsetSize(d.bgImageSubsetName);
		}
	}

	const float MARGIN = MK_WR_PRESET.GetMargin();
	
	// window size 유효성 검사
	float minWinLength = MARGIN * 4.f;
	if (d.windowSize.x < minWinLength) { d.windowSize.x = minWinLength; }
	if (d.windowSize.y < minWinLength) { d.windowSize.x = minWinLength; }

	// 반환 할 노드
	MkBaseWindowNode* rootWindow = NULL;

	// title bar
	MkBaseWindowNode* titleWindow = NULL;
	if (d.hasTitle)
	{
		MkFloat2 titleCompSize(d.windowSize.x - MARGIN * 2.f, d.titleHeight);
		if (targetWindow == NULL)
		{
			titleWindow = __CreateWindowPreset(NULL, nodeName, d.themeName, eS2D_WPC_TitleWindow, titleCompSize);
		}
		else if (targetWindow->CreateWindowPreset(d.themeName, eS2D_WPC_TitleWindow, titleCompSize))
		{
			titleWindow = targetWindow;
		}
		
		if (titleWindow != NULL)
		{
			rootWindow = titleWindow;

			titleWindow->SetLocalPosition(MkFloat2(0.f, -titleWindow->GetPresetComponentSize().y));

			// window icon
			if (d.hasIcon)
			{
				titleWindow->SetPresetComponentIcon
					(L"WindowIcon", eRAP_LeftCenter, MkFloat2(MARGIN, 0.f), d.iconImageHeightOffset, d.iconImageFilePath, d.iconImageSubsetName);
			}

			// close icon
			if (d.hasCloseIcon)
			{
				MkBaseWindowNode* closeWindow = __CreateWindowPreset(titleWindow, L"CloseIcon", d.themeName, eS2D_WPC_CloseIcon, MkFloat2(0.f, 0.f));
				if (closeWindow != NULL)
				{
					MkFloat2 localPos =	MkFloatRect(titleWindow->GetPresetComponentSize()).GetSnapPosition
						(MkFloatRect(closeWindow->GetPresetComponentSize()), eRAP_RightCenter, MkFloat2(MARGIN, 0.f));
					closeWindow->SetLocalPosition(MkVec3(localPos.x, localPos.y, -MKDEF_BASE_WINDOW_DEPTH_GRID));
					closeWindow->SetAttribute(eIgnoreMovement, true);
				}
			}

			// title caption
			titleWindow->SetPresetComponentCaption(d.themeName, CaptionDesc(d.titleCaption));
		}
	}

	// background
	MkBaseWindowNode* backgroundWindow = NULL;
	if (titleWindow == NULL)
	{
		if (d.hasFreeImageBG)
		{
			if (targetWindow == NULL)
			{
				backgroundWindow = new MkBaseWindowNode(nodeName);
				if ((backgroundWindow != NULL) && (!backgroundWindow->CreateFreeImageBaseBackgroundWindow(d.bgImageFilePath, d.bgImageSubsetName)))
				{
					MK_DELETE(backgroundWindow);
				}
			}
			else if (targetWindow->CreateFreeImageBaseBackgroundWindow(d.bgImageFilePath, d.bgImageSubsetName))
			{
				backgroundWindow = targetWindow;
			}
		}
		else
		{
			if (targetWindow == NULL)
			{
				backgroundWindow = __CreateWindowPreset(NULL, nodeName, d.themeName, eS2D_WPC_BackgroundWindow, d.windowSize);
			}
			else if (targetWindow->CreateWindowPreset(d.themeName, eS2D_WPC_BackgroundWindow, d.windowSize))
			{
				backgroundWindow = targetWindow;
			}
		}
	}
	else
	{
		MkHashStr bgNodeName = L"Background";
		if (d.hasFreeImageBG)
		{
			backgroundWindow = new MkBaseWindowNode(bgNodeName);
			if (backgroundWindow != NULL)
			{
				if (backgroundWindow->CreateFreeImageBaseBackgroundWindow(d.bgImageFilePath, d.bgImageSubsetName))
				{
					titleWindow->AttachChildNode(backgroundWindow);
				}
				else
				{
					MK_DELETE(backgroundWindow);
				}
			}
		}
		else
		{
			backgroundWindow = __CreateWindowPreset(titleWindow, bgNodeName, d.themeName, eS2D_WPC_BackgroundWindow, d.windowSize);
		}
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
			heightOffset = titleWindow->GetPresetComponentSize().y;
		}
		backgroundWindow->SetLocalPosition(MkVec3(-MARGIN, heightOffset - backgroundWindow->GetPresetComponentSize().y + MARGIN, MKDEF_BASE_WINDOW_DEPTH_GRID));
		backgroundWindow->SetAttribute(MkBaseWindowNode::eIgnoreMovement, true);

		const MkFloat2 buttonCompSize(60.f, 18.f);

		// ok button
		MkBaseWindowNode* okWindow = NULL;
		if (d.hasOKButton)
		{
			okWindow = __CreateWindowPreset(backgroundWindow, L"OKButton", d.themeName, eS2D_WPC_OKButton, buttonCompSize);
			okWindow->SetAttribute(MkBaseWindowNode::eConfinedToParent, true);
			okWindow->SetPresetComponentCaption(d.themeName, CaptionDesc(MKDEF_OK_BTN_SAMPLE_STRING));
		}

		// cancel button
		MkBaseWindowNode* cancelWindow = NULL;
		if (d.hasCancelButton)
		{
			cancelWindow = __CreateWindowPreset(backgroundWindow, L"CancelButton", d.themeName, eS2D_WPC_CancelButton, buttonCompSize);
			cancelWindow->SetAttribute(MkBaseWindowNode::eConfinedToParent, true);
			cancelWindow->SetPresetComponentCaption(d.themeName, CaptionDesc(MKDEF_CANCEL_BTN_SAMPLE_STRING));
		}

		if ((okWindow != NULL) && (cancelWindow == NULL))
		{
			float posX = (backgroundWindow->GetPresetComponentSize().x - okWindow->GetPresetComponentSize().x) / 2.f;
			okWindow->SetLocalPosition(MkVec3(posX, MARGIN, -MKDEF_BASE_WINDOW_DEPTH_GRID)); // 하단 중앙 정렬
		}
		else if ((okWindow == NULL) && (cancelWindow != NULL))
		{
			float posX = (backgroundWindow->GetPresetComponentSize().x - cancelWindow->GetPresetComponentSize().x) / 2.f;
			cancelWindow->SetLocalPosition(MkVec3(posX, MARGIN, -MKDEF_BASE_WINDOW_DEPTH_GRID)); // 하단 중앙 정렬
		}
		else if ((okWindow != NULL) && (cancelWindow != NULL))
		{
			float blank = (backgroundWindow->GetPresetComponentSize().x - okWindow->GetPresetComponentSize().x * 2.f) / 3.f;
			okWindow->SetLocalPosition(MkVec3(blank, MARGIN, -MKDEF_BASE_WINDOW_DEPTH_GRID));
			cancelWindow->SetLocalPosition(MkVec3(blank * 2.f + okWindow->GetPresetComponentSize().x, MARGIN, -MKDEF_BASE_WINDOW_DEPTH_GRID));
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

bool MkBaseWindowNode::CreateWindowPreset(const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& componentSize)
{
	if (m_PresetComponentName.Empty())
	{
		const MkArray<MkHashStr>& imageSets = MK_WR_PRESET.GetWindowTypeImageSet(themeName, component);
		if (!imageSets.Empty())
		{
			m_PresetThemeName = themeName;
			m_PresetComponentSize = componentSize;
			m_PresetComponentName = MkWindowPreset::GetWindowPresetComponentKeyword(component);

			return __TSI_ImageSetToComponentNode::ApplyImageSetAndSize(component, imageSets, this);
		}
	}
	return false;
}

bool MkBaseWindowNode::CreateFreeImageBaseBackgroundWindow(const MkPathName& imagePath, const MkHashStr& subsetName)
{
	if (m_PresetComponentName.Empty())
	{
		m_PresetThemeName.Clear();
		m_PresetComponentName = MkWindowPreset::GetWindowPresetComponentKeyword(eS2D_WPC_BackgroundWindow);

		return __TSI_ImageSetToStateNode<eS2D_BackgroundState>::ApplyFreeImageToState(eS2D_BS_DefaultState, imagePath, subsetName, this);
	}
	return false;
}

bool MkBaseWindowNode::CreateFreeImageBaseButtonWindow(const MkPathName& imagePath, const MkArray<MkHashStr>& subsetNames)
{
	if (m_PresetComponentName.Empty() && (subsetNames.GetSize() == static_cast<unsigned int>(eS2D_WS_MaxWindowState)))
	{
		m_PresetThemeName.Clear();
		m_PresetComponentName = MkWindowPreset::GetWindowPresetComponentKeyword(eS2D_WPC_NormalButton);

		for (int i=eS2D_WS_DefaultState; i<eS2D_WS_MaxWindowState; ++i)
		{
			if (!__TSI_ImageSetToStateNode<eS2D_WindowState>::ApplyFreeImageToState(static_cast<eS2D_WindowState>(i), imagePath, subsetNames[i], this))
				return false;
		}
		return true;
	}
	return false;
}

void MkBaseWindowNode::SetPresetThemeName(const MkHashStr& themeName)
{
	if ((!m_PresetThemeName.Empty()) && (themeName != m_PresetThemeName)) // 테마가 다르면
	{
		if (!m_PresetComponentName.Empty())
		{
			eS2D_WindowPresetComponent component = MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName);
			const MkArray<MkHashStr>& newImageSets = MK_WR_PRESET.GetWindowTypeImageSet(themeName, component);
			const MkArray<MkHashStr>& oldImageSets = MK_WR_PRESET.GetWindowTypeImageSet(m_PresetThemeName, component);

			if ((!newImageSets.Empty()) && (newImageSets != oldImageSets) && // 테마명은 다르더라도 image set은 같을 수 있으므로 비교 필요
				__TSI_ImageSetToComponentNode::ApplyImageSetAndSize(component, newImageSets, this))
			{
				// 위의 ApplyImageSetAndSize()을 거치면 visible 상태가 초기화 되버리기 때문에 disable일 경우 반영해 주어야 함
				if ((!m_Enable) && IsWindowStateType(component))
				{
					__TSI_ImageSetToStateNode<eS2D_WindowState>::SetState(eS2D_WS_DisableState, this);
				}
			}
		}

		// caption
		if (ExistSRect(COMPONENT_HIGHLIGHT_TAG_NAME))
		{
			MkSRect* srect = GetSRect(COMPONENT_HIGHLIGHT_TAG_NAME);
			if (srect->CheckFocedFontTypeAndState())
			{
				srect->SetFocedFontState(MK_WR_PRESET.GetHighlightThemeFontState(themeName));
			}
		}
		if (ExistSRect(COMPONENT_NORMAL_TAG_NAME))
		{
			MkSRect* srect = GetSRect(COMPONENT_NORMAL_TAG_NAME);
			if (srect->CheckFocedFontTypeAndState())
			{
				srect->SetFocedFontState(MK_WR_PRESET.GetNormalThemeFontState(themeName));
			}
		}

		m_PresetThemeName = themeName;
	}

	MK_INDEXING_LOOP(m_ChildWindows, i)
	{
		m_ChildWindows[i]->SetPresetThemeName(themeName);
	}
}

void MkBaseWindowNode::SetPresetComponentSize(const MkFloat2& componentSize)
{
	if ((!m_PresetComponentName.Empty()) && (componentSize != m_PresetComponentSize))
	{
		m_PresetComponentSize = componentSize;
		__TSI_ImageSetToComponentNode::ApplySize(MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName), this);
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
			iconRect->SetFocedFontState(MkHashStr::NullHash);
			iconRect->SetTexture(imagePath, subsetName);
			iconRect->AlignRect(GetPresetComponentSize(), alignment, border, heightOffset, -MKDEF_BASE_WINDOW_DEPTH_GRID);
			return true;
		}
	}
	return false;
}

bool MkBaseWindowNode::SetPresetComponentIcon
(const MkHashStr& iconName, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset, const MkHashStr& forcedState, const CaptionDesc& captionDesc)
{
	if (!m_PresetComponentName.Empty())
	{
		MkSRect* iconRect = ExistSRect(iconName) ? GetSRect(iconName) : CreateSRect(iconName);
		if (iconRect != NULL)
		{
			iconRect->SetFocedFontState(forcedState);

			bool ok = false;
			if (!captionDesc.GetString().Empty())
			{
				ok = iconRect->SetDecoString(captionDesc.GetString());
			}
			else if (!captionDesc.GetNameList().Empty())
			{
				ok = iconRect->SetDecoString(captionDesc.GetNameList());
			}

			if (ok)
			{
				iconRect->AlignRect(GetPresetComponentSize(), alignment, border, heightOffset, -MKDEF_BASE_WINDOW_DEPTH_GRID);
			}
			else
			{
				DeleteSRect(iconName);
			}
			return ok;
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
		if (IsTitleStateType(component) || IsWindowStateType(component))
		{
			return SetPresetComponentIcon((highlight) ? COMPONENT_HIGHLIGHT_TAG_NAME : COMPONENT_NORMAL_TAG_NAME, alignment, border, 0.f, imagePath, subsetName);
		}
	}
	return false;
}

bool MkBaseWindowNode::SetPresetComponentIcon(bool highlight, eRectAlignmentPosition alignment, const MkFloat2& border, const CaptionDesc& captionDesc)
{
	if (!m_PresetComponentName.Empty())
	{
		eS2D_WindowPresetComponent component = MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName);
		if (IsTitleStateType(component) || IsWindowStateType(component))
		{
			return SetPresetComponentIcon((highlight) ? COMPONENT_HIGHLIGHT_TAG_NAME : COMPONENT_NORMAL_TAG_NAME, alignment, border, 0.f,
				(highlight) ? MK_WR_PRESET.GetHighlightThemeFontState(m_PresetThemeName) : MK_WR_PRESET.GetNormalThemeFontState(m_PresetThemeName), captionDesc);
		}
	}
	return false;
}

bool MkBaseWindowNode::SetPresetComponentCaption(const MkHashStr& themeName, const CaptionDesc& captionDesc, eRectAlignmentPosition alignment, const MkFloat2& border)
{
	bool hOK = SetPresetComponentIcon(true, alignment, border, captionDesc);
	bool nOK = SetPresetComponentIcon(false, alignment, border, captionDesc);
	return (hOK && nOK);
}

eS2D_WindowPresetComponent MkBaseWindowNode::GetPresetComponentType(void) const
{
	return MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName);
}

bool MkBaseWindowNode::InputEventMousePress(unsigned int button, const MkFloat2& position)
{
	if (CheckCursorHitCondition(position))
	{
		switch (button)
		{
		case 0: _PushWindowEvent(MkSceneNodeFamilyDefinition::eCursorLeftPress); break;
		case 1: _PushWindowEvent(MkSceneNodeFamilyDefinition::eCursorMiddlePress); break;
		case 2: _PushWindowEvent(MkSceneNodeFamilyDefinition::eCursorRightPress); break;
		}

		if (HitEventMousePress(button, position))
			return true;
	}
	
	MkArray<MkBaseWindowNode*> buffer;
	if (_CollectUpdatableWindowNodes(position, buffer))
	{
		MK_INDEXING_LOOP(buffer, i)
		{
			if (!buffer[i]->GetAttribute(eIgnoreInputEvent))
			{
				if (buffer[i]->InputEventMousePress(button, position))
					return true;
			}
		}
	}
	return false;
}

bool MkBaseWindowNode::InputEventMouseRelease(unsigned int button, const MkFloat2& position)
{
	if (CheckCursorHitCondition(position))
	{
		switch (button)
		{
		case 0: _PushWindowEvent(MkSceneNodeFamilyDefinition::eCursorLeftRelease); break;
		case 1: _PushWindowEvent(MkSceneNodeFamilyDefinition::eCursorMiddleRelease); break;
		case 2: _PushWindowEvent(MkSceneNodeFamilyDefinition::eCursorRightRelease); break;
		}

		if (button == 0)
		{
			if ((!MK_WIN_EVENT_MGR.GetEditMode()) && (MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName) == eS2D_WPC_CloseIcon))
			{
				MK_WIN_EVENT_MGR.DeactivateWindow(GetRootWindow()->GetNodeName());
				return true;
			}

			if (static_cast<MkBaseWindowNode*>(this) == MK_WIN_EVENT_MGR.GetFrontHitWindow())
			{
				_PushWindowEvent(MkSceneNodeFamilyDefinition::eCursorLeftClick);
			}
		}

		if (HitEventMouseRelease(button, position))
			return true;
	}
	
	MkArray<MkBaseWindowNode*> buffer;
	if (_CollectUpdatableWindowNodes(position, buffer))
	{
		MK_INDEXING_LOOP(buffer, i)
		{
			if (!buffer[i]->GetAttribute(eIgnoreInputEvent))
			{
				if (buffer[i]->InputEventMouseRelease(button, position))
					return true;
			}
		}
	}
	return false;
}

bool MkBaseWindowNode::InputEventMouseDoubleClick(unsigned int button, const MkFloat2& position)
{
	if (CheckCursorHitCondition(position))
	{
		switch (button)
		{
		case 0: _PushWindowEvent(MkSceneNodeFamilyDefinition::eCursorLeftDoubleClick); break;
		case 1: _PushWindowEvent(MkSceneNodeFamilyDefinition::eCursorMiddleDoubleClick); break;
		case 2: _PushWindowEvent(MkSceneNodeFamilyDefinition::eCursorRightDoubleClick); break;
		}

		if (HitEventMouseDoubleClick(button, position))
			return true;
	}

	MkArray<MkBaseWindowNode*> buffer;
	if (_CollectUpdatableWindowNodes(position, buffer))
	{
		MK_INDEXING_LOOP(buffer, i)
		{
			if (!buffer[i]->GetAttribute(eIgnoreInputEvent))
			{
				if (buffer[i]->InputEventMouseDoubleClick(button, position))
					return true;
			}
		}
	}
	return false;
}

bool MkBaseWindowNode::InputEventMouseWheelMove(int delta, const MkFloat2& position)
{
	if (CheckWheelMoveCondition(position))
	{
		_PushWindowEvent((delta < 0) ? MkSceneNodeFamilyDefinition::eCursorWheelDecrease : MkSceneNodeFamilyDefinition::eCursorWheelIncrease);

		if (HitEventMouseWheelMove(delta, position))
			return true;
	}

	MkArray<MkBaseWindowNode*> buffer;
	if (_CollectUpdatableWindowNodes(position, buffer))
	{
		MK_INDEXING_LOOP(buffer, i)
		{
			if (!buffer[i]->GetAttribute(eIgnoreInputEvent))
			{
				if (buffer[i]->InputEventMouseWheelMove(delta, position))
					return true;
			}
		}
	}
	return false;
}

void MkBaseWindowNode::InputEventMouseMove(bool inside, bool (&btnPushing)[3], const MkFloat2& position)
{
	if (inside)
	{
		inside = GetWorldAABR().CheckGridIntersection(position);
	}

	// eS2D_WindowState가 적용된 노드면 eS2D_WS_DefaultState, eS2D_WS_OnClickState, eS2D_WS_OnCursorState 중 하나를 지정
	if (IsWindowStateType(MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName)))
	{
		eS2D_WindowState windowState = eS2D_WS_DefaultState;
		if (inside && GetWindowRect().CheckGridIntersection(position))
		{
			windowState = (btnPushing[0] && (static_cast<MkBaseWindowNode*>(this) == MK_WIN_EVENT_MGR.GetFrontHitWindow())) ? eS2D_WS_OnClickState : eS2D_WS_OnCursorState;
		}

		__TSI_ImageSetToStateNode<eS2D_WindowState>::SetState(windowState, this);
	}

	MK_INDEXING_LOOP(m_ChildWindows, i)
	{
		MkBaseWindowNode* currNode = m_ChildWindows[i];
		if (currNode->GetVisible() && currNode->GetEnable() && (!currNode->GetAttribute(eIgnoreInputEvent)))
		{
			currNode->InputEventMouseMove(inside, btnPushing, position);
		}
	}
}

bool MkBaseWindowNode::CheckCursorHitCondition(const MkFloat2& position) const
{
	eS2D_WindowPresetComponent component = MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName);
	return ((IsTitleStateType(component) || IsWindowStateType(component)) && GetWindowRect().CheckGridIntersection(position));
}

void MkBaseWindowNode::OnFocus(void)
{
	if (m_PresetComponentName == MkWindowPreset::GetWindowPresetComponentKeyword(eS2D_WPC_TitleWindow))
	{
		__TSI_ImageSetToStateNode<eS2D_TitleState>::SetState(eS2D_TS_OnFocusState, this);
	}

	MK_INDEXING_LOOP(m_ChildWindows, i)
	{
		m_ChildWindows[i]->OnFocus();
	}
}

void MkBaseWindowNode::LostFocus(void)
{
	if (m_PresetComponentName == MkWindowPreset::GetWindowPresetComponentKeyword(eS2D_WPC_TitleWindow))
	{
		__TSI_ImageSetToStateNode<eS2D_TitleState>::SetState(eS2D_TS_LostFocusState, this);
	}

	MK_INDEXING_LOOP(m_ChildWindows, i)
	{
		m_ChildWindows[i]->LostFocus();
	}
}

bool MkBaseWindowNode::AttachChildNode(MkSceneNode* childNode)
{
	bool ok = MkSceneNode::AttachChildNode(childNode);
	if (ok && (childNode->GetNodeType() >= eS2D_SNT_BaseWindowNode))
	{
		MkBaseWindowNode* childWindow = dynamic_cast<MkBaseWindowNode*>(childNode);
		if ((childWindow != NULL) && (m_ChildWindows.FindFirstInclusion(MkArraySection(0), childWindow) == MKDEF_ARRAY_ERROR))
		{
			m_ChildWindows.PushBack(childWindow);
		}
	}
	return ok;
}

bool MkBaseWindowNode::DetachChildNode(const MkHashStr& childNodeName)
{
	if (m_ChildrenNode.Exist(childNodeName))
	{
		MkSceneNode* childNode = m_ChildrenNode[childNodeName];
		if ((childNode != NULL) && (childNode->GetNodeType() >= eS2D_SNT_BaseWindowNode))
		{
			MkBaseWindowNode* childWindow = dynamic_cast<MkBaseWindowNode*>(childNode);
			m_ChildWindows.EraseFirstInclusion(MkArraySection(0), childWindow);
		}
	}
	
	return MkSceneNode::DetachChildNode(childNodeName);
}

void MkBaseWindowNode::Clear(void)
{
	m_ChildWindows.Clear();
	MkSceneNode::Clear();
}

MkBaseWindowNode::MkBaseWindowNode(const MkHashStr& name) : MkSceneNode(name)
{
	m_RootWindow = false;
	m_Enable = true;
}

//------------------------------------------------------------------------------------------------//

void MkBaseWindowNode::__GenerateBuildingTemplate(void)
{
	MkDataNode node;
	MkDataNode* tNode = node.CreateChildNode(MkSceneNodeFamilyDefinition::BaseWindow::TemplateName);
	MK_CHECK(tNode != NULL, MkSceneNodeFamilyDefinition::BaseWindow::TemplateName.GetString() + L" template node alloc 실패")
		return;

	tNode->ApplyTemplate(MKDEF_S2D_BT_SCENENODE_TEMPLATE_NAME); // MkSceneNode의 template 적용

	tNode->CreateUnit(MkSceneNodeFamilyDefinition::BaseWindow::EnableKey, true);
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::BaseWindow::PresetThemeNameKey, MkStr::Null);
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::BaseWindow::PresetComponentKey, MkStr::Null);
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::BaseWindow::PresetComponentSizeKey, MkInt2(0, 0));
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::BaseWindow::AttributeKey, static_cast<unsigned int>(0));

	tNode->DeclareToTemplate(true);
}

MkBaseWindowNode* MkBaseWindowNode::__CreateWindowPreset
(MkSceneNode* parentNode, const MkHashStr& nodeName, const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& componentSize)
{
	if ((parentNode == NULL) || (!parentNode->ChildExist(nodeName)))
	{
		MkBaseWindowNode* targetNode = new MkBaseWindowNode(nodeName);
		if (targetNode != NULL)
		{
			if (targetNode->CreateWindowPreset(themeName, component, componentSize))
			{
				if (parentNode != NULL)
				{
					parentNode->AttachChildNode(targetNode);
				}
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

MkBaseWindowNode* MkBaseWindowNode::__GetFrontHitWindow(const MkFloat2& position)
{
	if (!GetVisible())
		return NULL;

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

void MkBaseWindowNode::__GetHitWindows(const MkFloat2& position, MkPairArray<float, MkBaseWindowNode*>& hitWindows)
{
	if (GetWindowRect().CheckGridIntersection(position))
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

void MkBaseWindowNode::__ConsumeWindowEvent(void)
{
	if (!m_WindowEvents.Empty())
	{
		// leaf -> root
		MK_INDEXING_LOOP(m_ChildWindows, i)
		{
			MkBaseWindowNode* currNode = m_ChildWindows[i];
			if (currNode->GetVisible() && currNode->GetEnable())
			{
				currNode->__ConsumeWindowEvent();
			}
		}

		MK_INDEXING_LOOP(m_WindowEvents, i)
		{
			UseWindowEvent(m_WindowEvents[i]);
		}

#ifdef MKDEF_S2D_DEBUG_SHOW_WINDOW_EVENT
		if (CheckRootWindow())
		{
			if (!m_WindowEvents.Empty())
			{
				MK_DEV_PANEL.MsgToLog(L"> WindowEvent > " + GetNodeName().GetString());
			}
			MK_INDEXING_LOOP(m_WindowEvents, i)
			{
				WindowEvent& evt = m_WindowEvents[i];
				switch (evt.type)
				{
				// MkBaseWindowNode
				case MkSceneNodeFamilyDefinition::eEnable: MK_DEV_PANEL.MsgToLog(L"   eEnable : " + evt.node->GetNodeName().GetString()); break;
				case MkSceneNodeFamilyDefinition::eDisable: MK_DEV_PANEL.MsgToLog(L"   eDisable : " + evt.node->GetNodeName().GetString()); break;
				case MkSceneNodeFamilyDefinition::eCursorLeftPress: MK_DEV_PANEL.MsgToLog(L"   eCursorLeftPress : " + evt.node->GetNodeName().GetString()); break;
				case MkSceneNodeFamilyDefinition::eCursorMiddlePress: MK_DEV_PANEL.MsgToLog(L"   eCursorMiddlePress : " + evt.node->GetNodeName().GetString()); break;
				case MkSceneNodeFamilyDefinition::eCursorRightPress: MK_DEV_PANEL.MsgToLog(L"   eCursorRightPress : " + evt.node->GetNodeName().GetString()); break;
				case MkSceneNodeFamilyDefinition::eCursorLeftRelease: MK_DEV_PANEL.MsgToLog(L"   eCursorLeftRelease : " + evt.node->GetNodeName().GetString()); break;
				case MkSceneNodeFamilyDefinition::eCursorMiddleRelease: MK_DEV_PANEL.MsgToLog(L"   eCursorMiddleRelease : " + evt.node->GetNodeName().GetString()); break;
				case MkSceneNodeFamilyDefinition::eCursorRightRelease: MK_DEV_PANEL.MsgToLog(L"   eCursorRightRelease : " + evt.node->GetNodeName().GetString()); break;
				case MkSceneNodeFamilyDefinition::eCursorLeftClick: MK_DEV_PANEL.MsgToLog(L"   eCursorLeftClick : " + evt.node->GetNodeName().GetString()); break;
				case MkSceneNodeFamilyDefinition::eCursorLeftDoubleClick: MK_DEV_PANEL.MsgToLog(L"   eCursorLeftDoubleClick : " + evt.node->GetNodeName().GetString()); break;
				case MkSceneNodeFamilyDefinition::eCursorMiddleDoubleClick: MK_DEV_PANEL.MsgToLog(L"   eCursorMiddleDoubleClick : " + evt.node->GetNodeName().GetString()); break;
				case MkSceneNodeFamilyDefinition::eCursorRightDoubleClick: MK_DEV_PANEL.MsgToLog(L"   eCursorRightDoubleClick : " + evt.node->GetNodeName().GetString()); break;
				case MkSceneNodeFamilyDefinition::eCursorWheelDecrease: MK_DEV_PANEL.MsgToLog(L"   eCursorWheelDecrease : " + evt.node->GetNodeName().GetString()); break;
				case MkSceneNodeFamilyDefinition::eCursorWheelIncrease: MK_DEV_PANEL.MsgToLog(L"   eCursorWheelIncrease : " + evt.node->GetNodeName().GetString()); break;

				// MkSpreadButtonNode
				case MkSceneNodeFamilyDefinition::eOpenList: MK_DEV_PANEL.MsgToLog(L"   eOpenList : " + evt.node->GetNodeName().GetString()); break;
				case MkSceneNodeFamilyDefinition::eSetTargetItem: MK_DEV_PANEL.MsgToLog(L"   eSetTargetItem : " + evt.node->GetNodeName().GetString()); break;

				// MkCheckButtonNode
				case MkSceneNodeFamilyDefinition::eCheckOn: MK_DEV_PANEL.MsgToLog(L"   eCheckOn : " + evt.node->GetNodeName().GetString()); break;
				case MkSceneNodeFamilyDefinition::eCheckOff: MK_DEV_PANEL.MsgToLog(L"   eCheckOff : " + evt.node->GetNodeName().GetString()); break;
				}
			}
		}
#endif

		m_WindowEvents.Flush();
	}
}

void MkBaseWindowNode::__BuildInformationTree(MkBaseWindowNode* targetParentNode, unsigned int offset)
{
	if (targetParentNode != NULL)
	{
		MkBaseWindowNode* targetNode = __CreateWindowPreset
			(targetParentNode, GetNodeName(), MK_WR_PRESET.GetDefaultThemeName(), eS2D_WPC_SelectionButton, MKDEF_S2D_NODE_SEL_LINE_SIZE);

		if (targetNode != NULL)
		{
			// fill info
			MkStr windowType;
			switch (GetNodeType())
			{
			case eS2D_SNT_SceneNode: MkDecoStr::InsertFontStateTag(MK_FONT_MGR.LightGrayFS(), L"[SceneNode]", windowType); break;
			case eS2D_SNT_BaseWindowNode: MkDecoStr::InsertFontStateTag(MK_FONT_MGR.YellowFS(), L"[BaseWindow]", windowType); break;
			case eS2D_SNT_SpreadButtonNode: MkDecoStr::InsertFontStateTag(MK_FONT_MGR.CianFS(), L"[SpreadBtn]", windowType); break;
			case eS2D_SNT_CheckButtonNode: MkDecoStr::InsertFontStateTag(MK_FONT_MGR.PinkFS(), L"[CheckBtn]", windowType); break;
			case eS2D_SNT_ScrollBarNode: MkDecoStr::InsertFontStateTag(MK_FONT_MGR.OrangeFS(), L"[ScrollBar]", windowType); break;
			case eS2D_SNT_EditBoxNode: MkDecoStr::InsertFontStateTag(MK_FONT_MGR.GreenFS(), L"[EditBox]", windowType); break;
			}

			MkStr header;
			MkDecoStr::InsertFontTypeTag(MK_FONT_MGR.DSF(), windowType, header);

			MkStr compStr, compType;
			compStr += L"[";
			compStr += m_PresetComponentName.Empty() ? L"--" : m_PresetComponentName.GetString();
			compStr += L"]";

			eS2D_WindowPresetComponent component = MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName);
			if (IsBackgroundStateType(component))
			{
				MkDecoStr::InsertFontStateTag(MK_FONT_MGR.LightGrayFS(), compStr, compType);
			}
			else if (IsTitleStateType(component))
			{
				MkDecoStr::InsertFontStateTag(MK_FONT_MGR.RedFS(), compStr, compType);
			}
			else if (IsWindowStateType(component))
			{
				MkDecoStr::InsertFontStateTag(MK_FONT_MGR.GreenFS(), compStr, compType);
			}
			else
			{
				MkDecoStr::InsertFontStateTag(MK_FONT_MGR.DarkGrayFS(), compStr, compType);
			}

			MkStr nameBuf;
			MkDecoStr::InsertFontStateTag(MK_FONT_MGR.WhiteFS(), L" " + GetNodeName().GetString(), nameBuf);

			MkStr text;
			text.Reserve(header.GetSize() + compType.GetSize() + nameBuf.GetSize());
			text += header;
			text += compType;
			text += nameBuf;

			CaptionDesc captionDesc;
			captionDesc.SetString(text);

			targetNode->SetPresetComponentIcon(COMPONENT_HIGHLIGHT_TAG_NAME, eRAP_LeftCenter, MkFloat2(MK_WR_PRESET.GetMargin(), 0.f), 0.f,
				MkHashStr::NullHash, captionDesc);

			targetNode->SetPresetComponentIcon(COMPONENT_NORMAL_TAG_NAME, eRAP_LeftCenter, MkFloat2(MK_WR_PRESET.GetMargin(), 0.f), 0.f,
				MkHashStr::NullHash, captionDesc);

			if (!GetVisible())
			{
				targetNode->SetAlpha(0.5f);
			}

			targetNode->SetLocalPosition(MkFloat2(40.f, -targetNode->GetPresetComponentSize().y * static_cast<float>(offset)));

			int totalChildCnt = 0;
			MK_INDEXING_LOOP(m_ChildWindows, i)
			{
				if (i > 0)
				{
					totalChildCnt += m_ChildWindows[i-1]->__CountTotalWindowBasedChildren();
				}

				m_ChildWindows[i]->__BuildInformationTree(targetNode, i + 1 + totalChildCnt);
			}
		}
	}
	/*
	const MkHashStr& nodeName = GetNodeName();
	const MkVec3& wp = GetWorldPosition();
	const MkVec3& lp = GetLocalPosition();
	*/

	/*
	eS2D_SNT_SceneNode = 0, // MkSceneNode
	eS2D_SNT_BaseWindowNode, // MkBaseWindowNode
	eS2D_SNT_SpreadButtonNode, // MkSpreadButtonNode
	eS2D_SNT_CheckButtonNode, // MkCheckButtonNode
	eS2D_SNT_ScrollBarNode, // MkScrollBarNode
	eS2D_SNT_EditBoxNode // MkEditBoxNode
	eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_BaseWindowNode; }
	*/

	/* component
	bool m_RootWindow;
	bool m_Enable;
	MkHashStr m_PresetThemeName;
	MkHashStr m_PresetComponentName;
	MkFloat2 m_PresetComponentSize;
	MkBitFieldDW m_Attribute;
	*/
}

unsigned int MkBaseWindowNode::__CountTotalWindowBasedChildren(void) const
{
	unsigned int cnt = m_ChildWindows.GetSize();
	MK_INDEXING_LOOP(m_ChildWindows, i)
	{
		cnt += m_ChildWindows[i]->__CountTotalWindowBasedChildren();
	}
	return cnt;
}

bool MkBaseWindowNode::_CollectUpdatableWindowNodes(const MkFloat2& position, MkArray<MkBaseWindowNode*>& buffer)
{
	if (!m_ChildWindows.Empty())
	{
		buffer.Reserve(m_ChildWindows.GetSize());

		MK_INDEXING_LOOP(m_ChildWindows, i)
		{
			MkBaseWindowNode* winNode = m_ChildWindows[i];
			if (winNode->GetVisible() && winNode->GetEnable() && winNode->GetWorldAABR().CheckGridIntersection(position))
			{
				buffer.PushBack(winNode);
			}
		}
	}
	return (!buffer.Empty());
}

void MkBaseWindowNode::_PushWindowEvent(MkSceneNodeFamilyDefinition::eWindowEvent type)
{
	WindowEvent evt(type, this);

	// 부모부터 루트 윈도우까지 이벤트를 통지함
	MkSceneNode* parentNode = m_ParentNodePtr;
	while (true)
	{
		if (parentNode == NULL)
			break;

		if (parentNode->GetNodeType() >= eS2D_SNT_BaseWindowNode)
		{
			MkBaseWindowNode* targetNode = dynamic_cast<MkBaseWindowNode*>(parentNode);
			if (targetNode != NULL)
			{
				targetNode->__PushEvent(evt);

				if (targetNode->CheckRootWindow())
					break;
			}
		}

		parentNode = parentNode->GetParentNode();
	}
}

//------------------------------------------------------------------------------------------------//