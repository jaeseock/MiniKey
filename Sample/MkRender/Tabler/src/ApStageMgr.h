#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkType2.h"
#include "MkCore_MkUniformDice.h"
#include "MkCore_MkTimeCounter.h"

#include "ApField.h"
#include "ApPlayer.h"


class ApStageMgr
{
public:

	// field
	bool SetField(const MkUInt2& size, MkSceneNode* rootNode);

	// player
	bool AddPlayer(const MkHashStr& name, ApPlayer::eStandPosition standPos, ApResourceUnit::eTeamColor teamColor, unsigned int initResSlotSize, unsigned int initResCount);
	ApPlayer* GetPlayer(unsigned int index);

	// flow
	enum eMainState
	{
		eMS_Start = 0,
		eMS_Welcome,
		eMS_Play,
		eMS_Result,
		eMS_Exit
	};

	enum eTurnState
	{
		eTS_Start = 0,
		eTS_GenerateResource,
		eTS_Action,
		eTS_ApplyEvent,
		eTS_Next
	};

	void SetFlowInfo(unsigned int turnPerRound, unsigned int maxRound);

	void Update(const MkTimeState& timeState);

	void Clear(void);

	ApStageMgr();
	virtual ~ApStageMgr() { Clear(); }

protected:

	bool _GenerateResource(unsigned int index);

	void _UpdateCurrentTurnInfoText(unsigned int currPlayer);
	void _SetMessageText(const MkStr& msg, const MkTimeState& timeState);

	void _UpdateMS_Start(const MkTimeState& timeState);
	void _UpdateMS_Welcome(const MkTimeState& timeState);
	void _UpdateMS_Play(const MkTimeState& timeState);
	void _UpdateMS_Result(const MkTimeState& timeState);
	void _UpdateMS_Exit(const MkTimeState& timeState);

protected:

	MkSceneNode* m_StageNode;

	// field
	ApField m_Field;

	// player
	MkArray<ApPlayer> m_Players;

	// resource
	unsigned int m_CurrResourceID; // auto incremental index
	MkUniformDice m_ResourceDice;

	// game flow
	unsigned int m_TurnPerRound;
	unsigned int m_MaxRound;

	unsigned int m_CurrTurn;
	eMainState m_MainState;
	eTurnState m_TurnState;
	MkTimeCounter m_FlowCounter;

	// message
	MkTimeCounter m_MsgCounter;

};
