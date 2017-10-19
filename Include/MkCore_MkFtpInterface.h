#pragma once

//------------------------------------------------------------------------------------------------//
// ftp�� ����� ���� ó��
// �ٿ�ε�� MkFileDownloader, �������� �������� ����� MkWebBrowser, �������� �б�� MkVisitWebPage ����
//
// ex>
//	MkFtpInterface ftpInterface;
//	if (ftpInterface.Connect(L"210.207.252.151", L"test", L"eloadev", L"eloadev1234"))
//	{
//		ftpInterface.MoveToChild(L"A", true);
//		ftpInterface.MoveToChild(L"B", true);
//		ftpInterface.UploadFile(L"create.txt");
//		ftpInterface.UploadFile(L"empty.txt");
//		ftpInterface.DeleteChildFile(L"create.txt");
//		ftpInterface.DeleteChildFile(L"empty.txt");
//		ftpInterface.MoveToParent();
//		ftpInterface.DeleteChildDirectory(L"B");
//		ftpInterface.MoveToParent();
//		ftpInterface.DeleteChildDirectory(L"A");
//	}
//------------------------------------------------------------------------------------------------//

#include <WinInet.h>
#include "MkCore_MkStr.h"


class MkDataNode;

class MkFtpInterface
{
public:

	// ���� ����
	bool Connect(const MkStr& url, const MkStr& remotePath, const MkStr& userName, const MkStr& password);

	// ���� ������ ������
	bool Connect(void);

	// ���� ��� ��ȯ
	bool GetCurrentPath(MkStr& path);

	// �θ� ���丮�� �̵�
	bool MoveToParent(void);

	// �ڽ� ���丮�� �̵�. force�� true�� �ش� ���丮�� ���� �� ����
	bool MoveToChild(const MkStr& name, bool force = false);

	// ���� ��ο� ���� ���ε�
	bool UploadFile(const MkPathName& filePath);

	// ���� ��ο� �ش� ���丮�� ��ü ����(���� ���丮 ����) ���ε�
	bool UploadDirectory(const MkPathName& directoryPath);

	// ���� ����� ���� ����
	bool DeleteChildFile(const MkPathName& fileName);

	// �ڽ� ���丮 ����
	bool DeleteChildDirectory(const MkStr& name);

	// ����
	void Close(void);

	MkFtpInterface();
	~MkFtpInterface() { Close(); }

protected:

	bool _CheckConnection(void);
	bool _UploadData(HINTERNET hObject, const unsigned char* dataPtr, DWORD sendSize) const;
	bool _UploadCurrDirectory(const MkPathName& dirPath, const MkDataNode& directoryNode);

protected:

	MkStr m_URL;
	MkStr m_RemotePath;
	MkStr m_UserName;
	MkStr m_Password;

	HINTERNET m_Session;
	HINTERNET m_Connect;
};
