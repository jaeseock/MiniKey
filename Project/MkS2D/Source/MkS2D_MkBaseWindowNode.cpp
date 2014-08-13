
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkTexturePool.h"
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkCursorManager.h"
#include "MkS2D_MkBaseWindowNode.h"

#include "MkS2D_MkWindowEventManager.h"


#define MKDEF_TITLE_BAR_SAMPLE_STRING L"타 이 틀"
#define MKDEF_OK_BTN_SAMPLE_STRING L"확 인"
#define MKDEF_CANCEL_BTN_SAMPLE_STRING L"취 소"

//------------------------------------------------------------------------------------------------//

class __TSI_SceneNodeOp
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
class __TSI_WindowNodeOp
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
			const MkHashStr hTag = MKDEF_S2D_BASE_WND_HIGHLIGHT_CAP_TAG_NAME;
			const MkHashStr nTag = MKDEF_S2D_BASE_WND_NORMAL_CAP_TAG_NAME;
			bool onHighlight = (state == static_cast<DataType>(MkWindowPresetStateInterface<DataType>::GetHighlight()));

			if (targetNode->ExistSRect(hTag))
			{
				targetNode->GetSRect(hTag)->SetVisible(onHighlight);
			}
			if (targetNode->ExistSRect(nTag))
			{
				targetNode->GetSRect(nTag)->SetVisible(!onHighlight);
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
				if (__TSI_SceneNodeOp::ApplyImageSetAndSize(imageSets[currState], sizeIn, stateNode, sizeOut))
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
			if (__TSI_SceneNodeOp::ApplyFreeImage(imagePath, subsetName, stateNode, sizeOut))
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
				__TSI_SceneNodeOp::ApplySize(sizeIn, targetNode->GetChildNode(stateKeyword), sizeOut);
				if (currState == beginState)
				{
					targetNode->__SetPresetComponentSize(sizeOut);
				}

				currState = static_cast<DataType>(currState + 1);
			}
		}
	}

	static void SetState(DataType state, MkBaseWindowNode* targetNode)
	{
		if (targetNode != NULL)
		{
			targetNode->__SetCurrentComponentState(static_cast<int>(state));

			_SetState(state, targetNode);
			_SetHighlight(state, targetNode);
		}
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

class __TSI_ComponentNodeOp
{
public:
	static bool ApplyImageSetAndSize(const MkArray<MkHashStr>& imageSets, MkBaseWindowNode* targetNode)
	{
		eS2D_WindowPresetComponent component = targetNode->GetPresetComponentType();
		if (IsBackgroundStateType(component))
		{
			return __TSI_WindowNodeOp<eS2D_BackgroundState>::ApplyImageSetAndSize(imageSets, targetNode);
		}
		else if (IsTitleStateType(component))
		{
			targetNode->SetAttribute(MkBaseWindowNode::eShowActionCursor, true);
			return __TSI_WindowNodeOp<eS2D_TitleState>::ApplyImageSetAndSize(imageSets, targetNode);
		}
		else if (IsWindowStateType(component))
		{
			targetNode->SetAttribute(MkBaseWindowNode::eShowActionCursor, true);
			return __TSI_WindowNodeOp<eS2D_WindowState>::ApplyImageSetAndSize(imageSets, targetNode);
		}
		return false;
	}

	static void ApplySize(MkBaseWindowNode* targetNode)
	{
		eS2D_WindowPresetComponent component = targetNode->GetPresetComponentType();
		if (IsBackgroundStateType(component))
		{
			return __TSI_WindowNodeOp<eS2D_BackgroundState>::ApplySize(targetNode);
		}
		else if (IsTitleStateType(component))
		{
			return __TSI_WindowNodeOp<eS2D_TitleState>::ApplySize(targetNode);
		}
		else if (IsWindowStateType(component))
		{
			return __TSI_WindowNodeOp<eS2D_WindowState>::ApplySize(targetNode);
		}
	}

	static const MkFloatRect* GetWorldAABR(const MkBaseWindowNode* targetNode)
	{
		eS2D_WindowPresetComponent component = targetNode->GetPresetComponentType();
		if (IsBackgroundStateType(component))
		{
			return __TSI_WindowNodeOp<eS2D_BackgroundState>::GetWorldAABR(targetNode);
		}
		else if (IsTitleStateType(component))
		{
			return __TSI_WindowNodeOp<eS2D_TitleState>::GetWorldAABR(targetNode);
		}
		else if (IsWindowStateType(component))
		{
			return __TSI_WindowNodeOp<eS2D_WindowState>::GetWorldAABR(targetNode);
		}
		return NULL;
	}
};

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

void MkBaseWindowNode::BasicPresetWindowDesc::SetStandardDesc(const MkHashStr& themeName, bool hasTitle)
{
	this->themeName = themeName;
	dragMovement = true;

	this->hasTitle = hasTitle;
	if (hasTitle)
	{
		titleHeight = 20.f;
		titleType = eS2D_WPC_NormalTitle;
		iconImageFilePath = MK_WR_PRESET.GetSystemImageFilePath();
		iconImageSubsetName = MK_WR_PRESET.GetWindowIconSampleSubsetName();
		iconImageHeightOffset = 0.f;
		titleCaption.SetString(MKDEF_TITLE_BAR_SAMPLE_STRING);
	}
	hasCloseButton = hasTitle;
	
	hasFreeImageBG = true;
	bgImageFilePath = MK_WR_PRESET.GetSystemImageFilePath();
	bgImageSubsetName = MK_WR_PRESET.GetWindowBackgroundSampleSubsetName();

	hasOKButton = true;
	okBtnCaption.SetString(MKDEF_OK_BTN_SAMPLE_STRING);

	hasCancelButton = true;
	cancelBtnCaption.SetString(MKDEF_CANCEL_BTN_SAMPLE_STRING);
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
	m_PresetComponentType = MkWindowPreset::GetWindowPresetComponentEnum(MkHashStr(componentName));

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
	node.SetData(MkSceneNodeFamilyDefinition::BaseWindow::PresetComponentKey, GetPresetComponentName().GetString(), 0);
	node.SetData(MkSceneNodeFamilyDefinition::BaseWindow::PresetComponentSizeKey, MkInt2(static_cast<int>(m_PresetComponentSize.x), static_cast<int>(m_PresetComponentSize.y)), 0);
	node.SetData(MkSceneNodeFamilyDefinition::BaseWindow::AttributeKey, m_Attribute.m_Field, 0);

	// save는 현 상태 그대로를 저장하기 때문에 임시 상태까지 저장 될 수 있다(ex> 커서가 버튼의 위에 있을 경우 on cursor 상태로 저장됨)
	// 따라서 이에 해당하는 경우 일단 default로 돌려놓은 뒤 복구
	// eS2D_TitleState 계열은 window manager가 직접 관리하므로 무시해도 상관없으므로 현재는 eS2D_WindowState 계열만 고려해주면 됨
	bool restoreState = false;
	eS2D_WindowState currState;
	if (m_Enable && IsWindowStateType(m_PresetComponentType))
	{
		currState = static_cast<eS2D_WindowState>(m_CurrentComponentState);
		if (currState != eS2D_WS_DefaultState)
		{
			__TSI_WindowNodeOp<eS2D_WindowState>::SetState(eS2D_WS_DefaultState, this);
			restoreState = true;
		}
	}

	// MkSceneNode
	MkSceneNode::Save(node);

	// state 복구
	if (restoreState)
	{
		__TSI_WindowNodeOp<eS2D_WindowState>::SetState(currState, this);
	}
}

void MkBaseWindowNode::SetEnable(bool enable)
{
	if (enable != m_Enable)
	{
		if (IsWindowStateType(m_PresetComponentType))
		{
			if (_OnActiveWindowState())
			{
				__SetActiveWindowStateCounter(-1);
			}

			__TSI_WindowNodeOp<eS2D_WindowState>::SetState((enable) ? eS2D_WS_DefaultState : eS2D_WS_DisableState, this);
		}

		m_Enable = enable;

		MK_INDEXING_LOOP(m_ChildWindows, i)
		{
			m_ChildWindows[i]->SetEnable(enable);
		}
	}
}

const MkFloatRect& MkBaseWindowNode::GetWindowRect(void) const
{
	if (m_PresetComponentType != eS2D_WPC_None)
	{
		const MkFloatRect* targetRect = __TSI_ComponentNodeOp::GetWorldAABR(this);
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
		if (!IsTitleStateType(d.titleType))
		{
			d.titleType = eS2D_WPC_NormalTitle;
		}

		MkFloat2 titleCompSize(d.windowSize.x - MARGIN * 2.f, d.titleHeight);
		if (targetWindow == NULL)
		{
			titleWindow = __CreateWindowPreset(NULL, nodeName, d.themeName, d.titleType, titleCompSize);
		}
		else if (targetWindow->CreateWindowPreset(d.themeName, d.titleType, titleCompSize))
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
					(MKDEF_S2D_BASE_WND_ICON_TAG_NAME, eRAP_LeftCenter, MkFloat2(MARGIN, 0.f), d.iconImageHeightOffset, d.iconImageFilePath, d.iconImageSubsetName);
			}

			// close icon
			if (d.hasCloseButton)
			{
				MkBaseWindowNode* closeWindow = __CreateWindowPreset(titleWindow, L"CloseBtn", d.themeName, eS2D_WPC_CloseButton, MkFloat2(0.f, 0.f));
				if (closeWindow != NULL)
				{
					MkFloat2 localPos =	MkFloatRect(titleWindow->GetPresetComponentSize()).GetSnapPosition
						(MkFloatRect(closeWindow->GetPresetComponentSize()), eRAP_RightCenter, MkFloat2(MARGIN, 0.f));
					closeWindow->SetLocalPosition(MkVec3(localPos.x, localPos.y, -MKDEF_BASE_WINDOW_DEPTH_GRID));
					closeWindow->SetAttribute(eIgnoreMovement, true);
				}
			}

			// title caption
			titleWindow->SetPresetComponentCaption(d.themeName, d.titleCaption);
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

		const MkFloat2 buttonCompSize(60.f, 20.f);

		// ok button
		MkBaseWindowNode* okWindow = NULL;
		if (d.hasOKButton)
		{
			okWindow = __CreateWindowPreset(backgroundWindow, L"OKButton", d.themeName, eS2D_WPC_OKButton, buttonCompSize);
			okWindow->SetAttribute(MkBaseWindowNode::eConfinedToParent, true);
			okWindow->SetPresetComponentCaption(d.themeName, d.okBtnCaption);
		}

		// cancel button
		MkBaseWindowNode* cancelWindow = NULL;
		if (d.hasCancelButton)
		{
			cancelWindow = __CreateWindowPreset(backgroundWindow, L"CancelButton", d.themeName, eS2D_WPC_CancelButton, buttonCompSize);
			cancelWindow->SetAttribute(MkBaseWindowNode::eConfinedToParent, true);
			cancelWindow->SetPresetComponentCaption(d.themeName, d.cancelBtnCaption);
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
	if (m_PresetComponentType == eS2D_WPC_None)
	{
		const MkArray<MkHashStr>& imageSets = MK_WR_PRESET.GetWindowTypeImageSet(themeName, component);
		if (!imageSets.Empty())
		{
			m_PresetThemeName = themeName;
			m_PresetComponentSize = componentSize;
			m_PresetComponentType = component;

			return __TSI_ComponentNodeOp::ApplyImageSetAndSize(imageSets, this);
		}
	}
	return false;
}

bool MkBaseWindowNode::CreateFreeImageBaseBackgroundWindow(const MkPathName& imagePath, const MkHashStr& subsetName)
{
	if (m_PresetComponentType == eS2D_WPC_None)
	{
		m_PresetThemeName.Clear();
		m_PresetComponentType = eS2D_WPC_BackgroundWindow;

		return __TSI_WindowNodeOp<eS2D_BackgroundState>::ApplyFreeImageToState(eS2D_BS_DefaultState, imagePath, subsetName, this);
	}
	return false;
}

bool MkBaseWindowNode::CreateFreeImageBaseButtonWindow(const MkPathName& imagePath, const MkArray<MkHashStr>& subsetNames)
{
	if ((m_PresetComponentType == eS2D_WPC_None) && (subsetNames.GetSize() == static_cast<unsigned int>(eS2D_WS_MaxWindowState)))
	{
		m_PresetThemeName.Clear();
		m_PresetComponentType = eS2D_WPC_NormalButton;

		for (int i=eS2D_WS_DefaultState; i<eS2D_WS_MaxWindowState; ++i)
		{
			if (!__TSI_WindowNodeOp<eS2D_WindowState>::ApplyFreeImageToState(static_cast<eS2D_WindowState>(i), imagePath, subsetNames[i], this))
				return false;
		}
		
		SetAttribute(eShowActionCursor, true);
		return true;
	}
	return false;
}

void MkBaseWindowNode::SetPresetThemeName(const MkHashStr& themeName)
{
	if ((!m_PresetThemeName.Empty()) && (themeName != m_PresetThemeName)) // 테마가 다르면
	{
		if (m_PresetComponentType != eS2D_WPC_None)
		{
			const MkArray<MkHashStr>& newImageSets = MK_WR_PRESET.GetWindowTypeImageSet(themeName, m_PresetComponentType);
			const MkArray<MkHashStr>& oldImageSets = MK_WR_PRESET.GetWindowTypeImageSet(m_PresetThemeName, m_PresetComponentType);

			if ((!newImageSets.Empty()) && (newImageSets != oldImageSets) && // 테마명은 다르더라도 image set은 같을 수 있으므로 비교 필요
				__TSI_ComponentNodeOp::ApplyImageSetAndSize(newImageSets, this))
			{
				// 위의 ApplyImageSetAndSize()을 거치면 visible 상태가 초기화 되버리기 때문에 disable일 경우 반영해 주어야 함
				if ((!m_Enable) && IsWindowStateType(m_PresetComponentType))
				{
					__TSI_WindowNodeOp<eS2D_WindowState>::SetState(eS2D_WS_DisableState, this);
				}
			}
		}

		// caption
		const MkHashStr hTag = MKDEF_S2D_BASE_WND_HIGHLIGHT_CAP_TAG_NAME;
		const MkHashStr nTag = MKDEF_S2D_BASE_WND_NORMAL_CAP_TAG_NAME;

		if (ExistSRect(hTag))
		{
			MkSRect* srect = GetSRect(hTag);
			if (srect->CheckFocedFontTypeAndState())
			{
				srect->SetFocedFontState(MK_WR_PRESET.GetHighlightThemeFontState(themeName));
			}
		}
		if (ExistSRect(nTag))
		{
			MkSRect* srect = GetSRect(nTag);
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
	if ((m_PresetComponentType != eS2D_WPC_None) && (componentSize != m_PresetComponentSize))
	{
		m_PresetComponentSize = componentSize;
		__TSI_ComponentNodeOp::ApplySize(this);
	}
}

bool MkBaseWindowNode::SetFreeImageToBackgroundWindow(const MkPathName& imagePath, const MkHashStr& subsetName)
{
	// 테마가 없고 component가 eS2D_WPC_BackgroundWindow 타입이면
	if (m_PresetThemeName.Empty() && (m_PresetComponentType == eS2D_WPC_BackgroundWindow))
	{
		return __TSI_WindowNodeOp<eS2D_BackgroundState>::ApplyFreeImageToState(eS2D_BS_DefaultState, imagePath, subsetName, this);
	}
	return false;
}

bool MkBaseWindowNode::SetPresetComponentIcon
(const MkHashStr& iconName, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset, const MkPathName& imagePath, const MkHashStr& subsetName)
{
	if (m_PresetComponentType != eS2D_WPC_None)
	{
		MkSRect* iconRect = ExistSRect(iconName) ? GetSRect(iconName) : CreateSRect(iconName);
		if (iconRect != NULL)
		{
			iconRect->SetFocedFontState(MkHashStr::NullHash);
			iconRect->SetTexture(imagePath, subsetName);
			iconRect->AlignRect(GetPresetComponentSize(), alignment, border, heightOffset);
			iconRect->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
			return true;
		}
	}
	return false;
}

bool MkBaseWindowNode::SetPresetComponentIcon
(const MkHashStr& iconName, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset, const MkHashStr& forcedState, const CaptionDesc& captionDesc)
{
	if (m_PresetComponentType != eS2D_WPC_None)
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
				iconRect->AlignRect(GetPresetComponentSize(), alignment, border, heightOffset);
				iconRect->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
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
	if (m_PresetComponentType != eS2D_WPC_None)
	{
		if ((highlight) ? (IsTitleStateType(m_PresetComponentType) || IsWindowStateType(m_PresetComponentType)) : true)
		{
			const MkHashStr hTag = MKDEF_S2D_BASE_WND_HIGHLIGHT_CAP_TAG_NAME;
			const MkHashStr nTag = MKDEF_S2D_BASE_WND_NORMAL_CAP_TAG_NAME;
			if (SetPresetComponentIcon((highlight) ? hTag : nTag, alignment, border, 0.f, imagePath, subsetName))
			{
				if (highlight)
				{
					GetSRect(hTag)->SetVisible(false);
				}
				return true;
			}
		}
	}
	return false;
}

bool MkBaseWindowNode::SetPresetComponentIcon(bool highlight, eRectAlignmentPosition alignment, const MkFloat2& border, const CaptionDesc& captionDesc)
{
	if (m_PresetComponentType != eS2D_WPC_None)
	{
		if ((highlight) ? (IsTitleStateType(m_PresetComponentType) || IsWindowStateType(m_PresetComponentType)) : true)
		{
			const MkHashStr hTag = MKDEF_S2D_BASE_WND_HIGHLIGHT_CAP_TAG_NAME;
			const MkHashStr nTag = MKDEF_S2D_BASE_WND_NORMAL_CAP_TAG_NAME;
			if (SetPresetComponentIcon((highlight) ? hTag : nTag, alignment, border, 0.f,
				(highlight) ? MK_WR_PRESET.GetHighlightThemeFontState(m_PresetThemeName) : MK_WR_PRESET.GetNormalThemeFontState(m_PresetThemeName), captionDesc))
			{
				if (highlight)
				{
					GetSRect(hTag)->SetVisible(false);
				}
				return true;
			}
		}
	}
	return false;
}

bool MkBaseWindowNode::SetPresetComponentCaption(const MkHashStr& themeName, const CaptionDesc& captionDesc, eRectAlignmentPosition alignment, const MkFloat2& border)
{
	bool hOK = (SetPresetComponentIcon(true, alignment, border, captionDesc) || IsBackgroundStateType(m_PresetComponentType));
	bool nOK = SetPresetComponentIcon(false, alignment, border, captionDesc);
	return (hOK && nOK);
}

bool MkBaseWindowNode::SetPresetComponentItemTag(const ItemTagInfo& tagInfo)
{
	const float MARGIN = MK_WR_PRESET.GetMargin();

	bool iconOK = true;
	bool captionEnable = tagInfo.captionDesc.GetEnable();
	float captionBorderX = MARGIN;
	const MkHashStr iconTagName = MKDEF_S2D_BASE_WND_ICON_TAG_NAME;
	if (tagInfo.iconPath.Empty())
	{
		if (ExistSRect(iconTagName))
		{
			DeleteSRect(iconTagName);
		}
	}
	else
	{
		iconOK = SetPresetComponentIcon(iconTagName, (captionEnable) ? eRAP_LeftCenter : eRAP_MiddleCenter, MkFloat2(MARGIN, 0.f), 0.f, tagInfo.iconPath, tagInfo.iconSubset);
		captionBorderX += GetSRect(iconTagName)->GetLocalSize().x;
		captionBorderX += MARGIN;
	}

	bool captionOK = (captionEnable) ?
		SetPresetComponentCaption(m_PresetThemeName, tagInfo.captionDesc, (captionBorderX == MARGIN) ? eRAP_MiddleCenter : eRAP_LeftCenter, MkFloat2(captionBorderX, 0.f)) : true;

	return (iconOK && captionOK);
}

const MkHashStr& MkBaseWindowNode::GetPresetComponentName(void) const
{
	return MkWindowPreset::GetWindowPresetComponentKeyword(m_PresetComponentType);
}

void MkBaseWindowNode::SetPresetComponentWindowStateToDefault(void)
{
	if (m_Enable && IsWindowStateType(m_PresetComponentType))
	{
		if (_OnActiveWindowState())
		{
			__SetActiveWindowStateCounter(-1);
			__TSI_WindowNodeOp<eS2D_WindowState>::SetState(eS2D_WS_DefaultState, this);
		}
	}
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
			if ((m_PresetComponentType == eS2D_WPC_CloseButton) && // close button 클릭시 edit mode가 아니거나 system 윈도우가 아니면 윈도우 닫음
				((!MK_WIN_EVENT_MGR.GetEditMode()) || GetRootWindow()->GetAttribute(eSystemWindow))) // edit mode시 실수로 일반 윈도우가 닫히는 경우를 방지
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

	bool cursorOver = (inside && GetWindowRect().CheckGridIntersection(position));

	// eS2D_WindowState가 적용된 노드면 eS2D_WS_DefaultState, eS2D_WS_OnClickState, eS2D_WS_OnCursorState 중 하나를 지정
	if (IsWindowStateType(m_PresetComponentType))
	{
		eS2D_WindowState windowState = (cursorOver) ?
			((btnPushing[0] && (this == MK_WIN_EVENT_MGR.GetFrontHitWindow())) ? eS2D_WS_OnClickState : eS2D_WS_OnCursorState) : eS2D_WS_DefaultState;

		eS2D_WindowState lastState = static_cast<eS2D_WindowState>(m_CurrentComponentState);

		if (windowState != lastState)
		{
			if ((lastState == eS2D_WS_DefaultState) && ((windowState == eS2D_WS_OnCursorState) || (windowState == eS2D_WS_OnClickState)))
			{
				__SetActiveWindowStateCounter(1); // def -> on cursor/click
			}
			else if (((lastState == eS2D_WS_OnCursorState) || (lastState == eS2D_WS_OnClickState)) && (windowState == eS2D_WS_DefaultState))
			{
				__SetActiveWindowStateCounter(-1); // on cursor/click -> def
			}

			__TSI_WindowNodeOp<eS2D_WindowState>::SetState(windowState, this);
		}
	}

	if (cursorOver && GetAttribute(eShowActionCursor))
	{
		MK_CURSOR_MGR.SetActionCursor();
	}

	if (inside || (m_ActiveWindowStateCounter > 0)) // 대다수의 윈도우가 (inside == false) && (m_ActiveWindowStateCounter == 0) 조합이기 때문에 여기서 걸러짐
	{
		MK_INDEXING_LOOP(m_ChildWindows, i)
		{
			MkBaseWindowNode* currNode = m_ChildWindows[i];
			if (currNode->GetVisible() && currNode->GetEnable() && (!currNode->GetAttribute(eIgnoreInputEvent)))
			{
				currNode->InputEventMouseMove(inside, btnPushing, position);
			}
		}
	}
}

bool MkBaseWindowNode::CheckCursorHitCondition(const MkFloat2& position) const
{
	return ((IsTitleStateType(m_PresetComponentType) || IsWindowStateType(m_PresetComponentType)) && GetWindowRect().CheckGridIntersection(position));
}

void MkBaseWindowNode::StartDragMovement(MkBaseWindowNode* targetWindow)
{
	if (!CheckRootWindow())
	{
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
					targetNode->StartDragMovement(targetWindow);
					return;
				}
			}

			parentNode = parentNode->GetParentNode();
		}
	}
}

bool MkBaseWindowNode::ConfirmDragMovement(MkBaseWindowNode* targetWindow, MkFloat2& positionOffset)
{
	if (!CheckRootWindow())
	{
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
					return targetNode->ConfirmDragMovement(targetWindow, positionOffset);
				}
			}

			parentNode = parentNode->GetParentNode();
		}
	}
	return true;
}

