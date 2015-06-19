
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkScrollBarControlNode.h"


const MkHashStr MkScrollBarControlNode::PageBarNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"PageBar";
const float MkScrollBarControlNode::BarWidth = 10.f;

const MkHashStr MkScrollBarControlNode::ArgKey_NewItemPosOfScrollBar = L"NewItemPosOfScrollBar";

const MkHashStr MkScrollBarControlNode::ObjKey_TotalItemSize = L"TotalItemSize";
const MkHashStr MkScrollBarControlNode::ObjKey_OnePageItemSize = L"OnePageItemSize";
const MkHashStr MkScrollBarControlNode::ObjKey_TotalBarLength = L"TotalBarLength";
const MkHashStr MkScrollBarControlNode::ObjKey_Horizontal = L"Horizontal";
const MkHashStr MkScrollBarControlNode::ObjKey_ItemPosition = L"ItemPosition";
const MkHashStr MkScrollBarControlNode::ObjKey_WheelScrollLevel = L"WheelScrollLevel";

// scroll bar를 wheel로 움직일 때 이동 비율(한 page를 몇 번의 이동으로 움직일지 결정)
#define MKDEF_WHEEL_SCROLLING_LEVEL 10


//------------------------------------------------------------------------------------------------//

MkScrollBarControlNode* MkScrollBarControlNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkScrollBarControlNode* node = __TSI_SceneNodeDerivedInstanceOp<MkScrollBarControlNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkScrollBarControlNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkScrollBarControlNode::SetHorizontalScrollBar(const MkHashStr& themeName, int totalItemSize, int onePageItemSize, float length)
{
	_SetScrollBar(themeName, GetMax<int>(totalItemSize, 1), GetMax<int>(onePageItemSize, 1), length, true, true, 0);
}

void MkScrollBarControlNode::SetVerticalScrollBar(const MkHashStr& themeName, int totalItemSize, int onePageItemSize, float length)
{
	_SetScrollBar(themeName, GetMax<int>(totalItemSize, 1), GetMax<int>(onePageItemSize, 1), length, false, true, 0);
}

void MkScrollBarControlNode::SetTotalItemSize(int totalItemSize)
{
	totalItemSize = GetMax<int>(totalItemSize, 1);
	if (totalItemSize != m_TotalItemSize)
	{
		_SetScrollBar(GetThemeName(), GetMax<int>(totalItemSize, 1), GetOnePageItemSize(), m_TotalBarLength, m_Horizontal, false, 0);
	}
}

void MkScrollBarControlNode::SetOnePageItemSize(int onePageItemSize)
{
	onePageItemSize = GetMax<int>(onePageItemSize, 1);
	if (onePageItemSize != m_OnePageItemSize)
	{
		_SetScrollBar(GetThemeName(), GetTotalItemSize(), GetMax<int>(onePageItemSize, 1), m_TotalBarLength, m_Horizontal, false, 0);
	}
}

void MkScrollBarControlNode::SetItemPosition(int position)
{
	_SetItemPosition(position, true, true);
}

void MkScrollBarControlNode::WheelScrolling(int delta)
{
	int tick = (m_WheelScrollLevel <= 0) ? 1 : (m_OnePageItemSize / m_WheelScrollLevel);
	SetItemPosition(m_CurrentItemPosition - delta * tick);
}

void MkScrollBarControlNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	// drag movement이고 해당 window가 page bar면 item position을 산출 후 event 소멸
	// 만약 item position이 변경되었다면 ePA_SNE_ScrollBarMoved event 발생시킴
	if ((eventType == ePA_SNE_DragMovement) && (path.GetSize() == 1) && (path[0] == PageBarNodeName))
	{
		MkWindowBaseNode* node = dynamic_cast<MkWindowBaseNode*>(GetChildNode(PageBarNodeName));
		if (node != NULL)
		{
			float pageBarPos = _GetPageBarPosition(node);
			int newItemPos = _ConvertPageBarPositionToItemPosition(pageBarPos);
			if (newItemPos != m_CurrentItemPosition)
			{
				_SetItemPosition(newItemPos, false, false);
				return;
			}
		}
	}

	// left cursor pressed(or DBClicked)이고 해당 window가 자신이면 page 단위 이동 후 event 소멸
	// 만약 item position이 변경되었다면 ePA_SNE_ScrollBarMoved event 발생시킴
	if (((eventType == ePA_SNE_CursorLBtnPressed) || (eventType == ePA_SNE_CursorLBtnDBClicked))
		&& path.Empty() && (argument != NULL) && ChildExist(PageBarNodeName))
	{
		MkVec2 buffer;
		if (argument->GetData(MkWindowBaseNode::ArgKey_CursorLocalPosition, buffer, 0))
		{
			float hitPos = (m_Horizontal) ? buffer.x : (m_TotalBarLength - buffer.y);
			float divider = _GetPageBarPosition(GetChildNode(PageBarNodeName)) + m_PageBarLength * 0.5f; // page bar의 center
			int newItemPos = m_CurrentItemPosition + ((hitPos < divider) ? -m_OnePageItemSize : m_OnePageItemSize); // page up/down
			_SetItemPosition(newItemPos, true, true);
			return;
		}
	}

	// 수직 scroll bar인데 wheel이 움직였으면 item position 변경, event 소멸
	if ((eventType == ePA_SNE_WheelMoved) && (!m_Horizontal) && (argument != NULL))
	{
		int delta;
		if (argument->GetData(MkWindowBaseNode::ArgKey_WheelDelta, delta, 0))
		{
			WheelScrolling(delta);
			return;
		}
	}

	MkWindowBaseNode::SendNodeReportTypeEvent(eventType, path, argument);
}

