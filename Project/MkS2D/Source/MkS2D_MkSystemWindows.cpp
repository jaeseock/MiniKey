
//#include "MkS2D_MkCheckButtonNode.h"
#include "MkS2D_MkEditBoxNode.h"

#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkWindowEventManager.h"
#include "MkS2D_MkSystemWindows.h"


//const static MkHashStr SHOW_TARGET_WIN_REGION_NAME = L"ShowTargetWinReg";

//------------------------------------------------------------------------------------------------//

MkBaseSystemWindow::MkBaseSystemWindow(const MkHashStr& name) : MkBaseWindowNode(name)
{
	SetAttribute(eSystemWindow, true);
}

//------------------------------------------------------------------------------------------------//

bool MkNodeNameInputSystemWindow::Initialize(void)
{
	if (m_EditBox != NULL)
		return true;

	SetAttribute(eAlignCenterPos, true);

	const MkHashStr& themeName = MK_WR_PRESET.GetDefaultThemeName();

	const float MARGIN = 10.f;
	const float EB_HEIGHT = 20.f;

	BasicPresetWindowDesc winDesc;
	winDesc.SetStandardDesc(themeName, true, MkFloat2(512.f, MARGIN * 3.f + EB_HEIGHT + 20.f + 20.f));
	winDesc.titleType = eS2D_WPC_SystemMsgTitle;
	winDesc.titleCaption = L"노드 이름 입력";
	winDesc.hasIcon = false;

	if (CreateBasicWindow(this, MkHashStr::NullHash, winDesc) == NULL)
		return false;

	MkBaseWindowNode* bgNode = dynamic_cast<MkBaseWindowNode*>(GetChildNode(L"Background"));
	if (bgNode == NULL)
		return false;

	MkFloat2 clientSize = bgNode->GetPresetComponentSize();
	clientSize.y -= GetPresetComponentSize().y; // body size - title size

	// edit box
	m_EditBox = new MkEditBoxNode(L"EditBox");
	m_EditBox->CreateEditBox(themeName, MkFloat2(clientSize.x - MARGIN * 2.f, EB_HEIGHT),
		MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkStr::Null, false);

	m_EditBox->SetLocalPosition(MkVec3(MARGIN, clientSize.y - MARGIN - EB_HEIGHT, -MKDEF_BASE_WINDOW_DEPTH_GRID));
	bgNode->AttachChildNode(m_EditBox);

	// ok button
	m_OkButton = dynamic_cast<MkBaseWindowNode*>(bgNode->GetChildNode(L"OKButton"));

	return true;
}

void MkNodeNameInputSystemWindow::SetUp(MkSceneNode* targetNode)
{
	if ((m_EditBox != NULL) && (targetNode != NULL))
	{
		m_TargetNode = targetNode;
		
		m_OriginalName = m_TargetNode->GetNodeName();
		m_EditBox->SetText(m_OriginalName.GetString());
		m_OkButton->SetEnable(true);

		MK_WIN_EVENT_MGR.ActivateWindow(GetNodeName(), true);
	}
}

void MkNodeNameInputSystemWindow::UseWindowEvent(WindowEvent& evt)
{
	switch (evt.type)
	{
	case MkSceneNodeFamilyDefinition::eCursorLeftRelease:
		{
			if (evt.node->GetPresetComponentType() == eS2D_WPC_OKButton)
			{
				MkHashStr newName = m_EditBox->GetText();
				if ((m_TargetNode != NULL) && (!newName.Empty()) && (newName != m_OriginalName))
				{
					m_TargetNode->ChangeNodeName(newName);
				}
				MK_WIN_EVENT_MGR.DeactivateWindow(GetNodeName());
			}
			else if (evt.node->GetPresetComponentType() == eS2D_WPC_CancelButton)
			{
				MK_WIN_EVENT_MGR.DeactivateWindow(GetNodeName());
			}
		}
		break;

	case MkSceneNodeFamilyDefinition::eModifyText:
		{
			if (m_TargetNode != NULL)
			{
				MkSceneNode* parentNode = m_TargetNode->GetParentNode();
				if (parentNode == NULL)
				{
					m_OkButton->SetEnable(true);
				}
				else
				{
					MkHashStr newName = m_EditBox->GetText();
					m_OkButton->SetEnable((newName == m_OriginalName) || (!parentNode->ChildExist(newName)));
				}
			}
		}
		break;

	case MkSceneNodeFamilyDefinition::eCommitText:
		{
			MkHashStr newName = m_EditBox->GetText();
			if ((m_TargetNode != NULL) && m_OkButton->GetEnable() && (!newName.Empty()))
			{
				if (newName != m_OriginalName)
				{
					m_TargetNode->ChangeNodeName(newName);
				}
				MK_WIN_EVENT_MGR.DeactivateWindow(GetNodeName());
			}
		}
		break;
	}
}

MkNodeNameInputSystemWindow::MkNodeNameInputSystemWindow(const MkHashStr& name) : MkBaseSystemWindow(name)
{
	m_EditBox = NULL;
	m_OkButton = NULL;
	m_TargetNode = NULL;
}

//------------------------------------------------------------------------------------------------//
