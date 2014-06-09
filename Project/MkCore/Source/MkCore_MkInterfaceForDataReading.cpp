
#include "MkCore_MkVec2.h"
#include "MkCore_MkVec3.h"
#include "MkCore_MkStr.h"
#include "MkCore_MkInterfaceForDataReading.h"

//------------------------------------------------------------------------------------------------//

template <class DataType>
class __TSI_DataReading
{
public:
	static bool Extract(DataType& buffer, const MkByteArray& srcArray, unsigned int& currentPosition, unsigned int endPosition)
	{
		if ((endPosition == 0) || ((currentPosition + sizeof(DataType)) > endPosition))
			return false;

		MkByteArrayDescriptor descriptor = srcArray.GetMemoryBlockDescriptor(MkArraySection(currentPosition, sizeof(DataType)));
		if (!descriptor.IsValid())
			return false;

		memcpy_s(reinterpret_cast<unsigned char*>(&buffer), sizeof(DataType), descriptor, sizeof(DataType));
		currentPosition += sizeof(DataType);
		return true;
	}
};

//------------------------------------------------------------------------------------------------//

bool MkInterfaceForDataReading::SetUp(const MkByteArray& srcArray, unsigned int beginPosition)
{
	if (srcArray.Empty() || (srcArray.GetSize() < beginPosition))
		return false;

	m_SourcePtr = const_cast<MkByteArray*>(&srcArray);
	m_BeginPosition = beginPosition;
	m_CurrentPosition = m_BeginPosition;
	m_EndPosition = srcArray.GetSize();
	return true;
}

bool MkInterfaceForDataReading::Read(bool& buffer)
{
	return (m_SourcePtr == NULL) ? false : __TSI_DataReading<bool>::Extract(buffer, *m_SourcePtr, m_CurrentPosition, m_EndPosition);
}

bool MkInterfaceForDataReading::Read(int& buffer)
{
	return (m_SourcePtr == NULL) ? false : __TSI_DataReading<int>::Extract(buffer, *m_SourcePtr, m_CurrentPosition, m_EndPosition);
}

bool MkInterfaceForDataReading::Read(unsigned int& buffer)
{
	return (m_SourcePtr == NULL) ? false : __TSI_DataReading<unsigned int>::Extract(buffer, *m_SourcePtr, m_CurrentPosition, m_EndPosition);
}

bool MkInterfaceForDataReading::Read(float& buffer)
{
	return (m_SourcePtr == NULL) ? false : __TSI_DataReading<float>::Extract(buffer, *m_SourcePtr, m_CurrentPosition, m_EndPosition);
}

bool MkInterfaceForDataReading::Read(MkInt2& buffer)
{
	return (m_SourcePtr == NULL) ? false : __TSI_DataReading<MkInt2>::Extract(buffer, *m_SourcePtr, m_CurrentPosition, m_EndPosition);
}

bool MkInterfaceForDataReading::Read(MkVec2& buffer)
{
	return (m_SourcePtr == NULL) ? false : __TSI_DataReading<MkVec2>::Extract(buffer, *m_SourcePtr, m_CurrentPosition, m_EndPosition);
}

bool MkInterfaceForDataReading::Read(MkVec3& buffer)
{
	return (m_SourcePtr == NULL) ? false : __TSI_DataReading<MkVec3>::Extract(buffer, *m_SourcePtr, m_CurrentPosition, m_EndPosition);
}

bool MkInterfaceForDataReading::Read(MkStr& buffer)
{
	if (m_SourcePtr == NULL)
		return false;

	// 길이(unsigned int)를 먼저 읽고
	unsigned int size = 0;
	if (!__TSI_DataReading<unsigned int>::Extract(size, *m_SourcePtr, m_CurrentPosition, m_EndPosition))
		return false;

	// 문자열을 읽음
	buffer.Clear();
	if (size > 0)
	{
		unsigned int strSize = size * sizeof(wchar_t);
		buffer = m_SourcePtr->GetMemoryBlockDescriptor(MkArraySection(m_CurrentPosition, strSize));
		m_CurrentPosition += strSize;
	}
	return true;
}

bool MkInterfaceForDataReading::CheckEndOfBlock(void) const { return (m_CurrentPosition == m_EndPosition); }

void MkInterfaceForDataReading::SetCurrentPosition(unsigned int newPosition)
{
	assert(newPosition <= m_EndPosition);
	m_CurrentPosition = newPosition;
}

unsigned int MkInterfaceForDataReading::GetCurrentPosition(void) const { return m_CurrentPosition; }

MkInterfaceForDataReading::MkInterfaceForDataReading()
{
	m_SourcePtr = NULL;
	m_BeginPosition = 0;
	m_CurrentPosition = 0;
	m_EndPosition = 0;
}

//------------------------------------------------------------------------------------------------//