void MkBaseWindowNode::Activate(void)
{
	if (GetAttribute(eAlignCenterPos))
	{
		AlignPosition(MK_WIN_EVENT_MGR.GetRegionRect(), eRAP_MiddleCenter, MkInt2(0, 0)); // 중앙 정렬
	}
}

void MkBaseWindowNode::OnFocus(void)
{
	if (IsTitleStateType(m_PresetComponentType))
	{
		__TSI_WindowNodeOp<eS2D_TitleState>::SetState(eS2D_TS_OnFocusState, this);
	}

	MK_INDEXING_LOOP(m_ChildWindows, i)
	{
		m_ChildWindows[i]->OnFocus();
	}
}

void MkBaseWindowNode::LostFocus(void)
{
	if (IsTitleStateType(m_PresetComponentType))
	{
		__TSI_WindowNodeOp<eS2D_TitleState>::SetState(eS2D_TS_LostFocusState, this);
	}

	MK_INDEXING_LOOP(m_ChildWindows, i)
	{
		m_ChildWindows[i]->LostFocus();
	}
}

bool MkBaseWindowNode::ChangeNodeName(const MkHashStr& newName)
{
	if (newName == GetNodeName())
		return true;

	MkHashStr oldName = GetNodeName();
	bool ok = MkSceneNode::ChangeNodeName(newName);
	if (ok && CheckRootWindow())
	{
		MK_WIN_EVENT_MGR.__RootWindowNameChanged(oldName, newName);
	}
	return ok;
}

