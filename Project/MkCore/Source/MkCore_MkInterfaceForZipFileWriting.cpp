
#include "MkCore_MkCheck.h"
#include "MkCore_MkInterfaceForZipFileWriting.h"

#pragma comment(lib, "minizip")


//------------------------------------------------------------------------------------------------//

bool MkInterfaceForZipFileWriting::SetUp(const MkPathName& filePath, bool overwrite, bool makeDirectoryPath)
{
	Clear();

	m_CurrentFilePath.ConvertToRootBasisAbsolutePath(filePath);

	if (makeDirectoryPath)
	{
		MkPathName dirPath = m_CurrentFilePath.GetPath();
		bool ok = dirPath.MakeDirectoryPath();
		MK_CHECK(ok, m_CurrentFilePath.GetFileName() + L" 파일을 위한 " + dirPath + L" 경로 생성 실패")
			return false;
	}

	int append = APPEND_STATUS_CREATE;
	if (m_CurrentFilePath.CheckAvailable())
	{
		if (overwrite)
		{
			append = APPEND_STATUS_ADDINZIP;
		}
		else
		{
			m_CurrentFilePath.DeleteCurrentFile();
		}
	}

	std::string charPath;
	m_CurrentFilePath.ExportMultiByteString(charPath);
	m_ZipFile = zipOpen(charPath.c_str(), append);
	return (m_ZipFile != NULL);
}

bool MkInterfaceForZipFileWriting::Write(const MkPathName& filePath, const MkByteArray& srcArray)
{
	if (m_ZipFile == NULL)
		return false;

	std::string charPath;
	filePath.ExportMultiByteString(charPath);
	if (zipOpenNewFileInZip(m_ZipFile, charPath.c_str(), NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION) != ZIP_OK)
		return false;
	
	if (zipWriteInFileInZip(m_ZipFile, srcArray.GetPtr(), srcArray.GetSize()) < 0)
		return false;

	zipCloseFileInZip(m_ZipFile);
	return true;
}

void MkInterfaceForZipFileWriting::Clear(void)
{
	if (m_ZipFile != NULL)
	{
		m_CurrentFilePath.Clear();

		zipClose(m_ZipFile, NULL);
		m_ZipFile = NULL;
	}
}

MkInterfaceForZipFileWriting::MkInterfaceForZipFileWriting()
{
	m_ZipFile = NULL;
}

//------------------------------------------------------------------------------------------------//
