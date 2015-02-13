
#ifndef _WIN32_WINNT // Windows XP 이상에서 WM_MOUSEWHEEL을 사용하기 위해
#define _WIN32_WINNT 0x0501
#endif

#include "MkCore_MkProjectDefinition.h"
#include "MkCore_MkCheck.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkInputManager.h"

#define MKDEF_MAX_KEY_COUNT 60 // 사용가능한 최대 키 갯수

#define MKDEF_MOUSE_LEFT_BUTTON 0
#define MKDEF_MOUSE_MIDDLE_BUTTON 1
#define MKDEF_MOUSE_RIGHT_BUTTON 2


//------------------------------------------------------------------------------------------------//

void MkInputManager::SetUp(HWND targetWindowHandle)
{
	MK_CHECK(targetWindowHandle != NULL, L"MkInputManager에 NULL window로 초기화 시도")
		return;

	m_TargetWindowHandle = targetWindowHandle;
	UpdateTargetWindowClientSize(targetWindowHandle);

	MkArray<unsigned int> keyCodeList;
	keyCodeList.Reserve(256);
	
	// 특수키 등록
	keyCodeList.PushBack(VK_BACK);
	keyCodeList.PushBack(VK_TAB);
	keyCodeList.PushBack(VK_RETURN);
	keyCodeList.PushBack(VK_SHIFT);
	keyCodeList.PushBack(VK_CONTROL);
	keyCodeList.PushBack(VK_ESCAPE);
	keyCodeList.PushBack(VK_SPACE);

	// 방향키 등록
	keyCodeList.PushBack(VK_LEFT);
	keyCodeList.PushBack(VK_UP);
	keyCodeList.PushBack(VK_RIGHT);
	keyCodeList.PushBack(VK_DOWN);

	// 기능키 등록
	keyCodeList.PushBack(VK_F1);
	keyCodeList.PushBack(VK_F2);
	keyCodeList.PushBack(VK_F3);
	keyCodeList.PushBack(VK_F4);
	keyCodeList.PushBack(VK_F5);
	keyCodeList.PushBack(VK_F6);
	keyCodeList.PushBack(VK_F7);
	keyCodeList.PushBack(VK_F8);
	keyCodeList.PushBack(VK_F9);
	keyCodeList.PushBack(VK_F10);
	keyCodeList.PushBack(VK_F11);
	keyCodeList.PushBack(VK_F12);

	// 숫자 등록(0 ~ 9)
	for (unsigned int i=0x30; i<0x3A; ++i)
	{
		keyCodeList.PushBack(i);
	}

	// 영문 등록(A ~ Z)
	for (unsigned int i=0x41; i<0x5B; ++i)
	{
		keyCodeList.PushBack(i);
	}

	// 이벤트 큐
	m_EventQueue.Reserve(64);

	// snap shot 생성
	MkWrapInCriticalSection(m_SnapShotCS)
	{
		MK_INDEXING_LOOP(keyCodeList, i)
		{
			unsigned int keyCode = keyCodeList[i];
			if (!m_KeyState.Exist(keyCode))
			{
				m_KeyState.Create(keyCode);
			}
		}
		m_KeyState.Rehash();

		m_WheelMovement = 0;
		m_WheelPosition = 0;
		m_AvailablePoint = _GetCurrentMousePositionInClient(m_MousePosition);
	}
}

void MkInputManager::UpdateTargetWindowClientSize(HWND targetHWND)
{
	if (targetHWND == NULL)
	{
		targetHWND = m_TargetWindowHandle;
	}

	if ((m_TargetWindowHandle != NULL) && (targetHWND == m_TargetWindowHandle))
	{
		RECT rect;
		GetClientRect(m_TargetWindowHandle, &rect);
		m_TargetWindowClientSize.x = static_cast<int>(rect.right);
		m_TargetWindowClientSize.y = static_cast<int>(rect.bottom);
	}
}

void MkInputManager::IgnoreWindowKey(bool enable)
{
	if (enable)
	{
		if (m_HookingHandle == NULL)
		{
			m_HookingHandle = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
		}
	}
	else
	{
		if (m_HookingHandle != NULL)
		{
			UnhookWindowsHookEx(m_HookingHandle);
			m_HookingHandle = NULL;
		}
	}
}

unsigned int MkInputManager::GetInputEvent(MkArray<InputEvent>& eventList)
{
	MkScopedCriticalSection(m_SnapShotCS);
	if (m_CurrentFrameEvents.Empty())
		return 0;

	eventList = m_CurrentFrameEvents;
	return eventList.GetSize();
}