bool MkBaseWindowNode::ChangeChildNodeName(const MkHashStr& oldName, const MkHashStr& newName)
{
	bool ok = MkSceneNode::ChangeChildNodeName(oldName, newName);
	if (ok && (m_ChildrenNode[newName]->GetNodeType() >= eS2D_SNT_BaseWindowNode))
	{
		MkBaseWindowNode* targetNode = dynamic_cast<MkBaseWindowNode*>(m_ChildrenNode[newName]);
		if ((targetNode != NULL) && targetNode->CheckRootWindow())
		{
			MK_WIN_EVENT_MGR.__RootWindowNameChanged(oldName, newName);
		}
	}
	return ok;
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

			int notDefCnt = childWindow->__CountActiveWindowStateCounter();
			if (notDefCnt > 0)
			{
				__AddActiveWindowStateCounter(notDefCnt);
				__SetActiveWindowStateCounter(notDefCnt);
			}
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

			int notDefCnt = childWindow->__CountActiveWindowStateCounter();
			if (notDefCnt > 0)
			{
				__AddActiveWindowStateCounter(-notDefCnt);
				__SetActiveWindowStateCounter(-notDefCnt);
			}
		}
	}
	
	return MkSceneNode::DetachChildNode(childNodeName);
}

void MkBaseWindowNode::Clear(void)
{
	if (_OnActiveWindowState())
	{
		__SetActiveWindowStateCounter(-1);
	}

	m_CurrentComponentState = 0;
	m_ChildWindows.Clear();

	MkSceneNode::Clear();
}

