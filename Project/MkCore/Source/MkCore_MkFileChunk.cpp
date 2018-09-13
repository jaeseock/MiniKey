
#include "MkCore_MkCheck.h"
#include "MkCore_MkInterfaceForDataWriting.h"
#include "MkCore_MkInterfaceForFileWriting.h"
#include "MkCore_MkInterfaceForFileReading.h"
#include "MkCore_MkInterfaceForFileTag.h"
#include "MkCore_MkFilePathListContainer.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkFileChunk.h"


//------------------------------------------------------------------------------------------------//

bool MkFileChunk::AttachOriginalFiles
(const MkPathName& chunkFilePath, MkFilePathListContainer& filePathListContainer, MkArray<MkPathName>& memberFilePathList,
 unsigned int chunkSizeGuideline, unsigned int percentageForCompressing, bool createNewChunk)
{
	memberFilePathList.Clear();

	MkArray<MkPathName>& relativeFilePathList = filePathListContainer.GetRelativeFilePathList();
	unsigned int startCount = relativeFilePathList.GetSize();
	MK_CHECK(startCount > 0, L"파일 경로 리스트가 비어 " + m_AbsoluteChunkFilePath + L" 청크 파일 구성 불가")
		return false;

	if (createNewChunk)
	{
		MK_CHECK(!chunkFilePath.IsDirectoryPath(), chunkFilePath + L" 청크 파일 경로 오류")
			return false;

		Clear();

		// 새 청크 파일 이름
		m_AbsoluteChunkFilePath.ConvertToRootBasisAbsolutePath(chunkFilePath);
	}

	// 크기 예약
	m_BlockList.Reserve(m_BlockList.GetSize() + startCount);
	memberFilePathList.Reserve(startCount);

	// 인터페이스 생성. createNewChunk에 따라 청크 파일 생성
	MkInterfaceForFileWriting chunkFileInterface;
	if (!chunkFileInterface.SetUp(m_AbsoluteChunkFilePath, createNewChunk))
		return false;

	// 새 청크 파일의 경우 블록 카운트를 기록하기 위한 태그 공간 확보
	if (createNewChunk)
	{
		m_ChunkSize = sizeof(unsigned int);
		_AddCountTag(chunkFileInterface, m_ChunkSize);
	}

	// 뒤에서부터 앞으로 순회
	MK_INDEXING_RLOOP(relativeFilePathList, i)
	{
		const MkPathName& currPath = filePathListContainer.GetRelativeFilePath(i);

		MkFileBlock& currBlock = m_BlockList.PushBack();
		unsigned int blockSize = currBlock.SetUpFromOriginalFileAndAttachToChunkFile
			(filePathListContainer.GetAbsoluteOriginalFilePath(i), currPath, chunkFileInterface, percentageForCompressing, filePathListContainer.GetOuterWrittenTime(i));

		if (blockSize == 0)
		{
			if (createNewChunk)
			{
				m_AbsoluteChunkFilePath.DeleteCurrentFile();
			}
			return false;
		}

		memberFilePathList.PushBack(currPath);

		++m_AvailableBlockCount;

		m_ChunkSize += blockSize;
		if (m_ChunkSize >= chunkSizeGuideline)
			break;
	}

	chunkFileInterface.Clear();

	// 블록 카운트 수정
	MkInterfaceForFileTag<unsigned int> ftInterface;
	if (!ftInterface.SetUp(m_AbsoluteChunkFilePath))
	{
		if (createNewChunk)
		{
			m_AbsoluteChunkFilePath.DeleteCurrentFile();
		}
		return false;
	}

	ftInterface.Overwrite(m_BlockList.GetSize(), 0);
	ftInterface.Clear();

	// 청크에 구성된 파일들을 리스트에서 삭제
	filePathListContainer.PopBack(memberFilePathList.GetSize());
	return true;
}

