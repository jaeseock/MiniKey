
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkWindowManagerNode.h"
#include "MkPA_MkScrollBarControlNode.h"
#include "MkPA_MkScenePortalNode.h"


const MkHashStr MkScenePortalNode::MaskingPanelName(MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"Mask");
const MkHashStr MkScenePortalNode::SampleBackgroundName(MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"Sample");

const MkHashStr MkScenePortalNode::HScrollBarName(MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"HSB");
const MkHashStr MkScenePortalNode::VScrollBarName(MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"VSB");

const MkHashStr MkScenePortalNode::ObjKey_DestWindowMgr(L"DestWindowMgr");

//------------------------------------------------------------------------------------------------//

MkScenePortalNode* MkScenePortalNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkScenePortalNode* node = __TSI_SceneNodeDerivedInstanceOp<MkScenePortalNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkScenePortalNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkScenePortalNode::SetScenePortal(const MkHashStr& themeName, const MkFloat2& region, MkSceneNode* destNode)
{
	SetThemeName(themeName);

	m_ClientRect.size = region;
	
	// destination
	SetDestinationNode(destNode);

	// scroll bar는 생성해 크기와 위치만 맞추어 놓음
	// pixel과 item을 1:1 비례로 삼음
	// total item size는 update시 반영
	MkScrollBarControlNode* hScrollBar = ChildExist(HScrollBarName) ?
		dynamic_cast<MkScrollBarControlNode*>(GetChildNode(HScrollBarName)) : MkScrollBarControlNode::CreateChildNode(this, HScrollBarName);

	if (hScrollBar != NULL)
	{
		hScrollBar->SetHorizontalScrollBar(GetThemeName(), 1, static_cast<int>(m_WindowRect.size.x), region.x);
		hScrollBar->SetAlignmentPosition(eRAP_LeftUnder);
		hScrollBar->SetVisible(false);
	}

	MkScrollBarControlNode* vScrollBar = ChildExist(VScrollBarName) ?
		dynamic_cast<MkScrollBarControlNode*>(GetChildNode(VScrollBarName)) : MkScrollBarControlNode::CreateChildNode(this, VScrollBarName);

	if (vScrollBar != NULL)
	{
		vScrollBar->SetVerticalScrollBar(GetThemeName(), 1, static_cast<int>(m_WindowRect.size.y), region.y);
		vScrollBar->SetAlignmentPosition(eRAP_RMostBottom);
		vScrollBar->SetVisible(false);
	}
}

void MkScenePortalNode::SetDestinationNode(MkSceneNode* destNode)
{
	// 기존 자체 window mgr 삭제
	if (destNode != m_DestWindowManagerNode)
	{
		MK_DELETE(m_DestWindowManagerNode);
	}

	// 새 target 반영
	m_DestinationNode = destNode;

	// client size가 존재 하면 window size로 함(MaskingPanelName이 없을 수 있으므로 자동계산은 불가)
	if (!m_ClientRect.size.IsZero())
	{
		m_WindowRect.size = m_ClientRect.size;
	}

	// panel 처리
	if (m_DestinationNode == NULL)
	{
		DeletePanel(MaskingPanelName);

		// masking panel이 없으면 아무것도 보이지 않으므로 예시용 sample을 넣어 줌
		MkWindowThemedNode* sampleNode = MkWindowThemedNode::CreateChildNode(this, SampleBackgroundName);
		if (sampleNode != NULL)
		{
			sampleNode->SetThemeName(GetThemeName());
			sampleNode->SetComponentType(MkWindowThemeData::eCT_RedOutlineZone);
			sampleNode->SetClientSize(ConvertWindowToClientSize(GetThemeName(), MkWindowThemeData::eCT_RedOutlineZone, MkHashStr::EMPTY, m_WindowRect.size));
			sampleNode->SetFormState(MkWindowThemeFormData::eS_Default);
		}
	}
	else
	{
		// 영역 설정
		MkInt2 iRegion = MkInt2(static_cast<int>(m_WindowRect.size.x), static_cast<int>(m_WindowRect.size.y));

		// masking panel 생성
		MkPanel& panel = CreatePanel(MaskingPanelName, m_DestinationNode, iRegion);
		panel.SetLocalDepth(-0.1f); // 배경과 겹치지 않도록 0.1f만큼 앞으로 이동

		// sample bg 삭제
		RemoveChildNode(SampleBackgroundName);
	}
}

