
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
	MK_CHECK(startCount > 0, L"���� ��� ����Ʈ�� ��� " + m_AbsoluteChunkFilePath + L" ûũ ���� ���� �Ұ�")
		return false;

	if (createNewChunk)
	{
		MK_CHECK(!chunkFilePath.IsDirectoryPath(), chunkFilePath + L" ûũ ���� ��� ����")
			return false;

		Clear();

		// �� ûũ ���� �̸�
		m_AbsoluteChunkFilePath.ConvertToRootBasisAbsolutePath(chunkFilePath);
	}

	// ũ�� ����
	m_BlockList.Reserve(m_BlockList.GetSize() + startCount);
	memberFilePathList.Reserve(startCount);

	// �������̽� ����. createNewChunk�� ���� ûũ ���� ����
	MkInterfaceForFileWriting chunkFileInterface;
	if (!chunkFileInterface.SetUp(m_AbsoluteChunkFilePath, createNewChunk))
		return false;

	// �� ûũ ������ ��� ��� ī��Ʈ�� ����ϱ� ���� �±� ���� Ȯ��
	if (createNewChunk)
	{
		m_ChunkSize = sizeof(unsigned int);
		_AddCountTag(chunkFileInterface, m_ChunkSize);
	}

	// �ڿ������� ������ ��ȸ
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

	// ��� ī��Ʈ ����
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

	// ûũ�� ������ ���ϵ��� ����Ʈ���� ����
	filePathListContainer.PopBack(memberFilePathList.GetSize());
	return true;
}

