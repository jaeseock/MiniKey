#pragma once


//------------------------------------------------------------------------------------------------//

enum ePA_MaterialBlendType
{
	ePA_MBT_Opaque = 0,
	ePA_MBT_AlphaChannel,
	ePA_MBT_ColorAdd,
	ePA_MBT_ColorMult
};

// scene (render) object type
enum ePA_SceneObjectType
{
	ePA_SOT_Panel = 0, // MkPanel
	ePA_SOT_LineShape // MkLineShape
};

// scene node type
enum ePA_SceneNodeType
{
	ePA_SNT_SceneNode = 0, // MkSceneNode
	ePA_SNT_WindowManagerNode, // MkWindowManagerNode
	ePA_SNT_VisualPatternNode, // MkVisualPatternNode
	ePA_SNT_WindowTagNode, // MkWindowTagNode
	ePA_SNT_WindowThemedNode, // MkWindowThemedNode
	ePA_SNT_ProgressBarNode, // MkProgressBarNode
	ePA_SNT_WindowBaseNode, // MkWindowBaseNode
	ePA_SNT_TitleBarControlNode, // MkTitleBarControlNode
	ePA_SNT_BodyFrameControlNode, // MkBodyFrameControlNode
	ePA_SNT_CheckBoxControlNode, // MkCheckBoxControlNode
	ePA_SNT_ScrollBarControlNode, // MkScrollBarControlNode
	ePA_SNT_SliderControlNode, // MkSliderControlNode
	ePA_SNT_EditBoxControlNode, // MkEditBoxControlNode
	ePA_SNT_ListBoxControlNode, // MkListBoxControlNode
	ePA_SNT_DropDownListControlNode, // M_DropDownListControlNode
	ePA_SNT_ScenePortalNode // MkScenePortalNode
};

// scene node attribute
// (NOTE) data에 저장되는 값
enum ePA_SceneNodeAttribute
{
	// MkSceneNode
	ePA_SNA_Visible = 0,
	ePA_SNA_SkipUpdateWhileInvisible,

	ePA_SNA_SceneNodeBandwidth = 8, // 대역폭 8개 확보

	// MkVisualPatternNode
	ePA_SNA_AcceptInput = ePA_SNA_SceneNodeBandwidth,
	ePA_SNA_AlignmentPivotIsWindowRect,
	ePA_SNA_AlignmentTargetIsWindowRect,

	ePA_SNA_VisualPatternNodeBandwidth = ePA_SNA_SceneNodeBandwidth + 8, // 대역폭 8개 확보

	// MkWindowBaseNode
	ePA_SNA_Enable = ePA_SNA_VisualPatternNodeBandwidth,
	ePA_SNA_MovableByDragging,

	ePA_SNA_WindowBaseNodeBandwidth = ePA_SNA_VisualPatternNodeBandwidth + 8, // 대역폭 8개 확보

	ePA_SNA_EndOfBandwidth = 32 // attribute 최대 값. bit flag가 32bit이기 때문에 제한이 걸림
};

// scene node event
// (NOTE) 변경이 생기면 디버깅을 위해 MkWindowManagerNode.cpp의 NODE_EVT_NAME, SendNodeReportTypeEvent()에도 반영 할 것
enum ePA_SceneNodeEvent
{
	ePA_SNE_None = -1,

	// MkSceneNode
	ePA_SNE_DragMovement = 0, // ArgKey_DragMovement : MkVec2(world x, y)
	ePA_SNE_BeginDrag,
	ePA_SNE_EndDrag,

	// MkWindowThemedNode
	ePA_SNE_ChangeTheme, // ArgKey_ChangeTheme : MkArray<MkHashStr> src name / dest name