MkWindowManagerNode* MkScenePortalNode::CreateScenePortal(const MkHashStr& themeName, const MkFloat2& region)
{
	// 기존 portal 정보 삭제 및 재구성
	SetScenePortal(themeName, region, NULL);

	// 생성 및 지정
	m_DestWindowManagerNode = MkWindowManagerNode::CreateChildNode(NULL, MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + MkStr(L"DM:") + GetNodeName().GetString());
	SetDestinationNode(m_DestWindowManagerNode);

	return m_DestWindowManagerNode;
}

void MkScenePortalNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	// scroll bar가 이동되었으면 destination node에 반영 후 event 소멸
	if ((m_DestinationNode != NULL) && (eventType == ePA_SNE_ScrollBarMoved) && (path.GetSize() == 1) && (argument != NULL))
	{
		MkArray<int> buffer;
		if (argument->GetData(MkScrollBarControlNode::ArgKey_NewItemPosOfScrollBar, buffer) && (buffer.GetSize() == 3))
		{
			float movement = static_cast<float>(buffer[0] - buffer[1]); // new - last
			MkFloat2 localPos = m_DestinationNode->GetLocalPosition();

			if (path[0] == HScrollBarName)
			{
				localPos.x -= movement;
				m_DestinationNode->SetLocalPosition(localPos);
				return;
			}
			else if (path[0] == VScrollBarName)
			{
				localPos.y += movement;
				m_DestinationNode->SetLocalPosition(localPos);
				return;
			}
		}
	}

	// wheel이 움직였고 해당 window가 자신이고 현재 vertical scroll bar가 보이고 있으면 통지, event 소멸
	if ((eventType == ePA_SNE_WheelMoved) && path.Empty() && ChildExist(VScrollBarName))
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

void MkScenePortalNode::Update(double currTime)
{
	if ((m_DestinationNode != NULL) && m_WindowRect.size.IsPositive())
	{
		// window manager?
		if (m_DestinationNode->IsDerivedFrom(ePA_SNT_WindowManagerNode) && PanelExist(MaskingPanelName))
		{
			MkWindowManagerNode* mgrNode = dynamic_cast<MkWindowManagerNode*>(m_DestinationNode);
			MkPanel* maskingPanel = GetPanel(MaskingPanelName);
			if ((mgrNode != NULL) && (maskingPanel != NULL))
			{
				// 자신과 screen panel의 transform만 먼저 계산
				m_Transform.Update((m_ParentNodePtr == NULL) ? NULL : m_ParentNodePtr->__GetTransformPtr());
				maskingPanel->__Update(&m_Transform, currTime);

				// legacy 적용
				const MkFloatRect& aabr = maskingPanel->GetWorldAABR();
				mgrNode->__SetScenePortalLegacy
					(m_CursorInside, MkIntRect(static_cast<int>(aabr.position.x), static_cast<int>(aabr.position.y), static_cast<int>(aabr.size.x), static_cast<int>(aabr.size.y)));
			}
		}

		// update destination
		m_DestinationNode->Update(currTime);

		// update scroll bar
		MkScrollBarControlNode* hScrollBar = dynamic_cast<MkScrollBarControlNode*>(GetChildNode(HScrollBarName));
		MkScrollBarControlNode* vScrollBar = dynamic_cast<MkScrollBarControlNode*>(GetChildNode(VScrollBarName));
		if ((hScrollBar != NULL) || (vScrollBar != NULL))
		{
			// destination의 전체 영역을 구함
			MkFloat2 destRegion;
			if (m_DestinationNode->IsDerivedFrom(ePA_SNT_VisualPatternNode))
			{
				MkVisualPatternNode* vpNode = dynamic_cast<MkVisualPatternNode*>(m_DestinationNode);
				if (vpNode != NULL) // MkVisualPatternNode 파생 객체는 window size
				{
					destRegion = vpNode->GetWindowRect().size;
				}
			}

			if (destRegion.IsZero()) // 지정된 영역이 없으면 total AABR의 size
			{
				destRegion = m_DestinationNode->GetTotalAABR().size;
			}

			_UpdateScrollBar(hScrollBar, static_cast<int>(destRegion.x));
			_UpdateScrollBar(vScrollBar, static_cast<int>(destRegion.y));
		}
	}

	MkWindowBaseNode::Update(currTime);
}

