
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowTagNode.h"
#include "MkPA_MkScrollBarControlNode.h"
#include "MkPA_MkListBoxControlNode.h"


const MkHashStr MkListBoxControlNode::ItemTagNodeName(MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"ItemTag");
const MkHashStr MkListBoxControlNode::VScrollBarName(MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"VSB");

const MkHashStr MkListBoxControlNode::ArgKey_Item = L"ItemKey";

#define MKDEF_UNSHOWING_TAG_INDEX -1

//------------------------------------------------------------------------------------------------//

MkListBoxControlNode* MkListBoxControlNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkListBoxControlNode* node = __TSI_SceneNodeDerivedInstanceOp<MkListBoxControlNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkListBoxControlNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkListBoxControlNode::SetListBox(const MkHashStr& themeName, int onePageItemSize, float itemWidth, MkWindowThemeData::eFrameType frameType)
{
	m_WindowFrameType = frameType;

	SetThemeName(themeName);
	SetComponentType(MkWindowThemeData::eCT_DefaultBox);
	SetFormState(MkWindowThemeFormData::eS_Default);

	m_ItemWidth = itemWidth;

	SetOnePageItemSize(onePageItemSize);
}

void MkListBoxControlNode::SetOnePageItemSize(int onePageItemSize)
{
	m_CurrentItemPosition = 0;
	
	// 최대로 보여 줄 수 있는 item 수. 최소 둘
	onePageItemSize = GetMax<int>(onePageItemSize, 2);
	if (onePageItemSize != m_OnePageItemSize)
	{
		m_OnePageItemSize = onePageItemSize;

		// 기존 btn pool 삭제
		SetClientSize(MkFloat2::Zero);

		MK_INDEXING_LOOP(m_ButtonPoolData, i)
		{
			_BtnData& btnData = m_ButtonPoolData[i];

			_DetachItemTag(btnData);
			RemoveChildNode(btnData.btnName);
		}
		m_ButtonPoolData.Clear();

		// 새롭게 btn pool 생성
		float frameSize = MK_STATIC_RES.GetWindowThemeSet().GetFrameSize(GetThemeName(), m_WindowFrameType);
		const MkWindowThemeFormData* bgFormData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(GetThemeName(), MkWindowThemeData::eCT_DefaultBox, MkHashStr::EMPTY);
		const MkWindowThemeFormData* btnFormData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(GetThemeName(), MkWindowThemeData::eCT_BlueSelBtn, MkHashStr::EMPTY);
		if ((frameSize > 0.f) && (bgFormData != NULL) && (btnFormData != NULL))
		{
			// btn region. btn의 client size는 frame size 기준
			m_ItemWidth = GetMax<float>(m_ItemWidth, frameSize);
			MkFloat2 btnClientSize = MkFloat2(m_ItemWidth, frameSize);

			// form margin값을 더해 window size를 구함
			MkFloat2 btnWindowSize = btnClientSize;
			btnWindowSize.x += btnFormData->GetLeftMargin();
			btnWindowSize.x += btnFormData->GetRightMargin();
			btnWindowSize.y += btnFormData->GetTopMargin();
			btnWindowSize.y += btnFormData->GetBottomMargin();

			// bg region
			MkFloat2 bgClientSize = MkFloat2(btnWindowSize.x, btnWindowSize.y * static_cast<float>(m_OnePageItemSize));
			SetClientSize(bgClientSize);

			// btn pool 구성
			m_ButtonPoolData.Reserve(m_OnePageItemSize);
			for (int i=0; i<m_OnePageItemSize; ++i)
			{
				MkHashStr key = MKDEF_PA_WIN_CONTROL_PREFIX + MkStr(i);
				m_ButtonPoolData.PushBack().btnName = key;

				MkWindowBaseNode* btnNode = MkWindowBaseNode::CreateChildNode(this, key);
				btnNode->SetThemeName(GetThemeName());
				btnNode->SetComponentType(MkWindowThemeData::eCT_BlueSelBtn);
				btnNode->SetClientSize(btnClientSize);
				btnNode->SetFormState(MkWindowThemeFormData::eS_Default);
				btnNode->SetAlignmentPosition(eRAP_LeftTop);
				btnNode->SetAlignmentOffset(MkFloat2(0.f, 0.001f - btnWindowSize.y * static_cast<float>(i)));
				btnNode->SetLocalDepth(-0.1f); // this와 겹치는 것을 피하기 위해 0.1f만큼 앞에 위치
				btnNode->SetVisible(false);
			}

			// vertical scroll bar 갱신(없으면 생성)
			MkScrollBarControlNode* vScrollBar = NULL;
			if (ChildExist(VScrollBarName))
			{
				vScrollBar = dynamic_cast<MkScrollBarControlNode*>(GetChildNode(VScrollBarName));
			}
			else
			{
				vScrollBar = MkScrollBarControlNode::CreateChildNode(this, VScrollBarName);
				if (vScrollBar != NULL)
				{
					vScrollBar->SetAlignmentPivotIsWindowRect(true);
					vScrollBar->SetAlignmentPosition(eRAP_RMostCenter);
					vScrollBar->SetWheelScrollLevel(0); // delta 값 그대로 이동
				}
			}

			if (vScrollBar != NULL)
			{
				float vsbHeight = bgClientSize.y + bgFormData->GetTopMargin() + bgFormData->GetBottomMargin();
				vScrollBar->SetVerticalScrollBar(GetThemeName(), m_OnePageItemSize, m_OnePageItemSize, vsbHeight);
			}
		}

		m_UpdateCommand.Set(eUC_ItemList); // item list 갱신 요청
	}
}

