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
#define MKDEF_PA_TEXTNODE_COLOR_KEY 0xffff60C0

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

// ePA_SNE_Cursor(L/M/R)BtnHold event 발생 조건 시간(해당 window를 누르고 있는 시간)
// 값이 0. 이하면 ePA_SNE_Cursor(L/M/R)BtnPressed, ePA_SNE_Cursor(L/M/R)BtnDBClicked 직후 바로 동작
//#define MKDEF_PA_HOLD_EVENT_TIME_CONDITION 0.6 // sec.
#define MKDEF_PA_HOLD_EVENT_TIME_CONDITION 0.6 // sec.

//------------------------------------------------------------------------------------------------//
// DEBUG
//------------------------------------------------------------------------------------------------//

// MkWindowManagerNode에서 보고받은 event 출력 여부
#define MKDEF_PA_SHOW_WIN_MGR_EVENT_REPORT TRUE
