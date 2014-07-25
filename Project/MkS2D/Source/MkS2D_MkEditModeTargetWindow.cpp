
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkWindowEventManager.h"
#include "MkS2D_MkScrollBarNode.h"
#include "MkS2D_MkEditModeTargetWindow.h"


const static MkHashStr SEL_ICON_NAME = L"__#SelIcon";
const static MkHashStr NODE_TREE_ROOT_NAME = L"__#NodeTreeRoot";
const static MkHashStr NODE_TREE_VSB_NAME = L"__#NodeTreeVSB";

const static MkHashStr SAVE_NODE_BTN_NAME = L"__#SaveNode";

const static MkHashStr NODE_NAME_BTN_NAME = L"__#ChangeName";

const static MkStr NONE_SEL_CAPTION = L"���� �������� ������ ����";
const static MkStr SEL_CAPTION_PREFIX = L"���� ������ : ";

#define MKDEF_WINDOW_SIZE MkFloat2(500.f, 768)
#define MKDEF_CTRL_MARGIN 10.f
#define MKDEF_DEF_CTRL_HEIGHT 20.f

#define MKDEF_ONE_PAGE_NODE_ITEM_COUNT 20
#define MKDEF_NODE_TREE_TXT_BEGIN_POS 45.f

#define MKDEF_DEF_BTN_WIDTH 50.f
#define MKDEF_NODE_NAME_BTN_WIDTH 300.f

//------------------------------------------------------------------------------------------------//

void MkEditModeTargetWindow::SetTargetNode(MkBaseWindowNode* targetNode)
{
	if ((m_NodeTreeRoot != NULL) && (targetNode != m_TargetNode))
	{
		MkBaseWindowNode* lastRootWindow = (m_TargetNode == NULL) ? NULL : m_TargetNode->GetRootWindow();
		MkBaseWindowNode* newRootWindow = (targetNode == NULL) ? NULL : targetNode->GetRootWindow();

		// ���� �׷��� ������ ����. ���ø� ����. �� �� NULL�� ���� �����Ƿ� m_TargetNode�� �������� ����
		if (lastRootWindow == newRootWindow)
		{
			_SetSelectIcon(false);

			m_TargetNode = targetNode;
			_SetSelectIcon(true);

			unsigned int gridBegin = m_NodeTreeScrollBar->GetCurrentPagePosition();
			unsigned int gridEnd = gridBegin + MKDEF_ONE_PAGE_NODE_ITEM_COUNT;
			unsigned int targetIndex = m_NodeTreeSrc.FindFirstInclusion(MkArraySection(0), m_TargetNode);
			if ((targetIndex < gridBegin) || (targetIndex >= gridEnd))
			{
				m_NodeTreeScrollBar->SetGridPosition(targetIndex);
				_SetNodeTreeItemPosition(m_NodeTreeScrollBar->GetCurrentPagePosition());
			}

			_SetWindowTitleByTargetNode();
		}
		// �ٸ� �׷��� ������ ����. ���� ����
		else
		{
			m_TargetNode = targetNode;
			_RebuildNodeTree();
		}
	}
}

