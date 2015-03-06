#pragma once


//------------------------------------------------------------------------------------------------//
// window base node
// window system�� action part�� base class
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

	// �ڽŰ� ���� ��� node�� �� eAT_AcceptInput �Ӽ��� ���� node���� ������� picking�� ������ ����� node��
	// �����ϰ� �ִ� MkWindowBaseNode���� ��ȯ
	// (NOTE) ��Ģ �� argument ������ MkSceneNode::PickPanel() ����
	bool PickWindowBaseNode(MkArray<MkWindowBaseNode*>& buffer, const MkFloat2& worldPoint, float startDepth = 0.f) const;

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	enum eWindowBaseNodeAttribute
	{
		eAT_Enable = eAT_WindowThemedNodeBandwidth,

		eAT_WindowBaseNodeBandwidth = eAT_WindowThemedNodeBandwidth + 4 // 4bit �뿪�� Ȯ��
	};

	// enable. default�� true
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

	// ����
	virtual void Clear(void);

	MkWindowBaseNode(const MkHashStr& name);
	virtual ~MkWindowBaseNode() { Clear(); }

protected:
};
