
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkWindowResourceManager.h"
//#include "MkS2D_MkHiddenEditBox.h"
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkEditBoxNode.h"
#include "MkS2D_MkSceneNodeFamilyDefinition.h"


#define MKDEF_MAX_HISTORY_RECORD 100
#define MKDEF_TEXT_START_POSITION (MK_WR_PRESET.GetMargin())
#define MKDEF_CURSOR_CHAR L"│"

const static MkHashStr TEXT_SRECT_NAME = L"Text";
const static MkHashStr CURSOR_SRECT_NAME = L"Cursor";

//------------------------------------------------------------------------------------------------//

bool MkEditBoxNode::CreateEditBox
(const MkHashStr& themeName, const MkFloat2& windowSize,
 const MkHashStr& fontType, const MkHashStr& normalFontState, const MkHashStr& selectionFontState, const MkHashStr& cursorFontState,
 const MkStr& initMsg, bool useHistory)
{
	bool ok = CreateWindowPreset(themeName, eS2D_WPC_RootButton, windowSize);
	if (ok)
	{
		m_UseHistory = useHistory;
		if (m_UseHistory)
		{
			m_MessageHistory.SetUp(MKDEF_MAX_HISTORY_RECORD);
		}

		_SetFontInfo(fontType, normalFontState, selectionFontState, cursorFontState);
		
		SetText(initMsg);
	}
	return ok;
}

void MkEditBoxNode::SetText(const MkStr& msg)
{
	DWORD selPos = static_cast<DWORD>(msg.GetSize());
	__UpdateTextInfo(msg, selPos, selPos);
}

void MkEditBoxNode::Load(const MkDataNode& node)
{
	MkStr text;
	node.GetData(MkSceneNodeFamilyDefinition::EditBox::TextKey, text, 0);

	m_UseHistory = false;
	node.GetData(MkSceneNodeFamilyDefinition::EditBox::UseHistoryKey, m_UseHistory, 0);

	MkStr fontType;
	node.GetData(MkSceneNodeFamilyDefinition::EditBox::FontTypeKey, fontType, 0);

	MkStr normalFontState;
	node.GetData(MkSceneNodeFamilyDefinition::EditBox::NormalFontStateKey, normalFontState, 0);

	MkStr selectionFontState;
	node.GetData(MkSceneNodeFamilyDefinition::EditBox::SelectionFontStateKey, selectionFontState, 0);

	MkStr cursorFontState;
	node.GetData(MkSceneNodeFamilyDefinition::EditBox::CursorFontStateKey, cursorFontState, 0);

	_SetFontInfo(fontType, normalFontState, selectionFontState, cursorFontState);

	// MkBaseWindowNode
	MkBaseWindowNode::Load(node);

	SetText(text);
}

void MkEditBoxNode::Save(MkDataNode& node)
{
	// 기존 템플릿이 없으면 템플릿 적용
	_ApplyBuildingTemplateToSave(node, MkSceneNodeFamilyDefinition::EditBox::TemplateName);

	node.SetData(MkSceneNodeFamilyDefinition::EditBox::TextKey, m_Text, 0);
	node.SetData(MkSceneNodeFamilyDefinition::EditBox::UseHistoryKey, m_UseHistory, 0);
	node.SetData(MkSceneNodeFamilyDefinition::EditBox::FontTypeKey, m_FontType.GetString(), 0);
	node.SetData(MkSceneNodeFamilyDefinition::EditBox::NormalFontStateKey, m_NormalFontState.GetString(), 0);
	node.SetData(MkSceneNodeFamilyDefinition::EditBox::SelectionFontStateKey, m_SelectionFontState.GetString(), 0);
	node.SetData(MkSceneNodeFamilyDefinition::EditBox::CursorFontStateKey, m_CursorFontState.GetString(), 0);

	// MkBaseWindowNode
	MkBaseWindowNode::Save(node);

	// 하위 srect들(text, cursor 출력용)은 저장 필요성이 없으므로 삭제
	MkDataNode* srectNode = node.GetChildNode(MKDEF_S2D_SND_CHILD_SRECT_NODE_NAME);
	if (srectNode != NULL)
	{
		node.DetachChildNode(srectNode->GetNodeName());
		delete srectNode;
	}
}

