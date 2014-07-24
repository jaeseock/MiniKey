#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkSystemWindows.h"


class MkScrollBarNode;

class MkEditModeTargetWindow : public MkBaseSystemWindow, public MkNodeNameInputListener
{
public:

	//------------------------------------------------------------------------------------------------//

	void SetTargetNode(MkBaseWindowNode* targetNode);

	//------------------------------------------------------------------------------------------------//
	// MkBaseSystemWindow

	virtual bool Initialize(void);

	virtual void Activate(void);

	virtual bool HitEventMouseWheelMove(int delta, const MkFloat2& position);

	virtual void UseWindowEvent(WindowEvent& evt);

	virtual void UpdateNode(void);

	//------------------------------------------------------------------------------------------------//
	// MkNodeNameInputListener

	virtual void NodeNameChanged(const MkHashStr& oldName, const MkHashStr& newName, MkSceneNode* targetNode);

	//------------------------------------------------------------------------------------------------//

	MkEditModeTargetWindow(const MkHashStr& name);
	virtual ~MkEditModeTargetWindow() {}

protected:

	void _RebuildNodeTree(void);
	void _SetSelectIcon(bool enable);
	void _SetNodeTreeItemPosition(unsigned int pagePos);
	void _SetWindowTitleByTargetNode(void);

protected:

	// public:
	MkBaseWindowNode* m_TargetNode;
	MkFloatRect m_ClientRect;

	// node tree region
	MkBaseWindowNode* m_NodeTreeRoot;
	MkArray<MkBaseWindowNode*> m_NodeTreeSrc;
	MkScrollBarNode* m_NodeTreeScrollBar;

	// node name
	MkBaseWindowNode* m_NodeNameBtn;
	MkSRect* m_NodePositionRect;
	MkStr m_LastNodePositionStr;
};
