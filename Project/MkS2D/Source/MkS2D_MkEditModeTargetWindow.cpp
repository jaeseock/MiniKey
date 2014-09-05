
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkWindowEventManager.h"
#include "MkS2D_MkScrollBarNode.h"
#include "MkS2D_MkCheckButtonNode.h"
#include "MkS2D_MkSpreadButtonNode.h"
#include "MkS2D_MkTabWindowNode.h"
#include "MkS2D_MkEditBoxNode.h"
#include "MkS2D_MkWindowOpHelper.h"
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

const static MkHashStr CREATE_WND_TYPE_SB_NAME = L"__#CreateWndType";
const static MkHashStr CREATE_METHOD_SB_NAME = L"__#CreateMethod";
const static MkHashStr CREATE_WINDOW_BTN_NAME = L"__#CreateWindow";

const static MkHashStr CT_KEY_FROM_FILE = L"__#CT_FromFile";

const static MkHashStr CM_KEY_TITLE_OK = L"__#CM_TitleOK";
const static MkHashStr CM_KEY_TITLE_NO = L"__#CM_TitleNO";
const static MkHashStr CM_KEY_BG_TYPE = L"__#CM_BGType";
const static MkHashStr CM_KEY_TITLE_TYPE = L"__#CM_TitleType";
const static MkHashStr CM_KEY_BTN_TYPE = L"__#CM_BtnType";
const static MkHashStr CM_KEY_SELECTION_SB = L"__#CM_SelectionSB";
const static MkHashStr CM_KEY_STATIC_SB = L"__#CM_StaticSB";
const static MkHashStr CM_KEY_BTN_OK_SB = L"__#CM_BtnOkSB";
const static MkHashStr CM_KEY_BTN_NO_SB = L"__#CM_BtnNoSB";
const static MkHashStr CM_KEY_LEFT_AL_TW = L"__#CM_LeftAlignTW";
const static MkHashStr CM_KEY_RIGHT_AL_TW = L"__#CM_RightAlignTW";

const static MkHashStr CD_KEY_NONE = L"__#CD_None";
const static MkHashStr CD_KEY_THEME_BG = L"__#CD_ThemeBG";
const static MkHashStr CD_KEY_IMAGE_BG = L"__#CD_ImageBG";
const static MkHashStr CD_KEY_SPREAD_TO_DOWN = L"__#CD_SpreadD";
const static MkHashStr CD_KEY_SPREAD_TO_RIGHT = L"__#CD_SpreadR";
const static MkHashStr CD_KEY_SPREAD_TO_UP = L"__#CD_SpreadU";
const static MkHashStr CD_KEY_VERTICAL_SB = L"__#CD_VerSB";
const static MkHashStr CD_KEY_HORIZONTAL_SB = L"__#CD_HorSB";

const static MkHashStr TAB_WINDOW_NAME = L"__#Window";
const static MkHashStr TAB_COMPONENT_NAME = L"__#Component";
const static MkHashStr TAB_TAGS_NAME = L"__#Tags";

