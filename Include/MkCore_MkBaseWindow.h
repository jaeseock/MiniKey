#ifndef __MINIKEY_CORE_MKBASEWINDOW_H__
#define __MINIKEY_CORE_MKBASEWINDOW_H__

//------------------------------------------------------------------------------------------------//
// win32 윈도우
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include "MkCore_MkGlobalDefinition.h"
#include "MkCore_MkStr.h"


class MkBaseWindow
{
public:

	// 윈도우 생성으로 초기화
	// clientSize의 xy가 0 이하이면 워크스페이스(바탕화면에서 작업표시줄이 제외된 영역) 크기로 생성
	// ex>
	//	window.SetUpByWindowCreation(hI, WndProc, NULL, L"normal window", eSWP_All, MkInt2(CW_USEDEFAULT, CW_USEDEFAULT), MkInt2(640, 480));
	//	window.SetUpByWindowCreation(hI, WndProc, NULL, L"full window", eSWP_Close, MkInt2::Zero, MkInt2::Zero);
	virtual bool SetUpByWindowCreation
		(HINSTANCE hInstance, WNDPROC wndProc, HWND hParent, const MkStr& title, eSystemWindowProperty sysWinProp,
		const MkInt2& position, const MkInt2& clientSize, bool fullScreen = false, bool hide = false);

	// 외부에서 생성된 윈도우로 초기화
	virtual bool SetUpByOuterWindow(HWND hWnd);

	// 윈도우 위치 설정
	virtual bool SetPosition(const MkInt2& position);

	// 윈도우 위치 반환
	MkInt2 GetPosition(void) const;

	// 윈도우 크기 재설정
	virtual bool SetWindowSize(const MkInt2& size);

	// 윈도우 크기 반환
	MkInt2 GetWindowSize(void) const;

	// 클라이언트 크기를 고정해 윈도우 크기 재설정
	// resetStyle이 true일 경우 스타일 재설정
	virtual bool SetClientSize(const MkInt2& size);

	// 클라이언트 크기 반환
	MkInt2 GetClientSize(void) const;

	// 포커스 설정
	virtual bool SetOnTop(void);

	// 포커스여부 반환
	bool GetOnTop(void) const;

	// 보이기 상태 설정
	virtual bool SetShowCmd(unsigned int showCmd);

	// 보이기 상태 반환
	unsigned int GetShowCmd(void) const;

	// SW_SHOWMINIMIZED 상태에서 윈도우가 복원되었을 때 MkTitleBarHooker로부터 호출되는 콜백 펑션
	// 해당 펑션 호출 이전 다른 변경은 모두 완료된 상태(position, size 등)
	virtual void WindowRestored(void) {}

	// 윈도우 클래스 해제
	virtual void Clear(void);

	// 윈도우 타이틀 변경
	virtual void SetWindowTitle(const MkStr& title);

	// 현재 적용중인 윈도우 스타일 반환
	DWORD GetCurrentWindowStyle(void) const;

	// window mode시의 스타일 반환
	inline DWORD GetWindowModeStyle(void) const { return m_WindowModeStyle; }

	// window mode 스타일로 전환
	void ChangeToWindowModeStyle(void);

	// full mode 스타일로 전환
	void ChangeToFullModeStyle(void);

	// clientSize가 고정된 윈도우 크기를 반환
	MkInt2 ConvertClientToWindowSize(const MkInt2& clientSize) const;

	// 윈도우 크기로부터 clientSize를 계산해 반환
	MkInt2 ConvertWindowToClientSize(const MkInt2& windowSize) const;

	// 윈도우 핸들 반환
	inline HWND GetWindowHandle(void) const { return m_hWnd; }

	// 부모 윈도우 핸들 반환
	inline HWND GetParentWindowHandle(void) const { return m_hParent; }

	// 워크스페이스 영역 크기 얻기
	static MkInt2 GetWorkspaceSize(void);

	// 윈도우 위치 얻기(vista 이상의 경우 aero 무시)
	static MkInt2 GetWindowPosition(HWND hWnd);

	// 윈도우 크기 얻기(vista 이상의 경우 aero 포함)
	static MkInt2 GetWindowSize(HWND hWnd);

	MkBaseWindow();
	virtual ~MkBaseWindow() { Clear(); }

protected:
	
	bool _CreateWindow(HINSTANCE hInstance, WNDPROC wndProc, HWND hParent, const MkStr& title, const MkInt2& position, const MkInt2& windowSize, bool fullScreen);

protected:

	HINSTANCE m_hInstance;
	HWND m_hWnd;
	HWND m_hParent;
	MkStr m_ClassName;
	DWORD m_WindowModeStyle;
	DWORD m_CurrentWindowStyle;
};

#endif
