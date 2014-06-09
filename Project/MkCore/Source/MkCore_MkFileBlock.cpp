
#include "MkCore_MkCheck.h"
#include "MkCore_MkGlobalFunction.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkInterfaceForFileWriting.h"
#include "MkCore_MkInterfaceForFileReading.h"
#include "MkCore_MkInterfaceForDataWriting.h"
#include "MkCore_MkInterfaceForDataReading.h"
#include "MkCore_MkInterfaceForFileTag.h"
#include "MkCore_MkZipCompressor.h"
#include "MkCore_MkFileBlock.h"


#define MKDEF_FILE_BLOCK_TAG_SIZE  (sizeof(unsigned int) * 3) // sizeof(m_BlockState) + sizeof(m_BlockSize) + sizeof(m_HeaderSize)

//------------------------------------------------------------------------------------------------//

unsigned int MkFileBlock::SetUpFromOriginalFileAndAttachToChunkFile
(const MkPathName& absoluteOriginalFilePath, const MkPathName& relativeFilePath,
 MkInterfaceForFileWriting& chunkFileInterface, unsigned int percentageForCompressing)
{
	// relativeFilePath 유효성 검사
	MK_CHECK((!relativeFilePath.Empty()) && (!relativeFilePath.IsAbsolutePath()) && (!relativeFilePath.IsDirectoryPath()), L"파일 패킹중 비정상 상대경로 입력 : " + relativeFilePath)
		return 0;

	// 파일 체크 겸 수정시간 추출
	m_WrittenTime = absoluteOriginalFilePath.GetWrittenTime();
	MK_CHECK(m_WrittenTime != MKDEF_ARRAY_ERROR, absoluteOriginalFilePath + L" 경로의 원본 파일이 없음")
		return 0;

	// 원본 파일 읽음
	MkInterfaceForFileReading frInterface;
	if (!frInterface.SetUp(absoluteOriginalFilePath))
		return 0;

	MkByteArray uncompressedData;
	m_UncompressedSize = frInterface.Read(uncompressedData, MkArraySection(0));
	frInterface.Clear();

	// 원본 파일 크기 체크
	MK_CHECK(m_UncompressedSize > 0, absoluteOriginalFilePath + L" 경로의 원본 파일 크기가 0임")
		return 0;

	m_BlockState = eUncompressed;
	m_DataSize = m_UncompressedSize;
	m_CompressionEfficiency = 100;
	MkByteArray* dataBlockPtr = &uncompressedData;

	// debug msg
	MkStr infoMsg;
	if (MKDEF_SHOW_FILE_PACKING_INFO)
	{
		infoMsg = L"  - ";
		infoMsg += relativeFilePath;
		infoMsg += L"(";
		infoMsg += m_WrittenTime;
		infoMsg += L") ";
	}

	MkByteArray compressedData;
	if (percentageForCompressing > 0)
	{
		// 원본 압축
		unsigned int compressedSize = MkZipCompressor::Compress(uncompressedData.GetPtr(), uncompressedData.GetSize(), compressedData);
		MK_CHECK(compressedSize > 0, absoluteOriginalFilePath + L" 경로 파일은 압축되지 않는 형식임")
			return 0;

		// 압축률 체크
		unsigned int compressionEfficiency = ConvertToPercentage<unsigned int, unsigned int>(compressedSize, m_UncompressedSize);

		if (MKDEF_SHOW_FILE_PACKING_INFO)
		{
			infoMsg += L"CE ";
			infoMsg += compressionEfficiency;
			infoMsg += L"%, ";
		}

		if (compressionEfficiency > percentageForCompressing)
		{
			compressedData.Clear(); // 원본 데이터로 저장

			if (MKDEF_SHOW_FILE_PACKING_INFO)
			{
				infoMsg += L"N ";
				infoMsg += m_UncompressedSize;
			}
		}
		else
		{
			m_BlockState = eCompressed; // 압축 데이터로 저장
			m_DataSize = compressedSize;
			m_CompressionEfficiency = compressionEfficiency;
			dataBlockPtr = &compressedData;
			uncompressedData.Clear();

			if (MKDEF_SHOW_FILE_PACKING_INFO)
			{
				infoMsg += L"C ";
				infoMsg += compressedSize;
			}
		}
	}

	// 상대경로 갱신
	m_RelativeFilePath = relativeFilePath;

	// 헤더 블록 구성
	MkByteArray headerBlock;
	m_HeaderSize = _SetHeaderBlock(headerBlock);

	// block size
	m_BlockSize = MKDEF_FILE_BLOCK_TAG_SIZE + m_HeaderSize + m_DataSize;

	// 태그 블록 구성
	MkByteArray tagBlock;
	_SetTagBlock(tagBlock);

	// 청크 파일에 덧붙힘
	MK_CHECK(_WriteBlockToChunk(chunkFileInterface, tagBlock, headerBlock, *dataBlockPtr), absoluteOriginalFilePath + L" 패킹 실패")
		return 0;

	// debug
	if (!infoMsg.Empty())
	{
		MK_DEV_PANEL.MsgToLog(infoMsg);
	}

	return m_BlockSize;
}

