#pragma once

//------------------------------------------------------------------------------------------------//
// Ŭ�����忡 �ؽ�Ʈ ������/�������
// (NOTE) unicode �������� �����
//------------------------------------------------------------------------------------------------//

#include <windows.h>


class MkStr;

class MkClipboard
{
public:

	static bool SetText(HWND hWnd, const MkStr& msg);

	static bool GetText(HWND hWnd, MkStr& buffer);
};
