
#include "MkCore_MkCheck.h"
#include "MkCore_MkInterfaceForFileReading.h"


//------------------------------------------------------------------------------------------------//

bool MkInterfaceForFileReading::SetUp(const MkPathName& filePath)
{
	// 경로 구성
	m_CurrentFilePath.ConvertToRootBasisAbsolutePath(filePath);
	if (m_CurrentFilePath.CheckPostfix(L"\\") || m_CurrentFilePath.CheckPostfix(L"/"))
		m_CurrentFilePath.BackSpace(1);
	bool ok = m_CurrentFilePath.CheckAvailable();
	MK_CHECK(ok, filePath + L" 경로에 파일이 존재하지 않음")
		return false;

	// 파일 오픈
	m_Stream.open(m_CurrentFilePath, std::ios::in | std::ios::binary);
	ok = m_Stream.is_open();
	MK_CHECK(ok, filePath + L" 경로 파일 열기 실패")
		return false;
	
	// 크기 확인
	m_Stream.seekg(0, std::ios_base::end);
	m_FileSize = static_cast<unsigned int>(m_Stream.tellg());
	m_Stream.seekg(0);
	return true;
}

bool MkInterfaceForFileReading::IsValid(void) const
{
	return (m_Stream.is_open() && (!m_Stream.eof()));
}

void MkInterfaceForFileReading::SetCurrentPosition(unsigned int newPosition)
{
	assert((IsValid()) && (newPosition < m_FileSize));
	m_Stream.seekg(newPosition);
}

unsigned int MkInterfaceForFileReading::GetCurrentPosition(void)
{
	return IsValid() ? static_cast<unsigned int>(m_Stream.tellg()) : MKDEF_ARRAY_ERROR;
}

unsigned int MkInterfaceForFileReading::Read(MkByteArray& destBuffer, const MkArraySection& section)
{
	unsigned int size = section.GetSize();
	unsigned int position = section.GetPosition();
	if ((!IsValid()) || (size == 0) || (position >= m_FileSize))
		return 0;
	
	if (size == MKDEF_ARRAY_ERROR)
	{
		size = m_FileSize - position;
	}
	else
	{
		unsigned int nextPos = section.GetEndPosition();
		assert((nextPos >= position) && (nextPos >= size)); // overflow
		if (nextPos > m_FileSize)
		{
			size = m_FileSize - position;
		}
	}

	destBuffer.Clear();
	if (size > 0)
	{
		destBuffer.Fill(size);
		m_Stream.seekg(position);
		m_Stream.read(reinterpret_cast<char*>(destBuffer.GetPtr()), size);
	}
	return size;
}

void MkInterfaceForFileReading::Clear(void)
{
	if (m_Stream.is_open())
	{
		m_Stream.close();
		m_FileSize = 0;
	}
}

//------------------------------------------------------------------------------------------------//