MkWindowTagNode* MkListBoxControlNode::AddItem(const MkHashStr& uniqueKey, const MkStr& message)
{
	// 이미 존재하면 삭제
	RemoveItem(uniqueKey);

	// 부모가 없는 독자적인 tag node 생성해 유지(btn에 부착된 상태는 아님)
	MkWindowTagNode* tagNode = MkWindowTagNode::CreateChildNode(NULL, ItemTagNodeName);
	if (tagNode != NULL)
	{
		tagNode->SetLocalDepth(-0.1f); // btn과 겹치는 것을 피하기 위해 0.1f만큼 앞에 위치
		tagNode->SetTextName(MK_STATIC_RES.GetWindowThemeSet().GetCaptionTextNode(GetThemeName(), m_WindowFrameType), message);
		tagNode->SetAlignmentPosition(eRAP_LeftCenter);

		_ItemData& itemData = m_ItemData.Create(uniqueKey);
		itemData.tagNode = tagNode;
		itemData.linkedBtnIndex = MKDEF_UNSHOWING_TAG_INDEX;

		m_ItemSequence.PushBack(uniqueKey);

		m_UpdateCommand.Set(eUC_ItemList); // item list 갱신 요청
	}
	return tagNode;
}

bool MkListBoxControlNode::RemoveItem(const MkHashStr& uniqueKey)
{
	bool ok = m_ItemData.Exist(uniqueKey);
	if (ok)
	{
		_ItemData& itemData = m_ItemData[uniqueKey];

		_DetachItemTag(itemData); // btn에 부착되어 있으면 떼어내고
		delete itemData.tagNode; // tag instance 삭제
		m_ItemData.Erase(uniqueKey); // item data 삭제
		m_ItemSequence.EraseFirstInclusion(MkArraySection(0), uniqueKey); // item sequence에 존재하면 삭제

		m_UpdateCommand.Set(eUC_ItemList); // item list 갱신 요청
	}
	return ok;
}

void MkListBoxControlNode::ClearAllItems(void)
{
	// 모든 btn에 붙어 있는 tag들을 떼어내고
	MK_INDEXING_LOOP(m_ButtonPoolData, i)
	{
		_DetachItemTag(m_ButtonPoolData[i]);
	}

	// 모든 tag instance 삭제
	MkHashMapLooper<MkHashStr, _ItemData> looper(m_ItemData);
	MK_STL_LOOP(looper)
	{
		delete looper.GetCurrentField().tagNode;
	}
	m_ItemData.Clear();
	m_ItemSequence.Clear();

	m_UpdateCommand.Set(eUC_ItemList); // item list 갱신 요청
}

MkWindowTagNode* MkListBoxControlNode::GetItemTag(const MkHashStr& uniqueKey) const
{
	return m_ItemData.Exist(uniqueKey) ? m_ItemData[uniqueKey].tagNode : NULL;
}

void MkListBoxControlNode::SetItemSequence(const MkArray<MkHashStr>& keyList)
{
	m_ItemSequence.Clear();
	m_ItemSequence.Reserve(keyList.GetSize());

	MK_INDEXING_LOOP(keyList, i)
	{
		const MkHashStr& uniqueKey = keyList[i];
		MK_CHECK(m_ItemData.Exist(uniqueKey), GetNodeName().GetString() + L" MkListBoxControlNode 객체에 등록되지 않은 " + uniqueKey.GetString() + L" item 설정 시도")
			continue;

		m_ItemSequence.PushBack(uniqueKey);
	}

	m_UpdateCommand.Set(eUC_ItemList); // item list 갱신 요청
}

void MkListBoxControlNode::SortItemSequenceInAscendingOrder(void)
{
	m_ItemSequence.SortInAscendingOrder();
	m_UpdateCommand.Set(eUC_ItemList); // item list 갱신 요청
}

