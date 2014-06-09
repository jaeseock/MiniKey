#ifndef __MINIKEY_CORE_MKWINDOWUPDATELISTENER_H__
#define __MINIKEY_CORE_MKWINDOWUPDATELISTENER_H__

//------------------------------------------------------------------------------------------------//
// ������ ���� ���� �˶��� �޴� ������
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkRect.h"


class MkWindowUpdateListener
{
public:
	
	// ��ġ ����
	virtual void ListenPositionUpdate(const MkIntRect& newRect) {}

	// ũ�� ����
	virtual void ListenWindowSizeUpdate(const MkIntRect& newRect) {}

	// ��ġ, ũ�� ����
	virtual void ListenWindowRectUpdate(const MkIntRect& newRect) {}

	// ��Ŀ�� ����
	virtual void ListenOnTopUpdate(void) {}

	// ���̱� ���� ����
	virtual void ListenShowCmdUpdate(unsigned int newShowCmd, const MkIntRect& newRect) {}

	// minimize ���� ����
	virtual void ListenWindowRestored(const MkIntRect& newRect) {}

	virtual ~MkWindowUpdateListener() {}
};

#endif