bool MkInputManager::GetKeyPushing(unsigned int keyCode) { return _GetCurrentKeyState(keyCode, ePushing); }
bool MkInputManager::GetKeyPressed(unsigned int keyCode) { return _GetCurrentKeyState(keyCode, ePressed); }
bool MkInputManager::GetKeyReleased(unsigned int keyCode) { return _GetCurrentKeyState(keyCode, eReleased); }

bool MkInputManager::GetMouseLeftButtonPushing(void) { return _GetCurrentMouseState(MKDEF_MOUSE_LEFT_BUTTON, ePushing); }
bool MkInputManager::GetMouseLeftButtonPressed(void) { return _GetCurrentMouseState(MKDEF_MOUSE_LEFT_BUTTON, ePressed); }
bool MkInputManager::GetMouseLeftButtonReleased(void) { return _GetCurrentMouseState(MKDEF_MOUSE_LEFT_BUTTON, eReleased); }
bool MkInputManager::GetMouseLeftButtonDoubleClicked(void) { return _GetCurrentMouseState(MKDEF_MOUSE_LEFT_BUTTON, eDoubleClicked); }

bool MkInputManager::GetMouseMiddleButtonPushing(void) { return _GetCurrentMouseState(MKDEF_MOUSE_MIDDLE_BUTTON, ePushing); }
bool MkInputManager::GetMouseMiddleButtonPressed(void) { return _GetCurrentMouseState(MKDEF_MOUSE_MIDDLE_BUTTON, ePressed); }
bool MkInputManager::GetMouseMiddleButtonReleased(void) { return _GetCurrentMouseState(MKDEF_MOUSE_MIDDLE_BUTTON, eReleased); }
bool MkInputManager::GetMouseMiddleButtonDoubleClicked(void) { return _GetCurrentMouseState(MKDEF_MOUSE_MIDDLE_BUTTON, eDoubleClicked); }

bool MkInputManager::GetMouseRightButtonPushing(void) { return _GetCurrentMouseState(MKDEF_MOUSE_RIGHT_BUTTON, ePushing); }
bool MkInputManager::GetMouseRightButtonPressed(void) { return _GetCurrentMouseState(MKDEF_MOUSE_RIGHT_BUTTON, ePressed); }
bool MkInputManager::GetMouseRightButtonReleased(void) { return _GetCurrentMouseState(MKDEF_MOUSE_RIGHT_BUTTON, eReleased); }
bool MkInputManager::GetMouseRightButtonDoubleClicked(void) { return _GetCurrentMouseState(MKDEF_MOUSE_RIGHT_BUTTON, eDoubleClicked); }

int MkInputManager::GetMouseWheelMovement(void)
{
	MkScopedCriticalSection(m_SnapShotCS);
	return m_WheelMovement;
}

int MkInputManager::GetMouseWheelPosition(void)
{
	MkScopedCriticalSection(m_SnapShotCS);
	return m_WheelPosition;
}

MkInt2 MkInputManager::GetAbsoluteMouseMovement(bool flipY)
{
	MkInt2 pos;
	MkWrapInCriticalSection(m_SnapShotCS)
	{
		pos = m_MouseMovement;
	}

	if (flipY)
	{
		pos.y = -pos.y;
	}
	return pos;
}

MkInt2 MkInputManager::GetAbsoluteMousePosition(bool flipY)
{
	MkInt2 pos;
	MkWrapInCriticalSection(m_SnapShotCS)
	{
		pos = m_MousePosition;
	}

	if (flipY && (!m_TargetWindowClientSize.IsZero()))
	{
		pos.y = m_TargetWindowClientSize.y - pos.y;
	}
	return pos;
}

MkVec2 MkInputManager::GetRelativeMousePosition(bool flipY)
{
	if (m_TargetWindowClientSize.IsZero())
		return MkVec2::Zero;

	MkInt2 absPosition = GetAbsoluteMousePosition(flipY);
	return MkVec2(static_cast<float>(absPosition.x) / static_cast<float>(m_TargetWindowClientSize.x),
		static_cast<float>(absPosition.y) / static_cast<float>(m_TargetWindowClientSize.y));
}

bool MkInputManager::GetMousePointerAvailable(void)
{
	MkScopedCriticalSection(m_SnapShotCS);
	return m_AvailablePoint;
}

unsigned int MkInputManager::GetAllAvailableKeyCode(MkArray<unsigned int>& keyCodeList)
{
	MkScopedCriticalSection(m_SnapShotCS);
	return m_KeyState.GetKeyList(keyCodeList);
}

