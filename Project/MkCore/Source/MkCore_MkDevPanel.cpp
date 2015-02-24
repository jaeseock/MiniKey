
#include "MkCore_MkCheck.h"
#include "MkCore_MkFloatOp.h"
#include "MkCore_MkCheatMessage.h"
#include "MkCore_MkSystemEnvironment.h"
#include "MkCore_MkLogManager.h"
#include "MkCore_MkProfilingManager.h"
#include "MkCore_MkTimeManager.h"
#include "MkCore_MkTitleBarHooker.h"
#include "MkCore_MkDevPanel.h"

// base
#define MKDEF_FONT_NAME L"����" // ��Ʈ ����
#define MKDEF_FONT_SIZE 12 // ��Ʈ ũ��
#define MKDEF_CLIENT_WIDTH 440 // â ���� ũ��
#define MKDEF_TEXT_HEIGHT 12 // �ؽ�Ʈ�ڽ� ����
#define MKDEF_BUTTON_HEIGHT 20 // ��ư ����
#define MKDEF_BASE_OFFSET 5 // ��Ʈ�� ���� ����
#define MKDEF_TEXT_OFFSET 1 // �ؽ�Ʈ ���� ���� ����
#define MKDEF_INDEX_AREA_WIDTH 20 // board line���� �ε��� ���� ���� ũ��
#define MKDEF_LOG_BOX_HEIGHT 244 // �α� �ؽ�Ʈ �ڽ� ���� ũ��(20����)
#define MKDEF_MAX_LOG_LINE 100 // �ؽ�Ʈ�ڽ��� ��µ� �ִ� �α� �޽��� ��
#define MKDEF_MAX_INPUT_COUNT 512 // ��ǲâ�� �ִ� �Է°��� ���� ��
#define MKDEF_MAX_HISTORY_RECORD 100 // ��ǲ ��� �ִ� ���� ũ��
#define MKDEF_SEND_BUTTON_WIDTH 40 // SEND ��ư ���� ũ��

// id
#define MKDEF_VERSION_EDIT_ID 1
#define MKDEF_TIME_STATE_EDIT_ID 2

#define MKDEF_OPEN_LOG_BUTTON_ID 3
#define MKDEF_QUIT_BUTTON_ID 4

#define MKDEF_FREEBOARD_RADIO_ID 5
#define MKDEF_SYSTEM_RADIO_ID 6
#define MKDEF_PROFILER_RADIO_ID 7
#define MKDEF_THREAD_PROF_RADIO_ID 8
#define MKDEF_DRAWING_RADIO_ID 9

#define MKDEF_BOARD_LINE_ID_BEGIN 100 // MKDEF_BOARD_LINE_ID_BEGIN ~ (MKDEF_BOARD_LINE_ID_BEGIN + MKDEF_BOARD_LINE_COUNT * 2)
#define MKDEF_BOARD_LINE_COUNT 20

#define MKDEF_LOG_EDIT_ID 300

#define MKDEF_INPUT_EDIT_ID 301
#define MKDEF_SEND_BUTTON_ID 302

// title bar hooker
static MkTitleBarHooker gDevPanelTitleBarHooker;
static WNDPROC gOldInputProc = NULL;

//------------------------------------------------------------------------------------------------//

