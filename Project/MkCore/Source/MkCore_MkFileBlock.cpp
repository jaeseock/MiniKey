
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
	// relativeFilePath ��ȿ�� �˻�
	MK_CHECK((!relativeFilePath.Empty()) && (!relativeFilePath.IsAbsolutePath()) && (!relativeFilePath.IsDirectoryPath()), L"���� ��ŷ�� ������ ����� �Է� : " + relativeFilePath)
		return 0;

	// ���� üũ �� �����ð� ����
	m_WrittenTime = absoluteOriginalFilePath.GetWrittenTime();
	MK_CHECK(m_WrittenTime != MKDEF_ARRAY_ERROR, absoluteOriginalFilePath + L" ����� ���� ������ ����")
		return 0;

	// ���� ���� ����
	MkInterfaceForFileReading frInterface;
	if (!frInterface.SetUp(absoluteOriginalFilePath))
		return 0;

	MkByteArray uncompressedData;
	m_UncompressedSize = frInterface.Read(uncompressedData, MkArraySection(0));
	frInterface.Clear();

	// ���� ���� ũ�� üũ
	MK_CHECK(m_UncompressedSize > 0, absoluteOriginalFilePath + L" ����� ���� ���� ũ�Ⱑ 0��")
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
		// ���� ����
		unsigned int compressedSize = MkZipCompressor::Compress(uncompressedData.GetPtr(), uncompressedData.GetSize(), compressedData);
		MK_CHECK(compressedSize > 0, absoluteOriginalFilePath + L" ��� ������ ������� �ʴ� ������")
			return 0;

		// ����� üũ
		unsigned int compressionEfficiency = ConvertToPercentage<unsigned int, unsigned int>(compressedSize, m_UncompressedSize);

		if (MKDEF_SHOW_FILE_PACKING_INFO)
		{
			infoMsg += L"CE ";
			infoMsg += compressionEfficiency;
			infoMsg += L"%, ";
		}

		if (compressionEfficiency > percentageForCompressing)
		{
			compressedData.Clear(); // ���� �����ͷ� ����

			if (MKDEF_SHOW_FILE_PACKING_INFO)
			{
				infoMsg += L"N ";
				infoMsg += m_UncompressedSize;
			}
		}
		else
		{
			m_BlockState = eCompressed; // ���� �����ͷ� ����
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

	// ����� ����
	m_RelativeFilePath = relativeFilePath;

	// ��� ��� ����
	MkByteArray headerBlock;
	m_HeaderSize = _SetHeaderBlock(headerBlock);

	// block size
	m_BlockSize = MKDEF_FILE_BLOCK_TAG_SIZE + m_HeaderSize + m_DataSize;

	// �±� ��� ����
	MkByteArray tagBlock;
	_SetTagBlock(tagBlock);

	// ûũ ���Ͽ� ������
	MK_CHECK(_WriteBlockToChunk(chunkFileInterface, tagBlock, headerBlock, *dataBlockPtr), absoluteOriginalFilePath + L" ��ŷ ����")
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
	MK_CHECK(m_PositionOnChunk != MKDEF_ARRAY_ERROR, chunkFileInterface.GetCurrentFilePath() + L" ûũ���� ����ġ ���� ���� ����")
		return false;

	// �±� ��� ����
	MkByteArray tagBlock;
	MK_CHECK(chunkFileInterface.Read(tagBlock, MkArraySection(m_PositionOnChunk, MKDEF_FILE_BLOCK_TAG_SIZE)) == MKDEF_FILE_BLOCK_TAG_SIZE, chunkFileInterface.GetCurrentFilePath() + L" ûũ���� �±� ��� �б� ����")
		return false;

	MkInterfaceForDataReading drInterface;
	drInterface.SetUp(tagBlock, 0); // ������ ����

	// ��� ����
	unsigned int blockState = MKDEF_ARRAY_ERROR;
	MK_CHECK(drInterface.Read(blockState), chunkFileInterface.GetCurrentFilePath() + L" ûũ���� ���� ��� ���°� �б� ����")
		return false;
	m_BlockState = static_cast<eFileBlockState>(blockState);
	MK_CHECK((m_BlockState == eDisable) || (m_BlockState == eUncompressed) || (m_BlockState == eCompressed), chunkFileInterface.GetCurrentFilePath() + L" ûũ���� ���� ��� ���°� ����")
		return false;

	// ��� ũ��
	MK_CHECK(drInterface.Read(m_BlockSize), chunkFileInterface.GetCurrentFilePath() + L" ûũ���� ���� ��� ũ�� �б� ����")
		return false;
	MK_CHECK((m_BlockSize > 0) && (m_BlockSize != MKDEF_ARRAY_ERROR), chunkFileInterface.GetCurrentFilePath() + L" ûũ���� ���� ��� ũ�� ����")
		return false;

	// ��� �Ұ� ����� ��� ���⼭ ����. �� �̻��� ������ �ʿ� ����
	if (m_BlockState == eDisable)
		return true;

	// ��� ũ��
	MK_CHECK(drInterface.Read(m_HeaderSize), chunkFileInterface.GetCurrentFilePath() + L" ûũ���� ��� ũ�� �б� ����")
		return false;
	MK_CHECK((m_HeaderSize > 0) && (m_HeaderSize != MKDEF_ARRAY_ERROR), chunkFileInterface.GetCurrentFilePath() + L" ûũ���� ��� ũ�� ����")
		return false;

	// ��� ��� ����
	MkByteArray headerBlock;
	MK_CHECK(chunkFileInterface.Read(headerBlock, MkArraySection(m_PositionOnChunk + MKDEF_FILE_BLOCK_TAG_SIZE, m_HeaderSize)) == m_HeaderSize, chunkFileInterface.GetCurrentFilePath() + L" ûũ���� ��� ��� �б� ����")
		return false;

	drInterface.SetUp(headerBlock, 0); // ������ ����
	MK_CHECK(drInterface.Read(m_RelativeFilePath), chunkFileInterface.GetCurrentFilePath() + L" ûũ���� ��� ���� ��� ��� �б� ����")
		return false;
	MK_CHECK(drInterface.Read(m_UncompressedSize), chunkFileInterface.GetCurrentFilePath() + L" ûũ���� ���� ������ ũ�� �б� ����")
		return false;
	MK_CHECK(drInterface.Read(m_WrittenTime), chunkFileInterface.GetCurrentFilePath() + L" ûũ���� ���� ���� �����Ͻ� �б� ����")
		return false;
	MK_CHECK(drInterface.Read(m_DataSize), chunkFileInterface.GetCurrentFilePath() + L" ûũ���� ��ŷ�� �� ������ ũ�� �б� ����")
		return false;

	// �����
	m_CompressionEfficiency = (m_BlockState == eUncompressed) ? 100 : ConvertToPercentage<unsigned int, unsigned int>(m_DataSize, m_UncompressedSize);
	
	return true;
}

