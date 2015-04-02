#pragma once


//------------------------------------------------------------------------------------------------//
// bitmap texture ����, ���� ������
// - thread�̽��� �������(single-thread) �ʱ�ȭ/���� ��ġ�� ���� �Ű� ���� �ʾƵ� �ǹǷ� pooling�� �����ϸ� ��
// - render to texture�� pooling�� �ʿ伺�� �����Ƿ� �˾Ƽ� �ؾ� ��
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