bool MkFileBlock::SetUpFromChunkFile(MkInterfaceForFileReading& chunkFileInterface)
{
	m_PositionOnChunk = chunkFileInterface.GetCurrentPosition();
	MK_CHECK(m_PositionOnChunk != MKDEF_ARRAY_ERROR, chunkFileInterface.GetCurrentFilePath() + L" 청크에서 예기치 않은 파일 종료")
		return false;

	// 태그 블록 읽음
	MkByteArray tagBlock;
	MK_CHECK(chunkFileInterface.Read(tagBlock, MkArraySection(m_PositionOnChunk, MKDEF_FILE_BLOCK_TAG_SIZE)) == MKDEF_FILE_BLOCK_TAG_SIZE, chunkFileInterface.GetCurrentFilePath() + L" 청크에서 태그 블록 읽기 오류")
		return false;

	MkInterfaceForDataReading drInterface;
	drInterface.SetUp(tagBlock, 0); // 무조건 성공

	// 블록 상태
	unsigned int blockState = MKDEF_ARRAY_ERROR;
	MK_CHECK(drInterface.Read(blockState), chunkFileInterface.GetCurrentFilePath() + L" 청크에서 파일 블록 상태값 읽기 오류")
		return false;
	m_BlockState = static_cast<eFileBlockState>(blockState);
	MK_CHECK((m_BlockState == eDisable) || (m_BlockState == eUncompressed) || (m_BlockState == eCompressed), chunkFileInterface.GetCurrentFilePath() + L" 청크에서 파일 블록 상태값 오류")
		return false;

	// 블록 크기
	MK_CHECK(drInterface.Read(m_BlockSize), chunkFileInterface.GetCurrentFilePath() + L" 청크에서 파일 블록 크기 읽기 오류")
		return false;
	MK_CHECK((m_BlockSize > 0) && (m_BlockSize != MKDEF_ARRAY_ERROR), chunkFileInterface.GetCurrentFilePath() + L" 청크에서 파일 블록 크기 오류")
		return false;

	// 사용 불가 블록일 경우 여기서 종료. 더 이상의 정보는 필요 없음
	if (m_BlockState == eDisable)
		return true;

	// 헤더 크기
	MK_CHECK(drInterface.Read(m_HeaderSize), chunkFileInterface.GetCurrentFilePath() + L" 청크에서 헤더 크기 읽기 오류")
		return false;
	MK_CHECK((m_HeaderSize > 0) && (m_HeaderSize != MKDEF_ARRAY_ERROR), chunkFileInterface.GetCurrentFilePath() + L" 청크에서 헤더 크기 오류")
		return false;

	// 헤더 블록 읽음
	MkByteArray headerBlock;
	MK_CHECK(chunkFileInterface.Read(headerBlock, MkArraySection(m_PositionOnChunk + MKDEF_FILE_BLOCK_TAG_SIZE, m_HeaderSize)) == m_HeaderSize, chunkFileInterface.GetCurrentFilePath() + L" 청크에서 헤더 블록 읽기 오류")
		return false;

	drInterface.SetUp(headerBlock, 0); // 무조건 성공
	MK_CHECK(drInterface.Read(m_RelativeFilePath), chunkFileInterface.GetCurrentFilePath() + L" 청크에서 대상 파일 상대 경로 읽기 오류")
		return false;
	MK_CHECK(drInterface.Read(m_UncompressedSize), chunkFileInterface.GetCurrentFilePath() + L" 청크에서 원본 데이터 크기 읽기 오류")
		return false;
	MK_CHECK(drInterface.Read(m_WrittenTime), chunkFileInterface.GetCurrentFilePath() + L" 청크에서 원본 파일 수정일시 읽기 오류")
		return false;
	MK_CHECK(drInterface.Read(m_DataSize), chunkFileInterface.GetCurrentFilePath() + L" 청크에서 패킹된 실 데이터 크기 읽기 오류")
		return false;

	// 압축률
	m_CompressionEfficiency = (m_BlockState == eUncompressed) ? 100 : ConvertToPercentage<unsigned int, unsigned int>(m_DataSize, m_UncompressedSize);
	
	return true;
}

bool MkFileBlock::CopyBlockToNewChunk(MkInterfaceForFileReading& currentChunkFileInterface, MkInterfaceForFileWriting& newChunkFileInterface)
{
	MkByteArray dataBlock;
	unsigned int beginPosition = m_PositionOnChunk + MKDEF_FILE_BLOCK_TAG_SIZE + m_HeaderSize;
	MK_CHECK(currentChunkFileInterface.Read(dataBlock, MkArraySection(beginPosition, m_DataSize)) > 0, newChunkFileInterface.GetCurrentFilePath() + L" 청크에서 " + m_RelativeFilePath + L" 데이터 블록 읽기 실패")
		return false;

	// 헤더 블록 구성
	MkByteArray headerBlock;
	_SetHeaderBlock(headerBlock);

	// 태그 블록 구성
	MkByteArray tagBlock;
	_SetTagBlock(tagBlock);

	// 청크 파일에 덧붙힘
	MK_CHECK(_WriteBlockToChunk(newChunkFileInterface, tagBlock, headerBlock, dataBlock), newChunkFileInterface.GetCurrentFilePath() + L" 청크에 복사 실패")
		return false;

	return true;
}

