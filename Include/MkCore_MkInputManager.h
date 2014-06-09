#ifndef __MINIKEY_CORE_MKINPUTMANAGER_H__
#define __MINIKEY_CORE_MKINPUTMANAGER_H__

//------------------------------------------------------------------------------------------------//
// global instance - input manager
//
// 입력(키, 마우스) 관리자
//
// 이벤트 기록 후 Update()시 입력상태에 대한 snap shot을 만들어 다음 Update()까지 사용
//
// 기본 입력가능 키(필요시 확장할 것)
// - 특수키 : VK_BACK, VK_TAB, VK_RETURN, VK_SHIFT, VK_CONTROL, VK_ESCAPE, VK_SPACE
// - 방향키 : VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN
// - 기능키 : VK_F1 ~ VK_F12
// - 숫자키 : 0, 1 ~ 9
// - 알파벳키 : A ~ Z
//
// 버튼 상태
// - pushing : (키/마우스) 입력되고 있는 상태
// - pressed : (키/마우스) 해당 프레임에서 처음으로 입력됨
// - released : (키/마우스) 해당 프레임에서 입력이 종료됨
// - double-clicked : (마우스) 연속 입력 중 두번째 입력시 발생. pressed를 포괄함
//
// 드래그 기능이 필요하면, http://minjang.egloos.com/1803285 참조
//
// thread-safe
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkType2.h"
#include "MkCore_MkVec2.h"
#include "MkCore_MkStr.h"
#include "MkCore_MkEventQueuePattern.h"

#define MK_INPUT_MGR MkInputManager::Instance()


class MkInputManager : public MkSingletonPattern<MkInputManager>
{
public:

	// 초기화
	// targetWindowHandle는 입력 메시지가 발생되는 윈도우 핸들
	void SetUp(HWND targetWindowHandle);

	// main window가 focus 상태일때 L/R window key 무시여부 설정
	// enable : true일 경우 무시, false일 경우 이전 상태 복원
	void IgnoreWindowKey(bool enable);

public:

	//------------------------------------------------------------------------------------------------//

	// event
	enum eInputEvent
	{
		eKeyPress = 0, // arg0 : key code
		eKeyRelease, // arg0 : key code
		eMousePress, // arg0 : button
		eMouseRelease, // arg0 : button
		eMouseDoubleClick, // arg0 : button
		eMouseWheelMove, // arg1 : delta
		eMouseMove // arg0 : 0/1(client out/in), arg1 : x, arg2 : y
	};

	typedef MkEventUnitPack3<eInputEvent, unsigned int, int, int> InputEvent;

	//------------------------------------------------------------------------------------------------//

public:

	// 이전 프레임부터 현재 프레임 갱신시까지 발생한 이벤트 리스트 반환
	unsigned int GetInputEvent(MkArray<InputEvent>& eventList);

	// 현재 프레임의 해당 키 상태 반환
	bool GetKeyPushing(unsigned int keyCode);
	bool GetKeyPressed(unsigned int keyCode);
	bool GetKeyReleased(unsigned int keyCode);

	// 현재 프레임의 마우스 클릭 상태 반환
	bool GetMouseLeftButtonPushing(void);
	bool GetMouseLeftButtonPressed(void);
	bool GetMouseLeftButtonReleased(void);
	bool GetMouseLeftButtonDoubleClicked(void);

	bool GetMouseMiddleButtonPushing(void);
	bool GetMouseMiddleButtonPressed(void);
	bool GetMouseMiddleButtonReleased(void);
	bool GetMouseMiddleButtonDoubleClicked(void);

	bool GetMouseRightButtonPushing(void);
	bool GetMouseRightButtonPressed(void);
	bool GetMouseRightButtonReleased(void);
	bool GetMouseRightButtonDoubleClicked(void);

	// 현재 프레임의 마우스 휠 변경값 반환
	int GetMouseWheelMovement(void);

