#pragma once

//------------------------------------------------------------------------------------------------//
// library version
//------------------------------------------------------------------------------------------------//

#define MKDEF_PA_MAJOR_VERSION 1
#define MKDEF_PA_MINOR_VERSION 0
#define MKDEF_PA_BUILD_VERSION 0
#define MKDEF_PA_REVISION_VERSION 0

//------------------------------------------------------------------------------------------------//

// render config ���ϸ�
#define MKDEF_PA_CONFIG_FILE_NAME L"MkRenderConfig.txt"

// world���� panel�� ������ �� �ִ� ���� ���� �Ÿ�(���� ���� 0.f)
#define MKDEF_PA_MAX_WORLD_DEPTH 10000.f

// MkImageInfo file Ȯ����
#define MKDEF_PA_IMAGE_INFO_FILE_EXT L"mii"

// MkTextNode���� ���� ó�� �� color key(D3DCOLOR)
#define MKDEF_PA_TEXTNODE_COLOR_KEY 0xff010101

// window visual pattern node���� system�� ó�� �� node, panel name prefix
#define MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX L"__#WT:"

// window system���� control�� �ش� �ϴ� node, panel name prefix
#define MKDEF_PA_WIN_CONTROL_PREFIX L"__#WC:"

// window system���� �� component�� �⺻ x, y �Ÿ�
#define MKDEF_PA_DEFAULT_LENGTH_BETWEEN_WIN_COMPONENT_X 6.f
#define MKDEF_PA_DEFAULT_LENGTH_BETWEEN_WIN_COMPONENT_Y 8.f

// window system���� body frame�� �⺻ margin(x, y)
#define MKDEF_PA_DEFAULT_WIN_FRAME_MARGIN 4.f

//------------------------------------------------------------------------------------------------//



// ������ ���� grid ����
//#define MKDEF_BASE_WINDOW_DEPTH_GRID 0.025f

// list button(sub spread button) ���� grid ����
//#define MKDEF_LIST_BUTTON_DEPTH_GRID 0.002f

// ������ ������ draw step priority
//#define MKDEF_WINDOW_MGR_DRAW_STEP_PRIORITY 0x7fffffff

// ������ ������ ���� ǥ�� ���̾� ���ӽð�
//#define MKDEF_WINDOW_MGR_REGION_LAYER_LIFETIME 0.5f

//------------------------------------------------------------------------------------------------//
// building template node
//------------------------------------------------------------------------------------------------//

// MkSRect ���ø� �̸�
//#define MKDEF_PA_BT_SRECT_TEMPLATE_NAME L"T_SRect"

// MkSceneNode ���ø� �̸�
//#define MKDEF_PA_BT_SCENENODE_TEMPLATE_NAME L"T_SceneNode"

// MkBaseWindowNode ���ø� �̸�
//#define MKDEF_PA_BT_BASEWINNODE_TEMPLATE_NAME L"T_BaseWindowNode"

// MkSpreadButtonNode ���ø� �̸�
//#define MKDEF_PA_BT_SPREADBUTTON_TEMPLATE_NAME L"T_SpreadButtonNode"

// MkCheckButtonNode ���ø� �̸�
//#define MKDEF_PA_BT_CHECKBUTTON_TEMPLATE_NAME L"T_CheckButtonNode"

// MkScrollBarNode ���ø� �̸�
//#define MKDEF_PA_BT_SCROLLBAR_TEMPLATE_NAME L"T_ScrollBarNode"

// MkEditBoxNode ���ø� �̸�
//#define MKDEF_PA_BT_EDITBOX_TEMPLATE_NAME L"T_EditBoxNode"

// MkTabWindowNode ���ø� �̸�
//#define MKDEF_PA_BT_TABWINDOW_TEMPLATE_NAME L"T_TabWindowNode"

// MkSceneNode data���� �ڽ� SRect ����
//#define MKDEF_PA_SND_CHILD_SRECT_NODE_NAME L"<SRect>"

// MkSceneNode data���� �ڽ� scene node ����
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

// normalmode <-> edit mode ��ȯ key. VK_CONTROL + VK_SHIFT + 
//#define MKDEF_PA_TOGGLE_KEY_BETWEEN_NORMAL_AND_EDIT_MODE L'E'

// ����â ��� ��ư
//#define MKDEF_PA_SETTING_WINDOW_TOGGLE_KEY VK_ESCAPE

// ����â ��� ��ư
//#define MKDEF_PA_NODE_SEL_WINDOW_TOGGLE_KEY VK_TAB

// ����â���� �� ���� height
//#define MKDEF_PA_NODE_SEL_LINE_SIZE 16.f

// ����â���� node depth�� �鿩���� ũ��
//#define MKDEF_PA_NODE_SEL_DEPTH_OFFSET 20.f

// image ���� Ȯ����
//#define MKDEF_PA_IMAGE_FILE_EXT_00 L"png"
//#define MKDEF_PA_IMAGE_FILE_EXT_01 L"dds"

// scene ���� Ȯ����
//#define MKDEF_PA_SCENE_FILE_EXT_BINARY L"msd" // minikey scene data
//#define MKDEF_PA_SCENE_FILE_EXT_TEXT L"txt"

//------------------------------------------------------------------------------------------------//
// DEBUG
//------------------------------------------------------------------------------------------------//

// MkWindowManagerNode���� ������� event ��� ����
#define MKDEF_PA_SHOW_WIN_MGR_EVENT_REPORT TRUE
