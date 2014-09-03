
#include "MkS2D_MkCheckButtonNode.h"
#include "MkS2D_MkSpreadButtonNode.h"

#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkWindowEventManager.h"
#include "MkS2D_MkEditModeSettingWindow.h"


const static MkHashStr SHOW_TARGET_WIN_REGION_NAME = L"ShowTargetWinReg";
const static MkHashStr ALLOW_DRAG_MOVEMENT_NAME = L"AllowDragMovement";

const static MkHashStr KEY_MOVEMENT_OFFSET_NAME = L"KeyMovementOffset";
const static MkHashStr KEY_MOVEMENT_FACTOR_NAME = L"KeyMovementFactor";

#define MKDEF_SIDE_MARGIN MkFloat2(20.f, 20.f)
#define MKDEF_CTRL_MARGIN 10.f

//------------------------------------------------------------------------------------------------//

bool MkEditModeSettingWindow::Initialize(void)
{
	SetAttribute(eAlignCenterPos, true);

	const MkHashStr& themeName = MK_WR_PRESET.GetDefaultThemeName();

	BasicPresetWindowDesc winDesc;
	winDesc.SetStandardDesc(themeName, true, MkFloat2(300.f, 400.f));
	winDesc.titleType = eS2D_WPC_SystemMsgTitle;
	winDesc.titleCaption = L"편집 기능 설정";
	winDesc.hasIcon = false;
	winDesc.hasCloseButton = true;
	winDesc.hasCancelButton = false;
	winDesc.hasOKButton = false;

	if (CreateBasicWindow(this, MkHashStr::NullHash, winDesc) == NULL)
		return false;

	MkBaseWindowNode* bgNode = dynamic_cast<MkBaseWindowNode*>(GetChildNode(L"Background"));
	if (bgNode == NULL)
		return false;

	MkFloatRect clientRect = bgNode->GetPresetComponentSize();
	clientRect.size.y -= GetPresetComponentSize().y; // body size - title size

	MkFloat2 ctrlPos = MkFloat2(MKDEF_SIDE_MARGIN.x, clientRect.size.y - MKDEF_SIDE_MARGIN.y);
	MkBaseWindowNode::CaptionDesc captionDesc;

	for (unsigned int i=0; i<4; ++i)
	{
		MkSRect* descRect = NULL;
		MkBaseWindowNode* targetNode = NULL;
		float ctrlDepth = -0.001f;
		bool available = false;
		switch (i)
		{
		case 0:
			{
				MkCheckButtonNode* controlNode = new MkCheckButtonNode(SHOW_TARGET_WIN_REGION_NAME);
				captionDesc = L"대상 윈도우 영역 표시";
				available = MK_WIN_EVENT_MGR.__GetShowWindowSelection();
				controlNode->CreateCheckButton(themeName, captionDesc, available);
				targetNode = controlNode;
			}
			break;

		case 1:
			{
				MkCheckButtonNode* controlNode = new MkCheckButtonNode(ALLOW_DRAG_MOVEMENT_NAME);
				captionDesc = L"기본 드래그 이동 허용";
				available = MK_WIN_EVENT_MGR.__GetAllowDragMovement();
				controlNode->CreateCheckButton(themeName, captionDesc, available);
				targetNode = controlNode;
			}
			break;

		case 2:
			{
				descRect = bgNode->CreateSRect(L"__#MOffsetDesc");
				descRect->SetDecoString(L"방향키 이동거리 :");

				MkSpreadButtonNode* controlNode = new MkSpreadButtonNode(KEY_MOVEMENT_OFFSET_NAME);
				controlNode->CreateSelectionRootTypeButton(themeName, MkFloat2(80.f, 20.f), MkSpreadButtonNode::eDownward);
				ItemTagInfo ti;
				for (int i=1; i<=10; ++i)
				{
					MkStr key(i);
					ti.captionDesc.SetString(key + L" px");
					controlNode->AddItem(key, ti);
				}

				MkStr targetKey = MK_WIN_EVENT_MGR.__GetKeyMovementOffset();
				controlNode->SetTargetItem(targetKey);

				targetNode = controlNode;
				ctrlDepth = -0.005f;
			}
			break;

		case 3:
			{
				descRect = bgNode->CreateSRect(L"__#MFactorDesc");
				descRect->SetDecoString(L"SHIFT + 방향키 이동시 거리 배율 :");

				MkSpreadButtonNode* controlNode = new MkSpreadButtonNode(KEY_MOVEMENT_FACTOR_NAME);
				controlNode->CreateSelectionRootTypeButton(themeName, MkFloat2(70.f, 20.f), MkSpreadButtonNode::eDownward);
				ItemTagInfo ti;
				MkStr key;

				key = 2;
				ti.captionDesc.SetString(key + L" 배");
				controlNode->AddItem(key, ti);

				key = 5;
				ti.captionDesc.SetString(key + L" 배");
				controlNode->AddItem(key, ti);

				key = 10;
				ti.captionDesc.SetString(key + L" 배");
				controlNode->AddItem(key, ti);

				key = 100;
				ti.captionDesc.SetString(key + L" 배");
				controlNode->AddItem(key, ti);

				MkStr targetKey = MK_WIN_EVENT_MGR.__GetKeyMovementFactor();
				controlNode->SetTargetItem(targetKey);

				targetNode = controlNode;
			}
			break;
		}

		if (targetNode != NULL)
		{
			float nodePosX = ctrlPos.x;
			ctrlPos.y -= targetNode->GetPresetComponentSize().y;
			if (descRect != NULL)
			{
				descRect->SetLocalPosition(MkFloat2(ctrlPos.x, ctrlPos.y + 3));
				descRect->SetLocalDepth(-0.001f);
				nodePosX += descRect->GetLocalSize().x + 6;
			}

			targetNode->SetLocalPosition(MkFloat2(nodePosX, ctrlPos.y));
			targetNode->SetLocalDepth(ctrlDepth);
			bgNode->AttachChildNode(targetNode);

			ctrlPos.y -= MKDEF_CTRL_MARGIN;
		}
	}

	return true;
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
	else if (evt.type == MkSceneNodeFamilyDefinition::eSetTargetItem)
	{
		MkSpreadButtonNode* cbNode = dynamic_cast<MkSpreadButtonNode*>(evt.node);
		if (cbNode != NULL)
		{
			int value = cbNode->GetTargetItemKey().GetString().ToInteger();

			if (evt.node->GetNodeName() == KEY_MOVEMENT_OFFSET_NAME)
			{
				MK_WIN_EVENT_MGR.__SetKeyMovementOffset(value);
			}
			else if (evt.node->GetNodeName() == KEY_MOVEMENT_FACTOR_NAME)
			{
				MK_WIN_EVENT_MGR.__SetKeyMovementFactor(value);
			}
		}
	}
}

MkEditModeSettingWindow::MkEditModeSettingWindow(const MkHashStr& name) : MkBaseSystemWindow(name)
{
	
}

//------------------------------------------------------------------------------------------------//