	// 현재 프레임의 마우스 휠 위치 반환
	int GetMouseWheelPosition(void);

	// 현재 프레임의 마우스 client상 좌표 변위 반환
	MkInt2 GetAbsoluteMouseMovement(void);

	// 현재 프레임의 마우스 client상 좌표 반환 (0 <= x <= client width, 0 <= y <= client height)
	MkInt2 GetAbsoluteMousePosition(void);

	// 현재 프레임의 마우스 client상 좌표를 해상도에 따른 비율로 반환 (0.f <= x <= 1.f, 0.f <= y <= 1.f)
	MkVec2 GetRelativeMousePosition(void);

	// 현재 프레임의 마우스 포인터가 클라이언트 영역에 속하는지 여부 반환
	bool GetMousePointerAvailable(void);

public:

	//------------------------------------------------------------------------------------------------//
	// DEBUG
	//------------------------------------------------------------------------------------------------//

	// 등록된 모든 키코드 반환
	unsigned int GetAllAvailableKeyCode(MkArray<unsigned int>& keyCodeList);

	// key code를 문자열로 반환
	static MkStr ConvertKeyCodeToString(unsigned int keyCode);

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	void __CheckWndProc(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam);

	void __Update(void);

	HWND __GetTargetWindowHandle(void) const { return m_TargetWindowHandle; }

	inline HHOOK GetHookingHandle(void) const { return m_HookingHandle; }

	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

	MkInputManager();
	virtual ~MkInputManager();

public:

	//------------------------------------------------------------------------------------------------//

	// state
	enum eButtonState
	{
		ePushing = 0,
		ePressed,
		eReleased,
		eDoubleClicked
	};

	class _ButtonState
	{
	public:
		inline void SetState(eButtonState btnState, bool state) { m_State[btnState] = state; }
		inline bool GetState(eButtonState btnState) const { return m_State[btnState]; }

		inline bool Pressed(void)
		{
			bool ok = !GetState(ePushing);
			if (ok)
			{
				SetState(ePushing, true);
				SetState(ePressed, true);
			}
			return ok;
		}

		inline void Released(void)
		{
			SetState(ePushing, false);
			SetState(eReleased, true);
		}

		inline void DoubleClicked(void)
		{
			Pressed();
			SetState(eDoubleClicked, true);
		}

		inline void Reset(void)
		{
			SetState(ePressed, false);
			SetState(eReleased, false);
			SetState(eDoubleClicked, false);
		}

		_ButtonState()
		{
			SetState(ePushing, false);
			Reset();
		}

	protected:
		bool m_State[4];
	};

	//------------------------------------------------------------------------------------------------//

	bool _GetCurrentMousePositionInClient(MkInt2& buffer) const;

	bool _GetCurrentKeyState(unsigned int keyCode, eButtonState btnState);
	bool _GetCurrentMouseState(unsigned int button, eButtonState btnState);

	void _RegisterKeyEvent(eInputEvent inputEvent, WPARAM wParam);

	//------------------------------------------------------------------------------------------------//

protected:

	// target window
	HWND m_TargetWindowHandle;

	// 후킹 핸들
	HHOOK m_HookingHandle;

	// event
	MkEventQueuePattern<InputEvent> m_EventQueue;

	// state snap shot
	MkCriticalSection m_SnapShotCS;
	MkArray<InputEvent> m_CurrentFrameEvents;
	MkHashMap<unsigned int, _ButtonState> m_KeyState;
	_ButtonState m_MouseState[3];
	int m_WheelMovement;
	int m_WheelPosition;
	MkInt2 m_MouseMovement;
	MkInt2 m_MousePosition;
	bool m_AvailablePoint;
};

//------------------------------------------------------------------------------------------------//

MKDEF_DECLARE_FIXED_SIZE_TYPE(MkInputManager::InputEvent);

//------------------------------------------------------------------------------------------------//

#endif