bool MkDevPanel::SetUp(MkListeningWindow& rootWindow, eRectAlignmentPosition alignmentPosition, MkCheatMessage* cheatInterface)
{
	if (!IsEnable())
		return false;

	HWND hParent = rootWindow.GetWindowHandle();
	MK_CHECK(hParent != NULL, L"�θ� �����찡 �ʱ�ȭ���� �ʾ� MkDevPanel ���� �Ұ�")
	{
		m_Enable = false;
		return false;
	}

	// ������ ����
	const int totalClientHeight = MKDEF_BASE_OFFSET * 7 +
		MKDEF_TEXT_HEIGHT * (1 + MKDEF_BOARD_LINE_COUNT) +
		MKDEF_TEXT_OFFSET * (MKDEF_BOARD_LINE_COUNT - 1) +
		MKDEF_LOG_BOX_HEIGHT +
		MKDEF_BUTTON_HEIGHT * 3;

	HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hParent, GWL_HINSTANCE));
	m_Window.SetAlignmentPosition(alignmentPosition);
	bool ok = m_Window.SetUpByWindowCreation
		(hInstance, __DevPanelWndProc, hParent, L"MK_DEV_PANEL", eSWP_None, MkInt2::Zero, MkInt2(MKDEF_CLIENT_WIDTH, totalClientHeight));

	MK_CHECK(ok, L"MkDevPanel ���� ����")
	{
		m_Enable = false;
		return false;
	}

	// ��Ʈ �����쿡 ���
	rootWindow.RegisterWindowUpdateListener(&m_Window);

	// ġƮ �������̽� ����
	_SetCheatInterface(cheatInterface);

	// hooker
	gDevPanelTitleBarHooker.SetUp(&m_Window);

	// ��Ʈ�� ���� �غ�
	m_Font = CreateFont
		(MKDEF_FONT_SIZE, 0, 0, 0, 0, 0, 0, 0, HANGUL_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH | FF_ROMAN, MKDEF_FONT_NAME);

	HWND hWnd = m_Window.GetWindowHandle();

	const int fullWidth = MKDEF_CLIENT_WIDTH - MKDEF_BASE_OFFSET * 2;
	const int halfWidth = (fullWidth - MKDEF_BASE_OFFSET) / 2;
	const int pentaWidth = (fullWidth - MKDEF_BASE_OFFSET * 4) / 5;
	const DWORD staticTextControlStyle = WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY;
	const DWORD staticIndexControlStyle = WS_CHILD | WS_VISIBLE | ES_CENTER | ES_READONLY;
	const DWORD boardControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | ES_LEFT | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | ES_AUTOHSCROLL;
	const DWORD inputControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT;
	const DWORD buttonControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON;
	const DWORD radioControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_AUTORADIOBUTTON;
	int currentY = MKDEF_BASE_OFFSET;
	
	// version
	MkStr verStr = L" version : " + MK_SYS_ENV.GetApplicationVersionTag().ToString();
	_CreateControl(hWnd, hInstance, L"edit", MKDEF_VERSION_EDIT_ID, verStr.GetPtr(), staticTextControlStyle,
		MkIntRect(MKDEF_BASE_OFFSET, currentY, halfWidth, MKDEF_TEXT_HEIGHT));
	
	// time state edit
	m_TimeStateHWND = _CreateControl(hWnd, hInstance, L"edit", MKDEF_TIME_STATE_EDIT_ID, L" --:--:--:--- (0)", staticTextControlStyle,
		MkIntRect(MKDEF_BASE_OFFSET * 2 + halfWidth, currentY, halfWidth, MKDEF_TEXT_HEIGHT));

	// next line
	currentY += MKDEF_TEXT_HEIGHT + MKDEF_BASE_OFFSET;

	// open log button
	_CreateControl(hWnd, hInstance, L"button", MKDEF_OPEN_LOG_BUTTON_ID, L"�ý��� �α� ���� ����", buttonControlStyle,
		MkIntRect(MKDEF_BASE_OFFSET, currentY, halfWidth, MKDEF_BUTTON_HEIGHT));

	// quit button
	_CreateControl(hWnd, hInstance, L"button", MKDEF_QUIT_BUTTON_ID, L"���ø����̼� ����", buttonControlStyle,
		MkIntRect(MKDEF_BASE_OFFSET * 2 + halfWidth, currentY, halfWidth, MKDEF_BUTTON_HEIGHT));

	// next line
	currentY += MKDEF_BUTTON_HEIGHT + MKDEF_BASE_OFFSET;

	// freeboard radio
	_CreateControl(hWnd, hInstance, L"button", MKDEF_FREEBOARD_RADIO_ID, L"Freeboard", radioControlStyle,
		MkIntRect(MKDEF_BASE_OFFSET, currentY, pentaWidth, MKDEF_BUTTON_HEIGHT));

	// system info radio
	_CreateControl(hWnd, hInstance, L"button", MKDEF_SYSTEM_RADIO_ID, L"System", radioControlStyle,
		MkIntRect(MKDEF_BASE_OFFSET * 2 + pentaWidth, currentY, pentaWidth, MKDEF_BUTTON_HEIGHT));

	// profiler radio
	_CreateControl(hWnd, hInstance, L"button", MKDEF_PROFILER_RADIO_ID, L"Profiler", radioControlStyle,
		MkIntRect(MKDEF_BASE_OFFSET * 3 + pentaWidth * 2, currentY, pentaWidth, MKDEF_BUTTON_HEIGHT));

	// thread-profiler radio
	_CreateControl(hWnd, hInstance, L"button", MKDEF_THREAD_PROF_RADIO_ID, L"Thread", radioControlStyle,
		MkIntRect(MKDEF_BASE_OFFSET * 4 + pentaWidth * 3, currentY, pentaWidth, MKDEF_BUTTON_HEIGHT));

	// drawing radio
	_CreateControl(hWnd, hInstance, L"button", MKDEF_DRAWING_RADIO_ID, L"Drawing", radioControlStyle,
		MkIntRect(MKDEF_BASE_OFFSET * 5 + pentaWidth * 4, currentY, pentaWidth, MKDEF_BUTTON_HEIGHT));

	// next line
	currentY += MKDEF_BUTTON_HEIGHT + MKDEF_BASE_OFFSET;

	// board line
	m_BoardLineHWND.Reserve(MKDEF_BOARD_LINE_COUNT);
	for (unsigned int i=0; i<eTypeCount; ++i)
	{
		m_BoardLineMsg[i].Fill(MKDEF_BOARD_LINE_COUNT);
	}

	for (unsigned int i=0; i<MKDEF_BOARD_LINE_COUNT; ++i)
	{
		// �ε��� ��ȣ�� �� �ؽ�Ʈ�ڽ�
		unsigned int currIndexID = MKDEF_BOARD_LINE_ID_BEGIN + MKDEF_BOARD_LINE_COUNT + i;
		_CreateControl(hWnd, hInstance, L"edit", currIndexID, MkStr(i).GetPtr(), staticIndexControlStyle,
			MkIntRect(MKDEF_BASE_OFFSET, currentY, MKDEF_INDEX_AREA_WIDTH, MKDEF_TEXT_HEIGHT));

		// �޽��� ��¿� �ؽ�Ʈ�ڽ�
		unsigned int currTextID = MKDEF_BOARD_LINE_ID_BEGIN + i;
		HWND boardLindHWND = _CreateControl(hWnd, hInstance, L"edit", currTextID, NULL, staticTextControlStyle,
			MkIntRect(MKDEF_BASE_OFFSET * 2 + MKDEF_INDEX_AREA_WIDTH, currentY, fullWidth - MKDEF_INDEX_AREA_WIDTH - MKDEF_BASE_OFFSET, MKDEF_TEXT_HEIGHT));

		m_BoardLineHWND.PushBack(boardLindHWND);

		// next line
		currentY += MKDEF_TEXT_HEIGHT + MKDEF_TEXT_OFFSET;
	}

	_SetBoardType(eFreeboard); // �⺻ Ÿ���� freeboard
	//_SetBoardType(eSystem); // �⺻ Ÿ���� system

	// next line
	currentY += MKDEF_BASE_OFFSET - MKDEF_TEXT_OFFSET;

	// log edit
	m_LogHWND = _CreateControl(hWnd, hInstance, L"edit", MKDEF_LOG_EDIT_ID, NULL, boardControlStyle,
		MkIntRect(MKDEF_BASE_OFFSET, currentY, fullWidth, MKDEF_LOG_BOX_HEIGHT));

	// next line
	currentY += MKDEF_LOG_BOX_HEIGHT + MKDEF_BASE_OFFSET;

	// input edit
	m_InputHWND = _CreateControl(hWnd, hInstance, L"edit", MKDEF_INPUT_EDIT_ID, NULL, inputControlStyle,
		MkIntRect(MKDEF_BASE_OFFSET, currentY, fullWidth - MKDEF_SEND_BUTTON_WIDTH - MKDEF_BASE_OFFSET, MKDEF_BUTTON_HEIGHT));
	if (m_InputHWND != NULL)
	{
		gOldInputProc = reinterpret_cast<WNDPROC>(SetWindowLong(m_InputHWND, GWL_WNDPROC, reinterpret_cast<LONG>(__InputSubProc)));
		PostMessage(m_InputHWND, EM_LIMITTEXT, static_cast<WPARAM>(MKDEF_MAX_INPUT_COUNT), NULL);
	}

	m_MessageHistory.SetUp(MKDEF_MAX_HISTORY_RECORD);

	// send button
	_CreateControl(hWnd, hInstance, L"button", MKDEF_SEND_BUTTON_ID, L"Send", buttonControlStyle,
		MkIntRect(MKDEF_CLIENT_WIDTH - MKDEF_SEND_BUTTON_WIDTH - MKDEF_BASE_OFFSET, currentY, MKDEF_SEND_BUTTON_WIDTH, MKDEF_BUTTON_HEIGHT));

	m_EventQueue.Reserve(256);

	return true;
}

