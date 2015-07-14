#ifndef __MINIKEY_CORE_MKLISTENINGWINDOW_H__
#define __MINIKEY_CORE_MKLISTENINGWINDOW_H__

//------------------------------------------------------------------------------------------------//
// 변경시 하위 MkWindowUpdateListener에게 정보를 전달하는 윈도우
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkWindowUpdateListener.h"
#include "MkCore_MkBaseWindow.h"


class MkListeningWindow : public MkBaseWindow, public MkWindowUpdateListener
{
public:

	//------------------------------------------------------------------------------------------------//
	// listener 관리
	//------------------------------------------------------------------------------------------------//
	
	// listener 추가후 아이디 반환
	virtual unsigned int RegisterWindowUpdateListener(MkWindowUpdateListener* reciever);

	// id에 해당하는 listener 삭제
	virtual void RemoveWindowUpdateListener(unsigned int index);

	// 모든 listener 삭제
	virtual void ClearAllWindowUpdateListener(void);

	//------------------------------------------------------------------------------------------------//
	// 변경시 정보 전파
	//------------------------------------------------------------------------------------------------//

	// 윈도우 생성으로 초기화
	// clientSize의 xy가 0 이하이면 워크스페이스(바탕화면에서 작업표시줄이 제외된 영역) 크기로 생성
	virtual bool SetUpByWindowCreation
		(HINSTANCE hInstance, WNDPROC wndProc, HWND hParent, const MkStr& title, eSystemWindowProperty sysWinProp,
		const MkInt2& position, const MkInt2& clientSize, bool fullScreen = false);

	// 외부에서 생성된 윈도우로 초기화
	virtual bool SetUpByOuterWindow(HWND hWnd);

	// 윈도우 위치 설정
	virtual bool SetPosition(const MkInt2& position);

	// 윈도우 크기 재설정
	virtual bool SetWindowSize(const MkInt2& size);

	// 포커스 설정
	virtual bool SetOnTop(void);

	// 보이기 상태 설정
	virtual bool SetShowCmd(unsigned int showCmd);

	// SW_SHOWMINIMIZED 상태에서 윈도우가 복원되었을 때 MkTitleBarHooker로부터 호출되는 콜백 펑션
	// 해당 펑션 호출 이전 다른 변경은 모두 완료된 상태(position, size 등)
	virtual void WindowRestored(void);

	//------------------------------------------------------------------------------------------------//

	// 해제
	virtual void Clear(void);

public:

	MkListeningWindow() : MkBaseWindow(), MkWindowUpdateListener() {}
	virtual ~MkListeningWindow() { Clear(); }

	//------------------------------------------------------------------------------------------------//

protected:

	// 정보를 보낼 대상 리시버
	MkArray<MkWindowUpdateListener*> m_Listener;
};

#endif
