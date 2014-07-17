#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkEditModeSettingWindow : public MkBaseWindowNode
{
public:

	bool SetUp(void);

	virtual void UseWindowEvent(WindowEvent& evt);

	MkEditModeSettingWindow(const MkHashStr& name);
	virtual ~MkEditModeSettingWindow() {}

protected:
};
