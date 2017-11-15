#pragma once

//------------------------------------------------------------------------------------------------//
// 브라우저로 웹페이지 열기
// ftp 업로드는 MkFtpInterface, 다운로드는 MkFileDownloader, 웹페이지 읽기는 MkVisitWebPage 참조
//
// 그런데 브라우저가 다양화 된 요즘 특성상,
// ::ShellExcute(NULL, L"open", url, L"", L"", SW_SHOW);
// 이게 더 적합할 경우가 많다;;
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