bool MkFileBlock::CopyBlockToNewChunk(MkInterfaceForFileReading& currentChunkFileInterface, MkInterfaceForFileWriting& newChunkFileInterface)
{
	MkByteArray dataBlock;
	unsigned int beginPosition = m_PositionOnChunk + MKDEF_FILE_BLOCK_TAG_SIZE + m_HeaderSize;
	MK_CHECK(currentChunkFileInterface.Read(dataBlock, MkArraySection(beginPosition, m_DataSize)) > 0, newChunkFileInterface.GetCurrentFilePath() + L" ûũ���� " + m_RelativeFilePath + L" ������ ��� �б� ����")
		return false;

	// ��� ��� ����
	MkByteArray headerBlock;
	_SetHeaderBlock(headerBlock);

	// �±� ��� ����
	MkByteArray tagBlock;
	_SetTagBlock(tagBlock);

	// ûũ ���Ͽ� ������
	MK_CHECK(_WriteBlockToChunk(newChunkFileInterface, tagBlock, headerBlock, dataBlock), newChunkFileInterface.GetCurrentFilePath() + L" ûũ�� ���� ����")
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
	MK_CHECK(Available(), L"eDisable ��Ͽ� ���� �õ�")
		return false;

	buffer.Clear();
	unsigned int beginPosition = m_PositionOnChunk + MKDEF_FILE_BLOCK_TAG_SIZE + m_HeaderSize;

	// ����� ������ ��� �ٷ� ���ۿ� ��� ��ȯ
	if (m_BlockState == eUncompressed)
	{
		MK_CHECK(chunkFileInterface.Read(buffer, MkArraySection(beginPosition, m_DataSize)) > 0, m_RelativeFilePath + L" ���� ��� �б� ����")
			return false;
	}
	// ���� ������ ��� �ӽù��ۿ� ������ Ǯ�� ��ȯ
	else
	{
		MkByteArray blockBuffer;
		MK_CHECK(chunkFileInterface.Read(blockBuffer, MkArraySection(beginPosition, m_DataSize)) > 0, m_RelativeFilePath + L" ���� ��� �б� ����")
			return false;
		MK_CHECK(MkZipCompressor::Uncompress(blockBuffer.GetPtr(), blockBuffer.GetSize(), buffer) == m_UncompressedSize, m_RelativeFilePath + L" ���� ��� ���� ũ�Ⱑ ������ �ٸ�")
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

	MK_CHECK(ftInterface.Overwrite(static_cast<unsigned int>(newState), m_PositionOnChunk), absoluteChunkFilePath + L" ûũ ������ " + m_RelativeFilePath + L" ��� ���� ���� ����")
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

	// ���Ͽ� �±� ���
	MK_CHECK(chunkFileInterface.Write(tagBlock, MkArraySection(0)) > 0, m_RelativeFilePath + L" ��ŷ�� ûũ�� �±� ��� ����")
		return false;

	// ���Ͽ� ��� ���
	MK_CHECK(chunkFileInterface.Write(headerBlock, MkArraySection(0)) > 0, m_RelativeFilePath + L" ��ŷ�� ûũ�� ��� ��� ����")
		return false;

	// ���Ͽ� ������ ���
	MK_CHECK(chunkFileInterface.Write(dataBlock, MkArraySection(0)) > 0, m_RelativeFilePath + L" ��ŷ�� ûũ�� ������ ��� ����")
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
