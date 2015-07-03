#ifndef __MINIKEY_CORE_MKOPENWEBPAGE_H__
#define __MINIKEY_CORE_MKOPENWEBPAGE_H__

//------------------------------------------------------------------------------------------------//
// 웹페이지
//------------------------------------------------------------------------------------------------//

class MkStr;

class MkOpenWebPage
{
public:
	// 인터넷 익스플로어에서 웹 페이지 열기
	static HWND Open(const MkStr& pageUrl, int posX, int posY, int width, int height);
};

#endif
