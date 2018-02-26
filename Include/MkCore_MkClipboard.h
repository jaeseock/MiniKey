#pragma once

//------------------------------------------------------------------------------------------------//
// 클립보드에 텍스트 보내기/갖고오기
// (NOTE) unicode 포맷으로 입출력
//------------------------------------------------------------------------------------------------//

#include <windows.h>


class MkStr;

class MkClipboard
{
public:

	static bool SetText(HWND hWnd, const MkStr& msg);

	static bool GetText(HWND hWnd, MkStr& buffer);
};
