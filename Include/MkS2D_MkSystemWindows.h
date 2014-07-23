#pragma once


//------------------------------------------------------------------------------------------------//
// system windows
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"

//------------------------------------------------------------------------------------------------//

class MkBaseSystemWindow : public MkBaseWindowNode
{
public:
	virtual bool Initialize(void) { return false; }

	MkBaseSystemWindow(const MkHashStr& name);
	virtual ~MkBaseSystemWindow() {}
};

//------------------------------------------------------------------------------------------------//

class MkEditBoxNode;

class MkNodeNameInputSystemWindow : public MkBaseSystemWindow
{
public:

	virtual bool Initialize(void);

	void SetUp(MkSceneNode* targetNode);

	virtual void UseWindowEvent(WindowEvent& evt);

	MkNodeNameInputSystemWindow(const MkHashStr& name);
	virtual ~MkNodeNameInputSystemWindow() {}

protected:

	MkEditBoxNode* m_EditBox;
	MkBaseWindowNode* m_OkButton;
	MkSceneNode* m_TargetNode;
	MkHashStr m_OriginalName;
};
