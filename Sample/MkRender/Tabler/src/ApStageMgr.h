#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkType2.h"
#include "MkCore_MkUniformDice.h"

#include "ApField.h"
#include "ApPlayer.h"


class ApStageMgr
{
public:

	// field
	bool SetField(const MkUInt2& size, MkSceneNode* rootNode);

	// player
	bool AddPlayer(const MkHashStr& name, ApPlayer::eStandPosition standPos, ApResourceUnit::eTeamColor teamColor, unsigned int initResSlotSize);
	ApPlayer* GetPlayer(unsigned int index);

	// flow
	enum eMainState
	{
		eMS_None = -1,
		eMS_Start = 0,
		eMS_Play,
		eMS_Result,
		eMS_Exit
	};

	enum ePlayerState
	{
		ePS_None = -1,
		ePS_GenerateResource = 0,
		ePS_Action,
		ePS_ApplyEvent,
		ePS_Next
	};

	void SetFlowInfo(unsigned int turnPerRound, unsigned int maxRound);

	void Update(const MkTimeState& timeState);

	void Clear(void);

	ApStageMgr();
	virtual ~ApStageMgr() { Clear(); }

protected:

	bool _GenerateRandomResource(unsigned int playerIndex, unsigned int count, bool showMsg, const MkTimeState& timeState);

	void _UpdateCurrentTurnInfoText(bool showFlowTime, const MkTimeState& timeState);
	void _SetMessageText(const MkStr& msg, const MkTimeState& timeState);

	void _UpdateMS_Start(const MkTimeState& timeState);
	void _UpdateMS_Play(const MkTimeState& timeState);
	void _UpdateMS_Result(const MkTimeState& timeState);
	void _UpdateMS_Exit(const MkTimeState& timeState);
	void _UpdateMS_All(const MkTimeState& timeState);

	void _UpdatePS_GenerateResource(const MkTimeState& timeState);
	void _UpdatePS_Action(const MkTimeState& timeState);
	void _UpdatePS_ApplyEvent(const MkTimeState& timeState);
	void _UpdatePS_Next(const MkTimeState& timeState);

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
	eMainState m_CurrMainState;
	eMainState m_LastMainState;

	unsigned int m_CurrPlayerIndex;
	ePlayerState m_CurrPlayerState;
	ePlayerState m_LastPlayerState;
	MkTimeCounter m_FlowCounter;

	// message
	MkTimeCounter m_MsgCounter;

};
