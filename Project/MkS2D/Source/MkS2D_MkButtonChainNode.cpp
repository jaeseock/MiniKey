
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkButtonChainNode.h"


const static MkHashStr TEMPLATE_NAME = MKDEF_S2D_BT_BUTTONCHAIN_TEMPLATE_NAME;

// MkStr
const static MkHashStr TARGET_UNIQUE_KEY = L"TargetUniqueKey";


//------------------------------------------------------------------------------------------------//

const static MkHashStr ITEM_ICON_NAME = L"ItemIcon";
const static MkHashStr ARROW_PRESET_NAME = L"Arrow";

//------------------------------------------------------------------------------------------------//

MkButtonChainNode* MkButtonChainNode::GetRootListButton(void) const
{
	if (IsRootListButton())
		return const_cast<MkButtonChainNode*>(this);

	MkSceneNode* targetNode = const_cast<MkButtonChainNode*>(this);
	while (true)
	{
		if (targetNode == NULL)
			return NULL; // error

		if (targetNode->GetNodeType() >= eS2D_SNT_ButtonChainNode)
		{
			MkButtonChainNode* lbNode = dynamic_cast<MkButtonChainNode*>(targetNode);
			if ((lbNode != NULL) && lbNode->IsRootListButton())
			{
				return lbNode;
			}
		}

		targetNode = targetNode->GetParentNode();
	}
}

bool MkButtonChainNode::CreateSelectionRootTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eOpeningDirection openingDirection)
{
	return _CreateTypeButton(themeName, windowSize, eSeletionRoot, openingDirection);
}

bool MkButtonChainNode::CreateStaticRootTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eOpeningDirection openingDirection, const ItemTagInfo& tagInfo)
{
	bool ok = _CreateTypeButton(themeName, windowSize, eStaticRoot, openingDirection);
	if (ok)
	{
		ok = SetItemTag(tagInfo);
	}
	return ok;
}

bool MkButtonChainNode::CreateListTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eOpeningDirection openingDirection)
{
	return _CreateTypeButton(themeName, windowSize, eChildListButton, openingDirection);
}

MkButtonChainNode* MkButtonChainNode::AddItem(const MkHashStr& uniqueKey, const ItemTagInfo& tagInfo, bool initialItem)
{
	MkButtonChainNode* rootButton = GetRootListButton();
	if (rootButton != NULL)
	{
		// uniqueKey는 root button으로부터 유일해야 함
		MK_CHECK(rootButton->__CheckUniqueKey(uniqueKey), GetNodeName().GetString() + L" button node의 하위로 유일하지 않은 " + uniqueKey.GetString() + L"item 추가 시도")
			return NULL;

		bool createChild = true;
		if (initialItem)
		{
			rootButton->__SetTargetUniqueKey(uniqueKey);

			if (rootButton->GetListButtonType() == eSeletionRoot)
			{
				rootButton->SetItemTag(tagInfo);
				createChild = false;
			}
		}

		if (createChild)
		{
			MkButtonChainNode* lbNode = new MkButtonChainNode(uniqueKey);
			if (lbNode != NULL)
			{
				const MkFloat2& btnSize = rootButton->GetPresetFullSize();
				if (lbNode->CreateListTypeButton(rootButton->GetPresetThemeName(), btnSize, eRightside))
				{
					lbNode->SetItemTag(tagInfo);
					lbNode->SetVisible(false);
					lbNode->SetAttribute(eIgnoreMovement, true);
					AttachChildNode(lbNode);

					m_ItemSequence.PushBack(uniqueKey);
					AlignChildItems();

					if (m_ItemSequence.GetSize() == 1) // 최초 하위 아이템 생성시 전개 방향 아이콘 생성
					{
						eS2D_WindowPresetComponent component = eS2D_WPC_None;
						switch (m_OpeningDirection)
						{
						case eRightside: component = eS2D_WPC_ArrowRight; break;
						case eUpward: component = eS2D_WPC_ArrowUp; break;
						case eDownward: component = eS2D_WPC_ArrowDown; break;
						}
						if (component != eS2D_WPC_None)
						{
							MkBaseWindowNode* arrowNode = __CreateWindowPreset(ARROW_PRESET_NAME, rootButton->GetPresetThemeName(), component, MkFloat2(0.f, 0.f));
							MkFloat2 localPos = MkFloatRect(lbNode->GetPresetFullSize()).GetSnapPosition
								(MkFloatRect(arrowNode->GetPresetFullSize()), eRAP_RightCenter, MkFloat2(MK_WR_PRESET.GetMargin(), 0.f));
							arrowNode->SetLocalPosition(localPos);
							arrowNode->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
							arrowNode->SetAttribute(eIgnoreMovement, true);
						}
					}

					return lbNode;
				}
				else
				{
					MK_DELETE(lbNode);
				}
			}
		}
	}
	return NULL;
}