void MkFileBlock::Clear(void)
{
	m_BlockState = eDisable;
	m_HeaderSize = sizeof(unsigned int) * 4;
	m_BlockSize = MKDEF_FILE_BLOCK_TAG_SIZE + m_HeaderSize;

	m_RelativeFilePath.Clear();
	m_UncompressedSize = 0;
	m_WrittenTime = 0;
	m_DataSize = 0;

	m_CompressionEfficiency = 0;
}

bool MkFileBlock::GetOriginalFileData(MkInterfaceForFileReading& chunkFileInterface, MkByteArray& buffer) const
{
	MK_CHECK(Available(), L"eDisable 블록에 접근 시도")
		return false;

	buffer.Clear();
	unsigned int beginPosition = m_PositionOnChunk + MKDEF_FILE_BLOCK_TAG_SIZE + m_HeaderSize;

	// 비압축 형태의 경우 바로 버퍼에 담아 반환
	if (m_BlockState == eUncompressed)
	{
		MK_CHECK(chunkFileInterface.Read(buffer, MkArraySection(beginPosition, m_DataSize)) > 0, m_RelativeFilePath + L" 파일 블록 읽기 실패")
			return false;
	}
	// 압축 형태의 경우 임시버퍼에 압축을 풀어 반환
	else
	{
		MkByteArray blockBuffer;
		MK_CHECK(chunkFileInterface.Read(blockBuffer, MkArraySection(beginPosition, m_DataSize)) > 0, m_RelativeFilePath + L" 파일 블록 읽기 실패")
			return false;
		MK_CHECK(MkZipCompressor::Uncompress(blockBuffer.GetPtr(), blockBuffer.GetSize(), buffer) == m_UncompressedSize, m_RelativeFilePath + L" 파일 블록 압축 크기가 원본과 다름")
			return false;
	}
	return true;
}

bool MkFileBlock::SetBlockState(const MkPathName& absoluteChunkFilePath, eFileBlockState newState)
{
	if (m_BlockState == newState)
		return true;

	MkInterfaceForFileTag<unsigned int> ftInterface;
	if (!ftInterface.SetUp(absoluteChunkFilePath))
		return false;

	MK_CHECK(ftInterface.Overwrite(static_cast<unsigned int>(newState), m_PositionOnChunk), absoluteChunkFilePath + L" 청크 파일의 " + m_RelativeFilePath + L" 블록 상태 변경 실패")
		return false;

	m_BlockState = newState;
	return true;
}

//------------------------------------------------------------------------------------------------//

bool MkFileBlock::_WriteBlockToChunk
(MkInterfaceForFileWriting& chunkFileInterface, const MkByteArray& tagBlock, const MkByteArray& headerBlock, const MkByteArray& dataBlock)
{
	// position
	m_PositionOnChunk = chunkFileInterface.GetCurrentPosition();

	// 파일에 태그 기록
	MK_CHECK(chunkFileInterface.Write(tagBlock, MkArraySection(0)) > 0, m_RelativeFilePath + L" 패킹중 청크에 태그 기록 실패")
		return false;

	// 파일에 헤더 기록
	MK_CHECK(chunkFileInterface.Write(headerBlock, MkArraySection(0)) > 0, m_RelativeFilePath + L" 패킹중 청크에 헤더 기록 실패")
		return false;

	// 파일에 데이터 기록
	MK_CHECK(chunkFileInterface.Write(dataBlock, MkArraySection(0)) > 0, m_RelativeFilePath + L" 패킹중 청크에 데이터 기록 실패")
		return false;

	return true;
}

void MkFileBlock::_SetTagBlock(MkByteArray& block) const
{
	MkInterfaceForDataWriting dwInterface;
	dwInterface.SetInputSize(ePDT_Int, 1);
	dwInterface.SetInputSize(ePDT_UnsignedInt, 2);
	dwInterface.UpdateInputSize();
	dwInterface.Write(static_cast<unsigned int>(m_BlockState));
	dwInterface.Write(m_BlockSize);
	dwInterface.Write(m_HeaderSize);
	dwInterface.Flush(block);
}

unsigned int MkFileBlock::_SetHeaderBlock(MkByteArray& block) const
{
	MkInterfaceForDataWriting dwInterface;
	dwInterface.SetInputSize(ePDT_Str, 1);
	dwInterface.SetInputSize(ePDT_UnsignedInt, 3);
	dwInterface.UpdateInputSize();
	dwInterface.Write(m_RelativeFilePath);
	dwInterface.Write(m_UncompressedSize);
	dwInterface.Write(m_WrittenTime);
	dwInterface.Write(m_DataSize);
	return dwInterface.Flush(block);
}

//------------------------------------------------------------------------------------------------//