MkBaseWindowNode::MkBaseWindowNode(const MkHashStr& name) : MkSceneNode(name)
{
	m_RootWindow = false;
	m_Enable = true;
	m_PresetComponentType = eS2D_WPC_None;
	m_CurrentComponentState = 0;
	m_ActiveWindowStateCounter = 0;
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

void MkBaseWindowNode::__SetActiveWindowStateCounter(int offset)
{
	if ((!CheckRootWindow()) && (m_ParentNodePtr != NULL) && (m_ParentNodePtr->GetNodeType() >= eS2D_SNT_BaseWindowNode))
	{
		MkBaseWindowNode* targetNode = dynamic_cast<MkBaseWindowNode*>(m_ParentNodePtr);
		if (targetNode != NULL)
		{
			targetNode->__AddActiveWindowStateCounter(offset);
			targetNode->__SetActiveWindowStateCounter(offset);
		}
	}
}

int MkBaseWindowNode::__CountActiveWindowStateCounter(void) const
{
	int cnt = _OnActiveWindowState() ? 1 : 0;

	MK_INDEXING_LOOP(m_ChildWindows, i)
	{
		cnt += m_ChildWindows[i]->__CountActiveWindowStateCounter();
	}

	return cnt;
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

		m_WindowEvents.Flush();
	}
}

