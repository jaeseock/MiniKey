
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkListButtonNode.h"


const static MkHashStr TEMPLATE_NAME = MKDEF_S2D_BT_LISTBUTTON_TEMPLATE_NAME;

// MkStr
const static MkHashStr TARGET_UNIQUE_KEY = L"TargetUniqueKey";


//------------------------------------------------------------------------------------------------//

const static MkHashStr ITEM_ICON_NAME = L"ItemIcon";

//------------------------------------------------------------------------------------------------//

MkListButtonNode* MkListButtonNode::GetRootListButton(void) const
{
	if (IsRootListButton())
		return const_cast<MkListButtonNode*>(this);

	MkSceneNode* targetNode = const_cast<MkListButtonNode*>(this);
	while (true)
	{
		if (targetNode == NULL)
			return NULL; // error

		if (targetNode->GetNodeType() >= eS2D_SNT_ListButtonNode)
		{
			MkListButtonNode* lbNode = dynamic_cast<MkListButtonNode*>(targetNode);
			if ((lbNode != NULL) && lbNode->IsRootListButton())
			{
				return lbNode;
			}
		}

		targetNode = targetNode->GetParentNode();
	}
}

void MkListButtonNode::Load(const MkDataNode& node)
{
	MkStr targetUniqueKey;
	node.GetData(TARGET_UNIQUE_KEY, targetUniqueKey, 0);
	m_TargetUniqueKey = targetUniqueKey;

	// MkBaseWindowNode
	MkBaseWindowNode::Load(node);
}

void MkListButtonNode::Save(MkDataNode& node)
{
	// 기존 템플릿이 없으면 템플릿 적용
	_ApplyBuildingTemplateToSave(node, TEMPLATE_NAME);

	node.SetData(TARGET_UNIQUE_KEY, m_TargetUniqueKey.GetString(), 0);

	// MkBaseWindowNode
	MkBaseWindowNode::Save(node);
}

bool MkListButtonNode::CreateListTypeButton
(const MkHashStr& themeName, const MkFloat2& windowSize, eListButtonType buttonType, eOpeningDirection openingDirection)
{
	const float MARGIN = MK_WR_PRESET.GetMargin();

	// window size 유효성 검사
	MkFloat2 bodySize;
	bodySize.x = Clamp<float>(windowSize.x - MARGIN * 2.f, 0.f, windowSize.x);
	bodySize.y = Clamp<float>(windowSize.y - MARGIN * 2.f, 0.f, windowSize.y);

	bool ok = CreateWindowPreset(themeName, eS2D_WPC_ListButton, bodySize);
	if (ok)
	{
		m_ButtonType = buttonType;
		m_OpeningDirection = openingDirection;
	}
	return ok;
}

MkListButtonNode* MkListButtonNode::AddItem(const MkHashStr& uniqueKey, const ItemTagInfo& tagInfo, bool initialItem)
{
	MkListButtonNode* rootButton = GetRootListButton();
	if (rootButton != NULL)
	{
		if (rootButton->GetDecendentNode(uniqueKey) == NULL) // uniqueKey는 root button으로부터 유일해야 함
		{
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
				MkListButtonNode* lbNode = new MkListButtonNode(uniqueKey);
				if (lbNode != NULL)
				{
					const MkFloat2& btnSize = rootButton->GetPresetFullSize();
					if (lbNode->CreateListTypeButton(rootButton->GetPresetThemeName(), btnSize, eChildListButton, eRightside))
					{
						lbNode->SetItemTag(tagInfo);
						lbNode->SetVisible(false);
						lbNode->SetAttribute(eIgnoreMovement, true); // 이동 금지
						AttachChildNode(lbNode);

						m_ChildButtonSequence.PushBack(uniqueKey);
						AlignChildItems();

						return lbNode;
					}
					else
					{
						MK_DELETE(lbNode);
					}
				}
			}
		}
	}
	return NULL;
}

bool MkListButtonNode::SetItemTag(const ItemTagInfo& tagInfo)
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

	bool captionOK = SetPresetComponentCaption(m_PresetThemeName, m_ItemTagInfo.caption, eRAP_LeftCenter, MkFloat2(captionBorderX, 0.f));
	return (iconOK && captionOK);
}

void MkListButtonNode::AlignChildItems(void)
{
	unsigned int childCount = m_ChildButtonSequence.GetSize();
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
		case eDownward:
			pivotPos.y -= static_cast<float>(childCount) * btnSize.y;
			break;
		case eUpward:
			pivotPos.y += btnSize.y;
			break;
		}

		float height = 0.f;
		for (unsigned int i=childCount-1; i!=0xffffffff; --i)
		{
			GetChildNode(m_ChildButtonSequence[i])->SetLocalPosition(pivotPos);
			pivotPos.y += btnSize.y;
		}
	}
}

void MkListButtonNode::OpenAllItems(void)
{
	if ((!m_ChildButtonSequence.Empty()) && (!m_ItemsAreOpened))
	{
		MK_INDEXING_LOOP(m_ChildButtonSequence, i)
		{
			GetChildNode(m_ChildButtonSequence[i])->SetVisible(true);
		}
		m_ItemsAreOpened = true;
	}
}

void MkListButtonNode::CloseAllItems(void)
{
	if ((!m_ChildButtonSequence.Empty()) && m_ItemsAreOpened)
	{
		MK_INDEXING_LOOP(m_ChildButtonSequence, i)
		{
			GetChildNode(m_ChildButtonSequence[i])->SetVisible(false);
		}
		m_ItemsAreOpened = false;
	}
}

void MkListButtonNode::SetPresetThemeName(const MkHashStr& themeName)
{
	if ((!m_PresetThemeName.Empty()) && (themeName != m_PresetThemeName) && (!m_ItemTagInfo.caption.Empty()))
	{
		SetPresetComponentCaption(themeName, m_ItemTagInfo.caption, eRAP_NonePosition, MkFloat2(0.f, 0.f)); // 재정렬 안함
	}

	MkBaseWindowNode::SetPresetThemeName(themeName);
}

bool MkListButtonNode::InputEventMousePress(unsigned int button, const MkFloat2& position, bool managedRoot)
{
	if ((button == 0) && GetWindowRect().CheckGridIntersection(position)) // left press
	{
		if (m_ChildButtonSequence.Empty()) // leaf
		{
			MkListButtonNode* rootButton = GetRootListButton();
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

MkListButtonNode::MkListButtonNode(const MkHashStr& name) : MkBaseWindowNode(name)
{
	//m_ButtonType = eSeletionRoot;
	//m_OpeningDirection = openingDirection;
	m_ItemsAreOpened = false;
}

//------------------------------------------------------------------------------------------------//

void MkListButtonNode::__GenerateBuildingTemplate(void)
{
	MkDataNode node;
	MkDataNode* tNode = node.CreateChildNode(TEMPLATE_NAME);
	MK_CHECK(tNode != NULL, TEMPLATE_NAME.GetString() + L" template node alloc 실패")
		return;

	tNode->ApplyTemplate(MKDEF_S2D_BT_BASEWINNODE_TEMPLATE_NAME); // MkBaseWindowNode의 template 적용

	tNode->CreateUnit(TARGET_UNIQUE_KEY, MkStr::Null);

	tNode->DeclareToTemplate(true);
}

//------------------------------------------------------------------------------------------------//