void MkScrollBarControlNode::Save(MkDataNode& node) const
{
	// page bar 제외
	static MkArray<MkHashStr> exceptions;
	if (exceptions.Empty())
	{
		exceptions.PushBack(PageBarNodeName);
	}
	_AddExceptionList(node, SystemKey_NodeExceptions, exceptions);

	// run
	MkWindowBaseNode::Save(node);
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkScrollBarControlNode);

void MkScrollBarControlNode::SetObjectTemplate(MkDataNode& node)
{
	MkWindowBaseNode::SetObjectTemplate(node);

	node.CreateUnit(ObjKey_TotalItemSize, static_cast<int>(0));
	node.CreateUnit(ObjKey_OnePageItemSize, static_cast<int>(0));
	node.CreateUnit(ObjKey_TotalBarLength, 0.f);
	node.CreateUnit(ObjKey_Horizontal, false);
	node.CreateUnit(ObjKey_ItemPosition, static_cast<int>(0));
	node.CreateUnit(ObjKey_WheelScrollLevel, static_cast<int>(MKDEF_WHEEL_SCROLLING_LEVEL));
}

void MkScrollBarControlNode::LoadObject(const MkDataNode& node)
{
	MkWindowBaseNode::LoadObject(node);

	int totalItemSize;
	int onePageItemSize;
	float totalBarLength;
	bool horizontal;
	int itemPosition;
	if (node.GetData(ObjKey_TotalItemSize, totalItemSize, 0) &&
		node.GetData(ObjKey_OnePageItemSize, onePageItemSize, 0) &&
		node.GetData(ObjKey_TotalBarLength, totalBarLength, 0) &&
		node.GetData(ObjKey_Horizontal, horizontal, 0) &&
		node.GetData(ObjKey_ItemPosition, itemPosition, 0))
	{
		_SetScrollBar(GetThemeName(), totalItemSize, onePageItemSize, totalBarLength, horizontal, false, itemPosition);
	}

	// wheel scroll level
	int wheelScrollLevel;
	if (node.GetData(ObjKey_WheelScrollLevel, wheelScrollLevel, 0))
	{
		SetWheelScrollLevel(wheelScrollLevel);
	}
}

void MkScrollBarControlNode::SaveObject(MkDataNode& node) const
{
	MkWindowBaseNode::SaveObject(node);

	node.SetData(ObjKey_TotalItemSize, m_TotalItemSize, 0);
	node.SetData(ObjKey_OnePageItemSize, m_OnePageItemSize, 0);
	node.SetData(ObjKey_TotalBarLength, m_TotalBarLength, 0);
	node.SetData(ObjKey_Horizontal, m_Horizontal, 0);
	node.SetData(ObjKey_ItemPosition, m_CurrentItemPosition, 0);
	node.SetData(ObjKey_WheelScrollLevel, m_WheelScrollLevel, 0);
}

MkScrollBarControlNode::MkScrollBarControlNode(const MkHashStr& name) : MkWindowBaseNode(name)
{
	m_TotalItemSize = 0;
	m_OnePageItemSize = 0;
	m_TotalBarLength = 0.f;
	m_Horizontal = false;
	m_WheelScrollLevel = MKDEF_WHEEL_SCROLLING_LEVEL;
	m_PageBarLength = 0.f;
	m_CurrentItemPosition = 0;
}

