
#include <Windows.h>
#include <WinInet.h>

#include "MkCore_MkInterfaceForFileReading.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkFtpInterface.h"

#pragma comment (lib, "wininet.lib")

//------------------------------------------------------------------------------------------------//

bool MkFtpInterface::Connect(const MkStr& url, const MkStr& remotePath, const MkStr& userName, const MkStr& password)
{
	m_URL = url;
	m_RemotePath = remotePath;
	m_UserName = userName;
	m_Password = password;

	return Connect();
}

bool MkFtpInterface::Connect(void)
{
	Close();

	do
	{
		m_Session = ::InternetOpen(L"MkFtpInterface", INTERNET_OPEN_TYPE_PRECONFIG, NULL, 0, 0);
		if (m_Session == NULL)
			break;

		m_Connect = ::InternetConnect(m_Session, m_URL.GetPtr(), 21, m_UserName.GetPtr(), m_Password.GetPtr(), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE , 0);
		if (m_Connect == NULL)
			break;

		if (!m_RemotePath.Empty())
		{
			if (::FtpSetCurrentDirectory(m_Connect, m_RemotePath.GetPtr()) == FALSE)
			{
				if (::FtpCreateDirectory(m_Connect, m_RemotePath.GetPtr()) == TRUE)
				{
					if (::FtpSetCurrentDirectory(m_Connect, m_RemotePath.GetPtr()) == FALSE)
						break;
				}
				else
					break;
			}
		}

		GetCurrentPath(m_RemotePath, false);
		return true;
	}
	while (false);
	
	Close();
	return false;
}

bool MkFtpInterface::GetCurrentPath(MkStr& path, bool relativePathFromRemote)
{
	if (_CheckConnection())
	{
		wchar_t tmpBuf[INTERNET_MAX_PATH_LENGTH] = {0, };
		DWORD bufSize = INTERNET_MAX_PATH_LENGTH;
		if (::FtpGetCurrentDirectory(m_Connect, tmpBuf, &bufSize) == TRUE)
		{
			if (relativePathFromRemote)
			{
				path = &tmpBuf[m_RemotePath.GetSize()];
			}
			else
			{
				path = tmpBuf;
			}
			return true;
		}
	}
	return false;
}

bool MkFtpInterface::MoveToParent(void)
{
	return _CheckConnection() ? (::FtpSetCurrentDirectory(m_Connect, L"..") == TRUE) : false;
}

bool MkFtpInterface::MoveToChild(const MkStr& name, bool force)
{
	if (!_CheckConnection())
		return false;

	if (::FtpSetCurrentDirectory(m_Connect, name.GetPtr()) == TRUE)
		return true;

	if (force &&
		(::FtpCreateDirectory(m_Connect, name.GetPtr()) == TRUE) &&
		(::FtpSetCurrentDirectory(m_Connect, name.GetPtr()) == TRUE))
		return true;

	return false;
}

bool MkFtpInterface::UploadFile(const MkPathName& filePath)
{
	// 파일 확인
	MkPathName fullPath;
	fullPath.ConvertToRootBasisAbsolutePath(filePath);
	if (!fullPath.CheckAvailable())
		return false;

	// 파일 데이터 읽기
	MkByteArray fileData;
	MkInterfaceForFileReading frInterface;
	if (!frInterface.SetUp(fullPath))
		return false;

	frInterface.Read(fileData, MkArraySection(0));
	frInterface.Clear();

	// 연결 확인
	if (!_CheckConnection())
		return false;

	MkPathName fileName = fullPath.GetFileName();

	// 기존 파일이 존재한다면 삭제
	DeleteChildFile(fileName);

	// 네트워크 파일 생성
	HINTERNET hObject = ::FtpOpenFile(m_Connect, fileName.GetPtr(), GENERIC_WRITE, FTP_TRANSFER_TYPE_BINARY, 0);
	if (hObject == NULL)
		return false;

	// 전송. 한 번에 TRANSFER_SIZE씩 끊어서 보냄
	const unsigned int TRANSFER_SIZE = 1024 * 10; // 한 번에 10k씩 전송
	unsigned int workSize = fileData.GetSize();
	unsigned int dataPos = 0;

	bool ok = false;
	while (true)
	{
		unsigned int sendSize = GetMin<unsigned int>(TRANSFER_SIZE, workSize);
		if (_UploadData(hObject, fileData.GetPtr() + dataPos, static_cast<DWORD>(sendSize)))
		{
			workSize -= sendSize;
			if (workSize == 0)
			{
				ok = true;
				break;
			}

			dataPos += sendSize;
		}
		else
			break;
	}
	
	if (hObject != NULL)
	{
		::InternetCloseHandle(hObject);
	}

	// 업로드 중 오류로 실패했으면 삭제
	if (!ok)
	{
		DeleteChildFile(fileName);
	}

	return ok;
}

