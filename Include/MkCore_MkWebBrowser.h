#pragma once

//------------------------------------------------------------------------------------------------//
// 웹페이지
//------------------------------------------------------------------------------------------------//

#include "exdisp.h"


class MkStr;

class MkWebBrowser
{
public:

	// 웹 페이지 열기
	bool Open(HWND parent, const MkStr& pageUrl, int posX, int posY, int width, int height, bool show = true);

	// 웹 페이지 닫기
	void Close(void);

	// 핸들 반환
	inline HWND GetBrowserHandle(void) const { return m_hWnd; }

	MkWebBrowser();
	~MkWebBrowser() { Close(); }

	// 부모가 없는 독립적인 웹 페이지 열기
	static HWND Open(const MkStr& pageUrl, int posX, int posY, int width, int height);

protected:

	IWebBrowser2* m_Browser;
	HWND m_hWnd;
	DWORD m_StyleBackup;
};
