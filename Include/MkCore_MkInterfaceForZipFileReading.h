#pragma once


//------------------------------------------------------------------------------------------------//
// zip 파일에서 데이터를 MkByteArray으로 추출해내기 위한 interface
//
// ex>
//	// file -> MkByteArray(destBuffer)
//	MkByteArray destBuffer[2];
//	MkInterfaceForZipFileReading frInterface;
//	frInterface.SetUp(L"etc\\etc.zip"); // zip파일 안에 "sample.sst", "image/big.png"가 있다면
//	frInterface.Read(L"etc\\sample.sst", destBuffer[0]);
//	frInterface.Read(L"etc\\image\\big.png", destBuffer[1]);
//	frInterface.Clear();
//------------------------------------------------------------------------------------------------//

#include "minizip/unzip.h"
#include "MkCore_MkPathName.h"


class MkInterfaceForZipFileReading
{
public:

	// 초기화 (파일 오픈 후 대기)
	// (in) filePath : 읽을 zip파일의 절대 혹은 root directory기준 상대 경로
	// (in) addRelativePathToKey : zip파일 내 리스트에 root directory기준 상대 경로를 덧붙힐지 여부
	// return : 파일오픈 성공여부
	bool SetUp(const MkPathName& filePath, bool addRelativePathToKey = true);

	// 읽기 유효성 반환
	inline bool IsValid(void) const { return (m_ZipFile != NULL); }

	// 포함 된 파일 수 반환
	inline unsigned int GetFileCount(void) const { return m_FileInfos.GetSize(); }

	// 포함 된 모든 파일 경로와 수 반환
	inline unsigned int GetFileList(MkArray<MkHashStr>& buffer) const { return m_FileInfos.GetKeyList(buffer); }

	// 메모리 블록으로 압축 풀린 원본 데이터를 읽어들임
	// SetUp() ~ Clear() 사이에서 복수 호출 가능
	// (in) filePath : 읽어들일 파일의 root directory기준 상대 경로
	// (in) password : 암호화 파일의 경우 해당 암호
	// (in) destBuffer : 읽어들인 데이터가 담길 byte array
	// return : 실제 읽어들인 크기
	unsigned int Read(const MkPathName& filePath, MkByteArray& destBuffer);
	unsigned int Read(const MkPathName& filePath, const MkStr& password, MkByteArray& destBuffer);

	// 파일 닫음
	void Clear(void);

	// 현재 오픈중인 zip파일 절대 경로 반환
	inline const MkPathName& GetCurrentFilePath(void) const { return m_CurrentFilePath; }

	MkInterfaceForZipFileReading();
	~MkInterfaceForZipFileReading() { Clear(); }

protected:

	typedef struct __CompFileInfo
	{
		uLong posInCentralDir;
		uLong numFile;
		bool needPassword;
	}
	_CompFileInfo;

	unsigned int _Read(const MkPathName& filePath, const MkStr& password, MkByteArray& destBuffer);

protected:

	unzFile m_ZipFile;

	MkHashMap<MkHashStr, _CompFileInfo> m_FileInfos;

	MkPathName m_CurrentFilePath;
};
