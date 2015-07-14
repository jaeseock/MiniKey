
#include "MkCore_MkCheck.h"
#include "MkCore_MkTimeManager.h"

#include "MkPA_MkEditBoxControlNode.h"
#include "MkPA_MkHiddenEditBox.h"


//------------------------------------------------------------------------------------------------//

#define MKDEF_BOX_WIDTH 800
#define MKDEF_MAX_INPUT_COUNT 0xffff // ��ǲâ�� �ִ� �Է°��� ���� ��
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

	MK_CHECK(m_hWnd != NULL, L"hidden edit box ���� ����")
		return false;

	gOldInputProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(m_hWnd, GWL_WNDPROC, reinterpret_cast<LONG>(__InputSubProc)));
	PostMessage(m_hWnd, EM_LIMITTEXT, static_cast<WPARAM>(MKDEF_MAX_INPUT_COUNT), NULL);

	return true;
}

void MkHiddenEditBox::SetTargetEditBoxNode(const MkSceneNode* ownerMgr, MkEditBoxControlNode* targetEditBox)
{
	if ((m_hWnd == NULL) || (targetEditBox == m_TargetEditBox))
		return;

	if (targetEditBox == NULL)
	{
		if ((ownerMgr == NULL) || (ownerMgr == m_TargetWindowMgr)) // os�� ���� window manager�� ���� ��ɸ� �ν�
		{
			_ClearTargetEditBox(ownerMgr != NULL); // os�� ���� ������ ������ focus�� �������� ����
		}
	}
	else
	{
		_ClearTargetEditBox(false);

		m_TargetEditBox = targetEditBox;
		m_TargetWindowMgr = m_TargetEditBox->FindNearestDerivedNode(ePA_SNT_WindowManagerNode);

		m_TargetEditBox->__GainInputFocus();

		// ������ edit box�� ������ ������ window�� �ݿ�
		_ApplyTargetEditBoxStateToWindowControl();

		SetFocus(m_hWnd);

		// cursor toggle timer ����
		MkTimeState timeState;
		MK_TIME_MGR.GetCurrentTimeState(timeState);
		m_ToggleCursorCounter.SetUp(timeState, MKDEF_TOGGLE_CURSOR_TIME_COUNT);
	}
}

void MkHiddenEditBox::NotifyTextChange(MkEditBoxControlNode* editBox)
{
	if ((editBox != NULL) && (editBox == m_TargetEditBox))
	{
		_ApplyTargetEditBoxStateToWindowControl();
	}
}

void MkHiddenEditBox::Update(const MkSceneNode* ownerMgr)
{
	// target node�� ������ ��� �ش� owner manager update�� ����
	if ((m_hWnd != NULL) && (m_TargetEditBox != NULL) && (ownerMgr == m_TargetWindowMgr))
	{
		// target node�� disable�̸� �ڵ� ����
		if (!m_TargetEditBox->GetEnable())
		{
			_ClearTargetEditBox(true);
		}
		// target node�� �׷����� ������ �ڵ� ����
		else
		{
			MkSceneNode* targetNode = m_TargetEditBox;
			while (targetNode != NULL)
			{
				if (targetNode->GetVisible())
				{
					targetNode = targetNode->GetParentNode();
				}
				else
				{
					_ClearTargetEditBox(true);
					break;
				}
			}
		}

		// �������� �ʾ����� ����
		if (m_TargetEditBox != NULL)
		{
			// cursor toggle
			MkTimeState timeState;
			MK_TIME_MGR.GetCurrentTimeState(timeState);
			if (m_ToggleCursorCounter.OnTick(timeState))
			{
				m_TargetEditBox->__ToggleNormalCursor();
			}

			// input�� ������ �����Ǿ����� �ݿ�
			if (m_Modified)
			{
				wchar_t buffer[MKDEF_MAX_INPUT_COUNT + 1] = {0, };
				GetWindowText(m_hWnd, buffer, MKDEF_MAX_INPUT_COUNT + 1);
				
				DWORD selStart, selEnd;
				SendMessage(m_hWnd, EM_GETSEL, reinterpret_cast<WPARAM>(&selStart), reinterpret_cast<LPARAM>(&selEnd));

				m_TargetEditBox->__UpdateText(MkStr(buffer), selStart, selEnd);
				m_Modified = false;
			}
		}
	}
}

void MkHiddenEditBox::ReturnHit(void)
{
	if (m_TargetEditBox != NULL)
	{
		m_TargetEditBox->CommitText();
	}
}

void MkHiddenEditBox::StepBackMsgHistory(void)
{
	if (m_TargetEditBox != NULL)
	{
		m_TargetEditBox->__StepBackMsgHistory();
	}
}

void MkHiddenEditBox::StepForwardMsgHistory(void)
{
	if (m_TargetEditBox != NULL)
	{
		m_TargetEditBox->__StepForwardMsgHistory();
	}
}

MkHiddenEditBox::MkHiddenEditBox() : MkSingletonPattern<MkHiddenEditBox>()
{
	m_hWnd = NULL;
	m_TargetEditBox = NULL;
	m_TargetWindowMgr = NULL;
	m_Modified = false;
}

LRESULT MkHiddenEditBox::__InputSubProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CHAR:
		if (wParam == VK_RETURN)
			return 0; // �Ҹ� ���ſ�
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
		MK_EDIT_BOX.SetTargetEditBoxNode(NULL, NULL);
		break;
	}

	return (gOldInputProc == NULL) ? 0 : CallWindowProc(gOldInputProc, hWnd, msg, wParam, lParam);
}

void MkHiddenEditBox::_ClearTargetEditBox(bool changeInputFocus)
{
	if (m_TargetEditBox != NULL)
	{
		m_TargetEditBox->__LostInputFocus();
		m_TargetEditBox = NULL;
		m_TargetWindowMgr = NULL;

		if (changeInputFocus && (m_hWnd != NULL))
		{
			SetWindowText(m_hWnd, MkStr::EMPTY.GetPtr());
			SendMessage(m_hWnd, EM_SETSEL, 0, 0);
			HWND parentHandle = GetParent(m_hWnd);
			SetFocus(parentHandle);
		}
	}
}

void MkHiddenEditBox::_ApplyTargetEditBoxStateToWindowControl(void)
{
	if ((m_hWnd != NULL) && (m_TargetEditBox != NULL))
	{
		SetWindowText(m_hWnd, m_TargetEditBox->GetText().GetPtr());
		SendMessage(m_hWnd, EM_SETSEL, static_cast<WPARAM>(m_TargetEditBox->__GetSelStart()), static_cast<LPARAM>(m_TargetEditBox->__GetSelEnd()));
	}
}

//------------------------------------------------------------------------------------------------//
