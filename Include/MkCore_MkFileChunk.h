#ifndef __MINIKEY_CORE_MKFILECHUNK_H__
#define __MINIKEY_CORE_MKFILECHUNK_H__


//------------------------------------------------------------------------------------------------//
// 패킹된 청크 파일 관리자
// 파일 블록의 집합
// 하나의 청크 파일에 대한 관리
//
// chunk file에서의 구조는,
// 0. tag : 소속된 모든 블록 수 (unsigned int)
// 1. block 0
// 2. block 1
// ...
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkGlobalFunction.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkFileBlock.h"


class MkFilePathListContainer;

class MkFileChunk
{
public:

	//------------------------------------------------------------------------------------------------//
	// 초기화
	//------------------------------------------------------------------------------------------------//

	// 원시파일들로부터 청크 파일 구성
	// chunkFilePath 경로에 청크파일을 생성하고 filePathListContainer를 역순으로 읽어 블록 생성 후 청크파일 구성
	// 파일 구성 중 청크 파일 크기가 chunkSizeGuideline을 넘어가거나 파일 리스트가 종료되었을 경우
	// 구성에 사용된 파일 리스트들을 filePathListContainer에서 삭제한 후 종료
	//
	// (in) chunkFilePath : 청크 파일의 절대, 혹은 root directory 기준 상대 경로명
	// (in/out) filePathListContainer : 원본 파일 절대경로/상대경로 리스트. 소모된 파일은 청크 구성 완료시 삭제되어 반환됨
	// (out) memberFilePathList : 청크에 등록된(relativeFilePathList에서 소모된) 파일 리스트
	// (in) chunkSizeGuideline : chunk file 한계 크기
	// (in) percentageForCompressing : 압축 경계
	// (in) createNewChunk : 새 청크를 구축할지 여부. true면 기존 청크 정보/파일 삭제 후 새로 만들고
	//                       false면 기존 블록들 뒤에 추가
	// return : 성공 여부
	bool AttachOriginalFiles
		(const MkPathName& chunkFilePath, MkFilePathListContainer& filePathListContainer, MkArray<MkPathName>& memberFilePathList,
		unsigned int chunkSizeGuideline, unsigned int percentageForCompressing, bool createNewChunk = true);

	// 청크 파일로부터 구성
	// (in) chunkFilePath : 청크 파일의 절대, 혹은 root directory 기준 상대 경로명
	// (out) memberFilePathList : 청크에 등록된 파일 리스트. disable 파일의 경우 공문자열이 담김
	// return : 성공 여부
	bool SetUpFromChunkFile(const MkPathName& chunkFilePath, MkArray<MkPathName>& memberFilePathList);

	//------------------------------------------------------------------------------------------------//
	// 참조
	//------------------------------------------------------------------------------------------------//

	// 청크 파일 크기 반환
	inline unsigned int GetChunkSize(void) const { return m_ChunkSize; }

	// 사용 불가능한 블록 크기의 합 반환
	inline unsigned int GetBlankSize(void) const { return m_BlankSize; }

	// 전체 청크 파일 중 사용 불가능한 크기를 퍼센트로 반환
	inline unsigned int GetBlankPercentage(void) const { return ConvertToPercentage<unsigned int, unsigned int>(m_BlankSize, m_ChunkSize); }

	// 등록된 블록 수 반환
	inline unsigned int GetBlockCount(void) const { return m_BlockList.GetSize(); }

	// 청크 파일 경로 반환
	inline const MkPathName& GetAbsoluteChunkFilePath(void) const { return m_AbsoluteChunkFilePath; }

	// 블록 정보 반환
	// 갱신을 위한 정보(크기 및 수정일시)만 반환
	bool GetBlockInfo(unsigned int blockIndex, unsigned int& originalSize, unsigned int& dataSize, unsigned int& writtenTime) const;

	// 블록 정보 반환
	// data node에 블록 내 파일과 리렉토리 정보를 담아 반환
	// includeChunkInfo가 true일 경우 파일마다 소속 청크파일명과 block index를 추가 
	bool GetBlockInfo(unsigned int blockIndex, MkDataNode& directoryNode, bool includeChunkInfo) const;

	// blockIndex번 블록의 압축이 해제된 원본 파일 데이터를 buffer에 담아 반환
	// (in) blockIndex : 블록 번호
	// (out) buffer : 데이터가 담길 버퍼
	// return : 성공여부
	bool GetOriginalFileData(unsigned int blockIndex, MkByteArray& buffer) const;

	// 청크에 속한 유효 파일을 원본 파일로 복원
	// (in) blockIndex : 블록 번호
	// (in) destinationDirectoryPath : 복원될 절대, 혹은 root directory 기준 상대 디렉토리 경로
	// return : 성공 여부
	bool ExtractAvailableFile(unsigned int blockIndex, const MkPathName& destinationDirectoryPath) const;

	// 청크에 속한 모든 유효 파일들을 원본 파일로 복원
	// (in) destinationDirectoryPath : 복원될 절대, 혹은 root directory 기준 상대 디렉토리 경로
	// return : 성공 여부
	bool ExtractAllAvailableFiles(const MkPathName& destinationDirectoryPath) const;

	//------------------------------------------------------------------------------------------------//
	// 수정
	//------------------------------------------------------------------------------------------------//

	// blockIndex번 블록 삭제. disable 상태로 변경
	// 실제 파일에서 지워지는 것은 아니고 m_BlockList에도 남아 있음
	// return : 성공 여부
	bool RemoveFile(unsigned int blockIndex);

	// 유효한 블록만 남겨 청크 파일 재구성
	// 모든 블록이 disable일 경우 청크 파일 삭제
	// 인덱스가 변경된 블록 정보를 반환
	// (out) movedPathList : 변경 블록 상대경로
	// (out) newIndexList : 변경 블록 인덱스
	// return : 성공 여부
	bool OptimizeChunk(MkArray<MkPathName>& movedPathList, MkArray<unsigned int>& newIndexList);

	// 해제
	void Clear(void);

	MkFileChunk() { Clear(); }
	~MkFileChunk() { Clear(); }

	//------------------------------------------------------------------------------------------------//

protected:

	void _AddCountTag(MkInterfaceForFileWriting& fwInterface, unsigned int count) const;

	bool _CheckAvailableBlock(unsigned int blockIndex) const;

	bool _RebuildChunk(MkArray<MkPathName>& movedPathList, MkArray<unsigned int>& newIndexList);

	//------------------------------------------------------------------------------------------------//

protected:

	MkPathName m_AbsoluteChunkFilePath; // 청크 파일 절대경로

	MkArray<MkFileBlock> m_BlockList;

	unsigned int m_ChunkSize; // 청크 파일 크기
	unsigned int m_BlankSize; // 사용 불가능한 블록 크기의 합

	// for optimizing
	unsigned int m_AvailableBlockCount; // 유효한 블록 수
	unsigned int m_FirstDisableBlockIndex; // 최초 disable block index
};

//------------------------------------------------------------------------------------------------//

#endif
