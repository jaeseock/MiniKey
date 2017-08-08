#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <string>

#include "MkCore_MkRect.h"
#include "MkCore_MkMap.h"


//------------------------------------------------------------------------------------------------//

class MkLayeredWindow
{
public:

	// ��üȭ�� �߾ӿ� ����
	bool SetUp(const wchar_t* className, const wchar_t* bgFilePath);
	bool SetUp(const wchar_t* className, int resID);

	// �־��� ��ġ�� �־��� ���ν����� ����
	bool SetUp(const wchar_t* className, const wchar_t* bgFilePath, int x, int y, WNDPROC wndProc);
	bool SetUp(const wchar_t* className, int resID, int x, int y, WNDPROC wndProc);

	void Update(void);

	// ����
	void Clear(void);

	HWND GetHWND() const { return m_hWnd; }

	MkLayeredWindow();
	~MkLayeredWindow();

	static LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:

	bool _SetUp(const wchar_t* className, const wchar_t* bgFilePath, bool center, const MkInt2& position, WNDPROC wndProc);
	bool _SetUp(const wchar_t* className, int resID, bool center, const MkInt2& position, WNDPROC wndProc);

protected:

	//------------------------------------------------------------------------------------------------//

	class _ControlBase
	{
	public:
		bool SetUp(const MkInt2& position, const wchar_t* bgFilePath);
		bool SetUp(const MkInt2& position, int resID);

		virtual void Update(Gdiplus::Graphics& g);

		void Clear(void);

		inline const MkIntRect& GetRect(void) const { return m_Rect; }

		_ControlBase() { m_Image = NULL; }
		virtual ~_ControlBase() { Clear(); }

	protected:
		Gdiplus::Image* _LoadPNG(int resID);

	protected:

		MkIntRect m_Rect;
		Gdiplus::Image* m_Image;
	};

	//------------------------------------------------------------------------------------------------//

protected:

	HWND m_hWnd;
	ULONG_PTR m_GdiPlusToken;

	std::wstring m_ClassName;

	MkMap<unsigned int, _ControlBase*> m_Controls;
};
