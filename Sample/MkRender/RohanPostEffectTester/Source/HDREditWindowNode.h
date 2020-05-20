#pragma once

//------------------------------------------------------------------------------------------------//
// edit window - MainScene
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkTitleBarControlNode.h"


class HDREditWindowNode : public MkTitleBarControlNode
{
public:

	void SetUp(MkSceneNode* rootNode);

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	virtual void Activate(void);

	HDREditWindowNode(const MkHashStr& name) : MkTitleBarControlNode(name) { m_RootNode = NULL; }
	virtual ~HDREditWindowNode() {}

protected:

	void _ResetEffectValue(void);

protected:

	MkSceneNode* m_RootNode;
};
