#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBasePage.h"

#include "ApStageMgr.h"


class ApStagePlayPage : public MkBasePage
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(MkDataNode* sharingNode = NULL);

	ApStagePlayPage(const MkHashStr& name);

	virtual ~ApStagePlayPage() { Clear(); }

protected:

	// scene
	MkSceneNode* m_SceneRootNode;

	ApStageMgr m_StageMgr;

};
