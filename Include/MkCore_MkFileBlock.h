#ifndef __MINIKEY_CORE_MKFILEBLOCK_H__
#define __MINIKEY_CORE_MKFILEBLOCK_H__


//------------------------------------------------------------------------------------------------//
// 패킹된 개별 파일 관리자
//
// chunk file에서의 블록 부분 구조는,
// 0. tag : m_BlockState (unsigned int)
// 1. tag : m_BlockSize (unsigned int) -> sizeof(0 + 1 + 2) + m_HeaderSize + m_DataSize
// 2. tag : m_HeaderSize (unsigned int) -> sizeof(3 + 4 + 5 + 6)
// 3. header : m_RelativeFilePath (MkStr)
// 4. header : m_UncompressedSize (unsigned int)
// 5. header : m_WrittenTime (unsigned int)
// 6. header : m_DataSize (unsigned int)
// 7. data (MkByteArray)
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkStr.h"

class MkPathName;
class MkInterfaceForFileWriting;
class MkInterfaceForFileReading;

class MkFileBlock
{
public:

	enum eFileBlockState
	{
		eDisable = 0, // 사용 불가
		eUncompressed, // 사용 가능, 비압축 상태
		eCompressed // 사용 가능, 압축 상태
	};

public:

	// 원본 파일의 데이터를 읽어들여 블록 헤더를 구성한 뒤 압축여부를 판별해 헤더와 압축/비압축 된 데이터를
	// chunkFileInterface를 통해 chunk file 뒤에 출력
	//
	// 압축률(압축 후 크기/원본 크기)이 좋지 않다면 원본 유지가 오히려 유리하기때문에 압축 경계 이하일 경우만 압축해 저장
	// 0이면 전혀 압축을 하지 않고 100 이상이면 대부분 압축(jpg처럼 이미 압축된 파일의 경우 재압축하면 오히려 용량이 약간 늘어날 수 있음)
	// ex> (percentageForCompressing == 70), 1000크기의 파일이 압축후 700 이하면 압축, 아니면 원본유지
	//
	// (in) absoluteOriginalFilePath : 추가할 원본 파일의 절대 경로
	// (in) relativeFilePath : 파일 시스템상에서 사용될 가상의 상대 파일 경로
	// (in) chunkFileInterface : chunk file이 오픈된 쓰기 interface
	// (in) percentageForCompressing : 압축 경계
	// return : 블록 크기(기록된 크기==증가된 파일 크기==m_BlockSize) 반환
	//          패킹중 문제가 발생하거나 원본 파일 크기가 0이면 0 반환
	// (NOTE) chunkFileInterface 유효성 검사 하지 않음
	unsigned int SetUpFromOriginalFileAndAttachToChunkFile
		(const MkPathName& absoluteOriginalFilePath, const MkPathName& relativeFilePath,
		MkInterfaceForFileWriting& chunkFileInterface, unsigned int percentageForCompressing);

	// 청크 파일로부터 블록 구성
	// (in) chunkFileInterface : 청크파일이 오픈되어 블록 시작 위치를 가리키고 있는 file reading interface
	// return : 성공 여부
	// (NOTE) chunkFileInterface 유효성 검사 하지 않음
	bool SetUpFromChunkFile(MkInterfaceForFileReading& chunkFileInterface);

	// 현 블록을 newChunkFileInterface를 통해 새 청크 파일에 기록
	// 블록 정보는 새 청크 파일 기준으로 맞추어짐
	// (in) currentChunkFileInterface : 데이터를 읽어들일 원 청크 파일 interface
	// (in) newChunkFileInterface : 블록을 추가 할 새 청크 파일 interface
	// (NOTE) currentChunkFileInterface, newChunkFileInterface 유효성 검사 하지 않음
	bool CopyBlockToNewChunk(MkInterfaceForFileReading& currentChunkFileInterface, MkInterfaceForFileWriting& newChunkFileInterface);

	// 해제
	void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// 참조
	//------------------------------------------------------------------------------------------------//

	// 압축이 해제된 원본 파일 데이터 반환
	// (NOTE) chunkFileInterface 유효성 검사 하지 않음
	bool GetOriginalFileData(MkInterfaceForFileReading& chunkFileInterface, MkByteArray& buffer) const;

	// 블록 상태 반환
	inline eFileBlockState GetBlockState(void) const { return m_BlockState; }

	// 블록 사용여부 반환
	inline bool Available(void) const { return (m_BlockState != eDisable); }

	// 블록 크기 반환
	inline unsigned int GetBlockSize(void) const { return m_BlockSize; }

	// 헤더 크기 반환
	inline unsigned int GetHeaderSize(void) const { return m_HeaderSize; }

	// 대상 파일의 상대 경로 반환
	inline const MkStr& GetRelativeFilePath(void) const { return m_RelativeFilePath; }

	// 2000년 이후 초단위로 환산된 원본 파일 수정일시 반환
	inline unsigned int GetWrittenTime(void) const { return m_WrittenTime; }

	// 실 데이터 크기 반환
	inline unsigned int GetDataSize(void) const { return m_DataSize; }

	// 압축 전 데이터 크기 반환
	inline unsigned int GetUncompressedSize(void) const { return m_UncompressedSize; }

	// 압축률 반환(참조용)
	inline unsigned int GetCompressionEfficiency(void) const { return m_CompressionEfficiency; }

	// 크기와 수정일시를 비교해 다름 여부를 반환
	inline bool CheckDifference(unsigned int uncompressedSize, unsigned int writtenTime) const
	{
		return ((uncompressedSize != m_UncompressedSize) || (writtenTime != m_WrittenTime));
	}

	//------------------------------------------------------------------------------------------------//
	// 수정
	//------------------------------------------------------------------------------------------------//

	bool SetBlockState(const MkPathName& absoluteChunkFilePath, eFileBlockState newState);

	MkFileBlock() { Clear(); }
	~MkFileBlock() {}

	//------------------------------------------------------------------------------------------------//

protected:

	// 청크 파일에 tag, header, data block을 기록
	bool _WriteBlockToChunk(MkInterfaceForFileWriting& chunkFileInterface,
		const MkByteArray& tagBlock, const MkByteArray& headerBlock, const MkByteArray& dataBlock);

	void _SetTagBlock(MkByteArray& block) const;
	unsigned int _SetHeaderBlock(MkByteArray& block) const;

	//------------------------------------------------------------------------------------------------//

protected:

	eFileBlockState m_BlockState; // 블록 상태
	unsigned int m_BlockSize; // 블록 크기
	unsigned int m_HeaderSize; // 아래 값들로 구성된 헤더 크기

	MkStr m_RelativeFilePath; // 대상 파일의 상대 경로
	unsigned int m_UncompressedSize; // 원본 데이터 크기
	unsigned int m_WrittenTime; // 원본 파일 수정일시를 2000년 이후 초단위로 환산한 값
	unsigned int m_DataSize; // 패킹된 실 데이터 크기

	unsigned int m_CompressionEfficiency; // 압축률
	unsigned int m_PositionOnChunk;
};

//------------------------------------------------------------------------------------------------//

#endif
