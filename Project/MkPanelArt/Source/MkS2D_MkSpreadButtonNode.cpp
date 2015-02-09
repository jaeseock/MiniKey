
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkWindowEventManager.h"
#include "MkS2D_MkSpreadButtonNode.h"
#include "MkS2D_MkSceneNodeFamilyDefinition.h"


const static MkHashStr ARROW_PRESET_NAME = L"__#Arrow";

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

		if (targetNode->GetNodeType() == eS2D_SNT_SpreadButtonNode)
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
		ok = SetPresetComponentItemTag(tagInfo);
	}
	return ok;
}

MkSpreadButtonNode* MkSpreadButtonNode::AddItem(const MkHashStr& uniqueKey, const ItemTagInfo& tagInfo)
{
	MkSpreadButtonNode* rootButton = GetRootListButton();
	if (rootButton != NULL)
	{
		// uniqueKey�� root button���κ��� �����ؾ� ��
		MK_CHECK(rootButton->__CheckUniqueKey(uniqueKey), GetNodeName().GetString() + L" button node�� ������ �������� ���� " + uniqueKey.GetString() + L"item �߰� �õ�")
			return NULL;

		MkSpreadButtonNode* lbNode = new MkSpreadButtonNode(uniqueKey);
		if (lbNode != NULL)
		{
			const MkFloat2& btnSize = rootButton->GetPresetComponentSize();
			if (lbNode->__CreateListTypeButton(rootButton->GetPresetThemeName(), btnSize, eRightside))
			{
				lbNode->SetPresetComponentItemTag(tagInfo);
				lbNode->SetVisible(false);
				lbNode->SetAttribute(eIgnoreMovement, true);
				lbNode->SetLocalDepth(MKDEF_LIST_BUTTON_DEPTH_GRID); // �θ� ��ư���� ���� ��¦ ���� ��. �θ�� �ڽ��� ���� ������ ������ ����
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
						MkBaseWindowNode* arrowNode = __CreateWindowPreset(this, ARROW_PRESET_NAME, rootButton->GetPresetThemeName(), component, MkFloat2(0.f, 0.f));
						MkFloat2 localPos = MkFloatRect(lbNode->GetPresetComponentSize()).GetSnapPosition
							(MkFloatRect(arrowNode->GetPresetComponentSize()), eRAP_RightCenter, MkFloat2(MK_WR_PRESET.GetMargin(), 0.f));
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

bool MkSpreadButtonNode::SetTargetItem(const MkHashStr& uniqueKey)
{
	bool ok = IsRootSpreadButton();
	if (ok)
	{
		if (uniqueKey != m_TargetUniqueKey)
		{
			m_TargetUniqueKey = uniqueKey;

			if (m_ButtonType == eSeletionRoot)
			{
				MkSpreadButtonNode* button = GetItem(uniqueKey);
				MK_CHECK(button != NULL, GetNodeName().GetString() + L" selection root button�� Ÿ������ �������� �ʴ� " + uniqueKey.GetString() + L" ��ư�� ���� �Ǿ���")
					return false;

				SetPresetComponentItemTag(button->GetItemTagInfo());
			}
		}
	}
	return ok;
}

bool MkSpreadButtonNode::SetTargetItem(const MkSpreadButtonNode* targetNode)
{
	bool ok = (IsRootSpreadButton() && (targetNode != NULL));
	if (ok)
	{
		const MkHashStr& uniqueKey = targetNode->GetNodeName();
		if ((uniqueKey != GetNodeName()) && (uniqueKey != m_TargetUniqueKey))
		{
			m_TargetUniqueKey = uniqueKey;

			if (m_ButtonType == eSeletionRoot)
			{
				SetPresetComponentItemTag(targetNode->GetItemTagInfo());
			}

			// window event
			_PushWindowEvent(MkSceneNodeFamilyDefinition::eSetTargetItem);
		}
	}
	return ok;
}

bool MkSpreadButtonNode::ClearTargetItem(void)
{
	bool ok = IsRootSpreadButton();
	if (ok)
	{
		m_TargetUniqueKey.Clear();

		if (m_ButtonType == eSeletionRoot)
		{
			ItemTagInfo nullTag;
			SetPresetComponentItemTag(nullTag);
		}
	}
	return ok;
}

bool MkSpreadButtonNode::RemoveItem(const MkHashStr& uniqueKey)
{
	if (!m_ItemSequence.Empty())
	{
		if (m_ItemSequence.Exist(MkArraySection(0), uniqueKey))
		{
			CloseAllItems();

			MkSceneNode* targetNode = GetChildNode(uniqueKey);

			MkSpreadButtonNode* rootButton = GetRootListButton();
			if (rootButton != NULL)
			{
				const MkHashStr& targetKey = rootButton->GetTargetItemKey();
				if (!targetKey.Empty())
				{
					if (uniqueKey == targetKey)
					{
						rootButton->ClearTargetItem();
					}
					else
					{
						MkSpreadButtonNode* targetBtn = dynamic_cast<MkSpreadButtonNode*>(targetNode);
						if ((targetBtn != NULL) && (!targetBtn->__CheckUniqueKey(targetKey)))
						{
							rootButton->ClearTargetItem();
						}
					}
				}
			}

			if ((targetNode != NULL) && DetachChildNode(uniqueKey))
			{
				delete targetNode;
			}

			if (m_ItemSequence.Empty())
			{
				RemoveChildNode(ARROW_PRESET_NAME);
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

void MkSpreadButtonNode::RemoveAllItems(void)
{
	if (!m_ItemSequence.Empty())
	{
		CloseAllItems();

		MkSpreadButtonNode* rootButton = GetRootListButton();
		if (rootButton != NULL)
		{
			const MkHashStr& targetKey = rootButton->GetTargetItemKey();
			if ((!targetKey.Empty()) && (!__CheckUniqueKey(targetKey)))
			{
				rootButton->ClearTargetItem();
			}
		}

		MkArray<MkHashStr> itemSequence = m_ItemSequence;
		MK_INDEXING_LOOP(itemSequence, i)
		{
			RemoveChildNode(itemSequence[i]);
		}

		RemoveChildNode(ARROW_PRESET_NAME);
	}
}

bool MkSpreadButtonNode::SetPresetComponentItemTag(const ItemTagInfo& tagInfo, bool deleteIconIfEmpty, bool deleteCaptionIfEmpty)
{
	if ((!tagInfo.iconPath.Empty()) || deleteIconIfEmpty)
	{
		m_ItemTagInfo.iconPath = tagInfo.iconPath;
		m_ItemTagInfo.iconSubset = tagInfo.iconSubset;
	}
	if (tagInfo.captionDesc.GetEnable() || deleteCaptionIfEmpty)
	{
		m_ItemTagInfo.captionDesc = tagInfo.captionDesc;
	}
	
	m_ItemTagInfo.captionDesc.__Check(GetNodeName().GetString());

	return MkBaseWindowNode::SetPresetComponentItemTag(m_ItemTagInfo, deleteIconIfEmpty, deleteCaptionIfEmpty);
}

void MkSpreadButtonNode::OpenAllItems(void)
{
	if ((!m_ItemSequence.Empty()) && (!m_ItemsAreOpened) && GetVisible()) // �����ִٸ� hide ����
	{
		bool btnPushing[3] = { false, false, false };

		MK_INDEXING_LOOP(m_ItemSequence, i)
		{
			MkSceneNode* targetNode = GetChildNode(m_ItemSequence[i]);
			if (targetNode != NULL)
			{
				targetNode->SetVisible(true);

				// ���� ��ư�� ���°� default�� �ƴ� �� �����Ƿ� �������� �ڽĵ鿡�� InputEventMouseMove()�� �־� default�� �ʱ�ȭ
				if (targetNode->GetNodeType() >= eS2D_SNT_BaseWindowNode)
				{
					MkBaseWindowNode* windowNode = dynamic_cast<MkBaseWindowNode*>(targetNode);
					if ((windowNode != NULL) && windowNode->GetEnable())
					{
						windowNode->InputEventMouseMove(false, btnPushing, MkFloat2(0.f, 0.f));
					}
				}
			}
		}
		m_ItemsAreOpened = true;

		int depthFromRoot;
		if (GetRootListButton(depthFromRoot) != NULL)
		{
			MK_WIN_EVENT_MGR.__SpreadButtonOpened(depthFromRoot, this);
		}

		// hide�� ��ư���� ���� ��ư�� ���� ��ġ�� �� �����Ƿ� show�� �ٽ� ��ġ�� �־�� Update()�� �ݿ� ��
		__UpdateItemRegion();

		// window event
		_PushWindowEvent(MkSceneNodeFamilyDefinition::eOpenList);
	}
}

bool MkSpreadButtonNode::CloseAllItems(void)
{
	bool ok = (m_ItemsAreOpened && (!m_ItemSequence.Empty()));
	if (ok)
	{
		MK_INDEXING_LOOP(m_ItemSequence, i) // �������� ����
		{
			MkSpreadButtonNode* targetNode = dynamic_cast<MkSpreadButtonNode*>(GetChildNode(m_ItemSequence[i]));
			if (targetNode != NULL)
			{
				if (targetNode->CloseAllItems()) // ��Ʈ���� �������� �ϳ��� ���θ� ���� �����Ƿ� Ż��
					break;
			}
		}

		MK_INDEXING_LOOP(m_ItemSequence, i)
		{
			MkSceneNode* targetNode = GetChildNode(m_ItemSequence[i]);
			if (targetNode != NULL)
			{
				targetNode->SetVisible(false);
				targetNode->SetLocalPosition(MkFloat2(0.f, 0.f)); // item���� �θ� ��ư�� ��ħ
			}
		}
		m_ItemsAreOpened = false;

		int depthFromRoot;
		if (GetRootListButton(depthFromRoot) != NULL)
		{
			MK_WIN_EVENT_MGR.__SpreadButtonClosed(depthFromRoot);
		}

		// ��Ʈ ��ư�̸� managed root�� ���� ����
		// ������ ��ư�� ������ world AABR������ Ŭ������ ���� ��Ŀ�� �Ǻ��� �߸� �� �� �����Ƿ� ������ ��ư�� ���� �� ������ �� ����� �־�� ��
		if (depthFromRoot == 0)
		{
			MkBaseWindowNode* rootWindow = GetRootWindow();
			if (rootWindow != NULL)
			{
				rootWindow->UpdateAll();
			}
		}
	}
	return ok;
}

void MkSpreadButtonNode::Load(const MkDataNode& node)
{
	unsigned int btnType = 0;
	node.GetData(MkSceneNodeFamilyDefinition::SpreadButton::ButtonTypeKey, btnType, 0);
	m_ButtonType = static_cast<eSpreadButtonType>(btnType);

	unsigned int openingDir = 0;
	node.GetData(MkSceneNodeFamilyDefinition::SpreadButton::OpeningDirKey, openingDir, 0);
	m_OpeningDirection = static_cast<eOpeningDirection>(openingDir);

	MkStr iconPath;
	node.GetData(MkSceneNodeFamilyDefinition::SpreadButton::IconPathKey, iconPath, 0);
	m_ItemTagInfo.iconPath = iconPath;

	MkStr iconSubset;
	node.GetData(MkSceneNodeFamilyDefinition::SpreadButton::IconSubsetKey, iconSubset, 0);
	m_ItemTagInfo.iconSubset = iconSubset;

	MkArray<MkStr> captions;
	node.GetData(MkSceneNodeFamilyDefinition::SpreadButton::CaptionKey, captions);
	m_ItemTagInfo.captionDesc.__Load(GetNodeName().GetString(), captions);

	MkArray<MkStr> itemSeq;
	node.GetData(MkSceneNodeFamilyDefinition::SpreadButton::ItemSeqKey, itemSeq);
	if ((!itemSeq.Empty()) && (!itemSeq[0].Empty()))
	{
		m_ItemSequence.Reserve(itemSeq.GetSize());
		MK_INDEXING_LOOP(itemSeq, i)
		{
			m_ItemSequence.PushBack(itemSeq[i]);
		}
	}

	MkStr targetButton;
	node.GetData(MkSceneNodeFamilyDefinition::SpreadButton::TargetButtonKey, targetButton, 0);
	m_TargetUniqueKey = targetButton;

	// MkBaseWindowNode
	MkBaseWindowNode::Load(node);

	// selection root�� item tag info ����
	if ((m_ButtonType == eSeletionRoot) && (!m_TargetUniqueKey.Empty()))
	{
		MkSpreadButtonNode* button = GetItem(m_TargetUniqueKey);
		MK_CHECK(button != NULL, GetNodeName().GetString() + L" selection root button�� Ÿ������ �������� �ʴ� " + m_TargetUniqueKey.GetString() + L" ��ư�� ���� �Ǿ���")
			return;

		SetPresetComponentItemTag(button->GetItemTagInfo());
	}
}

void MkSpreadButtonNode::Save(MkDataNode& node)
{
	// ���� �� ��� ������ ����
	CloseAllItems();

	// ���� ���ø��� ������ ���ø� ����
	_ApplyBuildingTemplateToSave(node, MkSceneNodeFamilyDefinition::SpreadButton::TemplateName);

	node.SetData(MkSceneNodeFamilyDefinition::SpreadButton::ButtonTypeKey, static_cast<unsigned int>(m_ButtonType), 0);
	node.SetData(MkSceneNodeFamilyDefinition::SpreadButton::OpeningDirKey, static_cast<unsigned int>(m_OpeningDirection), 0);
	node.SetData(MkSceneNodeFamilyDefinition::SpreadButton::IconPathKey, m_ItemTagInfo.iconPath, 0);
	node.SetData(MkSceneNodeFamilyDefinition::SpreadButton::IconSubsetKey, m_ItemTagInfo.iconSubset.GetString(), 0);
	
	MkArray<MkStr> captions;
	m_ItemTagInfo.captionDesc.__Save(captions);
	node.SetData(MkSceneNodeFamilyDefinition::SpreadButton::CaptionKey, captions);

	MkArray<MkStr> itemSeq;
	if (!m_ItemSequence.Empty())
	{
		itemSeq.Reserve(m_ItemSequence.GetSize());
		MK_INDEXING_LOOP(m_ItemSequence, i)
		{
			itemSeq.PushBack(m_ItemSequence[i]);
		}
		node.SetData(MkSceneNodeFamilyDefinition::SpreadButton::ItemSeqKey, itemSeq);
	}

	node.SetData(MkSceneNodeFamilyDefinition::SpreadButton::TargetButtonKey, m_TargetUniqueKey.GetString(), 0);

	// MkBaseWindowNode
	MkBaseWindowNode::Save(node);
}

bool MkSpreadButtonNode::HitEventMousePress(unsigned int button, const MkFloat2& position)
{
	if (button == 0) // left press
	{
		if (m_ItemSequence.Empty()) // leaf means selection
		{
			MkSpreadButtonNode* rootButton = GetRootListButton();
			if (rootButton != NULL)
			{
				rootButton->SetTargetItem(this);
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

	return MkBaseWindowNode::HitEventMousePress(button, position);
}

void MkSpreadButtonNode::Clear(void)
{
	CloseAllItems();

	MkBaseWindowNode::Clear();

	m_ItemTagInfo.iconPath.Clear();
	m_ItemTagInfo.iconSubset.Clear();
	m_ItemTagInfo.captionDesc.Clear();

	m_ItemSequence.Clear();
	m_TargetUniqueKey.Clear();
	m_ItemWorldAABR.Clear();
	m_ItemsAreOpened = false;
}

void MkSpreadButtonNode::SetPresetComponentSize(const MkFloat2& componentSize)
{
	MkBaseWindowNode::SetPresetComponentSize(componentSize);

	MK_INDEXING_LOOP(m_ItemSequence, i)
	{
		MkSpreadButtonNode* childBtn = dynamic_cast<MkSpreadButtonNode*>(GetChildNode(m_ItemSequence[i]));
		if (childBtn != NULL)
		{
			childBtn->SetPresetComponentSize(componentSize);
		}
	}

	if (ChildExist(ARROW_PRESET_NAME))
	{
		MkBaseWindowNode* arrowNode = dynamic_cast<MkBaseWindowNode*>(GetChildNode(ARROW_PRESET_NAME));
		if (arrowNode != NULL)
		{
			MkFloat2 localPos = MkFloatRect(GetPresetComponentSize()).GetSnapPosition
				(MkFloatRect(arrowNode->GetPresetComponentSize()), eRAP_RightCenter, MkFloat2(MK_WR_PRESET.GetMargin(), 0.f));
			arrowNode->SetLocalPosition(localPos);
		}
	}
}

void MkSpreadButtonNode::DetachFromParentNode(void)
{
	if ((!GetReadOnly()) && (m_ParentNodePtr != NULL))
	{
		CloseAllItems();
		
		m_ParentNodePtr->DetachChildNode(GetNodeName());
	}
}

bool MkSpreadButtonNode::DetachChildNode(const MkHashStr& childNodeName)
{
	if (!m_ItemSequence.Empty())
	{
		if (m_ItemSequence.FindFirstInclusion(MkArraySection(0), childNodeName) != MKDEF_ARRAY_ERROR)
		{
			CloseAllItems();
		}
	}

	bool ok = MkBaseWindowNode::DetachChildNode(childNodeName);
	if (ok)
	{
		m_ItemSequence.EraseFirstInclusion(MkArraySection(0), childNodeName);
	}
	return ok;
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
	MkDataNode* tNode = node.CreateChildNode(MkSceneNodeFamilyDefinition::SpreadButton::TemplateName);
	MK_CHECK(tNode != NULL, MkSceneNodeFamilyDefinition::SpreadButton::TemplateName.GetString() + L" template node alloc ����")
		return;

	tNode->ApplyTemplate(MkSceneNodeFamilyDefinition::BaseWindow::TemplateName); // MkBaseWindowNode�� template ����

	tNode->CreateUnit(MkSceneNodeFamilyDefinition::SpreadButton::ButtonTypeKey, static_cast<unsigned int>(0));
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::SpreadButton::OpeningDirKey, static_cast<unsigned int>(0));
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::SpreadButton::IconPathKey, MkStr::Null);
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::SpreadButton::IconSubsetKey, MkStr::Null);
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::SpreadButton::CaptionKey, MkStr::Null);
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::SpreadButton::ItemSeqKey, MkStr::Null);
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::SpreadButton::TargetButtonKey, MkStr::Null);

	tNode->DeclareToTemplate(true);
}

bool MkSpreadButtonNode::__CreateListTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eOpeningDirection openingDirection)
{
	return _CreateTypeButton(themeName, windowSize, eChildListButton, openingDirection);
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
		const MkFloat2& btnSize = GetPresetComponentSize();
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
	bool ok = CreateWindowPreset(themeName, ((buttonType == eSeletionRoot) || (buttonType == eStaticRoot)) ? eS2D_WPC_RootButton : eS2D_WPC_ListButton, windowSize);
	if (ok)
	{
		m_ButtonType = buttonType;
		m_OpeningDirection = openingDirection;
	}
	return ok;
}

//------------------------------------------------------------------------------------------------//