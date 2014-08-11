
#include "MkS2D_MkCheckButtonNode.h"
#include "MkS2D_MkEditBoxNode.h"

#include "MkS2D_MkHiddenEditBox.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkWindowEventManager.h"
#include "MkS2D_MkSystemWindows.h"


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
	winDesc.titleCaption = L"��� �̸� �Է�";
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
	m_OkButton->SetLocalPosition(MkFloat2(m_OkButton->GetLocalPosition().x, MARGIN));

	// cancel button
	MkBaseWindowNode* cancelBtn = dynamic_cast<MkBaseWindowNode*>(bgNode->GetChildNode(L"CancelButton"));
	cancelBtn->SetLocalPosition(MkFloat2(cancelBtn->GetLocalPosition().x, MARGIN));

	return true;
}

void MkNodeNameInputSystemWindow::SetUp(MkSceneNode* targetNode, MkNodeNameInputListener* owner)
{
	if ((m_EditBox != NULL) && (targetNode != NULL))
	{
		m_TargetNode = targetNode;
		m_Owner = owner;
		
		m_OriginalName = m_TargetNode->GetNodeName();
		m_EditBox->SetText(m_OriginalName.GetString());
		m_OkButton->SetEnable(true);

		MK_WIN_EVENT_MGR.ActivateWindow(GetNodeName(), true);
	}
}

void MkNodeNameInputSystemWindow::Activate(void)
{
	MkBaseSystemWindow::Activate();

	if (m_EditBox != NULL)
	{
		MK_EDIT_BOX.BindControl(m_EditBox);
	}
}

void MkNodeNameInputSystemWindow::Deactivate(void)
{
	m_TargetNode = NULL;
	m_OriginalName.Clear();
	m_Owner = NULL;
}

void MkNodeNameInputSystemWindow::UseWindowEvent(WindowEvent& evt)
{
	switch (evt.type)
	{
	case MkSceneNodeFamilyDefinition::eCursorLeftRelease:
		{
			if (evt.node->GetPresetComponentType() == eS2D_WPC_OKButton)
			{
				if (m_TargetNode != NULL)
				{
					_ApplyNodeName(m_EditBox->GetText());
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
			if ((m_TargetNode != NULL) && m_OkButton->GetEnable())
			{
				_ApplyNodeName(m_EditBox->GetText());

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
	Deactivate();
}

void MkNodeNameInputSystemWindow::_ApplyNodeName(const MkHashStr& newName)
{
	if (newName != m_OriginalName)
	{
		m_TargetNode->ChangeNodeName(newName);

		if (m_Owner != NULL)
		{
			m_Owner->NodeNameChanged(m_OriginalName, newName, m_TargetNode);
		}
	}
}

//------------------------------------------------------------------------------------------------//

static const MkHashStr sWinAttrDesc[MkBaseWindowNode::eMaxAttribute] = {
	L"�Է� ����", L"�̵� ����", L"�ý��� ������", L"�巡�� �̵� ���",
	L"�̵��� �θ��� �������� ����", L"�̵��� ��ũ�� �������� ����", L"������ �巡�� ���", L"������ ��� ���",
	L"Active�� �߾� ����", L"Ŀ�� ��ġ�� ActionCursor ����" };

bool MkWindowAttributeSystemWindow::Initialize(void)
{
	SetAttribute(eAlignCenterPos, true);

	const MkHashStr& themeName = MK_WR_PRESET.GetDefaultThemeName();

	const float margin = 10.f;
	MkFloat2 unitSize(300.f, 30.f);

	BasicPresetWindowDesc winDesc;
	winDesc.SetStandardDesc(themeName, true, MkFloat2(unitSize.x * 2.f + margin * 2.f, unitSize.y * 10.f + margin * 2.f + 30.f));
	winDesc.titleType = eS2D_WPC_SystemMsgTitle;
	winDesc.titleCaption = L"������ �Ӽ� ����";
	winDesc.hasIcon = false;
	winDesc.hasCloseButton = true;
	winDesc.hasCancelButton = false;
	winDesc.hasOKButton = true;

	if (CreateBasicWindow(this, MkHashStr::NullHash, winDesc) == NULL)
		return false;

	MkBaseWindowNode* bgNode = dynamic_cast<MkBaseWindowNode*>(GetChildNode(L"Background"));
	if (bgNode == NULL)
		return false;

	MkFloat2 clientSize = bgNode->GetPresetComponentSize();
	clientSize.y -= GetPresetComponentSize().y; // body size - title size

	MkBaseWindowNode* okButton = dynamic_cast<MkBaseWindowNode*>(bgNode->GetChildNode(L"OKButton"));
	okButton->SetLocalPosition(MkFloat2(okButton->GetLocalPosition().x, margin));

	m_CheckButtons.Reserve(eMaxAttribute);

	for (unsigned int i=0; i<eMaxAttribute; ++i)
	{
		MkCheckButtonNode* cbNode = new MkCheckButtonNode(sWinAttrDesc[i]);
		if (cbNode != NULL)
		{
			cbNode->CreateCheckButton(themeName, CaptionDesc(sWinAttrDesc[i]), false);
			MkFloat2 position(margin + (((i % 2) == 0) ? 0.f : unitSize.x), clientSize.y - static_cast<float>(i / 2 + 1) * unitSize.y);
			cbNode->SetLocalPosition(position);
			cbNode->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
			bgNode->AttachChildNode(cbNode);
		}
		m_CheckButtons.PushBack(cbNode);
	}

	return true;
}

void MkWindowAttributeSystemWindow::SetUp(MkBaseWindowNode* targetWindow)
{
	if (targetWindow != NULL)
	{
		m_TargetWindow = targetWindow;

		for (unsigned int i=0; i<eMaxAttribute; ++i)
		{
			if (m_CheckButtons.IsValidIndex(i))
			{
				bool enable = m_TargetWindow->GetAttribute(static_cast<eAttribute>(i));
				m_CheckButtons[i]->SetCheck(enable);
			}
		}

		MK_WIN_EVENT_MGR.ActivateWindow(GetNodeName(), true);
	}
}

void MkWindowAttributeSystemWindow::UseWindowEvent(WindowEvent& evt)
{
	switch (evt.type)
	{
	case MkSceneNodeFamilyDefinition::eCheckOn:
		{
			if (m_TargetWindow != NULL)
			{
				for (unsigned int i=0; i<eMaxAttribute; ++i)
				{
					if (evt.node->GetNodeName() == sWinAttrDesc[i])
					{
						m_TargetWindow->SetAttribute(static_cast<eAttribute>(i), true);
					}
				}
			}
		}
		break;

	case MkSceneNodeFamilyDefinition::eCheckOff:
		{
			if (m_TargetWindow != NULL)
			{
				for (unsigned int i=0; i<eMaxAttribute; ++i)
				{
					if (evt.node->GetNodeName() == sWinAttrDesc[i])
					{
						m_TargetWindow->SetAttribute(static_cast<eAttribute>(i), false);
					}
				}
			}
		}
		break;

	case MkSceneNodeFamilyDefinition::eCursorLeftRelease:
		{
			if (evt.node->GetPresetComponentType() == eS2D_WPC_OKButton)
			{
				MK_WIN_EVENT_MGR.DeactivateWindow(GetNodeName());
			}
		}
		break;
	}
}

MkWindowAttributeSystemWindow::MkWindowAttributeSystemWindow(const MkHashStr& name) : MkBaseSystemWindow(name)
{
	m_TargetWindow = NULL;
}

//------------------------------------------------------------------------------------------------//