	// MkWindowBaseNode
	ePA_SNE_CursorEntered,
	ePA_SNE_CursorLeft,
	ePA_SNE_CursorMoved, // ArgKey_CursorLocalPosition : MkVec2(local x, y), ArgKey_CursorWorldPosition : MkVec2(world x, y)
	ePA_SNE_CursorLBtnPressed, // ArgKey_CursorLocalPosition : MkVec2(local x, y), ArgKey_CursorWorldPosition : MkVec2(world x, y)
	ePA_SNE_CursorLBtnReleased, // ArgKey_CursorLocalPosition : MkVec2(local x, y), ArgKey_CursorWorldPosition : MkVec2(world x, y)
	ePA_SNE_CursorLBtnDBClicked, // ArgKey_CursorLocalPosition : MkVec2(local x, y), ArgKey_CursorWorldPosition : MkVec2(world x, y)
	ePA_SNE_CursorLBtnHold, // ArgKey_CursorLocalPosition : MkVec2(local x, y), ArgKey_CursorWorldPosition : MkVec2(world x, y)
	ePA_SNE_CursorMBtnPressed, // ArgKey_CursorLocalPosition : MkVec2(local x, y), ArgKey_CursorWorldPosition : MkVec2(world x, y)
	ePA_SNE_CursorMBtnReleased, // ArgKey_CursorLocalPosition : MkVec2(local x, y), ArgKey_CursorWorldPosition : MkVec2(world x, y)
	ePA_SNE_CursorMBtnDBClicked, // ArgKey_CursorLocalPosition : MkVec2(local x, y), ArgKey_CursorWorldPosition : MkVec2(world x, y)
	ePA_SNE_CursorMBtnHold, // ArgKey_CursorLocalPosition : MkVec2(local x, y), ArgKey_CursorWorldPosition : MkVec2(world x, y)
	ePA_SNE_CursorRBtnPressed, // ArgKey_CursorLocalPosition : MkVec2(local x, y), ArgKey_CursorWorldPosition : MkVec2(world x, y)
	ePA_SNE_CursorRBtnReleased, // ArgKey_CursorLocalPosition : MkVec2(local x, y), ArgKey_CursorWorldPosition : MkVec2(world x, y)
	ePA_SNE_CursorRBtnDBClicked, // ArgKey_CursorLocalPosition : MkVec2(local x, y), ArgKey_CursorWorldPosition : MkVec2(world x, y)
	ePA_SNE_CursorRBtnHold, // ArgKey_CursorLocalPosition : MkVec2(local x, y), ArgKey_CursorWorldPosition : MkVec2(world x, y)
	ePA_SNE_WheelMoved, // ArgKey_WheelDelta : int delta
	ePA_SNE_Activate,
	ePA_SNE_Deactivate,
	ePA_SNE_OnFocus,
	ePA_SNE_LostFocus,
	ePA_SNE_ToggleExclusiveWindow, // ArgKey_ExclusiveWindow : MkArray<MkHashStr> target node path, ArgKey_ExclusiveException : MkArray<MkHashStr> exc. window exception

	// MkTitleBarControlNode
	ePA_SNE_CloseWindow,

	// MkCheckBoxControlNode
	ePA_SNE_CheckOn,
	ePA_SNE_CheckOff,

	// MkScrollBarControlNode
	ePA_SNE_ScrollBarMoved, // ArgKey_NewItemPosOfScrollBar : int(new item pos, last item pos, total item size)

	// MkSliderControlNode
	ePA_SNE_SliderMoved, // ArgKey_SliderPos : int

	// MkEditBoxControlNode
	ePA_SNE_TextModified, // ArgKey_Text : str
	ePA_SNE_TextCommitted, // ArgKey_Text : str

	// MkListBoxControlNode. ArgKey_Item : hash str
	ePA_SNE_ItemEntered,
	ePA_SNE_ItemLeft,
	ePA_SNE_ItemLBtnPressed,
	ePA_SNE_ItemLBtnReleased,
	ePA_SNE_ItemLBtnDBClicked,
	ePA_SNE_ItemMBtnPressed,
	ePA_SNE_ItemMBtnReleased,
	ePA_SNE_ItemMBtnDBClicked,
	ePA_SNE_ItemRBtnPressed,
	ePA_SNE_ItemRBtnReleased,
	ePA_SNE_ItemRBtnDBClicked,

	// MkDropDownListControlNode
	ePA_SNE_DropDownItemSet // MkListBoxControlNode::ArgKey_Item 공유
};
