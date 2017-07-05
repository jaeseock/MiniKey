#pragma once

#include <windows.h>
#include <gdiplus.h>


class MkLayeredImage
{
public:

	bool SetUp(const wchar_t* filePath = NULL);
	void Clear(void);

	MkLayeredImage();
	~MkLayeredImage();

	HWND GetHWND() const { return m_hWnd; }

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:

	HWND m_hWnd;
	Gdiplus::Image* m_GdiPlusImage;
	ULONG_PTR m_GdiPlusToken;
};
