
#include "MkCore_MkLayeredImage.h"

#pragma comment(lib, "gdiplus.lib")


const static wchar_t* g_LayeredImageClassName = L"MkLayeredImage";

//------------------------------------------------------------------------------------------------//

bool MkLayeredImage::SetUp(const wchar_t* filePath)
{
	Clear();

	do
	{
		Gdiplus::GdiplusStartupInput gpsi;
		if (Gdiplus::GdiplusStartup(&m_GdiPlusToken, &gpsi, NULL) != Gdiplus::Ok)
			break;

		m_GdiPlusImage = Gdiplus::Image::FromFile(filePath);
		if (m_GdiPlusImage == NULL)
			break;

		WNDCLASSEX wndClass;
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
		wndClass.lpfnWndProc = MkLayeredImage::WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = DLGWINDOWEXTRA;
		wndClass.hInstance = ::GetModuleHandle(NULL);
		wndClass.hIcon = NULL;
		wndClass.hCursor = ::LoadCursor( NULL, IDC_WAIT );
		wndClass.hbrBackground = (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = g_LayeredImageClassName;
		wndClass.hIconSm = NULL;

		if (!::RegisterClassEx(&wndClass))
			break;

		int imgWidth = static_cast<int>(m_GdiPlusImage->GetWidth());
		int imgHeight = static_cast<int>(m_GdiPlusImage->GetHeight());
		int scrWidth  = ::GetSystemMetrics(SM_CXFULLSCREEN);
		int scrHeight = ::GetSystemMetrics(SM_CYFULLSCREEN);
		int x = (scrWidth  - imgWidth) / 2;
		int y = (scrHeight - imgHeight) / 2;

		m_hWnd =
			::CreateWindowEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW, g_LayeredImageClassName, L"MkLayeredWindow", WS_POPUP,
			x, y, imgWidth, imgHeight, NULL, NULL, NULL, this);

		if (m_hWnd == NULL)
			break;

		HDC hdc = ::GetDC(m_hWnd);
		SIZE size = { imgWidth, imgHeight };
		RECT rect = { 0, };
		POINT pt = { 0, };

		::GetWindowRect(m_hWnd, &rect);

		HDC hMemDC = ::CreateCompatibleDC(hdc);
		HBITMAP hBitmap = ::CreateCompatibleBitmap(hdc, imgWidth, imgHeight);
		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

		Gdiplus::Graphics g(hMemDC);
		g.DrawImage(m_GdiPlusImage, 0, 0, imgWidth, imgHeight);

		BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		BOOL bRet = ::UpdateLayeredWindow(m_hWnd, hdc, ((LPPOINT)&rect), &size, hMemDC, &pt, 0x00000000, &bf, ULW_ALPHA);

		::SelectObject(hMemDC, hOldBitmap);
		::DeleteDC(hMemDC);
		::ReleaseDC(m_hWnd, hdc);
		::DeleteObject(hBitmap);

		::ShowWindow(m_hWnd, SW_SHOW);
		::UpdateWindow(m_hWnd);

		return true;
	}
	while (false);

	Clear();
	return false;
}

void MkLayeredImage::Clear(void)
{
	if (m_GdiPlusImage != NULL)
	{
		delete m_GdiPlusImage;
		m_GdiPlusImage = NULL;
	}

	if (m_GdiPlusToken != NULL)
	{
		Gdiplus::GdiplusShutdown(m_GdiPlusToken);
		m_GdiPlusToken = NULL;
	}

	if (m_hWnd != NULL)
	{
		::CloseWindow(m_hWnd);
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;

		::UnregisterClass(g_LayeredImageClassName, ::GetModuleHandle(NULL));
	}
}

MkLayeredImage::MkLayeredImage()
{
	m_hWnd = NULL;
	m_GdiPlusImage = NULL;
	m_GdiPlusToken = NULL;
}

MkLayeredImage::~MkLayeredImage()
{
	Clear();
}

//------------------------------------------------------------------------------------------------//

LRESULT CALLBACK MkLayeredImage::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}


//------------------------------------------------------------------------------------------------//