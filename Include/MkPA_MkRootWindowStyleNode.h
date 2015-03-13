#pragma once


//------------------------------------------------------------------------------------------------//
// window base node
// window system�� action part�� base class
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowBaseNode.h"


class MkRootWindowStyleNode : public MkWindowBaseNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_RootWindowStyleNode; }

	// alloc child instance
	static MkRootWindowStyleNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// window system interface
	//------------------------------------------------------------------------------------------------//

	// cursor ���� ����
	// cursor�� �����ϰ� �ְų� ���� �����ӿ� �����߾��ٸ� window manager node�� ���� ȣ���
	virtual void UpdateCursorState
		(
		const MkInt2& position, // cursor�� ��ġ
		const MkInt2& movement, // ���� �������� cursor ��ġ���� ������(���� �������� cursor ��ġ = position - movement)
		bool cursorInside, // cursor ��������
		eCursorState leftCS,
		eCursorState middleCS,
		eCursorState rightCS,
		int wheelDelta // wheel�� ������
		);

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	virtual void SendNodeCommandTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode& argument);

	//------------------------------------------------------------------------------------------------//

	MkRootWindowStyleNode(const MkHashStr& name);
	virtual ~MkRootWindowStyleNode() {}

public:

	//static const MkHashStr IconTagNodeName;
	//static const MkHashStr CaptionTagNodeName;
};
