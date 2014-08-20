
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

const static MkHashStr FLIP_H_BTN_NAME = L"FlipH";
const static MkHashStr FLIP_V_BTN_NAME = L"FlipV";
const static MkHashStr ALPHA_ROOT_BTN_NAME = L"Alpha";

const static MkHashStr SRC_TYPE_ROOT_BTN_NAME = L"ST_Root";
const static MkHashStr SRC_TYPE_IMG_BTN_NAME = L"ST_Img";
const static MkHashStr SRC_TYPE_CSTR_BTN_NAME = L"ST_CStr";
const static MkHashStr SRC_TYPE_SSTR_BTN_NAME = L"ST_SStr";

const static MkHashStr IMAGE_LOAD_BTN_NAME = L"Img_Load";
const static MkHashStr IMAGE_SUBSET_EB_NAME = L"Img_Subset";

const static MkHashStr CSTR_EB_NAME = L"CStr";
const static MkHashStr SSTR_EB_NAME = L"SStr";

const static MkHashStr SAMPLE_RECT_NAME = L"SampleRect";


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

	float posX = margin;
	float posY = clientSize.y - margin - 20.f;

	m_FlipHorizontalBtn = new MkCheckButtonNode(FLIP_H_BTN_NAME);
	m_FlipHorizontalBtn->CreateCheckButton(themeName, CaptionDesc(L"수평 반전"), false);
	m_FlipHorizontalBtn->SetLocalPosition(MkVec3(posX, posY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
	bgNode->AttachChildNode(m_FlipHorizontalBtn);

	posX += 120.f;
	m_FlipVerticalBtn = new MkCheckButtonNode(FLIP_V_BTN_NAME);
	m_FlipVerticalBtn->CreateCheckButton(themeName, CaptionDesc(L"수직 반전"), false);
	m_FlipVerticalBtn->SetLocalPosition(MkVec3(posX, posY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
	bgNode->AttachChildNode(m_FlipVerticalBtn);

	posX += 120.f;
	MkSRect* alphaTag = bgNode->CreateSRect(L"AlphaTag");
	alphaTag->SetDecoString(L"알파 :");
	alphaTag->SetLocalPosition(MkFloat2(posX, posY + 3));
	alphaTag->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);

	posX += alphaTag->GetLocalRect().size.x + 6.f;
	m_AlphaBtn = new MkSpreadButtonNode(ALPHA_ROOT_BTN_NAME);
	m_AlphaBtn->CreateSelectionRootTypeButton(themeName, MkFloat2(80.f, 20.f), MkSpreadButtonNode::eDownward);
	bgNode->AttachChildNode(m_AlphaBtn);
	m_AlphaBtn->SetLocalPosition(MkVec3(posX, posY, -MKDEF_BASE_WINDOW_DEPTH_GRID));

	ItemTagInfo ti;
	for (unsigned int i=0; i<=100; i+=10)
	{
		MkStr key(i); // 0, 10, ... , 90, 100
		ti.captionDesc.SetString(key + L" %");
		m_AlphaBtn->AddItem(key, ti);
	}

	posX = margin;
	posY -= 20.f + margin;
	m_SrcTypeBtn = new MkSpreadButtonNode(SRC_TYPE_ROOT_BTN_NAME);
	m_SrcTypeBtn->CreateSelectionRootTypeButton(themeName, MkFloat2(140.f, 20.f), MkSpreadButtonNode::eDownward);
	bgNode->AttachChildNode(m_SrcTypeBtn);
	m_SrcTypeBtn->SetLocalPosition(MkVec3(posX, posY, -MKDEF_BASE_WINDOW_DEPTH_GRID));

	ti.captionDesc.SetString(L"이미지");
	m_SrcTypeBtn->AddItem(SRC_TYPE_IMG_BTN_NAME, ti);
	ti.captionDesc.SetString(L"전용 문자열");
	m_SrcTypeBtn->AddItem(SRC_TYPE_CSTR_BTN_NAME, ti);
	ti.captionDesc.SetString(L"선언 문자열");
	m_SrcTypeBtn->AddItem(SRC_TYPE_SSTR_BTN_NAME, ti);

	posX += m_SrcTypeBtn->GetPresetComponentSize().x + margin;

	m_ImageLoadBtn = __CreateWindowPreset(bgNode, IMAGE_LOAD_BTN_NAME, themeName, eS2D_WPC_NormalButton, MkFloat2(140.f, 20.f));
	m_ImageLoadBtn->SetPresetComponentCaption(themeName, CaptionDesc(L"이미지 파일 지정"));
	m_ImageLoadBtn->SetLocalPosition(MkVec3(posX, posY, -MKDEF_BASE_WINDOW_DEPTH_GRID));

	float bPos = posX + m_ImageLoadBtn->GetPresetComponentSize().x + margin;
	m_ImageSubsetEB = new MkEditBoxNode(IMAGE_SUBSET_EB_NAME);
	m_ImageSubsetEB->CreateEditBox(themeName, MkFloat2(clientSize.x - bPos - margin, 20.f),
		MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkStr::Null, false);
	m_ImageSubsetEB->SetLocalPosition(MkVec3(bPos, posY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
	bgNode->AttachChildNode(m_ImageSubsetEB);

	m_CustomStringEB = new MkEditBoxNode(CSTR_EB_NAME);
	m_CustomStringEB->CreateEditBox(themeName, MkFloat2(clientSize.x - posX - margin, 20.f),
		MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkStr::Null, false);
	m_CustomStringEB->SetLocalPosition(MkVec3(posX, posY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
	bgNode->AttachChildNode(m_CustomStringEB);

	m_SceneStringEB = new MkEditBoxNode(SSTR_EB_NAME);
	m_SceneStringEB->CreateEditBox(themeName, MkFloat2(clientSize.x - posX - margin, 20.f),
		MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkStr::Null, false);
	m_SceneStringEB->SetLocalPosition(MkVec3(posX, posY, -MKDEF_BASE_WINDOW_DEPTH_GRID));
	bgNode->AttachChildNode(m_SceneStringEB);

	m_RectBGSize.x = clientSize.x;
	m_RectBGSize.y = posY - 20.f - margin;

	return true;
}

void MkSRectSetterSystemWindow::SetUp(MkSRectInfoListener* owner, MkSceneNode* targetNode, const MkHashStr& rectName, eInputType inputType)
{
	if ((owner != NULL) && (targetNode != NULL) && (!rectName.Empty()))
	{
		m_Owner = owner;
		m_TargetNode = targetNode;
		m_RectName = rectName;
		m_InputType = inputType;

		m_SrcType = MkSRect::eStaticImage;

		m_SampleRect = GetChildNode(L"Background")->CreateSRect(SAMPLE_RECT_NAME);

		bool fh = false, fv = false;
		unsigned int alpha = 100;
		if (m_TargetNode->ExistSRect(m_RectName))
		{
			MkSRect* targetRect = m_TargetNode->GetSRect(m_RectName);
			m_SrcType = targetRect->__GetSrcInfo(m_ImagePath, m_SubsetName, m_DecoStr, m_NodeNameAndKey);

			fh = targetRect->GetHorizontalReflection();
			fv = targetRect->GetVerticalReflection();
			
			float objAlpha = targetRect->GetObjectAlpha();
			unsigned int iAlpha = static_cast<unsigned int>(objAlpha * 100.f);
			alpha = iAlpha - (iAlpha % 10);

			switch (m_SrcType)
			{
			case MkSRect::eStaticImage:
				m_SampleRect->SetTexture(m_ImagePath, m_SubsetName);
				m_ImageSubsetEB->SetText(m_SubsetName);
				break;
			case MkSRect::eCustomDecoStr:
				m_SampleRect->SetDecoString(m_DecoStr);
				{
					MkStr tmpDS = m_DecoStr;
					tmpDS.ReplaceCRtoTag();
					m_CustomStringEB->SetText(tmpDS);
				}
				break;
			case MkSRect::eSceneDecoStr:
				m_SampleRect->SetDecoString(m_NodeNameAndKey);
				{
					MkStr buffer;
					_ConvertNodeNameAndKeyToStr(m_NodeNameAndKey, buffer);
					m_SceneStringEB->SetText(buffer);
				}
				break;
			}
			m_SampleRect->SetHorizontalReflection(fh);
			m_SampleRect->SetVerticalReflection(fv);
			m_SampleRect->SetObjectAlpha(objAlpha);
			_UpdateSamplePosition();
		}

		m_SampleRect->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);

		m_FlipHorizontalBtn->SetCheck(fh);
		if (m_InputType != eOneOfSelection)
		{
			m_FlipHorizontalBtn->SetEnable(false);
		}

		m_FlipVerticalBtn->SetCheck(fv);
		if (m_InputType != eOneOfSelection)
		{
			m_FlipVerticalBtn->SetEnable(false);
		}
		
		m_AlphaBtn->SetTargetItem(MkStr(alpha));
		if (m_InputType != eOneOfSelection)
		{
			m_AlphaBtn->SetEnable(false);
		}
		
		switch (m_InputType)
		{
		case eOneOfSelection:
			{
				switch (m_SrcType)
				{
				case MkSRect::eCustomDecoStr:
					m_SrcTypeBtn->SetTargetItem(SRC_TYPE_CSTR_BTN_NAME);
					break;
				case MkSRect::eSceneDecoStr:
					m_SrcTypeBtn->SetTargetItem(SRC_TYPE_SSTR_BTN_NAME);
					break;
				default:
					m_SrcTypeBtn->SetTargetItem(SRC_TYPE_IMG_BTN_NAME);
					break;
				}
			}
			break;

		case eImageOnly:
			m_SrcTypeBtn->SetTargetItem(SRC_TYPE_IMG_BTN_NAME);
			m_SrcTypeBtn->GetItem(SRC_TYPE_SSTR_BTN_NAME)->SetEnable(false);
			m_SrcTypeBtn->GetItem(SRC_TYPE_CSTR_BTN_NAME)->SetEnable(false);
			m_SrcType = MkSRect::eStaticImage;
			break;

		case eStringOnly:
			{
				m_SrcTypeBtn->GetItem(SRC_TYPE_IMG_BTN_NAME)->SetEnable(false);

				switch (m_SrcType)
				{
				case MkSRect::eSceneDecoStr:
					m_SrcTypeBtn->SetTargetItem(SRC_TYPE_SSTR_BTN_NAME);
					m_SrcType = MkSRect::eSceneDecoStr;
					break;
				default:
					m_SrcTypeBtn->SetTargetItem(SRC_TYPE_CSTR_BTN_NAME);
					m_SrcType = MkSRect::eCustomDecoStr;
					break;
				}
			}
			break;
		}

		_UpdataSubControl();

		MK_WIN_EVENT_MGR.ActivateWindow(GetNodeName(), true);
	}
}

void MkSRectSetterSystemWindow::UseWindowEvent(WindowEvent& evt)
{
	switch (evt.type)
	{
	case MkSceneNodeFamilyDefinition::eCheckOn:
	case MkSceneNodeFamilyDefinition::eCheckOff:
		{
			if (evt.node->GetNodeName() == FLIP_H_BTN_NAME)
			{
				m_SampleRect->SetHorizontalReflection(m_FlipHorizontalBtn->GetCheck());
			}
			else if (evt.node->GetNodeName() == FLIP_V_BTN_NAME)
			{
				m_SampleRect->SetVerticalReflection(m_FlipVerticalBtn->GetCheck());
			}
		}
		break;
		
	case MkSceneNodeFamilyDefinition::eSetTargetItem:
		{
			if (evt.node->GetNodeName() == SRC_TYPE_ROOT_BTN_NAME)
			{
				if (m_SrcTypeBtn->GetTargetItemKey() == SRC_TYPE_IMG_BTN_NAME)
				{
					m_SrcType = MkSRect::eStaticImage;
				}
				else if (m_SrcTypeBtn->GetTargetItemKey() == SRC_TYPE_CSTR_BTN_NAME)
				{
					m_SrcType = MkSRect::eCustomDecoStr;
				}
				else if (m_SrcTypeBtn->GetTargetItemKey() == SRC_TYPE_SSTR_BTN_NAME)
				{
					m_SrcType = MkSRect::eSceneDecoStr;
				}
				
				_UpdataSubControl();

				m_SampleRect->Clear();

				switch (m_SrcType)
				{
				case MkSRect::eStaticImage:
					{
						if (!m_ImagePath.Empty())
						{
							m_SampleRect->SetTexture(m_ImagePath, m_SubsetName);
						}
					}
					break;
				case MkSRect::eCustomDecoStr:
					{
						if (!m_DecoStr.Empty())
						{
							m_SampleRect->SetDecoString(m_DecoStr);
						}
					}
					break;
				case MkSRect::eSceneDecoStr:
					{
						if (!m_NodeNameAndKey.Empty())
						{
							m_SampleRect->SetDecoString(m_NodeNameAndKey);
						}
					}
					break;
				}

				_UpdateSamplePosition();
			}
			else if (evt.node->GetNodeName() == ALPHA_ROOT_BTN_NAME)
			{
				m_SampleRect->SetObjectAlpha(m_AlphaBtn->GetTargetItemKey().GetString().ToFloat() / 100.f);
			}
		}
		break;

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
						ok = !m_ImagePath.Empty();
						break;
					case MkSRect::eCustomDecoStr:
						ok = !m_DecoStr.Empty();
						break;
					case MkSRect::eSceneDecoStr:
						ok = !m_NodeNameAndKey.Empty();
						break;
					}

					if (ok)
					{
						m_Owner->SRectInfoUpdated
							(m_TargetNode, m_RectName, m_FlipHorizontalBtn->GetCheck(), m_FlipVerticalBtn->GetCheck(),
							m_AlphaBtn->GetTargetItemKey().GetString().ToFloat() / 100.f,
							m_SrcType, m_ImagePath, m_SubsetName, m_DecoStr, m_NodeNameAndKey);
					}

					MK_WIN_EVENT_MGR.DeactivateWindow(GetNodeName());
				}
			}
			else if (evt.node->GetPresetComponentType() == eS2D_WPC_CancelButton)
			{
				MK_WIN_EVENT_MGR.DeactivateWindow(GetNodeName());
			}
			else if (evt.node->GetNodeName() == IMAGE_LOAD_BTN_NAME)
			{
				MkPathName filePath;
				MkArray<MkStr> exts(2);
				exts.PushBack(MKDEF_S2D_IMAGE_FILE_EXT_00);
				exts.PushBack(MKDEF_S2D_IMAGE_FILE_EXT_01);
				if (filePath.GetSingleFilePathFromDialog(exts) && (!filePath.Empty()) && filePath.ConvertToRootBasisRelativePath())
				{
					if (filePath != m_ImagePath)
					{
						m_ImagePath = filePath;
						m_SubsetName.Clear();
						m_ImageSubsetEB->SetText(m_SubsetName);
						m_SampleRect->SetTexture(m_ImagePath, m_SubsetName);
						_UpdateSamplePosition();
					}
				}
			}
		}
		break;

	case MkSceneNodeFamilyDefinition::eCommitText:
		{
			if (evt.node->GetNodeName() == IMAGE_SUBSET_EB_NAME)
			{
				const MkStr& ss = m_ImageSubsetEB->GetText();
				if (ss.Empty())
				{
					m_SubsetName.Clear();
					m_SampleRect->SetSubset(m_SubsetName);
				}
				else
				{
					MkArray<MkHashStr> ssList;
					if ((m_SampleRect->GetSubsetList(ssList) > 0) && ssList.Exist(MkArraySection(0), ss))
					{
						m_SubsetName = ss;
						m_SampleRect->SetSubset(m_SubsetName);
					}
				}

				_UpdateSamplePosition();
			}
			else if (evt.node->GetNodeName() == CSTR_EB_NAME)
			{
				const MkStr& cstr = m_CustomStringEB->GetText();
				if (cstr.Empty())
				{
					m_DecoStr.Clear();
					m_SampleRect->Clear();
				}
				else
				{
					m_DecoStr = cstr;
					m_DecoStr.ReplaceTagtoCR();
					m_SampleRect->SetDecoString(m_DecoStr);
				}

				_UpdateSamplePosition();
			}
			else if (evt.node->GetNodeName() == SSTR_EB_NAME)
			{
				const MkStr& sstr = m_SceneStringEB->GetText();
				if (sstr.Empty())
				{
					m_NodeNameAndKey.Clear();
					m_SampleRect->Clear();
				}
				else
				{
					MkArray<MkHashStr> nodeNameAndKey;
					_ConvertStrToNodeNameAndKey(sstr, nodeNameAndKey);

					const MkDecoStr& decoStr = MK_WR_DECO_TEXT.GetDecoText(nodeNameAndKey);
					if (!decoStr.Empty())
					{
						m_NodeNameAndKey = nodeNameAndKey;
						m_SampleRect->SetDecoString(m_NodeNameAndKey);
					}
				}

				_UpdateSamplePosition();
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

	if (m_InputType != eOneOfSelection)
	{
		if (m_FlipHorizontalBtn != NULL)
		{
			m_FlipHorizontalBtn->SetEnable(true);
		}
		if (m_FlipVerticalBtn != NULL)
		{
			m_FlipVerticalBtn->SetEnable(true);
		}
		if (m_AlphaBtn != NULL)
		{
			m_AlphaBtn->SetEnable(true);
		}
	}

	switch (m_InputType)
	{
	case eImageOnly:
		m_SrcTypeBtn->GetItem(SRC_TYPE_SSTR_BTN_NAME)->SetEnable(true);
		m_SrcTypeBtn->GetItem(SRC_TYPE_CSTR_BTN_NAME)->SetEnable(true);
		break;
	case eStringOnly:
		m_SrcTypeBtn->GetItem(SRC_TYPE_IMG_BTN_NAME)->SetEnable(true);
		break;
	}

	m_ImagePath.Clear();
	m_SubsetName.Clear();
	m_DecoStr.Clear();
	m_NodeNameAndKey.Clear();
	m_SrcType = MkSRect::eNone;

	m_ImageSubsetEB->SetText(MkStr::Null, false);
	m_CustomStringEB->SetText(MkStr::Null, false);
	m_SceneStringEB->SetText(MkStr::Null, false);

	if (m_SampleRect != NULL)
	{
		GetChildNode(L"Background")->DeleteSRect(SAMPLE_RECT_NAME);
		m_SampleRect = NULL;
	}
	
	MkBaseSystemWindow::Deactivate();
}

MkSRectSetterSystemWindow::MkSRectSetterSystemWindow(const MkHashStr& name) : MkBaseSystemWindow(name)
{
	m_Owner = NULL;
	m_TargetNode = NULL;
	m_SrcType = MkSRect::eNone;
	m_InputType = eOneOfSelection;

	m_SampleRect = NULL;
	m_FlipHorizontalBtn = NULL;
	m_FlipVerticalBtn = NULL;
	m_AlphaBtn = NULL;
	m_SrcTypeBtn = NULL;
	m_ImageLoadBtn = NULL;
	m_ImageSubsetEB = NULL;
	m_CustomStringEB = NULL;
	m_SceneStringEB = NULL;
}

void MkSRectSetterSystemWindow::_UpdataSubControl(void)
{
	switch (m_SrcType)
	{
	case MkSRect::eStaticImage:
		m_ImageLoadBtn->SetVisible(true);
		m_ImageSubsetEB->SetVisible(true);
		m_CustomStringEB->SetVisible(false);
		m_SceneStringEB->SetVisible(false);
		break;
	case MkSRect::eCustomDecoStr:
		m_ImageLoadBtn->SetVisible(false);
		m_ImageSubsetEB->SetVisible(false);
		m_CustomStringEB->SetVisible(true);
		m_SceneStringEB->SetVisible(false);
		break;
	case MkSRect::eSceneDecoStr:
		m_ImageLoadBtn->SetVisible(false);
		m_ImageSubsetEB->SetVisible(false);
		m_CustomStringEB->SetVisible(false);
		m_SceneStringEB->SetVisible(true);
		break;
	}
}

void MkSRectSetterSystemWindow::_UpdateSamplePosition(void)
{
	const MkFloat2& ls = m_SampleRect->GetLocalSize();
	if ((ls.x > m_RectBGSize.x) || (ls.y > m_RectBGSize.y))
	{
		float rate = GetMin<float>(m_RectBGSize.x / ls.x, m_RectBGSize.y / ls.y);
		m_SampleRect->SetLocalSize(MkFloat2(ls.x * rate, ls.y * rate));
	}

	m_SampleRect->AlignRect(m_RectBGSize, eRAP_MiddleCenter, MkFloat2(0.f, 0.f), 0.f);
}

void MkSRectSetterSystemWindow::_ConvertNodeNameAndKeyToStr(const MkArray<MkHashStr>& nodeNameAndKey, MkStr& msg)
{
	if (!nodeNameAndKey.Empty())
	{
		const MkStr token = L"::";

		unsigned int totalSize = 0;
		MK_INDEXING_LOOP(nodeNameAndKey, i)
		{
			totalSize += nodeNameAndKey.GetSize();
			totalSize += (i == 0) ? 0 : token.GetSize();
		}
		msg.Reserve(totalSize);

		MK_INDEXING_LOOP(nodeNameAndKey, i)
		{
			if (i > 0)
			{
				msg += token;
			}

			msg += nodeNameAndKey[i].GetString();
		}
	}
}

void MkSRectSetterSystemWindow::_ConvertStrToNodeNameAndKey(const MkStr& msg, MkArray<MkHashStr>& nodeNameAndKey)
{
	if (!msg.Empty())
	{
		const MkStr token = L"::";

		MkArray<MkStr> buffer;
		if (msg.Tokenize(buffer, token) > 0)
		{
			nodeNameAndKey.Reserve(buffer.GetSize());
			MK_INDEXING_LOOP(buffer, i)
			{
				nodeNameAndKey.PushBack(buffer[i]);
			}
		}
	}
}

//------------------------------------------------------------------------------------------------//