void MkListBoxControlNode::SortItemSequenceInDescendingOrder(void)
{
	m_ItemSequence.SortInDescendingOrder();
	m_UpdateCommand.Set(eUC_ItemList); // item list 갱신 요청
}

void MkListBoxControlNode::UpdateItemSequenceByKeyPrefix(const MkStr& prefix)
{
	m_ItemSequence.Clear();
	m_ItemSequence.Reserve(m_ItemData.GetSize());

	MkHashMapLooper<MkHashStr, _ItemData> looper(m_ItemData);
	MK_STL_LOOP(looper)
	{
		const MkStr& itemKey = looper.GetCurrentKey().GetString();
		if (itemKey.CheckPrefix(prefix))
		{
			m_ItemSequence.PushBack(itemKey);
		}
	}

	m_UpdateCommand.Set(eUC_ItemList); // item list 갱신 요청
}

void MkListBoxControlNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	// 버튼 입력이 감지되었으면 통지 후 event 소멸
	if ((!m_ButtonPoolData.Empty()) && (!path.Empty()))
	{
		int evtType = -1;
		switch (eventType)
		{
		case ePA_SNE_CursorEntered: evtType = ePA_SNE_ItemEntered; break;
		case ePA_SNE_CursorLeft: evtType = ePA_SNE_ItemLeft; break;
		case ePA_SNE_CursorLBtnPressed: evtType = ePA_SNE_ItemLBtnPressed; break;
		case ePA_SNE_CursorLBtnReleased: evtType = ePA_SNE_ItemLBtnReleased; break;
		case ePA_SNE_CursorLBtnDBClicked: evtType = ePA_SNE_ItemLBtnDBClicked; break;
		case ePA_SNE_CursorMBtnPressed: evtType = ePA_SNE_ItemMBtnPressed; break;
		case ePA_SNE_CursorMBtnReleased: evtType = ePA_SNE_ItemMBtnReleased; break;
		case ePA_SNE_CursorMBtnDBClicked: evtType = ePA_SNE_ItemMBtnDBClicked; break;
		case ePA_SNE_CursorRBtnPressed: evtType = ePA_SNE_ItemRBtnPressed; break;
		case ePA_SNE_CursorRBtnReleased: evtType = ePA_SNE_ItemRBtnReleased; break;
		case ePA_SNE_CursorRBtnDBClicked: evtType = ePA_SNE_ItemRBtnDBClicked; break;
		}
		if (evtType != -1)
		{
			int btnIndex = _GetItemButtonIndex(path[0]);
			if (btnIndex >= 0)
			{
				MkDataNode arg;
				arg.CreateUnitEx(ArgKey_Item, m_ButtonPoolData[btnIndex].linkedItemName);
				StartNodeReportTypeEvent(static_cast<ePA_SceneNodeEvent>(evtType), &arg);
				return;
			}
		}
	}
	
	// scroll bar가 이동되었으면 item position 반영 후 event 소멸
	if ((!m_ButtonPoolData.Empty()) && (!m_ItemSequence.Empty()) &&
		(eventType == ePA_SNE_ScrollBarMoved) && (path.GetSize() == 1) && (path[0] == VScrollBarName) && (argument != NULL))
	{
		MkArray<int> buffer;
		if (argument->GetData(MkScrollBarControlNode::ArgKey_NewItemPosOfScrollBar, buffer) && (buffer.GetSize() == 3))
		{
			m_CurrentItemPosition = buffer[0];
			m_UpdateCommand.Set(eUC_ItemButton); // item btn 갱신 요청
			return;
		}
	}

	// wheel이 움직였고 해당 window가 자신이나 자식 버튼/태그들이고 현재 vertical scroll bar가 보이고 있으면 통지, event 소멸
	if ((eventType == ePA_SNE_WheelMoved) && (path.Empty() || _GetItemButtonIndex(path[0])) && ChildExist(VScrollBarName))
	{
		MkSceneNode* vsbNode = GetChildNode(VScrollBarName);
		if (vsbNode->GetVisible())
		{
			MkScrollBarControlNode* vScrollBar = dynamic_cast<MkScrollBarControlNode*>(vsbNode);
			if (vScrollBar != NULL)
			{
				int delta;
				if (argument->GetData(MkWindowBaseNode::ArgKey_WheelDelta, delta, 0))
				{
					vScrollBar->WheelScrolling(delta);
					return;
				}
			}
		}
	}

	MkWindowBaseNode::SendNodeReportTypeEvent(eventType, path, argument);
}

void MkListBoxControlNode::Clear(void)
{
	m_OnePageItemSize = 0;
	m_CurrentItemPosition = 0;

	ClearAllItems();
	m_ButtonPoolData.Clear();

	MkWindowBaseNode::Clear();
}

