
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkWindowEventManager.h"
#include "MkS2D_MkScrollBarNode.h"
#include "MkS2D_MkCheckButtonNode.h"
#include "MkS2D_MkSpreadButtonNode.h"
#include "MkS2D_MkTabWindowNode.h"
#include "MkS2D_MkEditBoxNode.h"
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

const static MkHashStr TAB_WINDOW_NAME = L"__#Window";
const static MkHashStr TAB_COMPONENT_NAME = L"__#Component";
const static MkHashStr TAB_TAGS_NAME = L"__#Tags";

const static MkHashStr EB_HISTORY_USAGE_CB_BTN_NAME = L"__#HistoryUsage";
const static MkHashStr SCROLL_BAR_LENGTH_EB_NAME = L"__#SBarLength";
const static MkHashStr SPREAD_BTN_NEW_KEY_EB_NAME = L"__#SBtnNewKey";
const static MkHashStr SPREAD_BTN_CREATE_NEW_BTN_NAME = L"__#SBCreateNewBtn";
const static MkHashStr TAB_WND_NEW_TAB_EB_NAME = L"__#TWNewTabEB";
const static MkHashStr TAB_WND_CREATE_NEW_BTN_NAME = L"__#TWNewTabBtn";
const static MkHashStr TAB_WND_TAB_SIZE_X_EB_NAME = L"__#TWTabSizeX";
const static MkHashStr TAB_WND_TAB_SIZE_Y_EB_NAME = L"__#TWTabSizeY";
const static MkHashStr TAB_WND_REG_SIZE_X_EB_NAME = L"__#TWRegSizeX";
const static MkHashStr TAB_WND_REG_SIZE_Y_EB_NAME = L"__#TWRegSizeY";
const static MkHashStr TAB_WND_TARGET_TAB_SB_NAME = L"__#TWTargetTab";
const static MkHashStr TAB_WND_USAGE_CB_BTN_NAME = L"__#TWUsage";
const static MkHashStr TAB_WND_DEL_TAB_BTN_NAME = L"__#TWDelTab";
const static MkHashStr TAB_WND_MOVE_TAB_TO_PREV_BTN_NAME = L"__#TWMovePrev";
const static MkHashStr TAB_WND_MOVE_TAB_TO_NEXT_BTN_NAME = L"__#TWMoveNext";

const static MkHashStr THEME_SPREAD_BTN_NAME = L"__#Themes";
const static MkHashStr THEME_DELETE_BTN_NAME = L"__#DelTheme";
const static MkHashStr PRESET_SIZE_X_EB_NAME = L"__#SizeX";
const static MkHashStr PRESET_SIZE_Y_EB_NAME = L"__#SizeY";
const static MkHashStr ENABLE_CLOSE_BTN_NAME = L"__#EnClose";
const static MkHashStr ADD_DEF_THEME_BTN_NAME = L"__#AddTheme";
const static MkHashStr SET_STATE_RES_BTN_NAME = L"__#StateRes";

