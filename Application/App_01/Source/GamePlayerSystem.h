#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "GameWizardGroupInfo.h"
#include "GameAgentGroupInfo.h"


//------------------------------------------------------------------------------------------------//

class GamePlayerBase
{
public:

	virtual const GameWizardUnitInfo& GetWizardInfo(void) const = NULL;
	virtual const MkArray<GameAgentUnitInfo>& GetAgentInfo(void) const = NULL;

	GamePlayerBase() {}
	virtual ~GamePlayerBase() {}
};

//------------------------------------------------------------------------------------------------//

class GameNormalPlayer : public GamePlayerBase
{
public:

	void SetUp(const GameWizardUnitInfo& wizard, const MkArray<GameAgentUnitInfo>& agent);

	virtual const GameWizardUnitInfo& GetWizardInfo(void) const { return m_WizardInfo; }
	virtual const MkArray<GameAgentUnitInfo>& GetAgentInfo(void) const { return m_AgentInfo; }

	GameNormalPlayer() : GamePlayerBase() {}
	virtual ~GameNormalPlayer() {}

protected:

	GameWizardUnitInfo m_WizardInfo;
	MkArray<GameAgentUnitInfo> m_AgentInfo;
};

//------------------------------------------------------------------------------------------------//

class GameMasterPlayer : public GamePlayerBase
{
public:

	bool Load(const MkDataNode& node);
	bool Save(MkDataNode& node) const;

	inline GameWizardGroupInfo& GetWizardGroup(void) { return m_WizardGroup; }
	inline const GameWizardGroupInfo& GetWizardGroup(void) const { return m_WizardGroup; }

	inline GameAgentGroupInfo& GetAgentGroup(void) { return m_AgentGroup; }
	inline const GameAgentGroupInfo& GetAgentGroup(void) const { return m_AgentGroup; }

	virtual const GameWizardUnitInfo& GetWizardInfo(void) const { return m_WizardGroup.GetWizardInfo(); }
	virtual const MkArray<GameAgentUnitInfo>& GetAgentInfo(void) const { return m_AgentGroup.GetAgentInfo(); }

	void Clear(void);

	GameMasterPlayer() : GamePlayerBase() {}
	virtual ~GameMasterPlayer() { Clear(); }

protected:

	GameWizardGroupInfo m_WizardGroup;
	GameAgentGroupInfo m_AgentGroup;
};

//------------------------------------------------------------------------------------------------//