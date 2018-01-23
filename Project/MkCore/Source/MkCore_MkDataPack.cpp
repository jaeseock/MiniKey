
#include "MkCore_MkDataUnit.h"
#include "MkCore_MkDataPack.h"


//------------------------------------------------------------------------------------------------//

template <class DataType>
class __TSI_DataUnitOperation
{
public:
	// 테이블에 존재하는 unit ptr 반환. 존재하지 않거나 타입이 다르면 NULL 반환
	static MkDataUnit<DataType>* FindUnit(const MkHashMap<MkHashStr, MkDataUnitInterface*>& unitTable, const MkHashStr& key)
	{
		if (unitTable.Exist(key) && (unitTable[key]->GetType() == MkPrimitiveDataType::GetEnum<DataType>()))
		{
			return dynamic_cast< MkDataUnit<DataType>* >(unitTable[key]);
		}
		return NULL;
	}

	// single data 할당
	static bool SetSingleData(MkHashMap<MkHashStr, MkDataUnitInterface*>& unitTable, const MkHashStr& key, const DataType& value, unsigned int index)
	{
		MkDataUnit<DataType>* unitPtr = FindUnit(unitTable, key);
		bool ok = (unitPtr != NULL);
		if (ok)
		{
			ok = unitPtr->SetValue(value, index);
		}
		return ok;
	}

	// data array 할당
	static bool SetDataArray(MkHashMap<MkHashStr, MkDataUnitInterface*>& unitTable, const MkHashStr& key, const MkArray<DataType>& values)
	{
		MkDataUnit<DataType>* unitPtr = FindUnit(unitTable, key);
		bool ok = (unitPtr != NULL);
		if (ok)
		{
			*unitPtr = values;
		}
		return ok;
	}

	// single data 반환
	static bool GetSingleData(const MkHashMap<MkHashStr, MkDataUnitInterface*>& unitTable, const MkHashStr& key, DataType& buffer, unsigned int index)
	{
		MkDataUnit<DataType>* unitPtr = FindUnit(unitTable, key);
		return (unitPtr != NULL) ? unitPtr->GetValue(buffer, index) : false;
	}

	// data array 반환
	static bool GetDataArray(const MkHashMap<MkHashStr, MkDataUnitInterface*>& unitTable, const MkHashStr& key, MkArray<DataType>& buffers)
	{
		MkDataUnit<DataType>* unitPtr = FindUnit(unitTable, key);
		bool ok = (unitPtr != NULL);
		if (ok)
		{
			buffers = unitPtr->GetArray();
		}
		return ok;
	}
};

//------------------------------------------------------------------------------------------------//

bool MkDataPack::IsValidKey(const MkHashStr& key) const { return m_UnitTable.Exist(key); }
unsigned int MkDataPack::GetUnitCount(void) const { return m_UnitTable.GetSize(); }
unsigned int MkDataPack::GetDataSize(const MkHashStr& key) const { return (m_UnitTable.Exist(key)) ? m_UnitTable[key]->GetSize() : 0; }
ePrimitiveDataType MkDataPack::GetUnitType(const MkHashStr& key) const { return (m_UnitTable.Exist(key)) ? m_UnitTable[key]->GetType() : ePDT_Undefined; }
unsigned int MkDataPack::GetUnitKeyList(MkArray<MkHashStr>& keyList) const { return m_UnitTable.GetKeyList(keyList); }

bool MkDataPack::CreateUnit(const MkHashStr& key, ePrimitiveDataType type, unsigned int size)
{
	bool ok = ((size > 0) && MkPrimitiveDataType::IsValid(type) && (!m_UnitTable.Exist(key)));
	if (ok)
	{
		MkDataUnitInterface* interfacePtr = NULL;
		switch (type)
		{
		case ePDT_Bool : interfacePtr = new MkDataUnit<bool>; break;
		case ePDT_Int : interfacePtr = new MkDataUnit<int>; break;
		case ePDT_UnsignedInt : interfacePtr = new MkDataUnit<unsigned int>; break;
		case ePDT_DInt : interfacePtr = new MkDataUnit<__int64>; break;
		case ePDT_DUnsignedInt : interfacePtr = new MkDataUnit<unsigned __int64>; break;
		case ePDT_Float : interfacePtr = new MkDataUnit<float>; break;
		case ePDT_Int2 : interfacePtr = new MkDataUnit<MkInt2>; break;
		case ePDT_Vec2 : interfacePtr = new MkDataUnit<MkVec2>; break;
		case ePDT_Vec3 : interfacePtr = new MkDataUnit<MkVec3>; break;
		case ePDT_Str : interfacePtr = new MkDataUnit<MkStr>; break;
		}
		assert(interfacePtr != NULL);
		m_UnitTable.Create(key, interfacePtr);
		interfacePtr->Expand(size);
	}
	return ok;
}

