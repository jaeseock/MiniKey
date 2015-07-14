#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "App02_GamePageBase.h"


class MkProgressBarNode;

class GamePageAppStart : public GamePageBase
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(MkDataNode* sharingNode = NULL);

	GamePageAppStart(void);
	virtual ~GamePageAppStart() {}

protected:

	enum ePageState
	{
		ePS_ShowInitMsg = 0,
		ePS_LoadGlobalResource,
		ePS_MoveToNextPage
	};

	void _SetLoadedResCount(int count);

protected:

	ePageState m_PageState;

	int m_LoadedResCount;
	int m_TotalResCount;

	MkProgressBarNode* m_ProgressBarNode;
};
