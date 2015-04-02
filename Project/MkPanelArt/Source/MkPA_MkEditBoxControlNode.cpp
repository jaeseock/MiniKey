
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
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkEditBoxControlNode 생성 실패") {}
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
	textPanel.SetLocalDepth(-0.1f); // 배경과 겹치지 않도록 0.1f만큼 앞에 위치
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
	// left cursor click이고 해당 window가 close button이면 event를 ePA_SNE_CloseWindow로 바꾸어 보냄
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
	// text panel 확인
	if (!PanelExist(TextPanelName))
		return false;

	MkPanel* textPanel = GetPanel(TextPanelName);
	if (textPanel == NULL)
		return false;

	MkTextNode* textNode = textPanel->GetTextNodePtr();
	if (textNode == NULL)
		return false;

	// 비속어 필터링
	MkStr safeMsg;
	MK_KEYWORD_FILTER.CheckSlang(msg, safeMsg);

	// 설정 시작
	bool newText = (safeMsg != m_Text);
	bool proceed = (newText || (selStart != m_SelStart) || (selEnd != m_SelEnd));
	if (proceed)
	{
		// 커서 변화를 통한 기준 위치 탐색
		int lookPos = -1; // 기준 위치
		if (selStart == selEnd) // 일반 커서
		{
			if (m_SelStart == m_SelEnd)
			{
				if (selEnd != m_SelEnd) // 일반 커서 상태에서의 이동
				{
					lookPos = selEnd;
				}
			}
			else // 선택 영역에서 일반 커서로 변경됨
			{
				lookPos = selEnd;
			}
		}
		else // 선택 영역. start/end 중 변화가 있는 쪽이 기준 좌표가 됨
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
		// 텍스트 크기가 윈도우 크기보다 클 수 있으므로 일정 영역(m_CharStart ~ m_CharEnd)만큼만 보여주어야 함
		// 영역의 이동은 유저의 인풋으로 인한 커서/선택영역 변화에 따름

		const MkHashStr& fontType = textNode->GetFontType();

		// 이전 정보
		float textPanelWidth = textPanel->GetPanelSize().x;
		float lastScrollPos = textPanel->GetPixelScrollPosition().x;
		float lastCoverage = textPanelWidth + lastScrollPos;
		float lastTextLength = static_cast<float>(textNode->GetWholePixelSize().x);

		// text 수정
		m_Text = safeMsg;
		textNode->SetText(m_Text);
		textPanel->BuildAndUpdateTextCache();

		// 현재 정보
		float currTextLength = static_cast<float>(textNode->GetWholePixelSize().x);

		//_GetTextLength(fontType, m_Text, 0, unsigned int endPos)
		//if (CheckInclusion<float>())
		if (currTextLength >= lastCoverage)
		{
			
		}
		


		float availableWidth = textPanel->GetPanelSize().x;
		
		
		
		/*

		// m_CharStart 결정
		if (lookPos != 0xffffffff)
		{
			// 기준 좌표가 출력 좌표보다 작을 경우 출력 좌표 이동
			if (lookPos < m_CharStart)
			{
				m_CharStart = lookPos;
			}
			// 기준 좌표가 출력 좌표보다 클 경우 범위에 맞는 m_CharStart을 찾음
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

		// 삭제는 재정렬 필요 할 수 있음
		bool updateCharEnd = true;
		if (safeMsg.GetSize() < m_Text.GetSize())
		{
			// 텍스트 크기가 작다면 리셋
			if (_GetTextWidth(0, safeMsg.GetSize(), safeMsg) <= availableWidth)
			{
				m_CharStart = 0;
				m_CharEnd = safeMsg.GetSize();
				updateCharEnd = false;
			}
			else
			{
				// 커서가 마지막 페이지 안에 있다면 이진탐색으로 출력 좌표 역산출
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

		// m_CharStart에 맞추어 m_CharEnd 결정
		if (updateCharEnd)
		{
			if (_GetTextWidth(m_CharStart, safeMsg.GetSize(), safeMsg) > availableWidth)
			{
				// 출력 좌표로부터의 텍스트가 범위를 넘어갈 경우 이진탐색으로 범위 계산
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
		// 새 값 반영
		m_Text = safeMsg;
		m_SelStart = selStart;
		m_SelEnd = selEnd;

		// 빈 텍스트이거나 공문자만 존재
		if (m_Text.Empty() || (m_Text.GetFirstValidPosition() == MKDEF_ARRAY_ERROR))
		{
			_DeleteTextRect();
		}
		// 유효문자가 존재
		else
		{
			MkStr textOut;
			m_Text.GetSubStr(MkArraySection(m_CharStart, m_CharEnd - m_CharStart), textOut);

			// 텍스트 출력
			MkStr buffer;
			if (m_SelStart == m_SelEnd)
			{
				MkDecoStr::Convert(_GetFontType(), _GetNormalFontState(), 0, textOut, buffer);
			}
			else // m_SelStart < m_SelEnd. 선택 영역이 화면에 보이지 않는 경우(m_CharStart >= m_SelEnd)는 존재하지 않음
			{
				MkArray<MkHashStr> fontState(3);
				MkArray<unsigned int> statePos(3);

				// 선택 영역이 온전히 출력 영역 안에 들어와 있는 경우
				if ((m_CharStart <= m_SelStart) && (m_CharEnd >= m_SelEnd))
				{
					fontState.PushBack(_GetNormalFontState());
					fontState.PushBack(_GetSelectionFontState());
					fontState.PushBack(_GetNormalFontState());

					statePos.PushBack(0);
					statePos.PushBack(static_cast<unsigned int>(m_SelStart - m_CharStart));
					statePos.PushBack(static_cast<unsigned int>(m_SelEnd - m_CharStart));
				}
				// 좌측 선택 영역이 잘려 있는 경우
				else if ((m_CharStart > m_SelStart) && (m_CharEnd >= m_SelEnd))
				{
					fontState.PushBack(_GetSelectionFontState());
					fontState.PushBack(_GetNormalFontState());

					statePos.PushBack(0);
					statePos.PushBack(static_cast<unsigned int>(m_SelEnd - m_CharStart));
				}
				// 우측 선택 영역이 잘려 있는 경우
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