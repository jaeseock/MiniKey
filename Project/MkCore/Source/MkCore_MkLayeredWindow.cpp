
#include "MkCore_MkLayeredWindow.h"

#pragma comment(lib, "gdiplus.lib")


//------------------------------------------------------------------------------------------------//

bool MkLayeredWindow::SetUp(const wchar_t* className, const wchar_t* bgFilePath)
{
	return _SetUp(className, bgFilePath, true, MkInt2::Zero, NULL);
}

bool MkLayeredWindow::SetUp(const wchar_t* className, int resID)
{
	return _SetUp(className, resID, true, MkInt2::Zero, NULL);
}

bool MkLayeredWindow::SetUp(const wchar_t* className, const wchar_t* bgFilePath, int x, int y, WNDPROC wndProc)
{
	return _SetUp(className, bgFilePath, false, MkInt2(x, y), wndProc);
}

bool MkLayeredWindow::SetUp(const wchar_t* className, int resID, int x, int y, WNDPROC wndProc)
{
	return _SetUp(className, resID, false, MkInt2(x, y), wndProc);
}

void MkLayeredWindow::Update(void)
{
	RECT currRect = { 0, };
	::GetWindowRect(m_hWnd, &currRect);

	POINT ptDst = { currRect.left, currRect.top };
	SIZE updateSize = { currRect.right - currRect.left, currRect.bottom - currRect.top };
	POINT ptSrc = { 0, 0 };

	HDC hdc = ::GetDC(m_hWnd);
	HDC hMemDC = ::CreateCompatibleDC(hdc);
	HBITMAP hBitmap = ::CreateCompatibleBitmap(hdc, updateSize.cx, updateSize.cy);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

	Gdiplus::Graphics g(hMemDC);

	MkMapLooper<unsigned int, _ControlBase*> looper(m_Controls);
	MK_STL_LOOP(looper)
	{
		looper.GetCurrentField()->Update(g);
	}
	
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	BOOL bRet = ::UpdateLayeredWindow(m_hWnd, hdc, &ptDst, &updateSize, hMemDC, &ptSrc, 0x00000000, &bf, ULW_ALPHA);

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);
	::ReleaseDC(m_hWnd, hdc);
	::DeleteObject(hBitmap);
}

void MkLayeredWindow::Clear(void)
{
	MkMapLooper<unsigned int, _ControlBase*> looper(m_Controls);
	MK_STL_LOOP(looper)
	{
		delete looper.GetCurrentField();
	}
	m_Controls.Clear();
	
	if (m_GdiPlusToken != NULL)
	{
		Gdiplus::GdiplusShutdown(m_GdiPlusToken);
		m_GdiPlusToken = NULL;
	}

	if ((m_hWnd != NULL) && (!m_ClassName.empty()))
	{
		::CloseWindow(m_hWnd);
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;

		::UnregisterClass(m_ClassName.c_str(), ::GetModuleHandle(NULL));
		m_ClassName.clear();
	}
}

MkLayeredWindow::MkLayeredWindow()
{
	m_hWnd = NULL;
	m_GdiPlusToken = NULL;
}

MkLayeredWindow::~MkLayeredWindow()
{
	Clear();
}

LRESULT MkLayeredWindow::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		return 0;

	case WM_ENABLE:
		return 0;
	}

	return 1;
}

LRESULT CALLBACK MkLayeredWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (MsgProc(hWnd, msg, wParam, lParam) == 0)
		return 0;

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

bool MkLayeredWindow::_SetUp(const wchar_t* className, const wchar_t* bgFilePath, bool center, const MkInt2& position, WNDPROC wndProc)
{
	Clear();

	do
	{
		Gdiplus::GdiplusStartupInput gpsi;
		if (Gdiplus::GdiplusStartup(&m_GdiPlusToken, &gpsi, NULL) != Gdiplus::Ok)
			break;

		_ControlBase* bgControl = new _ControlBase;
		m_Controls.Create(0xffffffff, bgControl);
		if (!bgControl->SetUp(MkInt2::Zero, bgFilePath))
			break;

		m_ClassName = className;

		WNDCLASSEX wndClass;
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
		wndClass.lpfnWndProc = (wndProc == NULL) ? MkLayeredWindow::WndProc : wndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = DLGWINDOWEXTRA;
		wndClass.hInstance = ::GetModuleHandle(NULL);
		wndClass.hIcon = NULL;
		wndClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = m_ClassName.c_str();
		wndClass.hIconSm = NULL;

		if (!::RegisterClassEx(&wndClass))
			break;

		const MkInt2& bgSize = bgControl->GetRect().size;
		int x, y;
		DWORD dwExStyle, dwStyle;
		if (center)
		{
			int scrWidth  = ::GetSystemMetrics(SM_CXFULLSCREEN);
			int scrHeight = ::GetSystemMetrics(SM_CYFULLSCREEN);
			x = (scrWidth  - bgSize.x) / 2;
			y = (scrHeight - bgSize.y) / 2;
			dwExStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW;
			dwStyle = WS_POPUP;
		}
		else
		{
			x = position.x;
			y = position.y;
			dwExStyle = WS_EX_LAYERED;
			dwStyle = WS_VISIBLE;
		}

		m_hWnd = ::CreateWindowEx(dwExStyle, m_ClassName.c_str(), L"MkLayeredWindow", dwStyle, x, y, bgSize.x, bgSize.y, NULL, NULL, NULL, this);
		if (m_hWnd == NULL)
			break;

		::ShowWindow(m_hWnd, SW_SHOW);
		::UpdateWindow(m_hWnd);

		Update();
		return true;
	}
	while (false);

	Clear();
	return false;
}

