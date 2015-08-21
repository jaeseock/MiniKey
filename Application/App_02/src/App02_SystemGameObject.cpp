
#include "App02_SystemGameObject.h"


//------------------------------------------------------------------------------------------------//

bool SystemGradeGameObject::GradeSetUp(GameObjectDef::eGrade grade, GameObjectDef::eLevel level)
{
	m_Grade = grade;
	m_Level = level;
	return false;
}

bool SystemGradeGameObject::GradeUp(void)
{
	return true;
}

bool SystemGradeGameObject::GradeDown(void)
{
	return true;
}

bool SystemGradeGameObject::LevelUp(void)
{
	return true;
}

SystemGradeGameObject::SystemGradeGameObject(void) : SystemGameObject()
{
	GradeSetUp(GameObjectDef::eG_Basic, GameObjectDef::eL_Noob);
}

//------------------------------------------------------------------------------------------------//