bool MkDataPack::Expand(const MkHashStr& key, unsigned int size)
{
	bool ok = m_UnitTable.Exist(key);
	if (ok)
	{
		m_UnitTable[key]->Expand(size);
	}
	return ok;
}

bool MkDataPack::SetData(const MkHashStr& key, bool value, unsigned int index)
{ return __TSI_DataUnitOperation<bool>::SetSingleData(m_UnitTable, key, value, index); }
bool MkDataPack::SetData(const MkHashStr& key, int value, unsigned int index)
{ return __TSI_DataUnitOperation<int>::SetSingleData(m_UnitTable, key, value, index); }
bool MkDataPack::SetData(const MkHashStr& key, unsigned int value, unsigned int index)
{ return __TSI_DataUnitOperation<unsigned int>::SetSingleData(m_UnitTable, key, value, index); }
bool MkDataPack::SetData(const MkHashStr& key, __int64 value, unsigned int index)
{ return __TSI_DataUnitOperation<__int64>::SetSingleData(m_UnitTable, key, value, index); }
bool MkDataPack::SetData(const MkHashStr& key, unsigned __int64 value, unsigned int index)
{ return __TSI_DataUnitOperation<unsigned __int64>::SetSingleData(m_UnitTable, key, value, index); }
bool MkDataPack::SetData(const MkHashStr& key, float value, unsigned int index)
{ return __TSI_DataUnitOperation<float>::SetSingleData(m_UnitTable, key, value, index); }
bool MkDataPack::SetData(const MkHashStr& key, const MkInt2& value, unsigned int index)
{ return __TSI_DataUnitOperation<MkInt2>::SetSingleData(m_UnitTable, key, value, index); }
bool MkDataPack::SetData(const MkHashStr& key, const MkVec2& value, unsigned int index)
{ return __TSI_DataUnitOperation<MkVec2>::SetSingleData(m_UnitTable, key, value, index); }
bool MkDataPack::SetData(const MkHashStr& key, const MkVec3& value, unsigned int index)
{ return __TSI_DataUnitOperation<MkVec3>::SetSingleData(m_UnitTable, key, value, index); }
bool MkDataPack::SetData(const MkHashStr& key, const MkStr& value, unsigned int index)
{ return __TSI_DataUnitOperation<MkStr>::SetSingleData(m_UnitTable, key, value, index); }

bool MkDataPack::SetData(const MkHashStr& key, const MkArray<bool>& values)
{ return __TSI_DataUnitOperation<bool>::SetDataArray(m_UnitTable, key, values); }
bool MkDataPack::SetData(const MkHashStr& key, const MkArray<int>& values)
{ return __TSI_DataUnitOperation<int>::SetDataArray(m_UnitTable, key, values); }
bool MkDataPack::SetData(const MkHashStr& key, const MkArray<unsigned int>& values)
{ return __TSI_DataUnitOperation<unsigned int>::SetDataArray(m_UnitTable, key, values); }
bool MkDataPack::SetData(const MkHashStr& key, const MkArray<__int64>& values)
{ return __TSI_DataUnitOperation<__int64>::SetDataArray(m_UnitTable, key, values); }
bool MkDataPack::SetData(const MkHashStr& key, const MkArray<unsigned __int64>& values)
{ return __TSI_DataUnitOperation<unsigned __int64>::SetDataArray(m_UnitTable, key, values); }
bool MkDataPack::SetData(const MkHashStr& key, const MkArray<float>& values)
{ return __TSI_DataUnitOperation<float>::SetDataArray(m_UnitTable, key, values); }
bool MkDataPack::SetData(const MkHashStr& key, const MkArray<MkInt2>& values)
{ return __TSI_DataUnitOperation<MkInt2>::SetDataArray(m_UnitTable, key, values); }
bool MkDataPack::SetData(const MkHashStr& key, const MkArray<MkVec2>& values)
{ return __TSI_DataUnitOperation<MkVec2>::SetDataArray(m_UnitTable, key, values); }
bool MkDataPack::SetData(const MkHashStr& key, const MkArray<MkVec3>& values)
{ return __TSI_DataUnitOperation<MkVec3>::SetDataArray(m_UnitTable, key, values); }
bool MkDataPack::SetData(const MkHashStr& key, const MkArray<MkStr>& values)
{ return __TSI_DataUnitOperation<MkStr>::SetDataArray(m_UnitTable, key, values); }

