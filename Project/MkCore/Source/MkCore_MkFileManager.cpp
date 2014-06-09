
#include "MkCore_MkCheck.h"
#include "MkCore_MkInterfaceForFileReading.h"
#include "MkCore_MkInterfaceForFileWriting.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkFileManager.h"

#define MK_FILE_MGR MkFileManager::Instance()
#define MK_FILE_MGR_AVAILABLE (MkFileManager::InstancePtr() != NULL)

//------------------------------------------------------------------------------------------------//

bool MkFileManager::CheckAvailable(const MkPathName& filePath)
{
	// ���� ��� �˻�
	MkPathName absoluteFilePath;
	absoluteFilePath.ConvertToRootBasisAbsolutePath(filePath);
	if (absoluteFilePath.CheckAvailable())
		return true;

	// ��ŷ �ý��ۿ��� �˻�
	return MK_FILE_MGR_AVAILABLE ? MK_FILE_MGR.__CheckAvailable(filePath) : false;
}

bool MkFileManager::GetFileData(const MkPathName& filePath, MkByteArray& buffer)
{
	// ���� ��ο� ������ �����ϸ� �ٷ� �о�鿩 ��ȯ
	if (__GetFileDataInRealPath(filePath, buffer))
		return true;

	// ��ŷ �ý��ۿ��� �˻�
	return MK_FILE_MGR_AVAILABLE ? MK_FILE_MGR.__GetOriginalFileData(filePath, buffer) : false;
}

bool MkFileManager::SaveDataToFile(const MkPathName& filePath, const MkByteArray& buffer, bool overwrite, bool makeDirectoryPath)
{
	MkInterfaceForFileWriting fwInterface;
	if (fwInterface.SetUp(filePath, overwrite, makeDirectoryPath))
	{
		return (fwInterface.Write(buffer, MkArraySection(0)) == buffer.GetSize());
	}
	return false;
}

bool MkFileManager::__GetFileDataInRealPath(const MkPathName& filePath, MkByteArray& buffer)
{
	MkPathName absoluteFilePath;
	absoluteFilePath.ConvertToRootBasisAbsolutePath(filePath);
	if (absoluteFilePath.CheckAvailable())
	{
		MkInterfaceForFileReading frInterface;
		if (frInterface.SetUp(absoluteFilePath))
		{
			frInterface.Read(buffer, MkArraySection(0));
			return true;
		}
	}
	return false;
}

bool MkFileManager::__CheckAvailable(const MkPathName& filePath) const
{
	return m_FileSystem.CheckAvailable(filePath);
}

bool MkFileManager::__GetOriginalFileData(const MkPathName& filePath, MkByteArray& buffer) const
{
	return m_FileSystem.GetOriginalFileData(filePath, buffer);
}

void MkFileManager::__PrintSystemInformationToLog(void) const
{
	MkStr buffer;
	buffer.Reserve(200);
	buffer += L"< File system >";
	buffer += MkStr::CR;
	buffer += L"   - Chunks : ";
	buffer += m_FileSystem.GetTotalChunkCount();
	buffer += MkStr::CR;
	buffer += L"   - Files : ";
	buffer += m_FileSystem.GetTotalFileCount();
	buffer += MkStr::CR;
	MK_DEV_PANEL.MsgToLog(buffer, false);
}

MkFileManager::MkFileManager(const MkPathName& workingDirectoryPath) : MkSingletonPattern<MkFileManager>()
{
	//m_FileSystem.SetChunkSizeGuideline(); // chunkSizeInMB
	//m_FileSystem.SetPercentageForCompressing(); // percentageForCompressing
	//m_FileSystem.SetChunkFileNamingRule(); // prefix, extension

	MK_CHECK(m_FileSystem.SetUpFromChunkFiles(workingDirectoryPath), L"MkFileManager �ʱ�ȭ ����")
		return;
}

//------------------------------------------------------------------------------------------------//
