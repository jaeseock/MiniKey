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

//------------------------------------------------------------------------------------------------//

class MkCheckButtonNode;

class MkWindowAttributeSystemWindow : public MkBaseSystemWindow
{
public:

	virtual bool Initialize(void);

	void SetUp(MkBaseWindowNode* targetWindow);

	virtual void UseWindowEvent(WindowEvent& evt);

	MkWindowAttributeSystemWindow(const MkHashStr& name);
	virtual ~MkWindowAttributeSystemWindow() {}

protected:

	MkBaseWindowNode* m_TargetWindow;
	MkArray<MkCheckButtonNode*> m_CheckButtons;
};

//------------------------------------------------------------------------------------------------//

class MkSRectInfoListener
{
public:
	virtual void SRectInfoUpdated(MkSceneNode* targetNode, const MkHashStr& rectName, MkSRect::eSrcType srcType, MkPathName& imagePath, MkHashStr& subsetName, MkStr& decoStr, MkArray<MkHashStr>& nodeNameAndKey) {}
	virtual ~MkSRectInfoListener() {}
};

class MkSRectSetterSystemWindow : public MkBaseSystemWindow
{
public:

	virtual bool Initialize(void);

	void SetUp(MkSRectInfoListener* owner, MkSceneNode* targetNode, const MkHashStr& rectName);

	virtual void UseWindowEvent(WindowEvent& evt);

	virtual void Deactivate(void);

	MkSRectSetterSystemWindow(const MkHashStr& name);
	virtual ~MkSRectSetterSystemWindow() {}

protected:

	MkSRectInfoListener* m_Owner;
	MkSceneNode* m_TargetNode;
	MkHashStr m_RectName;

	MkPathName m_ImagePath;
	MkHashStr m_SubsetName;
	MkStr m_DecoStr;
	MkArray<MkHashStr> m_NodeNameAndKey;
	MkSRect::eSrcType m_SrcType;
};
