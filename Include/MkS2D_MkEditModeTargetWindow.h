#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkSystemWindows.h"


class MkScrollBarNode;
class MkCheckButtonNode;
class MkTabWindowNode;
class MkSpreadButtonNode;

class MkEditModeTargetWindow : public MkBaseSystemWindow, public MkNodeNameInputListener, public MkSRectInfoListener
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
	// MkSRectInfoListener

	virtual void SRectInfoUpdated
		(MkSceneNode* targetNode, const MkHashStr& rectName,
		bool flipHorizontal, bool flipVertical, float alpha,
		MkSRect::eSrcType srcType, const MkPathName& imagePath, const MkHashStr& subsetName, const MkStr& decoStr, const MkArray<MkHashStr>& nodeNameAndKey);

	//------------------------------------------------------------------------------------------------//

	MkEditModeTargetWindow(const MkHashStr& name);
	virtual ~MkEditModeTargetWindow() {}

protected:

	void _RebuildNodeTree(void);
	void _SetSelectIcon(bool enable);
	void _SetNodeTreeItemPosition(unsigned int pagePos);
	void _UpdateControlsByTargetNode(void);

	void _UpdateTabTagDesc(void);
	bool _GetTargetComponentTagExistByTargetTab(void);
	void _UpdateTabTagControlEnable(bool enable);
	void _AlignTargetTagRect(const MkHashStr& tagName, eRectAlignmentPosition alignment);
	void _MoveTargetTagRect(const MkHashStr& tagName, const MkFloat2& offset);
	bool _GetNormalSRectList(MkArray<MkHashStr>& rectList) const;

protected:

	// public:
	MkBaseWindowNode* m_TargetNode;
	MkFloatRect m_ClientRect;

	// node tree region
	MkBaseWindowNode* m_NodeTreeRoot;
	MkArray<MkBaseWindowNode*> m_NodeTreeSrc;
	MkScrollBarNode* m_NodeTreeScrollBar;

	// node name
	MkBaseWindowNode* m_ToParentNodeBtn;
	MkBaseWindowNode* m_NodeNameBtn;
	MkBaseWindowNode* m_SaveNodeBtn;
	MkBaseWindowNode* m_DeleteNodeBtn;
	MkBaseWindowNode* m_WinAttrBtn;

	MkSRect* m_NodePositionRect;
	MkStr m_LastNodePositionStr;
	MkSRect* m_NodeWSizeRect;
	MkStr m_LastNodeWSizeStr;

	MkCheckButtonNode* m_EnableCB;

	// tab root
	MkTabWindowNode* m_TabWindow;

	// tab : tag
	MkSRect* m_TabTag_Desc;
	MkSpreadButtonNode* m_TabTag_TargetSelection;
	MkArray<MkBaseWindowNode*> m_TabTag_EnableWindows;
};
