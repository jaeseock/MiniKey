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
#define MKDEF_PA_TEXTNODE_COLOR_KEY 0xffff60C0

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

// ePA_SNE_Cursor(L/M/R)BtnHold event �߻� ���� �ð�(�ش� window�� ������ �ִ� �ð�)
// ���� 0. ���ϸ� ePA_SNE_Cursor(L/M/R)BtnPressed, ePA_SNE_Cursor(L/M/R)BtnDBClicked ���� �ٷ� ����
//#define MKDEF_PA_HOLD_EVENT_TIME_CONDITION 0.6 // sec.
#define MKDEF_PA_HOLD_EVENT_TIME_CONDITION 0.6 // sec.

//------------------------------------------------------------------------------------------------//
// DEBUG
//------------------------------------------------------------------------------------------------//

// MkWindowManagerNode���� ������� event ��� ����
#define MKDEF_PA_SHOW_WIN_MGR_EVENT_REPORT TRUE
