#pragma once


//------------------------------------------------------------------------------------------------//
// window control node : body frame
// 부모가 MkTitleBarControlNode의 파생 객체인지 여부에 따라 동작이 다름
//
// ex>
//	MkBodyFrameControlNode* titleBar = MkBodyFrameControlNode::CreateChildNode(NULL, L"TitleBar");
//	...
//	MkBodyFrameControlNode* bodyFrame = MkBodyFrameControlNode::CreateChildNode(titleBar, L"BodyFrame");
//	bodyFrame->SetBodyFrame
//		(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true,
//		MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(350.f, 250.f));
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowBaseNode.h"


class MkBodyFrameControlNode : public MkWindowBaseNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_BodyFrameControlNode; }

	// alloc child instance
	static MkBodyFrameControlNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// control interface
	//------------------------------------------------------------------------------------------------//

	// 부모 node에 부착되는 방식
	enum eHangingType
	{
		eHT_None = 0,
		eHT_OverParentWindow, // 부모 window rect 상단에 위치
		eHT_UnderParentWindow, // 부모 window rect 하단에 위치. 부모가 title bar일 경우 일반적인 방식
		eHT_IncludeParentAtTop, // 부모 node가 body frame의 client rect 상단에 위치
		eHT_IncludeParentAtBottom // 부모 node가 body frame의 client rect 하단에 위치
	};

	// regular component로 설정
	void SetBodyFrame
		(const MkHashStr& themeName, MkWindowThemeData::eComponentType componentType,
		bool useShadow, eHangingType hangingType, const MkFloat2& clientSize);

	// custom form으로 설정
	void SetBodyFrame
		(const MkHashStr& themeName, const MkHashStr& customFormName,
		bool useShadow, eHangingType hangingType, const MkFloat2& clientSize);

	//------------------------------------------------------------------------------------------------//
	// region
	//------------------------------------------------------------------------------------------------//

	// 부모가 MkTitleBarControlNode의 파생 객체이고 SetBodyFrame()이 사전 호출된 상태이면 부모의 크기도 조정 됨
	virtual void SetClientSize(const MkFloat2& clientSize);

	//------------------------------------------------------------------------------------------------//

	MKDEF_DECLARE_SCENE_OBJECT_HEADER; // MkSceneObject

	MkBodyFrameControlNode(const MkHashStr& name);
	virtual ~MkBodyFrameControlNode() {}

protected:

	void _SetBodyFrame
		(const MkHashStr& themeName, MkWindowThemeData::eComponentType componentType, const MkHashStr& customFormName,
		bool useShadow, eHangingType hangingType, const MkFloat2& clientSize);

	void _ApplyHangingType(void);

protected:

	eHangingType m_HangingType;

public:

	static const MkHashStr ObjKey_HangingType;
};