bool MkButtonChainNode::SetItemTag(const ItemTagInfo& tagInfo)
{
	m_ItemTagInfo = tagInfo;

	const float MARGIN = MK_WR_PRESET.GetMargin();

	bool iconOK = true;
	float captionBorderX = MARGIN;
	if (m_ItemTagInfo.iconPath.Empty())
	{
		if (ExistSRect(ITEM_ICON_NAME))
		{
			DeleteSRect(ITEM_ICON_NAME);
		}
	}
	else
	{
		iconOK = SetPresetComponentIcon(ITEM_ICON_NAME, eRAP_LeftCenter, MkFloat2(MARGIN, 0.f), 0.f, m_ItemTagInfo.iconPath, m_ItemTagInfo.iconSubset);
		captionBorderX += GetSRect(ITEM_ICON_NAME)->GetLocalSize().x;
		captionBorderX += MARGIN;
	}

	bool captionOK;
	if (!m_ItemTagInfo.captionStr.Empty()) // captionStr 우선
	{
		captionOK = SetPresetComponentCaption(m_PresetThemeName, m_ItemTagInfo.captionStr, eRAP_LeftCenter, MkFloat2(captionBorderX, 0.f));
	}
	else if (!m_ItemTagInfo.captionList.Empty()) // captionStr가 비었으면 captionList
	{
		captionOK = SetPresetComponentCaption(m_PresetThemeName, m_ItemTagInfo.captionList, eRAP_LeftCenter, MkFloat2(captionBorderX, 0.f));
	}
	else // 둘 다 비었으니 삭제
	{
		captionOK = SetPresetComponentCaption(m_PresetThemeName, MkHashStr::NullHash, eRAP_LeftCenter, MkFloat2(captionBorderX, 0.f));
	}
	return (iconOK && captionOK);
}

void MkButtonChainNode::AlignChildItems(void)
{
	unsigned int childCount = m_ItemSequence.GetSize();
	if (childCount > 0)
	{
		const MkFloat2& btnSize = GetPresetFullSize();

		MkFloat2 pivotPos;
		switch (m_OpeningDirection)
		{
		case eRightside:
			pivotPos.x += btnSize.x;
			pivotPos.y -= static_cast<float>(childCount - 1) * btnSize.y;
			break;
		case eUpward:
			pivotPos.y += btnSize.y;
			break;
		case eDownward:
			pivotPos.y -= static_cast<float>(childCount) * btnSize.y;
			break;
		}

		float height = 0.f;
		for (unsigned int i=childCount-1; i!=0xffffffff; --i)
		{
			GetChildNode(m_ItemSequence[i])->SetLocalPosition(pivotPos);
			pivotPos.y += btnSize.y;
		}
	}
}

void MkButtonChainNode::OpenAllItems(void)
{
	if ((!m_ItemSequence.Empty()) && (!m_ItemsAreOpened))
	{
		MK_INDEXING_LOOP(m_ItemSequence, i)
		{
			GetChildNode(m_ItemSequence[i])->SetVisible(true);
		}
		m_ItemsAreOpened = true;
	}
}

void MkButtonChainNode::CloseAllItems(void)
{
	if ((!m_ItemSequence.Empty()) && m_ItemsAreOpened)
	{
		MK_INDEXING_LOOP(m_ItemSequence, i)
		{
			GetChildNode(m_ItemSequence[i])->SetVisible(false);
		}
		m_ItemsAreOpened = false;
	}
}

