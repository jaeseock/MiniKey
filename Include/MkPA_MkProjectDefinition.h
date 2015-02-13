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

// render config 파일에서 scene deco text table 파일 경로명 key
//#define MKDEF_PA_SCENE_DECO_TEXT_TABLE_FILE_PATH L"SceneDecoTextTable"

// render config 파일에서 window type image set 노드명
//#define MKDEF_PA_WINDOW_TYPE_IMAGE_SET_NODE_NAME L"WindowTypeImageSet"

// render config 파일에서 window preset 노드명
//#define MKDEF_PA_WINDOW_PRESET_NODE_NAME L"WindowPreset"

//------------------------------------------------------------------------------------------------//

// MkTextNode에서 투명 처리 될 color key(D3DCOLOR)
#define MKDEF_PA_TEXTNODE_COLOR_KEY 0xff010101

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
