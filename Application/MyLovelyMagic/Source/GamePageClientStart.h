#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBasePage.h"
#include "MkCore_MkTimeCounter.h"


class MkSceneNode;

class GamePageClientStart : public MkBasePage
{
public:

	static const MkHashStr Name;

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(void);

	GamePageClientStart();
	virtual ~GamePageClientStart() { Clear(); }

protected:

	enum eState
	{
		eUnset = 0,
		eDrawStartMsg,
		eLoadAssets,
		eShowEffect,
		eOnEffect,
		eMoveToNextGamePage
	};

	void _LoadAssets(void);

protected:

	MkSceneNode* m_SceneNode;

	MkTimeCounter m_TimeCounter;

	eState m_State;
};