void MkBaseWindowNode::__BuildInformationTree(MkBaseWindowNode* parentNode, unsigned int depth, MkArray<MkBaseWindowNode*>& buffer)
{
	if (parentNode != NULL)
	{
		MkBaseWindowNode* targetNode = __CreateWindowPreset
			(parentNode, MkStr(buffer.GetSize()), MK_WR_PRESET.GetDefaultThemeName(), eS2D_WPC_SelectionButton, MkFloat2(100.f, MKDEF_S2D_NODE_SEL_LINE_SIZE));

		if (targetNode != NULL)
		{
			buffer.PushBack(this);

			targetNode->SetAttribute(eIgnoreMovement, true);

			__SetWindowInformation(targetNode);

			if (!GetVisible())
			{
				targetNode->SetAlpha(0.4f);
			}

			targetNode->SetLocalPosition
				(MkFloat2(MKDEF_S2D_NODE_SEL_DEPTH_OFFSET * static_cast<float>(depth), -targetNode->GetPresetComponentSize().y * static_cast<float>(buffer.GetSize() - 1)));

			// 하위 순회
			unsigned int newDepth = depth + 1;
			MK_INDEXING_LOOP(m_ChildWindows, i)
			{
				m_ChildWindows[i]->__BuildInformationTree(parentNode, newDepth, buffer);
			}
		}
	}
}