bool MkFileChunk::SetUpFromChunkFile(const MkPathName& chunkFilePath, MkArray<MkPathName>& memberFilePathList)
{
	MK_CHECK((!chunkFilePath.Empty()) && (!chunkFilePath.IsDirectoryPath()), L" 청크 파일 경로 오류 : " + chunkFilePath)
		return false;

	Clear();
	memberFilePathList.Clear();

	// 청크 파일 이름
	m_AbsoluteChunkFilePath.ConvertToRootBasisAbsolutePath(chunkFilePath);

	// 블록 카운트 탐색
	MkInterfaceForFileTag<unsigned int> ftInterface;
	if (!ftInterface.SetUp(m_AbsoluteChunkFilePath))
		return false;

	unsigned int blockCount = ftInterface.GetTag(0);
	ftInterface.Clear();
	MK_CHECK(blockCount != MKDEF_ARRAY_ERROR, m_AbsoluteChunkFilePath + L" 청크 파일 태그 읽기 실패")
		return false;

	MK_CHECK(blockCount > 0, m_AbsoluteChunkFilePath + L" 청크 파일은 0개의 파일 블록 소유")
		return false;

	// 블록 크기 예약
	m_BlockList.Reserve(blockCount);

	// 반환용 배열 크기 예약
	memberFilePathList.Reserve(blockCount);

	// 청크 파일 열기
	MkInterfaceForFileReading frInterface;
	if (!frInterface.SetUp(m_AbsoluteChunkFilePath))
		return false;

	// 청크 파일에서의 탐색 위치. 먼저 태그 크기만큼 스킵
	m_ChunkSize = sizeof(unsigned int);
	
	// 청크 파일을 순회하며 블록 생성
	unsigned int lastBlockIndex = blockCount - 1;
	for (unsigned int i=0; i<blockCount; ++i)
	{
		// 인터페이스상 위치 이동
		frInterface.SetCurrentPosition(m_ChunkSize);

		// 블록 구성
		MkFileBlock& currBlock = m_BlockList.PushBack();
		if (!currBlock.SetUpFromChunkFile(frInterface))
			return false;

		unsigned int blockSize = currBlock.GetBlockSize();
		m_ChunkSize += blockSize;

		// 마지막 블록의 경우 청크 크기 검증
		// 만약 이전 청크 파일 구성시 마지막 블록 데이터 기록 중 오류가 발생했다면 헤더상 크기보다 실 파일 크기가 작으므로
		// 마지막 블록 제거 후 청크 재구성
		// (NOTE) 헤더 기록 중 오류가 났을 경우는 어쩔 수 없음
		if ((i == lastBlockIndex) && (m_ChunkSize > frInterface.GetFileSize()))
		{
			m_ChunkSize -= blockSize;
			m_BlockList.PopBack();

			MkArray<MkPathName> movedPathList;
			MkArray<unsigned int> newIndexList;
			if (!_RebuildChunk(movedPathList, newIndexList))
				return false;
		}
		// 나머지 경우는 정상적인 내부값 갱신
		else
		{
			if (currBlock.Available())
			{
				memberFilePathList.PushBack(currBlock.GetRelativeFilePath());
				++m_AvailableBlockCount;
			}
			else
			{
				m_BlankSize += blockSize;
				memberFilePathList.PushBack(L"");

				if (m_FirstDisableBlockIndex == MKDEF_ARRAY_ERROR)
				{
					m_FirstDisableBlockIndex = i;
				}
			}
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------//

bool MkFileChunk::GetBlockInfo(unsigned int blockIndex, unsigned int& originalSize, unsigned int& dataSize, unsigned int& writtenTime) const
{
	if (!_CheckAvailableBlock(blockIndex))
		return false;

	const MkFileBlock& block = m_BlockList[blockIndex];
	originalSize = block.GetUncompressedSize();
	dataSize = block.GetDataSize();
	writtenTime = block.GetWrittenTime();
	return true;
}

bool MkFileChunk::GetBlockInfo(unsigned int blockIndex, MkDataNode& directoryNode, bool includeChunkInfo) const
{
	if (!_CheckAvailableBlock(blockIndex))
		return false;

	const MkFileBlock& block = m_BlockList[blockIndex];
	if (block.Available())
	{
		MkPathName filePath = block.GetRelativeFilePath();
		MkDataNode* fileNode = filePath.GetFileName().__CreateFileStructureInfo
			(directoryNode,
			block.GetBlockState() == MkFileBlock::eCompressed,
			block.GetUncompressedSize(),
			block.GetDataSize(),
			block.GetWrittenTime());

		// chunk & block 정보 기록
		if (includeChunkInfo && (fileNode != NULL))
		{
			const static MkHashStr KEY_BLOCK_FILE_NAME = L"BF";
			fileNode->CreateUnit(KEY_BLOCK_FILE_NAME, m_AbsoluteChunkFilePath.GetFileName());

			const static MkHashStr KEY_BLOCK_INDEX = L"BI";
			fileNode->CreateUnit(KEY_BLOCK_INDEX, blockIndex);
		}
		return (fileNode != NULL);
	}
	return true;
}

bool MkFileChunk::GetOriginalFileData(unsigned int blockIndex, MkByteArray& buffer) const
{
	if (!_CheckAvailableBlock(blockIndex))
		return false;

	MkInterfaceForFileReading frInterface;
	if (!frInterface.SetUp(m_AbsoluteChunkFilePath))
		return false;

	return m_BlockList[blockIndex].GetOriginalFileData(frInterface, buffer);
}

bool MkFileChunk::ExtractAvailableFile(unsigned int blockIndex, const MkPathName& destinationDirectoryPath) const
{
	MK_CHECK(destinationDirectoryPath.IsDirectoryPath(), destinationDirectoryPath + L"는 디렉토리 경로가 아님")
		return false;

	MkByteArray buffer;
	if (!GetOriginalFileData(blockIndex, buffer))
		return false;

	MkPathName targetPath = destinationDirectoryPath;
	targetPath += m_BlockList[blockIndex].GetRelativeFilePath();

	MkInterfaceForFileWriting fwInterface;
	if (!fwInterface.SetUp(targetPath))
		return false;

	fwInterface.Write(buffer, MkArraySection(0));
	return true;
}

bool MkFileChunk::ExtractAllAvailableFiles(const MkPathName& destinationDirectoryPath) const
{
	MK_CHECK(destinationDirectoryPath.IsDirectoryPath(), destinationDirectoryPath + L"는 디렉토리 경로가 아님")
		return false;

	MkInterfaceForFileReading frInterface;
	if (!frInterface.SetUp(m_AbsoluteChunkFilePath))
		return false;

	MkPathName dest;
	dest.ConvertToRootBasisAbsolutePath(destinationDirectoryPath);

	MK_INDEXING_LOOP(m_BlockList, i)
	{
		const MkFileBlock& currBlock = m_BlockList[i];
		if (currBlock.Available())
		{
			MkByteArray buffer;
			if (!currBlock.GetOriginalFileData(frInterface, buffer))
				return false;
			
			MkPathName targetPath = destinationDirectoryPath;
			targetPath += currBlock.GetRelativeFilePath();

			MkInterfaceForFileWriting fwInterface;
			if (!fwInterface.SetUp(targetPath))
				return false;

			fwInterface.Write(buffer, MkArraySection(0));
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------//

bool MkFileChunk::RemoveFile(unsigned int blockIndex)
{
	if (!_CheckAvailableBlock(blockIndex))
		return false;

	MkFileBlock& currBlock = m_BlockList[blockIndex];
	bool ok = currBlock.SetBlockState(m_AbsoluteChunkFilePath, MkFileBlock::eDisable);
	if (ok)
	{
		m_BlankSize += currBlock.GetBlockSize();
		--m_AvailableBlockCount;
		if (blockIndex < m_FirstDisableBlockIndex)
		{
			m_FirstDisableBlockIndex = blockIndex;
		}
	}
	return ok;
}

bool MkFileChunk::OptimizeChunk(MkArray<MkPathName>& movedPathList, MkArray<unsigned int>& newIndexList)
{
	// disable 블록 없음
	if (m_BlankSize == 0)
		return true;

	return _RebuildChunk(movedPathList, newIndexList);
}

//------------------------------------------------------------------------------------------------//

void MkFileChunk::Clear(void)
{
	m_AbsoluteChunkFilePath.Clear();
	m_BlockList.Clear();
	m_ChunkSize = 0;
	m_BlankSize = 0;
	m_AvailableBlockCount = 0;
	m_FirstDisableBlockIndex = MKDEF_ARRAY_ERROR;
}

//------------------------------------------------------------------------------------------------//

void MkFileChunk::_AddCountTag(MkInterfaceForFileWriting& fwInterface, unsigned int count) const
{
	MkInterfaceForDataWriting dwInterface;
	dwInterface.Write(count);
	MkByteArray tagBlock;
	dwInterface.Flush(tagBlock);
	fwInterface.Write(tagBlock, MkArraySection(0));
}

bool MkFileChunk::_CheckAvailableBlock(unsigned int blockIndex) const
{
	MK_CHECK(m_AbsoluteChunkFilePath.CheckAvailable(), m_AbsoluteChunkFilePath + L" 청크 파일은 사용 불가")
		return false;

	MK_CHECK(m_BlockList.IsValidIndex(blockIndex), MkStr(blockIndex) + L"번 블록은 " + m_AbsoluteChunkFilePath + L" 청크 파일에 없음")
		return false;

	return true;
}

bool MkFileChunk::_RebuildChunk(MkArray<MkPathName>& movedPathList, MkArray<unsigned int>& newIndexList)
{
	MK_CHECK(m_AbsoluteChunkFilePath.CheckAvailable(), m_AbsoluteChunkFilePath + L" 청크 파일은 사용 불가")
		return false;

	unsigned int blockCount = m_BlockList.GetSize();

	// 만약 블록이 비었거나 전체가 모두 disable이면 청크 파일 삭제
	if ((blockCount == 0) || (m_AvailableBlockCount == 0))
	{
		return m_AbsoluteChunkFilePath.DeleteCurrentFile();
	}

	// 인덱스가 변할 블록 수(m_FirstDisableBlockIndex 이후의 블록 수)
	unsigned int movedBlockCount = blockCount - m_FirstDisableBlockIndex - 1;

	// 새 블록 리스트
	MkArray<MkFileBlock> newBlockList(m_AvailableBlockCount);

	// 인덱스 변경이 발생한다면 배열 공간 확보
	if (movedBlockCount > 0)
	{
		movedPathList.Reserve(movedBlockCount);
		newIndexList.Reserve(movedBlockCount);
	}

	// 임시 청크 파일 이름
	MkPathName temporaryFilePath;
	MkStr tName, tExtension;
	m_AbsoluteChunkFilePath.SplitPath(temporaryFilePath, tName, tExtension);
	temporaryFilePath.Reserve(temporaryFilePath.GetSize() + tName.GetSize() + 5);
	temporaryFilePath += L"_";
	temporaryFilePath += tName;
	temporaryFilePath += L".tmp";

	// 기존 청크파일 오픈
	MkInterfaceForFileReading oldInterface;
	if (!oldInterface.SetUp(m_AbsoluteChunkFilePath))
		return false;

	// 임시 청크파일 생성
	MkInterfaceForFileWriting tempInterface;
	if (!tempInterface.SetUp(temporaryFilePath))
		return false;

	// 블록 카운트를 기록하기 위한 태그 공간 확보
	_AddCountTag(tempInterface, m_AvailableBlockCount);

	// 블록을 순회하며 최적화
	for (unsigned int i=0; i<blockCount; ++i)
	{
		const MkFileBlock& currBlock = m_BlockList[i];

		// 유효성 검사
		if (currBlock.Available())
		{
			unsigned int newIndex = newBlockList.GetSize();
			MkFileBlock& newBlock = newBlockList.PushBack(); // 새 블록을 생성
			newBlock = currBlock; // 이전 블록 복사
			if (!newBlock.CopyBlockToNewChunk(oldInterface, tempInterface)) // 새 블록에서 새 청크로 카피(새 청크에서의 위치도 갱신됨)
				return false;

			// 인덱스가 변하는 블록이라면 결과 리스트에 등록
			if (blockCount > m_FirstDisableBlockIndex)
			{
				movedPathList.PushBack(newBlock.GetRelativeFilePath());
				newIndexList.PushBack(newIndex);
			}
		}
	}

	tempInterface.Clear();
	oldInterface.Clear();

	// 원본 청크 삭제
	MK_CHECK(m_AbsoluteChunkFilePath.DeleteCurrentFile(), L"최적화 후 " + m_AbsoluteChunkFilePath + L" 파일이 삭제되지 않음")
	{
		temporaryFilePath.DeleteCurrentFile();
		return false;
	}

	// 임시 청크파일 이름 변경
	// (NOTE) 만약 실패할 경우 원본, 임시 청크가 모두 없는 상태가 됨
	MK_CHECK(temporaryFilePath.RenameCurrentFile(m_AbsoluteChunkFilePath.GetFileName()), temporaryFilePath + L" 임시 청크 파일 이름 변경 실패")
		return false;

	// 새 블록 리스트로 대체
	m_BlockList = newBlockList;

	// 정보 갱신
	m_ChunkSize -= m_BlankSize;
	m_BlankSize = 0;
	m_FirstDisableBlockIndex = MKDEF_ARRAY_ERROR;

	return true;
}

//------------------------------------------------------------------------------------------------//
