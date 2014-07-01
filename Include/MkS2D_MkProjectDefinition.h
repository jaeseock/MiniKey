#pragma once

//------------------------------------------------------------------------------------------------//
// library version
//------------------------------------------------------------------------------------------------//

#define MKDEF_S2D_MAJOR_VERSION 1
#define MKDEF_S2D_MINOR_VERSION 0
#define MKDEF_S2D_BUILD_VERSION 0
#define MKDEF_S2D_REVISION_VERSION 0

//------------------------------------------------------------------------------------------------//

// render config ���ϸ�
#define MKDEF_S2D_CONFIG_FILE_NAME L"MkRenderConfig.txt"

// render config ���Ͽ��� scene deco text table ���� ��θ� key
#define MKDEF_S2D_SCENE_DECO_TEXT_TABLE_FILE_PATH L"SceneDecoTextTable"

// render config ���Ͽ��� font resource ����
#define MKDEF_S2D_FONT_RESOURCE_NODE_NAME L"FontResource"

// font resource �׸񿡼� font type ����
#define MKDEF_S2D_FONT_TYPE_NODE_NAME L"FontType"

// font resource �׸񿡼� font color ����
#define MKDEF_S2D_FONT_COLOR_NODE_NAME L"FontColor"

// font resource �׸񿡼� font state ����
#define MKDEF_S2D_FONT_STATE_NODE_NAME L"FontState"

// render config ���Ͽ��� window type image set ����
#define MKDEF_S2D_WINDOW_TYPE_IMAGE_SET_NODE_NAME L"WindowTypeImageSet"

// render config ���Ͽ��� window preset ����
#define MKDEF_S2D_WINDOW_PRESET_NODE_NAME L"WindowPreset"

//------------------------------------------------------------------------------------------------//

// MkDecoStr���� tag block ������
#define MKDEF_S2D_DECOSTR_TAG_BEGIN L"<%"
#define MKDEF_S2D_DECOSTR_TAG_END L"%>"

// MkDecoStr���� tag seperator
#define MKDEF_S2D_DECOSTR_TAG_SEPERATOR L":"

// MkDecoStr���� font type tag keyword
#define MKDEF_S2D_DECOSTR_TYPE_KEYWORD L"T"

// MkDecoStr���� font state tag keyword
#define MKDEF_S2D_DECOSTR_STATE_KEYWORD L"S"

// MkDecoStr���� line feed size tag keyword
#define MKDEF_S2D_DECOSTR_LFS_KEYWORD L"LFS"

// MkDecoStr���� ���� ó�� �� color key(D3DCOLOR)
#define MKDEF_S2D_DECOSTR_COLOR_KEY 0xff010101

//------------------------------------------------------------------------------------------------//

// world���� image rect�� ������ �� �ִ� ���� ���� �Ÿ�(���� ���� 0.f)
#define MKDEF_S2D_MAX_WORLD_DEPTH 1000.f

// ������ ���� grid ����
#define MKDEF_BASE_WINDOW_DEPTH_GRID 0.025f

// list button(sub spread button) ���� grid ����
#define MKDEF_LIST_BUTTON_DEPTH_GRID 0.002f

// check button���� ������ ũ�⿡ �� �ȼ��� ���� ��� ũ��� ���� ����(ex> 4�� ��� �������� 14px�̸� ��ư ����� 18px)
#define MKDEF_CHECK_BUTTON_SIZE_OFFSET 4

// ������ ������ draw step priority
#define MKDEF_WINDOW_MGR_DRAW_STEP_PRIORITY 0x7fffffff

// ������ ������ ���� ǥ�� ���̾� ���ӽð�
#define MKDEF_WINDOW_MGR_REGION_LAYER_LIFETIME 0.5f

//------------------------------------------------------------------------------------------------//

// image subset ���� Ȯ����
#define MKDEF_S2D_IMAGE_SUBSET_FILE_EXTENSION L"mis"

// image subset ���Ͽ��� group key
#define MKDEF_S2D_IMAGE_SUBSET_FILE_GROUP_KEY L"Group"

// image subset ���Ͽ��� ��ġ �� key
#define MKDEF_S2D_IMAGE_SUBSET_FILE_POSITION_KEY L"Position"

// image subset ���Ͽ��� ũ�� �� key
#define MKDEF_S2D_IMAGE_SUBSET_FILE_SIZE_KEY L"Size"

// image subset ���Ͽ��� ���� �迭 �� key
#define MKDEF_S2D_IMAGE_SUBSET_FILE_QUILT_KEY L"Quilt"

// image subset ���Ͽ��� ���̺� �迭 �� key
#define MKDEF_S2D_IMAGE_SUBSET_FILE_TABLE_KEY L"Table"

//------------------------------------------------------------------------------------------------//
// building template node
//------------------------------------------------------------------------------------------------//

// MkSRect ���ø� �̸�
#define MKDEF_S2D_BT_SRECT_TEMPLATE_NAME L"T_SRect"

// MkSceneNode ���ø� �̸�
#define MKDEF_S2D_BT_SCENENODE_TEMPLATE_NAME L"T_SceneNode"

// MkBaseWindowNode ���ø� �̸�
#define MKDEF_S2D_BT_BASEWINNODE_TEMPLATE_NAME L"T_BaseWindowNode"

// MkSpreadButtonNode ���ø� �̸�
#define MKDEF_S2D_BT_SPREADBUTTON_TEMPLATE_NAME L"T_SpreadButtonNode"

// MkCheckButtonNode ���ø� �̸�
#define MKDEF_S2D_BT_CHECKBUTTON_TEMPLATE_NAME L"T_CheckButtonNode"

// MkSceneNode data���� �ڽ� SRect ����
#define MKDEF_S2D_SND_CHILD_SRECT_NODE_NAME L"<SRect>"

// MkSceneNode data���� �ڽ� scene node ����
#define MKDEF_S2D_SND_CHILD_SNODE_NODE_NAME L"<SceneNode>"

//------------------------------------------------------------------------------------------------//

// normalmode <-> edit mode ��ȯ key. VK_CONTROL + VK_SHIFT + 
#define MKDEF_S2D_TOGGLE_KEY_BETWEEN_NORMAL_AND_EDIT_MODE L'E'

// edit mode�� ����â ��� ��ư
#define MKDEF_S2D_SETTING_WINDOW_TOGGLE_KEY VK_F2

//------------------------------------------------------------------------------------------------//
// DEBUG
//------------------------------------------------------------------------------------------------//

// SRect�� �׸� �� ���õǰ� �з��� ���� ����� �׷� ���� ��� ����
//#define MKDEF_S2D_DEBUG_SHOW_SRECT_SET_INFO
