
#include "MkCore_MkCheck.h"
#include "MkCore_MkGlobalFunction.h"
#include "MkCore_MkSystemEnvironment.h"
#include "MkCore_MkBaseWindow.h"

#if (MKDEF_CORE_DEV_ENV_OVER_VISTA)
#if (WINVER >= 0x0600)
	#include <Dwmapi.h>
	#pragma comment (lib, "Dwmapi.lib")
#endif
#endif


const static DWORD FullModeStyle = (WS_EX_TOPMOST | WS_POPUP);

//------------------------------------------------------------------------------------------------//

bool MkBaseWindow::SetUpByWindowCreation
(HINSTANCE hInstance, WNDPROC wndProc, HWND hParent, const MkStr& title, eSystemWindowProperty sysWinProp,
 const MkInt2& position, const MkInt2& clientSize, bool fullScreen)
{
	// 윈도우 스타일
	m_WindowModeStyle = (WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION);
	if (MK_FLAG_EXIST(sysWinProp, eSWP_Minimize))
	{
		m_WindowModeStyle |= WS_MINIMIZEBOX;
	}
	if (MK_FLAG_EXIST(sysWinProp, eSWP_Maximize))
	{
		m_WindowModeStyle |= WS_MAXIMIZEBOX;
	}
	if (MK_FLAG_EXIST(sysWinProp, eSWP_Close))
	{
		m_WindowModeStyle |= WS_SYSMENU;
	}

	m_CurrentWindowStyle = (fullScreen) ? FullModeStyle : m_WindowModeStyle;

	// 윈도우 위치 및 크기 재설정
	MkInt2 windowPos = position;
	MkInt2 windowSize = clientSize;
	
	if (fullScreen)
	{
		if ((windowSize.x <= 0) || (windowSize.y <= 0))
		{
			windowSize = MK_SYS_ENV.GetBackgroundResolution();
		}
		windowPos = MkInt2::Zero;
	}
	else
	{
		if ((windowSize.x <= 0) || (windowSize.y <= 0))
		{
			windowSize = GetWorkspaceSize();
			if (windowSize == MkInt2::Zero)
				return false;

			windowPos = MkInt2::Zero;
		}
		else
		{
			windowSize = ConvertClientToWindowSize(clientSize);
		}
	}

	// 윈도우 생성
	return _CreateWindow(hInstance, wndProc, hParent, title, windowPos, windowSize, fullScreen);
}

bool MkBaseWindow::SetUpByOuterWindow(HWND hWnd)
{
	MK_CHECK(hWnd != NULL, L"초기화되지 않은 윈도우")
		return false;

	m_hWnd = hWnd;
	m_hParent = GetParent(m_hWnd);
	m_ClassName.Clear(); // 클래스 이름이 비었으므로 종료시 클래스 해제하지 않음(외부에 의존)
	m_CurrentWindowStyle = m_WindowModeStyle = static_cast<DWORD>(GetWindowLongPtr(m_hWnd, GWL_STYLE));
	m_hInstance = GetInstanceHandle();
	return true;
}

bool MkBaseWindow::SetPosition(const MkInt2& position)
{
	MK_CHECK(m_hWnd != NULL, L"초기화되지 않은 윈도우")
		return false;

	bool ok = (position != GetPosition());
	if (ok)
	{
		ok = (SetWindowPos(m_hWnd, NULL, position.x, position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE) != 0);
	}
	return ok;
}

MkInt2 MkBaseWindow::GetPosition(void) const
{
	return MkBaseWindow::GetWindowPosition(m_hWnd);
}

bool MkBaseWindow::SetWindowSize(const MkInt2& size)
{
	MK_CHECK(m_hWnd != NULL, L"초기화되지 않은 윈도우")
		return false;

	bool ok = (size != GetWindowSize());
	if (ok)
	{
		ok = (SetWindowPos(m_hWnd, NULL, 0, 0, size.x, size.y, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE) != 0);
	}
	return ok;
}

