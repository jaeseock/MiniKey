
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkWindowManagerNode.h"
#include "MkPA_MkScrollBarControlNode.h"
#include "MkPA_MkScenePortalNode.h"


const MkHashStr MkScenePortalNode::MaskingPanelName(MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"Mask");

const MkHashStr MkScenePortalNode::HScrollBarName(MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"HSB");
const MkHashStr MkScenePortalNode::VScrollBarName(MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"VSB");

//------------------------------------------------------------------------------------------------//

MkScenePortalNode* MkScenePortalNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkScenePortalNode* node = __TSI_SceneNodeDerivedInstanceOp<MkScenePortalNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkScenePortalNode ���� ����") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkScenePortalNode::SetScenePortal(const MkHashStr& themeName, const MkFloat2& region, MkSceneNode* destNode)
{
	m_WindowRect.size = m_ClientRect.size = region;
	
	// destination
	SetDestinationNode(destNode);

	// scroll bar�� ������ ũ��� ��ġ�� ���߾� ����
	// pixel�� item�� 1:1 ��ʷ� ����
	// total item size�� update�� �ݿ�
	MkScrollBarControlNode* hScrollBar = ChildExist(HScrollBarName) ?
		dynamic_cast<MkScrollBarControlNode*>(GetChildNode(HScrollBarName)) : MkScrollBarControlNode::CreateChildNode(this, HScrollBarName);

	if (hScrollBar != NULL)
	{
		hScrollBar->SetHorizontalScrollBar(themeName, 1, static_cast<int>(m_WindowRect.size.x), region.x);
		hScrollBar->SetAlignmentPosition(eRAP_LeftUnder);
		hScrollBar->SetVisible(false);
	}

	MkScrollBarControlNode* vScrollBar = ChildExist(VScrollBarName) ?
		dynamic_cast<MkScrollBarControlNode*>(GetChildNode(VScrollBarName)) : MkScrollBarControlNode::CreateChildNode(this, VScrollBarName);

	if (vScrollBar != NULL)
	{
		vScrollBar->SetVerticalScrollBar(themeName, 1, static_cast<int>(m_WindowRect.size.y), region.y);
		vScrollBar->SetAlignmentPosition(eRAP_RMostBottom);
		vScrollBar->SetVisible(false);
	}
}

void MkScenePortalNode::SetDestinationNode(MkSceneNode* destNode)
{
	m_DestinationNode = destNode;

	if (m_DestinationNode == NULL)
	{
		DeletePanel(MaskingPanelName);
	}
	else
	{
		MkInt2 iRegion(static_cast<int>(m_WindowRect.size.x), static_cast<int>(m_WindowRect.size.y));

		// masking panel ����
		MkPanel& panel = CreatePanel(MaskingPanelName, m_DestinationNode, iRegion);
		panel.SetLocalDepth(-0.1f); // ���� ��ġ�� �ʵ��� 0.1f��ŭ ������ �̵�

		// window manager�� target region���� �ڽ��� window rect�� ����
		if (m_DestinationNode->IsDerivedFrom(ePA_SNT_WindowManagerNode))
		{
			MkWindowManagerNode* mgrNode = dynamic_cast<MkWindowManagerNode*>(m_DestinationNode);
			if (mgrNode != NULL)
			{
				mgrNode->SetTargetRegion(iRegion);
				mgrNode->SetScenePortalBind(true);
			}
		}
	}
}

MkWindowManagerNode* MkScenePortalNode::SetScenePortal(const MkHashStr& themeName, const MkFloat2& region)
{
	SetScenePortal(themeName, region, NULL);
	return CreateWindowManagerAsDestinationNode();
}

MkWindowManagerNode* MkScenePortalNode::CreateWindowManagerAsDestinationNode(void)
{
	// window mgr/destination node�� �����ϸ� ���� �� �ʱ�ȭ
	MK_DELETE(m_DestWindowManagerNode);
	SetDestinationNode(NULL);

	// ���� �� ����
	m_DestWindowManagerNode = MkWindowManagerNode::CreateChildNode(NULL, MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + MkStr(L"DM:") + GetNodeName().GetString());
	SetDestinationNode(m_DestWindowManagerNode);

	return m_DestWindowManagerNode;
}

void MkScenePortalNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	// scroll bar�� �̵��Ǿ����� destination node�� �ݿ� �� event �Ҹ�
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

	// wheel�� �������� �ش� window�� �ڽ��̰� ���� vertical scroll bar�� ���̰� ������ ����, event �Ҹ�
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
		if (m_DestinationNode->IsDerivedFrom(ePA_SNT_WindowManagerNode))
		{
			MkWindowManagerNode* mgrNode = dynamic_cast<MkWindowManagerNode*>(m_DestinationNode);
			if (mgrNode != NULL)
			{
				// �ڽ��� transform�� ���� �ݿ�
				m_Transform.Update((m_ParentNodePtr == NULL) ? NULL : m_ParentNodePtr->__GetTransformPtr());
				const MkFloat2& worldPos = m_Transform.GetWorldPosition();

				// input pivot���� �ڽ��� world position�� ����
				mgrNode->SetInputPivotPosition(MkInt2(static_cast<int>(worldPos.x), static_cast<int>(worldPos.y)));

				// cursor�� ������ ���� ��츸 input update ���
				if (m_CursorInside)
				{
					mgrNode->ValidateInputAtThisFrame();
				}
			}
		}

		// update destination
		m_DestinationNode->Update(currTime);

		// update scroll bar
		MkScrollBarControlNode* hScrollBar = dynamic_cast<MkScrollBarControlNode*>(GetChildNode(HScrollBarName));
		MkScrollBarControlNode* vScrollBar = dynamic_cast<MkScrollBarControlNode*>(GetChildNode(VScrollBarName));
		if ((hScrollBar != NULL) || (vScrollBar != NULL))
		{
			// destination�� ��ü ������ ����
			MkFloat2 destRegion;
			if (m_DestinationNode->IsDerivedFrom(ePA_SNT_VisualPatternNode))
			{
				MkVisualPatternNode* vpNode = dynamic_cast<MkVisualPatternNode*>(m_DestinationNode);
				if (vpNode != NULL) // MkVisualPatternNode �Ļ� ��ü�� window size
				{
					destRegion = vpNode->GetWindowRect().size;
				}
			}

			if (destRegion.IsZero()) // ������ ������ ������ total AABR�� size
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