bool MkEditModeTargetWindow::Initialize(void)
{
	const MkHashStr& themeName = MK_WR_PRESET.GetDefaultThemeName();

	BasicPresetWindowDesc winDesc;
	winDesc.SetStandardDesc(themeName, true, MKDEF_WINDOW_SIZE);
	winDesc.titleType = eS2D_WPC_SystemMsgTitle;
	winDesc.titleCaption = NONE_SEL_CAPTION;
	winDesc.hasIcon = false;
	winDesc.hasCloseButton = true;
	winDesc.hasCancelButton = false;
	winDesc.hasOKButton = false;

	if (CreateBasicWindow(this, MkHashStr::NullHash, winDesc) == NULL)
		return false;

	MkFloat2 currentCtrlPos;
	float ctrlToTextRectOffset = (MKDEF_DEF_CTRL_HEIGHT - MK_FONT_MGR.GetFontHeight(MK_FONT_MGR.DSF())) * 0.5f;

	MkBaseWindowNode* bgNode = dynamic_cast<MkBaseWindowNode*>(GetChildNode(L"Background"));
	if (bgNode != NULL)
	{
		const float MARGIN = MK_WR_PRESET.GetMargin();
		currentCtrlPos.x = MARGIN;

		m_ClientRect = bgNode->GetPresetComponentSize();
		m_ClientRect.size.y -= GetPresetComponentSize().y; // body size - title size

		// node tree
		m_NodeTreeScrollBar = new MkScrollBarNode(NODE_TREE_VSB_NAME);
		if (m_NodeTreeScrollBar != NULL)
		{
			// vertical scroll bar
			bgNode->AttachChildNode(m_NodeTreeScrollBar);
			float sbHeight = static_cast<float>(MKDEF_ONE_PAGE_NODE_ITEM_COUNT) * MKDEF_S2D_NODE_SEL_LINE_SIZE;
			currentCtrlPos.y = m_ClientRect.size.y - MKDEF_CTRL_MARGIN - sbHeight;
			m_NodeTreeScrollBar->CreateScrollBar(themeName, MkScrollBarNode::eVertical, true, sbHeight);
			m_NodeTreeScrollBar->SetLocalPosition(currentCtrlPos);
			m_NodeTreeScrollBar->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
			m_NodeTreeScrollBar->SetPageInfo(0, MKDEF_ONE_PAGE_NODE_ITEM_COUNT, 1, 1);

			// list btn root
			m_NodeTreeRoot = new MkBaseWindowNode(NODE_TREE_ROOT_NAME);
			if (m_NodeTreeRoot != NULL)
			{
				bgNode->AttachChildNode(m_NodeTreeRoot);
				_SetNodeTreeItemPosition(0);
			}
		}

		currentCtrlPos.x = MKDEF_CTRL_MARGIN;
		currentCtrlPos.y -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;

		// node name
		MkSRect* nodeNamePrefix = bgNode->CreateSRect(L"__#NodeNamePrefix");
		if (nodeNamePrefix != NULL)
		{
			// prefix
			nodeNamePrefix->SetDecoString(L"�̸� :");
			nodeNamePrefix->SetLocalPosition(MkFloat2(currentCtrlPos.x, currentCtrlPos.y + ctrlToTextRectOffset));
			nodeNamePrefix->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);

			currentCtrlPos.x += nodeNamePrefix->GetLocalSize().x + 6.f;

			// name btn
			float nameBtnWidth = m_ClientRect.size.x - currentCtrlPos.x - MKDEF_DEF_BTN_WIDTH - MKDEF_CTRL_MARGIN * 2.f;
			m_NodeNameBtn = __CreateWindowPreset(bgNode, NODE_NAME_BTN_NAME, themeName, eS2D_WPC_RootButton, MkFloat2(nameBtnWidth, MKDEF_DEF_CTRL_HEIGHT));
			if (m_NodeNameBtn != NULL)
			{
				m_NodeNameBtn->SetLocalPosition(currentCtrlPos);
				m_NodeNameBtn->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
				m_NodeNameBtn->SetEnable(false);
			}

			// save node
			m_SaveNodeBtn =
				__CreateWindowPreset(bgNode, SAVE_NODE_BTN_NAME, themeName, eS2D_WPC_OKButton, MkFloat2(MKDEF_DEF_BTN_WIDTH, MKDEF_DEF_CTRL_HEIGHT));
			if (m_SaveNodeBtn != NULL)
			{
				currentCtrlPos.x += nameBtnWidth + MKDEF_CTRL_MARGIN;

				m_SaveNodeBtn->SetLocalPosition(currentCtrlPos);
				m_SaveNodeBtn->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
				m_SaveNodeBtn->SetPresetComponentCaption(themeName, CaptionDesc(L"�� ��"));
				m_SaveNodeBtn->SetEnable(false);
			}
		}

		// node pos
		currentCtrlPos.x = MKDEF_CTRL_MARGIN;
		currentCtrlPos.y -= MKDEF_DEF_CTRL_HEIGHT + MKDEF_CTRL_MARGIN;

		MkSRect* nodePosPrefix = bgNode->CreateSRect(L"__#NodePosPrefix");
		if (nodePosPrefix != NULL)
		{
			nodePosPrefix->SetDecoString(L"���� ��ġ :");
			nodePosPrefix->SetLocalPosition(MkFloat2(currentCtrlPos.x, currentCtrlPos.y + ctrlToTextRectOffset));
			nodePosPrefix->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);

			m_NodePositionRect = bgNode->CreateSRect(L"__#NodePosition");
			if (m_NodePositionRect != NULL)
			{
				currentCtrlPos.x += nodePosPrefix->GetLocalSize().x + 6.f;

				m_NodePositionRect->SetLocalPosition(MkFloat2(currentCtrlPos.x, currentCtrlPos.y + ctrlToTextRectOffset));
				m_NodePositionRect->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
			}
		}
	}

	if (m_NodeTreeRoot == NULL)
		return false;

	return true;
}