MkEditBoxNode::MkEditBoxNode(const MkHashStr& name) : MkBaseWindowNode(name)
{
	m_UseHistory = false;

	m_SelStart = 0;
	m_SelEnd = 0;
}

//------------------------------------------------------------------------------------------------//

void MkEditBoxNode::__GenerateBuildingTemplate(void)
{
	MkDataNode node;
	MkDataNode* tNode = node.CreateChildNode(MkSceneNodeFamilyDefinition::EditBox::TemplateName);
	MK_CHECK(tNode != NULL, MkSceneNodeFamilyDefinition::EditBox::TemplateName.GetString() + L" template node alloc 실패")
		return;

	tNode->ApplyTemplate(MkSceneNodeFamilyDefinition::BaseWindow::TemplateName); // MkBaseWindowNode의 template 적용

	tNode->CreateUnit(MkSceneNodeFamilyDefinition::EditBox::TextKey, MkStr::Null);
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::EditBox::UseHistoryKey, false);
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::EditBox::FontTypeKey, MkStr::Null);
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::EditBox::NormalFontStateKey, MkStr::Null);
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::EditBox::SelectionFontStateKey, MkStr::Null);
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::EditBox::CursorFontStateKey, MkStr::Null);

	tNode->DeclareToTemplate(true);
}

bool MkEditBoxNode::__StepBackMsgHistory(void)
{
	if (m_UseHistory)
	{
		SetText(m_MessageHistory.StepBack());
	}
	return m_UseHistory;
}

bool MkEditBoxNode::__StepForwardMsgHistory(void)
{
	if (m_UseHistory)
	{
		MkStr newMsg;
		if (m_MessageHistory.StepForward(newMsg))
		{
			SetText(newMsg);
			return true;
		}
	}
	return false;
}

void MkEditBoxNode::__BindingLost(void)
{
	if (ExistSRect(CURSOR_SRECT_NAME))
	{
		DeleteSRect(CURSOR_SRECT_NAME);
	}
}

void MkEditBoxNode::__SetFocus(void)
{
	MkSRect* srect = CreateSRect(CURSOR_SRECT_NAME);
	if (srect != NULL)
	{
		MkStr buffer;
		if (MkDecoStr::Convert(_GetFontType(), _GetFontState(m_CursorFontState), 0, MKDEF_CURSOR_CHAR, buffer) &&
			srect->SetDecoString(buffer))
		{
			_UpdateCursor();
		}
	}
}

bool MkEditBoxNode::__UpdateTextInfo(const MkStr& msg, DWORD setStart, DWORD setEnd)
{
	bool ok = ((msg != m_Text) || (setStart != m_SelStart) || (setEnd != m_SelEnd));
	if (ok)
	{
		m_Text = msg;
		m_SelStart = setStart;
		m_SelEnd = setEnd;

		if (m_Text.Empty())
		{
			if (ExistSRect(TEXT_SRECT_NAME))
			{
				DeleteSRect(TEXT_SRECT_NAME);
			}
		}
		else
		{
			MkSRect* textRect = ExistSRect(TEXT_SRECT_NAME) ? GetSRect(TEXT_SRECT_NAME) : CreateSRect(TEXT_SRECT_NAME);
			if (textRect != NULL)
			{
				if (m_SelStart == m_SelEnd) // 일반 커서상태면 바로 텍스트를 출력
				{
					MkStr buffer;
					if (MkDecoStr::Convert(_GetFontType(), _GetFontState(m_NormalFontState), 0, m_Text, buffer))
					{
						textRect->SetDecoString(buffer);
					}
				}

				textRect->AlignRect(GetPresetComponentSize(), eRAP_LeftCenter, MkFloat2(MKDEF_TEXT_START_POSITION, 0.f), 0.f, -MKDEF_BASE_WINDOW_DEPTH_GRID);
			}
		}

		_UpdateCursor();
	}
	return ok;
}

