
#include "MkCore_MkGlobalFunction.h"
#include "MkCore_MkCheck.h"

#include "MkS2D_MkEditBoxNode.h"
#include "MkS2D_MkHiddenEditBox.h"


#define MKDEF_BOX_WIDTH 800
#define MKDEF_MAX_INPUT_COUNT 512 // 인풋창에 최대 입력가능 글자 수
#define MKDEF_HIDDEN_EDIT_BOX_ID 0xffff

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

void MkHiddenEditBox::BindControl(MkBaseWindowNode* control)
{
	MkEditBoxNode* ebNode = NULL;
	if (control != NULL)
	{
		ebNode = dynamic_cast<MkEditBoxNode*>(control);
	}

	if (ebNode == m_BindingControl)
	{
		// 메인 윈도우 자체가 포커스를 잃을 경우 BindControl(NULL)을 호출해 주던가 아니면 이런 식으로 재활성화 시키면 됨
		if ((ebNode != NULL) && (m_hWnd != NULL))
		{
			SetFocus(m_hWnd);
		}
	}
	else
	{
		if (m_BindingControl != NULL)
		{
			m_BindingControl->__BindingLost();
		}

		m_BindingControl = ebNode;

		if (m_BindingControl == NULL)
		{
			m_RootNodeNameOfBindingControl.Clear();
			m_LastSelStart = 0;
			m_LastSelEnd = 0;
		}
		else
		{
			m_RootNodeNameOfBindingControl = m_BindingControl->GetRootWindow()->GetNodeName();
			m_LastText = m_BindingControl->GetText();
			m_LastSelStart = m_BindingControl->__GetSetStart();
			m_LastSelEnd = m_BindingControl->__GetSetEnd();

			m_BindingControl->__SetFocus();
		}

		if (m_hWnd != NULL)
		{
			SetWindowText(m_hWnd, m_LastText.GetPtr());
			SendMessage(m_hWnd, EM_SETSEL, static_cast<WPARAM>(m_LastSelStart), static_cast<LPARAM>(m_LastSelEnd));
			SetFocus((m_BindingControl == NULL) ? NULL : m_hWnd);
		}
	}
}

void MkHiddenEditBox::ReturnHit(void)
{
	if (m_BindingControl != NULL)
	{
		m_BindingControl->__TakeCurrentText();
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

void MkHiddenEditBox::Update(void)
{
	if (m_Modified && (m_hWnd != NULL) && (m_BindingControl != NULL))
	{
		wchar_t buffer[MKDEF_MAX_INPUT_COUNT + 1] = {0, };
		GetWindowText(m_hWnd, buffer, MKDEF_MAX_INPUT_COUNT + 1);
		MkStr currText = buffer;

		DWORD currSelStart, currSelEnd;
		SendMessage(m_hWnd, EM_GETSEL, reinterpret_cast<WPARAM>(&currSelStart), reinterpret_cast<LPARAM>(&currSelEnd));

		m_BindingControl->__UpdateTextInfo(currText, currSelStart, currSelEnd);

		m_Modified = false;
	}
}

MkHiddenEditBox::MkHiddenEditBox() : MkSingletonPattern<MkHiddenEditBox>()
{
	m_hWnd = NULL;
	m_BindingControl = NULL;

	m_Modified = false;
	m_LastSelStart = 0;
	m_LastSelEnd = 0;
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
	}

	return (gOldInputProc == NULL) ? 0 : CallWindowProc(gOldInputProc, hWnd, msg, wParam, lParam);
}


//------------------------------------------------------------------------------------------------//
