#ifndef __MINIKEY_CORE_MKINTERFACEFORFILEWRITING_H__
#define __MINIKEY_CORE_MKINTERFACEFORFILEWRITING_H__


//------------------------------------------------------------------------------------------------//
// MkByteArray을 파일에 저장하기 위한 interface
// 기록된 데이터는 특수한 구조나 태그 등이 없는 일반적인 바이너리 파일
// 다만 파일에서 MkByteArray로 다시 복원시키고 싶다면 MkInterfaceForFileReading 권장
//
// ex>
//	// MkByteArray(srcArray) -> file
//	MkInterfaceForFileWriting fwInterface;
//	fwInterface.SetUp(L"testblock\\test_block.bin", true, true);
//	fwInterface.Write(srcArray, MkArraySection(0));
//	fwInterface.Clear();
//------------------------------------------------------------------------------------------------//

#include <fstream>
#include "MkCore_MkPathName.h"


class MkInterfaceForFileWriting
{
public:

	// 초기화 (파일 오픈 후 대기)
	// (in) filePath : 기록할 파일의 절대 혹은 root directory기준 상대 경로
	// (in) overwrite : 기존 파일이 존재 할 경우 true면 덮어쓰고 false면 뒤에 덧붙힘
	// (in) makeDirectoryPath : 파일까지의 경로가 구성되어 있지 않은 경우 자동 경로 구성 여부
	// return : 파일오픈 성공여부
	bool SetUp(const MkPathName& filePath, bool overwrite = true, bool makeDirectoryPath = true);

	// SetUp부터 현재까지 기록된 파일 사이즈를 반환
	inline unsigned int GetWrittenSize(void) const { return m_WrittenSize; }

	// 쓰기 유효성(파일이 열려 있는 상태) 검사
	inline bool IsValid(void) const { return m_Stream.is_open(); }

	// 현재 탐색 위치 반환. 유효하지 않으면 MKDEF_ARRAY_ERROR 반환
	// 이어 쓰기(overwrite == false) 모드로 열었을 경우 기존 파일 크기 포함됨
	unsigned int GetCurrentPosition(void);

	// 메모리 블록을 기록
	// SetUp() ~ Clear() 사이에서 복수 호출 가능
	// (in) srcArray : 파일에 쓸 데이터가 담긴 byte array
	// (in) section : byte array 데이터 구간
	// return : 실제 기록된 크기
	unsigned int Write(const MkByteArray& srcArray, const MkArraySection& section);

	// 파일 닫음
	void Clear(void);

	// 현재 오픈중인 파일 절대 경로 반환
	inline const MkPathName& GetCurrentFilePath(void) const { return m_CurrentFilePath; }

	MkInterfaceForFileWriting() { m_WrittenSize = 0; }
	~MkInterfaceForFileWriting() { Clear(); }

protected:

	std::ofstream m_Stream;
	unsigned int m_WrittenSize;
	unsigned int m_InitialSize;

	MkPathName m_CurrentFilePath;
};

//------------------------------------------------------------------------------------------------//

#endif
