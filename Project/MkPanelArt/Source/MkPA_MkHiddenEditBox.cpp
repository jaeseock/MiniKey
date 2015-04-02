
#include "MkCore_MkCheck.h"
#include "MkCore_MkTimeManager.h"

//#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkEditBoxControlNode.h"
#include "MkPA_MkHiddenEditBox.h"


//------------------------------------------------------------------------------------------------//

#define MKDEF_BOX_WIDTH 800
#define MKDEF_MAX_INPUT_COUNT 0xffff // 인풋창에 최대 입력가능 글자 수
#define MKDEF_HIDDEN_EDIT_BOX_ID 0xffff
//#define MKDEF_TOGGLE_CURSOR_TIME_COUNT 0.6f

static WNDPROC gOldInputProc = NULL;

//------------------------------------------------------------------------------------------------//

bool MkHiddenEditBox::SetUp(HWND parentHandle, HINSTANCE hInstance)
{
	if ((parentHandle == NULL) || (hInstance == NULL))
		return false;

	m_hWnd = CreateWindow
		(L"edit", NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT, 0, 6000, MKDEF_BOX_WIDTH, 20,
		parentHandle, reinterpret_cast<HMENU>(MKDEF_HIDDEN_EDIT_BOX_ID), hInstance, NULL);

	MK_CHECK(m_hWnd != NULL, L"hidden edit box 생성 실패")
		return false;

	gOldInputProc = reinterpret_cast<WNDPROC>(SetWindowLong(m_hWnd, GWL_WNDPROC, reinterpret_cast<LONG>(__InputSubProc)));
	PostMessage(m_hWnd, EM_LIMITTEXT, static_cast<WPARAM>(MKDEF_MAX_INPUT_COUNT), NULL);

	return true;
}

void MkHiddenEditBox::ReturnHit(void)
{
	if (m_BindingControl != NULL)
	{
		//m_BindingControl->CommitText();
	}
}

void MkHiddenEditBox::StepBackMsgHistory(void)
{
	if ((m_hWnd != NULL) && (m_BindingControl != NULL) && m_BindingControl->__StepBackMsgHistory())
	{
		SetWindowText(m_hWnd, m_BindingControl->GetText().GetPtr());
	}
}

void MkHiddenEditBox::StepForwardMsgHistory(void)
{
	if ((m_hWnd != NULL) && (m_BindingControl != NULL) && m_BindingControl->__StepForwardMsgHistory())
	{
		SetWindowText(m_hWnd, m_BindingControl->GetText().GetPtr());
	}
}

MkHiddenEditBox::MkHiddenEditBox() : MkSingletonPattern<MkHiddenEditBox>()
{
	m_hWnd = NULL;
	m_BindingControl = NULL;
	m_Modified = false;
}

LRESULT MkHiddenEditBox::__InputSubProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CHAR:
		if (wParam == VK_RETURN)
			return 0; // 소리 제거용
		break;

	case WM_KEYDOWN:
		{
			bool textChanged = true;
			switch (wParam)
			{
			case VK_RETURN:
				MK_EDIT_BOX.ReturnHit();
				textChanged = false;
				return 0;

			case VK_UP:
				MK_EDIT_BOX.StepBackMsgHistory();
				textChanged = false;
				return 0;

			case VK_DOWN:
				MK_EDIT_BOX.StepForwardMsgHistory();
				textChanged = false;
				return 0;
			}

			if (textChanged)
			{
				MK_EDIT_BOX.TextModified();
			}
		}
		break;

	case WM_KILLFOCUS:
		//MK_EDIT_BOX.BindControl(NULL);
		break;
	}

	return (gOldInputProc == NULL) ? 0 : CallWindowProc(gOldInputProc, hWnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------------------------//