void MkScrollBarControlNode::_SetScrollBar
(const MkHashStr& themeName, int totalItemSize, int onePageItemSize, float length, bool horizontal, bool updateTotalBar, int itemPos)
{
	if (length <= 0.f)
		return;

	m_TotalItemSize = totalItemSize;
	m_OnePageItemSize = onePageItemSize;
	m_TotalBarLength = length;
	m_Horizontal = horizontal;

	// total bar
	if (updateTotalBar)
	{
		MkFloat2 totalBarCS((m_Horizontal) ? m_TotalBarLength : BarWidth, (m_Horizontal) ? BarWidth : m_TotalBarLength);

		SetThemeName(themeName);
		SetComponentType(MkWindowThemeData::eCT_FlatBtn);
		SetClientSize(ConvertWindowToClientSize(GetThemeName(), MkWindowThemeData::eCT_FlatBtn, MkHashStr::EMPTY, totalBarCS));
		SetFormState(MkWindowThemeFormData::eS_Normal);
	}

	// page bar
	MkWindowBaseNode* node = NULL;
	if (ChildExist(PageBarNodeName))
	{
		node = dynamic_cast<MkWindowBaseNode*>(GetChildNode(PageBarNodeName));
	}
	else
	{
		node = MkWindowBaseNode::CreateChildNode(this, PageBarNodeName);
		if (node != NULL)
		{
			node->SetLocalDepth(-0.1f); // total bar와 겹치는 것을 피하기 위해 0.1f만큼 앞에 위치
			node->SetThemeName(GetThemeName());
			node->SetComponentType(MkWindowThemeData::eCT_GuideBtn);
			node->SetFormState(MkWindowThemeFormData::eS_Normal);
			node->SetMovableByDragging(true);
		}
	}

	if (node != NULL)
	{
		float pageRate = (m_TotalItemSize <= m_OnePageItemSize) ? 1.f : (static_cast<float>(m_OnePageItemSize) / static_cast<float>(m_TotalItemSize));
		m_PageBarLength = pageRate * m_TotalBarLength;
		MkFloat2 pageBarCS((m_Horizontal) ? m_PageBarLength : BarWidth, (m_Horizontal) ? BarWidth : m_PageBarLength);

		node->SetClientSize(ConvertWindowToClientSize(GetThemeName(), MkWindowThemeData::eCT_GuideBtn, MkHashStr::EMPTY, pageBarCS));
	}
	
	// item position
	m_CurrentItemPosition = itemPos - 1; // itemPos와 다르기만 하면 됨
	_SetItemPosition(itemPos, false, true);
}

void MkScrollBarControlNode::_SetItemPosition(int position, bool filter, bool modifyLocalPos)
{
	if (filter)
	{
		position = _ItemPositionFilter(position);
	}

	if (position != m_CurrentItemPosition)
	{
		if (modifyLocalPos && ChildExist(PageBarNodeName))
		{
			float pageBarPos = static_cast<float>(position) * m_TotalBarLength / static_cast<float>(m_TotalItemSize);
			MkFloat2 localPos((m_Horizontal) ? pageBarPos : 0.f, (m_Horizontal) ? 0.f : (m_TotalBarLength - m_PageBarLength - pageBarPos));
			GetChildNode(PageBarNodeName)->SetLocalPosition(localPos);
		}

		// report event
		MkDataNode arg;
		MkArray<int> buffer(3);
		buffer.PushBack(position);
		buffer.PushBack(GetMax<int>(0, m_CurrentItemPosition)); // 최초 설정시 m_CurrentItemPosition은 -1임
		buffer.PushBack(m_TotalItemSize);
		arg.CreateUnit(MkScrollBarControlNode::ArgKey_NewItemPosOfScrollBar, buffer);
		StartNodeReportTypeEvent(ePA_SNE_ScrollBarMoved, &arg);

		m_CurrentItemPosition = position;
	}
}

int MkScrollBarControlNode::_ItemPositionFilter(int position) const
{
	return Clamp<int>(position, 0, GetMax<int>(0, m_TotalItemSize - m_OnePageItemSize));
}

int MkScrollBarControlNode::_ConvertPageBarPositionToItemPosition(float pageBarPos) const
{
	return (m_TotalBarLength > 0.f) ? _ItemPositionFilter(static_cast<int>(static_cast<float>(m_TotalItemSize) * pageBarPos / m_TotalBarLength)) : 0;
}

float MkScrollBarControlNode::_GetPageBarPosition(const MkSceneNode* node) const
{
	float offset = (m_Horizontal) ? node->GetLocalPosition().x : (m_TotalBarLength - m_PageBarLength - node->GetLocalPosition().y);
	return (offset + 0.001f); // floating 계산의 오차 감안
}

//------------------------------------------------------------------------------------------------//