MkStr MkInputManager::ConvertKeyCodeToString(unsigned int keyCode)
{
	switch (keyCode)
	{
	case VK_BACK: return L"VK_BACK";
	case VK_TAB: return L"VK_TAB";
	case VK_RETURN: return L"VK_RETURN";
	case VK_SHIFT: return L"VK_SHIFT";
	case VK_CONTROL: return L"VK_CONTROL";
	case VK_ESCAPE: return L"VK_ESCAPE";
	case VK_SPACE: return L"VK_SPACE";

	case VK_LEFT: return L"VK_LEFT";
	case VK_UP: return L"VK_UP";
	case VK_RIGHT: return L"VK_RIGHT";
	case VK_DOWN: return L"VK_DOWN";

	case VK_F1: return L"VK_F1";
	case VK_F2: return L"VK_F2";
	case VK_F3: return L"VK_F3";
	case VK_F4: return L"VK_F4";
	case VK_F5: return L"VK_F5";
	case VK_F6: return L"VK_F6";
	case VK_F7: return L"VK_F7";
	case VK_F8: return L"VK_F8";
	case VK_F9: return L"VK_F9";
	case VK_F10: return L"VK_F10";
	case VK_F11: return L"VK_F11";
	case VK_F12: return L"VK_F12";

	case 0x30: return L"0";
	case 0x31: return L"1";
	case 0x32: return L"2";
	case 0x33: return L"3";
	case 0x34: return L"4";
	case 0x35: return L"5";
	case 0x36: return L"6";
	case 0x37: return L"7";
	case 0x38: return L"8";
	case 0x39: return L"9";

	case 0x41: return L"A";
	case 0x42: return L"B";
	case 0x43: return L"C";
	case 0x44: return L"D";
	case 0x45: return L"E";
	case 0x46: return L"F";
	case 0x47: return L"G";
	case 0x48: return L"H";
	case 0x49: return L"I";
	case 0x4A: return L"J";
	case 0x4B: return L"K";
	case 0x4C: return L"L";
	case 0x4D: return L"M";
	case 0x4E: return L"N";
	case 0x4F: return L"O";
	case 0x50: return L"P";
	case 0x51: return L"Q";
	case 0x52: return L"R";
	case 0x53: return L"S";
	case 0x54: return L"T";
	case 0x55: return L"U";
	case 0x56: return L"V";
	case 0x57: return L"W";
	case 0x58: return L"X";
	case 0x59: return L"Y";
	case 0x5A: return L"Z";
	}
	return L"";
}

//------------------------------------------------------------------------------------------------//

void MkInputManager::__CheckWndProc(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
	if ((m_TargetWindowHandle == NULL) || (hWnd != m_TargetWindowHandle))
		return;

	switch (msg)
	{
	case WM_KEYDOWN:
		_RegisterKeyEvent(eKeyPress, wParam);
		break;
	case WM_KEYUP:
		_RegisterKeyEvent(eKeyRelease, wParam);
		break;

	case WM_LBUTTONDOWN:
		m_EventQueue.RegisterEvent(InputEvent(eMousePress, MKDEF_MOUSE_LEFT_BUTTON, static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam))));
		break;
	case WM_LBUTTONUP:
		m_EventQueue.RegisterEvent(InputEvent(eMouseRelease, MKDEF_MOUSE_LEFT_BUTTON, static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam))));
		break;
	case WM_LBUTTONDBLCLK:
		m_EventQueue.RegisterEvent(InputEvent(eMouseDoubleClick, MKDEF_MOUSE_LEFT_BUTTON, static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam))));
		break;

	case WM_MBUTTONDOWN:
		m_EventQueue.RegisterEvent(InputEvent(eMousePress, MKDEF_MOUSE_MIDDLE_BUTTON, static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam))));
		break;
	case WM_MBUTTONUP:
		m_EventQueue.RegisterEvent(InputEvent(eMouseRelease, MKDEF_MOUSE_MIDDLE_BUTTON, static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam))));
		break;
	case WM_MBUTTONDBLCLK:
		m_EventQueue.RegisterEvent(InputEvent(eMouseDoubleClick, MKDEF_MOUSE_MIDDLE_BUTTON, static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam))));
		break;

	case WM_RBUTTONDOWN:
		m_EventQueue.RegisterEvent(InputEvent(eMousePress, MKDEF_MOUSE_RIGHT_BUTTON, static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam))));
		break;
	case WM_RBUTTONUP:
		m_EventQueue.RegisterEvent(InputEvent(eMouseRelease, MKDEF_MOUSE_RIGHT_BUTTON, static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam))));
		break;
	case WM_RBUTTONDBLCLK:
		m_EventQueue.RegisterEvent(InputEvent(eMouseDoubleClick, MKDEF_MOUSE_RIGHT_BUTTON, static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam))));
		break;

	case WM_MOUSEWHEEL:
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			ScreenToClient(m_TargetWindowHandle, &pt);
			m_EventQueue.RegisterEvent(InputEvent(eMouseWheelMove, static_cast<int>(GET_WHEEL_DELTA_WPARAM(wParam)), static_cast<int>(pt.x), static_cast<int>(pt.y)));
		}
		break;

	case WM_MOUSEMOVE:
		m_EventQueue.RegisterEvent(InputEvent(eMouseMove, 0, static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam))));
		break;
	}
}

