#pragma once

//------------------------------------------------------------------------------------------------//
// 간단한 형태의 파일(url) 다운로더
//------------------------------------------------------------------------------------------------//

class MkStr;
class MkPathName;

class MkFileDownloader
{
public:

	enum eResult
	{
		eSuccess = 0, // 성공

		eInvalidURL, // fileURL에 해당하는 파일이 네트워크상 존재하지 않음
		eDestFilePathIsDirectory, // destFilePath가 디렉토리 경로임
		eNetworkError, // 무언가 네트워크 관련 에러 발생
		eOutOfMemory // 메모리 부족
	};

	// 다운 시작
	// fileURL : 네트워크상 파일 경로
	// destFilePath : 저장될 로컬 파일 경로. 루트 디렉토리 기준 상대, 혹은 절대 경로
	// tryCount : 네트워크 문제로 실패했을 경우 재도전 시도 회수(1초 텀 존재)
	static eResult Start(const MkStr& fileURL, const MkPathName& destFilePath, int retryCount = 9);
};

//------------------------------------------------------------------------------------------------//