MkListBoxControlNode::MkListBoxControlNode(const MkHashStr& name) : MkWindowBaseNode(name)
{
	m_OnePageItemSize = 0;
	m_ItemWidth = 100.f;
	m_CurrentItemPosition = 0;
}

//------------------------------------------------------------------------------------------------//

void MkListBoxControlNode::_ExcuteUpdateCommand(void)
{
	if (m_UpdateCommand[eUC_ItemList])
	{
		_UpdateItemList();
	}

	if (m_UpdateCommand[eUC_ItemButton])
	{
		_UpdateItemButton();
	}

	MkWindowBaseNode::_ExcuteUpdateCommand();
}

void MkListBoxControlNode::_UpdateItemList(void)
{
	if ((m_OnePageItemSize > 0) && (!m_ButtonPoolData.Empty()) && ChildExist(VScrollBarName))
	{
		MkScrollBarControlNode* vScrollBar = dynamic_cast<MkScrollBarControlNode*>(GetChildNode(VScrollBarName));
		if (vScrollBar != NULL)
		{
			int itemSize = static_cast<int>(m_ItemSequence.GetSize());
			if (itemSize > m_OnePageItemSize)
			{
				int itemPosition = m_CurrentItemPosition;
				int floodItemSize = itemPosition + m_OnePageItemSize - itemSize;
				if (floodItemSize > 0)
				{
					itemPosition -= floodItemSize;
				}

				vScrollBar->SetVisible(true);
				vScrollBar->SetTotalItemSize(itemSize);
				vScrollBar->SetItemPosition(itemPosition);

				m_CurrentItemPosition = itemPosition;
			}
			else
			{
				m_CurrentItemPosition = 0;

				vScrollBar->SetVisible(false);
			}

			m_UpdateCommand.Set(eUC_ItemButton); // item btn 갱신 요청
		}
	}
}

void MkListBoxControlNode::_UpdateItemButton(void)
{
	int itemSize = static_cast<int>(m_ItemSequence.GetSize());
	int validSize = GetMin<int>(itemSize, m_OnePageItemSize);

	for (int i=0; i<m_OnePageItemSize; ++i)
	{
		if (i < validSize)
		{
			_AttachItemTag(i, m_ItemSequence[m_CurrentItemPosition + i]);
		}
		else
		{
			_DetachItemTag(m_ButtonPoolData[i]);
		}
	}
}

void MkListBoxControlNode::_DetachItemTag(_BtnData& btnData)
{
	if (!btnData.linkedItemName.Empty())
	{
		_DetachItemTag(btnData, m_ItemData[btnData.linkedItemName]);
	}
}

void MkListBoxControlNode::_DetachItemTag(_ItemData& itemData)
{
	if (itemData.linkedBtnIndex != MKDEF_UNSHOWING_TAG_INDEX)
	{
		_DetachItemTag(m_ButtonPoolData[itemData.linkedBtnIndex], itemData);
	}
}

void MkListBoxControlNode::_DetachItemTag(_BtnData& btnData, _ItemData& itemData)
{
	btnData.linkedItemName.Clear();
	itemData.linkedBtnIndex = MKDEF_UNSHOWING_TAG_INDEX;

	MkSceneNode* btnNode = GetChildNode(btnData.btnName);
	if (btnNode != NULL)
	{
		btnNode->SetVisible(false);
		btnNode->DetachChildNode(ItemTagNodeName);
	}
}

void MkListBoxControlNode::_AttachItemTag(int btnIndex, const MkHashStr& uniqueKey)
{
	if (m_ButtonPoolData.IsValidIndex(btnIndex) && m_ItemData.Exist(uniqueKey))
	{
		_BtnData& btnData = m_ButtonPoolData[btnIndex];
		if (btnData.linkedItemName != uniqueKey)
		{
			_DetachItemTag(btnData);

			MkSceneNode* btnNode = GetChildNode(btnData.btnName);
			if (btnNode != NULL)
			{
				_ItemData& itemData = m_ItemData[uniqueKey];
				_DetachItemTag(itemData);

				btnNode->AttachChildNode(itemData.tagNode);
				btnNode->SetVisible(true);

				itemData.linkedBtnIndex = btnIndex;
				btnData.linkedItemName = uniqueKey;
			}
		}
	}
}

int MkListBoxControlNode::_GetItemButtonIndex(const MkHashStr& name) const
{
	MK_INDEXING_LOOP(m_ButtonPoolData, i)
	{
		if (name == m_ButtonPoolData[i].btnName)
			return i;
	}
	return -1;
}

//------------------------------------------------------------------------------------------------//
