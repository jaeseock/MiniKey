#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkSystemWindows.h"


class MkScrollBarNode;

class MkEditModeTargetWindow : public MkBaseSystemWindow
{
public:

	//------------------------------------------------------------------------------------------------//

	virtual bool Initialize(void);

	void SetTargetNode(MkBaseWindowNode* targetNode);

	//------------------------------------------------------------------------------------------------//

	virtual void Activate(void);

	virtual bool HitEventMouseWheelMove(int delta, const MkFloat2& position);

	virtual void UseWindowEvent(WindowEvent& evt);

	//------------------------------------------------------------------------------------------------//

	MkEditModeTargetWindow(const MkHashStr& name);
	virtual ~MkEditModeTargetWindow() {}

protected:

	void _RebuildNodeTree(void);
	void _SetSelectIcon(bool enable);
	void _SetNodeTreeItemPosition(unsigned int pagePos);

protected:

	MkBaseWindowNode* m_NodeTreeRoot;
	MkArray<MkBaseWindowNode*> m_NodeTreeSrc;
	MkScrollBarNode* m_NodeTreeScrollBar;
	MkBaseWindowNode* m_TargetNode;
	MkFloatRect m_ClientRect;
};
