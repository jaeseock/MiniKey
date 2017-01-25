
#include "MkCore_MkInterfaceForDataWriting.h"


#define MKDEF_DATA_INPUT_OPERATION(type, container) \
	_RegisterInput(type, container.GetSize()); \
	container.PushBack(source);

//------------------------------------------------------------------------------------------------//

template <class DataType>
class _TSI_DataWriting
{
public:
	static void Record(const MkByteArrayDescriptor& descriptor, MkByteArray& destBuffer, unsigned int& currentPosition)
	{
		if (destBuffer.Overwrite(currentPosition, descriptor))
		{
			currentPosition += descriptor.GetSize();
		}
	}

	static void Record(const DataType* source, unsigned int size, MkByteArray& destBuffer, unsigned int& currentPosition)
	{
		if (size > 0)
		{
			MkByteArrayDescriptor descriptor(reinterpret_cast<const unsigned char*>(source), size * sizeof(DataType));
			Record(descriptor, destBuffer, currentPosition);
		}
	}

	static void Record(const DataType& source, MkByteArray& destBuffer, unsigned int& currentPosition)
	{
		Record(&source, 1, destBuffer, currentPosition);
	}
};

//------------------------------------------------------------------------------------------------//

bool MkInterfaceForDataWriting::SetInputSize(ePrimitiveDataType type, unsigned int size)
{
	if (size == 0)
		return false;

	bool ok = MkPrimitiveDataType::IsValid(type);
	if (ok)
	{
		switch (type)
		{
		case ePDT_Bool: m_BoolUnits.Reserve(size); break;
		case ePDT_Int: m_IntUnits.Reserve(size); break;
		case ePDT_UnsignedInt: m_UIntUnits.Reserve(size); break;
		case ePDT_Float: m_FloatUnits.Reserve(size); break;
		case ePDT_Int2: m_Int2Units.Reserve(size); break;
		case ePDT_Vec2: m_Vec2Units.Reserve(size); break;
		case ePDT_Vec3: m_Vec3Units.Reserve(size); break;
		case ePDT_Str: m_StrUnits.Reserve(size); break;
		}
	}
	return ok;
}

void MkInterfaceForDataWriting::UpdateInputSize(void)
{
	unsigned int totalSize = 0;
	totalSize += m_BoolUnits.GetSize();
	totalSize += m_IntUnits.GetSize();
	totalSize += m_UIntUnits.GetSize();
	totalSize += m_FloatUnits.GetSize();
	totalSize += m_Int2Units.GetSize();
	totalSize += m_Vec2Units.GetSize();
	totalSize += m_Vec3Units.GetSize();
	totalSize += m_StrUnits.GetSize();

	if (totalSize > 0)
	{
		m_InputList.Reserve(totalSize);
	}
}

void MkInterfaceForDataWriting::Write(bool source) { MKDEF_DATA_INPUT_OPERATION(ePDT_Bool, m_BoolUnits) }
void MkInterfaceForDataWriting::Write(int source) { MKDEF_DATA_INPUT_OPERATION(ePDT_Int, m_IntUnits) }
void MkInterfaceForDataWriting::Write(unsigned int source) { MKDEF_DATA_INPUT_OPERATION(ePDT_UnsignedInt, m_UIntUnits) }
void MkInterfaceForDataWriting::Write(float source) { MKDEF_DATA_INPUT_OPERATION(ePDT_Float, m_FloatUnits) }
void MkInterfaceForDataWriting::Write(const MkInt2& source) { MKDEF_DATA_INPUT_OPERATION(ePDT_Int2, m_Int2Units) }
void MkInterfaceForDataWriting::Write(const MkVec2& source) { MKDEF_DATA_INPUT_OPERATION(ePDT_Vec2, m_Vec2Units) }
void MkInterfaceForDataWriting::Write(const MkVec3& source) { MKDEF_DATA_INPUT_OPERATION(ePDT_Vec3, m_Vec3Units) }
void MkInterfaceForDataWriting::Write(const MkStr& source) { MKDEF_DATA_INPUT_OPERATION(ePDT_Str, m_StrUnits) }

