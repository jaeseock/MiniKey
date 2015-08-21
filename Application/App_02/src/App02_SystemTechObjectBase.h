#pragma once

//------------------------------------------------------------------------------------------------//
// system : tech base
// system tech¿« ±‚π›
//------------------------------------------------------------------------------------------------//

#include "App02_SystemGameObject.h"


class SystemTechObjectBase : public SystemGradeGameObject
{
public:

	enum eTechType
	{
		eTT_Command = 0,
		eTT_Supplement,
		eTT_Soldier,
		eTT_Tactics,

		eTT_Max
	};

	SystemTechObjectBase(void);
	virtual ~SystemTechObjectBase() {}

protected:

	
};
