#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkEditModeNodeSelectionWindow : public MkBaseWindowNode
{
public:

	void SetUp(MkBaseWindowNode* targetNode);

	virtual void Activate(void);

	virtual void UseWindowEvent(WindowEvent& evt);

	MkEditModeNodeSelectionWindow(const MkHashStr& name) : MkBaseWindowNode(name) {}
	virtual ~MkEditModeNodeSelectionWindow() {}

protected:
};
