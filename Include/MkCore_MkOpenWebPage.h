#ifndef __MINIKEY_CORE_MKOPENWEBPAGE_H__
#define __MINIKEY_CORE_MKOPENWEBPAGE_H__

//------------------------------------------------------------------------------------------------//
// ��������
//------------------------------------------------------------------------------------------------//

class MkStr;

class MkOpenWebPage
{
public:
	// ���ͳ� �ͽ��÷ξ�� �� ������ ����
	static HWND Open(const MkStr& pageUrl, int posX, int posY, int width, int height);
};

#endif
