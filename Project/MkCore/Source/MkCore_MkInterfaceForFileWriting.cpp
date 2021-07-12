
#include "MkCore_MkCheck.h"
#include "MkCore_MkInterfaceForFileWriting.h"


//------------------------------------------------------------------------------------------------//

bool MkInterfaceForFileWriting::SetUp(const MkPathName& filePath, bool overwrite, bool makeDirectoryPath)
{
	// ��� ����
	m_CurrentFilePath.ConvertToRootBasisAbsolutePath(filePath);
	if (m_CurrentFilePath.CheckPostfix(L"\\") || m_CurrentFilePath.CheckPostfix(L"/"))
		m_CurrentFilePath.BackSpace(1);

	if (makeDirectoryPath)
	{
		MkPathName dirPath = m_CurrentFilePath.GetPath();
		bool ok = dirPath.MakeDirectoryPath();
		MK_CHECK(ok, m_CurrentFilePath.GetFileName() + L" ������ ���� " + dirPath + L" ��� ���� ����")
			return false;
	}

	m_WrittenSize = 0;
	m_InitialSize = 0;

	// �̾� ���� ����̰� ���� ������ �����ϸ� ����ũ�� ���� 
	if ((!overwrite) && m_CurrentFilePath.CheckAvailable())
	{
		std::ifstream iStream;
		iStream.open(m_CurrentFilePath, std::ios::in | std::ios::binary);
		if (iStream.is_open())
		{
			iStream.seekg(0, std::ios_base::end);
			m_InitialSize = static_cast<unsigned int>(iStream.tellg());
			iStream.close();
		}
	}

	// ���� ����
	m_Stream.open(m_CurrentFilePath, std::ios::out | std::ios::binary | (overwrite ? std::ios::trunc : std::ios::app));
	bool ok = m_Stream.is_open();
	MK_CHECK(ok, filePath + L" ��� ���� ���� ����") {}

	return ok;
}

unsigned int MkInterfaceForFileWriting::GetCurrentPosition(void)
{
	return IsValid() ? (m_InitialSize + m_WrittenSize) : MKDEF_ARRAY_ERROR;
}

unsigned int MkInterfaceForFileWriting::Write(const MkByteArray& srcArray, const MkArraySection& section)
{
	if ((!IsValid()) || srcArray.Empty())
		return 0;

	MkByteArrayDescriptor descriptor = srcArray.GetMemoryBlockDescriptor(section);
	if (!descriptor.IsValid())
		return 0;

	unsigned int size = descriptor.GetSize();
	m_Stream.write(reinterpret_cast<const char*>(descriptor.GetPtr()), size);
	m_WrittenSize += size;
	return size;
}

void MkInterfaceForFileWriting::Clear(void)
{
	if (m_Stream.is_open())
	{
		m_Stream.close();
		m_WrittenSize = 0;
	}
}

//------------------------------------------------------------------------------------------------//
