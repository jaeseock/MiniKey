
#include "MkS2D_MkCheckButtonNode.h"

#include "MkS2D_MkWindowEventManager.h"
#include "MkS2D_MkEditModeSettingWindow.h"


const static MkHashStr SHOW_TARGET_WIN_REGION_NAME = L"ShowTargetWinReg";

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

	MkFloatRect clientRect = bgNode->GetPresetFullSize();
	clientRect.size.y -= GetPresetFullSize().y; // body size - title size

	MkCheckButtonNode* cbNode = new MkCheckButtonNode(SHOW_TARGET_WIN_REGION_NAME);
	MkBaseWindowNode::CaptionDesc captionDesc;
	captionDesc = L"타겟 윈도우 영역 표시";
	cbNode->CreateCheckButton(L"Default", captionDesc, MK_WIN_EVENT_MGR.__GetShowWindowSelection());
	cbNode->SetLocalPosition(clientRect.GetSnapPosition(MkFloatRect(cbNode->GetPresetFullSize()), eRAP_LeftTop, MkFloat2(20.f, 20.f)));
	cbNode->SetLocalDepth(-0.001f);
	bgNode->AttachChildNode(cbNode);

	return true;
}

void MkEditModeSettingWindow::Activate(void)
{
	AlignPosition(MK_WIN_EVENT_MGR.GetRegionRect(), eRAP_MiddleCenter, MkInt2(0, 0)); // 중앙 정렬
}

void MkEditModeSettingWindow::UpdateManagedRoot(void)
{
	MK_INDEXING_LOOP(m_WindowEvents, i)
	{
		WindowEvent& evt = m_WindowEvents[i];
		const MkHashStr& targetName = evt.node->GetNodeName();

		switch (evt.type)
		{
		case MkSceneNodeFamilyDefinition::eCheckOn:
			{
				if (targetName == SHOW_TARGET_WIN_REGION_NAME)
				{
					MK_WIN_EVENT_MGR.__SetShowWindowSelection(true);
				}
			}
			break;

		case MkSceneNodeFamilyDefinition::eCheckOff:
			{
				if (targetName == SHOW_TARGET_WIN_REGION_NAME)
				{
					MK_WIN_EVENT_MGR.__SetShowWindowSelection(false);
				}
			}
			break;
		}
	}

	MkBaseWindowNode::UpdateManagedRoot();
}

MkEditModeSettingWindow::MkEditModeSettingWindow(const MkHashStr& name) : MkBaseWindowNode(name)
{
	
}

//------------------------------------------------------------------------------------------------//
