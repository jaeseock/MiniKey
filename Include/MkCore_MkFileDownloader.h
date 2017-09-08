#pragma once

//------------------------------------------------------------------------------------------------//
// URL���� ���� �ٿ�δ�
// �� ���� ���·� ��밡��
//
// - �Ϲ����� blocking ������ static �Լ�
//   ��뷮 ���� ���� �� �ٿ��� �Ϸ�Ǳ� �� ��Ұ� �ȵǹǷ�(blocking) ���� �� �� ����
//	ex> MkFileDownloader::DownloadFileRightNow(L"www.abc.co.kr/abc.bin.rp", L"download\\abc.bin");
//
// - none-blocking ������ singleton �Լ�
//   ��ȯ�Ǵ� interface(MkFileDownInfo instance)�� �˻� �� �ٿ� ���� ���¸� ���� �ʿ�(���ϸ� �޸� ���� �߻�)
//   �ٿ� �� ���(StopDownload() ȣ��) ����
//	ex>
//	// ���
//	MkFileDownInfo* downInfo = MK_FILE_DOWNLOADER.DownloadFile(L"www.abc.co.kr/abc.bin.rp", L"download\\abc.bin");
//	...
//	// ���θ��� ���� Ž��
//	MkFileDownInfo::eDownState downState = downInfo->GetDownState();
//	if ((downState == MkFileDownInfo::eDS_Wait) || (downState == MkFileDownInfo::eDS_Downloading))
//	{
//		::Sleep(1); // ��� ����� ���� ������
//	}
//	else
//	{
//		if (downState == MkFileDownInfo::eDS_Complete) // �Ϸ�
//		{
//			... // �Ϸ� ���� ó��
//		}
//		else // ���� Ȥ�� ����(eDS_Abort, eDS_InvalidURL, eDS_OutOfMemory, eDS_Failed)
//		{
//			... // ������ ���� ���
//		}
//		delete downInfo; // �ٿ��� ����Ǿ����Ƿ� ����
//	}
//
// (NOTE) ���� �ٿ�ε��� ���� ���̴� ����� �ƴѵ���, blocking���ε� ��κ� �ذ� �����ϱ� ������
// ���� framework�� ������ singleton�� thread�� ����ϱ�� ����� �Ʊ��. ���� ��ü���� instance��
// thread manager�� ������ �ʿ� �� �� ���������� ������ ���
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkDeque.h"
#include "MkCore_MkLockable.h"
#include "MkCore_MkPathName.h"


#define MK_FILE_DOWNLOADER MkFileDownloader::Instance()

//------------------------------------------------------------------------------------------------//

class MkFileDownInfo
{
public:

	enum eDownState
	{
		eDS_Wait = 0, // ��� ��. �ʱⰪ
		eDS_Downloading, // �ٿ�ε� ��

		// result
		eDS_Complete, // �Ϸ�
		eDS_Abort, // ����ڿ� ���� �ߵ�����
		eDS_InvalidURL, // fileURL�� �ش��ϴ� ������ ��Ʈ��ũ�� �������� ����
		eDS_OutOfMemory, // �޸� ����
		eDS_Failed // ��Ÿ ������ ����
	};

	bool SetUp(const MkStr& fileURL, const MkPathName& destFilePath);
	eDownState GetDownState(void);
	float GetProgress(void);

	MkFileDownInfo();
	~MkFileDownInfo() {}

	void __GetTargetInfo(MkStr& fileURL, MkPathName& destLocalPath);
	void __SetDownState(eDownState downState);
	void __SetFileState(unsigned int fileSize, unsigned int downSize);

protected:

	MkStr m_FileURL;
	MkPathName m_DestLocalPath;

	MkLockable<eDownState> m_DownState; // producer:MkDownloadThreadUnit, consumer:other thread
	MkLockable<unsigned int> m_FileSize; // producer:MkDownloadThreadUnit, consumer:other thread
	MkLockable<unsigned int> m_DownSize; // producer:MkDownloadThreadUnit, consumer:other thread
};

//------------------------------------------------------------------------------------------------//

class MkThreadManager;

class MkFileDownloader
{
public:

	//------------------------------------------------------------------------------------------------//
	// blocking
	//------------------------------------------------------------------------------------------------//

	// ���� �ٿ�
	// fileURL : ��Ʈ��ũ�� ���� ���
	// destFilePath : ����� ���� ���� ���. ��Ʈ ���丮 ���� ���, Ȥ�� ���� ���
	static bool DownloadFileRightNow(const MkStr& fileURL, const MkPathName& destFilePath);

	//------------------------------------------------------------------------------------------------//
	// none - blocking
	//------------------------------------------------------------------------------------------------//

	// ���� �ٿ�
	// fileURL : ��Ʈ��ũ�� ���� ���
	// destFilePath : ����� ���� ���� ���. ��Ʈ ���丮 ���� ���, Ȥ�� ���� ���
	// (NOTE) ��ȯ�� fileDownInfo�� state�� result�� �ٲ� ���� �����ؾ� ��
	MkFileDownInfo* DownloadFile(const MkStr& fileURL, const MkPathName& destFilePath);

	// none-blocking ���·� �ٿ� ���� ��� ����. ���� �ִ� ��� �ٿ� ����Ʈ ��ȿȭ
	void StopDownload(void);

	//------------------------------------------------------------------------------------------------//

	// meyers's implementation
	static MkFileDownloader& Instance(void);

	MkFileDownloader();
	~MkFileDownloader() {}

	MkFileDownInfo* __GetNextFileInfo(void);
	void __CloseDownloadThread(void);

protected:

	void _OpenDownloadThread(void);

protected:

	MkThreadManager* m_ThreadMgr;

	MkCriticalSection m_CS;
	MkDeque<MkFileDownInfo*> m_FileDownInfoList;
};

//------------------------------------------------------------------------------------------------//
