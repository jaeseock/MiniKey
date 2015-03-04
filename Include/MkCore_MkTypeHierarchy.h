#pragma once

//------------------------------------------------------------------------------------------------//
// type hierarchy
// 계층 구조 참조자
// RTTI에서 가장 중요한 것은 'A는 B의 파생 class인가?'이지만 dynamic_cast로 판별하기에는 비용이 너무 큼
//
// (NOTE) 모든 key는 전체에 있어서 unique 해야 함
//
// ex>
//	// 생성
//	MkTypeHierarchy<MkHashStr> typeHierarchy;
//
//	// 등록
//	typeHierarchy.SetHierarchy(L"Car");
//	typeHierarchy.SetHierarchy(L"Car", L"Power");
//	typeHierarchy.SetHierarchy(L"Car", L"Speed");
//	typeHierarchy.SetHierarchy(L"Power", L"Truck");
//	typeHierarchy.SetHierarchy(L"Power", L"Tank");
//	typeHierarchy.SetHierarchy(L"Speed", L"Taxi");
//	typeHierarchy.SetHierarchy(L"Speed", L"Racing");
//	typeHierarchy.SetHierarchy(L"Racing", L"SilverArrow");
//
//	// 사용
//	bool ok = typeHierarchy.IsDerivedFrom(L"Car", L"Truck"); // true;
//	bool ok = typeHierarchy.IsDerivedFrom(L"Power", L"Tank"); // true;
//	bool ok = typeHierarchy.IsDerivedFrom(L"Power", L"Racing"); // false;
//	bool ok = typeHierarchy.IsDerivedFrom(L"Speed", L"SilverArrow"); // true;
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"


template<class DataType>
class MkTypeHierarchy
{
public:

	typedef struct _TypeInfo
	{
		DataType directBase;
		MkMap<DataType, const _TypeInfo*> baseList;
	}
	TypeInfo;

public:

	// root type 선언
	inline void SetHierarchy(const DataType& rootType)
	{
		m_RootType = rootType;
		m_Table.Create(m_RootType);
	}

	// derived type 선언
	inline bool SetHierarchy(const DataType& sourceType, const DataType& delivedType)
	{
		bool ok = m_Table.Exist(sourceType) && (!m_Table.Exist(delivedType));
		if (ok)
		{
			TypeInfo& dInfo = m_Table.Create(delivedType);
			dInfo.directBase = sourceType;
			
			// 직계 base type을 모두 등록
			DataType targetType = sourceType;
			while (true)
			{
				if (!m_Table.Exist(targetType))
					return false;

				TypeInfo& tInfo = m_Table[targetType];
				dInfo.baseList.Create(targetType, &tInfo);

				if (targetType != m_RootType)
				{
					targetType = tInfo.directBase;
				}
				else
					break;
			}
		}
		return ok;
	}

	// delivedType이 sourceType의 파생 type인지 여부를 반환
	inline bool IsDerivedFrom(const DataType& sourceType, const DataType& delivedType)
	{
		return (sourceType == delivedType) ? true : (m_Table.Exist(delivedType) ? m_Table[delivedType].baseList.Exist(sourceType) : false);
	}

	MkTypeHierarchy() {}
	MkTypeHierarchy(const DataType& rootType)
	{
		SetHierarchy(rootType);
	}

	~MkTypeHierarchy()
	{
		m_Table.Clear();
	}

public:

	MkMap<DataType, TypeInfo> m_Table;
	DataType m_RootType;
};

//------------------------------------------------------------------------------------------------//
