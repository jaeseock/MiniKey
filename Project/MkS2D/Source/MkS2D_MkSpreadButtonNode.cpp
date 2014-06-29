
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkWindowEventManager.h"
#include "MkS2D_MkSpreadButtonNode.h"


const static MkHashStr TEMPLATE_NAME = MKDEF_S2D_BT_SPREADBUTTON_TEMPLATE_NAME;

// MkStr
const static MkHashStr TARGET_UNIQUE_KEY = L"TargetUniqueKey";


//------------------------------------------------------------------------------------------------//

const static MkHashStr ITEM_ICON_NAME = L"ItemIcon";
const static MkHashStr ARROW_PRESET_NAME = L"Arrow";

//------------------------------------------------------------------------------------------------//

MkSpreadButtonNode* MkSpreadButtonNode::GetRootListButton(void) const
{
	int depthFromRoot;
	return GetRootListButton(depthFromRoot);
}

MkSpreadButtonNode* MkSpreadButtonNode::GetRootListButton(int& depthFromRoot) const
{
	int depth = 0;
	MkSceneNode* targetNode = const_cast<MkSpreadButtonNode*>(this);
	while (true)
	{
		if (targetNode == NULL)
		{
			depthFromRoot = -1;
			return NULL; // error
		}

		if (targetNode->GetNodeType() >= eS2D_SNT_SpreadButtonNode)
		{
			MkSpreadButtonNode* lbNode = dynamic_cast<MkSpreadButtonNode*>(targetNode);
			if ((lbNode != NULL) && lbNode->IsRootSpreadButton())
			{
				depthFromRoot = depth;
				return lbNode;
			}
		}

		targetNode = targetNode->GetParentNode();
		++depth;
	}
}

bool MkSpreadButtonNode::CreateSelectionRootTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eOpeningDirection openingDirection)
{
	return _CreateTypeButton(themeName, windowSize, eSeletionRoot, openingDirection);
}

bool MkSpreadButtonNode::CreateStaticRootTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eOpeningDirection openingDirection, const ItemTagInfo& tagInfo)
{
	bool ok = _CreateTypeButton(themeName, windowSize, eStaticRoot, openingDirection);
	if (ok)
	{
		ok = SetItemTag(tagInfo);
	}
	return ok;
}

bool MkSpreadButtonNode::CreateListTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eOpeningDirection openingDirection)
{
	return _CreateTypeButton(themeName, windowSize, eChildListButton, openingDirection);
}