void MkEditModeTargetWindow::Activate(void)
{
	AlignPosition(MK_WIN_EVENT_MGR.GetRegionRect(), eRAP_RightBottom, MkInt2(0, 0));
}

bool MkEditModeTargetWindow::HitEventMouseWheelMove(int delta, const MkFloat2& position)
{
	if (m_NodeTreeScrollBar != NULL)
	{
		// node tree scroll bar���� ������ �ʾƵ� ���̴� ������ ���
		const MkFloatRect& sbAABR = m_NodeTreeScrollBar->GetWorldAABR();
		if ((position.y > sbAABR.position.y) && (position.y < (sbAABR.position.y + sbAABR.size.y)))
		{
			if (!sbAABR.CheckGridIntersection(position))
			{
				m_NodeTreeScrollBar->HitEventMouseWheelMove(delta, position); // �������� event�� ���� ��
			}
		}
	}
	return false;
}

void MkEditModeTargetWindow::UseWindowEvent(WindowEvent& evt)
{
	switch (evt.type)
	{
	case MkSceneNodeFamilyDefinition::eScrollPositionChanged:
		{
			if (evt.node->GetNodeName() == NODE_TREE_VSB_NAME)
			{
				unsigned int pp = m_NodeTreeScrollBar->GetCurrentPagePosition();
				_SetNodeTreeItemPosition(pp);
			}
		}
		break;

	case MkSceneNodeFamilyDefinition::eCursorLeftRelease:
		{
			if (evt.node->GetNodeName() == NODE_NAME_BTN_NAME)
			{
				if (m_TargetNode != NULL)
				{
					MK_WIN_EVENT_MGR.OpenNodeNameInputSystemWindow(m_TargetNode, this);
				}
			}
			else if (evt.node->GetParentNode()->GetNodeName() == NODE_TREE_ROOT_NAME)// && evt.node->GetNodeName().GetString().IsDigit()
			{
				unsigned int index = evt.node->GetNodeName().GetString().ToUnsignedInteger();
				if (m_NodeTreeSrc.IsValidIndex(index))
				{
					MK_WIN_EVENT_MGR.SetTargetWindowNode(m_NodeTreeSrc[index]);
				}
			}
			else if (evt.node->GetNodeName() == SAVE_NODE_BTN_NAME)
			{
				if (m_TargetNode != NULL)
				{
					MkPathName filePath;
					MkArray<MkStr> exts(2);
					exts.PushBack(MKDEF_S2D_SCENE_FILE_EXT_BINARY);
					exts.PushBack(MKDEF_S2D_SCENE_FILE_EXT_TEXT);
					if (filePath.GetSaveFilePathFromDialog(exts))
					{
						MK_WIN_EVENT_MGR.SaveCurrentTargetWindowNode(filePath); // UpdateAll() ȣ�� ���� ������ ��Ȯ�ϱ� ������ mgr�� ����
					}
				}
			}
		}
		break;
	}
}

void MkEditModeTargetWindow::UpdateNode(void)
{
	if (GetVisible() && (m_NodePositionRect != NULL))
	{
		MkStr currNodePositionStr = (m_TargetNode == NULL) ?
			L"(--, --)" : MkStr(MkInt2(static_cast<int>(m_TargetNode->GetLocalPosition().x), static_cast<int>(m_TargetNode->GetLocalPosition().y)));

		if (currNodePositionStr != m_LastNodePositionStr)
		{
			m_NodePositionRect->SetDecoString(currNodePositionStr);
			m_LastNodePositionStr = currNodePositionStr;
		}
	}

	MkBaseSystemWindow::UpdateNode();
}

void MkEditModeTargetWindow::NodeNameChanged(const MkHashStr& oldName, const MkHashStr& newName, MkSceneNode* targetNode)
{
	if (targetNode != NULL)
	{
		MkBaseWindowNode* targetWindow = dynamic_cast<MkBaseWindowNode*>(targetNode);
		if (targetWindow != NULL)
		{
			unsigned int targetIndex = m_NodeTreeSrc.FindFirstInclusion(MkArraySection(0), targetWindow);
			if (targetIndex != MKDEF_ARRAY_ERROR)
			{
				MkBaseWindowNode* targetListBtn = m_NodeTreeRoot->__GetWindowBasedChild(targetIndex);
				targetWindow->__SetWindowInformation(targetListBtn);

				_SetWindowTitleByTargetNode();
			}
		}
	}
}

MkEditModeTargetWindow::MkEditModeTargetWindow(const MkHashStr& name) : MkBaseSystemWindow(name), MkNodeNameInputListener()
{
	m_TargetNode = NULL;

	m_NodeTreeRoot = NULL;
	m_NodeTreeScrollBar = NULL;

	m_NodeNameBtn = NULL;
	m_SaveNodeBtn = NULL;
	m_NodePositionRect = NULL;
}