MkInt2 MkBaseWindow::GetWindowSize(void) const
{
	return MkBaseWindow::GetWindowSize(m_hWnd);
}

bool MkBaseWindow::SetClientSize(const MkInt2& size)
{
	MK_CHECK(m_hWnd != NULL, L"초기화되지 않은 윈도우")
		return false;

	bool ok = (size != GetClientSize());
	if (ok)
	{
		MkInt2 windowSize = ConvertClientToWindowSize(size);
		ok = SetWindowSize(windowSize);
	}
	return ok;
}

MkInt2 MkBaseWindow::GetClientSize(void) const
{
	MK_CHECK(m_hWnd != NULL, L"초기화되지 않은 윈도우")
		return MkInt2::Zero;

	RECT rect;
	GetClientRect(m_hWnd, &rect);
	return MkInt2(static_cast<int>(rect.right), static_cast<int>(rect.bottom));
}

bool MkBaseWindow::SetOnTop(void)
{
	MK_CHECK(m_hWnd != NULL, L"초기화되지 않은 윈도우")
		return false;

	bool ok = !GetOnTop();
	if (ok)
	{
		SetFocus(m_hWnd);
	}
	return ok;
}

bool MkBaseWindow::GetOnTop(void) const
{
	MK_CHECK(m_hWnd != NULL, L"초기화되지 않은 윈도우")
		return false;

	return (GetFocus() == m_hWnd);
}

bool MkBaseWindow::SetShowCmd(unsigned int showCmd)
{
	MK_CHECK(m_hWnd != NULL, L"초기화되지 않은 윈도우")
		return false;

	bool ok = (showCmd != GetShowCmd());
	if (ok)
	{
		ShowWindow(m_hWnd, showCmd);
	}
	return ok;
}

unsigned int MkBaseWindow::GetShowCmd(void) const
{
	MK_CHECK(m_hWnd != NULL, L"초기화되지 않은 윈도우")
		return 0;

	WINDOWPLACEMENT pm;
	GetWindowPlacement(m_hWnd, &pm);
	return pm.showCmd;
}

void MkBaseWindow::Clear(void)
{
	if ((!m_ClassName.Empty()) && (m_hInstance != NULL))
	{
		UnregisterClass(m_ClassName.GetPtr(), m_hInstance);
	}
	m_ClassName.Clear();
	m_hInstance = NULL;
	m_hWnd = NULL;
}

void MkBaseWindow::SetWindowTitle(const MkStr& title)
{
	if ((!title.Empty()) && (m_hWnd != NULL))
	{
		SetWindowText(m_hWnd, title.GetPtr());
	}
}

DWORD MkBaseWindow::GetWindowStyle(void) const
{
	return (m_hWnd == NULL) ? 0 : static_cast<DWORD>(::GetWindowLongPtr(m_hWnd, GWL_STYLE));
}

void MkBaseWindow::ChangeToWindowModeStyle(void)
{
	if (m_hWnd != NULL)
	{
		m_CurrentWindowStyle = m_WindowModeStyle;

		::SetWindowLongPtr(m_hWnd, GWL_STYLE, m_WindowModeStyle);
		::SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_DRAWFRAME);
	}
}

void MkBaseWindow::ChangeToFullModeStyle(void)
{
	if (m_hWnd != NULL)
	{
		m_CurrentWindowStyle = FullModeStyle;

		::SetWindowLongPtr(m_hWnd, GWL_STYLE, m_WindowModeStyle);
		::SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_DRAWFRAME);
	}
}

MkInt2 MkBaseWindow::ConvertClientToWindowSize(const MkInt2& clientSize) const
{
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = clientSize.x;
	rect.bottom = clientSize.y;
	AdjustWindowRect(&rect, m_CurrentWindowStyle, FALSE);
	return MkInt2(static_cast<int>(rect.right - rect.left), static_cast<int>(rect.bottom - rect.top));
}