void MkInputManager::__Update(void)
{
	// 이벤트큐 복사
	MkArray<InputEvent> eventQueue;
	m_EventQueue.Consume(eventQueue);

	// 포인터 위치로 클라이언트 영역 소속여부 및 좌표 판단
	MkInt2 newMousePosition;
	bool availablePoint = _GetCurrentMousePositionInClient(newMousePosition);

	MkWrapInCriticalSection(m_SnapShotCS)
	{
		// 이전 snap shot 초기화
		m_CurrentFrameEvents.Flush();
		m_CurrentFrameEvents.Reserve(eventQueue.GetSize() + 1);

		MkHashMapLooper<unsigned int, _ButtonState> looper(m_KeyState);
		MK_STL_LOOP(looper)
		{
			looper.GetCurrentField().Reset();
		}
		m_MouseState[MKDEF_MOUSE_LEFT_BUTTON].Reset();
		m_MouseState[MKDEF_MOUSE_MIDDLE_BUTTON].Reset();
		m_MouseState[MKDEF_MOUSE_RIGHT_BUTTON].Reset();
		m_WheelMovement = 0;

		// 마우스 위치 관련 갱신
		m_AvailablePoint = availablePoint;
		if (newMousePosition != m_MousePosition)
		{
			m_MouseMovement = newMousePosition - m_MousePosition;
			m_MousePosition = newMousePosition;
			m_CurrentFrameEvents.PushBack(InputEvent(eMouseMove, (m_AvailablePoint) ? 1 : 0, m_MousePosition.x, m_MousePosition.y));
		}
		else
		{
			m_MouseMovement.Clear();

			if (MKDEF_GEN_MOUSE_MOVE_EVENT_ALWAYS)
			{
				m_CurrentFrameEvents.PushBack(InputEvent(eMouseMove, (m_AvailablePoint) ? 1 : 0, m_MousePosition.x, m_MousePosition.y));
			}
		}
		
		// 이벤트 처리
		if (!eventQueue.Empty())
		{
			MK_INDEXING_LOOP(eventQueue, i)
			{
				const InputEvent& evt = eventQueue[i];
				switch (evt.eventType)
				{
				case eKeyPress:
					if (m_KeyState[evt.arg0].Pressed())
					{
						m_CurrentFrameEvents.PushBack(evt); // 유효할때만 기록
					}
					break;
				case eKeyRelease:
					m_KeyState[evt.arg0].Released();
					m_CurrentFrameEvents.PushBack(evt);
					break;

				case eMousePress:
					m_MouseState[evt.arg0].Pressed();
					m_CurrentFrameEvents.PushBack(evt);
					break;
				case eMouseRelease:
					m_MouseState[evt.arg0].Released();
					m_CurrentFrameEvents.PushBack(evt);
					break;
				case eMouseDoubleClick:
					m_MouseState[evt.arg0].DoubleClicked();
					m_CurrentFrameEvents.PushBack(evt);
					break;

				case eMouseWheelMove:
					m_WheelMovement += evt.arg0;
					m_CurrentFrameEvents.PushBack(evt);
					break;

				case eMouseMove: // mouse position에 대한 사항은 상위에서 별도 처리
					break;
				}
			}

			m_WheelPosition += m_WheelMovement;

			if (MKDEF_SHOW_INPUT_EVENT)
			{
				MK_INDEXING_LOOP(m_CurrentFrameEvents, i)
				{
					const InputEvent& evt = m_CurrentFrameEvents[i];
					switch (evt.eventType)
					{
					case eKeyPress:
						MK_DEV_PANEL.MsgToLog(L"   - input evt eKeyPress : " + ConvertKeyCodeToString(evt.arg0));
						break;
					case eKeyRelease:
						MK_DEV_PANEL.MsgToLog(L"   - input evt eKeyRelease : " + ConvertKeyCodeToString(evt.arg0));
						break;
					case eMousePress:
						MK_DEV_PANEL.MsgToLog(L"   - input evt eMousePress : btn(" + MkStr(evt.arg0) + L"), pos(" + MkStr(evt.arg1) + L", " + MkStr(evt.arg2) + L")");
						break;
					case eMouseRelease:
						MK_DEV_PANEL.MsgToLog(L"   - input evt eMouseRelease : btn(" + MkStr(evt.arg0) + L"), pos(" + MkStr(evt.arg1) + L", " + MkStr(evt.arg2) + L")");
						break;
					case eMouseDoubleClick:
						MK_DEV_PANEL.MsgToLog(L"   - input evt eMouseDoubleClick : btn(" + MkStr(evt.arg0) + L"), pos(" + MkStr(evt.arg1) + L", " + MkStr(evt.arg2) + L")");
						break;
					case eMouseWheelMove:
						MK_DEV_PANEL.MsgToLog(L"   - input evt eMouseWheelMove : delta(" + MkStr(evt.arg0) + L"), pos(" + MkStr(evt.arg1) + L", " + MkStr(evt.arg2) + L")");
						break;
					case eMouseMove:
						MK_DEV_PANEL.MsgToLog(L"   - input evt eMouseMove : inside(" + MkStr(evt.arg0 == 1) + L"), pos(" + MkStr(evt.arg1) + L", " + MkStr(evt.arg2) + L")");
						break;
					}
				}
			}
		}
	}
}

