#ifndef __MINIKEY_CORE_MKWINDOWUPDATELISTENER_H__
#define __MINIKEY_CORE_MKWINDOWUPDATELISTENER_H__

//------------------------------------------------------------------------------------------------//
// 윈도우 상태 변경 알람을 받는 리스너
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkRect.h"


class MkWindowUpdateListener
{
public:
	
	// 위치 변경
	virtual void ListenPositionUpdate(const MkIntRect& newRect) {}

	// 크기 변경
	virtual void ListenWindowSizeUpdate(const MkIntRect& newRect) {}

	// 위치, 크기 변경
	virtual void ListenWindowRectUpdate(const MkIntRect& newRect) {}

	// 포커스 받음
	virtual void ListenOnTopUpdate(void) {}

	// 보이기 상태 변경
	virtual void ListenShowCmdUpdate(unsigned int newShowCmd, const MkIntRect& newRect) {}

	// minimize 이후 복원
	virtual void ListenWindowRestored(const MkIntRect& newRect) {}

	virtual ~MkWindowUpdateListener() {}
};

#endif
