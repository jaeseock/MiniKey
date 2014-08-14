
#include "MkS2D_MkCheckButtonNode.h"
#include "MkS2D_MkEditBoxNode.h"
#include "MkS2D_MkSpreadButtonNode.h"

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
	L"입력 무시", L"이동 금지", L"시스템 윈도우", L"드래그 이동 허용",
	L"이동시 부모의 영역으로 제한", L"이동시 스크린 영역으로 제한", L"아이템 드래그 허용", L"아이템 드롭 허용",
	L"Active시 중앙 정렬", L"커서 위치시 ActionCursor 지정" };

bool MkWindowAttributeSystemWindow::Initialize(void)
{
	SetAttribute(eAlignCenterPos, true);

	const MkHashStr& themeName = MK_WR_PRESET.GetDefaultThemeName();

	const float margin = 10.f;
	MkFloat2 unitSize(300.f, 30.f);

	BasicPresetWindowDesc winDesc;
	winDesc.SetStandardDesc(themeName, true, MkFloat2(unitSize.x * 2.f + margin * 2.f, unitSize.y * 10.f + margin * 2.f + 30.f));
	winDesc.titleType = eS2D_WPC_SystemMsgTitle;
	winDesc.titleCaption = L"윈도우 속성 설정";
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

const static MkHashStr SRC_TYPE_ROOT_BTN_NAME = L"ST_Root";
const static MkHashStr SRC_TYPE_IMG_BTN_NAME = L"ST_Img";
const static MkHashStr SRC_TYPE_CSTR_BTN_NAME = L"ST_CStr";
const static MkHashStr SRC_TYPE_SSTR_BTN_NAME = L"ST_SStr";

bool MkSRectSetterSystemWindow::Initialize(void)
{
	SetAttribute(eAlignCenterPos, true);

	const MkHashStr& themeName = MK_WR_PRESET.GetDefaultThemeName();

	const float margin = 10.f;
	MkFloat2 winSize(780.f, 580.f);

	BasicPresetWindowDesc winDesc;
	winDesc.SetStandardDesc(themeName, true, winSize);
	winDesc.titleType = eS2D_WPC_SystemMsgTitle;
	winDesc.titleCaption = L"SRect 설정";
	winDesc.hasIcon = false;
	winDesc.hasCloseButton = true;
	winDesc.hasCancelButton = true;
	winDesc.hasOKButton = true;

	if (CreateBasicWindow(this, MkHashStr::NullHash, winDesc) == NULL)
		return false;

	MkBaseWindowNode* bgNode = dynamic_cast<MkBaseWindowNode*>(GetChildNode(L"Background"));
	if (bgNode == NULL)
		return false;

	MkFloat2 clientSize = bgNode->GetPresetComponentSize();
	clientSize.y -= GetPresetComponentSize().y; // body size - title size

	MkBaseWindowNode* okButton = dynamic_cast<MkBaseWindowNode*>(bgNode->GetChildNode(L"OKButton"));
	MkBaseWindowNode* cancelButton = dynamic_cast<MkBaseWindowNode*>(bgNode->GetChildNode(L"CancelButton"));
	okButton->SetLocalPosition(MkFloat2(clientSize.x - okButton->GetPresetComponentSize().x - cancelButton->GetPresetComponentSize().x - margin * 2.f, margin));
	cancelButton->SetLocalPosition(MkFloat2(clientSize.x - cancelButton->GetPresetComponentSize().x - margin, margin));

	MkSpreadButtonNode* srcTypeBtn = new MkSpreadButtonNode(SRC_TYPE_ROOT_BTN_NAME);
	srcTypeBtn->CreateSelectionRootTypeButton(themeName, MkFloat2(100.f, 20.f), MkSpreadButtonNode::eDownward);
	bgNode->AttachChildNode(srcTypeBtn);

	float posX = margin;
	const float posY = clientSize.y - margin - srcTypeBtn->GetPresetComponentSize().y;
	srcTypeBtn->SetLocalPosition(MkVec3(posX, posY, -MKDEF_BASE_WINDOW_DEPTH_GRID));

	ItemTagInfo ti;
	ti.captionDesc.SetString(L"이미지");
	srcTypeBtn->AddItem(SRC_TYPE_IMG_BTN_NAME, ti);
	ti.captionDesc.SetString(L"전용 문자열");
	srcTypeBtn->AddItem(SRC_TYPE_CSTR_BTN_NAME, ti);
	ti.captionDesc.SetString(L"선언 문자열");
	srcTypeBtn->AddItem(SRC_TYPE_SSTR_BTN_NAME, ti);

	return true;
}

void MkSRectSetterSystemWindow::SetUp(MkSRectInfoListener* owner, MkSceneNode* targetNode, const MkHashStr& rectName)
{
	if ((owner != NULL) && (targetNode != NULL) && (!rectName.Empty()))
	{
		m_Owner = owner;
		m_TargetNode = targetNode;
		m_RectName = rectName;

		if (m_TargetNode->ExistSRect(m_RectName))
		{
			m_SrcType = m_TargetNode->GetSRect(m_RectName)->__GetSrcInfo(m_ImagePath, m_SubsetName, m_DecoStr, m_NodeNameAndKey);
		}

		MK_WIN_EVENT_MGR.ActivateWindow(GetNodeName(), true);
	}
}

void MkSRectSetterSystemWindow::UseWindowEvent(WindowEvent& evt)
{
	switch (evt.type)
	{
		/*
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
		*/

	case MkSceneNodeFamilyDefinition::eCursorLeftRelease:
		{
			if (evt.node->GetPresetComponentType() == eS2D_WPC_OKButton)
			{
				if (m_Owner != NULL)
				{
					bool ok = false;
					switch (m_SrcType)
					{
					case MkSRect::eStaticImage:
						m_DecoStr.Clear();
						m_NodeNameAndKey.Clear();
						ok = !m_ImagePath.Empty();
						break;
					case MkSRect::eCustomDecoStr:
						m_ImagePath.Clear();
						m_SubsetName.Clear();
						m_NodeNameAndKey.Clear();
						ok = !m_DecoStr.Empty();
						break;
					case MkSRect::eSceneDecoStr:
						m_ImagePath.Clear();
						m_SubsetName.Clear();
						m_DecoStr.Clear();
						ok = !m_NodeNameAndKey.Empty();
						break;
					}

					if (ok)
					{
						m_Owner->SRectInfoUpdated(m_TargetNode, m_RectName, m_SrcType, m_ImagePath, m_SubsetName, m_DecoStr, m_NodeNameAndKey);
					}

					MK_WIN_EVENT_MGR.DeactivateWindow(GetNodeName());
				}
			}
			else if (evt.node->GetPresetComponentType() == eS2D_WPC_CancelButton)
			{
				MK_WIN_EVENT_MGR.DeactivateWindow(GetNodeName());
			}
		}
		break;
	}
}

void MkSRectSetterSystemWindow::Deactivate(void)
{
	m_Owner = NULL;
	m_TargetNode = NULL;
	m_RectName.Clear();

	m_ImagePath.Clear();
	m_SubsetName.Clear();
	m_DecoStr.Clear();
	m_NodeNameAndKey.Clear();
	m_SrcType = MkSRect::eNone;

	MkBaseSystemWindow::Deactivate();
}

MkSRectSetterSystemWindow::MkSRectSetterSystemWindow(const MkHashStr& name) : MkBaseSystemWindow(name)
{
	m_Owner = NULL;
	m_TargetNode = NULL;
	m_SrcType = MkSRect::eNone;
}

//------------------------------------------------------------------------------------------------//