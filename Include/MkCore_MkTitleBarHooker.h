#ifndef __MINIKEY_CORE_MKTITLEBARHOOKER_H__
#define __MINIKEY_CORE_MKTITLEBARHOOKER_H__

//------------------------------------------------------------------------------------------------//
// MkBaseWindow의 윈도우 타이틀바 클릭시 발생하는 이벤트 후킹
// : 멀티스레드시 다른 스레드에서 윈도우를 생성하는 것과 비슷한 결과 목표
//
// - 싱글스레드라도 메인/서브윈도우의 타이틀바 클릭시 프로세스 정지현상 회피
// - 3d 렌더링의 경우 서브윈도우가 메인 윈도우 위로 드래그시 이동궤적이 클리어되는 현상 방지
// - 최대화 버튼이 없을 경우 타이틀바 더블클릭도 무시
//
// (NOTE) thread-safe 하지 않음
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include "MkCore_MkType2.h"


class MkBaseWindow;

class MkTitleBarHooker
{
public:

	// 대상 윈도우 지정
	// maxSizeIsWorkspace가 true일 경우 maximize시 클라이언트 영역이 바탕화면이 아닌 워크스페이스 크기로 설정됨
	void SetUp(MkBaseWindow* targetWindow, bool maxSizeIsWorkspace = false);

	// 초기화
	void Clear(void);

	// 대상 윈도우의 WndProc 내부에서 호출
	// 반환값은 탈출여부(return 0)
	bool CheckWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// 매 프레임 호출
	void Update(void);

	MkTitleBarHooker() { Clear(); }
	~MkTitleBarHooker() { Clear(); }

protected:

	bool _ToggleShowMode(void);

protected:

	MkBaseWindow* m_TargetWindow;
	bool m_MaximizeEnable;
	bool m_MaxSizeIsWorkspace;

	bool m_Minimized;
	bool m_OnDrag;
	MkInt2 m_CursorStartPosition;
	MkInt2 m_WindowStartPosition;
};

//------------------------------------------------------------------------------------------------//

#endif
