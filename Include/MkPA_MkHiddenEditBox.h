#pragma once


//------------------------------------------------------------------------------------------------//
// MkEditBoxControlNode에서 공용으로 사용하는 window control 관리자
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include "MkCore_MkHashStr.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkTimeCounter.h"


#define MK_EDIT_BOX MkHiddenEditBox::Instance()


class MkSceneNode;
class MkEditBoxControlNode;

class MkHiddenEditBox : public MkSingletonPattern<MkHiddenEditBox>
{
public:

	bool SetUp(HWND parentHandle, HINSTANCE hInstance);

	void SetTargetEditBoxNode(const MkSceneNode* ownerMgr, MkEditBoxControlNode* targetEditBox);

	void NotifyTextChange(MkEditBoxControlNode* editBox);

	void Update(const MkSceneNode* ownerMgr);

	void ReturnHit(void);
	void StepBackMsgHistory(void);
	void StepForwardMsgHistory(void);
	inline void TextModified(void) { m_Modified = true; }

	inline bool IsBinding(void) const { return (m_TargetEditBox != NULL); }

	MkHiddenEditBox();
	virtual ~MkHiddenEditBox() {}

	static LRESULT CALLBACK __InputSubProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:

	void _ClearTargetEditBox(bool changeInputFocus);
	void _ApplyTargetEditBoxStateToWindowControl(void);

protected:

	HWND m_hWnd;

	MkEditBoxControlNode* m_TargetEditBox;
	const MkSceneNode* m_TargetWindowMgr;

	bool m_Modified;

	MkTimeCounter m_ToggleCursorCounter;
};
