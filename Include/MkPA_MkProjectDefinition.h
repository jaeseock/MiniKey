#pragma once

//------------------------------------------------------------------------------------------------//
// library version
//------------------------------------------------------------------------------------------------//

#define MKDEF_PA_MAJOR_VERSION 1
#define MKDEF_PA_MINOR_VERSION 0
#define MKDEF_PA_BUILD_VERSION 0
#define MKDEF_PA_REVISION_VERSION 0

//------------------------------------------------------------------------------------------------//

// render config 파일명
#define MKDEF_PA_CONFIG_FILE_NAME L"MkRenderConfig.txt"

// world에서 panel이 존재할 수 있는 가장 뒤의 거리(가장 앞은 0.f)
#define MKDEF_PA_MAX_WORLD_DEPTH 10000.f

// MkImageInfo file 확장자
#define MKDEF_PA_IMAGE_INFO_FILE_EXT L"mii"

// MkTextNode에서 투명 처리 될 color key(D3DCOLOR)
#define MKDEF_PA_TEXTNODE_COLOR_KEY 0xff010101

// window visual pattern node에서 system이 처리 할 node, panel name prefix
#define MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX L"__#WT:"

// window system에서 control에 해당 하는 node, panel name prefix
#define MKDEF_PA_WIN_CONTROL_PREFIX L"__#WC:"

// window system에서 각 component간 기본 x, y 거리
#define MKDEF_PA_DEFAULT_LENGTH_BETWEEN_WIN_COMPONENT_X 6.f
#define MKDEF_PA_DEFAULT_LENGTH_BETWEEN_WIN_COMPONENT_Y 8.f

// window system에서 body frame의 기본 margin(x, y)
#define MKDEF_PA_DEFAULT_WIN_FRAME_MARGIN 4.f

//------------------------------------------------------------------------------------------------//



// 윈도우 깊이 grid 단위
//#define MKDEF_BASE_WINDOW_DEPTH_GRID 0.025f

// list button(sub spread button) 깊이 grid 단위
//#define MKDEF_LIST_BUTTON_DEPTH_GRID 0.002f

// 윈도우 관리자 draw step priority
//#define MKDEF_WINDOW_MGR_DRAW_STEP_PRIORITY 0x7fffffff

// 윈도우 관리자 영역 표시 레이어 지속시간
//#define MKDEF_WINDOW_MGR_REGION_LAYER_LIFETIME 0.5f

//------------------------------------------------------------------------------------------------//
// building template node
//------------------------------------------------------------------------------------------------//

// MkSRect 템플릿 이름
//#define MKDEF_PA_BT_SRECT_TEMPLATE_NAME L"T_SRect"

// MkSceneNode 템플릿 이름
//#define MKDEF_PA_BT_SCENENODE_TEMPLATE_NAME L"T_SceneNode"

// MkBaseWindowNode 템플릿 이름
//#define MKDEF_PA_BT_BASEWINNODE_TEMPLATE_NAME L"T_BaseWindowNode"

// MkSpreadButtonNode 템플릿 이름
//#define MKDEF_PA_BT_SPREADBUTTON_TEMPLATE_NAME L"T_SpreadButtonNode"

// MkCheckButtonNode 템플릿 이름
//#define MKDEF_PA_BT_CHECKBUTTON_TEMPLATE_NAME L"T_CheckButtonNode"

// MkScrollBarNode 템플릿 이름
//#define MKDEF_PA_BT_SCROLLBAR_TEMPLATE_NAME L"T_ScrollBarNode"

// MkEditBoxNode 템플릿 이름
//#define MKDEF_PA_BT_EDITBOX_TEMPLATE_NAME L"T_EditBoxNode"

// MkTabWindowNode 템플릿 이름
//#define MKDEF_PA_BT_TABWINDOW_TEMPLATE_NAME L"T_TabWindowNode"

// MkSceneNode data에서 자식 SRect 노드명
//#define MKDEF_PA_SND_CHILD_SRECT_NODE_NAME L"<SRect>"

// MkSceneNode data에서 자식 scene node 노드명
//#define MKDEF_PA_SND_CHILD_SNODE_NODE_NAME L"<SceneNode>"

//------------------------------------------------------------------------------------------------//
// MkBaseWindowNode
//------------------------------------------------------------------------------------------------//

//#define MKDEF_PA_BASE_WND_ICON_TAG_NAME L"__#IconTag"
//#define MKDEF_PA_BASE_WND_HIGHLIGHT_CAP_TAG_NAME L"__#HCapTag"
//#define MKDEF_PA_BASE_WND_NORMAL_CAP_TAG_NAME L"__#NCapTag"

//#define MKDEF_PA_BASE_WND_CLOSE_BTN_NAME L"__#CloseBtn"

//------------------------------------------------------------------------------------------------//
// window edit mode
//------------------------------------------------------------------------------------------------//

// normalmode <-> edit mode 전환 key. VK_CONTROL + VK_SHIFT + 
//#define MKDEF_PA_TOGGLE_KEY_BETWEEN_NORMAL_AND_EDIT_MODE L'E'

// 설정창 토글 버튼
//#define MKDEF_PA_SETTING_WINDOW_TOGGLE_KEY VK_ESCAPE

// 선택창 토글 버튼
//#define MKDEF_PA_NODE_SEL_WINDOW_TOGGLE_KEY VK_TAB

// 선택창에서 한 라인 height
//#define MKDEF_PA_NODE_SEL_LINE_SIZE 16.f

// 선택창에서 node depth당 들여쓰기 크기
//#define MKDEF_PA_NODE_SEL_DEPTH_OFFSET 20.f

// image 파일 확장자
//#define MKDEF_PA_IMAGE_FILE_EXT_00 L"png"
//#define MKDEF_PA_IMAGE_FILE_EXT_01 L"dds"

// scene 파일 확장자
//#define MKDEF_PA_SCENE_FILE_EXT_BINARY L"msd" // minikey scene data
//#define MKDEF_PA_SCENE_FILE_EXT_TEXT L"txt"

//------------------------------------------------------------------------------------------------//
// DEBUG
//------------------------------------------------------------------------------------------------//

// MkWindowManagerNode에서 보고받은 event 출력 여부
#define MKDEF_PA_SHOW_WIN_MGR_EVENT_REPORT TRUE
