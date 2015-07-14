#pragma once


//------------------------------------------------------------------------------------------------//
// render framework
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBaseFramework.h"


class MkRenderFramework : public MkBaseFramework
{
public:

	//virtual MkCheatMessage* CreateCheatMessage(void) const;

	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg);

	virtual void ConsumeSetCursorMsg(void);

	virtual void Update(void);

	// renderer는 main window handle이 살아 있는 상태에서 종료해야 하기 때문에 Clear()가 아닌 Free()를 사용
	virtual void Free(void);

	MkRenderFramework() : MkBaseFramework() {}
	virtual ~MkRenderFramework() {}
};
