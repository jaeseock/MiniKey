
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkWindowEventManager.h"
#include "MkS2D_MkScrollBarNode.h"
#include "MkS2D_MkCheckButtonNode.h"
#include "MkS2D_MkEditModeTargetWindow.h"


const static MkHashStr SEL_ICON_NAME = L"__#SelIcon";
const static MkHashStr NODE_TREE_ROOT_NAME = L"__#NodeTreeRoot";
const static MkHashStr NODE_TREE_VSB_NAME = L"__#NodeTreeVSB";

const static MkHashStr NODE_NAME_BTN_NAME = L"__#ChangeName";
const static MkHashStr TOPARENT_BTN_NAME = L"__#ToParent";
const static MkHashStr SAVE_NODE_BTN_NAME = L"__#SaveNode";
const static MkHashStr DEL_NODE_BTN_NAME = L"__#DelNode";
const static MkHashStr ENABLE_NODE_CB_NAME = L"__#EnableCB";
const static MkHashStr WIN_ATTR_BTN_NAME = L"__#WinAttr";

const static MkStr NONE_SEL_CAPTION = L"현재 선택중인 윈도우 없음";
const static MkStr SEL_CAPTION_PREFIX = L"선택 윈도우 : ";

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

		// 같은 그룹의 윈도우 선택. 선택만 변경. 둘 다 NULL인 경우는 없으므로 m_TargetNode가 존재함을 보장
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
		// 다른 그룹의 윈도우 선택. 새로 구성
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
			nodeNamePrefix->SetDecoString(L"이름 :");
			nodeNamePrefix->SetLocalPosition(MkFloat2(currentCtrlPos.x, currentCtrlPos.y + ctrlToTextRectOffset));
			nodeNamePrefix->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);

			currentCtrlPos.x += nodeNamePrefix->GetLocalSize().x + 6.f;

			// name btn
			float nameBtnWidth = m_ClientRect.size.x - currentCtrlPos.x - MKDEF_DEF_BTN_WIDTH * 3.f - MKDEF_CTRL_MARGIN * 4.f;
			m_NodeNameBtn = __CreateWindowPreset(bgNode, NODE_NAME_BTN_NAME, themeName, eS2D_WPC_RootButton, MkFloat2(nameBtnWidth, MKDEF_DEF_CTRL_HEIGHT));
			if (m_NodeNameBtn != NULL)
			{
				m_NodeNameBtn->SetLocalPosition(currentCtrlPos);
				m_NodeNameBtn->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
				m_NodeNameBtn->SetEnable(false);
			}

			// to parent
			m_ToParentNodeBtn = __CreateWindowPreset(bgNode, TOPARENT_BTN_NAME, themeName, eS2D_WPC_NormalButton, MkFloat2(MKDEF_DEF_BTN_WIDTH, MKDEF_DEF_CTRL_HEIGHT));
			if (m_ToParentNodeBtn != NULL)
			{
				currentCtrlPos.x += nameBtnWidth + MKDEF_CTRL_MARGIN;

				m_ToParentNodeBtn->SetLocalPosition(currentCtrlPos);
				m_ToParentNodeBtn->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
				m_ToParentNodeBtn->SetPresetComponentCaption(themeName, CaptionDesc(L"└ 위로"));
				m_ToParentNodeBtn->SetEnable(false);
			}

			// save node
			m_SaveNodeBtn =
				__CreateWindowPreset(bgNode, SAVE_NODE_BTN_NAME, themeName, eS2D_WPC_OKButton, MkFloat2(MKDEF_DEF_BTN_WIDTH, MKDEF_DEF_CTRL_HEIGHT));
			if (m_SaveNodeBtn != NULL)
			{
				currentCtrlPos.x += MKDEF_DEF_BTN_WIDTH + MKDEF_CTRL_MARGIN;

				m_SaveNodeBtn->SetLocalPosition(currentCtrlPos);
				m_SaveNodeBtn->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
				m_SaveNodeBtn->SetPresetComponentCaption(themeName, CaptionDesc(L"저 장"));
				m_SaveNodeBtn->SetEnable(false);
			}

			// delete node
			m_DeleteNodeBtn =
				__CreateWindowPreset(bgNode, DEL_NODE_BTN_NAME, themeName, eS2D_WPC_CancelButton, MkFloat2(MKDEF_DEF_BTN_WIDTH, MKDEF_DEF_CTRL_HEIGHT));
			if (m_SaveNodeBtn != NULL)
			{
				currentCtrlPos.x += MKDEF_DEF_BTN_WIDTH + MKDEF_CTRL_MARGIN;

				m_DeleteNodeBtn->SetLocalPosition(currentCtrlPos);
				m_DeleteNodeBtn->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
				m_DeleteNodeBtn->SetPresetComponentCaption(themeName, CaptionDesc(L"삭 제"));
				m_DeleteNodeBtn->SetEnable(false);
			}
		}

		// node pos
		currentCtrlPos.x = MKDEF_CTRL_MARGIN;
		currentCtrlPos.y -= MKDEF_DEF_CTRL_HEIGHT + MKDEF_CTRL_MARGIN;

		MkSRect* nodePosPrefix = bgNode->CreateSRect(L"__#NodePosPrefix");
		if (nodePosPrefix != NULL)
		{
			nodePosPrefix->SetDecoString(L"로컬 위치 :");
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

		currentCtrlPos.x += 70.f;
		MkSRect* nodeAABRPrefix = bgNode->CreateSRect(L"__#NodeWSizePrefix");
		if (nodeAABRPrefix != NULL)
		{
			nodeAABRPrefix->SetDecoString(L"월드 크기 :");
			nodeAABRPrefix->SetLocalPosition(MkFloat2(currentCtrlPos.x, currentCtrlPos.y + ctrlToTextRectOffset));
			nodeAABRPrefix->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);

			m_NodeWSizeRect = bgNode->CreateSRect(L"__#NodeWSize");
			if (m_NodeWSizeRect != NULL)
			{
				currentCtrlPos.x += nodeAABRPrefix->GetLocalSize().x + 6.f;

				m_NodeWSizeRect->SetLocalPosition(MkFloat2(currentCtrlPos.x, currentCtrlPos.y + ctrlToTextRectOffset));
				m_NodeWSizeRect->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
			}
		}

		currentCtrlPos.x += 70.f;
		m_EnableCB = new MkCheckButtonNode(ENABLE_NODE_CB_NAME);
		if (m_EnableCB != NULL)
		{
			m_EnableCB->CreateCheckButton(themeName, CaptionDesc(L"활성화 여부"), false);
			m_EnableCB->SetLocalPosition(MkFloat2(currentCtrlPos.x, currentCtrlPos.y + 1));
			m_EnableCB->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
			m_EnableCB->SetEnable(false);
			bgNode->AttachChildNode(m_EnableCB);
		}

		float attrBtnWidth = MKDEF_DEF_BTN_WIDTH * 2.f + MKDEF_CTRL_MARGIN;
		m_WinAttrBtn = __CreateWindowPreset(bgNode, WIN_ATTR_BTN_NAME, themeName, eS2D_WPC_NormalButton, MkFloat2(attrBtnWidth, MKDEF_DEF_CTRL_HEIGHT));
		if (m_WinAttrBtn != NULL)
		{
			currentCtrlPos.x = m_ClientRect.size.x - MKDEF_CTRL_MARGIN - attrBtnWidth;

			m_WinAttrBtn->SetLocalPosition(currentCtrlPos);
			m_WinAttrBtn->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
			m_WinAttrBtn->SetPresetComponentCaption(themeName, CaptionDesc(L"윈도우 속성"));
			m_WinAttrBtn->SetEnable(false);
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
		// node tree scroll bar에는 속하지 않아도 높이는 만족할 경우
		const MkFloatRect& sbAABR = m_NodeTreeScrollBar->GetWorldAABR();
		if ((position.y > sbAABR.position.y) && (position.y < (sbAABR.position.y + sbAABR.size.y)))
		{
			if (!sbAABR.CheckGridIntersection(position))
			{
				m_NodeTreeScrollBar->HitEventMouseWheelMove(delta, position); // 가상으로 event를 보내 줌
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
			else if (evt.node->GetNodeName() == TOPARENT_BTN_NAME)
			{
				if (m_TargetNode != NULL) // TOPARENT_BTN_NAME 버튼이 enable 되었다는 것 자체가 부모 윈도우가 존재함을 증명
				{
					MK_WIN_EVENT_MGR.SetTargetWindowNode(dynamic_cast<MkBaseWindowNode*>(m_TargetNode->GetParentNode()));
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
						MK_WIN_EVENT_MGR.SaveCurrentTargetWindowNode(filePath); // UpdateAll() 호출 이후 저장이 정확하기 때문에 mgr에 위임
					}
				}
			}
			else if (evt.node->GetNodeName() == DEL_NODE_BTN_NAME)
			{
				if (m_TargetNode != NULL)
				{
					if (m_TargetNode->CheckRootWindow())
					{
						MkHashStr nodeName = m_TargetNode->GetNodeName();
						MK_WIN_EVENT_MGR.RemoveWindow(nodeName); // 여기서 모든 후속사항을 다 처리
					}
					else
					{
						MkSceneNode* parentNode = m_TargetNode->GetParentNode();
						MkBaseWindowNode* parentWindow = NULL;
						if (parentNode != NULL)
						{
							parentNode->DetachChildNode(m_TargetNode->GetNodeName());
							if (parentNode->GetNodeType() >= eS2D_SNT_BaseWindowNode)
							{
								parentWindow = dynamic_cast<MkBaseWindowNode*>(parentNode);
							}
						}

						delete m_TargetNode;
						MK_WIN_EVENT_MGR.SetTargetWindowNode(NULL);

						if (parentWindow != NULL)
						{
							MK_WIN_EVENT_MGR.SetTargetWindowNode(parentWindow);
						}
					}
				}
			}
			else if (evt.node->GetNodeName() == WIN_ATTR_BTN_NAME)
			{
				if (m_TargetNode != NULL)
				{
					MK_WIN_EVENT_MGR.OpenWindowAttributeSystemWindow(m_TargetNode);
				}
			}
		}
		break;

	case MkSceneNodeFamilyDefinition::eCheckOn:
		{
			if ((evt.node->GetNodeName() == ENABLE_NODE_CB_NAME) && (m_TargetNode != NULL))
			{
				m_TargetNode->SetEnable(true);
			}
		}
		break;
	case MkSceneNodeFamilyDefinition::eCheckOff:
		{
			if ((evt.node->GetNodeName() == ENABLE_NODE_CB_NAME) && (m_TargetNode != NULL))
			{
				m_TargetNode->SetEnable(false);
			}
		}
		break;
	}
}

