#pragma once

//------------------------------------------------------------------------------------------------//
// type hierarchy
// ���� ���� ������
// RTTI���� ���� �߿��� ���� 'A�� B�� �Ļ� class�ΰ�?'������ dynamic_cast�� �Ǻ��ϱ⿡�� ����� �ʹ� ŭ
//
// (NOTE) ��� key�� ��ü�� �־ unique �ؾ� ��
//
// ex>
//	// ����
//	MkTypeHierarchy<MkHashStr> typeHierarchy;
//
//	// ���
//	typeHierarchy.SetHierarchy(L"Car");
//	typeHierarchy.SetHierarchy(L"Car", L"Power");
//	typeHierarchy.SetHierarchy(L"Car", L"Speed");
//	typeHierarchy.SetHierarchy(L"Power", L"Truck");
//	typeHierarchy.SetHierarchy(L"Power", L"Tank");
//	typeHierarchy.SetHierarchy(L"Speed", L"Taxi");
//	typeHierarchy.SetHierarchy(L"Speed", L"Racing");
//	typeHierarchy.SetHierarchy(L"Racing", L"SilverArrow");
//
//	// ���
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

	// root type ����
	inline void SetHierarchy(const DataType& rootType)
	{
		m_RootType = rootType;
		m_Table.Create(m_RootType);
	}

	// derived type ����
	inline bool SetHierarchy(const DataType& sourceType, const DataType& delivedType)
	{
		bool ok = m_Table.Exist(sourceType) && (!m_Table.Exist(delivedType));
		if (ok)
		{
			TypeInfo& dInfo = m_Table.Create(delivedType);
			dInfo.directBase = sourceType;
			
			// ���� base type�� ��� ���
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

	// delivedType�� sourceType�� �Ļ� type���� ���θ� ��ȯ
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
