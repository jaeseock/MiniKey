#pragma once


//------------------------------------------------------------------------------------------------//

enum ePA_MaterialBlendType
{
	ePA_MBT_Opaque = 0,
	ePA_MBT_AlphaChannel,
	ePA_MBT_ColorAdd,
	ePA_MBT_ColorMult
};
/*
// scene node type
enum eS2D_SceneNodeType
{
	eS2D_SNT_SceneNode = 0, // MkSceneNode
	eS2D_SNT_BaseWindowNode, // MkBaseWindowNode

	eS2D_SNT_ControlWindowNodeBegin,

	eS2D_SNT_SpreadButtonNode = eS2D_SNT_ControlWindowNodeBegin, // MkSpreadButtonNode
	eS2D_SNT_CheckButtonNode, // MkCheckButtonNode
	eS2D_SNT_ScrollBarNode, // MkScrollBarNode
	eS2D_SNT_EditBoxNode, // MkEditBoxNode
	eS2D_SNT_TabWindowNode, // MkTabWindowNode

	eS2D_SNT_ControlWindowNodeEnd
};

// window preset component
enum eS2D_WindowPresetComponent
{
	eS2D_WPC_None = -1,

	// eS2D_BackgroundState
	eS2D_WPC_BackgroundStateTypeBegin = 0,

	eS2D_WPC_BackgroundWindow = eS2D_WPC_BackgroundStateTypeBegin, // L"BackgroundWindow"
	eS2D_WPC_StaticWindow, // L"StaticWindow"
	eS2D_WPC_GuideBox, // L"GuideBox"
	eS2D_WPC_ArrowLeft, // L"ArrowLeft"
	eS2D_WPC_ArrowRight, // L"ArrowRight"
	eS2D_WPC_ArrowUp, // L"ArrowUp"
	eS2D_WPC_ArrowDown, // L"ArrowDown"
	eS2D_WPC_VSlideBar, // L"VSlideBar"
	eS2D_WPC_HSlideBar, // L"HSlideBar"
	eS2D_WPC_TabFront, // L"TabFront"

	eS2D_WPC_BackgroundStateTypeEnd,

	// eS2D_TitleState
	eS2D_WPC_TitleStateTypeBegin = eS2D_WPC_BackgroundStateTypeEnd,

	eS2D_WPC_NormalTitle = eS2D_WPC_TitleStateTypeBegin, // L"TitleWindow"
	eS2D_WPC_SystemMsgTitle,
	eS2D_WPC_SystemErrorTitle,

	eS2D_WPC_TitleStateTypeEnd,

	// eS2D_WindowState
	eS2D_WPC_WindowStateTypeBegin = eS2D_WPC_TitleStateTypeEnd,

	eS2D_WPC_NormalButton = eS2D_WPC_WindowStateTypeBegin, // L"NormalBtn"
	eS2D_WPC_OKButton, // L"OKBtn"
	eS2D_WPC_CancelButton, // L"CancelBtn"
	eS2D_WPC_SelectionButton, // L"SelectionBtn"
	eS2D_WPC_TabRearButton, // L"TabRearButton"
	eS2D_WPC_RootButton, // L"RootBtn"
	eS2D_WPC_ListButton, // L"ListBtn"
	eS2D_WPC_UncheckButton, // L"UncheckBtn"
	eS2D_WPC_CheckButton, // L"CheckBtn"
	eS2D_WPC_DirLeftButton, // L"DirLeftBtn"
	eS2D_WPC_DirRightButton, // L"DirRightBtn"
	eS2D_WPC_DirUpButton, // L"DirUpBtn"
	eS2D_WPC_DirDownButton, // L"DirDownBtn"
	eS2D_WPC_VSlideButton, // L"VSlideBtn"
	eS2D_WPC_HSlideButton, // L"HSlideBtn"
	eS2D_WPC_CloseButton, // L"CloseBtn"

	eS2D_WPC_WindowStateTypeEnd,

	eS2D_WPC_MaxWindowPresetComponent = eS2D_WPC_WindowStateTypeEnd
};

enum eS2D_BackgroundState
{
	eS2D_BS_DefaultState = 0, // L"__#BG"

	eS2D_BS_MaxBackgroundState
};

enum eS2D_TitleState
{
	eS2D_TS_OnFocusState = 0, // L"__#OnFocus"
	eS2D_TS_LostFocusState, // L"__#LostFocus"

	eS2D_TS_MaxTitleState
};

enum eS2D_WindowState
{
	eS2D_WS_DefaultState = 0, // L"__#Default"
	eS2D_WS_OnCursorState, // L"__#OnCursor"
	eS2D_WS_OnClickState, // L"__#OnClick"
	eS2D_WS_DisableState, // L"__#Disable"

	eS2D_WS_MaxWindowState
};

// window preset component 대역폭 판별
inline bool IsBackgroundStateType(eS2D_WindowPresetComponent component)
{
	return ((component >= eS2D_WPC_BackgroundStateTypeBegin) && (component < eS2D_WPC_BackgroundStateTypeEnd));
}

inline bool IsTitleStateType(eS2D_WindowPresetComponent component)
{
	return ((component >= eS2D_WPC_TitleStateTypeBegin) && (component < eS2D_WPC_TitleStateTypeEnd));
}

inline bool IsWindowStateType(eS2D_WindowPresetComponent component)
{
	return ((component >= eS2D_WPC_WindowStateTypeBegin) && (component < eS2D_WPC_WindowStateTypeEnd));
}

// window preset component 별 state 수를 반환
inline unsigned int GetWindowPresetComponentSize(eS2D_WindowPresetComponent component)
{
	if (IsBackgroundStateType(component))
	{
		return eS2D_BS_MaxBackgroundState;
	}
	else if (IsTitleStateType(component))
	{
		return eS2D_TS_MaxTitleState;
	}
	else if (IsWindowStateType(component))
	{
		return eS2D_WS_MaxWindowState;
	}
	return 0;
}
*/