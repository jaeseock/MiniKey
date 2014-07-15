
#include "MkS2D_MkWindowEventManager.h"
#include "MkS2D_MkEditModeNodeSelectionWindow.h"


//const static MkHashStr SHOW_TARGET_WIN_REGION_NAME = L"ShowTargetWinReg";
//const static MkHashStr ALLOW_DRAG_MOVEMENT_NAME = L"AllowDragMovement";

//#define MKDEF_EDIT_MODE_SETTING_WIN_SIDE_MARGIN MkFloat2(20.f, 20.f)
//#define MKDEF_EDIT_MODE_SETTING_WIN_CTRL_MARGIN 10.f

//------------------------------------------------------------------------------------------------//

void MkEditModeNodeSelectionWindow::SetUp(MkBaseWindowNode* targetNode)
{
	Clear();

	if (targetNode != NULL)
	{
		targetNode->__BuildInformationTree(this, 0);
		Update();
	}
}

void MkEditModeNodeSelectionWindow::Activate(void)
{
	AlignPosition(MK_WIN_EVENT_MGR.GetRegionRect(), eRAP_LeftTop, MkInt2(30, MKDEF_S2D_NODE_SEL_WINDOW_MARGIN));
}

void MkEditModeNodeSelectionWindow::UseWindowEvent(WindowEvent& evt)
{
	/*
	if (evt.node->GetNodeName() == SHOW_TARGET_WIN_REGION_NAME)
	{
		switch (evt.type)
		{
		case MkSceneNodeFamilyDefinition::eCheckOn: MK_WIN_EVENT_MGR.__SetShowWindowSelection(true); break;
		case MkSceneNodeFamilyDefinition::eCheckOff: MK_WIN_EVENT_MGR.__SetShowWindowSelection(false); break;
		}
	}
	else if (evt.node->GetNodeName() == ALLOW_DRAG_MOVEMENT_NAME)
	{
		switch (evt.type)
		{
		case MkSceneNodeFamilyDefinition::eCheckOn: MK_WIN_EVENT_MGR.__SetAllowDragMovement(true); break;
		case MkSceneNodeFamilyDefinition::eCheckOff: MK_WIN_EVENT_MGR.__SetAllowDragMovement(false); break;
		}
	}
	*/
}

//------------------------------------------------------------------------------------------------//
