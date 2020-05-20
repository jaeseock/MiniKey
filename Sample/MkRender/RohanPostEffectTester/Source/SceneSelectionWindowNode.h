#pragma once

//------------------------------------------------------------------------------------------------//
// window - scene selection
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkTitleBarControlNode.h"


class SceneSelectionWindowNode : public MkTitleBarControlNode
{
public:

	void SetUp(MkDataNode& setting, const MkArray<MkSceneNode*>& rootNode);

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	virtual void Deactivate(void);

	bool IsValidScene(const MkHashStr& key) const { return m_ItemInfo.Exist(key); }

	void SelectTargetScene(void);

	SceneSelectionWindowNode(const MkHashStr& name) : MkTitleBarControlNode(name) {}
	virtual ~SceneSelectionWindowNode() {}

protected:

	void _UpdateScenes(const MkPathName& dirPath);
	bool _CheckPairScene
		(const MkArray<MkStr>& fileNameList, unsigned int index, MkArray<bool>& skipTag, const MkStr& positfixA, const MkStr& positfixB, MkArray<MkHashStr>& newKeyList) const;

	typedef struct __ItemInfo
	{
		MkHashStr baseKey;
		MkHashStr depthKey;
		bool loaded;
	}
	_ItemInfo;
	
protected:

	MkArray<MkSceneNode*> m_RootNode;

	MkPathName m_ScreenShotPath;

	MkHashMap<MkHashStr, _ItemInfo> m_ItemInfo;
};
