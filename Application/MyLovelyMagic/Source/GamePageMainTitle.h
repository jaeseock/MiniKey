#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBasePage.h"


class MkSceneNode;

class GamePageMainTitle : public MkBasePage
{
public:

	static const MkHashStr Name;

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(void);

	GamePageMainTitle();
	virtual ~GamePageMainTitle() { Clear(); }

protected:

	enum eState
	{
		eUnset = 0,
		eDrawWelcomeMsg,
		eLoadAssets,
		eMoveToNextGamePage
	};

	void _LoadAssets(void);

protected:

	MkSceneNode* m_SceneNode;

	eState m_State;

	MkSceneNode* m_TestNode;
};