void MkBaseWindowNode::__SetWindowInformation(MkBaseWindowNode* targetNode) const
{
	if (targetNode != NULL)
	{
		// fill info
		MkStr windowType;
		switch (GetNodeType())
		{
		case eS2D_SNT_BaseWindowNode: MkDecoStr::InsertFontStateTag(MK_FONT_MGR.YellowFS(), L"[BaseWindow]", windowType); break;
		case eS2D_SNT_SpreadButtonNode: MkDecoStr::InsertFontStateTag(MK_FONT_MGR.CianFS(), L"[SpreadBtn]", windowType); break;
		case eS2D_SNT_CheckButtonNode: MkDecoStr::InsertFontStateTag(MK_FONT_MGR.PinkFS(), L"[CheckBtn]", windowType); break;
		case eS2D_SNT_ScrollBarNode: MkDecoStr::InsertFontStateTag(MK_FONT_MGR.OrangeFS(), L"[ScrollBar]", windowType); break;
		case eS2D_SNT_EditBoxNode: MkDecoStr::InsertFontStateTag(MK_FONT_MGR.GreenFS(), L"[EditBox]", windowType); break;
		case eS2D_SNT_TabWindowNode: MkDecoStr::InsertFontStateTag(MK_FONT_MGR.VioletFS(), L"[TabWindow]", windowType); break;
		default: MkDecoStr::InsertFontStateTag(MK_FONT_MGR.LightGrayFS(), L"[SceneNode]", windowType); break;
		}

		MkStr header;
		MkDecoStr::InsertFontTypeTag(MK_FONT_MGR.DSF(), windowType, header);

		MkStr compStr, compType;
		compStr += L"[";
		compStr += (m_PresetComponentType == eS2D_WPC_None) ? L"--" : GetPresetComponentName().GetString();
		compStr += L"]";

		if (IsBackgroundStateType(m_PresetComponentType))
		{
			MkDecoStr::InsertFontStateTag(MK_FONT_MGR.LightGrayFS(), compStr, compType);
		}
		else if (IsTitleStateType(m_PresetComponentType))
		{
			MkDecoStr::InsertFontStateTag(MK_FONT_MGR.RedFS(), compStr, compType);
		}
		else if (IsWindowStateType(m_PresetComponentType))
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

		const MkHashStr hTag = MKDEF_S2D_BASE_WND_HIGHLIGHT_CAP_TAG_NAME;
		targetNode->SetPresetComponentIcon(hTag, eRAP_LeftCenter, MkFloat2(MK_WR_PRESET.GetMargin(), 0.f), 0.f,
			MkHashStr::NullHash, captionDesc);

		const MkHashStr nTag = MKDEF_S2D_BASE_WND_NORMAL_CAP_TAG_NAME;
		targetNode->SetPresetComponentIcon(nTag, eRAP_LeftCenter, MkFloat2(MK_WR_PRESET.GetMargin(), 0.f), 0.f,
			MkHashStr::NullHash, captionDesc);

		// text에 맞게 component 크기 재조정
		const MkInt2& strSize = targetNode->GetSRect(hTag)->GetDecoString().GetDrawingSize();
		MkFloat2 newCompSize = MkFloat2(static_cast<float>(strSize.x) + MK_WR_PRESET.GetMargin() * 2.f, MKDEF_S2D_NODE_SEL_LINE_SIZE);
		targetNode->SetPresetComponentSize(newCompSize);
	}
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

bool MkBaseWindowNode::_OnActiveWindowState(void) const
{
	if (IsWindowStateType(m_PresetComponentType))
	{
		eS2D_WindowState currState = static_cast<eS2D_WindowState>(m_CurrentComponentState);
		return ((currState == eS2D_WS_OnCursorState) || (currState == eS2D_WS_OnClickState));
	}
	return false;
}

//------------------------------------------------------------------------------------------------//