void MkEditModeTargetWindow::_RebuildNodeTree(void)
{
	if (m_NodeTreeRoot != NULL)
	{
		m_NodeTreeRoot->Clear();
		m_NodeTreeSrc.Clear();
		
		MkBaseWindowNode* rootWindow = NULL;
		if (m_TargetNode != NULL)
		{
			// root window���� tree ����
			rootWindow = m_TargetNode->GetRootWindow();

			m_NodeTreeSrc.Reserve(rootWindow->__CountTotalWindowBasedChildren() + 1);
			rootWindow->__BuildInformationTree(m_NodeTreeRoot, 0, m_NodeTreeSrc);

			// target node�� ��Ī�Ǵ� item�� ���� ������ �߰�
			_SetSelectIcon(true);
		}

		// scroll bar�� item ����
		if (m_NodeTreeScrollBar != NULL)
		{
			m_NodeTreeScrollBar->SetPageInfo(m_NodeTreeSrc.GetSize());

			unsigned int targetIndex = m_NodeTreeSrc.FindFirstInclusion(MkArraySection(0), m_TargetNode);
			if (targetIndex == MKDEF_ARRAY_ERROR)
			{
				targetIndex = 0;
			}
			m_NodeTreeScrollBar->SetGridPosition(targetIndex);
			_SetNodeTreeItemPosition(m_NodeTreeScrollBar->GetCurrentPagePosition());
		}

		_SetWindowTitleByTargetNode();
	}
}

void MkEditModeTargetWindow::_SetSelectIcon(bool enable)
{
	unsigned int targetIndex = m_NodeTreeSrc.FindFirstInclusion(MkArraySection(0), m_TargetNode);
	MkBaseWindowNode* targetItem = m_NodeTreeRoot->__GetWindowBasedChild(targetIndex);
	if (targetItem != NULL)
	{
		if (enable)
		{
			targetItem->SetPresetComponentIcon
				(SEL_ICON_NAME, eRAP_LMostCenter, MkFloat2(1.f, 0.f), 0.f, MK_WR_PRESET.GetSystemImageFilePath(), MK_WR_PRESET.GetWindowSelectIconSubsetName());
		}
		else
		{
			targetItem->DeleteSRect(SEL_ICON_NAME);
		}
	}
}

void MkEditModeTargetWindow::_SetNodeTreeItemPosition(unsigned int pagePos)
{
	float pos = m_ClientRect.size.y - MKDEF_S2D_NODE_SEL_LINE_SIZE - MKDEF_CTRL_MARGIN + static_cast<float>(pagePos) * MKDEF_S2D_NODE_SEL_LINE_SIZE;
	m_NodeTreeRoot->SetLocalPosition(MkVec3(MKDEF_NODE_TREE_TXT_BEGIN_POS, pos, -MKDEF_BASE_WINDOW_DEPTH_GRID));

	unsigned int itemCount = m_NodeTreeScrollBar->GetTotalPageSize();
	unsigned int endLine = pagePos + MKDEF_ONE_PAGE_NODE_ITEM_COUNT;
	for (unsigned int i=0; i<itemCount; ++i)
	{
		m_NodeTreeRoot->__GetWindowBasedChild(i)->SetVisible((i >= pagePos) && (i < endLine));
	}
}

void MkEditModeTargetWindow::_SetWindowTitleByTargetNode(void)
{
	const MkHashStr& themeName = MK_WR_PRESET.GetDefaultThemeName();

	CaptionDesc capDesc((m_TargetNode == NULL) ? NONE_SEL_CAPTION : (SEL_CAPTION_PREFIX + m_TargetNode->GetNodeName().GetString()));
	SetPresetComponentCaption(themeName, capDesc);

	if (m_NodeNameBtn != NULL)
	{
		CaptionDesc capDesc((m_TargetNode == NULL) ? MkStr::Null : m_TargetNode->GetNodeName().GetString());
		m_NodeNameBtn->SetPresetComponentCaption(themeName, capDesc, eRAP_LeftCenter, MkFloat2(5.f, 0.f));
		m_NodeNameBtn->SetEnable(m_TargetNode != NULL);
	}

	if (m_SaveNodeBtn != NULL)
	{
		m_SaveNodeBtn->SetEnable(m_TargetNode != NULL);
	}

	m_LastNodePositionStr.Clear();
}

//------------------------------------------------------------------------------------------------//
