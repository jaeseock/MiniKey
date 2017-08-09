#ifndef __MINIKEY_CORE_MKSNAPPINGWINDOW_H__
#define __MINIKEY_CORE_MKSNAPPINGWINDOW_H__

//------------------------------------------------------------------------------------------------//
// 상위 윈도우를 기준으로 정렬되는 윈도우
//
// - 초기화(SetUpBy...)시 부모 윈도우가 존재하고 정렬위치가 세팅되어 있으면 자동으로 정렬됨
// - 부모 윈도우에서 이동, 크기변경, 최소/최대화, 복원 등의 이벤트 발생 시 재정렬됨
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkListeningWindow.h"


class MkSnappingWindow : public MkListeningWindow
{
public:

	//------------------------------------------------------------------------------------------------//
	// 초기화
	//------------------------------------------------------------------------------------------------//

	// 윈도우 생성으로 초기화
	// clientSize의 xy가 0 이하이면 워크스페이스(바탕화면에서 작업표시줄이 제외된 영역) 크기로 생성
	virtual bool SetUpByWindowCreation
		(HINSTANCE hInstance, WNDPROC wndProc, HWND hParent, const MkStr& title, eSystemWindowProperty sysWinProp,
		const MkInt2& position, const MkInt2& clientSize, bool fullScreen = false, bool hide = false);

	// 외부에서 생성된 윈도우로 초기화
	virtual bool SetUpByOuterWindow(HWND hWnd);

	//------------------------------------------------------------------------------------------------//
	// 리시버
	//------------------------------------------------------------------------------------------------//
	
	// 위치 변경
	virtual void ListenPositionUpdate(const MkIntRect& newRect);

	// 크기 변경
	virtual void ListenWindowSizeUpdate(const MkIntRect& newRect);

	// 위치, 크기 변경
	virtual void ListenWindowRectUpdate(const MkIntRect& newRect);

	// 보이기 상태 변경
	virtual void ListenShowStateUpdate(unsigned int newShowCmd, const MkIntRect& newRect);

	// minimize 이후 복원
	virtual void ListenWindowRestored(const MkIntRect& newRect);

	//------------------------------------------------------------------------------------------------//
	// snapping
	//------------------------------------------------------------------------------------------------//

	// 정렬 위치 설정
	void SetAlignmentPosition(eRectAlignmentPosition alignmentPosition);

	// 정렬 위치 반환
	inline eRectAlignmentPosition GetAlignmentPosition(void) { return m_AlignmentPosition; }

	// 추가 경계 픽셀 수 설정
	inline void SetBorder(const MkInt2& size) { m_Border = size; }

	// 추가 경계 픽셀 수 반환
	inline MkInt2 GetBorder(void) const { return m_Border; }

	// pivotRect를 기준으로 정렬
	void SnapWindow(const MkIntRect& pivotRect);

public:

	MkSnappingWindow();
	virtual ~MkSnappingWindow() {}

protected:

	void _SnapToParent(void);

protected:

	eRectAlignmentPosition m_AlignmentPosition;
	MkInt2 m_Border;
};

#endif
