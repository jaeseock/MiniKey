#pragma once


//------------------------------------------------------------------------------------------------//
// window base node
// window system의 action part의 base class
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowThemedNode.h"


class MkWindowBaseNode : public MkWindowThemedNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_WindowBaseNode; }

	// alloc child instance
	static MkWindowBaseNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// pick
	//------------------------------------------------------------------------------------------------//

	// 자신과 하위 모든 node들 중 eAT_AcceptInput 속성을 가진 node들을 대상으로 picking을 실행해 검출된 node를
	// 소유하고 있는 MkWindowBaseNode들을 반환
	// (NOTE) 규칙 및 argument 설명은 MkSceneNode::PickPanel() 참조
	bool PickWindowBaseNode(MkArray<MkWindowBaseNode*>& buffer, const MkFloat2& worldPoint, float startDepth = 0.f) const;

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	enum eWindowBaseNodeAttribute
	{
		eAT_Enable = eAT_WindowThemedNodeBandwidth,

		eAT_WindowBaseNodeBandwidth = eAT_WindowThemedNodeBandwidth + 4 // 4bit 대역폭 확보
	};

	// enable. default는 true
	inline void SetEnable(bool enable) { m_Attribute.Assign(eAT_Enable, enable); }
	inline bool GetEnable(void) const { return m_Attribute[eAT_Enable]; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	enum eWindowBaseNodeEventType
	{
		eET_WindowBaseNodeBandwidth = eET_WindowThemedNodeBandwidth
	};

	//virtual void SendRootToLeafDirectionNodeEvent(int eventType, MkDataNode& argument);

	// 해제
	virtual void Clear(void);

	MkWindowBaseNode(const MkHashStr& name);
	virtual ~MkWindowBaseNode() { Clear(); }

protected:
};
