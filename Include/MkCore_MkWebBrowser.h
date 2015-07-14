#pragma once

//------------------------------------------------------------------------------------------------//
// ��������
//------------------------------------------------------------------------------------------------//

#include "exdisp.h"


class MkStr;

class MkWebBrowser
{
public:

	// �� ������ ����
	bool Open(HWND parent, const MkStr& pageUrl, int posX, int posY, int width, int height, bool show = true);

	// �� ������ �ݱ�
	void Close(void);

	// �ڵ� ��ȯ
	inline HWND GetBrowserHandle(void) const { return m_hWnd; }

	MkWebBrowser();
	~MkWebBrowser() { Close(); }

	// �θ� ���� �������� �� ������ ����
	static HWND Open(const MkStr& pageUrl, int posX, int posY, int width, int height);

protected:

	IWebBrowser2* m_Browser;
	HWND m_hWnd;
	DWORD m_StyleBackup;
};
