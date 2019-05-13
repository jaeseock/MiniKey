#pragma once


//------------------------------------------------------------------------------------------------//
// MkByteArray을 zip 파일에 저장하기 위한 interface
//
// ex>
// MkByteArray(srcArray) -> file
//	MkInterfaceForZipFileWriting fwInterface;
//	fwInterface.SetUp(L"etc\\etc.zip", true, true);
//	fwInterface.Write(L"sample.sst", srcArray[0]);
//	fwInterface.Write(L"big.png", srcArray[1]);
//	fwInterface.Clear();
//------------------------------------------------------------------------------------------------//

#include "minizip/zip.h"
#include "MkCore_MkPathName.h"


class MkInterfaceForZipFileWriting
{
public:

	// 초기화 (파일 오픈 후 대기)
	// (in) filePath : 기록할 파일의 절대 혹은 root directory기준 상대 경로
	// (in) overwrite : 기존 파일이 존재 할 경우 true면 덮어쓰고 false면 추가로 덧붙힘
	// (in) makeDirectoryPath : 파일까지의 경로가 구성되어 있지 않은 경우 자동 경로 구성 여부
	// return : 파일오픈 성공여부
	bool SetUp(const MkPathName& filePath, bool overwrite = true, bool makeDirectoryPath = true);

	// 쓰기 유효성(파일이 열려 있는 상태) 검사
	inline bool IsValid(void) const { return (m_ZipFile != NULL); }

	// 메모리 블록을 압축해 기록
	// SetUp() ~ Clear() 사이에서 복수 호출 가능
	// (NOTE) 파일 중복체크 하지 않음. zip 파일은 동일 파일이 복수로 저장 될 수 있음
	// (in) filePath : 저장 할 파일의 상대 경로
	// (in) srcArray : 파일에 쓸 데이터가 담긴 byte array
	// return : 성공여부
	bool Write(const MkPathName& filePath, const MkByteArray& srcArray);

	// 파일 닫음
	void Clear(void);

	// 현재 오픈중인 zip파일 절대 경로 반환
	inline const MkPathName& GetCurrentFilePath(void) const { return m_CurrentFilePath; }

	MkInterfaceForZipFileWriting();
	~MkInterfaceForZipFileWriting() { Clear(); }

protected:

	zipFile m_ZipFile;

	MkPathName m_CurrentFilePath;
};