bool MkFileChunk::SetUpFromChunkFile(const MkPathName& chunkFilePath, MkArray<MkPathName>& memberFilePathList)
{
	MK_CHECK((!chunkFilePath.Empty()) && (!chunkFilePath.IsDirectoryPath()), L" ûũ ���� ��� ���� : " + chunkFilePath)
		return false;

	Clear();
	memberFilePathList.Clear();

	// ûũ ���� �̸�
	m_AbsoluteChunkFilePath.ConvertToRootBasisAbsolutePath(chunkFilePath);

	// ��� ī��Ʈ Ž��
	MkInterfaceForFileTag<unsigned int> ftInterface;
	if (!ftInterface.SetUp(m_AbsoluteChunkFilePath))
		return false;

	unsigned int blockCount = ftInterface.GetTag(0);
	ftInterface.Clear();
	MK_CHECK(blockCount != MKDEF_ARRAY_ERROR, m_AbsoluteChunkFilePath + L" ûũ ���� �±� �б� ����")
		return false;

	MK_CHECK(blockCount > 0, m_AbsoluteChunkFilePath + L" ûũ ������ 0���� ���� ��� ����")
		return false;

	// ��� ũ�� ����
	m_BlockList.Reserve(blockCount);

	// ��ȯ�� �迭 ũ�� ����
	memberFilePathList.Reserve(blockCount);

	// ûũ ���� ����
	MkInterfaceForFileReading frInterface;
	if (!frInterface.SetUp(m_AbsoluteChunkFilePath))
		return false;

	// ûũ ���Ͽ����� Ž�� ��ġ. ���� �±� ũ�⸸ŭ ��ŵ
	m_ChunkSize = sizeof(unsigned int);
	
	// ûũ ������ ��ȸ�ϸ� ��� ����
	unsigned int lastBlockIndex = blockCount - 1;
	for (unsigned int i=0; i<blockCount; ++i)
	{
		// �������̽��� ��ġ �̵�
		frInterface.SetCurrentPosition(m_ChunkSize);

		// ��� ����
		MkFileBlock& currBlock = m_BlockList.PushBack();
		if (!currBlock.SetUpFromChunkFile(frInterface))
			return false;

		unsigned int blockSize = currBlock.GetBlockSize();
		m_ChunkSize += blockSize;

		// ������ ����� ��� ûũ ũ�� ����
		// ���� ���� ûũ ���� ������ ������ ��� ������ ��� �� ������ �߻��ߴٸ� ����� ũ�⺸�� �� ���� ũ�Ⱑ �����Ƿ�
		// ������ ��� ���� �� ûũ �籸��
		// (NOTE) ��� ��� �� ������ ���� ���� ��¿ �� ����
		if ((i == lastBlockIndex) && (m_ChunkSize > frInterface.GetFileSize()))
		{
			m_ChunkSize -= blockSize;
			m_BlockList.PopBack();

			MkArray<MkPathName> movedPathList;
			MkArray<unsigned int> newIndexList;
			if (!_RebuildChunk(movedPathList, newIndexList))
				return false;
		}
		// ������ ���� �������� ���ΰ� ����
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

		// chunk & block ���� ���
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
	MK_CHECK(destinationDirectoryPath.IsDirectoryPath(), destinationDirectoryPath + L"�� ���丮 ��ΰ� �ƴ�")
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
	MK_CHECK(destinationDirectoryPath.IsDirectoryPath(), destinationDirectoryPath + L"�� ���丮 ��ΰ� �ƴ�")
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
	// disable ��� ����
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
	MK_CHECK(m_AbsoluteChunkFilePath.CheckAvailable(), m_AbsoluteChunkFilePath + L" ûũ ������ ��� �Ұ�")
		return false;

	MK_CHECK(m_BlockList.IsValidIndex(blockIndex), MkStr(blockIndex) + L"�� ����� " + m_AbsoluteChunkFilePath + L" ûũ ���Ͽ� ����")
		return false;

	return true;
}

bool MkFileChunk::_RebuildChunk(MkArray<MkPathName>& movedPathList, MkArray<unsigned int>& newIndexList)
{
	MK_CHECK(m_AbsoluteChunkFilePath.CheckAvailable(), m_AbsoluteChunkFilePath + L" ûũ ������ ��� �Ұ�")
		return false;

	unsigned int blockCount = m_BlockList.GetSize();

	// ���� ����� ����ų� ��ü�� ��� disable�̸� ûũ ���� ����
	if ((blockCount == 0) || (m_AvailableBlockCount == 0))
	{
		return m_AbsoluteChunkFilePath.DeleteCurrentFile();
	}

	// �ε����� ���� ��� ��(m_FirstDisableBlockIndex ������ ��� ��)
	unsigned int movedBlockCount = blockCount - m_FirstDisableBlockIndex - 1;

	// �� ��� ����Ʈ
	MkArray<MkFileBlock> newBlockList(m_AvailableBlockCount);

	// �ε��� ������ �߻��Ѵٸ� �迭 ���� Ȯ��
	if (movedBlockCount > 0)
	{
		movedPathList.Reserve(movedBlockCount);
		newIndexList.Reserve(movedBlockCount);
	}

	// �ӽ� ûũ ���� �̸�
	MkPathName temporaryFilePath;
	MkStr tName, tExtension;
	m_AbsoluteChunkFilePath.SplitPath(temporaryFilePath, tName, tExtension);
	temporaryFilePath.Reserve(temporaryFilePath.GetSize() + tName.GetSize() + 5);
	temporaryFilePath += L"_";
	temporaryFilePath += tName;
	temporaryFilePath += L".tmp";

	// ���� ûũ���� ����
	MkInterfaceForFileReading oldInterface;
	if (!oldInterface.SetUp(m_AbsoluteChunkFilePath))
		return false;

	// �ӽ� ûũ���� ����
	MkInterfaceForFileWriting tempInterface;
	if (!tempInterface.SetUp(temporaryFilePath))
		return false;

	// ��� ī��Ʈ�� ����ϱ� ���� �±� ���� Ȯ��
	_AddCountTag(tempInterface, m_AvailableBlockCount);

	// ����� ��ȸ�ϸ� ����ȭ
	for (unsigned int i=0; i<blockCount; ++i)
	{
		const MkFileBlock& currBlock = m_BlockList[i];

		// ��ȿ�� �˻�
		if (currBlock.Available())
		{
			unsigned int newIndex = newBlockList.GetSize();
			MkFileBlock& newBlock = newBlockList.PushBack(); // �� ����� ����
			newBlock = currBlock; // ���� ��� ����
			if (!newBlock.CopyBlockToNewChunk(oldInterface, tempInterface)) // �� ��Ͽ��� �� ûũ�� ī��(�� ûũ������ ��ġ�� ���ŵ�)
				return false;

			// �ε����� ���ϴ� ����̶�� ��� ����Ʈ�� ���
			if (blockCount > m_FirstDisableBlockIndex)
			{
				movedPathList.PushBack(newBlock.GetRelativeFilePath());
				newIndexList.PushBack(newIndex);
			}
		}
	}

	tempInterface.Clear();
	oldInterface.Clear();

	// ���� ûũ ����
	MK_CHECK(m_AbsoluteChunkFilePath.DeleteCurrentFile(), L"����ȭ �� " + m_AbsoluteChunkFilePath + L" ������ �������� ����")
	{
		temporaryFilePath.DeleteCurrentFile();
		return false;
	}

	// �ӽ� ûũ���� �̸� ����
	// (NOTE) ���� ������ ��� ����, �ӽ� ûũ�� ��� ���� ���°� ��
	MK_CHECK(temporaryFilePath.RenameCurrentFile(m_AbsoluteChunkFilePath.GetFileName()), temporaryFilePath + L" �ӽ� ûũ ���� �̸� ���� ����")
		return false;

	// �� ��� ����Ʈ�� ��ü
	m_BlockList = newBlockList;

	// ���� ����
	m_ChunkSize -= m_BlankSize;
	m_BlankSize = 0;
	m_FirstDisableBlockIndex = MKDEF_ARRAY_ERROR;

	return true;
}

//------------------------------------------------------------------------------------------------//
