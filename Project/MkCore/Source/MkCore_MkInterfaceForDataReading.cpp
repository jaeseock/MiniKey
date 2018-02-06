
#include "MkCore_MkVec2.h"
#include "MkCore_MkVec3.h"
#include "MkCore_MkStr.h"
#include "MkCore_MkInterfaceForDataReading.h"

//------------------------------------------------------------------------------------------------//

template <class DataType>
class __TSI_DataReading
{
public:
	static bool Extract(DataType& buffer, const MkByteArray& srcArray, unsigned int& currentPosition, unsigned int endPosition, unsigned int count = 1)
	{
		unsigned int dataSize = sizeof(DataType) * count;
		if ((endPosition == 0) || ((currentPosition + dataSize) > endPosition))
			return false;

		MkByteArrayDescriptor descriptor = srcArray.GetMemoryBlockDescriptor(MkArraySection(currentPosition, dataSize));
		if (!descriptor.IsValid())
			return false;

		memcpy_s(reinterpret_cast<unsigned char*>(&buffer), dataSize, descriptor, dataSize);
		currentPosition += dataSize;
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

bool MkInterfaceForDataReading::Read(__int64& buffer)
{
	return (m_SourcePtr == NULL) ? false : __TSI_DataReading<__int64>::Extract(buffer, *m_SourcePtr, m_CurrentPosition, m_EndPosition);
}

bool MkInterfaceForDataReading::Read(unsigned __int64& buffer)
{
	return (m_SourcePtr == NULL) ? false : __TSI_DataReading<unsigned __int64>::Extract(buffer, *m_SourcePtr, m_CurrentPosition, m_EndPosition);
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
		unsigned int dataSize = size * sizeof(wchar_t);
		buffer = m_SourcePtr->GetMemoryBlockDescriptor(MkArraySection(m_CurrentPosition, dataSize));
		m_CurrentPosition += dataSize;
	}
	return true;
}

bool MkInterfaceForDataReading::Read(MkByteArray& buffer)
{
	if (m_SourcePtr == NULL)
		return false;

	// 길이(unsigned int)를 먼저 읽고
	unsigned int size = 0;
	if (!__TSI_DataReading<unsigned int>::Extract(size, *m_SourcePtr, m_CurrentPosition, m_EndPosition))
		return false;

	// 데이터를 읽음
	buffer.Clear();
	if (size > 0)
	{
		unsigned int dataSize = size * sizeof(unsigned char);
		if (!m_SourcePtr->GetSubArray(MkArraySection(m_CurrentPosition, dataSize), buffer))
			return false;

		m_CurrentPosition += dataSize;
	}
	return true;
}

bool MkInterfaceForDataReading::Read(unsigned char* buffer, unsigned int size)
{
	return ((buffer == NULL) || (m_SourcePtr == NULL)) ? false : __TSI_DataReading<unsigned char>::Extract(*buffer, *m_SourcePtr, m_CurrentPosition, m_EndPosition, size);
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