void MkEditBoxNode::_SetFontInfo(const MkHashStr& fontType, const MkHashStr& normalFontState, const MkHashStr& selectionFontState, const MkHashStr& cursorFontState)
{
	m_FontType = fontType;
	MK_CHECK(m_FontType.Empty() || MK_FONT_MGR.CheckAvailableFontType(m_FontType), GetNodeName().GetString() + L" MkEditBoxNode에 잘못된 font type 지정 : " + m_FontType.GetString())
		m_FontType.Clear();

	m_NormalFontState = normalFontState;
	MK_CHECK(m_NormalFontState.Empty() || MK_FONT_MGR.CheckAvailableFontState(m_NormalFontState), GetNodeName().GetString() + L" MkEditBoxNode에 잘못된 font state 지정 : " + m_NormalFontState.GetString())
		m_NormalFontState.Clear();

	m_SelectionFontState = selectionFontState;
	MK_CHECK(m_SelectionFontState.Empty() || MK_FONT_MGR.CheckAvailableFontState(m_SelectionFontState), GetNodeName().GetString() + L" MkEditBoxNode에 잘못된 font state 지정 : " + m_SelectionFontState.GetString())
		m_SelectionFontState.Clear();

	m_CursorFontState = cursorFontState;
	MK_CHECK(m_CursorFontState.Empty() || MK_FONT_MGR.CheckAvailableFontState(m_CursorFontState), GetNodeName().GetString() + L" MkEditBoxNode에 잘못된 font state 지정 : " + m_CursorFontState.GetString())
		m_CursorFontState.Clear();
}

void MkEditBoxNode::_UpdateCursor(void)
{
	if (ExistSRect(CURSOR_SRECT_NAME))
	{
		MkSRect* cursorRect = GetSRect(CURSOR_SRECT_NAME);

		if (m_SelStart == m_SelEnd)
		{
			cursorRect->SetVisible(true);

			const MkInt2& cs = cursorRect->GetDecoString().GetDrawingSize();
			float offset = static_cast<float>(cs.x) / 2.f;
			float position = MKDEF_TEXT_START_POSITION - offset;

			if ((m_SelEnd > 0) && ExistSRect(TEXT_SRECT_NAME))
			{
				MkStr subset;
				m_Text.GetSubStr(MkArraySection(0, static_cast<unsigned int>(m_SelEnd)), subset);
				subset.ReplaceKeyword(L" ", L"."); // GetTextSize()는 뒷부분의 공문자 인식을 못하기 때문에 치환
				position += static_cast<float>(MK_FONT_MGR.GetTextSize(_GetFontType(), subset).x);
			}

			cursorRect->AlignRect(GetPresetComponentSize(), eRAP_LeftCenter, MkFloat2(position, 0.f), 0.f, -MKDEF_BASE_WINDOW_DEPTH_GRID * 2.f);
		}
		else // m_SelStart < m_SelEnd. 텍스트가 반드시 존재
		{
			cursorRect->SetVisible(false);

			MkSRect* textRect = ExistSRect(TEXT_SRECT_NAME) ? GetSRect(TEXT_SRECT_NAME) : NULL;
			if (textRect != NULL)
			{
				MkStr buffer = m_Text;
				//if (MkDecoStr::Convert(_GetFontType(), _GetFontState(m_CursorFontState), 0, MKDEF_CURSOR_CHAR, buffer))
				{
					textRect->SetDecoString(buffer);
				}
			}
			//m_FontType;
			//m_NormalFontState;
			//m_SelectionFontState;
		}
	}
}

const MkHashStr& MkEditBoxNode::_GetFontType(void) const
{
	return (m_FontType.Empty() ? MK_FONT_MGR.DSF() : m_FontType);
}

const MkHashStr& MkEditBoxNode::_GetFontState(const MkHashStr& fontState) const
{
	return (fontState.Empty() ? MK_FONT_MGR.DSF() : fontState);
}

//------------------------------------------------------------------------------------------------//