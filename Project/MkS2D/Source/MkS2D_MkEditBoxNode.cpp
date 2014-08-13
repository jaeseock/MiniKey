
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkSlangFilter.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkEditBoxNode.h"
#include "MkS2D_MkSceneNodeFamilyDefinition.h"


#define MKDEF_MAX_HISTORY_RECORD 100
#define MKDEF_TEXT_START_POSITION (MK_WR_PRESET.GetMargin())
#define MKDEF_CURSOR_CHAR L"��"

const static MkHashStr TEXT_SRECT_NAME = L"__#Text";
const static MkHashStr CURSOR_SRECT_NAME = L"__#Cursor";
const static MkHashStr SEL_SRECT_NAME = L"__#Selection";

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

void MkEditBoxNode::CommitText(void)
{
	_PushWindowEvent(MkSceneNodeFamilyDefinition::eCommitText);
	
	if (m_UseHistory && (!m_Text.Empty()))
	{
		m_MessageHistory.Record(m_Text);
	}
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
	// ���� ���ø��� ������ ���ø� ����
	_ApplyBuildingTemplateToSave(node, MkSceneNodeFamilyDefinition::EditBox::TemplateName);

	node.SetData(MkSceneNodeFamilyDefinition::EditBox::TextKey, m_Text, 0);
	node.SetData(MkSceneNodeFamilyDefinition::EditBox::UseHistoryKey, m_UseHistory, 0);
	node.SetData(MkSceneNodeFamilyDefinition::EditBox::FontTypeKey, m_FontType.GetString(), 0);
	node.SetData(MkSceneNodeFamilyDefinition::EditBox::NormalFontStateKey, m_NormalFontState.GetString(), 0);
	node.SetData(MkSceneNodeFamilyDefinition::EditBox::SelectionFontStateKey, m_SelectionFontState.GetString(), 0);
	node.SetData(MkSceneNodeFamilyDefinition::EditBox::CursorFontStateKey, m_CursorFontState.GetString(), 0);

	// MkBaseWindowNode
	MkBaseWindowNode::Save(node);

	// ���� srect��(text, cursor ��¿�)�� ���� �ʿ伺�� �����Ƿ� ����
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
	m_CharStart = 0;
	m_CharEnd = 0;

	m_ShowCursorRect = false;
}

//------------------------------------------------------------------------------------------------//

void MkEditBoxNode::__GenerateBuildingTemplate(void)
{
	MkDataNode node;
	MkDataNode* tNode = node.CreateChildNode(MkSceneNodeFamilyDefinition::EditBox::TemplateName);
	MK_CHECK(tNode != NULL, MkSceneNodeFamilyDefinition::EditBox::TemplateName.GetString() + L" template node alloc ����")
		return;

	tNode->ApplyTemplate(MkSceneNodeFamilyDefinition::BaseWindow::TemplateName); // MkBaseWindowNode�� template ����

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
	if (ExistSRect(SEL_SRECT_NAME))
	{
		DeleteSRect(SEL_SRECT_NAME);
	}
}

void MkEditBoxNode::__SetFocus(void)
{
	// �ؽ�Ʈ ������ ó�� �� �ʿ� ���� Ŀ���� ���� ������ ����
	MkSRect* cursorRect = CreateSRect(CURSOR_SRECT_NAME);
	if (cursorRect != NULL)
	{
		MkStr buffer;
		if (MkDecoStr::Convert(_GetFontType(), _GetCursorFontState(), 0, MKDEF_CURSOR_CHAR, buffer))
		{
			cursorRect->SetDecoString(buffer);
		}
	}
	
	MkSRect* selRect = CreateSRect(SEL_SRECT_NAME);
	if (selRect != NULL)
	{
		selRect->SetTexture(MK_WR_PRESET.GetSystemImageFilePath(), MK_WR_PRESET.GetEditBoxSelectionSubsetName());
	}

	_UpdateCursorAndSelection();
}