bool MkDataPack::GetData(const MkHashStr& key, bool& buffer, unsigned int index) const
{ return __TSI_DataUnitOperation<bool>::GetSingleData(m_UnitTable, key, buffer, index); }
bool MkDataPack::GetData(const MkHashStr& key, int& buffer, unsigned int index) const
{ return __TSI_DataUnitOperation<int>::GetSingleData(m_UnitTable, key, buffer, index); }
bool MkDataPack::GetData(const MkHashStr& key, unsigned int& buffer, unsigned int index) const
{ return __TSI_DataUnitOperation<unsigned int>::GetSingleData(m_UnitTable, key, buffer, index); }
bool MkDataPack::GetData(const MkHashStr& key, __int64& buffer, unsigned int index) const
{ return __TSI_DataUnitOperation<__int64>::GetSingleData(m_UnitTable, key, buffer, index); }
bool MkDataPack::GetData(const MkHashStr& key, unsigned __int64& buffer, unsigned int index) const
{ return __TSI_DataUnitOperation<unsigned __int64>::GetSingleData(m_UnitTable, key, buffer, index); }
bool MkDataPack::GetData(const MkHashStr& key, float& buffer, unsigned int index) const
{ return __TSI_DataUnitOperation<float>::GetSingleData(m_UnitTable, key, buffer, index); }
bool MkDataPack::GetData(const MkHashStr& key, MkInt2& buffer, unsigned int index) const
{ return __TSI_DataUnitOperation<MkInt2>::GetSingleData(m_UnitTable, key, buffer, index); }
bool MkDataPack::GetData(const MkHashStr& key, MkVec2& buffer, unsigned int index) const
{ return __TSI_DataUnitOperation<MkVec2>::GetSingleData(m_UnitTable, key, buffer, index); }
bool MkDataPack::GetData(const MkHashStr& key, MkVec3& buffer, unsigned int index) const
{ return __TSI_DataUnitOperation<MkVec3>::GetSingleData(m_UnitTable, key, buffer, index); }
bool MkDataPack::GetData(const MkHashStr& key, MkStr& buffer, unsigned int index) const
{ return __TSI_DataUnitOperation<MkStr>::GetSingleData(m_UnitTable, key, buffer, index); }

bool MkDataPack::GetData(const MkHashStr& key, MkArray<bool>& buffers) const
{ return __TSI_DataUnitOperation<bool>::GetDataArray(m_UnitTable, key, buffers); }
bool MkDataPack::GetData(const MkHashStr& key, MkArray<int>& buffers) const
{ return __TSI_DataUnitOperation<int>::GetDataArray(m_UnitTable, key, buffers); }
bool MkDataPack::GetData(const MkHashStr& key, MkArray<unsigned int>& buffers) const
{ return __TSI_DataUnitOperation<unsigned int>::GetDataArray(m_UnitTable, key, buffers); }
bool MkDataPack::GetData(const MkHashStr& key, MkArray<__int64>& buffers) const
{ return __TSI_DataUnitOperation<__int64>::GetDataArray(m_UnitTable, key, buffers); }
bool MkDataPack::GetData(const MkHashStr& key, MkArray<unsigned __int64>& buffers) const
{ return __TSI_DataUnitOperation<unsigned __int64>::GetDataArray(m_UnitTable, key, buffers); }
bool MkDataPack::GetData(const MkHashStr& key, MkArray<float>& buffers) const
{ return __TSI_DataUnitOperation<float>::GetDataArray(m_UnitTable, key, buffers); }
bool MkDataPack::GetData(const MkHashStr& key, MkArray<MkInt2>& buffers) const
{ return __TSI_DataUnitOperation<MkInt2>::GetDataArray(m_UnitTable, key, buffers); }
bool MkDataPack::GetData(const MkHashStr& key, MkArray<MkVec2>& buffers) const
{ return __TSI_DataUnitOperation<MkVec2>::GetDataArray(m_UnitTable, key, buffers); }
bool MkDataPack::GetData(const MkHashStr& key, MkArray<MkVec3>& buffers) const
{ return __TSI_DataUnitOperation<MkVec3>::GetDataArray(m_UnitTable, key, buffers); }
bool MkDataPack::GetData(const MkHashStr& key, MkArray<MkStr>& buffers) const
{ return __TSI_DataUnitOperation<MkStr>::GetDataArray(m_UnitTable, key, buffers); }