void MkDevPanel::MsgToFreeboard(unsigned int index, const MkStr& msg)
{
	if (IsEnable())
	{
		m_EventQueue.RegisterEvent(_PanelEvent(eMsgToFreeboard, index, msg));
	}
}

void MkDevPanel::MsgToLog(const MkStr& msg, bool addTime)
{
	if (!IsEnable())
		return;

	m_EventQueue.RegisterEvent(_PanelEvent(eMsgToLog, (addTime) ? TRUE : FALSE, msg));

	MK_LOG_MGR.Msg(msg, addTime);
}

void MkDevPanel::ClearLogWindow(void)
{
	if (!IsEnable())
		return;

	m_EventQueue.RegisterEvent(_PanelEvent(eClearLog, 0, L""));
}

//------------------------------------------------------------------------------------------------//

void MkDevPanel::__Update(void)
{
	if (!IsEnable())
		return;

	// title bar hooker
	gDevPanelTitleBarHooker.Update();

	// event
	MkArray<_PanelEvent> eventQueue;
	if (m_EventQueue.Consume(eventQueue))
	{
		MK_INDEXING_LOOP(eventQueue, i)
		{
			const _PanelEvent& evt = eventQueue[i];
			switch (evt.eventType)
			{
			case eMsgToFreeboard:
				_SetBoardMsg(eFreeboard, evt.arg0, evt.arg1);
				break;

			case eMsgToSystem:
				_SetBoardMsg(eSystem, evt.arg0, evt.arg1);
				break;

			case eMsgToDrawing:
				_SetBoardMsg(eDrawing, evt.arg0, evt.arg1);
				break;

			case eMsgToLog:
				_PutMsgToLogBox(evt.arg1, (evt.arg0 == TRUE));
				break;

			case eClearLog:
				_ClearLogBox();
				break;

			case eChangeSnapPosition:
				m_Window.SetAlignmentPosition(static_cast<eRectAlignmentPosition>(evt.arg0));
				break;

			case eResetSnapPosition:
				m_Window.SetAlignmentPosition(m_Window.GetAlignmentPosition());
				break;

			case eQuit:
				DestroyWindow(m_Window.GetParentWindowHandle()); // �θ� ���� �޼��� ����
				break;
			}
		}
	}

	// profiling board
	_UpdateProfiling();

	// ���� �ð� ���� ���
	if (m_TimeStateHWND != NULL)
	{
		MkStr buffer;
		MK_TIME_MGR.GetCurrentTimeState(buffer);
		SetWindowText(m_TimeStateHWND, buffer.GetPtr());
	}
}

