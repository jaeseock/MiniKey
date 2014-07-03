
#include "MkS2D_MkCheckButtonNode.h"

#include "MkS2D_MkWindowEventManager.h"
#include "MkS2D_MkEditModeSettingWindow.h"


const static MkHashStr SHOW_TARGET_WIN_REGION_NAME = L"ShowTargetWinReg";
const static MkHashStr ALLOW_DRAG_MOVEMENT_NAME = L"AllowDragMovement";

#define MKDEF_EDIT_MODE_SETTING_WIN_SIDE_MARGIN MkFloat2(20.f, 20.f)
#define MKDEF_EDIT_MODE_SETTING_WIN_CTRL_MARGIN 10.f

//------------------------------------------------------------------------------------------------//

bool MkEditModeSettingWindow::SetUp(const MkHashStr& themeName)
{
	BasicPresetWindowDesc winDesc;
	winDesc.SetStandardDesc(themeName, true, MkFloat2(300, 400.f));
	winDesc.titleCaption = L"에디트 모드 세팅";
	winDesc.hasIcon = false;
	winDesc.hasCancelIcon = false;
	winDesc.hasCancelButton = false;
	winDesc.hasOKButton = false;

	if (CreateBasicWindow(this, MkHashStr::NullHash, winDesc) == NULL)
		return false;

	MkBaseWindowNode* bgNode = dynamic_cast<MkBaseWindowNode*>(GetChildNode(L"Background"));
	if (bgNode == NULL)
		return false;

	MkFloatRect clientRect = bgNode->GetPresetComponentSize();
	clientRect.size.y -= GetPresetComponentSize().y; // body size - title size

	MkFloat2 alignBorder = MKDEF_EDIT_MODE_SETTING_WIN_SIDE_MARGIN;
	MkBaseWindowNode::CaptionDesc captionDesc;

	for (unsigned int i=0; i<2; ++i)
	{
		MkCheckButtonNode* cbNode = NULL;
		switch (i)
		{
		case 0:
			cbNode = new MkCheckButtonNode(SHOW_TARGET_WIN_REGION_NAME);
			captionDesc = L"타겟 윈도우 영역 표시";
			break;
		case 1:
			cbNode = new MkCheckButtonNode(ALLOW_DRAG_MOVEMENT_NAME);
			captionDesc = L"기본 드래그 이동 허용";
			break;
		}

		if (cbNode != NULL)
		{
			cbNode->CreateCheckButton(themeName, captionDesc, MK_WIN_EVENT_MGR.__GetShowWindowSelection());
			cbNode->SetLocalPosition(clientRect.GetSnapPosition(MkFloatRect(cbNode->GetPresetComponentSize()), eRAP_LeftTop, alignBorder));
			cbNode->SetLocalDepth(-0.001f);
			bgNode->AttachChildNode(cbNode);

			alignBorder.y += cbNode->GetPresetComponentSize().y + MKDEF_EDIT_MODE_SETTING_WIN_CTRL_MARGIN;
		}
	}

	return true;
}

void MkEditModeSettingWindow::Activate(void)
{
	AlignPosition(MK_WIN_EVENT_MGR.GetRegionRect(), eRAP_MiddleCenter, MkInt2(0, 0)); // 중앙 정렬
}

void MkEditModeSettingWindow::UseWindowEvent(WindowEvent& evt)
{
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
}

MkEditModeSettingWindow::MkEditModeSettingWindow(const MkHashStr& name) : MkBaseWindowNode(name)
{
	
}

//------------------------------------------------------------------------------------------------//
