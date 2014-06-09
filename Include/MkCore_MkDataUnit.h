#ifndef __MINIKEY_CORE_MKDATAUNIT_H__
#define __MINIKEY_CORE_MKDATAUNIT_H__


//------------------------------------------------------------------------------------------------//
// data unit
// �� ������ ���� ���� ����
// �迭 ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkDataTypeDefinition.h"


//------------------------------------------------------------------------------------------------//
// data unit�� interface ����
//------------------------------------------------------------------------------------------------//

class MkDataUnitInterface
{
public:

	// �� Ÿ�� ��ȯ
	virtual ePrimitiveDataType GetType(void) const { return ePDT_Undefined; }

	// �� ���� ��ȯ
	virtual unsigned int GetSize(void) const { return 0; }

	// size��ŭ ���� Ȯ��
	virtual void Expand(unsigned int size) = NULL;

	// index ��ġ�� ���� ���� �� �������� ��ȯ
	virtual bool RemoveValue(unsigned int index) = NULL;

	// �ش� unit ��ü�� ���纻 ��ȯ
	virtual MkDataUnitInterface* CreateCopy(void) const = NULL;

	// �� ������
	virtual bool operator == (const MkDataUnitInterface& source) const = NULL;
	inline bool operator != (const MkDataUnitInterface& source) const { return !operator==(source); }

	MkDataUnitInterface() {}
	virtual ~MkDataUnitInterface() {}
};

//------------------------------------------------------------------------------------------------//
// ���� �� �迭�� ���� data unit
//
// ������ �Ҵ�
//	- Expand()�� ��ü ũ�⸦ ��� SetValue()�� ���� �Ҵ�
//	- operator = �� �迭�� ���� �Ҵ�
//
// ������ ����
//	- GetValue()�� ���� ����
//	- GetArray()�� �迭 ��ä�� ����
//
// ������ ����
//	- RemoveValue()�� ���� ����
//	- Clear()�� ��ü ����
//------------------------------------------------------------------------------------------------//

template <class DataType>
class MkDataUnit : public MkDataUnitInterface
{
public:

	// �� Ÿ�� ��ȯ
	virtual ePrimitiveDataType GetType(void) const { return MkPrimitiveDataType::GetEnum<DataType>(); }

	// �� ���� ��ȯ
	virtual unsigned int GetSize(void) const { return m_DataArray.GetSize(); }

	// size��ŭ ���� Ȯ��
	virtual void Expand(unsigned int size)
	{
		assert(size > 0);
		if (size > 0)
		{
			m_DataArray.Fill(size);
		}
	}

	// index ��ġ�� ���� ���� �� �������� ��ȯ
	virtual bool RemoveValue(unsigned int index)
	{
		bool ok = m_DataArray.IsValidIndex(index);
		if (ok)
		{
			m_DataArray.Erase(MkArraySection(index, 1));
		}
		return ok;
	}

	// �ش� unit ��ü�� ���纻 ��ȯ
	virtual MkDataUnitInterface* CreateCopy(void) const
	{
		MkDataUnit<DataType>* newPtr = new MkDataUnit<DataType>;
		if (newPtr != NULL)
		{
			*newPtr = m_DataArray;
		}
		return newPtr;
	}

	// �� ������(dynamic_cast)
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

	// index ��ġ�� �� �Ҵ��ϰ� �������� ��ȯ
	inline bool SetValue(const DataType& value, unsigned int index)
	{
		bool ok = m_DataArray.IsValidIndex(index);
		if (ok)
		{
			m_DataArray[index] = value;
		}
		return ok;
	}

	// index ��ġ�� �� ��ȯ�ϰ� �������� ��ȯ
	inline bool GetValue(DataType& value, unsigned int index) const
	{
		bool ok = m_DataArray.IsValidIndex(index);
		if (ok)
		{
			value = m_DataArray[index];
		}
		return ok;
	}

	// �迭�� �Ҵ�
	inline MkDataUnit<DataType>& operator = (const MkArray<DataType>& values)
	{
		m_DataArray = values;
		return *this;
	}

	// �迭 ���� ��ȯ
	inline const MkArray<DataType>& GetArray(void) const { return m_DataArray; }

	// ����
	inline void Clear(void) { m_DataArray.Clear(); }

	MkDataUnit() : MkDataUnitInterface() {}
	MkDataUnit(const MkArray<DataType>& values) : MkDataUnitInterface() { m_DataArray = values; }
	virtual ~MkDataUnit() { Clear(); }

protected:

	MkArray<DataType> m_DataArray;
};

//------------------------------------------------------------------------------------------------//

#endif
