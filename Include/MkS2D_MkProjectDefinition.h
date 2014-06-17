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

//------------------------------------------------------------------------------------------------//

// image subset 파일 확장자
#define MKDEF_S2D_IMAGE_SUBSET_FILE_EXTENSION L"mis"

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

// MkSceneNode data에서 자식 SRect 노드명
#define MKDEF_S2D_SND_CHILD_SRECT_NODE_NAME L"<SRect>"

// MkSceneNode data에서 자식 scene node 노드명
#define MKDEF_S2D_SND_CHILD_SNODE_NODE_NAME L"<SceneNode>"

//------------------------------------------------------------------------------------------------//

// MkWindowEventManager에서의 깊이 대역폭
// 시스템에서의 깊이 영역(0.f ~ MKDEF_S2D_MAX_WORLD_DEPTH) 중,
// 해당 깊이(MKDEF_S2D_MIN_WINDOW_DEPTH_BANDWIDTH ~ MKDEF_S2D_MAX_WINDOW_DEPTH_BANDWIDTH)를 윈도우용으로 사용한다는 의미
#define MKDEF_S2D_MIN_WINDOW_DEPTH_BANDWIDTH 10.f
#define MKDEF_S2D_MAX_WINDOW_DEPTH_BANDWIDTH 100.f

// normalmode <-> edit mode 전환 key. VK_CONTROL + VK_SHIFT + 
#define MKDEF_S2D_TOGGLE_KEY_BETWEEN_NORMAL_AND_EDIT_MODE L'E'

//------------------------------------------------------------------------------------------------//
// DEBUG
//------------------------------------------------------------------------------------------------//

// SRect를 그릴 때 소팅되고 분류된 최종 결과인 그룹 정보 출력 여부
//#define MKDEF_S2D_DEBUG_SHOW_SRECT_SET_INFO
