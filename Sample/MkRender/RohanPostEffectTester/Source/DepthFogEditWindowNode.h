#pragma once

//------------------------------------------------------------------------------------------------//
// edit window - DepthFog
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkTitleBarControlNode.h"


class DepthFogEditWindowNode : public MkTitleBarControlNode
{
public:

	void SetUp(MkSceneNode* rootNode);

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	virtual void Activate(void);

	DepthFogEditWindowNode(const MkHashStr& name) : MkTitleBarControlNode(name) { m_RootNode = NULL; }
	virtual ~DepthFogEditWindowNode() {}

protected:

	void _ResetEffectValue(void);

	void _UpdateGraph(void);
	void _SetFogColor(const MkColor& color);
	void _UpdateColorTag(void);

protected:

	MkSceneNode* m_RootNode;
};
