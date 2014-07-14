#pragma once


//------------------------------------------------------------------------------------------------//
// text ют╥б©К edit box
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include "MkCore_MkHashStr.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkTimeCounter.h"


#define MK_EDIT_BOX MkHiddenEditBox::Instance()


class MkBaseWindowNode;
class MkEditBoxNode;

class MkHiddenEditBox : public MkSingletonPattern<MkHiddenEditBox>
{
public:

	bool SetUp(HWND parentHandle, HINSTANCE hInstance);

	void BindControl(MkBaseWindowNode* control);

	void ReturnHit(void);

	void StepBackMsgHistory(void);
	void StepForwardMsgHistory(void);

	inline void TextModified(void) { m_Modified = true; }

	inline const MkHashStr& GetRootNodeNameOfBindingControl(void) const { return m_RootNodeNameOfBindingControl; }

	void Update(void);

	MkHiddenEditBox();
	virtual ~MkHiddenEditBox() {}

	static LRESULT CALLBACK __InputSubProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:

	HWND m_hWnd;

	MkEditBoxNode* m_BindingControl;
	MkHashStr m_RootNodeNameOfBindingControl;

	bool m_Modified;
	MkStr m_LastText;
	DWORD m_LastSelStart;
	DWORD m_LastSelEnd;

	MkTimeCounter m_ToggleCursorCounter;
};
