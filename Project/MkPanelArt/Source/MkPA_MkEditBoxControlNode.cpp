
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkKeywordFilter.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkFontManager.h"
#include "MkPA_MkTextNode.h"
#include "MkPA_MkEditBoxControlNode.h"


const MkHashStr MkEditBoxControlNode::TextPanelName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Text";
const MkHashStr MkEditBoxControlNode::CursorPanelName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Cursor";
const MkHashStr MkEditBoxControlNode::SelectionPanelName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Selection";

const MkHashStr MkEditBoxControlNode::ArgKey_Text = L"Text";

//------------------------------------------------------------------------------------------------//

MkEditBoxControlNode* MkEditBoxControlNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkEditBoxControlNode* node = __TSI_SceneNodeDerivedInstanceOp<MkEditBoxControlNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkEditBoxControlNode ���� ����") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkEditBoxControlNode::SetSingleLineEditBox
(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length, const MkStr& initMsg, bool useHistory)
{
	m_WindowFrameType = frameType;
	float frameSize = MK_STATIC_RES.GetWindowThemeSet().GetFrameSize(themeName, m_WindowFrameType);
	
	// background
	SetThemeName(themeName);
	SetComponentType(MkWindowThemeData::eCT_DefaultBox); // eCT_NoticeBox
	SetClientSize(MkFloat2(GetMax<float>(length, frameSize), frameSize));
	SetFormState(MkWindowThemeFormData::eS_Default);

	// history
	m_UseHistory = useHistory;
	if (m_UseHistory)
	{
		m_MessageHistory.SetUp(64);
	}

	// text panel
	MkPanel& textPanel = CreatePanel(TextPanelName);
	const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(GetThemeName(), GetComponentType(), MkHashStr::EMPTY);
	if (formData != NULL)
	{
		textPanel.SetLocalPosition(MkFloat2(formData->GetLeftMargin(), formData->GetBottomMargin()));
	}
	textPanel.SetLocalDepth(-0.1f); // ���� ��ġ�� �ʵ��� 0.1f��ŭ �տ� ��ġ
	textPanel.SetSmallerSourceOp(MkPanel::eAttachToLeftTop);
	textPanel.SetBiggerSourceOp(MkPanel::eCutSource);
	textPanel.SetPanelSize(GetClientRect().size);
	textPanel.SetTextNode(MK_STATIC_RES.GetWindowThemeSet().GetEditTextNode(GetThemeName(), m_WindowFrameType), false);

	// init
	SetText(initMsg);
}

void MkEditBoxControlNode::SetText(const MkStr& msg)
{
	DWORD selPos = static_cast<DWORD>(msg.GetSize());
	__UpdateText(msg, selPos, selPos);
}

void MkEditBoxControlNode::CommitText(void)
{
	MkDataNode arg;
	arg.CreateUnit(ArgKey_Text, m_Text);
	StartNodeReportTypeEvent(ePA_SNE_TextCommitted, &arg);
	
	if (m_UseHistory && (!m_Text.Empty()))
	{
		m_MessageHistory.Record(m_Text);
	}
}

void MkEditBoxControlNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	/*
	// left cursor click�̰� �ش� window�� close button�̸� event�� ePA_SNE_CloseWindow�� �ٲپ� ����
	if ((eventType == ePA_SNE_CursorLBtnReleased) && (path.GetSize() == 1) && (path[0] == CloseButtonNodeName))
	{
		StartNodeReportTypeEvent(ePA_SNE_CloseWindow, NULL);
	}
	else
	{
		MkWindowBaseNode::SendNodeReportTypeEvent(eventType, path, argument);
	}
	*/
}

MkEditBoxControlNode::MkEditBoxControlNode(const MkHashStr& name) : MkWindowBaseNode(name)
{
	m_UseHistory = false;
	m_SelStart = 0;
	m_SelEnd = 0;
}