void MkButtonChainNode::Load(const MkDataNode& node)
{
	MkStr targetUniqueKey;
	node.GetData(TARGET_UNIQUE_KEY, targetUniqueKey, 0);
	m_TargetUniqueKey = targetUniqueKey;

	// MkBaseWindowNode
	MkBaseWindowNode::Load(node);
}

void MkButtonChainNode::Save(MkDataNode& node)
{
	// 기존 템플릿이 없으면 템플릿 적용
	_ApplyBuildingTemplateToSave(node, TEMPLATE_NAME);

	node.SetData(TARGET_UNIQUE_KEY, m_TargetUniqueKey.GetString(), 0);

	// MkBaseWindowNode
	MkBaseWindowNode::Save(node);
}

bool MkButtonChainNode::InputEventMousePress(unsigned int button, const MkFloat2& position, bool managedRoot)
{
	if ((button == 0) && GetWindowRect().CheckGridIntersection(position)) // left press
	{
		if (m_ItemSequence.Empty()) // leaf
		{
			MkButtonChainNode* rootButton = GetRootListButton();
			if (rootButton != NULL)
			{
				rootButton->__SetTargetUniqueKey(GetNodeName());

				if (rootButton->GetListButtonType() == eSeletionRoot)
				{
					rootButton->SetItemTag(GetItemTagInfo());
				}

				rootButton->CloseAllItems();
			}
		}
		else // toggle openning
		{
			if (m_ItemsAreOpened)
			{
				CloseAllItems();
			}
			else
			{
				OpenAllItems();
			}
		}
		return true;
	}

	return MkBaseWindowNode::InputEventMousePress(button, position, managedRoot);
}

MkButtonChainNode::MkButtonChainNode(const MkHashStr& name) : MkBaseWindowNode(name)
{
	m_ButtonType = eSeletionRoot;
	m_OpeningDirection = eDownward;
	m_ItemsAreOpened = false;
}

//------------------------------------------------------------------------------------------------//

void MkButtonChainNode::__GenerateBuildingTemplate(void)
{
	MkDataNode node;
	MkDataNode* tNode = node.CreateChildNode(TEMPLATE_NAME);
	MK_CHECK(tNode != NULL, TEMPLATE_NAME.GetString() + L" template node alloc 실패")
		return;

	tNode->ApplyTemplate(MKDEF_S2D_BT_BASEWINNODE_TEMPLATE_NAME); // MkBaseWindowNode의 template 적용

	tNode->CreateUnit(TARGET_UNIQUE_KEY, MkStr::Null);

	tNode->DeclareToTemplate(true);
}

bool MkButtonChainNode::__CheckUniqueKey(const MkHashStr& uniqueKey)
{
	if (!m_ItemSequence.Empty())
	{
		if (m_ItemSequence.Exist(MkArraySection(0), uniqueKey))
			return false;

		MK_INDEXING_LOOP(m_ItemSequence, i)
		{
			MkButtonChainNode* node = dynamic_cast<MkButtonChainNode*>(GetChildNode(m_ItemSequence[i]));
			if ((node != NULL) && (!node->__CheckUniqueKey(uniqueKey)))
				return false;
		}
	}
	return true;
}

bool MkButtonChainNode::_CreateTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eListButtonType buttonType, eOpeningDirection openingDirection)
{
	const float MARGIN = MK_WR_PRESET.GetMargin();

	// window size 유효성 검사
	MkFloat2 bodySize;
	bodySize.x = Clamp<float>(windowSize.x - MARGIN * 2.f, 0.f, windowSize.x);
	bodySize.y = Clamp<float>(windowSize.y - MARGIN * 2.f, 0.f, windowSize.y);

	bool ok = CreateWindowPreset(themeName, ((buttonType == eSeletionRoot) || (buttonType == eStaticRoot)) ? eS2D_WPC_RootButton : eS2D_WPC_ListButton, bodySize);
	if (ok)
	{
		m_ButtonType = buttonType;
		m_OpeningDirection = openingDirection;
	}
	return ok;
}

//------------------------------------------------------------------------------------------------//