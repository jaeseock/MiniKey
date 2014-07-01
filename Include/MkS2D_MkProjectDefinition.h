#pragma once

//------------------------------------------------------------------------------------------------//
// library version
//------------------------------------------------------------------------------------------------//

#define MKDEF_S2D_MAJOR_VERSION 1
#define MKDEF_S2D_MINOR_VERSION 0
#define MKDEF_S2D_BUILD_VERSION 0
#define MKDEF_S2D_REVISION_VERSION 0

//------------------------------------------------------------------------------------------------//

// render config 파일명
#define MKDEF_S2D_CONFIG_FILE_NAME L"MkRenderConfig.txt"

// render config 파일에서 scene deco text table 파일 경로명 key
#define MKDEF_S2D_SCENE_DECO_TEXT_TABLE_FILE_PATH L"SceneDecoTextTable"

// render config 파일에서 font resource 노드명
#define MKDEF_S2D_FONT_RESOURCE_NODE_NAME L"FontResource"

// font resource 항목에서 font type 노드명
#define MKDEF_S2D_FONT_TYPE_NODE_NAME L"FontType"

// font resource 항목에서 font color 노드명
#define MKDEF_S2D_FONT_COLOR_NODE_NAME L"FontColor"

// font resource 항목에서 font state 노드명
#define MKDEF_S2D_FONT_STATE_NODE_NAME L"FontState"

// render config 파일에서 window type image set 노드명
#define MKDEF_S2D_WINDOW_TYPE_IMAGE_SET_NODE_NAME L"WindowTypeImageSet"

// render config 파일에서 window preset 노드명
#define MKDEF_S2D_WINDOW_PRESET_NODE_NAME L"WindowPreset"

//------------------------------------------------------------------------------------------------//

// MkDecoStr에서 tag block 구분자
#define MKDEF_S2D_DECOSTR_TAG_BEGIN L"<%"
#define MKDEF_S2D_DECOSTR_TAG_END L"%>"

// MkDecoStr에서 tag seperator
#define MKDEF_S2D_DECOSTR_TAG_SEPERATOR L":"

// MkDecoStr에서 font type tag keyword
#define MKDEF_S2D_DECOSTR_TYPE_KEYWORD L"T"

// MkDecoStr에서 font state tag keyword
#define MKDEF_S2D_DECOSTR_STATE_KEYWORD L"S"

// MkDecoStr에서 line feed size tag keyword
#define MKDEF_S2D_DECOSTR_LFS_KEYWORD L"LFS"

// MkDecoStr에서 투명 처리 될 color key(D3DCOLOR)
#define MKDEF_S2D_DECOSTR_COLOR_KEY 0xff010101

//------------------------------------------------------------------------------------------------//

// world에서 image rect가 존재할 수 있는 가장 뒤의 거리(가장 앞은 0.f)
#define MKDEF_S2D_MAX_WORLD_DEPTH 1000.f

// 윈도우 깊이 grid 단위
#define MKDEF_BASE_WINDOW_DEPTH_GRID 0.025f

// list button(sub spread button) 깊이 grid 단위
#define MKDEF_LIST_BUTTON_DEPTH_GRID 0.002f

// check button에서 아이콘 크기에 몇 픽셀을 더해 배경 크기로 할지 결정(ex> 4일 경우 아이콘이 14px이면 버튼 배경은 18px)
#define MKDEF_CHECK_BUTTON_SIZE_OFFSET 4

// 윈도우 관리자 draw step priority
#define MKDEF_WINDOW_MGR_DRAW_STEP_PRIORITY 0x7fffffff

// 윈도우 관리자 영역 표시 레이어 지속시간
#define MKDEF_WINDOW_MGR_REGION_LAYER_LIFETIME 0.5f

//------------------------------------------------------------------------------------------------//

// image subset 파일 확장자
#define MKDEF_S2D_IMAGE_SUBSET_FILE_EXTENSION L"mis"

// image subset 파일에서 group key
#define MKDEF_S2D_IMAGE_SUBSET_FILE_GROUP_KEY L"Group"

// image subset 파일에서 위치 값 key
#define MKDEF_S2D_IMAGE_SUBSET_FILE_POSITION_KEY L"Position"

// image subset 파일에서 크기 값 key
#define MKDEF_S2D_IMAGE_SUBSET_FILE_SIZE_KEY L"Size"

// image subset 파일에서 조각 배열 값 key
#define MKDEF_S2D_IMAGE_SUBSET_FILE_QUILT_KEY L"Quilt"

// image subset 파일에서 테이블 배열 값 key
#define MKDEF_S2D_IMAGE_SUBSET_FILE_TABLE_KEY L"Table"

//------------------------------------------------------------------------------------------------//
// building template node
//------------------------------------------------------------------------------------------------//

// MkSRect 템플릿 이름
#define MKDEF_S2D_BT_SRECT_TEMPLATE_NAME L"T_SRect"

// MkSceneNode 템플릿 이름
#define MKDEF_S2D_BT_SCENENODE_TEMPLATE_NAME L"T_SceneNode"

// MkBaseWindowNode 템플릿 이름
#define MKDEF_S2D_BT_BASEWINNODE_TEMPLATE_NAME L"T_BaseWindowNode"

// MkSpreadButtonNode 템플릿 이름
#define MKDEF_S2D_BT_SPREADBUTTON_TEMPLATE_NAME L"T_SpreadButtonNode"

// MkCheckButtonNode 템플릿 이름
#define MKDEF_S2D_BT_CHECKBUTTON_TEMPLATE_NAME L"T_CheckButtonNode"

// MkSceneNode data에서 자식 SRect 노드명
#define MKDEF_S2D_SND_CHILD_SRECT_NODE_NAME L"<SRect>"

// MkSceneNode data에서 자식 scene node 노드명
#define MKDEF_S2D_SND_CHILD_SNODE_NODE_NAME L"<SceneNode>"

//------------------------------------------------------------------------------------------------//

// normalmode <-> edit mode 전환 key. VK_CONTROL + VK_SHIFT + 
#define MKDEF_S2D_TOGGLE_KEY_BETWEEN_NORMAL_AND_EDIT_MODE L'E'

// edit mode시 설정창 토글 버튼
#define MKDEF_S2D_SETTING_WINDOW_TOGGLE_KEY VK_F2

//------------------------------------------------------------------------------------------------//
// DEBUG
//------------------------------------------------------------------------------------------------//

// SRect를 그릴 때 소팅되고 분류된 최종 결과인 그룹 정보 출력 여부
//#define MKDEF_S2D_DEBUG_SHOW_SRECT_SET_INFO
