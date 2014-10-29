#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"

#include "GameHitPointGage.h"
#include "GameStateType.h"


class MkDataNode;
class GameNormalPlayer;

class GameTroopState
{
public:

	bool SetUp(const GameNormalPlayer* player);

	void Clear(void);

	void UpdateTroopState(void);

	
	GameTroopState();
	~GameTroopState() { Clear(); }

protected:

	void _SetState(const MkDataNode* baseState, const MkHashStr& key, int defValue, int minCap, int maxCap);

protected:

	const GameNormalPlayer* m_Player;

	GameHitPointGage m_HitPointGage;

	MkHashMap<MkHashStr, IntStateType> m_StateTable;
};