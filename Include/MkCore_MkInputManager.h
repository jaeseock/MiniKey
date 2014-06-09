#ifndef __MINIKEY_CORE_MKINPUTMANAGER_H__
#define __MINIKEY_CORE_MKINPUTMANAGER_H__

//------------------------------------------------------------------------------------------------//
// global instance - input manager
//
// �Է�(Ű, ���콺) ������
//
// �̺�Ʈ ��� �� Update()�� �Է»��¿� ���� snap shot�� ����� ���� Update()���� ���
//
// �⺻ �Է°��� Ű(�ʿ�� Ȯ���� ��)
// - Ư��Ű : VK_BACK, VK_TAB, VK_RETURN, VK_SHIFT, VK_CONTROL, VK_ESCAPE, VK_SPACE
// - ����Ű : VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN
// - ���Ű : VK_F1 ~ VK_F12
// - ����Ű : 0, 1 ~ 9
// - ���ĺ�Ű : A ~ Z
//
// ��ư ����
// - pushing : (Ű/���콺) �Էµǰ� �ִ� ����
// - pressed : (Ű/���콺) �ش� �����ӿ��� ó������ �Էµ�
// - released : (Ű/���콺) �ش� �����ӿ��� �Է��� �����
// - double-clicked : (���콺) ���� �Է� �� �ι�° �Է½� �߻�. pressed�� ������
//
// �巡�� ����� �ʿ��ϸ�, http://minjang.egloos.com/1803285 ����
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

	// �ʱ�ȭ
	// targetWindowHandle�� �Է� �޽����� �߻��Ǵ� ������ �ڵ�
	void SetUp(HWND targetWindowHandle);

	// main window�� focus �����϶� L/R window key ���ÿ��� ����
	// enable : true�� ��� ����, false�� ��� ���� ���� ����
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

	// ���� �����Ӻ��� ���� ������ ���Žñ��� �߻��� �̺�Ʈ ����Ʈ ��ȯ
	unsigned int GetInputEvent(MkArray<InputEvent>& eventList);

	// ���� �������� �ش� Ű ���� ��ȯ
	bool GetKeyPushing(unsigned int keyCode);
	bool GetKeyPressed(unsigned int keyCode);
	bool GetKeyReleased(unsigned int keyCode);

	// ���� �������� ���콺 Ŭ�� ���� ��ȯ
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

	// ���� �������� ���콺 �� ���氪 ��ȯ
	int GetMouseWheelMovement(void);

	// ���� �������� ���콺 �� ��ġ ��ȯ
	int GetMouseWheelPosition(void);

	// ���� �������� ���콺 client�� ��ǥ ���� ��ȯ
	MkInt2 GetAbsoluteMouseMovement(void);

	// ���� �������� ���콺 client�� ��ǥ ��ȯ (0 <= x <= client width, 0 <= y <= client height)
	MkInt2 GetAbsoluteMousePosition(void);

	// ���� �������� ���콺 client�� ��ǥ�� �ػ󵵿� ���� ������ ��ȯ (0.f <= x <= 1.f, 0.f <= y <= 1.f)
	MkVec2 GetRelativeMousePosition(void);

	// ���� �������� ���콺 �����Ͱ� Ŭ���̾�Ʈ ������ ���ϴ��� ���� ��ȯ
	bool GetMousePointerAvailable(void);

public:

	//------------------------------------------------------------------------------------------------//
	// DEBUG
	//------------------------------------------------------------------------------------------------//

	// ��ϵ� ��� Ű�ڵ� ��ȯ
	unsigned int GetAllAvailableKeyCode(MkArray<unsigned int>& keyCodeList);

	// key code�� ���ڿ��� ��ȯ
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

	// ��ŷ �ڵ�
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
