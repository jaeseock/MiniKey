#pragma once


//------------------------------------------------------------------------------------------------//

// blend type
enum eS2D_MaterialBlendType
{
	eS2D_MBT_Opaque = 0,
	eS2D_MBT_AlphaChannel,
	eS2D_MBT_ColorAdd,
	eS2D_MBT_ColorMult
};

// scene node type
enum eS2D_SceneNodeType
{
	eS2D_SNT_SceneNode = 0, // MkSceneNode
	eS2D_SNT_BaseWindowNode, // MkBaseWindowNode
	eS2D_SNT_SpreadButtonNode, // MkSpreadButtonNode
	eS2D_SNT_CheckButtonNode // MkCheckButtonNode
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

	eS2D_WPC_BackgroundStateTypeEnd,

	// eS2D_TitleState
	eS2D_WPC_TitleWindow = eS2D_WPC_BackgroundStateTypeEnd, // L"TitleWindow"

	// eS2D_WindowState
	eS2D_WPC_WindowStateTypeBegin,

	eS2D_WPC_NormalButton = eS2D_WPC_WindowStateTypeBegin, // L"NormalButton"
	eS2D_WPC_OKButton, // L"OKButton"
	eS2D_WPC_CancelButton, // L"CancelButton"
	eS2D_WPC_RootButton, // L"RootButton"
	eS2D_WPC_ListButton, // L"ListButton"
	eS2D_WPC_UncheckButton, // L"UncheckButton"
	eS2D_WPC_CheckButton, // L"CheckButton"
	eS2D_WPC_CloseIcon, // L"CloseIcon"

	eS2D_WPC_WindowStateTypeEnd,

	eS2D_WPC_MaxWindowPresetComponent = eS2D_WPC_WindowStateTypeEnd
};

enum eS2D_BackgroundState
{
	eS2D_BS_DefaultState = 0, // L"BG"

	eS2D_BS_MaxBackgroundState
};

enum eS2D_TitleState
{
	eS2D_TS_OnFocusState = 0, // L"OnFocus"
	eS2D_TS_LostFocusState, // L"LostFocus"

	eS2D_TS_MaxTitleState
};

enum eS2D_WindowState
{
	eS2D_WS_DefaultState = 0, // L"Default"
	eS2D_WS_OnCursorState, // L"OnCursor"
	eS2D_WS_OnClickState, // L"OnClick"
	eS2D_WS_DisableState, // L"Disable"

	eS2D_WS_MaxWindowState
};

// window preset component 대역폭 판별
inline bool IsBackgroundStateType(eS2D_WindowPresetComponent component)
{
	return ((component >= eS2D_WPC_BackgroundStateTypeBegin) && (component < eS2D_WPC_BackgroundStateTypeEnd));
}

inline bool IsTitleStateType(eS2D_WindowPresetComponent component)
{
	return (component == eS2D_WPC_TitleWindow);
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