const static MkHashStr TAG_SPREAD_BTN_NAME = L"__#TargetTag";
const static MkHashStr TAG_ICON_UK = L"__#IconUK";
const static MkHashStr TAG_CAPTION_UK = L"__#CapUK";
const static MkHashStr SET_TAG_BTN_NAME = L"__#SetTag";
const static MkHashStr DEL_TAG_BTN_NAME = L"__#DelTag";
const static MkHashStr TAG_ALIGN_LEFT_BTN_NAME = L"__#TagAlignLeft";
const static MkHashStr TAG_ALIGN_CENTER_BTN_NAME = L"__#TagAlignCenter";
const static MkHashStr TAG_ALIGN_RIGHT_BTN_NAME = L"__#TagAlignRight";
const static MkHashStr TAG_PX_MOVE_LL_BTN_NAME = L"__#TagPxMoveLL";
const static MkHashStr TAG_PX_MOVE_LH_BTN_NAME = L"__#TagPxMoveLH";
const static MkHashStr TAG_PX_MOVE_RL_BTN_NAME = L"__#TagPxMoveRL";
const static MkHashStr TAG_PX_MOVE_RH_BTN_NAME = L"__#TagPxMoveRH";
const static MkHashStr TAG_PX_MOVE_UL_BTN_NAME = L"__#TagPxMoveUL";
const static MkHashStr TAG_PX_MOVE_UH_BTN_NAME = L"__#TagPxMoveUH";
const static MkHashStr TAG_PX_MOVE_DL_BTN_NAME = L"__#TagPxMoveDL";
const static MkHashStr TAG_PX_MOVE_DH_BTN_NAME = L"__#TagPxMoveDH";

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

			_UpdateControlsByTargetNode();
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

		m_TabWindow = new MkTabWindowNode(L"__#TabWnd");
		if (m_TabWindow != NULL)
		{
			MkFloat2 tabBodySize(m_ClientRect.size.x - MKDEF_CTRL_MARGIN * 2.f, 300.f);
			m_TabWindow->CreateTabRoot(themeName, MkTabWindowNode::eLeftside, MkFloat2(100.f, 20.f), tabBodySize);
			m_TabWindow->SetLocalPosition(MkFloat2(MKDEF_CTRL_MARGIN, MARGIN));
			m_TabWindow->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);

			ItemTagInfo ti;
			ti.captionDesc.SetString(L"윈도우");
			MkBaseWindowNode* wndWin = m_TabWindow->AddTab(TAB_WINDOW_NAME, ti);
			if (wndWin != NULL)
			{
				float startY = tabBodySize.y - MKDEF_CTRL_MARGIN - MKDEF_DEF_CTRL_HEIGHT;
				m_TabWnd_Desc = wndWin->CreateSRect(L"__#Desc");
				m_TabWnd_Desc->SetLocalPosition(MkFloat2(MKDEF_CTRL_MARGIN, startY));
				m_TabWnd_Desc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);

				for (int i=eS2D_SNT_ControlWindowNodeBegin; i<eS2D_SNT_ControlWindowNodeEnd; ++i)
				{
					eS2D_SceneNodeType snType = static_cast<eS2D_SceneNodeType>(i);

					MkBaseWindowNode* wndRegion = new MkBaseWindowNode(MkStr(i));
					wndWin->AttachChildNode(wndRegion);
					m_TabWnd_RegionTable.Create(snType, wndRegion);

					float btnPosY = startY;

					switch (snType)
					{
					case eS2D_SNT_SpreadButtonNode:
						{
							m_TabWnd_SpreadBtn_UniqueKey = new MkEditBoxNode(SPREAD_BTN_NEW_KEY_EB_NAME);
							m_TabWnd_SpreadBtn_UniqueKey->CreateEditBox(themeName, MkFloat2(200.f, MKDEF_DEF_CTRL_HEIGHT), MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkStr::Null, false);
							m_TabWnd_SpreadBtn_UniqueKey->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
							wndRegion->AttachChildNode(m_TabWnd_SpreadBtn_UniqueKey);

							MkBaseWindowNode* btnWin = __CreateWindowPreset(wndRegion, SPREAD_BTN_CREATE_NEW_BTN_NAME, themeName, eS2D_WPC_OKButton, MkFloat2(200.f, MKDEF_DEF_CTRL_HEIGHT));
							btnWin->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN * 2.f + m_TabWnd_SpreadBtn_UniqueKey->GetPresetComponentSize().x, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
							btnWin->SetPresetComponentCaption(themeName, CaptionDesc(L"-> 이름으로 하위 버튼 생성"));
						}
						break;

					case eS2D_SNT_CheckButtonNode: break;

					case eS2D_SNT_ScrollBarNode:
						{
							MkSRect* sizeDesc = wndRegion->CreateSRect(L"__#SizeDesc");
							sizeDesc->SetLocalPosition(MkFloat2(MKDEF_CTRL_MARGIN, btnPosY + 3));
							sizeDesc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
							sizeDesc->SetDecoString(L"길이 :");

							m_TabWnd_ScrollBar_Length = new MkEditBoxNode(SCROLL_BAR_LENGTH_EB_NAME);
							m_TabWnd_ScrollBar_Length->CreateEditBox(themeName, MkFloat2(40.f, MKDEF_DEF_CTRL_HEIGHT), MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkStr::Null, false);
							m_TabWnd_ScrollBar_Length->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN + sizeDesc->GetLocalSize().x + 6.f, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
							wndRegion->AttachChildNode(m_TabWnd_ScrollBar_Length);
						}
						break;

					case eS2D_SNT_EditBoxNode:
						{
							m_TabWnd_EditBox_HistoryUsage = new MkCheckButtonNode(EB_HISTORY_USAGE_CB_BTN_NAME);
							m_TabWnd_EditBox_HistoryUsage->CreateCheckButton(themeName, CaptionDesc(L"입력 히스토리 기능 사용"), false);
							m_TabWnd_EditBox_HistoryUsage->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
							wndRegion->AttachChildNode(m_TabWnd_EditBox_HistoryUsage);
						}
						break;

					case eS2D_SNT_TabWindowNode:
						{
							m_TabWnd_TabWnd_TabName = new MkEditBoxNode(TAB_WND_NEW_TAB_EB_NAME);
							m_TabWnd_TabWnd_TabName->CreateEditBox(themeName, MkFloat2(200.f, MKDEF_DEF_CTRL_HEIGHT), MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkStr::Null, false);
							m_TabWnd_TabWnd_TabName->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
							wndRegion->AttachChildNode(m_TabWnd_TabWnd_TabName);

							MkBaseWindowNode* btnWin = __CreateWindowPreset(wndRegion, TAB_WND_CREATE_NEW_BTN_NAME, themeName, eS2D_WPC_OKButton, MkFloat2(200.f, MKDEF_DEF_CTRL_HEIGHT));
							btnWin->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN * 2.f + m_TabWnd_TabWnd_TabName->GetPresetComponentSize().x, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
							btnWin->SetPresetComponentCaption(themeName, CaptionDesc(L"-> 이름으로 하위 탭 생성"));

							btnPosY -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;
							float tmpX = MKDEF_CTRL_MARGIN;

							MkSRect* sizeDesc = wndRegion->CreateSRect(L"__#TabSD");
							sizeDesc->SetLocalPosition(MkFloat2(tmpX, btnPosY + 3));
							sizeDesc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
							sizeDesc->SetDecoString(L"탭 버튼 크기 :");

							tmpX += sizeDesc->GetLocalSize().x + 6.f;
							m_TabWnd_TabWnd_BtnSizeX = new MkEditBoxNode(TAB_WND_TAB_SIZE_X_EB_NAME);
							m_TabWnd_TabWnd_BtnSizeX->CreateEditBox(themeName, MkFloat2(40.f, MKDEF_DEF_CTRL_HEIGHT), MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkStr::Null, false);
							m_TabWnd_TabWnd_BtnSizeX->SetLocalPosition(MkVec3(tmpX, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
							wndRegion->AttachChildNode(m_TabWnd_TabWnd_BtnSizeX);

							tmpX += m_TabWnd_TabWnd_BtnSizeX->GetPresetComponentSize().x + 6.f;
							m_TabWnd_TabWnd_BtnSizeY = new MkEditBoxNode(TAB_WND_TAB_SIZE_Y_EB_NAME);
							m_TabWnd_TabWnd_BtnSizeY->CreateEditBox(themeName, MkFloat2(40.f, MKDEF_DEF_CTRL_HEIGHT), MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkStr::Null, false);
							m_TabWnd_TabWnd_BtnSizeY->SetLocalPosition(MkVec3(tmpX, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
							wndRegion->AttachChildNode(m_TabWnd_TabWnd_BtnSizeY);

							tmpX += m_TabWnd_TabWnd_BtnSizeY->GetPresetComponentSize().x + MKDEF_CTRL_MARGIN;
							sizeDesc = wndRegion->CreateSRect(L"__#RegSD");
							sizeDesc->SetLocalPosition(MkFloat2(tmpX, btnPosY + 3));
							sizeDesc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
							sizeDesc->SetDecoString(L"영역 크기 :");

							tmpX += sizeDesc->GetLocalSize().x + 6.f;
							m_TabWnd_TabWnd_BodySizeX = new MkEditBoxNode(TAB_WND_REG_SIZE_X_EB_NAME);
							m_TabWnd_TabWnd_BodySizeX->CreateEditBox(themeName, MkFloat2(40.f, MKDEF_DEF_CTRL_HEIGHT), MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkStr::Null, false);
							m_TabWnd_TabWnd_BodySizeX->SetLocalPosition(MkVec3(tmpX, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
							wndRegion->AttachChildNode(m_TabWnd_TabWnd_BodySizeX);

							tmpX += m_TabWnd_TabWnd_BodySizeX->GetPresetComponentSize().x + 6.f;
							m_TabWnd_TabWnd_BodySizeY = new MkEditBoxNode(TAB_WND_REG_SIZE_Y_EB_NAME);
							m_TabWnd_TabWnd_BodySizeY->CreateEditBox(themeName, MkFloat2(40.f, MKDEF_DEF_CTRL_HEIGHT), MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkStr::Null, false);
							m_TabWnd_TabWnd_BodySizeY->SetLocalPosition(MkVec3(tmpX, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
							wndRegion->AttachChildNode(m_TabWnd_TabWnd_BodySizeY);

							btnPosY -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;

							m_TabWnd_TabWnd_TargetTab = new MkSpreadButtonNode(TAB_WND_TARGET_TAB_SB_NAME);
							m_TabWnd_TabWnd_TargetTab->CreateSelectionRootTypeButton(themeName, MkFloat2(m_TabWnd_TabWnd_TabName->GetPresetComponentSize().x, MKDEF_DEF_CTRL_HEIGHT), MkSpreadButtonNode::eDownward);
							m_TabWnd_TabWnd_TargetTab->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
							wndRegion->AttachChildNode(m_TabWnd_TabWnd_TargetTab);

							tmpX = MKDEF_CTRL_MARGIN * 2.f + m_TabWnd_TabWnd_TargetTab->GetPresetComponentSize().x;
							m_TabWnd_TabWnd_Usage = new MkCheckButtonNode(TAB_WND_USAGE_CB_BTN_NAME);
							m_TabWnd_TabWnd_Usage->CreateCheckButton(themeName, CaptionDesc(L"사용여부"), false);
							m_TabWnd_TabWnd_Usage->SetLocalPosition(MkVec3(tmpX, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
							wndRegion->AttachChildNode(m_TabWnd_TabWnd_Usage);

							btnPosY -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;
							btnWin = __CreateWindowPreset(wndRegion, TAB_WND_DEL_TAB_BTN_NAME, themeName, eS2D_WPC_CancelButton, MkFloat2(100.f, MKDEF_DEF_CTRL_HEIGHT));
							btnWin->SetLocalPosition(MkVec3(tmpX, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
							btnWin->SetPresetComponentCaption(themeName, CaptionDesc(L"탭 삭제"));

							btnPosY -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;
							btnWin = __CreateWindowPreset(wndRegion, TAB_WND_MOVE_TAB_TO_PREV_BTN_NAME, themeName, eS2D_WPC_DirLeftButton, MkFloat2(0.f, 0.f));
							btnWin->SetLocalPosition(MkVec3(tmpX, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));

							tmpX += btnWin->GetPresetComponentSize().x + MKDEF_CTRL_MARGIN;
							sizeDesc = wndRegion->CreateSRect(L"__#TabMoveD");
							sizeDesc->SetLocalPosition(MkFloat2(tmpX, btnPosY + 3));
							sizeDesc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
							sizeDesc->SetDecoString(L"탭 순서 변경");

							tmpX += sizeDesc->GetLocalSize().x + MKDEF_CTRL_MARGIN;
							btnWin = __CreateWindowPreset(wndRegion, TAB_WND_MOVE_TAB_TO_NEXT_BTN_NAME, themeName, eS2D_WPC_DirRightButton, MkFloat2(0.f, 0.f));
							btnWin->SetLocalPosition(MkVec3(tmpX, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
						}
						break;
					}
				}
			}

			ti.captionDesc.SetString(L"컴포넌트");
			MkBaseWindowNode* compWin = m_TabWindow->AddTab(TAB_COMPONENT_NAME, ti);
			if (compWin != NULL)
			{
				float btnPosY = tabBodySize.y - MKDEF_CTRL_MARGIN - MKDEF_DEF_CTRL_HEIGHT;

				m_TabComp_Desc = compWin->CreateSRect(L"__#Desc");
				m_TabComp_Desc->SetLocalPosition(MkFloat2(MKDEF_CTRL_MARGIN, btnPosY));
				m_TabComp_Desc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);

				btnPosY -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;
				const float btnWidth = 100.f;

				m_TabComp_DeleteTheme = __CreateWindowPreset(compWin, THEME_DELETE_BTN_NAME, themeName, eS2D_WPC_CancelButton, MkFloat2(btnWidth, MKDEF_DEF_CTRL_HEIGHT));
				m_TabComp_DeleteTheme->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				m_TabComp_DeleteTheme->SetPresetComponentCaption(themeName, CaptionDesc(L"테마 삭제"));

				m_TabComp_EnableCloseBtn = new MkCheckButtonNode(ENABLE_CLOSE_BTN_NAME);
				m_TabComp_EnableCloseBtn->CreateCheckButton(themeName, CaptionDesc(L"닫기 버튼 사용"), false);
				m_TabComp_EnableCloseBtn->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				compWin->AttachChildNode(m_TabComp_EnableCloseBtn);

				m_TabComp_AddDefaultTheme = __CreateWindowPreset(compWin, ADD_DEF_THEME_BTN_NAME, themeName, eS2D_WPC_OKButton, MkFloat2(btnWidth, MKDEF_DEF_CTRL_HEIGHT));
				m_TabComp_AddDefaultTheme->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				m_TabComp_AddDefaultTheme->SetPresetComponentCaption(themeName, CaptionDesc(L"기본 테마 적용"));

				float tmpX = MKDEF_CTRL_MARGIN * 2.f + btnWidth + 10.f;
				MkSRect* sizeDesc = compWin->CreateSRect(L"__#SizeDesc");
				sizeDesc->SetLocalPosition(MkFloat2(tmpX, btnPosY + 3));
				sizeDesc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
				sizeDesc->SetDecoString(L"크기(X,Y) :");

				tmpX += sizeDesc->GetLocalSize().x + 6.f;
				m_TabComp_SizeX = new MkEditBoxNode(PRESET_SIZE_X_EB_NAME);
				m_TabComp_SizeX->CreateEditBox(themeName, MkFloat2(40.f, MKDEF_DEF_CTRL_HEIGHT), MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkStr::Null, false);
				m_TabComp_SizeX->SetLocalPosition(MkVec3(tmpX, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				compWin->AttachChildNode(m_TabComp_SizeX);

				tmpX += m_TabComp_SizeX->GetPresetComponentSize().x + 6.f;
				m_TabComp_SizeY = new MkEditBoxNode(PRESET_SIZE_Y_EB_NAME);
				m_TabComp_SizeY->CreateEditBox(themeName, MkFloat2(40.f, MKDEF_DEF_CTRL_HEIGHT), MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkStr::Null, false);
				m_TabComp_SizeY->SetLocalPosition(MkVec3(tmpX, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				compWin->AttachChildNode(m_TabComp_SizeY);

				btnPosY -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;
				const float selectionBtnWidth = 160.f;

				m_TabComp_ThemeSelection = new MkSpreadButtonNode(THEME_SPREAD_BTN_NAME);
				m_TabComp_ThemeSelection->CreateSelectionRootTypeButton(themeName, MkFloat2(selectionBtnWidth, MKDEF_DEF_CTRL_HEIGHT), MkSpreadButtonNode::eDownward);
				m_TabComp_ThemeSelection->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				compWin->AttachChildNode(m_TabComp_ThemeSelection);

				MkArray<MkHashStr> themeList;
				if (MK_WR_PRESET.GetAllThemeName(themeList) > 0)
				{
					MK_INDEXING_LOOP(themeList, i)
					{
						const MkHashStr& currName = themeList[i];
						ti.captionDesc.SetString(currName.GetString());
						m_TabComp_ThemeSelection->AddItem(currName, ti);
					}

					m_TabComp_ThemeSelection->SetTargetItem(themeName);
				}

				m_TabComp_BackgroundState = new MkSpreadButtonNode(L"__#BGState");
				m_TabComp_BackgroundState->CreateSelectionRootTypeButton(themeName, MkFloat2(selectionBtnWidth, MKDEF_DEF_CTRL_HEIGHT), MkSpreadButtonNode::eDownward);
				m_TabComp_BackgroundState->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				compWin->AttachChildNode(m_TabComp_BackgroundState);

				for (unsigned int i=0; i<eS2D_BS_MaxBackgroundState; ++i)
				{
					MkStr currState = MkWindowPreset::GetBackgroundStateKeyword(static_cast<eS2D_BackgroundState>(i)).GetString();
					currState.PopFront(3); // L"__#" 제거
					ti.captionDesc.SetString(currState);
					MkStr indexStr(i);
					m_TabComp_BackgroundState->AddItem(indexStr, ti);

					if (i == 0)
					{
						m_TabComp_BackgroundState->SetTargetItem(indexStr);
					}
				}

				m_TabComp_WindowState = new MkSpreadButtonNode(L"__#WinState");
				m_TabComp_WindowState->CreateSelectionRootTypeButton(themeName, MkFloat2(selectionBtnWidth, MKDEF_DEF_CTRL_HEIGHT), MkSpreadButtonNode::eDownward);
				m_TabComp_WindowState->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				compWin->AttachChildNode(m_TabComp_WindowState);

				for (unsigned int i=0; i<eS2D_WS_MaxWindowState; ++i)
				{
					MkStr currState = MkWindowPreset::GetWindowStateKeyword(static_cast<eS2D_WindowState>(i)).GetString();
					currState.PopFront(3); // L"__#" 제거
					ti.captionDesc.SetString(currState);
					MkStr indexStr(i);
					m_TabComp_WindowState->AddItem(indexStr, ti);

					if (i == 0)
					{
						m_TabComp_WindowState->SetTargetItem(indexStr);
					}
				}

				m_TabComp_SetStateRes = __CreateWindowPreset(compWin, SET_STATE_RES_BTN_NAME, themeName, eS2D_WPC_NormalButton, MkFloat2(btnWidth, MKDEF_DEF_CTRL_HEIGHT));
				m_TabComp_SetStateRes->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN * 2.f + selectionBtnWidth, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				m_TabComp_SetStateRes->SetPresetComponentCaption(themeName, CaptionDesc(L"이미지 설정"));
			}

			ti.captionDesc.SetString(L"태 그");
			MkBaseWindowNode* tagWin = m_TabWindow->AddTab(TAB_TAGS_NAME, ti);
			if (tagWin != NULL)
			{
				m_TabTag_EnableWindows.Reserve(12);
				float btnPosY = tabBodySize.y - MKDEF_CTRL_MARGIN - MKDEF_DEF_CTRL_HEIGHT;

				m_TabTag_Desc = tagWin->CreateSRect(L"__#Desc");
				m_TabTag_Desc->SetLocalPosition(MkFloat2(MKDEF_CTRL_MARGIN, btnPosY));
				m_TabTag_Desc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);

				btnPosY -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;

				m_TabTag_TargetSelection = new MkSpreadButtonNode(TAG_SPREAD_BTN_NAME);
				m_TabTag_TargetSelection->CreateSelectionRootTypeButton(themeName, MkFloat2(160.f, MKDEF_DEF_CTRL_HEIGHT), MkSpreadButtonNode::eDownward);
				m_TabTag_TargetSelection->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				tagWin->AttachChildNode(m_TabTag_TargetSelection);

				ti.captionDesc.SetString(L"[ 아이콘 ]");
				m_TabTag_TargetSelection->AddItem(TAG_ICON_UK, ti);
				ti.captionDesc.SetString(L"[ 캡션 ]");
				m_TabTag_TargetSelection->AddItem(TAG_CAPTION_UK, ti);
				m_TabTag_TargetSelection->SetTargetItem(TAG_ICON_UK);

				const float selectionBtnWidth = m_TabTag_TargetSelection->GetPresetComponentSize().x;
				const float btnWidth = 80.f;
				MkBaseWindowNode* setTag = __CreateWindowPreset(tagWin, SET_TAG_BTN_NAME, themeName, eS2D_WPC_OKButton, MkFloat2(btnWidth, MKDEF_DEF_CTRL_HEIGHT));
				setTag->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN * 2.f + selectionBtnWidth, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				setTag->SetPresetComponentCaption(themeName, CaptionDesc(L"태그 설정"));

				MkBaseWindowNode* delTag = __CreateWindowPreset(tagWin, DEL_TAG_BTN_NAME, themeName, eS2D_WPC_CancelButton, MkFloat2(btnWidth, MKDEF_DEF_CTRL_HEIGHT));
				delTag->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN * 3.f + selectionBtnWidth + btnWidth, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				delTag->SetPresetComponentCaption(themeName, CaptionDesc(L"태그 삭제"));
				m_TabTag_EnableWindows.PushBack(delTag);

				btnPosY -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;

				MkBaseWindowNode* alignLeft = __CreateWindowPreset(tagWin, TAG_ALIGN_LEFT_BTN_NAME, themeName, eS2D_WPC_NormalButton, MkFloat2(btnWidth, MKDEF_DEF_CTRL_HEIGHT));
				alignLeft->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN * 2.f + selectionBtnWidth, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				alignLeft->SetPresetComponentCaption(themeName, CaptionDesc(L"좌측 정렬"));
				m_TabTag_EnableWindows.PushBack(alignLeft);

				MkBaseWindowNode* alignCenter = __CreateWindowPreset(tagWin, TAG_ALIGN_CENTER_BTN_NAME, themeName, eS2D_WPC_NormalButton, MkFloat2(btnWidth, MKDEF_DEF_CTRL_HEIGHT));
				alignCenter->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN * 3.f + selectionBtnWidth + btnWidth, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				alignCenter->SetPresetComponentCaption(themeName, CaptionDesc(L"중앙 정렬"));
				m_TabTag_EnableWindows.PushBack(alignCenter);

				MkBaseWindowNode* alignRight = __CreateWindowPreset(tagWin, TAG_ALIGN_RIGHT_BTN_NAME, themeName, eS2D_WPC_NormalButton, MkFloat2(btnWidth, MKDEF_DEF_CTRL_HEIGHT));
				alignRight->SetLocalPosition(MkVec3(MKDEF_CTRL_MARGIN * 4.f + selectionBtnWidth + btnWidth * 2.f, btnPosY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				alignRight->SetPresetComponentCaption(themeName, CaptionDesc(L"우측 정렬"));
				m_TabTag_EnableWindows.PushBack(alignRight);

				MkFloat2 pivot(MKDEF_CTRL_MARGIN * 2.f + selectionBtnWidth + 42.f, 140.f);

				MkBaseWindowNode* moveLL = __CreateWindowPreset(tagWin, TAG_PX_MOVE_LL_BTN_NAME, themeName, eS2D_WPC_DirLeftButton, MkFloat2(0.f, 0.f));
				const MkFloat2& btnSize = moveLL->GetPresetComponentSize();
				moveLL->SetLocalPosition(MkVec3(pivot.x - btnSize.x - MARGIN, pivot.y, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				m_TabTag_EnableWindows.PushBack(moveLL);
				MkBaseWindowNode* moveLH = __CreateWindowPreset(tagWin, TAG_PX_MOVE_LH_BTN_NAME, themeName, eS2D_WPC_DirLeftButton, MkFloat2(0.f, 0.f));
				moveLH->SetLocalPosition(MkVec3(pivot.x - btnSize.x * 2.f - MARGIN * 2.f, pivot.y, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				m_TabTag_EnableWindows.PushBack(moveLH);

				MkBaseWindowNode* moveRL = __CreateWindowPreset(tagWin, TAG_PX_MOVE_RL_BTN_NAME, themeName, eS2D_WPC_DirRightButton, MkFloat2(0.f, 0.f));
				moveRL->SetLocalPosition(MkVec3(pivot.x + btnSize.x + MARGIN, pivot.y, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				m_TabTag_EnableWindows.PushBack(moveRL);
				MkBaseWindowNode* moveRH = __CreateWindowPreset(tagWin, TAG_PX_MOVE_RH_BTN_NAME, themeName, eS2D_WPC_DirRightButton, MkFloat2(0.f, 0.f));
				moveRH->SetLocalPosition(MkVec3(pivot.x + btnSize.x * 2.f + MARGIN * 2.f, pivot.y, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				m_TabTag_EnableWindows.PushBack(moveRH);

				MkBaseWindowNode* moveUL = __CreateWindowPreset(tagWin, TAG_PX_MOVE_UL_BTN_NAME, themeName, eS2D_WPC_DirUpButton, MkFloat2(0.f, 0.f));
				moveUL->SetLocalPosition(MkVec3(pivot.x, pivot.y + btnSize.y + MARGIN, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				m_TabTag_EnableWindows.PushBack(moveUL);
				MkBaseWindowNode* moveUH = __CreateWindowPreset(tagWin, TAG_PX_MOVE_UH_BTN_NAME, themeName, eS2D_WPC_DirUpButton, MkFloat2(0.f, 0.f));
				moveUH->SetLocalPosition(MkVec3(pivot.x, pivot.y + btnSize.y * 2.f + MARGIN * 2.f, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				m_TabTag_EnableWindows.PushBack(moveUH);

				MkBaseWindowNode* moveDL = __CreateWindowPreset(tagWin, TAG_PX_MOVE_DL_BTN_NAME, themeName, eS2D_WPC_DirDownButton, MkFloat2(0.f, 0.f));
				moveDL->SetLocalPosition(MkVec3(pivot.x, pivot.y - btnSize.y - MARGIN, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				m_TabTag_EnableWindows.PushBack(moveDL);
				MkBaseWindowNode* moveDH = __CreateWindowPreset(tagWin, TAG_PX_MOVE_DH_BTN_NAME, themeName, eS2D_WPC_DirDownButton, MkFloat2(0.f, 0.f));
				moveDH->SetLocalPosition(MkVec3(pivot.x, pivot.y - btnSize.y * 2.f - MARGIN * 2.f, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				m_TabTag_EnableWindows.PushBack(moveDH);
			}

			bgNode->AttachChildNode(m_TabWindow);

			m_TabWindow->SetTabEnable(TAB_WINDOW_NAME, false);
			m_TabWindow->SetTabEnable(TAB_COMPONENT_NAME, false);
			m_TabWindow->SetTabEnable(TAB_TAGS_NAME, false);
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
			else if (evt.node->GetNodeName() == THEME_DELETE_BTN_NAME)
			{
				if (m_TargetNode != NULL)
				{
					m_TargetNode->__ClearCurrentTheme();
					_UpdateTabComponentDesc();
					_UpdateTabComponentControlEnable();
				}
			}
			else if (evt.node->GetNodeName() == ADD_DEF_THEME_BTN_NAME)
			{
				if (m_TargetNode != NULL)
				{
					m_TargetNode->__ApplyDefaultTheme();
					_UpdateTabComponentDesc();
					_UpdateTabComponentControlEnable();
				}
			}
			else if (evt.node->GetNodeName() == SET_STATE_RES_BTN_NAME)
			{
				if (m_TargetNode != NULL)
				{
					const MkHashStr& snKey = MkWindowTypeImageSet::GetImageSetKeyword(MkWindowTypeImageSet::eSingleType, MkWindowTypeImageSet::eL);
					MkSceneNode* stateNode = NULL;
					eS2D_WindowPresetComponent component = m_TargetNode->GetPresetComponentType();
					if (IsBackgroundStateType(component))
					{
						eS2D_BackgroundState state = static_cast<eS2D_BackgroundState>(m_TabComp_BackgroundState->GetTargetItemKey().GetString().ToInteger());
						stateNode = m_TargetNode->GetComponentStateNode(state);
						if (stateNode != NULL)
						{
							MK_WIN_EVENT_MGR.OpenSRectSetterSystemWindow(this, stateNode, snKey, MkSRectSetterSystemWindow::eImageOnly, L"BG");
						}
					}
					else if (IsWindowStateType(component))
					{
						const MkStr sKey = m_TabComp_WindowState->GetTargetItemKey().GetString();
						eS2D_WindowState state = static_cast<eS2D_WindowState>(sKey.ToInteger());
						stateNode = m_TargetNode->GetComponentStateNode(state);
						if (stateNode != NULL)
						{
							MK_WIN_EVENT_MGR.OpenSRectSetterSystemWindow(this, stateNode, snKey, MkSRectSetterSystemWindow::eImageOnly, sKey);
						}
					}
				}
			}
			else if (evt.node->GetNodeName() == SET_TAG_BTN_NAME)
			{
				const MkHashStr& targetKey = m_TabTag_TargetSelection->GetTargetItemKey();
				if (targetKey == TAG_ICON_UK)
				{
					MK_WIN_EVENT_MGR.OpenSRectSetterSystemWindow(this, m_TargetNode, MKDEF_S2D_BASE_WND_ICON_TAG_NAME, MkSRectSetterSystemWindow::eImageOnly, MKDEF_S2D_BASE_WND_ICON_TAG_NAME);
				}
				else if (targetKey == TAG_CAPTION_UK)
				{
					MK_WIN_EVENT_MGR.OpenSRectSetterSystemWindow(this, m_TargetNode, MKDEF_S2D_BASE_WND_NORMAL_CAP_TAG_NAME, MkSRectSetterSystemWindow::eStringOnly, MKDEF_S2D_BASE_WND_NORMAL_CAP_TAG_NAME);
				}
				else
				{
					MK_WIN_EVENT_MGR.OpenSRectSetterSystemWindow(this, m_TargetNode, targetKey, MkSRectSetterSystemWindow::eOneOfSelection, MkStr::Null);
				}
			}
			else if (evt.node->GetNodeName() == DEL_TAG_BTN_NAME)
			{
				ItemTagInfo tagInfo;
				const MkHashStr& targetKey = m_TabTag_TargetSelection->GetTargetItemKey();
				if (targetKey == TAG_ICON_UK)
				{
					if (m_TargetNode->SetPresetComponentItemTag(tagInfo, true, false))
					{
						_UpdateTabTagDesc();
						_UpdateTabTagControlEnable(false);
					}
				}
				else if (targetKey == TAG_CAPTION_UK)
				{
					if ((m_TargetNode->GetNodeType() != eS2D_SNT_SpreadButtonNode) && m_TargetNode->SetPresetComponentItemTag(tagInfo, false, true))
					{
						_UpdateTabTagDesc();
						_UpdateTabTagControlEnable(false);
					}
				}
				else
				{
					if (m_TargetNode->DeleteSRect(targetKey))
					{
						m_TabTag_TargetSelection->RemoveItem(targetKey);
						m_TabTag_TargetSelection->SetTargetItem(TAG_ICON_UK);
						_UpdateTabTagDesc();
						_UpdateTabTagControlEnable(_GetTargetComponentTagExistByTargetTab());
					}
				}
			}
			else if ((m_TargetNode != NULL) && (m_TargetNode->GetPresetComponentType() != eS2D_WPC_None) &&
				((evt.node->GetNodeName() == TAG_ALIGN_LEFT_BTN_NAME) ||
				(evt.node->GetNodeName() == TAG_ALIGN_CENTER_BTN_NAME) ||
				(evt.node->GetNodeName() == TAG_ALIGN_RIGHT_BTN_NAME)))
			{
				eRectAlignmentPosition alignment = eRAP_MiddleCenter;
				if (evt.node->GetNodeName() == TAG_ALIGN_LEFT_BTN_NAME)
				{
					alignment = eRAP_LeftCenter;
				}
				else if (evt.node->GetNodeName() == TAG_ALIGN_RIGHT_BTN_NAME)
				{
					alignment = eRAP_RightCenter;
				}

				const MkHashStr& targetKey = m_TabTag_TargetSelection->GetTargetItemKey();
				if (targetKey == TAG_ICON_UK)
				{
					_AlignTargetTagRect(MKDEF_S2D_BASE_WND_ICON_TAG_NAME, alignment);
				}
				else if (targetKey == TAG_CAPTION_UK)
				{
					_AlignTargetTagRect(MKDEF_S2D_BASE_WND_HIGHLIGHT_CAP_TAG_NAME, alignment);
					_AlignTargetTagRect(MKDEF_S2D_BASE_WND_NORMAL_CAP_TAG_NAME, alignment);
				}
				else
				{
					_AlignTargetTagRect(targetKey, alignment);
				}
			}
			else if ((m_TargetNode != NULL) &&
				((evt.node->GetNodeName() == TAG_PX_MOVE_LL_BTN_NAME) ||
				(evt.node->GetNodeName() == TAG_PX_MOVE_LH_BTN_NAME) ||
				(evt.node->GetNodeName() == TAG_PX_MOVE_RL_BTN_NAME) ||
				(evt.node->GetNodeName() == TAG_PX_MOVE_RH_BTN_NAME) ||
				(evt.node->GetNodeName() == TAG_PX_MOVE_UL_BTN_NAME) ||
				(evt.node->GetNodeName() == TAG_PX_MOVE_UH_BTN_NAME) ||
				(evt.node->GetNodeName() == TAG_PX_MOVE_DL_BTN_NAME) ||
				(evt.node->GetNodeName() == TAG_PX_MOVE_DH_BTN_NAME)))
			{
				MkFloat2 offset;
				if (evt.node->GetNodeName() == TAG_PX_MOVE_LL_BTN_NAME)
				{
					offset.x -= 1.f;
				}
				else if (evt.node->GetNodeName() == TAG_PX_MOVE_LH_BTN_NAME)
				{
					offset.x -= MK_WR_PRESET.GetMargin() * 3.f;
				}
				else if (evt.node->GetNodeName() == TAG_PX_MOVE_RL_BTN_NAME)
				{
					offset.x += 1.f;
				}
				else if (evt.node->GetNodeName() == TAG_PX_MOVE_RH_BTN_NAME)
				{
					offset.x += MK_WR_PRESET.GetMargin() * 3.f;
				}
				else if (evt.node->GetNodeName() == TAG_PX_MOVE_UL_BTN_NAME)
				{
					offset.y += 1.f;
				}
				else if (evt.node->GetNodeName() == TAG_PX_MOVE_UH_BTN_NAME)
				{
					offset.y += MK_WR_PRESET.GetMargin() * 3.f;
				}
				else if (evt.node->GetNodeName() == TAG_PX_MOVE_DL_BTN_NAME)
				{
					offset.y -= 1.f;
				}
				else if (evt.node->GetNodeName() == TAG_PX_MOVE_DH_BTN_NAME)
				{
					offset.y -= MK_WR_PRESET.GetMargin() * 3.f;
				}

				const MkHashStr& targetKey = m_TabTag_TargetSelection->GetTargetItemKey();
				if (targetKey == TAG_ICON_UK)
				{
					_MoveTargetTagRect(MKDEF_S2D_BASE_WND_ICON_TAG_NAME, offset);
				}
				else if (targetKey == TAG_CAPTION_UK)
				{
					_MoveTargetTagRect(MKDEF_S2D_BASE_WND_HIGHLIGHT_CAP_TAG_NAME, offset);
					_MoveTargetTagRect(MKDEF_S2D_BASE_WND_NORMAL_CAP_TAG_NAME, offset);
				}
				else
				{
					_MoveTargetTagRect(targetKey, offset);
				}
			}
		}
		break;

	case MkSceneNodeFamilyDefinition::eCheckOn:
		{
			if (evt.node->GetNodeName() == ENABLE_NODE_CB_NAME)
			{
				if (m_TargetNode != NULL)
				{
					m_TargetNode->SetEnable(true);
				}
			}
			else if (evt.node->GetNodeName() == ENABLE_CLOSE_BTN_NAME)
			{
				if ((m_TargetNode != NULL) && IsTitleStateType(m_TargetNode->GetPresetComponentType()) && (!_CheckTitleHasCloseButton(m_TargetNode)))
				{
					MkBaseWindowNode* closeWindow = __CreateWindowPreset(m_TargetNode, MKDEF_S2D_BASE_WND_CLOSE_BTN_NAME, m_TargetNode->GetPresetThemeName(), eS2D_WPC_CloseButton, MkFloat2(0.f, 0.f));
					if (closeWindow != NULL)
					{
						MkFloat2 localPos =	MkFloatRect(m_TargetNode->GetPresetComponentSize()).GetSnapPosition
							(MkFloatRect(closeWindow->GetPresetComponentSize()), eRAP_RightCenter, MkFloat2(MK_WR_PRESET.GetMargin(), 0.f));
						closeWindow->SetLocalPosition(MkVec3(localPos.x, localPos.y, -MKDEF_BASE_WINDOW_DEPTH_GRID));
						closeWindow->SetAttribute(eIgnoreMovement, true);

						MkBaseWindowNode* targetNode = m_TargetNode;
						MK_WIN_EVENT_MGR.SetTargetWindowNode(NULL);
						MK_WIN_EVENT_MGR.SetTargetWindowNode(targetNode);
					}
				}
			}
		}
		break;
	case MkSceneNodeFamilyDefinition::eCheckOff:
		{
			if ((evt.node->GetNodeName() == ENABLE_NODE_CB_NAME) && (m_TargetNode != NULL))
			{
				m_TargetNode->SetEnable(false);
			}
			else if (evt.node->GetNodeName() == ENABLE_CLOSE_BTN_NAME)
			{
				if ((m_TargetNode != NULL) && IsTitleStateType(m_TargetNode->GetPresetComponentType()) && _CheckTitleHasCloseButton(m_TargetNode))
				{
					MkHashStr cName = MKDEF_S2D_BASE_WND_CLOSE_BTN_NAME;
					if (m_TargetNode->ChildExist(cName))
					{
						MkSceneNode* closeWindow = m_TargetNode->GetChildNode(cName);
						m_TargetNode->DetachChildNode(cName);
						delete closeWindow;

						MkBaseWindowNode* targetNode = m_TargetNode;
						MK_WIN_EVENT_MGR.SetTargetWindowNode(NULL);
						MK_WIN_EVENT_MGR.SetTargetWindowNode(targetNode);
					}
				}
			}
		}
		break;

	case MkSceneNodeFamilyDefinition::eSetTargetItem:
		{
			if (evt.node->GetNodeName() == THEME_SPREAD_BTN_NAME)
			{
				if (m_TargetNode != NULL)
				{
					m_TargetNode->SetPresetThemeName(m_TabComp_ThemeSelection->GetTargetItemKey());
				}
			}
			else if (evt.node->GetNodeName() == TAG_SPREAD_BTN_NAME)
			{
				_UpdateTabTagControlEnable(_GetTargetComponentTagExistByTargetTab());
			}
		}
		break;

	case MkSceneNodeFamilyDefinition::eCommitText:
		{
			if (evt.node->GetNodeName() == PRESET_SIZE_X_EB_NAME)
			{
				if ((m_TargetNode != NULL) && m_TabComp_SizeX->GetText().IsDigit())
				{
					MkFloat2 size = m_TargetNode->GetPresetComponentSize();
					size.x = GetMax<float>(m_TabComp_SizeX->GetText().ToFloat(), MK_WR_PRESET.GetMargin() * 2.f);
					m_TargetNode->SetPresetComponentSize(size);
				}
			}
			else if (evt.node->GetNodeName() == PRESET_SIZE_Y_EB_NAME)
			{
				if ((m_TargetNode != NULL) && m_TabComp_SizeY->GetText().IsDigit())
				{
					MkFloat2 size = m_TargetNode->GetPresetComponentSize();
					size.y = GetMax<float>(m_TabComp_SizeY->GetText().ToFloat(), MK_WR_PRESET.GetMargin() * 2.f);
					m_TargetNode->SetPresetComponentSize(size);
				}
			}
		}
		break;

	//case MkSceneNodeFamilyDefinition::eTabSelection:
	//	{
	//		const MkHashStr& currTab = m_TabWindow->GetCurrentFrontTab();
	//	}
	//	break;
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

				_UpdateControlsByTargetNode();
			}
		}
	}
}

void MkEditModeTargetWindow::SRectInfoUpdated
(MkSceneNode* targetNode, const MkHashStr& rectName, const MkStr& comment, bool flipHorizontal, bool flipVertical, float alpha,
 MkSRect::eSrcType srcType, const MkPathName& imagePath, const MkHashStr& subsetName, const MkStr& decoStr, const MkArray<MkHashStr>& nodeNameAndKey)
{
	if ((srcType == MkSRect::eStaticImage) || (srcType == MkSRect::eCustomDecoStr) || (srcType == MkSRect::eSceneDecoStr))
	{
		// normal SRect
		if (comment.Empty())
		{
			MkSRect* targetRect = targetNode->GetSRect(rectName);
			if (targetRect != NULL)
			{
				targetRect->SetHorizontalReflection(flipHorizontal);
				targetRect->SetVerticalReflection(flipVertical);
				targetRect->SetObjectAlpha(alpha);

				switch (srcType)
				{
				case MkSRect::eStaticImage:
					targetRect->SetTexture(imagePath, subsetName);
					break;
				case MkSRect::eCustomDecoStr:
					targetRect->SetDecoString(decoStr);
					break;
				case MkSRect::eSceneDecoStr:
					targetRect->SetDecoString(nodeNameAndKey);
					break;
				}
			}
		}
		// icon & caption
		else if ((comment == MKDEF_S2D_BASE_WND_ICON_TAG_NAME) || (comment == MKDEF_S2D_BASE_WND_NORMAL_CAP_TAG_NAME))
		{
			MkBaseWindowNode* targetWindow = dynamic_cast<MkBaseWindowNode*>(targetNode);
			if (targetWindow != NULL)
			{
				ItemTagInfo tagInfo;
				switch (srcType)
				{
				case MkSRect::eStaticImage:
					tagInfo.iconPath = imagePath;
					tagInfo.iconSubset = subsetName;
					break;
				case MkSRect::eCustomDecoStr:
					tagInfo.captionDesc.SetString(decoStr);
					break;
				case MkSRect::eSceneDecoStr:
					tagInfo.captionDesc.SetNameList(nodeNameAndKey);
					break;
				}

				targetWindow->SetPresetComponentItemTag(tagInfo, false, false);

				_UpdateTabTagDesc();
				_UpdateTabTagControlEnable(_GetTargetComponentTagExistByTargetTab());
			}
		}
		// component bg image
		else if (comment == L"BG")
		{
			if ((m_TargetNode != NULL) && m_TargetNode->SetFreeImageToBackgroundWindow(imagePath, subsetName, false))
			{
				_UpdateTabComponentControlEnable();
			}
		}
		// component btn image
		else if (comment.IsDigit())
		{
			if (m_TargetNode != NULL)
			{
				eS2D_WindowState state = static_cast<eS2D_WindowState>(comment.ToInteger());
				if (m_TargetNode->SetFreeImageBaseButtonWindow(imagePath, state, subsetName, false))
				{
					_UpdateTabComponentControlEnable();
				}
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
	m_TabWindow = NULL;
	m_TabWnd_Desc = NULL;
	m_TabWnd_EditBox_HistoryUsage = NULL;
	m_TabWnd_ScrollBar_Length = NULL;
	m_TabWnd_SpreadBtn_UniqueKey = NULL;
	m_TabWnd_TabWnd_TabName = NULL;
	m_TabWnd_TabWnd_BtnSizeX = NULL;
	m_TabWnd_TabWnd_BtnSizeY = NULL;
	m_TabWnd_TabWnd_BodySizeX = NULL;
	m_TabWnd_TabWnd_BodySizeY = NULL;
	m_TabWnd_TabWnd_TargetTab = NULL;
	m_TabWnd_TabWnd_Usage = NULL;
	m_TabComp_Desc = NULL;
	m_TabComp_ThemeSelection = NULL;
	m_TabComp_DeleteTheme = NULL;
	m_TabComp_SizeX = NULL;
	m_TabComp_SizeY = NULL;
	m_TabComp_EnableCloseBtn = NULL;
	m_TabComp_BackgroundState = NULL;
	m_TabComp_WindowState = NULL;
	m_TabComp_SetStateRes = NULL;
	m_TabTag_Desc = NULL;
	m_TabTag_TargetSelection = NULL;
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

		_UpdateControlsByTargetNode();
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

void MkEditModeTargetWindow::_UpdateControlsByTargetNode(void)
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

	if (m_TabWindow != NULL)
	{
		bool tabWndEnable = nodeEnable && (m_TargetNode->GetNodeType() >= eS2D_SNT_ControlWindowNodeBegin);
		m_TabWindow->SetTabEnable(TAB_WINDOW_NAME, tabWndEnable);
		m_TabWindow->SetTabEnable(TAB_COMPONENT_NAME, nodeEnable);
		m_TabWindow->SetTabEnable(TAB_TAGS_NAME, nodeEnable);

		if (tabWndEnable)
		{
			for (int i=eS2D_SNT_ControlWindowNodeBegin; i<eS2D_SNT_ControlWindowNodeEnd; ++i)
			{
				eS2D_SceneNodeType snType = static_cast<eS2D_SceneNodeType>(i);
				if (m_TabWnd_RegionTable.Exist(snType))
				{
					bool selected = (snType == m_TargetNode->GetNodeType());
					m_TabWnd_RegionTable[snType]->SetVisible(selected);

					if (selected)
					{
						switch (snType)
						{
						case eS2D_SNT_SpreadButtonNode:
						case eS2D_SNT_CheckButtonNode: break;

						case eS2D_SNT_ScrollBarNode:
							{
								MkScrollBarNode* targetNode = dynamic_cast<MkScrollBarNode*>(m_TargetNode);
								if (targetNode != NULL)
								{
									int length = static_cast<int>(targetNode->GetScrollBarLength());
									m_TabWnd_ScrollBar_Length->SetText(MkStr(length), false);
								}
							}
							break;

						case eS2D_SNT_EditBoxNode:
							{
								MkEditBoxNode* targetNode = dynamic_cast<MkEditBoxNode*>(m_TargetNode);
								if (targetNode != NULL)
								{
									m_TabWnd_EditBox_HistoryUsage->SetCheck(targetNode->SetHistoryUsage());
								}
							}
							break;

						case eS2D_SNT_TabWindowNode:
							{
								MkTabWindowNode* targetNode = dynamic_cast<MkTabWindowNode*>(m_TargetNode);
								if (targetNode != NULL)
								{
									m_TabWnd_TabWnd_BtnSizeX->SetText(MkStr(static_cast<int>(targetNode->GetTabButtonSize().x)), false);
									m_TabWnd_TabWnd_BtnSizeY->SetText(MkStr(static_cast<int>(targetNode->GetTabButtonSize().y)), false);

									m_TabWnd_TabWnd_BodySizeX->SetText(MkStr(static_cast<int>(targetNode->GetTabBodySize().x)), false);
									m_TabWnd_TabWnd_BodySizeY->SetText(MkStr(static_cast<int>(targetNode->GetTabBodySize().y)), false);

									//m_TabWnd_TabWnd_TargetTab->RemoveItem
								}
								
								//MkSpreadButtonNode* m_TabWnd_TabWnd_TargetTab;
								//MkCheckButtonNode* m_TabWnd_TabWnd_Usage;
							}
							break;
						}
					}
				}
			}
		}
		
		if (m_TabTag_TargetSelection != NULL)
		{
			// 아이콘, 태그 외의 리스트 삭제
			const MkArray<MkHashStr>& itemSeq = m_TabTag_TargetSelection->GetItemSequence();
			if (itemSeq.GetSize() > 2)
			{
				for (unsigned int i=2; i<itemSeq.GetSize(); ++i)
				{
					m_TabTag_TargetSelection->RemoveItem(itemSeq[i]);
				}
				m_TabTag_TargetSelection->SetTargetItem(itemSeq[0]);
			}
		}

		if (nodeEnable)
		{
			_UpdateTabComponentDesc();
			_UpdateTabComponentControlEnable();

			if (m_TabTag_TargetSelection != NULL)
			{
				MkArray<MkHashStr> rectList;
				if (_GetNormalSRectList(rectList))
				{
					ItemTagInfo ti;
					MK_INDEXING_LOOP(rectList, i)
					{
						ti.captionDesc.SetString(rectList[i].GetString());
						m_TabTag_TargetSelection->AddItem(rectList[i], ti);
					}
				}
			}

			_UpdateTabTagDesc();
			_UpdateTabTagControlEnable(_GetTargetComponentTagExistByTargetTab());
		}
	}
}

void MkEditModeTargetWindow::_UpdateTabComponentDesc(void)
{
	if ((m_TargetNode != NULL) && (m_TabComp_Desc != NULL))
	{
		MkStr msg;
		msg.Reserve(128);
		eS2D_WindowPresetComponent component = m_TargetNode->GetPresetComponentType();
		if (component == eS2D_WPC_None)
		{
			msg += L"콤포넌트 없음";
		}
		else
		{
			if (m_TargetNode->GetPresetThemeName().Empty())
			{
				msg += L"이미지";
			}
			else
			{
				msg += L"테마";
			}
			msg += L" 기반 ";

			if (IsBackgroundStateType(component))
			{
				msg += L"배경";
			}
			else if (IsTitleStateType(component))
			{
				msg += L"타이틀";
			}
			else if (IsWindowStateType(component))
			{
				msg += L"버튼";
			}

			msg += L" : ";
			msg += m_TargetNode->GetPresetComponentName().GetString();
		}
		m_TabComp_Desc->SetDecoString(msg);
	}
}

void MkEditModeTargetWindow::_UpdateTabComponentControlEnable(void)
{
	if ((m_TargetNode != NULL) &&
		(m_TabComp_ThemeSelection != NULL) && (m_TabComp_DeleteTheme != NULL) && (m_TabComp_SizeX != NULL) &&
		(m_TabComp_SizeY != NULL) && (m_TabComp_EnableCloseBtn != NULL) && (m_TabComp_BackgroundState != NULL) &&
		(m_TabComp_WindowState != NULL) && (m_TabComp_AddDefaultTheme != NULL) && (m_TabComp_SetStateRes != NULL))
	{
		eS2D_WindowPresetComponent component = m_TargetNode->GetPresetComponentType();
		if (component == eS2D_WPC_None)
		{
			m_TabComp_ThemeSelection->SetVisible(false);
			m_TabComp_DeleteTheme->SetVisible(false);
			m_TabComp_SizeX->SetVisible(false);
			m_TabComp_SizeY->SetVisible(false);
			m_TabComp_EnableCloseBtn->SetVisible(false);
			m_TabComp_BackgroundState->SetVisible(false);
			m_TabComp_WindowState->SetVisible(false);
			m_TabComp_AddDefaultTheme->SetVisible(false);
			m_TabComp_SetStateRes->SetVisible(false);
		}
		else
		{
			bool hasTheme = !m_TargetNode->GetPresetThemeName().Empty();

			m_TabComp_ThemeSelection->SetVisible(hasTheme);
			if (hasTheme)
			{
				m_TabComp_ThemeSelection->SetTargetItem(m_TargetNode->GetPresetThemeName());
			}
			
			bool widthOk, heightOk;
			m_TargetNode->CheckPresetComponentSizeAvailable(widthOk, heightOk);

			const MkFloat2& currSize = m_TargetNode->GetPresetComponentSize();
			m_TabComp_SizeX->SetText(MkStr(static_cast<int>(currSize.x)), false);
			m_TabComp_SizeX->SetEnable(hasTheme && widthOk);
			m_TabComp_SizeX->SetVisible(true);
			m_TabComp_SizeY->SetText(MkStr(static_cast<int>(currSize.y)), false);
			m_TabComp_SizeY->SetEnable(hasTheme && heightOk);
			m_TabComp_SizeY->SetVisible(true);

			if (IsBackgroundStateType(component))
			{
				m_TabComp_DeleteTheme->SetVisible(hasTheme);
				m_TabComp_EnableCloseBtn->SetVisible(false);
				m_TabComp_BackgroundState->SetVisible(!hasTheme);
				m_TabComp_WindowState->SetVisible(false);
			}
			else if (IsTitleStateType(component))
			{
				m_TabComp_DeleteTheme->SetVisible(false);
				m_TabComp_EnableCloseBtn->SetCheck(_CheckTitleHasCloseButton(m_TargetNode));
				m_TabComp_EnableCloseBtn->SetVisible(true);
				m_TabComp_BackgroundState->SetVisible(false);
				m_TabComp_WindowState->SetVisible(false);
			}
			else if (IsWindowStateType(component))
			{
				m_TabComp_DeleteTheme->SetVisible(hasTheme);
				m_TabComp_EnableCloseBtn->SetVisible(false);
				m_TabComp_BackgroundState->SetVisible(false);
				m_TabComp_WindowState->SetVisible(!hasTheme);
			}
			
			m_TabComp_AddDefaultTheme->SetVisible(!hasTheme);
			m_TabComp_SetStateRes->SetVisible(!hasTheme);
		}
	}
}

bool MkEditModeTargetWindow::_CheckTitleHasCloseButton(MkBaseWindowNode* targetNode) const
{
	bool ok = false;
	if ((targetNode != NULL) && IsTitleStateType(targetNode->GetPresetComponentType()))
	{
		unsigned int index = 0;
		while (true)
		{
			MkBaseWindowNode* childWnd = m_TargetNode->__GetWindowBasedChild(index);
			if (childWnd != NULL)
			{
				if (childWnd->GetPresetComponentType() == eS2D_WPC_CloseButton)
				{
					ok = true;
					break;
				}
				++index;
			}
			else
				break;
		}
	}
	return ok;
}

void MkEditModeTargetWindow::_UpdateTabTagDesc(void)
{
	if ((m_TargetNode != NULL) && (m_TabTag_Desc != NULL))
	{
		MkStr msg;
		msg.Reserve(128);
		msg += L"아이콘 : ";
		msg += m_TargetNode->ExistSRect(MKDEF_S2D_BASE_WND_ICON_TAG_NAME) ? L"O" : L"X";
		msg += L" , 캡션 : ";
		msg += m_TargetNode->ExistSRect(MKDEF_S2D_BASE_WND_NORMAL_CAP_TAG_NAME) ? L"O" : L"X";

		MkArray<MkHashStr> rectList;
		if (_GetNormalSRectList(rectList))
		{
			msg += L" , 전용 태그 : ";
			msg += rectList.GetSize();
			msg += L" 개";
		}
		m_TabTag_Desc->SetDecoString(msg);
	}
}

bool MkEditModeTargetWindow::_GetTargetComponentTagExistByTargetTab(void)
{
	MkHashStr tagName;
	if ((m_TargetNode != NULL) && (m_TabTag_TargetSelection != NULL))
	{
		const MkHashStr& targetKey = m_TabTag_TargetSelection->GetTargetItemKey();
		if (targetKey == TAG_ICON_UK)
		{
			tagName = MKDEF_S2D_BASE_WND_ICON_TAG_NAME;
		}
		else if (targetKey == TAG_CAPTION_UK)
		{
			tagName = MKDEF_S2D_BASE_WND_NORMAL_CAP_TAG_NAME;
		}
		else
		{
			tagName = targetKey;
		}
	}
	return ((!tagName.Empty()) && m_TargetNode->ExistSRect(tagName));
}

void MkEditModeTargetWindow::_UpdateTabTagControlEnable(bool enable)
{
	MK_INDEXING_LOOP(m_TabTag_EnableWindows, i)
	{
		m_TabTag_EnableWindows[i]->SetEnable(enable);
	}
}

void MkEditModeTargetWindow::_AlignTargetTagRect(const MkHashStr& tagName, eRectAlignmentPosition alignment)
{
	if (m_TargetNode->ExistSRect(tagName))
	{
		MkSRect* rect = m_TargetNode->GetSRect(tagName);
		rect->AlignRect(m_TargetNode->GetPresetComponentSize(), alignment, MkFloat2(MK_WR_PRESET.GetMargin(), 0.f), 0.f);
	}
}

void MkEditModeTargetWindow::_MoveTargetTagRect(const MkHashStr& tagName, const MkFloat2& offset)
{
	if (m_TargetNode->ExistSRect(tagName))
	{
		MkSRect* rect = m_TargetNode->GetSRect(tagName);
		MkFloat2 newPos = rect->GetLocalPosition() + offset;
		rect->SetLocalPosition(newPos);
	}
}

bool MkEditModeTargetWindow::_GetNormalSRectList(MkArray<MkHashStr>& rectList) const
{
	if ((m_TargetNode != NULL) && (m_TargetNode->GetSRectNameList(rectList) > 0))
	{
		rectList.EraseFirstInclusion(MkArraySection(0), MKDEF_S2D_BASE_WND_ICON_TAG_NAME);
		rectList.EraseFirstInclusion(MkArraySection(0), MKDEF_S2D_BASE_WND_HIGHLIGHT_CAP_TAG_NAME);
		rectList.EraseFirstInclusion(MkArraySection(0), MKDEF_S2D_BASE_WND_NORMAL_CAP_TAG_NAME);
	}
	return !rectList.Empty();
}

//------------------------------------------------------------------------------------------------//