void MkDevPanel::__SetAlignmentPosition(eRectAlignmentPosition alignmentPosition)
{
	if (IsEnable())
	{
		m_EventQueue.RegisterEvent(_PanelEvent(eChangeSnapPosition, static_cast<unsigned int>(alignmentPosition), L""));
	}
}

void MkDevPanel::__ResetAlignmentPosition(void)
{
	if (IsEnable())
	{
		m_EventQueue.RegisterEvent(_PanelEvent(eResetSnapPosition, 0, L""));
	}
}

void MkDevPanel::__QuitApplication(void)
{
	m_EventQueue.RegisterEvent(_PanelEvent(eQuit, 0, L""));
}

void MkDevPanel::__StepBackMsgHistory(void)
{
	if ((m_InputHWND != NULL) && (m_CheatMsg != NULL))
	{
		MkStr newMsg = m_MessageHistory.StepBack();
		SetWindowText(m_InputHWND, newMsg.GetPtr());
	}
}

void MkDevPanel::__StepForwardMsgHistory(void)
{
	if ((m_InputHWND != NULL) && (m_CheatMsg != NULL))
	{
		MkStr newMsg;
		if (m_MessageHistory.StepForward(newMsg))
		{
			SetWindowText(m_InputHWND, newMsg.GetPtr());
		}
	}
}

