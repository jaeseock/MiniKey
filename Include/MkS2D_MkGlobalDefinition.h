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

// window preset component
enum eS2D_WindowPresetComponent
{
	eS2D_WPC_None = -1,

	eS2D_WPC_BackgroundWindow = 0, // L"BackgroundWindow"
	eS2D_WPC_TitleWindow, // eS2D_TitleState, L"TitleWindow"
	eS2D_WPC_NegativeButton, // eS2D_WindowState, L"NegativeButton"
	eS2D_WPC_PossitiveButton, // eS2D_WindowState, L"PossitiveButton"
	eS2D_WPC_CancelIcon, // eS2D_WindowState, L"CancelIcon"

	eS2D_WPC_MaxWindowPresetComponent
};

// eS2D_WPC_BackgroundWindow state
enum eS2D_BackgroundState
{
	eS2D_BS_DefaultState = 0, // L"Default"

	eS2D_BS_MaxBackgroundState
};

// eS2D_WPC_TitleWindow state
enum eS2D_TitleState
{
	eS2D_TS_OnFocusState = 0, // L"OnFocus"
	eS2D_TS_LostFocusState, // L"LostFocus"

	eS2D_TS_MaxTitleState
};

// eS2D_WPC_NegativeButton, eS2D_WPC_PossitiveButton, eS2D_WPC_CancelIcon state
enum eS2D_WindowState
{
	eS2D_WS_DefaultState = 0, // L"Default"
	eS2D_WS_OnCursorState, // L"OnCursor"
	eS2D_WS_OnClickState, // L"OnClick"
	eS2D_WS_DisableState, // L"Disable"

	eS2D_WS_MaxWindowState
};

// window preset component 별 state 수를 반환
inline unsigned int GetWindowPresetComponentSize(eS2D_WindowPresetComponent component)
{
	switch (component)
	{
	case eS2D_WPC_BackgroundWindow:
		return eS2D_BS_MaxBackgroundState;

	case eS2D_WPC_TitleWindow:
		return eS2D_TS_MaxTitleState;

	case eS2D_WPC_NegativeButton:
	case eS2D_WPC_PossitiveButton:
	case eS2D_WPC_CancelIcon:
		return eS2D_WS_MaxWindowState;
	}
	return 0;
}