bool MkFtpInterface::UploadDirectory(const MkPathName& directoryPath)
{
	MkDataNode dirRootNode;
	if (directoryPath.ExportSystemStructure(dirRootNode) == 0)
		return false;

	MkPathName dirPath;
	dirPath.ConvertToRootBasisAbsolutePath(directoryPath);
	return _UploadCurrDirectory(dirPath, dirRootNode);
}

bool MkFtpInterface::DeleteChildFile(const MkPathName& fileName)
{
	if (!_CheckConnection())
		return false;

	return (::FtpDeleteFile(m_Connect, fileName.GetPtr()) == TRUE);
}

bool MkFtpInterface::DeleteChildDirectory(const MkStr& name)
{
	if (!_CheckConnection())
		return false;

	return (::FtpRemoveDirectory(m_Connect, name.GetPtr()) == TRUE);
}

void MkFtpInterface::Close(void)
{
	if (m_Connect != NULL)
	{
		::InternetCloseHandle(m_Connect);
		m_Connect = NULL;
	}

	if (m_Session != NULL)
	{
		::InternetCloseHandle(m_Session);
		m_Session = NULL;
	}
}

MkFtpInterface::MkFtpInterface()
{
	m_Session = NULL;
	m_Connect = NULL;
}

//------------------------------------------------------------------------------------------------//

bool MkFtpInterface::_CheckConnection(void)
{
	if (m_Connect == NULL)
	{
		Connect();
	}
	return (m_Connect != NULL);
}

bool MkFtpInterface::_UploadData(HINTERNET hObject, const unsigned char* dataPtr, DWORD sendSize) const
{
	if (sendSize == 0)
		return true;

	if ((hObject == NULL) || (dataPtr == NULL))
		return false;

	DWORD writtenSize;
	do
	{
		if (::InternetWriteFile(hObject, dataPtr, sendSize, &writtenSize) == FALSE)
			return false;

		sendSize -= writtenSize;
	}
	while ((sendSize > 0) && (writtenSize != 0));

	return (sendSize == 0);
}

bool MkFtpInterface::_UploadCurrDirectory(const MkPathName& dirPath, const MkDataNode& directoryNode)
{
	MkArray<MkHashStr> subObjects;
	MkPathName::__GetSubFiles(directoryNode, subObjects);
	MK_INDEXING_LOOP(subObjects, i)
	{
		if (!UploadFile(dirPath + subObjects[i].GetString()))
			return false;
	}

	subObjects.Clear();
	MkPathName::__GetSubDirectories(directoryNode, subObjects);
	MK_INDEXING_LOOP(subObjects, i)
	{
		const MkHashStr& childDirName = subObjects[i];

		if (!MoveToChild(childDirName.GetString(), true))
			return false;

		MkPathName targetPath = dirPath;
		targetPath += childDirName.GetString();
		targetPath += L"\\";

		if (!_UploadCurrDirectory(targetPath, *directoryNode.GetChildNode(childDirName))) // 재귀
			return false;

		if (!MoveToParent())
			return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------//
