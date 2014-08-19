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
	virtual void SRectInfoUpdated
		(MkSceneNode* targetNode, const MkHashStr& rectName, bool flipHorizontal, bool flipVertical, float alpha,
		MkSRect::eSrcType srcType, const MkPathName& imagePath, const MkHashStr& subsetName, const MkStr& decoStr, const MkArray<MkHashStr>& nodeNameAndKey) {}
	virtual ~MkSRectInfoListener() {}
};

class MkSpreadButtonNode;

class MkSRectSetterSystemWindow : public MkBaseSystemWindow
{
public:

	enum eInputType
	{
		eOneOfSelection = 0, // image, custom str, scene str 중 하나. flip horizontal/vertical, alpha 설정 가능
		eImageOnly, // image만. flip horizontal/vertical, alpha 설정 불가
		eStringOnly // custom str, scene str 중 하나만. flip horizontal/vertical, alpha 설정 불가
	};

	virtual bool Initialize(void);

	void SetUp(MkSRectInfoListener* owner, MkSceneNode* targetNode, const MkHashStr& rectName, eInputType inputType);

	virtual void UseWindowEvent(WindowEvent& evt);

	virtual void Deactivate(void);

	MkSRectSetterSystemWindow(const MkHashStr& name);
	virtual ~MkSRectSetterSystemWindow() {}

protected:

	void _UpdataSubControl(void);
	void _UpdateSamplePosition(void);
	void _ConvertNodeNameAndKeyToStr(const MkArray<MkHashStr>& nodeNameAndKey, MkStr& msg);
	void _ConvertStrToNodeNameAndKey(const MkStr& msg, MkArray<MkHashStr>& nodeNameAndKey);

protected:

	MkSRectInfoListener* m_Owner;
	MkSceneNode* m_TargetNode;
	MkHashStr m_RectName;
	eInputType m_InputType;
	MkFloat2 m_RectBGSize;

	MkPathName m_ImagePath;
	MkHashStr m_SubsetName;
	MkStr m_DecoStr;
	MkArray<MkHashStr> m_NodeNameAndKey;
	MkSRect::eSrcType m_SrcType;

	MkSRect* m_SampleRect;

	MkCheckButtonNode* m_FlipHorizontalBtn;
	MkCheckButtonNode* m_FlipVerticalBtn;
	MkSpreadButtonNode* m_AlphaBtn;
	MkSpreadButtonNode* m_SrcTypeBtn;
	MkBaseWindowNode* m_ImageLoadBtn;
	MkEditBoxNode* m_ImageSubsetEB;
	MkEditBoxNode* m_CustomStringEB;
	MkEditBoxNode* m_SceneStringEB;
};