unsigned int MkInterfaceForDataWriting::Flush(MkByteArray& destBuffer, unsigned int beginPosition)
{
	// 총 용량 계산
	unsigned int inputByteSize = 0;
	inputByteSize += m_BoolUnits.GetSize() * sizeof(bool);
	inputByteSize += m_IntUnits.GetSize() * sizeof(int);
	inputByteSize += m_UIntUnits.GetSize() * sizeof(unsigned int);
	inputByteSize += m_FloatUnits.GetSize() * sizeof(float);
	inputByteSize += m_Int2Units.GetSize() * sizeof(MkInt2);
	inputByteSize += m_Vec2Units.GetSize() * sizeof(MkVec2);
	inputByteSize += m_Vec3Units.GetSize() * sizeof(MkVec3);
	MK_INDEXING_LOOP(m_StrUnits, i)
	{
		inputByteSize += sizeof(unsigned int) + m_StrUnits[i].GetSize() * sizeof(wchar_t);
	}

	if (inputByteSize == 0) // 기록할 값이 없음
		return 0;

	// beginPosition이 MKDEF_MAX_ARRAY_SIZE면 마지막 위치 지정
	if (beginPosition == MKDEF_MAX_ARRAY_SIZE)
	{
		beginPosition = destBuffer.GetSize();
	}

	// 공간 예약
	unsigned int newByteSize = beginPosition + inputByteSize;
	destBuffer.Fill(newByteSize);
	
	// destBuffer에 순차적으로 입력 기록
	unsigned int currentPosition = beginPosition;
	MK_INDEXING_LOOP(m_InputList, i)
	{
		const InputUnit& unit = m_InputList[i];
		switch (unit.type)
		{
		case ePDT_Bool:
			_TSI_DataWriting<bool>::Record(m_BoolUnits[unit.index], destBuffer, currentPosition);
			break;

		case ePDT_Int:
			_TSI_DataWriting<int>::Record(m_IntUnits[unit.index], destBuffer, currentPosition);
			break;

		case ePDT_UnsignedInt:
			_TSI_DataWriting<unsigned int>::Record(m_UIntUnits[unit.index], destBuffer, currentPosition);
			break;

		case ePDT_Float:
			_TSI_DataWriting<float>::Record(m_FloatUnits[unit.index], destBuffer, currentPosition);
			break;

		case ePDT_Int2:
			_TSI_DataWriting<MkInt2>::Record(m_Int2Units[unit.index], destBuffer, currentPosition);
			break;

		case ePDT_Vec2:
			_TSI_DataWriting<MkVec2>::Record(m_Vec2Units[unit.index], destBuffer, currentPosition);
			break;

		case ePDT_Vec3:
			_TSI_DataWriting<MkVec3>::Record(m_Vec3Units[unit.index], destBuffer, currentPosition);
			break;

		case ePDT_Str:
			{
				unsigned int strSize = m_StrUnits[unit.index].GetSize();
				_TSI_DataWriting<unsigned int>::Record(strSize, destBuffer, currentPosition); // 크기
				if (strSize > 0)
				{
					_TSI_DataWriting<wchar_t>::Record(m_StrUnits[unit.index].GetPtr(), strSize, destBuffer, currentPosition); // 문자열
				}
			}
			break;
		}
	}

	return inputByteSize;
}

void MkInterfaceForDataWriting::Clear(void)
{
	m_BoolUnits.Clear();
	m_IntUnits.Clear();
	m_UIntUnits.Clear();
	m_FloatUnits.Clear();
	m_Int2Units.Clear();
	m_Vec2Units.Clear();
	m_Vec3Units.Clear();
	m_StrUnits.Clear();
	m_InputList.Clear();
}

//------------------------------------------------------------------------------------------------//

void MkInterfaceForDataWriting::_RegisterInput(ePrimitiveDataType type, unsigned int index)
{
	InputUnit inputUnit;
	inputUnit.type = type;
	inputUnit.index = index;
	m_InputList.PushBack(inputUnit);
}

//------------------------------------------------------------------------------------------------//
