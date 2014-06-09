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

//------------------------------------------------------------------------------------------------//

// image subset ���� Ȯ����
#define MKDEF_S2D_IMAGE_SUBSET_FILE_EXTENSION L"mis"

// image subset ���Ͽ��� ��ġ �� key
#define MKDEF_S2D_IMAGE_SUBSET_FILE_POSITION_KEY L"Position"

// image subset ���Ͽ��� ũ�� �� key
#define MKDEF_S2D_IMAGE_SUBSET_FILE_SIZE_KEY L"Size"

// image subset ���Ͽ��� ���� �迭 �� key
#define MKDEF_S2D_IMAGE_SUBSET_FILE_QUILT_KEY L"Quilt"

// image subset ���Ͽ��� ���̺� �迭 �� key
#define MKDEF_S2D_IMAGE_SUBSET_FILE_TABLE_KEY L"Table"

//------------------------------------------------------------------------------------------------//
// building template node : MkSRect
//------------------------------------------------------------------------------------------------//

// ���ø� �̸�
#define MKDEF_S2D_BT_SRECT_TEMPLATE_NAME L"T_SRect"

// ��ġ key
#define MKDEF_S2D_BT_SRECT_POSITION_KEY L"Position"

// ���� key
#define MKDEF_S2D_BT_SRECT_DEPTH_KEY L"Depth"

// ���ҽ� key
#define MKDEF_S2D_BT_SRECT_RESOURCE_KEY L"Resource"

// ���ҽ� data�� bitmap texture���� �����ϴ� �±�
#define MKDEF_S2D_BT_SRECT_RESOURCE_MAP_TAG L"map"

// ���ҽ� data�� original deco text���� �����ϴ� �±�
#define MKDEF_S2D_BT_SRECT_RESOURCE_TEXT_ORIGIN_TAG L"odt"

// ���ҽ� data�� scene deco text table�� key���� �����ϴ� �±�
#define MKDEF_S2D_BT_SRECT_RESOURCE_TEXT_SCENE_TAG L"sdt"

// ���İ� key
#define MKDEF_S2D_BT_SRECT_ALPHA_KEY L"Alpha"

// ���� �÷��� key
#define MKDEF_S2D_BT_SRECT_REFLECTION_KEY L"Reflection"

// ���̱� ���� �÷��� key
#define MKDEF_S2D_BT_SRECT_VISIBLE_KEY L"Visible"

//------------------------------------------------------------------------------------------------//
// building template node : MkSceneNode
//------------------------------------------------------------------------------------------------//

// ���ø� �̸�
#define MKDEF_S2D_BT_SCENENODE_TEMPLATE_NAME L"T_SceneNode"

// ��� ���� key
#define MKDEF_S2D_BT_SCENENODE_NODE_TYPE_KEY L"NodeType"

// ��ġ key
#define MKDEF_S2D_BT_SCENENODE_POSITION_KEY L"Position"

// ȸ�� key
#define MKDEF_S2D_BT_SCENENODE_ROTATION_KEY L"Rotation"

// ũ�� key
#define MKDEF_S2D_BT_SCENENODE_SCALE_KEY L"Scale"

// ���İ� key
#define MKDEF_S2D_BT_SCENENODE_ALPHA_KEY L"Alpha"

// ���̱� ���� �÷��� key
#define MKDEF_S2D_BT_SCENENODE_VISIBLE_KEY L"Visible"

//------------------------------------------------------------------------------------------------//
// building template node : MkBaseWindowNode
//------------------------------------------------------------------------------------------------//

// ���ø� �̸�
#define MKDEF_S2D_BT_BASEWINNODE_TEMPLATE_NAME L"T_BaseWindowNode"

// ������ ���� key
#define MKDEF_S2D_BT_BASEWINNODE_ENABLE_KEY L"Enable"

// ���� ��� key
#define MKDEF_S2D_BT_BASEWINNODE_ALIGNMENT_TYPE_KEY L"AlignType"

// ���� ��� key
#define MKDEF_S2D_BT_BASEWINNODE_ALIGNMENT_BORDER_KEY L"AlignBorder"

// ���� ��� �̸� key
#define MKDEF_S2D_BT_BASEWINNODE_ALIGNMENT_TARGET_NAME_KEY L"AlignTargetName"

//------------------------------------------------------------------------------------------------//

// scene node data���� �ڽ� SRect ����
#define MKDEF_S2D_SND_CHILD_SRECT_NODE_NAME L"<SRect>"

// scene node data���� �ڽ� scene node ����
#define MKDEF_S2D_SND_CHILD_SNODE_NODE_NAME L"<SceneNode>"

//------------------------------------------------------------------------------------------------//
// DEBUG
//------------------------------------------------------------------------------------------------//

// SRect�� �׸� �� ���õǰ� �з��� ���� ����� �׷� ���� ��� ����
//#define MKDEF_S2D_DEBUG_SHOW_SRECT_SET_INFO
