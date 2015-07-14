
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
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkScenePortalNode ���� ����") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkScenePortalNode::SetScenePortal(const MkHashStr& themeName, const MkFloat2& region, MkSceneNode* destNode)
{
	SetThemeName(themeName);

	m_ClientRect.size = region;
	
	// destination
	SetDestinationNode(destNode);

	// scroll bar�� ������ ũ��� ��ġ�� ���߾� ����
	// pixel�� item�� 1:1 ��ʷ� ����
	// total item size�� update�� �ݿ�
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
	// ���� ��ü window mgr ����
	if (destNode != m_DestWindowManagerNode)
	{
		MK_DELETE(m_DestWindowManagerNode);
	}

	// �� target �ݿ�
	m_DestinationNode = destNode;

	// client size�� ���� �ϸ� window size�� ��(MaskingPanelName�� ���� �� �����Ƿ� �ڵ������ �Ұ�)
	if (!m_ClientRect.size.IsZero())
	{
		m_WindowRect.size = m_ClientRect.size;
	}

	// panel ó��
	if (m_DestinationNode == NULL)
	{
		DeletePanel(MaskingPanelName);

		// masking panel�� ������ �ƹ��͵� ������ �����Ƿ� ���ÿ� sample�� �־� ��
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
		// ���� ����
		MkInt2 iRegion = MkInt2(static_cast<int>(m_WindowRect.size.x), static_cast<int>(m_WindowRect.size.y));

		// masking panel ����
		MkPanel& panel = CreatePanel(MaskingPanelName, m_DestinationNode, iRegion);
		panel.SetLocalDepth(-0.1f); // ���� ��ġ�� �ʵ��� 0.1f��ŭ ������ �̵�

		// sample bg ����
		RemoveChildNode(SampleBackgroundName);
	}
}

MkWindowManagerNode* MkScenePortalNode::CreateScenePortal(const MkHashStr& themeName, const MkFloat2& region)
{
	// ���� portal ���� ���� �� �籸��
	SetScenePortal(themeName, region, NULL);

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
		if (m_DestinationNode->IsDerivedFrom(ePA_SNT_WindowManagerNode) && PanelExist(MaskingPanelName))
		{
			MkWindowManagerNode* mgrNode = dynamic_cast<MkWindowManagerNode*>(m_DestinationNode);
			MkPanel* maskingPanel = GetPanel(MaskingPanelName);
			if ((mgrNode != NULL) && (maskingPanel != NULL))
			{
				// �ڽŰ� screen panel�� transform�� ���� ���
				m_Transform.Update((m_ParentNodePtr == NULL) ? NULL : m_ParentNodePtr->__GetTransformPtr());
				maskingPanel->__Update(&m_Transform, currTime);

				// legacy ����
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

void MkScenePortalNode::Save(MkDataNode& node) const
{
	// masking panel ����
	static MkArray<MkHashStr> panelExceptions;
	if (panelExceptions.Empty())
	{
		panelExceptions.PushBack(MaskingPanelName);
	}
	_AddExceptionList(node, SystemKey_PanelExceptions, panelExceptions);

	// sample bg, v/h scroll bar ����
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

	// ��ü window mgr�� �����ϸ� ����
	if (node.ChildExist(ObjKey_DestWindowMgr))
	{
		MkWindowManagerNode* windowMgr = CreateScenePortal(GetThemeName(), MkFloat2(m_ClientRect.size));
		if (windowMgr != NULL)
		{
			windowMgr->Load(*node.GetChildNode(ObjKey_DestWindowMgr));
		}
	}

	// �ƴϸ� �⺻ frame ����
	if (m_DestWindowManagerNode == NULL)
	{
		SetScenePortal(GetThemeName(), MkFloat2(m_ClientRect.size), NULL);
	}
}

void MkScenePortalNode::SaveObject(MkDataNode& node) const
{
	MkWindowBaseNode::SaveObject(node);

	// ��ü window mgr�� �����ϸ� ���
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