void MkDevPanel::__MsgToSystemBoard(unsigned int index, const MkStr& msg)
{
	if (IsEnable())
	{
		m_EventQueue.RegisterEvent(_PanelEvent(eMsgToSystem, index, msg));
	}
}

void MkDevPanel::__MsgToDrawingBoard(unsigned int index, const MkStr& msg)
{
	if (IsEnable())
	{
		m_EventQueue.RegisterEvent(_PanelEvent(eMsgToDrawing, index, msg));
	}
}

void MkDevPanel::__CheckWndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (!IsEnable())
		return;

	switch (msg)
	{
	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
			switch (LOWORD(wParam))
			{
			case MKDEF_FREEBOARD_RADIO_ID: _SetBoardType(eFreeboard); break;
			case MKDEF_SYSTEM_RADIO_ID: _SetBoardType(eSystem); break;
			case MKDEF_PROFILER_RADIO_ID: _SetBoardType(eProfiler); break;
			case MKDEF_THREAD_PROF_RADIO_ID: _SetBoardType(eThreadProf); break;
			case MKDEF_DRAWING_RADIO_ID: _SetBoardType(eDrawing); break;
			
			case MKDEF_SEND_BUTTON_ID: _ConsumeInputMsg(); break;

			case MKDEF_OPEN_LOG_BUTTON_ID:
				MK_LOG_MGR.OpenSystemLogFile();
				break;

			case MKDEF_QUIT_BUTTON_ID: __QuitApplication(); break;
			}
			break;
		}
		break;
	}
}