bool MkEditBoxNode::__UpdateTextInfo(const MkStr& msg, DWORD selStart, DWORD selEnd)
{
	bool newText = (msg != m_Text);
	bool ok = (newText || (selStart != m_SelStart) || (selEnd != m_SelEnd));
	if (ok)
	{
		// �ؽ�Ʈ ũ�Ⱑ ������ ũ�⺸�� Ŭ �� �����Ƿ� ���� ����(m_CharStart ~ m_CharEnd)��ŭ�� �����־�� ��
		// ������ �̵��� ������ ��ǲ���� ���� Ŀ��/���ÿ��� ��ȭ�� ����
		MkStr safeMsg;
		if (msg.Empty())
		{
			m_CharStart = 0;
			m_CharEnd = 0;
		}
		else
		{
			// ��Ӿ� �Ǻ�
			MK_SLANG_FILTER.CheckString(msg, safeMsg);

			float availableWidth = GetPresetComponentSize().x - MKDEF_TEXT_START_POSITION * 2.f;
			DWORD lookPos = 0xffffffff; // ���� ��ǥ ��ġ. 0xffffffff�� ���� ��ȭ ������ �ǹ�
			
			// Ŀ�� ��ȭ�� ���� ���� ��ǥ Ž��
			if (selStart == selEnd) // �Ϲ� Ŀ��
			{
				if (m_SelStart == m_SelEnd)
				{
					if (selEnd != m_SelEnd) // �Ϲ� Ŀ�� �̵��� ������ ���� ��ǥ ����
					{
						lookPos = selEnd;
					}
				}
				else // ���� ���°� ���� ����
				{
					lookPos = selEnd;
				}
			}
			else // ���� ���� ������ ��� start/end �� ��ȭ�� �ִ� ���� ���� ��ǥ�� ��
			{
				if (selStart != m_SelStart)
				{
					lookPos = selStart;
				}
				else if (selEnd != m_SelEnd)
				{
					lookPos = selEnd;
				}
			}

			// m_CharStart ����
			if (lookPos != 0xffffffff)
			{
				// ���� ��ǥ�� ��� ��ǥ���� ���� ��� ��� ��ǥ �̵�
				if (lookPos < m_CharStart)
				{
					m_CharStart = lookPos;
				}
				// ���� ��ǥ�� ��� ��ǥ���� Ŭ ��� ������ �´� m_CharStart�� ã��
				else if (lookPos > m_CharStart)
				{
					while (true)
					{
						if (_GetTextWidth(m_CharStart, lookPos, safeMsg) <= availableWidth)
							break;

						++m_CharStart;
					}
				}
			}

			// ������ ������ �ʿ� �� �� ����
			bool updateCharEnd = true;
			if (safeMsg.GetSize() < m_Text.GetSize())
			{
				// �ؽ�Ʈ ũ�Ⱑ �۴ٸ� ����
				if (_GetTextWidth(0, safeMsg.GetSize(), safeMsg) <= availableWidth)
				{
					m_CharStart = 0;
					m_CharEnd = safeMsg.GetSize();
					updateCharEnd = false;
				}
				else
				{
					// Ŀ���� ������ ������ �ȿ� �ִٸ� ����Ž������ ��� ��ǥ ������
					if (_GetTextWidth(selEnd, safeMsg.GetSize(), safeMsg) <= availableWidth)
					{
						unsigned int beginPos = 0;
						unsigned int endPos = safeMsg.GetSize();
						while (true)
						{
							unsigned int currentPos = (beginPos + endPos) / 2;
							if (beginPos == currentPos)
								break;

							float textWidth = _GetTextWidth(currentPos, safeMsg.GetSize(), safeMsg);
							if (textWidth > availableWidth)
							{
								beginPos = currentPos;
							}
							else if (textWidth < availableWidth)
							{
								endPos = currentPos;
							}
							else // textWidth == availableWidth
							{
								beginPos = currentPos;
								break;
							}
						}

						if (_GetTextWidth(beginPos, safeMsg.GetSize(), safeMsg) > availableWidth)
						{
							++beginPos;
						}

						m_CharStart = static_cast<DWORD>(beginPos);
						m_CharEnd = safeMsg.GetSize();
						updateCharEnd = false;
					}
				}
			}

			// m_CharStart�� ���߾� m_CharEnd ����
			if (updateCharEnd)
			{
				if (_GetTextWidth(m_CharStart, safeMsg.GetSize(), safeMsg) > availableWidth)
				{
					// ��� ��ǥ�κ����� �ؽ�Ʈ�� ������ �Ѿ ��� ����Ž������ ���� ���
					unsigned int beginPos = m_CharStart;
					unsigned int endPos = safeMsg.GetSize();
					while (true)
					{
						unsigned int currentPos = (beginPos + endPos) / 2;
						if (beginPos == currentPos)
							break;

						float textWidth = _GetTextWidth(m_CharStart, currentPos, safeMsg);
						if (textWidth > availableWidth)
						{
							endPos = currentPos;
						}
						else if (textWidth < availableWidth)
						{
							beginPos = currentPos;
						}
						else // textWidth == availableWidth
						{
							beginPos = currentPos;
							break;
						}
					}

					m_CharEnd = beginPos;
				}
				else
				{
					m_CharEnd = safeMsg.GetSize();
				}
			}
		}

		// �� �� �ݿ�
		m_Text = safeMsg;
		m_SelStart = selStart;
		m_SelEnd = selEnd;

		// �� �ؽ�Ʈ�̰ų� �����ڸ� ����
		if (m_Text.Empty() || (m_Text.GetFirstValidPosition() == MKDEF_ARRAY_ERROR))
		{
			_DeleteTextRect();
		}
		// ��ȿ���ڰ� ����
		else
		{
			MkStr textOut;
			m_Text.GetSubStr(MkArraySection(m_CharStart, m_CharEnd - m_CharStart), textOut);

			// �ؽ�Ʈ ���
			MkStr buffer;
			if (m_SelStart == m_SelEnd)
			{
				MkDecoStr::Convert(_GetFontType(), _GetNormalFontState(), 0, textOut, buffer);
			}
			else // m_SelStart < m_SelEnd. ���� ������ ȭ�鿡 ������ �ʴ� ���(m_CharStart >= m_SelEnd)�� �������� ����
			{
				MkArray<MkHashStr> fontState(3);
				MkArray<unsigned int> statePos(3);

				// ���� ������ ������ ��� ���� �ȿ� ���� �ִ� ���
				if ((m_CharStart <= m_SelStart) && (m_CharEnd >= m_SelEnd))
				{
					fontState.PushBack(_GetNormalFontState());
					fontState.PushBack(_GetSelectionFontState());
					fontState.PushBack(_GetNormalFontState());

					statePos.PushBack(0);
					statePos.PushBack(static_cast<unsigned int>(m_SelStart - m_CharStart));
					statePos.PushBack(static_cast<unsigned int>(m_SelEnd - m_CharStart));
				}
				// ���� ���� ������ �߷� �ִ� ���
				else if ((m_CharStart > m_SelStart) && (m_CharEnd >= m_SelEnd))
				{
					fontState.PushBack(_GetSelectionFontState());
					fontState.PushBack(_GetNormalFontState());

					statePos.PushBack(0);
					statePos.PushBack(static_cast<unsigned int>(m_SelEnd - m_CharStart));
				}
				// ���� ���� ������ �߷� �ִ� ���
				else if ((m_CharStart <= m_SelStart) && (m_CharEnd < m_SelEnd))
				{
					fontState.PushBack(_GetNormalFontState());
					fontState.PushBack(_GetSelectionFontState());

					statePos.PushBack(0);
					statePos.PushBack(static_cast<unsigned int>(m_SelStart - m_CharStart));
				}

				MkDecoStr::Convert(_GetFontType(), fontState, statePos, 0, textOut, buffer);
			}

			if (!buffer.Empty())
			{
				MkSRect* textRect = ExistSRect(TEXT_SRECT_NAME) ? GetSRect(TEXT_SRECT_NAME) : CreateSRect(TEXT_SRECT_NAME);
				textRect->SetDecoString(buffer);
				textRect->AlignRect(GetPresetComponentSize(), eRAP_LeftCenter, MkFloat2(MKDEF_TEXT_START_POSITION, 0.f), 0.f);
				textRect->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
			}
		}

		_UpdateCursorAndSelection();

		if (newText)
		{
			_PushWindowEvent(MkSceneNodeFamilyDefinition::eModifyText);
		}
	}
	return ok;
}

