
#include "MkCore_MkGlobalFunction.h"
#include "MkCore_MkCheck.h"
#include "MkCore_MkTimeManager.h"

#include "MkS2D_MkEditBoxNode.h"
#include "MkS2D_MkHiddenEditBox.h"


#define MKDEF_BOX_WIDTH 800
#define MKDEF_MAX_INPUT_COUNT 0 // 인풋창에 최대 입력가능 글자 수. 0이면 시스템에 의해 0x7FFFFFFE가 설정 됨
#define MKDEF_HIDDEN_EDIT_BOX_ID 0xffff
#define MKDEF_TOGGLE_CURSOR_TIME_COUNT 0.6f

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
	if (control == NULL)
	{
		if (m_BindingControl != NULL)
		{
			m_BindingControl->__BindingLost();
			m_BindingControl = NULL;
			m_RootNodeNameOfBindingControl.Clear();
			m_LastSelStart = 0;
			m_LastSelEnd = 0;

			if (m_hWnd != NULL)
			{
				SetWindowText(m_hWnd, L"");
				SendMessage(m_hWnd, EM_SETSEL, 0, 0);
				HWND parentHandle = GetParent(m_hWnd);
				SetFocus(parentHandle);
			}
		}
	}
	else
	{
		MkEditBoxNode* ebNode = dynamic_cast<MkEditBoxNode*>(control);
		if (ebNode != NULL)
		{
			if ((m_BindingControl == NULL) || (ebNode != m_BindingControl))
			{
				if (m_BindingControl != NULL)
				{
					m_BindingControl->__BindingLost();
				}

				m_BindingControl = ebNode;

				m_RootNodeNameOfBindingControl = m_BindingControl->GetRootWindow()->GetNodeName();
				m_LastText = m_BindingControl->GetText();
				m_LastSelStart = m_BindingControl->__GetSetStart();
				m_LastSelEnd = m_BindingControl->__GetSetEnd();

				m_BindingControl->__SetFocus();

				MkTimeState timeState;
				MK_TIME_MGR.GetCurrentTimeState(timeState);
				m_ToggleCursorCounter.SetUp(timeState, MKDEF_TOGGLE_CURSOR_TIME_COUNT);

				if (m_hWnd != NULL)
				{
					SetWindowText(m_hWnd, m_LastText.GetPtr());
					SendMessage(m_hWnd, EM_SETSEL, static_cast<WPARAM>(m_LastSelStart), static_cast<LPARAM>(m_LastSelEnd));
					SetFocus(m_hWnd);
				}
			}
		}
	}
}

void MkHiddenEditBox::ReturnHit(void)
{
	if (m_BindingControl != NULL)
	{
		m_BindingControl->CommitText();
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
	if ((m_hWnd != NULL) && (m_BindingControl != NULL))
	{
		MkTimeState timeState;
		MK_TIME_MGR.GetCurrentTimeState(timeState);

		if (m_ToggleCursorCounter.OnTick(timeState))
		{
			m_BindingControl->__ToggleCursorRect();
		}

		if (m_Modified)
		{
			wchar_t buffer[MKDEF_MAX_INPUT_COUNT + 1] = {0, };
			GetWindowText(m_hWnd, buffer, MKDEF_MAX_INPUT_COUNT + 1);
			MkStr currText = buffer;

			DWORD currSelStart, currSelEnd;
			SendMessage(m_hWnd, EM_GETSEL, reinterpret_cast<WPARAM>(&currSelStart), reinterpret_cast<LPARAM>(&currSelEnd));

			m_BindingControl->__UpdateTextInfo(currText, currSelStart, currSelEnd);

			m_Modified = false;
		}

		// 포커싱중인 콘트롤이 disable 되면 자동 해제
		if (!m_BindingControl->GetEnable())
		{
			BindControl(NULL);
		}
		// 포커싱중인 콘트롤이 그려지지 않으면 자동 해제
		else
		{
			MkSceneNode* targetNode = m_BindingControl;
			while (targetNode != NULL)
			{
				if (targetNode->GetVisible())
				{
					targetNode = targetNode->GetParentNode();
				}
				else
				{
					BindControl(NULL);
					break;
				}
			}
		}
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

	case WM_KILLFOCUS:
		MK_EDIT_BOX.BindControl(NULL);
		break;
	}

	return (gOldInputProc == NULL) ? 0 : CallWindowProc(gOldInputProc, hWnd, msg, wParam, lParam);
}


//------------------------------------------------------------------------------------------------//
