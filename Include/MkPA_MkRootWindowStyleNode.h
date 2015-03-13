#pragma once


//------------------------------------------------------------------------------------------------//
// window base node
// window system의 action part의 base class
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

	// cursor 상태 통지
	// cursor를 소유하고 있거나 이전 프레임에 소유했었다면 window manager node에 의해 호출됨
	virtual void UpdateCursorState
		(
		const MkInt2& position, // cursor의 위치
		const MkInt2& movement, // 이전 프레임의 cursor 위치와의 변동값(이전 프레임의 cursor 위치 = position - movement)
		bool cursorInside, // cursor 소유여부
		eCursorState leftCS,
		eCursorState middleCS,
		eCursorState rightCS,
		int wheelDelta // wheel의 변동값
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
