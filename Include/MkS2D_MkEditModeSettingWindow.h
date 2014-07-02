#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkEditModeSettingWindow : public MkBaseWindowNode
{
public:

	bool SetUp(const MkHashStr& themeName);

	virtual void Activate(void);

	virtual void UpdateManagedRoot(void);

	MkEditModeSettingWindow(const MkHashStr& name);
	virtual ~MkEditModeSettingWindow() {}

protected:
};