void MkEditBoxNode::__ToggleCursorRect(void)
{
	if (m_ShowCursorRect)
	{
		MkSRect* cursorRect = GetSRect(CURSOR_SRECT_NAME);
		if (cursorRect != NULL)
		{
			cursorRect->SetVisible(!cursorRect->GetVisible());
		}
	}
}

void MkEditBoxNode::_SetFontInfo(const MkHashStr& fontType, const MkHashStr& normalFontState, const MkHashStr& selectionFontState, const MkHashStr& cursorFontState)
{
	m_FontType = fontType;
	MK_CHECK(m_FontType.Empty() || MK_FONT_MGR.CheckAvailableFontType(m_FontType), GetNodeName().GetString() + L" MkEditBoxNode�� �߸��� font type ���� : " + m_FontType.GetString())
		m_FontType.Clear();

	m_NormalFontState = normalFontState;
	MK_CHECK(m_NormalFontState.Empty() || MK_FONT_MGR.CheckAvailableFontState(m_NormalFontState), GetNodeName().GetString() + L" MkEditBoxNode�� �߸��� font state ���� : " + m_NormalFontState.GetString())
		m_NormalFontState.Clear();

	m_SelectionFontState = selectionFontState;
	MK_CHECK(m_SelectionFontState.Empty() || MK_FONT_MGR.CheckAvailableFontState(m_SelectionFontState), GetNodeName().GetString() + L" MkEditBoxNode�� �߸��� font state ���� : " + m_SelectionFontState.GetString())
		m_SelectionFontState.Clear();

	m_CursorFontState = cursorFontState;
	MK_CHECK(m_CursorFontState.Empty() || MK_FONT_MGR.CheckAvailableFontState(m_CursorFontState), GetNodeName().GetString() + L" MkEditBoxNode�� �߸��� font state ���� : " + m_CursorFontState.GetString())
		m_CursorFontState.Clear();
}

