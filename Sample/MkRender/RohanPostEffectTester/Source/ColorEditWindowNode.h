#pragma once

//------------------------------------------------------------------------------------------------//
// edit window - MainScene
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkTitleBarControlNode.h"


class ColorEditWindowNode : public MkTitleBarControlNode
{
public:

	void SetUp(MkSceneNode* rootNode);

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	virtual void Activate(void);

	ColorEditWindowNode(const MkHashStr& name) : MkTitleBarControlNode(name) { m_RootNode = NULL; }
	virtual ~ColorEditWindowNode() {}

protected:

	static int _ValueToFactor(float value);
	static float _FactorToValue(int factor);

	void _ResetEffectValue(void);

protected:

	MkSceneNode* m_RootNode;
};
