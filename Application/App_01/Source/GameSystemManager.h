#pragma once


//------------------------------------------------------------------------------------------------//
// text ют╥б©К edit box
//------------------------------------------------------------------------------------------------//

//#include "MkCore_MkHashStr.h"
#include "MkCore_MkSingletonPattern.h"

#include "GameWizardGroupInfo.h"
#include "GameAgentGroupInfo.h"


#define GAME_SYSTEM GameSystemManager::Instance()

//------------------------------------------------------------------------------------------------//

class GamePlayerBase
{
public:

	virtual const GameWizardUnitInfo* GetTargetWizardInfo(void) const = NULL;
	virtual void GetTroopers(MkArray<GameAgentUnitInfo>& buffer) const = NULL;

	GamePlayerBase() {}
	virtual ~GamePlayerBase() {}
};

//------------------------------------------------------------------------------------------------//

class NormalPlayer : public GamePlayerBase
{
public:

	void SetUp(const GameWizardUnitInfo& wizard, const MkArray<GameAgentUnitInfo>& troop);

	virtual const GameWizardUnitInfo* GetTargetWizardInfo(void) const { return &m_WizardInfo; }
	virtual void GetTroopers(MkArray<GameAgentUnitInfo>& buffer) const { buffer = m_TroopInfo; }

	NormalPlayer() : GamePlayerBase() {}
	virtual ~NormalPlayer() {}

protected:

	GameWizardUnitInfo m_WizardInfo;
	MkArray<GameAgentUnitInfo> m_TroopInfo;
};

//------------------------------------------------------------------------------------------------//

class MasterPlayer : public GamePlayerBase
{
public:

	bool Load(const MkDataNode& node);
	bool Save(MkDataNode& node) const;

	inline GameWizardGroupInfo& GetWizardGroup(void) { return m_WizardGroup; }
	inline const GameWizardGroupInfo& GetWizardGroup(void) const { return m_WizardGroup; }

	inline GameAgentGroupInfo& GetAgentGroup(void) { return m_AgentGroup; }
	inline const GameAgentGroupInfo& GetAgentGroup(void) const { return m_AgentGroup; }

	virtual const GameWizardUnitInfo* GetTargetWizardInfo(void) const { return m_WizardGroup.GetTargetWizardInfo(); }
	virtual void GetTroopers(MkArray<GameAgentUnitInfo>& buffer) const { m_AgentGroup.GetTroopers(buffer); }

	void Clear(void);

	MasterPlayer() : GamePlayerBase() {}
	virtual ~MasterPlayer() { Clear(); }

protected:

	GameWizardGroupInfo m_WizardGroup;
	GameAgentGroupInfo m_AgentGroup;
};

//------------------------------------------------------------------------------------------------//

class MkPathName;

class GameSystemManager : public MkSingletonPattern<GameSystemManager>
{
public:

	inline MasterPlayer& GetMasterPlayer(void) { return m_MasterPlayer; }
	inline const MasterPlayer& GetMasterPlayer(void) const { return m_MasterPlayer; }

	bool LoadMasterUserData(const MkPathName& filePath);
	bool SaveMasterUserData(const MkPathName& filePath) const;

	GameSystemManager() {}
	virtual ~GameSystemManager() {}

protected:

	MasterPlayer m_MasterPlayer;

	// battle
	MkArray<NormalPlayer> m_Ally;
	MkArray<NormalPlayer> m_Enemy;
};

//------------------------------------------------------------------------------------------------//