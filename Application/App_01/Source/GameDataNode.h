#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

class MkDataNode;

class GameDataNode
{
public:

	static MkDataNode* WizardSet;
	static MkDataNode* AgentSet;

	static void SetUp(void);
	static void Clear(void);
};