LRESULT MkDevPanel::__DevPanelWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// check msg
	MK_DEV_PANEL.__CheckWndProc(msg, wParam, lParam);

	// title bar hooker
	if (gDevPanelTitleBarHooker.CheckWndProc(hWnd, msg, wParam, lParam))
		return 0;

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT MkDevPanel::__InputSubProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CHAR:
		if (wParam == VK_RETURN)
			return 0; // �Ҹ� ���ſ�
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			MK_DEV_PANEL.__CheckWndProc(WM_COMMAND, MAKELONG(MKDEF_SEND_BUTTON_ID, BN_CLICKED), NULL); // SEND ��ư �Է�
			return 0;

		case VK_UP:
			MK_DEV_PANEL.__StepBackMsgHistory();
			return 0;

		case VK_DOWN:
			MK_DEV_PANEL.__StepForwardMsgHistory();
			return 0;
		}
		break;
	}

	return (gOldInputProc == NULL) ? 0 : CallWindowProc(gOldInputProc, hWnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------------------------//

MkDevPanel::MkDevPanel() : MkSwitchedSingletonPattern<MkDevPanel>()
{
	m_Font = NULL;
	m_TimeStateHWND = NULL;
	m_CurrentBoardType = eTypeCount;
	m_LogHWND = NULL;
	m_LogLineCount = 0;
	m_InputHWND = NULL;
	m_CheatMsg = NULL;
}

MkDevPanel::MkDevPanel(bool enable) : MkSwitchedSingletonPattern<MkDevPanel>(enable)
{
	m_Font = NULL;
	m_TimeStateHWND = NULL;
	m_CurrentBoardType = eTypeCount;
	m_LogHWND = NULL;
	m_LogLineCount = 0;
	m_InputHWND = NULL;
	m_CheatMsg = NULL;
}

MkDevPanel::~MkDevPanel()
{
	m_Window.Clear();

	MK_DELETE_OBJECT(m_Font);
	MK_DELETE(m_CheatMsg);
}

//------------------------------------------------------------------------------------------------//

void MkDevPanel::_SetCheatInterface(MkCheatMessage* cheatInterface)
{
	if (m_CheatMsg != NULL)
	{
		delete m_CheatMsg;
	}

	m_CheatMsg = cheatInterface;
	if (m_CheatMsg == NULL)
	{
		m_CheatMsg = new MkCheatMessage;
	}
}

HWND MkDevPanel::_CreateControl
(HWND hWnd, HINSTANCE hInstance, const wchar_t* type, unsigned int id, const wchar_t* caption, DWORD style, const MkIntRect& rect)
{
	HWND hControl = CreateWindow
		(type, caption, style, rect.position.x, rect.position.y, rect.size.x, rect.size.y, hWnd, reinterpret_cast<HMENU>(id), hInstance, NULL);

	MK_CHECK(hControl != NULL, MkStr(type) + L"::" + MkStr(caption) + L" ��Ʈ�� ���� ����")
		return NULL;

	SendMessage(hControl, WM_SETFONT, reinterpret_cast<WPARAM>(m_Font), 0);
	return hControl;
}

void MkDevPanel::_SetBoardMsg(eBoardType boardType, unsigned int index, const MkStr& msg)
{
	if (!IsEnable())
		return;

	MK_CHECK((boardType >= eFreeboard) && (boardType < eTypeCount), L"���� Ÿ���� ������ ��� : " + MkStr(static_cast<int>(boardType)))
		return;
	MK_CHECK((index >= 0) && (index < MKDEF_BOARD_LINE_COUNT), L"���� �ε����� ������ ��� : " + MkStr(index))
		return;

	HWND hWnd = m_BoardLineHWND[index];
	if (hWnd != NULL)
	{
		MkStr oldMsg = m_BoardLineMsg[boardType][index];
		if (msg != oldMsg)
		{
			if (boardType == m_CurrentBoardType)
			{
				SetWindowText(hWnd, msg.GetPtr());
			}
			m_BoardLineMsg[boardType][index] = msg;
		}
	}
}

void MkDevPanel::_SetBoardType(eBoardType boardType)
{
	MK_CHECK((boardType >= eFreeboard) && (boardType < eTypeCount), L"���� Ÿ���� ������ ��� : " + MkStr(static_cast<int>(boardType)))
		return;

	if (boardType != m_CurrentBoardType)
	{
		// ���� ����
		MkArray< MkLockable<MkStr> >& targetMsgList = m_BoardLineMsg[boardType];
		MK_INDEXING_LOOP(targetMsgList, i)
		{
			HWND hWnd = m_BoardLineHWND[i];
			if (hWnd != NULL)
			{
				MkStr msg = targetMsgList[i];
				SetWindowText(hWnd, msg.GetPtr());
			}
		}

		CheckRadioButton(m_Window.GetWindowHandle(), MKDEF_FREEBOARD_RADIO_ID, MKDEF_DRAWING_RADIO_ID, MKDEF_FREEBOARD_RADIO_ID + static_cast<int>(boardType));
		m_CurrentBoardType = boardType;
	}
}

void MkDevPanel::_UpdateProfiling(void)
{
	if ((MkProfilingManager::InstancePtr() == NULL) || (!MK_PROF_MGR.IsEnable()))
		return;

	MkArray<MkHashStr> keyList;
	unsigned int cnt = MK_PROF_MGR.GetKeyList(keyList);
	if (cnt == 0)
		return;

	if (cnt > MKDEF_BOARD_LINE_COUNT) // �ִ� ���� ����
		cnt = MKDEF_BOARD_LINE_COUNT;

	keyList.SortInAscendingOrder();

	static const MkStr threadPrefix = MKDEF_PROFILING_PREFIX_FOR_THREAD;
	static unsigned int prefixSize = threadPrefix.GetSize();
	unsigned int threadBoardIndex = 0;
	unsigned int profilergBoardIndex = 0;
	for (unsigned int i=0; i<cnt; ++i)
	{
		const MkHashStr& currKey = keyList[i];
		if (currKey.GetString().CheckPrefix(threadPrefix))
		{
			if (MK_PROF_MGR.GetUpdated(currKey))
			{
				MkStr buffer;
				MK_PROF_MGR.GetEverageTimeStr(currKey, buffer);
				buffer.PopFront(prefixSize);

				unsigned int microsec = MK_PROF_MGR.GetEverageTime(currKey);
				if (microsec > 0.f)
				{
					float fps = 1000000.f / static_cast<float>(microsec);
					buffer += L", ";
					buffer += fps;
					buffer += L" fps";

					_SetBoardMsg(eThreadProf, threadBoardIndex, buffer);
				}
			}
			++threadBoardIndex;
		}
		else
		{
			if (MK_PROF_MGR.GetUpdated(currKey))
			{
				MkStr buffer;
				MK_PROF_MGR.GetEverageTimeStr(currKey, buffer);
				_SetBoardMsg(eProfiler, profilergBoardIndex, buffer);
			}
			++profilergBoardIndex;
		}
	}
}

void MkDevPanel::_PutMsgToLogBox(const MkStr& msg, bool addTime)
{
	if ((m_LogHWND != NULL) && (MKDEF_MAX_LOG_LINE > 0))
	{
		MkStr newMsg = msg;
		unsigned int additionalLineCount = newMsg.CountKeyword(MkStr::CRLF) + 1;
		MkStr timeBuf;
		if (addTime)
		{
			timeBuf = MK_SYS_ENV.GetCurrentSystemTime();
		}

		MkWrapInCriticalSection(m_LogCS)
		{
			m_LogMsg.Reserve(m_LogMsg.GetSize() + 2 + newMsg.GetSize() + ((addTime) ? 20 : 0));
			if (!m_LogMsg.Empty())
			{
				m_LogMsg += MkStr::CRLF;
			}
			
			if (addTime)
			{
				m_LogMsg += L"[";
				m_LogMsg += timeBuf;
				m_LogMsg += L"] ";
			}

			m_LogMsg += newMsg;
			m_LogLineCount += additionalLineCount;

			// ���μ� ����
			if (m_LogLineCount > MKDEF_MAX_LOG_LINE)
			{
				unsigned int removeLineCount = m_LogLineCount - MKDEF_MAX_LOG_LINE;
				unsigned int lfPos = 0;
				unsigned int crSize = MkStr::CRLF.GetSize();
				for (unsigned int i=0; i<removeLineCount; ++i)
				{
					lfPos = m_LogMsg.GetFirstKeywordPosition(MkArraySection(lfPos), MkStr::CRLF);
					lfPos += crSize;
				}
				m_LogMsg.PopFront(lfPos);
				m_LogLineCount = MKDEF_MAX_LOG_LINE;
			}
			
			SetWindowText(m_LogHWND, m_LogMsg.GetPtr()); // �ؽ�Ʈ �Է�
			SendMessage(m_LogHWND, EM_LINESCROLL, NULL, m_LogLineCount); // �������ٷ� Ŀ�� �̵�
		}
	}
}

void MkDevPanel::_ClearLogBox(void)
{
	if ((m_LogHWND != NULL) && (MKDEF_MAX_LOG_LINE > 0))
	{
		MkWrapInCriticalSection(m_LogCS)
		{
			m_LogMsg.Clear();
			m_LogLineCount = 0;

			SetWindowText(m_LogHWND, m_LogMsg.GetPtr());
			SendMessage(m_LogHWND, EM_LINESCROLL, NULL, m_LogLineCount);
		}
	}
}

void MkDevPanel::_ConsumeInputMsg(void)
{
	if (m_InputHWND == NULL)
		return;

	// ��ǲ ���ڿ� �߶����
	wchar_t buffer[MKDEF_MAX_INPUT_COUNT + 1] = {0, };
	GetWindowText(m_InputHWND, buffer, MKDEF_MAX_INPUT_COUNT + 1);
	SetWindowText(m_InputHWND, L"");
	MkStr inputMsg = buffer;
	if ((m_CheatMsg == NULL) || inputMsg.Empty())
		return;
	
	// ����
	MkStr resultMsg;
	if (m_CheatMsg->__ExcuteMsg(inputMsg, resultMsg))
	{
		m_MessageHistory.Record(inputMsg);
	}
	else
	{
		_PutMsgToLogBox(L"> MkDevPanel::Invalid input", true); // ����
	}

	// ��� ���
	if (!resultMsg.Empty())
	{
		_PutMsgToLogBox(resultMsg, true);
	}
}

//------------------------------------------------------------------------------------------------//

