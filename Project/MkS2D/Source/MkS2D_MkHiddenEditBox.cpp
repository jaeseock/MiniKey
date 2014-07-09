
#include "MkCore_MkGlobalFunction.h"
#include "MkCore_MkCheck.h"

#include "MkCore_MkDevPanel.h"

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

	//m_Font = CreateFont(14, 0, 0, 0, 0, 0, 0, 0,
	//	HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH | FF_ROMAN, L"굴림");

	//MK_CHECK(m_Font != NULL, L"hidden edit box용 폰트 생성 실패")
	//	return false;

	m_hWnd = CreateWindow
		(L"edit", NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT, 0, 6000, MKDEF_BOX_WIDTH, 20,
		parentHandle, reinterpret_cast<HMENU>(MKDEF_HIDDEN_EDIT_BOX_ID), hInstance, NULL);

	MK_CHECK(m_hWnd != NULL, L"hidden edit box 생성 실패")
		return false;

	//SendMessage(m_hWnd, WM_SETFONT, reinterpret_cast<WPARAM>(m_Font), 0);

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

	if (ebNode != m_BindingControl)
	{
		if (m_BindingControl != NULL)
		{
			m_BindingControl->__BindingLost();

			MK_DEV_PANEL.MsgToLog(L"__BindingLost : " + m_BindingControl->GetNodeName().GetString());
		}

		m_BindingControl = ebNode;

		if (m_BindingControl == NULL)
		{
			m_RootNodeNameOfBindingControl.Clear();
			m_LastSelStart = 0;
			m_LastSelEnd = 0;

			MK_DEV_PANEL.MsgToLog(L"focus lost");
		}
		else
		{
			m_RootNodeNameOfBindingControl = m_BindingControl->GetRootWindow()->GetNodeName();
			m_LastText = m_BindingControl->GetText();
			m_LastSelStart = m_BindingControl->__GetSetStart();
			m_LastSelEnd = m_BindingControl->__GetSetEnd();

			m_BindingControl->__SetFocus();

			MK_DEV_PANEL.MsgToLog(L"__SetFocus : " + m_BindingControl->GetNodeName().GetString());
		}

		if (m_hWnd != NULL)
		{
			SetWindowText(m_hWnd, m_LastText.GetPtr());
			SendMessage(m_hWnd, EM_SETSEL, static_cast<WPARAM>(m_LastSelStart), static_cast<LPARAM>(m_LastSelEnd));
			SetFocus((m_BindingControl == NULL) ? NULL : m_hWnd);
		}
	}
}

void MkHiddenEditBox::StepBackMsgHistory(void)
{
	if ((m_hWnd != NULL) && (m_BindingControl != NULL) && m_BindingControl->__StepBackMsgHistory())
	{
		SetWindowText(m_hWnd, m_BindingControl->GetText().GetPtr());

		MK_DEV_PANEL.MsgToLog(L"StepBackMsgHistory : " + m_BindingControl->GetNodeName().GetString());
	}
}

void MkHiddenEditBox::StepForwardMsgHistory(void)
{
	if ((m_hWnd != NULL) && (m_BindingControl != NULL) && m_BindingControl->__StepForwardMsgHistory())
	{
		SetWindowText(m_hWnd, m_BindingControl->GetText().GetPtr());

		MK_DEV_PANEL.MsgToLog(L"StepForwardMsgHistory : " + m_BindingControl->GetNodeName().GetString());
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

		if (m_BindingControl->__UpdateTextInfo(currText, currSelStart, currSelEnd))
		{
			MK_DEV_PANEL.MsgToLog(L"Update : " + currText + L", " + MkStr(static_cast<int>(currSelStart)) + L" ~ " + MkStr(static_cast<int>(currSelEnd)));
		}

		m_Modified = false;
	}
}

MkHiddenEditBox::MkHiddenEditBox() : MkSingletonPattern<MkHiddenEditBox>()
{
	m_Font = NULL;
	m_hWnd = NULL;
	m_BindingControl = NULL;

	m_Modified = false;
	m_LastSelStart = 0;
	m_LastSelEnd = 0;
}

MkHiddenEditBox::~MkHiddenEditBox()
{
	MK_DELETE_OBJECT(m_Font);
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
		switch (wParam)
		{
		case VK_RETURN:
			//MK_DEV_PANEL.__CheckWndProc(WM_COMMAND, MAKELONG(MKDEF_SEND_BUTTON_ID, BN_CLICKED), NULL); // SEND 버튼 입력
			return 0;

		case VK_UP:
			MK_EDIT_BOX.StepBackMsgHistory();
			return 0;

		case VK_DOWN:
			MK_EDIT_BOX.StepForwardMsgHistory();
			return 0;
		}

		MK_DEV_PANEL.MsgToLog(L"WM_KEYDOWN : " + MkStr(static_cast<int>(wParam)));
		MK_EDIT_BOX.TextModified();
		break;
	}

	return (gOldInputProc == NULL) ? 0 : CallWindowProc(gOldInputProc, hWnd, msg, wParam, lParam);
}


//------------------------------------------------------------------------------------------------//
