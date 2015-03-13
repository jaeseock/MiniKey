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

#include "MkCore_MkGlobalDefinition.h"
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

	// targetHWND�� SetUp()���� ������ targetWindow�� ��� client ���� ũ�� ���� ��û
	void UpdateTargetWindowClientSize(HWND targetHWND);

	// SetUp()���� ������ targetWindow client ���� ũ�� ��ȯ
	inline const MkInt2& GetTargetWindowClientSize(void) const { return m_TargetWindowClientSize; }

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
		eMousePress, // arg0 : button, arg1 : x, arg2 : y
		eMouseRelease, // arg0 : button, arg1 : x, arg2 : y
		eMouseDoubleClick, // arg0 : button, arg1 : x, arg2 : y
		eMouseWheelMove, // arg0 : delta, arg1 : x, arg2 : y
		eMouseMove // arg0 : 0/1(client out/in), arg1 : x, arg2 : y
	};

	typedef MkEventUnitPack3<eInputEvent, int, int, int> InputEvent;

	//------------------------------------------------------------------------------------------------//

public:

	// ���� �����Ӻ��� ���� ������ ���Žñ��� �߻��� �̺�Ʈ ����Ʈ ��ȯ
	unsigned int GetInputEvent(MkArray<InputEvent>& eventList);

	// ���� �������� �ش� Ű ���� ��ȯ. ����(A~Z)�� �빮�ڸ� �ν�
	bool GetKeyPushing(unsigned int keyCode);
	bool GetKeyPressed(unsigned int keyCode);
	bool GetKeyReleased(unsigned int keyCode);

	// ���� �������� ���콺 Ŭ�� ���� ��ȯ
	bool GetMouseLeftButtonPushing(void);
	bool GetMouseLeftButtonPressed(void);
	bool GetMouseLeftButtonReleased(void);
	bool GetMouseLeftButtonDoubleClicked(void);
	eCursorState GetMouseLeftButtonState(void);

	bool GetMouseMiddleButtonPushing(void);
	bool GetMouseMiddleButtonPressed(void);
	bool GetMouseMiddleButtonReleased(void);
	bool GetMouseMiddleButtonDoubleClicked(void);
	eCursorState GetMouseMiddleButtonState(void);

	bool GetMouseRightButtonPushing(void);
	bool GetMouseRightButtonPressed(void);
	bool GetMouseRightButtonReleased(void);
	bool GetMouseRightButtonDoubleClicked(void);
	eCursorState GetMouseRightButtonState(void);

	// ���� �������� ���콺 �� ���氪 ��ȯ
	int GetMouseWheelMovement(void);

	// ���� �������� ���콺 �� ��ġ ��ȯ
	int GetMouseWheelPosition(void);

	// ���� �������� ���콺 client�� ��ǥ ���� ��ȯ
	MkInt2 GetAbsoluteMouseMovement(bool flipY = false);

	// ���� �������� ���콺 client�� ��ǥ ��ȯ (0 <= x <= client width, 0 <= y <= client height)
	MkInt2 GetAbsoluteMousePosition(bool flipY = false);

	// ���� �������� ���콺 client�� ��ǥ�� �ػ󵵿� ���� ������ ��ȯ (0.f <= x <= 1.f, 0.f <= y <= 1.f)
	MkVec2 GetRelativeMousePosition(bool flipY = false);

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

	class _ButtonState
	{
	public:
		inline void SetState(eCursorState btnState, bool state) { m_State[btnState] = state; }
		inline bool GetState(eCursorState btnState) const { return m_State[btnState]; }

		inline bool Pressed(void)
		{
			bool ok = !GetState(eCS_Pushing);
			if (ok)
			{
				SetState(eCS_Pushing, true);
				SetState(eCS_Pressed, true);
			}
			return ok;
		}

		inline void Released(void)
		{
			SetState(eCS_Pushing, false);
			SetState(eCS_Released, true);
		}

		inline void DoubleClicked(void)
		{
			Pressed();
			SetState(eCS_DoubleClicked, true);
		}

		inline void Reset(void)
		{
			SetState(eCS_Pressed, false);
			SetState(eCS_Released, false);
			SetState(eCS_DoubleClicked, false);
		}

		_ButtonState()
		{
			SetState(eCS_Pushing, false);
			Reset();
		}

	protected:
		bool m_State[eCS_Max];
	};

	//------------------------------------------------------------------------------------------------//

	bool _GetCurrentMousePositionInClient(MkInt2& buffer) const;

	bool _GetCurrentKeyState(unsigned int keyCode, eCursorState btnState);
	bool _GetCurrentMouseState(unsigned int button, eCursorState btnState);
	eCursorState _GetCurrentMouseState(unsigned int button);

	void _RegisterKeyEvent(eInputEvent inputEvent, WPARAM wParam);

	//------------------------------------------------------------------------------------------------//

protected:

	// target window
	HWND m_TargetWindowHandle;

	// target window size
	MkInt2 m_TargetWindowClientSize;

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
