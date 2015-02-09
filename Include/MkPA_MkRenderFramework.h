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

	virtual void Clear(void);

	MkRenderFramework() : MkBaseFramework() {}
	virtual ~MkRenderFramework() {}
};
