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

	DepthFogEditWindowNode(const MkHashStr& name) : MkTitleBarControlNode(name) { m_RootNode = NULL; m_Reloading = false; }
	virtual ~DepthFogEditWindowNode() {}

	bool UpdateMultGradation(void);
	bool UpdateAddGradation(void);

protected:

	void _UpdateMapDir(const MkPathName& dirPath, const MkHashStr& ddListName);

	void _ResetEffectValue(void);

protected:

	MkPathName m_MultMapPath;
	MkPathName m_AddMapPath;

	bool m_Reloading;

	MkSceneNode* m_RootNode;
};
