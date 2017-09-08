#pragma once

//------------------------------------------------------------------------------------------------//
// URL상의 파일 다운로더
// 두 가지 형태로 사용가능
//
// - 일반적인 blocking 형태의 static 함수
//   대용량 파일 받을 시 다운이 완료되기 전 취소가 안되므로(blocking) 문제 될 수 있음
//	ex> MkFileDownloader::DownloadFileRightNow(L"www.abc.co.kr/abc.bin.rp", L"download\\abc.bin");
//
// - none-blocking 형태의 singleton 함수
//   반환되는 interface(MkFileDownInfo instance)를 검사 해 다운 종료 상태면 삭제 필요(안하면 메모리 누수 발생)
//   다운 중 취소(StopDownload() 호출) 가능
//	ex>
//	// 등록
//	MkFileDownInfo* downInfo = MK_FILE_DOWNLOADER.DownloadFile(L"www.abc.co.kr/abc.bin.rp", L"download\\abc.bin");
//	...
//	// 루핑마다 상태 탐색
//	MkFileDownInfo::eDownState downState = downInfo->GetDownState();
//	if ((downState == MkFileDownInfo::eDS_Wait) || (downState == MkFileDownInfo::eDS_Downloading))
//	{
//		::Sleep(1); // 잠깐 대기후 다음 루프로
//	}
//	else
//	{
//		if (downState == MkFileDownInfo::eDS_Complete) // 완료
//		{
//			... // 완료 파일 처리
//		}
//		else // 중지 혹은 오류(eDS_Abort, eDS_InvalidURL, eDS_OutOfMemory, eDS_Failed)
//		{
//			... // 적절한 에러 출력
//		}
//		delete downInfo; // 다운이 종료되었으므로 삭제
//	}
//
// (NOTE) 파일 다운로딩은 흔히 쓰이는 기능이 아닌데다, blocking으로도 대부분 해결 가능하기 때문에
// 정규 framework에 별도의 singleton과 thread를 등록하기는 비용이 아까움. 따라서 자체적인 instance와
// thread manager를 가지고 필요 할 때 독자적으로 생성해 사용
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
		eDS_Wait = 0, // 대기 중. 초기값
		eDS_Downloading, // 다운로드 중

		// result
		eDS_Complete, // 완료
		eDS_Abort, // 사용자에 의한 중도포기
		eDS_InvalidURL, // fileURL에 해당하는 파일이 네트워크상 존재하지 않음
		eDS_OutOfMemory, // 메모리 부족
		eDS_Failed // 기타 사유로 실패
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

	// 파일 다운
	// fileURL : 네트워크상 파일 경로
	// destFilePath : 저장될 로컬 파일 경로. 루트 디렉토리 기준 상대, 혹은 절대 경로
	static bool DownloadFileRightNow(const MkStr& fileURL, const MkPathName& destFilePath);

	//------------------------------------------------------------------------------------------------//
	// none - blocking
	//------------------------------------------------------------------------------------------------//

	// 파일 다운
	// fileURL : 네트워크상 파일 경로
	// destFilePath : 저장될 로컬 파일 경로. 루트 디렉토리 기준 상대, 혹은 절대 경로
	// (NOTE) 반환된 fileDownInfo은 state가 result로 바뀐 이후 삭제해야 함
	MkFileDownInfo* DownloadFile(const MkStr& fileURL, const MkPathName& destFilePath);

	// none-blocking 형태로 다운 중일 경우 중지. 남아 있는 모든 다운 리스트 무효화
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