MkSpreadButtonNode* MkSpreadButtonNode::AddItem(const MkHashStr& uniqueKey, const ItemTagInfo& tagInfo, bool initialItem)
{
	MkSpreadButtonNode* rootButton = GetRootListButton();
	if (rootButton != NULL)
	{
		// uniqueKey�� root button���κ��� �����ؾ� ��
		MK_CHECK(rootButton->__CheckUniqueKey(uniqueKey), GetNodeName().GetString() + L" button node�� ������ �������� ���� " + uniqueKey.GetString() + L"item �߰� �õ�")
			return NULL;

		if (initialItem)
		{
			rootButton->__SetTargetUniqueKey(uniqueKey);

			if (rootButton->GetSpreadButtonType() == eSeletionRoot)
			{
				rootButton->SetItemTag(tagInfo);
			}
		}

		MkSpreadButtonNode* lbNode = new MkSpreadButtonNode(uniqueKey);
		if (lbNode != NULL)
		{
			const MkFloat2& btnSize = rootButton->GetPresetFullSize();
			if (lbNode->CreateListTypeButton(rootButton->GetPresetThemeName(), btnSize, eRightside))
			{
				lbNode->SetItemTag(tagInfo);
				lbNode->SetVisible(false);
				lbNode->SetAttribute(eIgnoreMovement, true);
				lbNode->SetLocalDepth(MKDEF_BASE_WINDOW_DEPTH_GRID * 0.01f); // �θ� ��ư���� ���� ��¦ ��� ��. �θ�� �ڽ��� ���� ������ ������ ����
				AttachChildNode(lbNode);

				m_ItemSequence.PushBack(uniqueKey);

				if (m_ItemSequence.GetSize() == 1) // ���� ���� ������ ������ ���� ���� ������ ����
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
	return NULL;
}

MkSpreadButtonNode* MkSpreadButtonNode::GetItem(const MkHashStr& uniqueKey)
{
	if (!m_ItemSequence.Empty())
	{
		unsigned int pos = m_ItemSequence.FindFirstInclusion(MkArraySection(0), uniqueKey);
		if (pos != MKDEF_ARRAY_ERROR)
			return dynamic_cast<MkSpreadButtonNode*>(GetChildNode(m_ItemSequence[pos]));

		MK_INDEXING_LOOP(m_ItemSequence, i)
		{
			MkSpreadButtonNode* childNode = dynamic_cast<MkSpreadButtonNode*>(GetChildNode(m_ItemSequence[i]));
			if (childNode != NULL)
			{
				MkSpreadButtonNode* targetNode = childNode->GetItem(uniqueKey);
				if (targetNode != NULL)
					return targetNode;
			}
		}
	}
	return NULL;
}

bool MkSpreadButtonNode::RemoveItem(const MkHashStr& uniqueKey)
{
	if (!m_ItemSequence.Empty())
	{
		if (m_ItemSequence.Exist(MkArraySection(0), uniqueKey))
		{
			MkSceneNode* targetNode = GetChildNode(uniqueKey);
			if ((targetNode != NULL) && DetachChildNode(uniqueKey))
			{
				delete targetNode;
				m_ItemSequence.EraseFirstInclusion(MkArraySection(0), uniqueKey);

				MkSpreadButtonNode* rootButton = GetRootListButton();
				if ((rootButton != NULL) && (rootButton->GetTargetUniqueKey() == uniqueKey))
				{
					rootButton->__SetTargetUniqueKey(MkHashStr::NullHash);

					if (rootButton->GetSpreadButtonType() == eSeletionRoot)
					{
						ItemTagInfo nullTag;
						rootButton->SetItemTag(nullTag);
					}
				}
			}
			return true;
		}

		MK_INDEXING_LOOP(m_ItemSequence, i)
		{
			MkSpreadButtonNode* node = dynamic_cast<MkSpreadButtonNode*>(GetChildNode(m_ItemSequence[i]));
			if (node != NULL)
			{
				if (node->RemoveItem(uniqueKey))
					return true;
			}
		}
	}
	return false;
}

bool MkSpreadButtonNode::SetItemTag(const ItemTagInfo& tagInfo)
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
	if (!m_ItemTagInfo.captionStr.Empty()) // captionStr �켱
	{
		captionOK = SetPresetComponentCaption(m_PresetThemeName, m_ItemTagInfo.captionStr, eRAP_LeftCenter, MkFloat2(captionBorderX, 0.f));
	}
	else if (!m_ItemTagInfo.captionList.Empty()) // captionStr�� ������� captionList
	{
		captionOK = SetPresetComponentCaption(m_PresetThemeName, m_ItemTagInfo.captionList, eRAP_LeftCenter, MkFloat2(captionBorderX, 0.f));
	}
	else // �� �� ������� ����
	{
		captionOK = SetPresetComponentCaption(m_PresetThemeName, MkHashStr::NullHash, eRAP_LeftCenter, MkFloat2(captionBorderX, 0.f));
	}
	return (iconOK && captionOK);
}

void MkSpreadButtonNode::OpenAllItems(void)
{
	if ((!m_ItemSequence.Empty()) && (!m_ItemsAreOpened) && GetVisible()) // �����ִٸ� hide ����
	{
		MK_INDEXING_LOOP(m_ItemSequence, i)
		{
			GetChildNode(m_ItemSequence[i])->SetVisible(true);
		}
		m_ItemsAreOpened = true;

		int depthFromRoot;
		if (GetRootListButton(depthFromRoot) != NULL)
		{
			MK_WIN_EVENT_MGR.__SpreadButtonOpened(depthFromRoot, this);
		}
	}
}

bool MkSpreadButtonNode::CloseAllItems(void)
{
	if (!m_ItemSequence.Empty())
	{
		MK_INDEXING_LOOP(m_ItemSequence, i) // �������� ����
		{
			MkSpreadButtonNode* node = dynamic_cast<MkSpreadButtonNode*>(GetChildNode(m_ItemSequence[i]));
			if (node != NULL)
			{
				if (node->CloseAllItems()) // ��Ʈ���� �������� �ϳ��� ���θ� ���� �����Ƿ� Ż��
					break;
			}
		}

		if (m_ItemsAreOpened)
		{
			MK_INDEXING_LOOP(m_ItemSequence, i)
			{
				GetChildNode(m_ItemSequence[i])->SetVisible(false);
			}
			m_ItemsAreOpened = false;

			int depthFromRoot;
			if (GetRootListButton(depthFromRoot) != NULL)
			{
				MK_WIN_EVENT_MGR.__SpreadButtonClosed(depthFromRoot);
			}

			// item���� ��Ʈ�� ��ħ
			_FlushItemRegion();

			// ��Ʈ ��ư�̸� managed root�� ���� ����
			// ������ ��ư�� ������ world AABR������ Ŭ������ ���� ��Ŀ�� �Ǻ��� �߸� �� �� �����Ƿ� ������ ��ư�� ���� �� ������ �� ����� �־�� ��
			if (depthFromRoot == 0)
			{
				MkBaseWindowNode* managedRoot = GetManagedRoot();
				if (managedRoot != NULL)
				{
					managedRoot->Update();
				}
			}

			return true;
		}
	}
	return false;
}

void MkSpreadButtonNode::Load(const MkDataNode& node)
{
	MkStr targetUniqueKey;
	node.GetData(TARGET_UNIQUE_KEY, targetUniqueKey, 0);
	m_TargetUniqueKey = targetUniqueKey;

	// MkBaseWindowNode
	MkBaseWindowNode::Load(node);
}

void MkSpreadButtonNode::Save(MkDataNode& node)
{
	// ���� �� ��� ������ ����
	if (IsRootSpreadButton())
	{
		CloseAllItems();
	}

	// ���� ���ø��� ������ ���ø� ����
	_ApplyBuildingTemplateToSave(node, TEMPLATE_NAME);

	node.SetData(TARGET_UNIQUE_KEY, m_TargetUniqueKey.GetString(), 0);

	// MkBaseWindowNode
	MkBaseWindowNode::Save(node);
}

bool MkSpreadButtonNode::InputEventMousePress(unsigned int button, const MkFloat2& position, bool managedRoot)
{
	if ((button == 0) && GetWindowRect().CheckGridIntersection(position)) // left press
	{
		if (m_ItemSequence.Empty()) // leaf means selection
		{
			MkSpreadButtonNode* rootButton = GetRootListButton();
			if (rootButton != NULL)
			{
				if (rootButton->GetTargetUniqueKey() != GetNodeName())
				{
					rootButton->__SetTargetUniqueKey(GetNodeName());

					if (rootButton->GetSpreadButtonType() == eSeletionRoot)
					{
						rootButton->SetItemTag(GetItemTagInfo());
					}
				}

				rootButton->CloseAllItems();
			}
		}
		else // toggle spread
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

MkSpreadButtonNode::MkSpreadButtonNode(const MkHashStr& name) : MkBaseWindowNode(name)
{
	m_ButtonType = eSeletionRoot;
	m_OpeningDirection = eDownward;
	m_ItemsAreOpened = false;
}

//------------------------------------------------------------------------------------------------//

void MkSpreadButtonNode::__GenerateBuildingTemplate(void)
{
	MkDataNode node;
	MkDataNode* tNode = node.CreateChildNode(TEMPLATE_NAME);
	MK_CHECK(tNode != NULL, TEMPLATE_NAME.GetString() + L" template node alloc ����")
		return;

	tNode->ApplyTemplate(MKDEF_S2D_BT_BASEWINNODE_TEMPLATE_NAME); // MkBaseWindowNode�� template ����

	tNode->CreateUnit(TARGET_UNIQUE_KEY, MkStr::Null);

	tNode->DeclareToTemplate(true);
}

bool MkSpreadButtonNode::__CheckUniqueKey(const MkHashStr& uniqueKey)
{
	if (!m_ItemSequence.Empty())
	{
		if (m_ItemSequence.Exist(MkArraySection(0), uniqueKey))
			return false;

		MK_INDEXING_LOOP(m_ItemSequence, i)
		{
			MkSpreadButtonNode* node = dynamic_cast<MkSpreadButtonNode*>(GetChildNode(m_ItemSequence[i]));
			if ((node != NULL) && (!node->__CheckUniqueKey(uniqueKey)))
				return false;
		}
	}
	return true;
}

void MkSpreadButtonNode::__UpdateItemRegion(void)
{
	unsigned int childCount = m_ItemSequence.GetSize();
	if (childCount > 0)
	{
		const MkFloat2& btnSize = GetPresetFullSize();
		float fCount = static_cast<float>(childCount);
		
		MkFloat2 pivotPos;
		switch (m_OpeningDirection)
		{
			case eRightside:
				pivotPos = MkFloat2(btnSize.x, (1.f - fCount) * btnSize.y);
				break;
			case eUpward:
				pivotPos = MkFloat2(0.f, btnSize.y);
				break;
			case eDownward:
				pivotPos = MkFloat2(0.f, -fCount * btnSize.y);
				break;
		}

		// �θ��� rotation�� �����ϰ� scale�� position�� �ݿ��� world rect ���
		MkFloatRect wr(pivotPos, MkFloat2(btnSize.x, fCount * btnSize.y));
		float scale = GetWorldScale();
		if (scale != 1.f)
		{
			wr.position.x *= scale;
			wr.position.y *= scale;
			wr.size.x *= scale;
			wr.size.y *= scale;
		}
		wr.position += MkFloat2(GetWorldPosition().x, GetWorldPosition().y);

		if (m_OpeningDirection == eRightside)
		{
			// wr�� ��ũ�� ���� �ȿ� ������ ���ԵǾ� ������ �Ϲ� ����
			MkFloatRect intersection;
			if ((!MK_WIN_EVENT_MGR.GetRegionRect().GetIntersection(wr, intersection)) || (wr != intersection))
			{
				// ���� ������ ������� �� �ٽ� ���� �׽�Ʈ
				wr.position.y += (fCount - 1.f) * btnSize.y * scale;
				pivotPos.y = 0.f;

				if ((!MK_WIN_EVENT_MGR.GetRegionRect().GetIntersection(wr, intersection)) || (wr != intersection))
				{
					// �Ϲ�, ��� ��� �����߱� ������ �̻ڰ� ���� ó���� ���־�� ������ �װ� ���߿�... �ϴ� ������� ��
				}
			}
		}

		m_ItemWorldAABR = wr;

		float height = 0.f;
		for (unsigned int i=childCount-1; i!=0xffffffff; --i)
		{
			GetChildNode(m_ItemSequence[i])->SetLocalPosition(pivotPos);
			pivotPos.y += btnSize.y;
		}
	}
}

bool MkSpreadButtonNode::_CreateTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eSpreadButtonType buttonType, eOpeningDirection openingDirection)
{
	const float MARGIN = MK_WR_PRESET.GetMargin();

	// window size ��ȿ�� �˻�
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

void MkSpreadButtonNode::_FlushItemRegion(void)
{
	MK_INDEXING_LOOP(m_ItemSequence, i)
	{
		GetChildNode(m_ItemSequence[i])->SetLocalPosition(MkFloat2(0.f, 0.f));
	}
}

//------------------------------------------------------------------------------------------------//