bool MkDataPack::RemoveUnit(const MkHashStr& key)
{
	bool ok = m_UnitTable.Exist(key);
	if (ok)
	{
		delete m_UnitTable[key];
		m_UnitTable.Erase(key);
	}
	return ok;
}

bool MkDataPack::RemoveData(const MkHashStr& key, unsigned int index)
{
	bool ok = m_UnitTable.Exist(key);
	if (ok)
	{
		ok = m_UnitTable[key]->RemoveValue(index);
		if (ok && (m_UnitTable[key]->GetSize() == 0))
		{
			delete m_UnitTable[key];
			m_UnitTable.Erase(key);
		}
	}
	return ok;
}

void MkDataPack::Rehash(void) { m_UnitTable.Rehash(); }

void MkDataPack::Clear(void)
{
	MkHashMapLooper<MkHashStr, MkDataUnitInterface*> looper(m_UnitTable);
	MK_STL_LOOP(looper)
	{
		delete looper.GetCurrentField();
	}
	m_UnitTable.Clear();
}

MkDataUnitInterface* MkDataPack::GetUnitCopy(const MkHashStr& key) const
{
	return m_UnitTable.Exist(key) ? m_UnitTable[key]->CreateCopy() : NULL;
}

bool MkDataPack::Equals(const MkHashStr& key, const MkDataUnitInterface& source) const
{
	return m_UnitTable.Exist(key) ? (source == *m_UnitTable[key]) : false;
}

bool MkDataPack::Equals(const MkDataPack& source, const MkHashStr& key) const
{
	return m_UnitTable.Exist(key) ? source.Equals(key, *m_UnitTable[key]) : false;
}

MkDataPack& MkDataPack::operator = (const MkDataPack& source)
{
	Clear();
	MkArray<MkHashStr> keyList;
	source.GetUnitKeyList(keyList);
	MK_INDEXING_LOOP(keyList, i)
	{
		const MkHashStr& currKey = keyList[i];
		m_UnitTable.Create(currKey, source.GetUnitCopy(currKey));
	}
	return *this;
}

bool MkDataPack::operator == (const MkDataPack& source) const
{
	if (GetUnitCount() != source.GetUnitCount())
		return false;

	MkArray<MkHashStr> myOwn;
	GetUnitKeyList(myOwn);
	MK_INDEXING_LOOP(myOwn, i)
	{
		if (!source.IsValidKey(myOwn[i]))
			return false;
	}
	MK_INDEXING_LOOP(myOwn, i)
	{
		const MkHashStr& currKey = myOwn[i];
		if (!source.Equals(currKey, *m_UnitTable[currKey]))
			return false;
	}
	return true;
}

void MkDataPack::GetValidUnitList(const MkDataPack& source, MkArray<MkHashStr>& keyList) const
{
	keyList.Clear();
	MkArray<MkHashStr> myOwn;
	GetUnitKeyList(myOwn);
	if (!myOwn.Empty())
	{
		keyList.Reserve(myOwn.GetSize());
		MK_INDEXING_LOOP(myOwn, i)
		{
			const MkHashStr& currKey = myOwn[i];
			if (!(source.IsValidKey(currKey) && source.Equals(currKey, *m_UnitTable[currKey])))
			{
				keyList.PushBack(currKey); // 자신에게만 속하거나 값이 다른 key
			}
		}
	}
}

//------------------------------------------------------------------------------------------------//