LRESULT MkInputManager::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		HWND hWnd = MK_INPUT_MGR.__GetTargetWindowHandle();
		if (hWnd != NULL)
		{
			if (GetFocus() == hWnd)
			{
				switch (wParam)
				{
				case WM_KEYDOWN:
				case WM_KEYUP:
					{
						KBDLLHOOKSTRUCT* p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
						assert(p != NULL);
						if ((p->vkCode == VK_LWIN) || (p->vkCode == VK_RWIN))
							return TRUE;
					}
					break;
				}
			}
		}
	}

	return CallNextHookEx(MK_INPUT_MGR.GetHookingHandle(), nCode, wParam, lParam);
}

MkInputManager::MkInputManager() : MkSingletonPattern<MkInputManager>()
{
	m_TargetWindowHandle = NULL;
	m_HookingHandle = NULL;
}

MkInputManager::~MkInputManager()
{
	IgnoreWindowKey(false);
}

//------------------------------------------------------------------------------------------------//

bool MkInputManager::_GetCurrentMousePositionInClient(MkInt2& buffer) const
{
	if (m_TargetWindowHandle == NULL)
		return false;

	POINT mousePoint;
	if (GetCursorPos(&mousePoint) != FALSE)
	{
		if (ScreenToClient(m_TargetWindowHandle, &mousePoint) != FALSE)
		{
			RECT rect;
			GetClientRect(m_TargetWindowHandle, &rect);
			int w = static_cast<int>(rect.right);
			int h = static_cast<int>(rect.bottom);
			buffer.x = mousePoint.x;
			buffer.y = mousePoint.y;

			bool xUnder = (mousePoint.x < 0);
			if (xUnder)
				buffer.x = 0;

			bool xOver = (mousePoint.x > w);
			if (xOver)
				buffer.x = w;

			bool yUnder = (mousePoint.y < 0);
			if (yUnder)
				buffer.y = 0;

			bool yOver = (mousePoint.y > h);
			if (yOver)
				buffer.y = h;

			return (xUnder || xOver || yUnder || yOver) ? false : true;
		}
	}
	return false;
}

bool MkInputManager::_GetCurrentKeyState(unsigned int keyCode, eButtonState btnState)
{
	MkScopedCriticalSection(m_SnapShotCS);
	return m_KeyState.Exist(keyCode) ? m_KeyState[keyCode].GetState(btnState) : false;
}

bool MkInputManager::_GetCurrentMouseState(unsigned int button, eButtonState btnState)
{
	MkScopedCriticalSection(m_SnapShotCS);
	return (button < 3) ? m_MouseState[button].GetState(btnState) : false;
}

void MkInputManager::_RegisterKeyEvent(eInputEvent inputEvent, WPARAM wParam)
{
	unsigned int keyCode = static_cast<unsigned int>(wParam);
	bool existKey = false;
	MkWrapInCriticalSection(m_SnapShotCS)
	{
		existKey = m_KeyState.Exist(keyCode);
	}

	if (existKey)
	{
		m_EventQueue.RegisterEvent(InputEvent(inputEvent, static_cast<int>(keyCode), 0, 0));
	}
}

//------------------------------------------------------------------------------------------------//