MkInt2 MkBaseWindow::ConvertWindowToClientSize(const MkInt2& windowSize) const
{
	return (windowSize - ConvertClientToWindowSize(MkInt2::Zero));
}

HINSTANCE MkBaseWindow::GetInstanceHandle(void) const
{
	return (m_hWnd == NULL) ? NULL : reinterpret_cast<HINSTANCE>(GetWindowLongPtr(m_hWnd, GWL_HINSTANCE));
}

MkInt2 MkBaseWindow::GetWorkspaceSize(void)
{
	// 워크스페이스 영역 얻기
	RECT rect;
	MK_CHECK(SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0) != 0, L"워크스페이스 영역을 얻을 수 없음")
		return MkInt2::Zero;

	return MkInt2(static_cast<int>(rect.right - rect.left), static_cast<int>(rect.bottom - rect.top));
}

MkInt2 MkBaseWindow::GetWindowPosition(HWND hWnd)
{
	MK_CHECK(hWnd != NULL, L"초기화되지 않은 윈도우")
		return MkInt2::Zero;

	RECT rect;
	::GetWindowRect(hWnd, &rect);
	return MkInt2(static_cast<int>(rect.left), static_cast<int>(rect.top));
}

MkInt2 MkBaseWindow::GetWindowSize(HWND hWnd)
{
	MK_CHECK(hWnd != NULL, L"초기화되지 않은 윈도우")
		return MkInt2::Zero;

	RECT rect;
#if (MKDEF_CORE_DEV_ENV_OVER_VISTA)
#if (WINVER >= 0x0600)
	if (MK_SYS_ENV.CheckWindowsIsVistaOrHigher())
	{
		rect.left = rect.right = rect.top = rect.bottom = 0;
		DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(rect));

		if ((rect.left == 0) && (rect.right == 0) && (rect.top == 0) && (rect.bottom == 0)) // 테마 필터가 꺼져 있을 수 있음
		{
			::GetWindowRect(hWnd, &rect);
		}
	}
	else
#endif
#endif
	{
		::GetWindowRect(hWnd, &rect);
	}
	return MkInt2(static_cast<int>(rect.right - rect.left), static_cast<int>(rect.bottom - rect.top));
}

MkBaseWindow::MkBaseWindow()
{
	m_hInstance = NULL;
	m_hWnd = NULL;
	m_hParent = NULL;
}

//------------------------------------------------------------------------------------------------//

bool MkBaseWindow::_CreateWindow
(HINSTANCE hInstance, WNDPROC wndProc, HWND hParent, const MkStr& title, const MkInt2& position, const MkInt2& windowSize, bool fullScreen)
{
	MK_CHECK((m_hWnd == NULL) && (m_ClassName.Empty()), L"이미 초기화된 윈도우임")
		return false;

	MK_CHECK(hInstance != NULL, L"NULL instance handle")
		return false;
	m_hInstance = hInstance;

	MK_CHECK(!title.Empty(), L"윈도우 타이틀이 비어있음")
		return false;

	MK_CHECK(wndProc != NULL, L"WNDPROC이 NULL임")
		return false;

	// class 등록
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; // 더블클릭 허용
	wc.lpfnWndProc   = wndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH); // or WHITE_BRUSH
	wc.lpszMenuName  = 0;
	wc.lpszClassName = title.GetPtr();
	RegisterClass(&wc);

	// 윈도우 등록
	m_hWnd = CreateWindowEx(NULL, title.GetPtr(), title.GetPtr(), m_CurrentWindowStyle,
		position.x, position.y, windowSize.x, windowSize.y, hParent, NULL, m_hInstance, NULL);
	MK_CHECK(m_hWnd != NULL, title + L" 윈도우 생성 실패")
		return false;

	m_hParent = hParent;
	m_ClassName = title;

	return true;
}

//------------------------------------------------------------------------------------------------//