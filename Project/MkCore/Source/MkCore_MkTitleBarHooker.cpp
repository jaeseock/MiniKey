
#include "MkCore_MkBaseWindow.h"
#include "MkCore_MkInputManager.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkTitleBarHooker.h"


//------------------------------------------------------------------------------------------------//

void MkTitleBarHooker::SetUp(MkBaseWindow* targetWindow, bool maxSizeIsWorkspace)
{
	m_TargetWindow = targetWindow;
	m_MaximizeEnable = (targetWindow == NULL) ? false : MK_FLAG_EXIST(m_TargetWindow->GetWindowModeStyle(), WS_MAXIMIZEBOX);
	m_MaxSizeIsWorkspace = maxSizeIsWorkspace;
	m_Minimized = false;
	m_OnDrag = false;
}

void MkTitleBarHooker::Clear(void)
{
	SetUp(NULL);
}

bool MkTitleBarHooker::CheckWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ((m_TargetWindow == NULL) || (hWnd != m_TargetWindow->GetWindowHandle()))
		return false;

	switch (msg)
	{
	case WM_NCLBUTTONDOWN:
		switch (wParam)
		{
		case HTCLOSE: // 종료
			MkBaseFramework::Close();
			return true;

		case HTCAPTION: // 드래그 시작
			m_TargetWindow->SetOnTop();
			m_OnDrag = true;

			m_CursorStartPosition.x = static_cast<int>(LOWORD(lParam));
			m_CursorStartPosition.y = static_cast<int>(HIWORD(lParam));

			// WORD -> int 캐스팅시 음수 고려
			if (m_CursorStartPosition.x >= 0x8000) m_CursorStartPosition.x -= 0x10000;
			if (m_CursorStartPosition.y >= 0x8000) m_CursorStartPosition.y -= 0x10000;
			
			m_WindowStartPosition = m_TargetWindow->GetPosition();
			return true;

		case HTMAXBUTTON: // maximize가 허용될 경우 노멀 -> 최대화, 최대 -> 노멀화
			if (m_MaximizeEnable && _ToggleShowMode())
				return true;
			break;

		case HTMINBUTTON: // 최소화
			m_TargetWindow->SetShowCmd(SW_SHOWMINIMIZED);
			m_Minimized = true;
			return true;
		}
		break;

	case WM_NCLBUTTONUP:
		m_OnDrag = false;
		return true;

	case WM_NCLBUTTONDBLCLK:
		switch (wParam)
		{
		case HTCAPTION: // maximize가 허용될 경우 HTMAXBUTTON과 동일 동작
			if (m_MaximizeEnable && _ToggleShowMode())
				return true;
			break;
		}
		break;

	case WM_SIZE:
		switch (wParam)
		{
		case SIZE_RESTORED:
		case SIZE_MAXIMIZED:
			if (m_Minimized)
			{
				m_TargetWindow->WindowRestored();
				m_Minimized = false;

				MK_INPUT_MGR.UpdateTargetWindowClientSize(m_TargetWindow->GetWindowHandle());
			}
			break;
		}
		break;

	case WM_GETMINMAXINFO:
		if (m_MaxSizeIsWorkspace)
		{
			RECT rect;
			if (SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0) != 0)
			{
				LONG x = rect.right - rect.left;
				LONG y = rect.bottom - rect.top;

				LPMINMAXINFO mmi = reinterpret_cast<LPMINMAXINFO>(lParam);
				mmi->ptMaxTrackSize.x = x;
				mmi->ptMaxTrackSize.y = y;
				mmi->ptMaxSize.x = x;
				mmi->ptMaxSize.y = y;
				return true;
			}
		}
		break;
	}
	
	return false;
}

void MkTitleBarHooker::Update(void)
{
	if (m_TargetWindow == NULL)
		return;

	// update
	if (m_OnDrag)
	{
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			POINT point;
			GetCursorPos(&point);

			MkInt2 offset(static_cast<int>(point.x), static_cast<int>(point.y));
			offset -= m_CursorStartPosition;

			m_TargetWindow->SetPosition(m_WindowStartPosition + offset);
		}
		else
		{
			SendMessage(m_TargetWindow->GetWindowHandle(), WM_NCLBUTTONUP, 0, 0);
		}
	}
}

bool MkTitleBarHooker::_ToggleShowMode(void)
{
	assert(m_TargetWindow != NULL);

	switch (m_TargetWindow->GetShowCmd())
	{
	case SW_SHOWMAXIMIZED:
		m_TargetWindow->SetShowCmd(SW_SHOWNORMAL);
		MK_INPUT_MGR.UpdateTargetWindowClientSize(m_TargetWindow->GetWindowHandle());
		return true;
	case SW_SHOWNORMAL:
		m_TargetWindow->SetShowCmd(SW_SHOWMAXIMIZED);
		MK_INPUT_MGR.UpdateTargetWindowClientSize(m_TargetWindow->GetWindowHandle());
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------//