bool MkLayeredWindow::_SetUp(const wchar_t* className, int resID, bool center, const MkInt2& position, WNDPROC wndProc)
{
	Clear();

	do
	{
		Gdiplus::GdiplusStartupInput gpsi;
		if (Gdiplus::GdiplusStartup(&m_GdiPlusToken, &gpsi, NULL) != Gdiplus::Ok)
			break;

		_ControlBase* bgControl = new _ControlBase;
		m_Controls.Create(0xffffffff, bgControl);
		if (!bgControl->SetUp(MkInt2::Zero, resID))
			break;

		m_ClassName = className;

		WNDCLASSEX wndClass;
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
		wndClass.lpfnWndProc = (wndProc == NULL) ? MkLayeredWindow::WndProc : wndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = DLGWINDOWEXTRA;
		wndClass.hInstance = ::GetModuleHandle(NULL);
		wndClass.hIcon = NULL;
		wndClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = m_ClassName.c_str();
		wndClass.hIconSm = NULL;

		if (!::RegisterClassEx(&wndClass))
			break;

		const MkInt2& bgSize = bgControl->GetRect().size;
		int x, y;
		DWORD dwExStyle, dwStyle;
		if (center)
		{
			int scrWidth  = ::GetSystemMetrics(SM_CXFULLSCREEN);
			int scrHeight = ::GetSystemMetrics(SM_CYFULLSCREEN);
			x = (scrWidth  - bgSize.x) / 2;
			y = (scrHeight - bgSize.y) / 2;
			dwExStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW;
			dwStyle = WS_POPUP;
		}
		else
		{
			x = position.x;
			y = position.y;
			dwExStyle = WS_EX_LAYERED;
			dwStyle = WS_VISIBLE;
		}

		m_hWnd = ::CreateWindowEx(dwExStyle, m_ClassName.c_str(), L"MkLayeredWindow", dwStyle, x, y, bgSize.x, bgSize.y, NULL, NULL, NULL, this);
		if (m_hWnd == NULL)
			break;

		::ShowWindow(m_hWnd, SW_SHOW);
		::UpdateWindow(m_hWnd);

		Update();
		return true;
	}
	while (false);

	Clear();
	return false;
}

//------------------------------------------------------------------------------------------------//

bool MkLayeredWindow::_ControlBase::SetUp(const MkInt2& position, const wchar_t* bgFilePath)
{
	Clear();

	do
	{
		m_Image = Gdiplus::Image::FromFile(bgFilePath);
		if (m_Image == NULL)
			break;

		int imgWidth = static_cast<int>(m_Image->GetWidth());
		int imgHeight = static_cast<int>(m_Image->GetHeight());
		if ((imgWidth == 0) || (imgHeight == 0))
			break;

		m_Rect.position = position;
		m_Rect.size = MkInt2(imgWidth, imgHeight);
		return true;
	}
	while (false);

	Clear();
	return false;
}

bool MkLayeredWindow::_ControlBase::SetUp(const MkInt2& position, int resID)
{
	Clear();

	do
	{
		m_Image = _LoadPNG(resID);
		if (m_Image == NULL)
			break;

		int imgWidth = static_cast<int>(m_Image->GetWidth());
		int imgHeight = static_cast<int>(m_Image->GetHeight());
		if ((imgWidth == 0) || (imgHeight == 0))
			break;

		m_Rect.position = position;
		m_Rect.size = MkInt2(imgWidth, imgHeight);
		return true;
	}
	while (false);

	Clear();
	return false;
}

void MkLayeredWindow::_ControlBase::Update(Gdiplus::Graphics& g)
{
	if (m_Image != NULL)
	{
		g.DrawImage(m_Image, m_Rect.position.x, m_Rect.position.y, m_Rect.size.x, m_Rect.size.y);
	}
}

void MkLayeredWindow::_ControlBase::Clear(void)
{
	MK_DELETE(m_Image);
}

Gdiplus::Image* MkLayeredWindow::_ControlBase::_LoadPNG(int resID)
{
	Gdiplus::Image* image = NULL;
	HINSTANCE hInstance = ::GetModuleHandle(NULL);
	HRSRC hRes = ::FindResource(hInstance, MAKEINTRESOURCE(resID), L"PNG");
	if (hRes != NULL)
	{
		IStream* stream = NULL;
		if (::CreateStreamOnHGlobal(NULL, TRUE, &stream) == S_OK)
		{
			void* resData = ::LockResource(::LoadResource(hInstance, hRes));
			DWORD resSize = ::SizeofResource(hInstance, hRes);
			DWORD rw = 0;
			stream->Write(resData, resSize, &rw);
			image = Gdiplus::Image::FromStream(stream);
			stream->Release();
		}
	}

	return image;
}

//------------------------------------------------------------------------------------------------//