void MkScenePortalNode::Clear(void)
{
	m_DestinationNode = NULL;
	MK_DELETE(m_DestWindowManagerNode);

	MkWindowBaseNode::Clear();
}

void MkScenePortalNode::Save(MkDataNode& node) const
{
	// masking panel 제외
	static MkArray<MkHashStr> panelExceptions;
	if (panelExceptions.Empty())
	{
		panelExceptions.PushBack(MaskingPanelName);
	}
	_AddExceptionList(node, SystemKey_PanelExceptions, panelExceptions);

	// sample bg, v/h scroll bar 제외
	static MkArray<MkHashStr> nodeExceptions;
	if (nodeExceptions.Empty())
	{
		nodeExceptions.PushBack(SampleBackgroundName);
		nodeExceptions.PushBack(HScrollBarName);
		nodeExceptions.PushBack(VScrollBarName);
	}
	_AddExceptionList(node, SystemKey_NodeExceptions, nodeExceptions);

	// run
	MkWindowBaseNode::Save(node);
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkScenePortalNode);

void MkScenePortalNode::SetObjectTemplate(MkDataNode& node)
{
	MkWindowBaseNode::SetObjectTemplate(node);
}

void MkScenePortalNode::LoadObject(const MkDataNode& node)
{
	MkWindowBaseNode::LoadObject(node);

	// 자체 window mgr이 존재하면 구성
	if (node.ChildExist(ObjKey_DestWindowMgr))
	{
		MkWindowManagerNode* windowMgr = CreateScenePortal(GetThemeName(), MkFloat2(m_ClientRect.size));
		if (windowMgr != NULL)
		{
			windowMgr->Load(*node.GetChildNode(ObjKey_DestWindowMgr));
		}
	}

	// 아니면 기본 frame 구성
	if (m_DestWindowManagerNode == NULL)
	{
		SetScenePortal(GetThemeName(), MkFloat2(m_ClientRect.size), NULL);
	}
}

void MkScenePortalNode::SaveObject(MkDataNode& node) const
{
	MkWindowBaseNode::SaveObject(node);

	// 자체 window mgr이 존재하면 출력
	if (m_DestWindowManagerNode != NULL)
	{
		MkDataNode* destNode = node.CreateChildNode(ObjKey_DestWindowMgr);
		if (destNode != NULL)
		{
			m_DestWindowManagerNode->Save(*destNode);
		}
	}
}

MkScenePortalNode::MkScenePortalNode(const MkHashStr& name) : MkWindowBaseNode(name)
{
	m_DestinationNode = NULL;
	m_DestWindowManagerNode = NULL;
}

void MkScenePortalNode::_UpdateScrollBar(MkScrollBarControlNode* node, int destSize)
{
	if (node != NULL)
	{
		bool available = (destSize > node->GetOnePageItemSize());
		if (available)
		{
			node->SetTotalItemSize(destSize);
		}
		node->SetVisible(available);
	}
}

//------------------------------------------------------------------------------------------------//
