#pragma once

#include "App02_GameDefinition.h"


//------------------------------------------------------------------------------------------------//
// system : game object
//------------------------------------------------------------------------------------------------//

class SystemGameObject
{
public:

	SystemGameObject(void) {}
	virtual ~SystemGameObject() {}

protected:
};


//------------------------------------------------------------------------------------------------//
// system : game object(+ grade & level)
//------------------------------------------------------------------------------------------------//

class SystemGradeGameObject : public SystemGameObject
{
public:

	virtual bool GradeSetUp(GameObjectDef::eGrade grade, GameObjectDef::eLevel level);

	virtual bool GradeUp(void);
	virtual bool GradeDown(void);

	virtual bool LevelUp(void);

	inline GameObjectDef::eGrade GetGrade(void) const { return m_Grade; }
	inline GameObjectDef::eLevel GetLevel(void) const { return m_Level; }

	SystemGradeGameObject(void);
	virtual ~SystemGradeGameObject() {}

protected:

	GameObjectDef::eGrade m_Grade;
	GameObjectDef::eLevel m_Level;
};
