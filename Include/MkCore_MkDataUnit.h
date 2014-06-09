#ifndef __MINIKEY_CORE_MKDATAUNIT_H__
#define __MINIKEY_CORE_MKDATAUNIT_H__


//------------------------------------------------------------------------------------------------//
// data unit
// 한 종류의 값에 대한 정의
// 배열 형태
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkDataTypeDefinition.h"


//------------------------------------------------------------------------------------------------//
// data unit의 interface 정의
//------------------------------------------------------------------------------------------------//

class MkDataUnitInterface
{
public:

	// 값 타입 반환
	virtual ePrimitiveDataType GetType(void) const { return ePDT_Undefined; }

	// 값 개수 반환
	virtual unsigned int GetSize(void) const { return 0; }

	// size만큼 값을 확장
	virtual void Expand(unsigned int size) = NULL;

	// index 위치의 값을 삭제 후 성공여부 반환
	virtual bool RemoveValue(unsigned int index) = NULL;

	// 해당 unit 객체의 복사본 반환
	virtual MkDataUnitInterface* CreateCopy(void) const = NULL;

	// 비교 연산자
	virtual bool operator == (const MkDataUnitInterface& source) const = NULL;
	inline bool operator != (const MkDataUnitInterface& source) const { return !operator==(source); }

	MkDataUnitInterface() {}
	virtual ~MkDataUnitInterface() {}
};

//------------------------------------------------------------------------------------------------//
// 실제 값 배열을 가진 data unit
//
// 데이터 할당
//	- Expand()로 전체 크기를 잡고 SetValue()로 개별 할당
//	- operator = 로 배열을 직접 할당
//
// 데이터 참조
//	- GetValue()로 개별 참조
//	- GetArray()로 배열 통채로 참조
//
// 데이터 삭제
//	- RemoveValue()로 개별 삭제
//	- Clear()로 전체 삭제
//------------------------------------------------------------------------------------------------//

template <class DataType>
class MkDataUnit : public MkDataUnitInterface
{
public:

	// 값 타입 반환
	virtual ePrimitiveDataType GetType(void) const { return MkPrimitiveDataType::GetEnum<DataType>(); }

	// 값 개수 반환
	virtual unsigned int GetSize(void) const { return m_DataArray.GetSize(); }

	// size만큼 값을 확장
	virtual void Expand(unsigned int size)
	{
		assert(size > 0);
		if (size > 0)
		{
			m_DataArray.Fill(size);
		}
	}

	// index 위치의 값을 삭제 후 성공여부 반환
	virtual bool RemoveValue(unsigned int index)
	{
		bool ok = m_DataArray.IsValidIndex(index);
		if (ok)
		{
			m_DataArray.Erase(MkArraySection(index, 1));
		}
		return ok;
	}

	// 해당 unit 객체의 복사본 반환
	virtual MkDataUnitInterface* CreateCopy(void) const
	{
		MkDataUnit<DataType>* newPtr = new MkDataUnit<DataType>;
		if (newPtr != NULL)
		{
			*newPtr = m_DataArray;
		}
		return newPtr;
	}

	// 비교 연산자(dynamic_cast)
	virtual bool operator == (const MkDataUnitInterface& source) const
	{
		if (GetType() == source.GetType())
		{
			const MkDataUnit<DataType>* srcPtr = dynamic_cast< const MkDataUnit<DataType>* >(&source);
			assert(srcPtr != NULL);
			return (m_DataArray == srcPtr->GetArray());
		}
		return false;
	}

	// index 위치에 값 할당하고 성공여부 반환
	inline bool SetValue(const DataType& value, unsigned int index)
	{
		bool ok = m_DataArray.IsValidIndex(index);
		if (ok)
		{
			m_DataArray[index] = value;
		}
		return ok;
	}

	// index 위치의 값 반환하고 성공여부 반환
	inline bool GetValue(DataType& value, unsigned int index) const
	{
		bool ok = m_DataArray.IsValidIndex(index);
		if (ok)
		{
			value = m_DataArray[index];
		}
		return ok;
	}

	// 배열로 할당
	inline MkDataUnit<DataType>& operator = (const MkArray<DataType>& values)
	{
		m_DataArray = values;
		return *this;
	}

	// 배열 참조 반환
	inline const MkArray<DataType>& GetArray(void) const { return m_DataArray; }

	// 해제
	inline void Clear(void) { m_DataArray.Clear(); }

	MkDataUnit() : MkDataUnitInterface() {}
	MkDataUnit(const MkArray<DataType>& values) : MkDataUnitInterface() { m_DataArray = values; }
	virtual ~MkDataUnit() { Clear(); }

protected:

	MkArray<DataType> m_DataArray;
};

//------------------------------------------------------------------------------------------------//

#endif
