#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkSystemWindows.h"


class MkEditModeSettingWindow : public MkBaseSystemWindow
{
public:

	virtual bool Initialize(void);

	virtual void UseWindowEvent(WindowEvent& evt);

	MkEditModeSettingWindow(const MkHashStr& name);
	virtual ~MkEditModeSettingWindow() {}

protected:
};
