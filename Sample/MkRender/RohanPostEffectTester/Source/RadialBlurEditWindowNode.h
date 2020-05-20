#pragma once

//------------------------------------------------------------------------------------------------//
// edit window - MainScene
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkTitleBarControlNode.h"


class RadialBlurEditWindowNode : public MkTitleBarControlNode
{
public:

	void SetUp(MkSceneNode* rootNode);

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	virtual void Activate(void);

	RadialBlurEditWindowNode(const MkHashStr& name) : MkTitleBarControlNode(name) { m_RootNode = NULL; }
	virtual ~RadialBlurEditWindowNode() {}

protected:

	void _ResetEffectValue(void);

protected:

	MkSceneNode* m_RootNode;
};
