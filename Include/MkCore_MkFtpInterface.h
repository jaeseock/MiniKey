#pragma once

//------------------------------------------------------------------------------------------------//
// ftp를 사용한 파일 처리
// 다운로드는 MkFileDownloader, 브라우저로 웹페이지 열기는 MkWebBrowser, 웹페이지 읽기는 MkVisitWebPage 참조
//
// ex>
//	MkFtpInterface ftpInterface;
//	if (ftpInterface.Connect(L"210.207.252.151", L"test", L"eloadev", L"eloadev1234", true))
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

	// 최초 접속
	bool Connect(const MkStr& url, const MkStr& remotePath, const MkStr& userName, const MkStr& password, bool passiveMode);

	// 기존 정보로 재접속
	bool Connect(void);

	// 현재 경로 반환
	bool GetCurrentPath(MkStr& path, bool relativePathFromRemote = true);

	// 부모 디렉토리로 이동
	bool MoveToParent(void);

	// 자식 디렉토리로 이동. force가 true면 해당 디렉토리가 없을 시 생성
	bool MoveToChild(const MkStr& name, bool force = false);

	// 현재 경로에 파일 업로드
	// (NOTE) '+'문자는 표준파일경로에서는 허용되어도 웹경로에서는 인정 안될 수 있으므로 PlusTag로 치환해 업로드
	// ex> a++.mmd -> a_-plus-__-plus-_.mmd
	bool UploadFile(const MkPathName& filePath);

	// 현재 경로에 해당 디렉토리내 전체 파일(하위 디렉토리 포함) 업로드
	bool UploadDirectory(const MkPathName& directoryPath);

	// 현재 경로의 파일 삭제
	bool DeleteChildFile(const MkPathName& fileName);

	// 자식 디렉토리 삭제
	bool DeleteChildDirectory(const MkStr& name);

	// 해제
	void Close(void);

	MkFtpInterface();
	~MkFtpInterface() { Close(); }

protected:

	bool _CheckConnection(void);
	bool _UploadData(HINTERNET hObject, const unsigned char* dataPtr, DWORD sendSize) const;
	bool _UploadCurrDirectory(const MkPathName& dirPath, const MkDataNode& directoryNode);

protected:

	MkStr m_URL;
	int m_Port;
	MkStr m_RemotePath;
	MkStr m_UserName;
	MkStr m_Password;
	bool m_PassiveMode;

	HINTERNET m_Session;
	HINTERNET m_Connect;

public:

	static const MkStr PlusTag;
};
