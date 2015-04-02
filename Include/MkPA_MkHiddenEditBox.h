#pragma once


//------------------------------------------------------------------------------------------------//
// bitmap texture 생성, 삭제 관리자
// - thread이슈가 사라지면(single-thread) 초기화/종료 위치에 대해 신경 쓰지 않아도 되므로 pooling만 관리하면 됨
// - render to texture는 pooling의 필요성이 없으므로 알아서 해야 함
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include "MkCore_MkHashStr.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkTimeCounter.h"


#define MK_EDIT_BOX MkHiddenEditBox::Instance()


class MkEditBoxControlNode;

class MkHiddenEditBox : public MkSingletonPattern<MkHiddenEditBox>
{
public:

	bool SetUp(HWND parentHandle, HINSTANCE hInstance);

	void ReturnHit(void);
	void StepBackMsgHistory(void);
	void StepForwardMsgHistory(void);
	inline void TextModified(void) { m_Modified = true; }

	MkHiddenEditBox();
	virtual ~MkHiddenEditBox() {}

	static LRESULT CALLBACK __InputSubProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:

	HWND m_hWnd;

	MkEditBoxControlNode* m_BindingControl;

	bool m_Modified;

	MkTimeCounter m_ToggleCursorCounter;
};
