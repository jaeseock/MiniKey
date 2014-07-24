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

class MkNodeNameInputListener
{
public:
	virtual void NodeNameChanged(const MkHashStr& oldName, const MkHashStr& newName, MkSceneNode* targetNode) {}
	virtual ~MkNodeNameInputListener() {}
};

class MkEditBoxNode;

class MkNodeNameInputSystemWindow : public MkBaseSystemWindow
{
public:

	virtual bool Initialize(void);

	void SetUp(MkSceneNode* targetNode, MkNodeNameInputListener* owner);

	virtual void Activate(void);
	virtual void Deactivate(void);

	virtual void UseWindowEvent(WindowEvent& evt);

	MkNodeNameInputSystemWindow(const MkHashStr& name);
	virtual ~MkNodeNameInputSystemWindow() {}

protected:

	void _ApplyNodeName(const MkHashStr& newName);

protected:

	MkEditBoxNode* m_EditBox;
	MkBaseWindowNode* m_OkButton;
	MkSceneNode* m_TargetNode;
	MkHashStr m_OriginalName;
	MkNodeNameInputListener* m_Owner;
};