const static MkHashStr EB_HISTORY_USAGE_CB_BTN_NAME = L"__#HistoryUsage";
const static MkHashStr EB_FONT_TYPE_EB_NAME = L"__#FType";
const static MkHashStr EB_NORMAL_FONT_STATE_EB_NAME = L"__#FStateN";
const static MkHashStr EB_SELECTION_FONT_STATE_EB_NAME = L"__#FStateS";
const static MkHashStr EB_CURSOR_FONT_STATE_EB_NAME = L"__#FStateC";
const static MkHashStr SCROLL_BAR_LENGTH_EB_NAME = L"__#SBarLength";
const static MkHashStr SPREAD_BTN_CREATE_NEW_BTN_NAME = L"__#SBCreateNewBtn";
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
const static MkHashStr FAR_DEPTH_BTN_NAME = L"__#FarDepth";
const static MkHashStr NEAR_DEPTH_BTN_NAME = L"__#NearDepth";
const static MkHashStr ENABLE_CLOSE_BTN_NAME = L"__#EnClose";
const static MkHashStr ADD_THEME_BTN_NAME = L"__#AddTheme";
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

		MkFloat2 defSmallBtnSize(MKDEF_DEF_BTN_WIDTH, MKDEF_DEF_CTRL_HEIGHT);
		MkFloat2 defMiddleBtnSize(100.f, MKDEF_DEF_CTRL_HEIGHT);
		MkFloat2 defLargeBtnSize(200.f, MKDEF_DEF_CTRL_HEIGHT);

		ItemTagInfo ti;

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
			m_NodeNameBtn = MkWindowOpHelper::CreateWindowPreset
				(bgNode, NODE_NAME_BTN_NAME, themeName, eS2D_WPC_RootButton, MkFloat2(nameBtnWidth, MKDEF_DEF_CTRL_HEIGHT), MkStr::Null, currentCtrlPos, -MKDEF_BASE_WINDOW_DEPTH_GRID, false);

			// to parent
			currentCtrlPos.x += nameBtnWidth + MKDEF_CTRL_MARGIN;
			m_ToParentNodeBtn = MkWindowOpHelper::CreateWindowPreset
				(bgNode, TOPARENT_BTN_NAME, themeName, eS2D_WPC_NormalButton, defSmallBtnSize, L"└ 위로", currentCtrlPos, -MKDEF_BASE_WINDOW_DEPTH_GRID, false);

			// save node
			currentCtrlPos.x += MKDEF_DEF_BTN_WIDTH + MKDEF_CTRL_MARGIN;
			m_SaveNodeBtn = MkWindowOpHelper::CreateWindowPreset
				(bgNode, SAVE_NODE_BTN_NAME, themeName, eS2D_WPC_OKButton, defSmallBtnSize, L"저 장", currentCtrlPos, -MKDEF_BASE_WINDOW_DEPTH_GRID, false);

			// delete node
			currentCtrlPos.x += MKDEF_DEF_BTN_WIDTH + MKDEF_CTRL_MARGIN;
			m_DeleteNodeBtn = MkWindowOpHelper::CreateWindowPreset
				(bgNode, DEL_NODE_BTN_NAME, themeName, eS2D_WPC_CancelButton, defSmallBtnSize, L"삭 제", currentCtrlPos, -MKDEF_BASE_WINDOW_DEPTH_GRID, false);
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
		m_EnableCB = MkWindowOpHelper::CreateCheckButton
			(bgNode, ENABLE_NODE_CB_NAME, themeName, false, L"활성화 여부", MkFloat2(currentCtrlPos.x, currentCtrlPos.y + 1), -MKDEF_BASE_WINDOW_DEPTH_GRID, false);

		float attrBtnWidth = MKDEF_DEF_BTN_WIDTH * 2.f + MKDEF_CTRL_MARGIN;
		currentCtrlPos.x = m_ClientRect.size.x - MKDEF_CTRL_MARGIN - attrBtnWidth;
		m_WinAttrBtn = MkWindowOpHelper::CreateWindowPreset
			(bgNode, WIN_ATTR_BTN_NAME, themeName, eS2D_WPC_NormalButton, MkFloat2(attrBtnWidth, MKDEF_DEF_CTRL_HEIGHT), L"윈도우 속성", currentCtrlPos, -MKDEF_BASE_WINDOW_DEPTH_GRID, false);

		currentCtrlPos.x = MKDEF_CTRL_MARGIN;
		currentCtrlPos.y -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;

		m_CreateWindowTypeSB = MkWindowOpHelper::CreateDownwardSelectionSpreadButton
			(bgNode, CREATE_WND_TYPE_SB_NAME, themeName, defMiddleBtnSize, currentCtrlPos, -MKDEF_BASE_WINDOW_DEPTH_GRID * 4.f);

		MkArray<MkHashStr> sbKeyList(eS2D_SNT_ControlWindowNodeEnd + 1);
		MkArray<MkStr> sbDescList(eS2D_SNT_ControlWindowNodeEnd + 1);
		sbKeyList.PushBack(CT_KEY_FROM_FILE);
		sbDescList.PushBack(L"파일로부터");
		sbKeyList.PushBack(MkStr(eS2D_SNT_SceneNode));
		sbDescList.PushBack(L"샘플 세트");
		sbKeyList.PushBack(MkStr(eS2D_SNT_BaseWindowNode));
		sbDescList.PushBack(L"기본 윈도우");
		sbKeyList.PushBack(MkStr(eS2D_SNT_SpreadButtonNode));
		sbDescList.PushBack(L"콤보 박스");
		sbKeyList.PushBack(MkStr(eS2D_SNT_CheckButtonNode));
		sbDescList.PushBack(L"체크 버튼");
		sbKeyList.PushBack(MkStr(eS2D_SNT_ScrollBarNode));
		sbDescList.PushBack(L"스크롤바");
		sbKeyList.PushBack(MkStr(eS2D_SNT_EditBoxNode));
		sbDescList.PushBack(L"에디트 박스");
		sbKeyList.PushBack(MkStr(eS2D_SNT_TabWindowNode));
		sbDescList.PushBack(L"윈도우 탭");
		MkWindowOpHelper::AddItemListToSpreadButton(m_CreateWindowTypeSB, sbKeyList, sbDescList);

		currentCtrlPos.x += m_CreateWindowTypeSB->GetPresetComponentSize().x + MKDEF_CTRL_MARGIN;
		m_CreateMethodSB = MkWindowOpHelper::CreateDownwardSelectionSpreadButton
			(bgNode, CREATE_METHOD_SB_NAME, themeName, defMiddleBtnSize, currentCtrlPos, -MKDEF_BASE_WINDOW_DEPTH_GRID * 4.f);

		currentCtrlPos.x += m_CreateMethodSB->GetPresetComponentSize().x + MKDEF_CTRL_MARGIN;
		m_CreateDetailSB = MkWindowOpHelper::CreateDownwardSelectionSpreadButton
			(bgNode, L"__#CreateDetail", themeName, MkFloat2(140.f, MKDEF_DEF_CTRL_HEIGHT), currentCtrlPos, -MKDEF_BASE_WINDOW_DEPTH_GRID * 4.f);

		_SetCreationMethodSB();
		_SetCreationDetailSB();

		currentCtrlPos.x += m_CreateDetailSB->GetPresetComponentSize().x + MKDEF_CTRL_MARGIN;
		MkBaseWindowNode* crBtnWin = MkWindowOpHelper::CreateWindowPreset
			(bgNode, CREATE_WINDOW_BTN_NAME, themeName, eS2D_WPC_OKButton, MkFloat2(110.f, MKDEF_DEF_CTRL_HEIGHT), L"-> 설정으로 생성", currentCtrlPos);

		m_TabWindow = new MkTabWindowNode(L"__#TabWnd");
		if (m_TabWindow != NULL)
		{
			MkFloat2 tabBodySize(m_ClientRect.size.x - MKDEF_CTRL_MARGIN * 2.f, 295.f);
			m_TabWindow->CreateTabRoot(themeName, MkTabWindowNode::eLeftside, MkFloat2(100.f, 20.f), tabBodySize);
			m_TabWindow->SetLocalPosition(MkFloat2(MKDEF_CTRL_MARGIN, MARGIN));
			m_TabWindow->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);

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
							m_TabWnd_SpreadBtn_UniqueKey = MkWindowOpHelper::CreateEditBox
								(wndRegion, L"__#SBtnNewKey", themeName, defLargeBtnSize, MkStr::Null, MkFloat2(MKDEF_CTRL_MARGIN, btnPosY));

							MkBaseWindowNode* btnWin = MkWindowOpHelper::CreateWindowPreset
								(wndRegion, SPREAD_BTN_CREATE_NEW_BTN_NAME, themeName, eS2D_WPC_OKButton, defLargeBtnSize,
								L"-> 이름으로 하위 버튼 생성", MkFloat2(MKDEF_CTRL_MARGIN * 2.f + m_TabWnd_SpreadBtn_UniqueKey->GetPresetComponentSize().x, btnPosY));
						}
						break;

					case eS2D_SNT_CheckButtonNode: break;

					case eS2D_SNT_ScrollBarNode:
						{
							MkSRect* sizeDesc = wndRegion->CreateSRect(L"__#SizeDesc");
							sizeDesc->SetLocalPosition(MkFloat2(MKDEF_CTRL_MARGIN, btnPosY + 3));
							sizeDesc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
							sizeDesc->SetDecoString(L"길이 :");

							m_TabWnd_ScrollBar_Length = MkWindowOpHelper::CreateEditBox
								(wndRegion, SCROLL_BAR_LENGTH_EB_NAME, themeName, MkFloat2(40.f, MKDEF_DEF_CTRL_HEIGHT), MkStr::Null, MkFloat2(MKDEF_CTRL_MARGIN + sizeDesc->GetLocalSize().x + 6.f, btnPosY));
						}
						break;

					case eS2D_SNT_EditBoxNode:
						{
							m_TabWnd_EditBox_HistoryUsage = MkWindowOpHelper::CreateCheckButton
								(wndRegion, EB_HISTORY_USAGE_CB_BTN_NAME, themeName, false, L"입력 히스토리 기능 사용", MkFloat2(MKDEF_CTRL_MARGIN, btnPosY));

							float ebYPos = btnPosY - MKDEF_CTRL_MARGIN - MKDEF_DEF_CTRL_HEIGHT;
							MkSRect* sDesc = wndRegion->CreateSRect(L"__#FTDesc");
							sDesc->SetLocalPosition(MkFloat2(MKDEF_CTRL_MARGIN, ebYPos + 3));
							sDesc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
							sDesc->SetDecoString(L"폰트 종류(" + MK_WR_PRESET.GetEditBoxFontType().GetString() + L")");
							float ebXPos = sDesc->GetLocalSize().x;

							ebYPos -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;
							sDesc = wndRegion->CreateSRect(L"__#FSNDesc");
							sDesc->SetLocalPosition(MkFloat2(MKDEF_CTRL_MARGIN, ebYPos + 3));
							sDesc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
							sDesc->SetDecoString(L"일반 폰트 형태(" + MK_WR_PRESET.GetEditBoxNormalFontState().GetString() + L")");
							ebXPos = GetMax<float>(sDesc->GetLocalSize().x, ebXPos);

							ebYPos -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;
							sDesc = wndRegion->CreateSRect(L"__#FSSDesc");
							sDesc->SetLocalPosition(MkFloat2(MKDEF_CTRL_MARGIN, ebYPos + 3));
							sDesc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
							sDesc->SetDecoString(L"선택 폰트 형태(" + MK_WR_PRESET.GetEditBoxSelectionFontState().GetString() + L")");
							ebXPos = GetMax<float>(sDesc->GetLocalSize().x, ebXPos);

							ebYPos -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;
							sDesc = wndRegion->CreateSRect(L"__#FSCDesc");
							sDesc->SetLocalPosition(MkFloat2(MKDEF_CTRL_MARGIN, ebYPos + 3));
							sDesc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
							sDesc->SetDecoString(L"커서 폰트 형태(" + MK_WR_PRESET.GetEditBoxCursorFontState().GetString() + L")");
							ebXPos = GetMax<float>(sDesc->GetLocalSize().x, ebXPos);

							ebXPos += MKDEF_CTRL_MARGIN * 2.f;

							ebYPos = btnPosY - MKDEF_CTRL_MARGIN - MKDEF_DEF_CTRL_HEIGHT;
							m_TabWnd_EditBox_FontType = MkWindowOpHelper::CreateEditBox
								(wndRegion, EB_FONT_TYPE_EB_NAME, themeName, defLargeBtnSize, MkStr::Null, MkFloat2(ebXPos, ebYPos));

							ebYPos -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;
							m_TabWnd_EditBox_NormalFontState = MkWindowOpHelper::CreateEditBox
								(wndRegion, EB_NORMAL_FONT_STATE_EB_NAME, themeName, defLargeBtnSize, MkStr::Null, MkFloat2(ebXPos, ebYPos));

							ebYPos -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;
							m_TabWnd_EditBox_SelectionFontState = MkWindowOpHelper::CreateEditBox
								(wndRegion, EB_SELECTION_FONT_STATE_EB_NAME, themeName, defLargeBtnSize, MkStr::Null, MkFloat2(ebXPos, ebYPos));

							ebYPos -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;
							m_TabWnd_EditBox_CursorFontState = MkWindowOpHelper::CreateEditBox
								(wndRegion, EB_CURSOR_FONT_STATE_EB_NAME, themeName, defLargeBtnSize, MkStr::Null, MkFloat2(ebXPos, ebYPos));
						}
						break;

					case eS2D_SNT_TabWindowNode:
						{
							m_TabWnd_TabWnd_TabName = MkWindowOpHelper::CreateEditBox
								(wndRegion, L"__#TWNewTabEB", themeName, defLargeBtnSize, MkStr::Null, MkFloat2(MKDEF_CTRL_MARGIN, btnPosY));

							MkBaseWindowNode* btnWin = MkWindowOpHelper::CreateWindowPreset
								(wndRegion, TAB_WND_CREATE_NEW_BTN_NAME, themeName, eS2D_WPC_OKButton, defLargeBtnSize,
								L"-> 이름으로 하위 탭 생성", MkFloat2(MKDEF_CTRL_MARGIN * 2.f + m_TabWnd_TabWnd_TabName->GetPresetComponentSize().x, btnPosY));

							btnPosY -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;
							float tmpX = MKDEF_CTRL_MARGIN;

							MkSRect* sizeDesc = wndRegion->CreateSRect(L"__#TabSD");
							sizeDesc->SetLocalPosition(MkFloat2(tmpX, btnPosY + 3));
							sizeDesc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
							sizeDesc->SetDecoString(L"탭 버튼 크기 :");

							tmpX += sizeDesc->GetLocalSize().x + 6.f;
							m_TabWnd_TabWnd_BtnSizeX = MkWindowOpHelper::CreateEditBox
								(wndRegion, TAB_WND_TAB_SIZE_X_EB_NAME, themeName, MkFloat2(40.f, MKDEF_DEF_CTRL_HEIGHT), MkStr::Null, MkFloat2(tmpX, btnPosY));

							tmpX += m_TabWnd_TabWnd_BtnSizeX->GetPresetComponentSize().x + 6.f;
							m_TabWnd_TabWnd_BtnSizeY = MkWindowOpHelper::CreateEditBox
								(wndRegion, TAB_WND_TAB_SIZE_Y_EB_NAME, themeName, MkFloat2(40.f, MKDEF_DEF_CTRL_HEIGHT), MkStr::Null, MkFloat2(tmpX, btnPosY));

							tmpX += m_TabWnd_TabWnd_BtnSizeY->GetPresetComponentSize().x + MKDEF_CTRL_MARGIN;
							sizeDesc = wndRegion->CreateSRect(L"__#RegSD");
							sizeDesc->SetLocalPosition(MkFloat2(tmpX, btnPosY + 3));
							sizeDesc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
							sizeDesc->SetDecoString(L"영역 크기 :");

							tmpX += sizeDesc->GetLocalSize().x + 6.f;
							m_TabWnd_TabWnd_BodySizeX = MkWindowOpHelper::CreateEditBox
								(wndRegion, TAB_WND_REG_SIZE_X_EB_NAME, themeName, MkFloat2(40.f, MKDEF_DEF_CTRL_HEIGHT), MkStr::Null, MkFloat2(tmpX, btnPosY));

							tmpX += m_TabWnd_TabWnd_BodySizeX->GetPresetComponentSize().x + 6.f;
							m_TabWnd_TabWnd_BodySizeY = MkWindowOpHelper::CreateEditBox
								(wndRegion, TAB_WND_REG_SIZE_Y_EB_NAME, themeName, MkFloat2(40.f, MKDEF_DEF_CTRL_HEIGHT), MkStr::Null, MkFloat2(tmpX, btnPosY));

							btnPosY -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;

							m_TabWnd_TabWnd_TargetTab = MkWindowOpHelper::CreateDownwardSelectionSpreadButton
								(wndRegion, TAB_WND_TARGET_TAB_SB_NAME, themeName, MkFloat2(m_TabWnd_TabWnd_TabName->GetPresetComponentSize().x, MKDEF_DEF_CTRL_HEIGHT), MkFloat2(MKDEF_CTRL_MARGIN, btnPosY));

							tmpX = MKDEF_CTRL_MARGIN * 2.f + m_TabWnd_TabWnd_TargetTab->GetPresetComponentSize().x;
							m_TabWnd_TabWnd_Usage = MkWindowOpHelper::CreateCheckButton
								(wndRegion, TAB_WND_USAGE_CB_BTN_NAME, themeName, false, L"사용여부", MkFloat2(tmpX, btnPosY));

							btnPosY -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;
							btnWin = MkWindowOpHelper::CreateWindowPreset
								(wndRegion, TAB_WND_DEL_TAB_BTN_NAME, themeName, eS2D_WPC_CancelButton, defMiddleBtnSize, L"탭 삭제", MkFloat2(tmpX, btnPosY));

							btnPosY -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;
							btnWin = MkWindowOpHelper::CreateWindowPreset
								(wndRegion, TAB_WND_MOVE_TAB_TO_PREV_BTN_NAME, themeName, eS2D_WPC_DirLeftButton, MkFloat2(0.f, 0.f), MkStr::Null, MkFloat2(tmpX, btnPosY));

							tmpX += btnWin->GetPresetComponentSize().x + MKDEF_CTRL_MARGIN;
							sizeDesc = wndRegion->CreateSRect(L"__#TabMoveD");
							sizeDesc->SetLocalPosition(MkFloat2(tmpX, btnPosY + 3));
							sizeDesc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
							sizeDesc->SetDecoString(L"탭 순서 변경");

							tmpX += sizeDesc->GetLocalSize().x + MKDEF_CTRL_MARGIN;
							btnWin = MkWindowOpHelper::CreateWindowPreset
								(wndRegion, TAB_WND_MOVE_TAB_TO_NEXT_BTN_NAME, themeName, eS2D_WPC_DirRightButton, MkFloat2(0.f, 0.f), MkStr::Null, MkFloat2(tmpX, btnPosY));
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
				
				m_TabComp_DeleteTheme = MkWindowOpHelper::CreateWindowPreset
					(compWin, THEME_DELETE_BTN_NAME, themeName, eS2D_WPC_CancelButton, defMiddleBtnSize, L"테마 삭제", MkFloat2(MKDEF_CTRL_MARGIN, btnPosY));

				m_TabComp_EnableCloseBtn = MkWindowOpHelper::CreateCheckButton
					(compWin, ENABLE_CLOSE_BTN_NAME, themeName, false, L"닫기 버튼 사용", MkFloat2(MKDEF_CTRL_MARGIN, btnPosY));

				m_TabComp_AddDefaultTheme = MkWindowOpHelper::CreateWindowPreset
					(compWin, ADD_THEME_BTN_NAME, themeName, eS2D_WPC_OKButton, defMiddleBtnSize, L"테마 적용", MkFloat2(MKDEF_CTRL_MARGIN, btnPosY));

				float tmpX = MKDEF_CTRL_MARGIN * 2.f + defMiddleBtnSize.x + 10.f;
				MkSRect* sizeDesc = compWin->CreateSRect(L"__#SizeDesc");
				sizeDesc->SetLocalPosition(MkFloat2(tmpX, btnPosY + 3));
				sizeDesc->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
				sizeDesc->SetDecoString(L"크기(X,Y) :");

				tmpX += sizeDesc->GetLocalSize().x + 6.f;
				m_TabComp_SizeX = MkWindowOpHelper::CreateEditBox
					(compWin, PRESET_SIZE_X_EB_NAME, themeName, MkFloat2(40.f, MKDEF_DEF_CTRL_HEIGHT), MkStr::Null, MkFloat2(tmpX, btnPosY));

				tmpX += m_TabComp_SizeX->GetPresetComponentSize().x + 6.f;
				m_TabComp_SizeY = MkWindowOpHelper::CreateEditBox
					(compWin, PRESET_SIZE_Y_EB_NAME, themeName, MkFloat2(40.f, MKDEF_DEF_CTRL_HEIGHT), MkStr::Null, MkFloat2(tmpX, btnPosY));

				tmpX += m_TabComp_SizeY->GetPresetComponentSize().x + MKDEF_CTRL_MARGIN * 2.f;
				m_TabComp_Depth = compWin->CreateSRect(L"__#DepthDesc");
				m_TabComp_Depth->SetLocalPosition(MkFloat2(tmpX, btnPosY + 3.f));
				m_TabComp_Depth->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
				m_TabComp_Depth->SetDecoString(L"깊이(Z) : 앞(-.---)");

				tmpX += m_TabComp_Depth->GetLocalSize().x + MKDEF_CTRL_MARGIN;
				MkBaseWindowNode* farBtn = MkWindowOpHelper::CreateWindowPreset
					(compWin, FAR_DEPTH_BTN_NAME, themeName, eS2D_WPC_DirUpButton, MkFloat2(0.f, 0.f), MkStr::Null, MkFloat2(tmpX, btnPosY));

				tmpX += farBtn->GetPresetComponentSize().x + MKDEF_CTRL_MARGIN;
				MkBaseWindowNode* nearBtn = MkWindowOpHelper::CreateWindowPreset
					(compWin, NEAR_DEPTH_BTN_NAME, themeName, eS2D_WPC_DirDownButton, MkFloat2(0.f, 0.f), MkStr::Null, MkFloat2(tmpX, btnPosY));

				btnPosY -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;
				const float selectionBtnWidth = 160.f;

				m_TabComp_ThemeSelection = MkWindowOpHelper::CreateDownwardSelectionSpreadButton
					(compWin, THEME_SPREAD_BTN_NAME, themeName, MkFloat2(selectionBtnWidth, MKDEF_DEF_CTRL_HEIGHT), MkFloat2(MKDEF_CTRL_MARGIN, btnPosY));

				MkArray<MkHashStr> themeList;
				if (MK_WR_PRESET.GetAllThemeName(themeList) > 0)
				{
					MkArray<MkStr> emptyDescList;
					MkWindowOpHelper::AddItemListToSpreadButton(m_TabComp_ThemeSelection, themeList, emptyDescList);
					m_TabComp_ThemeSelection->SetTargetItem(themeName);
				}

				m_TabComp_BackgroundState = MkWindowOpHelper::CreateDownwardSelectionSpreadButton
					(compWin, L"__#BGState", themeName, MkFloat2(selectionBtnWidth, MKDEF_DEF_CTRL_HEIGHT), MkFloat2(MKDEF_CTRL_MARGIN, btnPosY));

				sbKeyList.Clear();
				sbKeyList.Reserve(eS2D_BS_MaxBackgroundState);
				sbDescList.Clear();
				sbDescList.Reserve(eS2D_BS_MaxBackgroundState);

				for (unsigned int i=0; i<eS2D_BS_MaxBackgroundState; ++i)
				{
					sbKeyList.PushBack(MkStr(i));

					MkStr currState = MkWindowPreset::GetBackgroundStateKeyword(static_cast<eS2D_BackgroundState>(i)).GetString();
					currState.PopFront(3); // L"__#" 제거
					sbDescList.PushBack(currState);
				}
				MkWindowOpHelper::AddItemListToSpreadButton(m_TabComp_BackgroundState, sbKeyList, sbDescList);

				m_TabComp_WindowState = MkWindowOpHelper::CreateDownwardSelectionSpreadButton
					(compWin, L"__#WinState", themeName, MkFloat2(selectionBtnWidth, MKDEF_DEF_CTRL_HEIGHT), MkFloat2(MKDEF_CTRL_MARGIN, btnPosY));

				sbKeyList.Clear();
				sbKeyList.Reserve(eS2D_WS_MaxWindowState);
				sbDescList.Clear();
				sbDescList.Reserve(eS2D_WS_MaxWindowState);

				for (unsigned int i=0; i<eS2D_WS_MaxWindowState; ++i)
				{
					sbKeyList.PushBack(MkStr(i));

					MkStr currState = MkWindowPreset::GetWindowStateKeyword(static_cast<eS2D_WindowState>(i)).GetString();
					currState.PopFront(3); // L"__#" 제거
					sbDescList.PushBack(currState);
				}
				MkWindowOpHelper::AddItemListToSpreadButton(m_TabComp_WindowState, sbKeyList, sbDescList);

				m_TabComp_SetStateRes = MkWindowOpHelper::CreateWindowPreset
					(compWin, SET_STATE_RES_BTN_NAME, themeName, eS2D_WPC_NormalButton, defMiddleBtnSize, L"이미지 설정", MkFloat2(MKDEF_CTRL_MARGIN * 2.f + selectionBtnWidth, btnPosY));
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

				m_TabTag_TargetSelection = MkWindowOpHelper::CreateDownwardSelectionSpreadButton
					(tagWin, TAG_SPREAD_BTN_NAME, themeName, MkFloat2(160.f, MKDEF_DEF_CTRL_HEIGHT), MkFloat2(MKDEF_CTRL_MARGIN, btnPosY));

				ti.captionDesc.SetString(L"[ 아이콘 ]");
				m_TabTag_TargetSelection->AddItem(TAG_ICON_UK, ti);
				ti.captionDesc.SetString(L"[ 캡션 ]");
				m_TabTag_TargetSelection->AddItem(TAG_CAPTION_UK, ti);
				m_TabTag_TargetSelection->SetTargetItem(TAG_ICON_UK);

				const float selectionBtnWidth = m_TabTag_TargetSelection->GetPresetComponentSize().x;
				MkFloat2 currBtnSize(80.f, MKDEF_DEF_CTRL_HEIGHT);

				MkBaseWindowNode* setTag = MkWindowOpHelper::CreateWindowPreset
					(tagWin, SET_TAG_BTN_NAME, themeName, eS2D_WPC_OKButton, currBtnSize, L"태그 설정", MkFloat2(MKDEF_CTRL_MARGIN * 2.f + selectionBtnWidth, btnPosY));

				MkBaseWindowNode* delTag = MkWindowOpHelper::CreateWindowPreset
					(tagWin, DEL_TAG_BTN_NAME, themeName, eS2D_WPC_CancelButton, currBtnSize, L"태그 삭제", MkFloat2(MKDEF_CTRL_MARGIN * 3.f + selectionBtnWidth + currBtnSize.x, btnPosY));
				m_TabTag_EnableWindows.PushBack(delTag);

				btnPosY -= MKDEF_CTRL_MARGIN + MKDEF_DEF_CTRL_HEIGHT;

				MkBaseWindowNode* alignLeft = MkWindowOpHelper::CreateWindowPreset
					(tagWin, TAG_ALIGN_LEFT_BTN_NAME, themeName, eS2D_WPC_NormalButton, currBtnSize, L"좌측 정렬", MkFloat2(MKDEF_CTRL_MARGIN * 2.f + selectionBtnWidth, btnPosY));
				m_TabTag_EnableWindows.PushBack(alignLeft);

				MkBaseWindowNode* alignCenter = MkWindowOpHelper::CreateWindowPreset
					(tagWin, TAG_ALIGN_CENTER_BTN_NAME, themeName, eS2D_WPC_NormalButton, currBtnSize, L"중앙 정렬", MkFloat2(MKDEF_CTRL_MARGIN * 3.f + selectionBtnWidth + currBtnSize.x, btnPosY));
				m_TabTag_EnableWindows.PushBack(alignCenter);

				MkBaseWindowNode* alignRight = MkWindowOpHelper::CreateWindowPreset
					(tagWin, TAG_ALIGN_RIGHT_BTN_NAME, themeName, eS2D_WPC_NormalButton, currBtnSize, L"우측 정렬", MkFloat2(MKDEF_CTRL_MARGIN * 4.f + selectionBtnWidth + currBtnSize.x * 2.f, btnPosY));
				m_TabTag_EnableWindows.PushBack(alignRight);

				MkFloat2 pivot(MKDEF_CTRL_MARGIN * 2.f + selectionBtnWidth + 42.f, 140.f);

				MkBaseWindowNode* moveLL = __CreateWindowPreset(tagWin, TAG_PX_MOVE_LL_BTN_NAME, themeName, eS2D_WPC_DirLeftButton, MkFloat2(0.f, 0.f));
				const MkFloat2& dirBtnSize = moveLL->GetPresetComponentSize();
				moveLL->SetLocalPosition(MkVec3(pivot.x - dirBtnSize.x - MARGIN, pivot.y, -MKDEF_BASE_WINDOW_DEPTH_GRID));
				m_TabTag_EnableWindows.PushBack(moveLL);

				MkBaseWindowNode* moveLH = MkWindowOpHelper::CreateWindowPreset
					(tagWin, TAG_PX_MOVE_LH_BTN_NAME, themeName, eS2D_WPC_DirLeftButton, MkFloat2(0.f, 0.f), MkStr::Null, MkFloat2(pivot.x - dirBtnSize.x * 2.f - MARGIN * 2.f, pivot.y));
				m_TabTag_EnableWindows.PushBack(moveLH);

				MkBaseWindowNode* moveRL = MkWindowOpHelper::CreateWindowPreset
					(tagWin, TAG_PX_MOVE_RL_BTN_NAME, themeName, eS2D_WPC_DirRightButton, MkFloat2(0.f, 0.f), MkStr::Null, MkFloat2(pivot.x + dirBtnSize.x + MARGIN, pivot.y));
				m_TabTag_EnableWindows.PushBack(moveRL);

				MkBaseWindowNode* moveRH = MkWindowOpHelper::CreateWindowPreset
					(tagWin, TAG_PX_MOVE_RH_BTN_NAME, themeName, eS2D_WPC_DirRightButton, MkFloat2(0.f, 0.f), MkStr::Null, MkFloat2(pivot.x + dirBtnSize.x * 2.f + MARGIN * 2.f, pivot.y));
				m_TabTag_EnableWindows.PushBack(moveRH);

				MkBaseWindowNode* moveUL = MkWindowOpHelper::CreateWindowPreset
					(tagWin, TAG_PX_MOVE_UL_BTN_NAME, themeName, eS2D_WPC_DirUpButton, MkFloat2(0.f, 0.f), MkStr::Null, MkFloat2(pivot.x, pivot.y + dirBtnSize.y + MARGIN));
				m_TabTag_EnableWindows.PushBack(moveUL);

				MkBaseWindowNode* moveUH = MkWindowOpHelper::CreateWindowPreset
					(tagWin, TAG_PX_MOVE_UH_BTN_NAME, themeName, eS2D_WPC_DirUpButton, MkFloat2(0.f, 0.f), MkStr::Null, MkFloat2(pivot.x, pivot.y + dirBtnSize.y * 2.f + MARGIN * 2.f));
				m_TabTag_EnableWindows.PushBack(moveUH);

				MkBaseWindowNode* moveDL = MkWindowOpHelper::CreateWindowPreset
					(tagWin, TAG_PX_MOVE_DL_BTN_NAME, themeName, eS2D_WPC_DirDownButton, MkFloat2(0.f, 0.f), MkStr::Null, MkFloat2(pivot.x, pivot.y - dirBtnSize.y - MARGIN));
				m_TabTag_EnableWindows.PushBack(moveDL);

				MkBaseWindowNode* moveDH = MkWindowOpHelper::CreateWindowPreset
					(tagWin, TAG_PX_MOVE_DH_BTN_NAME, themeName, eS2D_WPC_DirDownButton, MkFloat2(0.f, 0.f), MkStr::Null, MkFloat2(pivot.x, pivot.y - dirBtnSize.y * 2.f - MARGIN * 2.f));
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
						
						if ((m_TargetNode->GetNodeType() == eS2D_SNT_SpreadButtonNode) &&
							(parentNode != NULL) && (parentNode->GetNodeType() == eS2D_SNT_SpreadButtonNode))
						{
							MkSpreadButtonNode* parentBtn = dynamic_cast<MkSpreadButtonNode*>(parentNode);
							if (parentBtn != NULL)
							{
								MkHashStr nodeName = m_TargetNode->GetNodeName();
								parentBtn->RemoveItem(nodeName);
							}
						}
						else
						{
							if (parentNode != NULL)
							{
								parentNode->DetachChildNode(m_TargetNode->GetNodeName());
							}
							delete m_TargetNode;
						}

						MK_WIN_EVENT_MGR.SetTargetWindowNode(NULL);

						MkBaseWindowNode* parentWindow = ((parentNode != NULL) && (parentNode->GetNodeType() >= eS2D_SNT_BaseWindowNode)) ? 
							dynamic_cast<MkBaseWindowNode*>(parentNode) : NULL;

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
			else if (evt.node->GetNodeName() == CREATE_WINDOW_BTN_NAME)
			{
				MkBaseWindowNode* newWin = NULL;

				const MkHashStr& wtKey = m_CreateWindowTypeSB->GetTargetItemKey();
				if (!wtKey.Empty())
				{
					if (wtKey == CT_KEY_FROM_FILE)
					{
						newWin = _LoadBaseWindowFromFile();
					}
					else
					{
						const MkHashStr& mKey = m_CreateMethodSB->GetTargetItemKey();
						const MkHashStr& dKey = m_CreateDetailSB->GetTargetItemKey();

						const MkHashStr& themeName = (m_TargetNode == NULL) ? MK_WR_PRESET.GetDefaultThemeName() : m_TargetNode->GetAncestorThemeName();

						eS2D_SceneNodeType nodeType = static_cast<eS2D_SceneNodeType>(wtKey.GetString().ToInteger());
						switch (nodeType)
						{
						case eS2D_SNT_SceneNode:
							{
								BasicPresetWindowDesc winDesc;
								bool hasTitle = true;
								if (mKey == CM_KEY_TITLE_OK)
								{
									hasTitle = true;
								}
								else if (mKey == CM_KEY_TITLE_NO)
								{
									hasTitle = false;
								}
								if (dKey == CD_KEY_THEME_BG)
								{
									winDesc.SetStandardDesc(themeName, hasTitle, MkFloat2(300.f, 200.f));
								}
								else if (dKey == CD_KEY_IMAGE_BG)
								{
									winDesc.SetStandardDesc(themeName, hasTitle, MK_WR_PRESET.GetSystemImageFilePath(), MK_WR_PRESET.GetWindowBackgroundSampleSubsetName());
								}
								newWin = MkBaseWindowNode::CreateBasicWindow(NULL, _GenerateNewWindowName(), winDesc);
							}
							break;

						case eS2D_SNT_BaseWindowNode:
							{
								eS2D_WindowPresetComponent component = MkWindowPreset::GetWindowPresetComponentEnum(dKey);
								if (component != eS2D_WPC_None)
								{
									MkFloat2 compSize;
									float parentWidth = (m_TargetNode == NULL) ? 300.f : m_TargetNode->GetWorldAABR().size.x;
									if (IsBackgroundStateType(component))
									{
										float parentHeight = (m_TargetNode == NULL) ? 200.f : m_TargetNode->GetWorldAABR().size.y;
										compSize = MkFloat2(parentWidth - MK_WR_PRESET.GetMargin() * 2.f, parentHeight - MK_WR_PRESET.GetMargin() * 2.f);
									}
									else if (IsTitleStateType(component))
									{
										compSize = MkFloat2(parentWidth, MKDEF_DEF_CTRL_HEIGHT);
									}
									else if (IsWindowStateType(component))
									{
										compSize = MkFloat2(60.f, MKDEF_DEF_CTRL_HEIGHT);
									}
									newWin = __CreateWindowPreset(NULL, _GenerateNewWindowName(), themeName, component, compSize);
								}
							}
							break;

						case eS2D_SNT_SpreadButtonNode:
							{
								MkSpreadButtonNode::eOpeningDirection opDir = MkSpreadButtonNode::eDownward;
								if (dKey == CD_KEY_SPREAD_TO_DOWN)
								{
									opDir = MkSpreadButtonNode::eDownward;
								}
								else if (dKey == CD_KEY_SPREAD_TO_RIGHT)
								{
									opDir = MkSpreadButtonNode::eRightside;
								}
								else if (dKey == CD_KEY_SPREAD_TO_UP)
								{
									opDir = MkSpreadButtonNode::eUpward;
								}

								MkSpreadButtonNode* newCtrl = new MkSpreadButtonNode(_GenerateNewWindowName());
								if (mKey == CM_KEY_SELECTION_SB)
								{
									newCtrl->CreateSelectionRootTypeButton(themeName, MkFloat2(100.f, MKDEF_DEF_CTRL_HEIGHT), opDir);
								}
								else if (mKey == CM_KEY_STATIC_SB)
								{
									ItemTagInfo ti;
									ti.iconPath = MK_WR_PRESET.GetSystemImageFilePath();
									ti.iconSubset = MK_WR_PRESET.GetWindowIconSampleSubsetName();
									ti.captionDesc.SetString(L"시작 버튼");
									newCtrl->CreateStaticRootTypeButton(themeName, MkFloat2(100.f, MKDEF_DEF_CTRL_HEIGHT), opDir, ti);
								}
								newWin = newCtrl;
							}
							break;

						case eS2D_SNT_CheckButtonNode:
							{
								MkCheckButtonNode* newCtrl = new MkCheckButtonNode(_GenerateNewWindowName());
								newCtrl->CreateCheckButton(themeName, CaptionDesc(L"체크 버튼"), false);
								newWin = newCtrl;
							}
							break;

						case eS2D_SNT_ScrollBarNode:
							{
								bool hasDirBtn = true;
								if (mKey == CM_KEY_BTN_OK_SB)
								{
									hasDirBtn = true;
								}
								else if (mKey == CM_KEY_BTN_NO_SB)
								{
									hasDirBtn = false;
								}

								MkScrollBarNode::eBarDirection barDir = MkScrollBarNode::eVertical;
								if (dKey == CD_KEY_VERTICAL_SB)
								{
									barDir = MkScrollBarNode::eVertical;
								}
								else if (dKey == CD_KEY_HORIZONTAL_SB)
								{
									barDir = MkScrollBarNode::eHorizontal;
								}

								MkScrollBarNode* newCtrl = new MkScrollBarNode(_GenerateNewWindowName());
								newCtrl->CreateScrollBar(themeName, barDir, hasDirBtn, 100.f);
								newWin = newCtrl;
							}
							break;

						case eS2D_SNT_EditBoxNode:
							{
								MkEditBoxNode* newCtrl = new MkEditBoxNode(_GenerateNewWindowName());
								newCtrl->CreateEditBox(themeName, MkFloat2(100.f, MKDEF_DEF_CTRL_HEIGHT), MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkStr::Null, false);
								newCtrl->SetText(L"에디트 박스", false);
								newWin = newCtrl;
							}
							break;

						case eS2D_SNT_TabWindowNode:
							{
								MkTabWindowNode::eTabAlignment align = MkTabWindowNode::eLeftside;
								if (mKey == CM_KEY_LEFT_AL_TW)
								{
									align = MkTabWindowNode::eLeftside;
								}
								else if (mKey == CM_KEY_RIGHT_AL_TW)
								{
									align = MkTabWindowNode::eRightside;
								}

								MkTabWindowNode* newCtrl = new MkTabWindowNode(_GenerateNewWindowName());
								newCtrl->CreateTabRoot(themeName, align, MkFloat2(40.f, MKDEF_DEF_CTRL_HEIGHT), MkFloat2(130.f, 80.f));

								ItemTagInfo ti;
								ti.captionDesc.SetString(L"탭");
								newCtrl->AddTab(L"tab", ti);
								newWin = newCtrl;
							}
							break;
						}
					}
				}

				if (newWin != NULL)
				{
					MkFloatRect anchorRect;
					if (m_TargetNode == NULL)
					{
						MK_WIN_EVENT_MGR.RegisterWindow(newWin, true);
						anchorRect = MK_WIN_EVENT_MGR.GetRegionRect();
					}
					else
					{
						m_TargetNode->AttachChildNode(newWin);
						anchorRect = m_TargetNode->GetWorldAABR();
					}

					newWin->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
					newWin->UpdateAll();

					if (wtKey != CT_KEY_FROM_FILE)
					{
						newWin->AlignPosition(anchorRect, eRAP_MiddleCenter, MkInt2(0, 0));
					}
					
					MK_WIN_EVENT_MGR.SetTargetWindowNode(NULL);
					MK_WIN_EVENT_MGR.SetTargetWindowNode(newWin);
				}
			}
			else if (evt.node->GetNodeName() == SPREAD_BTN_CREATE_NEW_BTN_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_SpreadButtonNode))
				{
					MkSpreadButtonNode* targetBtn = dynamic_cast<MkSpreadButtonNode*>(m_TargetNode);
					if (targetBtn != NULL)
					{
						const MkStr uniqueStr = m_TabWnd_SpreadBtn_UniqueKey->GetText();
						if (!uniqueStr.Empty())
						{
							MkHashStr uniqueKey = uniqueStr;
							ItemTagInfo ti;
							ti.captionDesc.SetString(uniqueStr);
							if (targetBtn->AddItem(uniqueKey, ti) != NULL)
							{
								MkBaseWindowNode* targetNode = m_TargetNode;
								MK_WIN_EVENT_MGR.SetTargetWindowNode(NULL);
								MK_WIN_EVENT_MGR.SetTargetWindowNode(targetNode);
							}
						}
					}
				}
			}
			else if (evt.node->GetNodeName() == TAB_WND_CREATE_NEW_BTN_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_TabWindowNode))
				{
					MkTabWindowNode* targetWnd = dynamic_cast<MkTabWindowNode*>(m_TargetNode);
					if (targetWnd != NULL)
					{
						const MkStr tabName = m_TabWnd_TabWnd_TabName->GetText();
						if (!tabName.Empty())
						{
							ItemTagInfo ti;
							ti.captionDesc.SetString(tabName);
							if (targetWnd->AddTab(tabName, ti, NULL) != NULL)
							{
								MkBaseWindowNode* targetNode = m_TargetNode;
								MK_WIN_EVENT_MGR.SetTargetWindowNode(NULL);
								MK_WIN_EVENT_MGR.SetTargetWindowNode(targetNode);
							}
						}
					}
				}
			}
			else if (evt.node->GetNodeName() == TAB_WND_DEL_TAB_BTN_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_TabWindowNode))
				{
					MkTabWindowNode* targetWnd = dynamic_cast<MkTabWindowNode*>(m_TargetNode);
					if (targetWnd != NULL)
					{
						const MkHashStr& tabName = m_TabWnd_TabWnd_TargetTab->GetTargetItemKey();
						if (targetWnd->RemoveTab(tabName))
						{
							MkBaseWindowNode* targetNode = m_TargetNode;
							MK_WIN_EVENT_MGR.SetTargetWindowNode(NULL);
							MK_WIN_EVENT_MGR.SetTargetWindowNode(targetNode);
						}
					}
				}
			}
			else if (evt.node->GetNodeName() == TAB_WND_MOVE_TAB_TO_PREV_BTN_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_TabWindowNode))
				{
					MkTabWindowNode* targetWnd = dynamic_cast<MkTabWindowNode*>(m_TargetNode);
					if (targetWnd != NULL)
					{
						const MkHashStr& tabName = m_TabWnd_TabWnd_TargetTab->GetTargetItemKey();
						targetWnd->MoveTabToOneStepForward(tabName);
					}
				}
			}
			else if (evt.node->GetNodeName() == TAB_WND_MOVE_TAB_TO_NEXT_BTN_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_TabWindowNode))
				{
					MkTabWindowNode* targetWnd = dynamic_cast<MkTabWindowNode*>(m_TargetNode);
					if (targetWnd != NULL)
					{
						const MkHashStr& tabName = m_TabWnd_TabWnd_TargetTab->GetTargetItemKey();
						targetWnd->MoveTabToOneStepBackword(tabName);
					}
				}
			}
			else if (evt.node->GetNodeName() == FAR_DEPTH_BTN_NAME)
			{
				if ((m_TargetNode != NULL) && (!m_TargetNode->CheckRootWindow()))
				{
					m_TargetNode->SetLocalDepth(m_TargetNode->GetLocalDepth() + MKDEF_BASE_WINDOW_DEPTH_GRID);
					_UpdateTabComponentDepth();
				}
			}
			else if (evt.node->GetNodeName() == NEAR_DEPTH_BTN_NAME)
			{
				if ((m_TargetNode != NULL) && (!m_TargetNode->CheckRootWindow()))
				{
					m_TargetNode->SetLocalDepth(m_TargetNode->GetLocalDepth() - MKDEF_BASE_WINDOW_DEPTH_GRID);
					_UpdateTabComponentDepth();
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
			else if (evt.node->GetNodeName() == ADD_THEME_BTN_NAME)
			{
				if (m_TargetNode != NULL)
				{
					m_TargetNode->__ApplyAncestorTheme();
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
			else if (evt.node->GetNodeName() == EB_HISTORY_USAGE_CB_BTN_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_EditBoxNode))
				{
					MkEditBoxNode* targetNode = dynamic_cast<MkEditBoxNode*>(m_TargetNode);
					if (targetNode != NULL)
					{
						targetNode->SetHistoryUsage(true);
					}
				}
			}
			else if (evt.node->GetNodeName() == TAB_WND_USAGE_CB_BTN_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_TabWindowNode))
				{
					MkTabWindowNode* targetNode = dynamic_cast<MkTabWindowNode*>(m_TargetNode);
					if (targetNode != NULL)
					{
						const MkHashStr& tabName = m_TabWnd_TabWnd_TargetTab->GetTargetItemKey();
						targetNode->SetTabEnable(tabName, true);
					}
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
			else if (evt.node->GetNodeName() == EB_HISTORY_USAGE_CB_BTN_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_EditBoxNode))
				{
					MkEditBoxNode* targetNode = dynamic_cast<MkEditBoxNode*>(m_TargetNode);
					if (targetNode != NULL)
					{
						targetNode->SetHistoryUsage(false);
					}
				}
			}
			else if (evt.node->GetNodeName() == TAB_WND_USAGE_CB_BTN_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_TabWindowNode))
				{
					MkTabWindowNode* targetNode = dynamic_cast<MkTabWindowNode*>(m_TargetNode);
					if (targetNode != NULL)
					{
						const MkHashStr& tabName = m_TabWnd_TabWnd_TargetTab->GetTargetItemKey();
						targetNode->SetTabEnable(tabName, false);
					}
				}
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
			if (evt.node->GetNodeName() == CREATE_WND_TYPE_SB_NAME)
			{
				_SetCreationMethodSB();
				_SetCreationDetailSB();
			}
			else if (evt.node->GetNodeName() == CREATE_METHOD_SB_NAME)
			{
				_SetCreationDetailSB();
			}
			else if (evt.node->GetNodeName() == TAB_WND_TARGET_TAB_SB_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_TabWindowNode))
				{
					MkTabWindowNode* targetNode = dynamic_cast<MkTabWindowNode*>(m_TargetNode);
					if (targetNode != NULL)
					{
						const MkHashStr& tabName = m_TabWnd_TabWnd_TargetTab->GetTargetItemKey();
						m_TabWnd_TabWnd_Usage->SetCheck(targetNode->GetTabEnable(tabName));
					}
				}
			}
			else if (evt.node->GetNodeName() == THEME_SPREAD_BTN_NAME)
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
			if (evt.node->GetNodeName() == EB_FONT_TYPE_EB_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_EditBoxNode))
				{
					MkEditBoxNode* targetNode = dynamic_cast<MkEditBoxNode*>(m_TargetNode);
					if (targetNode != NULL)
					{
						targetNode->SetFontType(m_TabWnd_EditBox_FontType->GetText());
						m_TabWnd_EditBox_FontType->SetText(targetNode->GetFontType().GetString(), false);
					}
				}
			}
			else if (evt.node->GetNodeName() == EB_NORMAL_FONT_STATE_EB_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_EditBoxNode))
				{
					MkEditBoxNode* targetNode = dynamic_cast<MkEditBoxNode*>(m_TargetNode);
					if (targetNode != NULL)
					{
						targetNode->SetNormalFontState(m_TabWnd_EditBox_NormalFontState->GetText());
						m_TabWnd_EditBox_NormalFontState->SetText(targetNode->GetNormalFontState().GetString(), false);
					}
				}
			}
			else if (evt.node->GetNodeName() == EB_SELECTION_FONT_STATE_EB_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_EditBoxNode))
				{
					MkEditBoxNode* targetNode = dynamic_cast<MkEditBoxNode*>(m_TargetNode);
					if (targetNode != NULL)
					{
						targetNode->SetSelectionFontState(m_TabWnd_EditBox_SelectionFontState->GetText());
						m_TabWnd_EditBox_SelectionFontState->SetText(targetNode->GetSelectionFontState().GetString(), false);
					}
				}
			}
			else if (evt.node->GetNodeName() == EB_CURSOR_FONT_STATE_EB_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_EditBoxNode))
				{
					MkEditBoxNode* targetNode = dynamic_cast<MkEditBoxNode*>(m_TargetNode);
					if (targetNode != NULL)
					{
						targetNode->SetCursorFontState(m_TabWnd_EditBox_CursorFontState->GetText());
						m_TabWnd_EditBox_CursorFontState->SetText(targetNode->GetCursorFontState().GetString(), false);
					}
				}
			}
			else if (evt.node->GetNodeName() == SCROLL_BAR_LENGTH_EB_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_ScrollBarNode))
				{
					MkScrollBarNode* targetNode = dynamic_cast<MkScrollBarNode*>(m_TargetNode);
					if (targetNode != NULL)
					{
						float length = m_TabWnd_ScrollBar_Length->GetText().ToFloat();
						targetNode->SetScrollBarLength(length);
					}
				}
			}
			else if (evt.node->GetNodeName() == TAB_WND_TAB_SIZE_X_EB_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_TabWindowNode))
				{
					MkTabWindowNode* targetNode = dynamic_cast<MkTabWindowNode*>(m_TargetNode);
					if (targetNode != NULL)
					{
						MkFloat2 size(m_TabWnd_TabWnd_BtnSizeX->GetText().ToFloat(), targetNode->GetTabButtonSize().y);
						targetNode->SetTabButtonSize(size);
					}
				}
			}
			else if (evt.node->GetNodeName() == TAB_WND_TAB_SIZE_Y_EB_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_TabWindowNode))
				{
					MkTabWindowNode* targetNode = dynamic_cast<MkTabWindowNode*>(m_TargetNode);
					if (targetNode != NULL)
					{
						MkFloat2 size(targetNode->GetTabButtonSize().x, m_TabWnd_TabWnd_BtnSizeY->GetText().ToFloat());
						targetNode->SetTabButtonSize(size);
					}
				}
			}
			else if (evt.node->GetNodeName() == TAB_WND_REG_SIZE_X_EB_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_TabWindowNode))
				{
					MkTabWindowNode* targetNode = dynamic_cast<MkTabWindowNode*>(m_TargetNode);
					if (targetNode != NULL)
					{
						MkFloat2 size(m_TabWnd_TabWnd_BodySizeX->GetText().ToFloat(), targetNode->GetTabBodySize().y);
						targetNode->SetTabBodySize(size);
					}
				}
			}
			else if (evt.node->GetNodeName() == TAB_WND_REG_SIZE_Y_EB_NAME)
			{
				if ((m_TargetNode != NULL) && (m_TargetNode->GetNodeType() == eS2D_SNT_TabWindowNode))
				{
					MkTabWindowNode* targetNode = dynamic_cast<MkTabWindowNode*>(m_TargetNode);
					if (targetNode != NULL)
					{
						MkFloat2 size(targetNode->GetTabBodySize().x, m_TabWnd_TabWnd_BodySizeY->GetText().ToFloat());
						targetNode->SetTabBodySize(size);
					}
				}
			}
			else if (evt.node->GetNodeName() == PRESET_SIZE_X_EB_NAME)
			{
				if ((m_TargetNode != NULL) && m_TabComp_SizeX->GetText().IsDigit())
				{
					float newWidth = GetMax<float>(m_TabComp_SizeX->GetText().ToFloat(), MK_WR_PRESET.GetMargin() * 2.f);
					MkFloat2 size = m_TargetNode->GetPresetComponentSize();
					m_TargetNode->SetPresetComponentSize(MkFloat2(newWidth, size.y));
					newWidth = m_TargetNode->GetPresetComponentSize().x;

					// 타겟이 bg type이고 부모로 title type이 존재 할 경우 부모의 크기도 수정
					if (IsBackgroundStateType(m_TargetNode->GetPresetComponentType()) &&
						(m_TargetNode->GetParentNode() != NULL) && (m_TargetNode->GetNodeType() >= eS2D_SNT_BaseWindowNode))
					{
						MkBaseWindowNode* parentWindow = dynamic_cast<MkBaseWindowNode*>(m_TargetNode->GetParentNode());
						if ((parentWindow != NULL) && IsTitleStateType(parentWindow->GetPresetComponentType()))
						{
							MkFloat2 titleSize = parentWindow->GetPresetComponentSize();
							titleSize.x += newWidth - size.x;
							parentWindow->SetPresetComponentSize(titleSize);
						}
					}
				}
			}
			else if (evt.node->GetNodeName() == PRESET_SIZE_Y_EB_NAME)
			{
				if ((m_TargetNode != NULL) && m_TabComp_SizeY->GetText().IsDigit())
				{
					float newHeight = GetMax<float>(m_TabComp_SizeY->GetText().ToFloat(), MK_WR_PRESET.GetMargin() * 2.f);
					MkFloat2 size = m_TargetNode->GetPresetComponentSize();
					m_TargetNode->SetPresetComponentSize(MkFloat2(size.x, newHeight));
					newHeight = m_TargetNode->GetPresetComponentSize().y;

					// 타겟이 bg type이고 부모로 title type이 존재 할 경우 위치를 맞추기 위해 local position 수정
					if (IsBackgroundStateType(m_TargetNode->GetPresetComponentType()) &&
						(m_TargetNode->GetParentNode() != NULL) && (m_TargetNode->GetNodeType() >= eS2D_SNT_BaseWindowNode))
					{
						MkBaseWindowNode* parentWindow = dynamic_cast<MkBaseWindowNode*>(m_TargetNode->GetParentNode());
						if ((parentWindow != NULL) && IsTitleStateType(parentWindow->GetPresetComponentType()))
						{
							MkVec3 newPos = m_TargetNode->GetLocalPosition();
							newPos.y -= newHeight - size.y;
							m_TargetNode->SetLocalPosition(newPos);
						}
					}
				}
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
	m_CreateWindowTypeSB = NULL;
	m_CreateMethodSB = NULL;
	m_CreateDetailSB = NULL;
	m_TabWindow = NULL;
	m_TabWnd_Desc = NULL;
	m_TabWnd_EditBox_HistoryUsage = NULL;
	m_TabWnd_EditBox_FontType = NULL;
	m_TabWnd_EditBox_NormalFontState = NULL;
	m_TabWnd_EditBox_SelectionFontState = NULL;
	m_TabWnd_EditBox_CursorFontState = NULL;
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
	m_TabComp_Depth = NULL;
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

	if (m_CreateWindowTypeSB != NULL)
	{
		m_CreateWindowTypeSB->GetItem(MkStr(eS2D_SNT_SceneNode))->SetEnable(!nodeEnable);
		m_CreateWindowTypeSB->SetTargetItem(CT_KEY_FROM_FILE);

		_SetCreationMethodSB();
		_SetCreationDetailSB();
	}

	if (m_TabWindow != NULL)
	{
		bool tabWndEnable = nodeEnable &&
			(m_TargetNode->GetNodeType() >= eS2D_SNT_ControlWindowNodeBegin) &&
			(m_TargetNode->GetNodeType() != eS2D_SNT_CheckButtonNode);

		m_TabWindow->SetTabEnable(TAB_WINDOW_NAME, tabWndEnable);
		m_TabWindow->SetTabEnable(TAB_COMPONENT_NAME, nodeEnable);
		m_TabWindow->SetTabEnable(TAB_TAGS_NAME, nodeEnable);

		if (tabWndEnable)
		{
			m_TabWindow->SelectTab(TAB_WINDOW_NAME);
		}
		else if (nodeEnable)
		{
			m_TabWindow->SelectTab((m_TargetNode->GetPresetComponentType() == eS2D_WPC_None) ? TAB_TAGS_NAME : TAB_COMPONENT_NAME);
		}

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
									m_TabWnd_EditBox_FontType->SetText(targetNode->GetFontType().GetString(), false);
									m_TabWnd_EditBox_NormalFontState->SetText(targetNode->GetNormalFontState().GetString(), false);
									m_TabWnd_EditBox_SelectionFontState->SetText(targetNode->GetSelectionFontState().GetString(), false);
									m_TabWnd_EditBox_CursorFontState->SetText(targetNode->GetCursorFontState().GetString(), false);
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

									m_TabWnd_TabWnd_TargetTab->RemoveAllItems();

									const MkArray<MkHashStr>& tabList = targetNode->GetTabList();
									ItemTagInfo ti;
									MK_INDEXING_LOOP(tabList, j)
									{
										const MkHashStr& tabName = tabList[j];
										ti.captionDesc.SetString(tabName.GetString());
										m_TabWnd_TabWnd_TargetTab->AddItem(tabName, ti);

										if (j == 0)
										{
											m_TabWnd_TabWnd_TargetTab->SetTargetItem(tabName);
											m_TabWnd_TabWnd_Usage->SetCheck(targetNode->GetTabEnable(tabName));
										}
									}
								}
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
			_UpdateTabComponentDepth();
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

void MkEditModeTargetWindow::_UpdateTabComponentDepth(void)
{
	if ((m_TargetNode != NULL) && (m_TabComp_Depth != NULL))
	{
		MkStr msg;
		msg.Reserve(128);
		msg += L"깊이(Z) : ";
		if (m_TargetNode->CheckRootWindow())
		{
			msg += L"기준(0)";
		}
		else
		{
			float depth = m_TargetNode->GetLocalDepth();
			if (depth < 0.f)
			{
				msg += L"앞(";
				msg += depth;
				msg += L")";
			}
			else if (depth > 0.f)
			{
				msg += L"뒤(";
				msg += depth;
				msg += L")";
			}
			else // depth == 0.f
			{
				msg += L"평행(0)";
			}
		}
		m_TabComp_Depth->SetDecoString(msg);
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

void MkEditModeTargetWindow::_SetCreationMethodSB(void)
{
	const MkHashStr& wtKey = m_CreateWindowTypeSB->GetTargetItemKey();
	if (!wtKey.Empty())
	{
		ItemTagInfo ti;
		m_CreateMethodSB->RemoveAllItems();

		if (wtKey == CT_KEY_FROM_FILE)
		{
			_SetSpreadBtnItemTo(m_CreateMethodSB, L"--", CD_KEY_NONE, true);
		}
		else
		{
			eS2D_SceneNodeType nodeType = static_cast<eS2D_SceneNodeType>(wtKey.GetString().ToInteger());
			switch (nodeType)
			{
			case eS2D_SNT_SceneNode:
				_SetSpreadBtnItemTo(m_CreateMethodSB, L"타이틀 O", CM_KEY_TITLE_OK, true);
				_SetSpreadBtnItemTo(m_CreateMethodSB, L"타이틀 X", CM_KEY_TITLE_NO, false);
				break;

			case eS2D_SNT_BaseWindowNode:
				_SetSpreadBtnItemTo(m_CreateMethodSB, L"배경 속성", CM_KEY_BG_TYPE, true);
				_SetSpreadBtnItemTo(m_CreateMethodSB, L"타이틀 속성", CM_KEY_TITLE_TYPE, false);
				_SetSpreadBtnItemTo(m_CreateMethodSB, L"버튼 속성", CM_KEY_BTN_TYPE, false);
				break;

			case eS2D_SNT_SpreadButtonNode:
				_SetSpreadBtnItemTo(m_CreateMethodSB, L"선택형", CM_KEY_SELECTION_SB, true);
				_SetSpreadBtnItemTo(m_CreateMethodSB, L"고정형", CM_KEY_STATIC_SB, false);
				break;

			case eS2D_SNT_CheckButtonNode:
				_SetSpreadBtnItemTo(m_CreateMethodSB, L"--", CD_KEY_NONE, true);
				break;

			case eS2D_SNT_ScrollBarNode:
				_SetSpreadBtnItemTo(m_CreateMethodSB, L"이동 버튼 O", CM_KEY_BTN_OK_SB, true);
				_SetSpreadBtnItemTo(m_CreateMethodSB, L"이동 버튼 X", CM_KEY_BTN_NO_SB, false);
				break;

			case eS2D_SNT_EditBoxNode:
				_SetSpreadBtnItemTo(m_CreateMethodSB, L"--", CD_KEY_NONE, true);
				break;

			case eS2D_SNT_TabWindowNode:
				_SetSpreadBtnItemTo(m_CreateMethodSB, L"좌측 정렬형", CM_KEY_LEFT_AL_TW, true);
				_SetSpreadBtnItemTo(m_CreateMethodSB, L"우측 정렬형", CM_KEY_RIGHT_AL_TW, false);
				break;
			}
		}
	}
}

void MkEditModeTargetWindow::_SetCreationDetailSB(void)
{
	const MkHashStr& wtKey = m_CreateWindowTypeSB->GetTargetItemKey();
	if (!wtKey.Empty())
	{
		ItemTagInfo ti;
		m_CreateDetailSB->RemoveAllItems();

		if (wtKey == CT_KEY_FROM_FILE)
		{
			_SetSpreadBtnItemTo(m_CreateDetailSB, L"--", CD_KEY_NONE, true);
		}
		else
		{
			eS2D_SceneNodeType nodeType = static_cast<eS2D_SceneNodeType>(wtKey.GetString().ToInteger());
			switch (nodeType)
			{
			case eS2D_SNT_SceneNode:
				_SetSpreadBtnItemTo(m_CreateDetailSB, L"테마 배경", CD_KEY_THEME_BG, true);
				_SetSpreadBtnItemTo(m_CreateDetailSB, L"이미지 배경", CD_KEY_IMAGE_BG, false);
				break;

			case eS2D_SNT_BaseWindowNode:
				{
					const MkHashStr& mKey = m_CreateMethodSB->GetTargetItemKey();
					eS2D_WindowPresetComponent beginComp = eS2D_WPC_None, endComp = eS2D_WPC_None;
					if (mKey == CM_KEY_BG_TYPE)
					{
						beginComp = eS2D_WPC_BackgroundStateTypeBegin;
						endComp = eS2D_WPC_BackgroundStateTypeEnd;
					}
					else if (mKey == CM_KEY_TITLE_TYPE)
					{
						beginComp = eS2D_WPC_TitleStateTypeBegin;
						endComp = eS2D_WPC_TitleStateTypeEnd;
					}
					else if (mKey == CM_KEY_BTN_TYPE)
					{
						beginComp = eS2D_WPC_WindowStateTypeBegin;
						endComp = eS2D_WPC_WindowStateTypeEnd;
					}

					if (beginComp != eS2D_WPC_None)
					{
						for (int i=beginComp; i<endComp; ++i)
						{
							const MkHashStr compName = MkWindowPreset::GetWindowPresetComponentKeyword(static_cast<eS2D_WindowPresetComponent>(i));
							_SetSpreadBtnItemTo(m_CreateDetailSB, compName.GetString(), compName, i == beginComp);
						}
					}
				}
				break;

			case eS2D_SNT_SpreadButtonNode:
				_SetSpreadBtnItemTo(m_CreateDetailSB, L"전개방향 ▽", CD_KEY_SPREAD_TO_DOWN, true);
				_SetSpreadBtnItemTo(m_CreateDetailSB, L"전개방향 ▷", CD_KEY_SPREAD_TO_RIGHT, false);
				_SetSpreadBtnItemTo(m_CreateDetailSB, L"전개방향 △", CD_KEY_SPREAD_TO_UP, false);
				break;

			case eS2D_SNT_ScrollBarNode:
				_SetSpreadBtnItemTo(m_CreateDetailSB, L"수직형 ↕", CD_KEY_VERTICAL_SB, true);
				_SetSpreadBtnItemTo(m_CreateDetailSB, L"수평형 ↔", CD_KEY_HORIZONTAL_SB, false);
				break;

			case eS2D_SNT_CheckButtonNode:
			case eS2D_SNT_EditBoxNode:
			case eS2D_SNT_TabWindowNode:
				_SetSpreadBtnItemTo(m_CreateDetailSB, L"--", CD_KEY_NONE, true);
				break;
			}
		}
	}
}

void MkEditModeTargetWindow::_SetSpreadBtnItemTo(MkSpreadButtonNode* btn, const MkStr& tag, const MkHashStr& key, bool target)
{
	if (btn != NULL)
	{
		ItemTagInfo ti;
		ti.captionDesc.SetString(tag);
		btn->AddItem(key, ti);

		if (target)
		{
			btn->SetTargetItem(key);
		}
	}
}

MkHashStr MkEditModeTargetWindow::_GenerateNewWindowName(void)
{
	static int autoIncIndex = 0;
	MkStr tag = L"#NewWindow_";
	tag += autoIncIndex;
	++autoIncIndex;
	return MkHashStr(tag);
}

MkBaseWindowNode* MkEditModeTargetWindow::_LoadBaseWindowFromFile(void)
{
	MkPathName filePath;
	MkArray<MkStr> exts(2);
	exts.PushBack(MKDEF_S2D_SCENE_FILE_EXT_BINARY);
	exts.PushBack(MKDEF_S2D_SCENE_FILE_EXT_TEXT);
	if (filePath.GetSingleFilePathFromDialog(exts) && (!filePath.Empty()) && filePath.ConvertToRootBasisRelativePath())
	{
		MkDataNode dataNode;
		if (dataNode.Load(filePath))
		{
			// node type에 따라 생성이 다름
			MkHashStr templateName;
			if (dataNode.GetAppliedTemplateName(templateName))
			{
				MkSceneNode* newNode = MkSceneNodeFamilyDefinition::Alloc(templateName, _GenerateNewWindowName());
				if (newNode->GetNodeType() >= eS2D_SNT_BaseWindowNode)
				{
					MkBaseWindowNode* newWin = dynamic_cast<MkBaseWindowNode*>(newNode);
					if (newWin != NULL)
					{
						newWin->Load(dataNode);
						return newWin;
					}
				}
			}
		}
	}
	return NULL;
}

//------------------------------------------------------------------------------------------------//
