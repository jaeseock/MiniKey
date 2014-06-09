#ifndef __MINIKEY_CORE_MKSINTERFACEFORFILEREADING_H__
#define __MINIKEY_CORE_MKSINTERFACEFORFILEREADING_H__


//------------------------------------------------------------------------------------------------//
// 이진 파일에서 데이터를 MkByteArray으로 추출해내기 위한 interface
//
// ex>
//	// file -> MkByteArray(destBuffer)
//	MkByteArray destBuffer;
//	MkInterfaceForFileReading frInterface;
//	frInterface.SetUp(L"testblock\\test_block.bin");
//	frInterface.Read(destBuffer, MkArraySection(0));
//	frInterface.Clear();
//------------------------------------------------------------------------------------------------//

#include <fstream>
#include "MkCore_MkPathName.h"


class MkInterfaceForFileReading
{
public:

	// 초기화 (파일 오픈 후 대기)
	// (in) filePath : 기록할 파일의 절대 혹은 root directory기준 상대 경로
	// return : 파일오픈 성공여부
	bool SetUp(const MkPathName& filePath);

	// 현재 열려 있는 파일의 전체크기를 반환
	inline unsigned int GetFileSize(void) const { return m_FileSize; }

	// 읽기 유효성(파일이 열려 있고 EOF가 아닌 상태) 반환
	bool IsValid(void) const;

	// 현재 탐색 위치 이동
	void SetCurrentPosition(unsigned int newPosition);

	// 현재 탐색 위치 반환. 유효하지 않으면 MKDEF_ARRAY_ERROR 반환
	unsigned int GetCurrentPosition(void);

	// 메모리 블록으로 읽어들임
	// SetUp() ~ Clear() 사이에서 복수 호출 가능
	// (in) destBuffer : 읽어들인 데이터가 담길 byte array
	// (in) section : 파일내 읽어들일 구간
	// return : 실제 읽어들인 크기
	unsigned int Read(MkByteArray& destBuffer, const MkArraySection& section);

	// 파일 닫음
	void Clear(void);

	// 현재 오픈중인 파일 절대 경로 반환
	inline const MkPathName& GetCurrentFilePath(void) const { return m_CurrentFilePath; }

	MkInterfaceForFileReading() { m_FileSize = 0; }
	~MkInterfaceForFileReading() { Clear(); }

protected:

	std::ifstream m_Stream;
	unsigned int m_FileSize;

	MkPathName m_CurrentFilePath;
};

//------------------------------------------------------------------------------------------------//

#endif