void MkEditModeTargetWindow::UpdateNode(void)
{
	if (GetVisible())
	{
		if (m_NodePositionRect != NULL)
		{
			MkStr msg = (m_TargetNode == NULL) ?
				L"(--, --)" : MkStr(MkInt2(static_cast<int>(m_TargetNode->GetLocalPosition().x), static_cast<int>(m_TargetNode->GetLocalPosition().y)));

			if (msg != m_LastNodePositionStr)
			{
				m_NodePositionRect->SetDecoString(msg);
				m_LastNodePositionStr = msg;
			}
		}

		if (m_NodeWSizeRect != NULL)
		{
			MkStr msg = (m_TargetNode == NULL) ?
				L"(--, --)" : MkStr(MkInt2(static_cast<int>(m_TargetNode->GetWorldAABR().size.x), static_cast<int>(m_TargetNode->GetWorldAABR().size.y)));

			if (msg != m_LastNodeWSizeStr)
			{
				m_NodeWSizeRect->SetDecoString(msg);
				m_LastNodeWSizeStr = msg;
			}
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

	m_ToParentNodeBtn = NULL;
	m_NodeNameBtn = NULL;
	m_SaveNodeBtn = NULL;
	m_DeleteNodeBtn = NULL;
	m_NodePositionRect = NULL;
	m_NodeWSizeRect = NULL;
	m_EnableCB = NULL;
	m_WinAttrBtn = NULL;
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
			// root window부터 tree 구성
			rootWindow = m_TargetNode->GetRootWindow();

			m_NodeTreeSrc.Reserve(rootWindow->__CountTotalWindowBasedChildren() + 1);
			rootWindow->__BuildInformationTree(m_NodeTreeRoot, 0, m_NodeTreeSrc);

			// target node와 매칭되는 item에 선택 아이콘 추가
			_SetSelectIcon(true);
		}

		// scroll bar에 item 세팅
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

	bool nodeEnable = (m_TargetNode != NULL);

	CaptionDesc capDesc((nodeEnable) ? (SEL_CAPTION_PREFIX + m_TargetNode->GetNodeName().GetString()) : NONE_SEL_CAPTION);
	SetPresetComponentCaption(themeName, capDesc);

	if (m_NodeNameBtn != NULL)
	{
		CaptionDesc capDesc((nodeEnable) ? m_TargetNode->GetNodeName().GetString() : MkStr::Null);
		m_NodeNameBtn->SetPresetComponentCaption(themeName, capDesc, eRAP_LeftCenter, MkFloat2(5.f, 0.f));
		m_NodeNameBtn->SetEnable(nodeEnable);
	}

	if (m_ToParentNodeBtn != NULL)
	{
		bool parentExist = false;
		if (nodeEnable)
		{
			if (!m_TargetNode->CheckRootWindow())
			{
				MkSceneNode* parentNode = m_TargetNode->GetParentNode();
				if ((parentNode != NULL) && (parentNode->GetNodeType() >= eS2D_SNT_BaseWindowNode))
				{
					MkBaseWindowNode* parentWindow = dynamic_cast<MkBaseWindowNode*>(parentNode);
					parentExist = (parentWindow != NULL);
				}
			}
		}
		m_ToParentNodeBtn->SetEnable(parentExist);
	}

	if (m_SaveNodeBtn != NULL)
	{
		m_SaveNodeBtn->SetEnable(nodeEnable);
	}
	if (m_DeleteNodeBtn != NULL)
	{
		m_DeleteNodeBtn->SetEnable(nodeEnable);
	}
	if (m_EnableCB != NULL)
	{
		m_EnableCB->SetCheck((nodeEnable) ? m_TargetNode->GetEnable() : false);
		m_EnableCB->SetEnable(nodeEnable);
	}
	if (m_WinAttrBtn != NULL)
	{
		m_WinAttrBtn->SetEnable(nodeEnable);
	}

	m_LastNodePositionStr.Clear();
	m_LastNodeWSizeStr.Clear();
}

//------------------------------------------------------------------------------------------------//