void MkEditBoxNode::_UpdateCursorAndSelection(void)
{
	// �� srect�� ������ __BindingLost()���� ������ ���������� �ƹ� �͵� ���� ����
	if (ExistSRect(CURSOR_SRECT_NAME) && ExistSRect(SEL_SRECT_NAME))
	{
		MkStr textOut;
		m_Text.GetSubStr(MkArraySection(m_CharStart, m_CharEnd - m_CharStart), textOut);

		if (m_SelStart == m_SelEnd)
		{
			MkSRect* cursorRect = GetSRect(CURSOR_SRECT_NAME);

			m_ShowCursorRect = true;
			
			const MkInt2& cs = cursorRect->GetDecoString().GetDrawingSize();
			float offset = static_cast<float>(cs.x) / 2.f;
			float position = MKDEF_TEXT_START_POSITION - offset + _GetTextWidth(0, m_SelEnd - m_CharStart, textOut);
			cursorRect->AlignRect(GetPresetComponentSize(), eRAP_LeftCenter, MkFloat2(position, 0.f), 0.f);
			cursorRect->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID * 2.f);

			GetSRect(SEL_SRECT_NAME)->SetVisible(false);
		}
		else // m_SelStart < m_SelEnd
		{
			GetSRect(CURSOR_SRECT_NAME)->SetVisible(false);
			m_ShowCursorRect = false;

			MkSRect* selRect = GetSRect(SEL_SRECT_NAME);
			selRect->SetVisible(true);

			float offset = MKDEF_TEXT_START_POSITION;
			float beginPos, width;

			// ���� ������ ������ ��� ���� �ȿ� ���� �ִ� ���
			if ((m_CharStart <= m_SelStart) && (m_CharEnd >= m_SelEnd))
			{
				beginPos = _GetTextWidth(0, m_SelStart - m_CharStart, textOut);
				width = _GetTextWidth(m_SelStart - m_CharStart, m_SelEnd - m_CharStart, textOut);
			}
			// ���� ���� ������ �߷� �ִ� ���
			else if ((m_CharStart > m_SelStart) && (m_CharEnd >= m_SelEnd))
			{
				beginPos = 0.f;
				width = _GetTextWidth(0, m_SelEnd - m_CharStart, textOut);
			}
			// ���� ���� ������ �߷� �ִ� ���
			else if ((m_CharStart <= m_SelStart) && (m_CharEnd < m_SelEnd))
			{
				beginPos = _GetTextWidth(0, m_SelStart - m_CharStart, textOut);
				width = _GetTextWidth(m_SelStart - m_CharStart, m_CharEnd - m_CharStart, textOut);
			}
			
			selRect->SetLocalSize(MkFloat2(width, static_cast<float>(MK_FONT_MGR.GetFontHeight(_GetFontType()) + 2))); // ���� 1px�� ũ��
			selRect->AlignRect(GetPresetComponentSize(), eRAP_LeftCenter, MkFloat2(offset + beginPos, 0.f), 0.f);
			selRect->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID * 0.5f);
		}
	}
}

void MkEditBoxNode::_DeleteTextRect(void)
{
	if (ExistSRect(TEXT_SRECT_NAME))
	{
		DeleteSRect(TEXT_SRECT_NAME);
	}
}

float MkEditBoxNode::_GetTextWidth(DWORD beginPos, DWORD endPos, const MkStr& text) const
{
	if ((beginPos >= 0) && (beginPos < endPos))
	{
		MkStr subset;
		text.GetSubStr(MkArraySection(static_cast<unsigned int>(beginPos), static_cast<unsigned int>(endPos - beginPos)), subset);
		subset.ReplaceKeyword(L" ", L"."); // GetTextSize()�� �޺κ��� ������ �ν��� ���ϱ� ������ ġȯ
		return static_cast<float>(MK_FONT_MGR.GetTextSize(_GetFontType(), subset).x);
	}
	return 0.f;
}

const MkHashStr& MkEditBoxNode::_GetFontType(void) const
{
	return (m_FontType.Empty() ? MK_WR_PRESET.GetEditBoxFontType() : m_FontType);
}

const MkHashStr& MkEditBoxNode::_GetNormalFontState(void) const
{
	return (m_NormalFontState.Empty() ? MK_WR_PRESET.GetEditBoxNormalFontState() : m_NormalFontState);
}

const MkHashStr& MkEditBoxNode::_GetSelectionFontState(void) const
{
	return (m_SelectionFontState.Empty() ? MK_WR_PRESET.GetEditBoxSelectionFontState() : m_SelectionFontState);
}

const MkHashStr& MkEditBoxNode::_GetCursorFontState(void) const
{
	return (m_CursorFontState.Empty() ? MK_WR_PRESET.GetEditBoxCursorFontState() : m_CursorFontState);
}

//------------------------------------------------------------------------------------------------//