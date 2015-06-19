#pragma once

//------------------------------------------------------------------------------------------------//
// 간단한 RTTI 사용
//------------------------------------------------------------------------------------------------//

#include <typeinfo>
#include "MkCore_MkStr.h"


template<class ClassType>
inline MkStr GetClassNameOfInstance(const ClassType& instance)
{
	MkStr nameStr;
	nameStr.ImportMultiByteString(typeid(instance).name());
	if (nameStr.CheckPrefix(L"class "))
	{
		nameStr.PopFront(6);
	}
	else
	{
		nameStr.Clear();
	}
	return nameStr;
}

#define MK_CLASS_NAME(classType) GetClassNameOfInstance<classType>(*this)

//------------------------------------------------------------------------------------------------//