bool MkEditBoxControlNode::__UpdateText(const MkStr& msg, int selStart, int selEnd)
{
	// text panel Ȯ��
	if (!PanelExist(TextPanelName))
		return false;

	MkPanel* textPanel = GetPanel(TextPanelName);
	if (textPanel == NULL)
		return false;

	MkTextNode* textNode = textPanel->GetTextNodePtr();
	if (textNode == NULL)
		return false;

	// ��Ӿ� ���͸�
	MkStr safeMsg;
	MK_KEYWORD_FILTER.CheckSlang(msg, safeMsg);

	// ���� ����
	bool newText = (safeMsg != m_Text);
	bool proceed = (newText || (selStart != m_SelStart) || (selEnd != m_SelEnd));
	if (proceed)
	{
		// Ŀ�� ��ȭ�� ���� ���� ��ġ Ž��
		int lookPos = -1; // ���� ��ġ
		if (selStart == selEnd) // �Ϲ� Ŀ��
		{
			if (m_SelStart == m_SelEnd)
			{
				if (selEnd != m_SelEnd) // �Ϲ� Ŀ�� ���¿����� �̵�
				{
					lookPos = selEnd;
				}
			}
			else // ���� �������� �Ϲ� Ŀ���� �����
			{
				lookPos = selEnd;
			}
		}
		else // ���� ����. start/end �� ��ȭ�� �ִ� ���� ���� ��ǥ�� ��
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
		// �ؽ�Ʈ ũ�Ⱑ ������ ũ�⺸�� Ŭ �� �����Ƿ� ���� ����(m_CharStart ~ m_CharEnd)��ŭ�� �����־�� ��
		// ������ �̵��� ������ ��ǲ���� ���� Ŀ��/���ÿ��� ��ȭ�� ����

		const MkHashStr& fontType = textNode->GetFontType();

		// ���� ����
		float textPanelWidth = textPanel->GetPanelSize().x;
		float lastScrollPos = textPanel->GetPixelScrollPosition().x;
		float lastCoverage = textPanelWidth + lastScrollPos;
		float lastTextLength = static_cast<float>(textNode->GetWholePixelSize().x);

		// text ����
		m_Text = safeMsg;
		textNode->SetText(m_Text);
		textPanel->BuildAndUpdateTextCache();

		// ���� ����
		float currTextLength = static_cast<float>(textNode->GetWholePixelSize().x);

		//_GetTextLength(fontType, m_Text, 0, unsigned int endPos)
		//if (CheckInclusion<float>())
		if (currTextLength >= lastCoverage)
		{
			
		}
		


		float availableWidth = textPanel->GetPanelSize().x;
		
		
		
		/*

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
		*/
		

		/*
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

		if (newText && pushEvent)
		{
			_PushWindowEvent(MkSceneNodeFamilyDefinition::eModifyText);
		}
		*/
	}
	
	MkDataNode arg;
	arg.CreateUnit(ArgKey_Text, m_Text);
	StartNodeReportTypeEvent(ePA_SNE_TextModified, &arg);

	return proceed;
}

bool MkEditBoxControlNode::__StepBackMsgHistory(void)
{
	if (m_UseHistory)
	{
		SetText(m_MessageHistory.StepBack());
	}
	return m_UseHistory;
}

bool MkEditBoxControlNode::__StepForwardMsgHistory(void)
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

//------------------------------------------------------------------------------------------------//

float MkEditBoxControlNode::_GetTextLength(const MkHashStr& fontType, const MkStr& text, unsigned int beginPos, unsigned int endPos) const
{
	if ((beginPos >= 0) && (beginPos < endPos))
	{
		MkStr subset;
		text.GetSubStr(MkArraySection(static_cast<unsigned int>(beginPos), static_cast<unsigned int>(endPos - beginPos)), subset);
		return static_cast<float>(MK_FONT_MGR.GetTextSize(fontType, subset, false).x);
	}
	return 0.f;
}

//------------------------------------------------------------------------------------------------//