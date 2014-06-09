
#include "MkCore_MkCheck.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkFilePathListContainer.h"
#include "MkCore_MkFileSystem.h"

//------------------------------------------------------------------------------------------------//

void MkFileSystem::SetChunkSizeGuideline(unsigned int chunkSizeInMB)
{
	chunkSizeInMB = Clamp<unsigned int>(chunkSizeInMB, 1, 4095);
	m_ChunkSizeGuideline = 1024 * 1024 * chunkSizeInMB; // mega byte -> byte
}

void MkFileSystem::SetPercentageForCompressing(unsigned int percentageForCompressing)
{
	percentageForCompressing = Clamp<unsigned int>(percentageForCompressing, 0, 100);
	m_PercentageForCompressing = percentageForCompressing;
}

void MkFileSystem::SetChunkFileNamingRule(const MkStr& prefix, const MkStr& extension)
{
	m_ChunkFilePrefix = prefix;
	m_ChunkFileExtension = extension;
	if (m_ChunkFilePrefix.Empty())
	{
		m_ChunkFilePrefix = L"MK_PACK_";
	}
	if (m_ChunkFileExtension.Empty())
	{
		m_ChunkFilePrefix = L"mcf";
	}
}

//------------------------------------------------------------------------------------------------//

bool MkFileSystem::SetUpFromOriginalDirectory
(const MkPathName& absolutePathOfBaseDirectory, const MkArray<MkPathName>& nameFilter, const MkArray<MkPathName>& extensionFilter,
 const MkArray<MkStr>& prefixFilter, const MkArray<MkPathName>& exceptionFilter, const MkPathName& workingDirectoryPath)
{
	// ��� ��ȿ�� �˻�
	MK_CHECK(absolutePathOfBaseDirectory.IsAbsolutePath() && absolutePathOfBaseDirectory.IsDirectoryPath() && absolutePathOfBaseDirectory.CheckAvailable(), absolutePathOfBaseDirectory + L" ��� ����")
		return false;

	MkPathName absBase = absolutePathOfBaseDirectory;
	absBase.CheckAndAddBackslash();

	m_AbsoluteWorkingDirectoryPath.ConvertToRootBasisAbsolutePath(workingDirectoryPath);
	MK_CHECK(m_AbsoluteWorkingDirectoryPath.IsDirectoryPath(), m_AbsoluteWorkingDirectoryPath + L" ��� ����")
		return false;

	Clear();

	// ���� ûũ ���ϵ��� ���� �ִٸ� ����
	if (m_AbsoluteWorkingDirectoryPath.CheckAvailable())
	{
		MkMap<unsigned int, MkPathName> filePathTable;
		m_AbsoluteWorkingDirectoryPath.GetIndexFormFileList(filePathTable, m_ChunkFilePrefix, m_ChunkFileExtension, true);
		MkMapLooper<unsigned int, MkPathName> looper(filePathTable);
		MK_STL_LOOP(looper)
		{
			if (!looper.GetCurrentField().DeleteCurrentFile())
				return false;
		}
	}
	else
	{
		if (!m_AbsoluteWorkingDirectoryPath.MakeDirectoryPath()) // ��� ��� ����
			return false;
	}

	// ���͸� �� ��� ���� ���� ũ�� Ž��
	unsigned int totalFileCount = absBase.GetFileCount(true);
	if (totalFileCount == 0)
		return true;

	// ���� ��� ����Ʈ
	MkFilePathListContainer filePathListContainer(absBase);

	// ��� ���� Ž��. �� ���� ���� ��� ������ ������ ��θ��� ������ �����Ƿ� �ߺ� ���� ����
	MkArray<MkPathName>& relativeFilePathList = filePathListContainer.GetRelativeFilePathList();
	relativeFilePathList.Reserve(totalFileCount);
	absBase.GetBlackFileList(relativeFilePathList, nameFilter, extensionFilter, prefixFilter, exceptionFilter, true, false);
	if (relativeFilePathList.Empty())
		return true;

	// ûũ ����
	unsigned int chunkIndex = 0;
	while (!filePathListContainer.Empty())
	{
		if (!_CreateNewChunkFromOriginalFiles(chunkIndex, filePathListContainer))
			return false;

		++chunkIndex;
	}

	m_SearchTable.Rehash();
	return true;
}

bool MkFileSystem::SetUpFromChunkFiles(const MkPathName& workingDirectoryPath)
{
	m_AbsoluteWorkingDirectoryPath.ConvertToRootBasisAbsolutePath(workingDirectoryPath);
	MK_CHECK(m_AbsoluteWorkingDirectoryPath.IsDirectoryPath() && m_AbsoluteWorkingDirectoryPath.CheckAvailable(), m_AbsoluteWorkingDirectoryPath + L" ��� ����")
		return false;

	Clear();

	// ���� ûũ ���ϵ� �˻�
	MkMap<unsigned int, MkPathName> filePathTable;
	m_AbsoluteWorkingDirectoryPath.GetIndexFormFileList(filePathTable, m_ChunkFilePrefix, m_ChunkFileExtension, true);
	if (filePathTable.Empty())
		return true;

	MkMapLooper<unsigned int, MkPathName> looper(filePathTable);
	MK_STL_LOOP(looper)
	{
		unsigned int chunkIndex = looper.GetCurrentKey();

		// ûũ ���� ��θ� ����
		MkPathName chunkFilePath;
		_GenerateChunkFilePath(chunkFilePath, chunkIndex);

		// ûũ�� ������ ����
		MkArray<MkPathName> memberFilePathList;
		if (!m_ChunkTable.Create(chunkIndex).SetUpFromChunkFile(chunkFilePath, memberFilePathList))
			return false;

		// ûũ�� ��ϵ� ���� ��ε��� key�� �˻� ���̺� ����
		if (chunkFilePath.CheckAvailable())
		{
			_RegisterPathListToSearchTable(memberFilePathList, chunkIndex, 0);
		}
		// ���� ûũ������ ���ٸ� ��� �Ҽ� ����� disable ���¸� �ǹ�. ûũ ����
		else
		{
			m_ChunkTable.Erase(chunkIndex);
		}
	}

	m_SearchTable.Rehash();
	return true;
}

//------------------------------------------------------------------------------------------------//

bool MkFileSystem::GetBlankPercentage(MkMap<MkStr, unsigned int>& buffer) const
{
	buffer.Clear();

	MkConstMapLooper<unsigned int, MkFileChunk> looper(m_ChunkTable);
	MK_STL_LOOP(looper)
	{
		const MkFileChunk& currChunk = looper.GetCurrentField();
		buffer.Create(currChunk.GetAbsoluteChunkFilePath().GetFileName(), currChunk.GetBlankPercentage());
	}

	return (!buffer.Empty());
}

bool MkFileSystem::CheckAvailable(const MkPathName& filePath) const
{
	// ����η� ��ȯ. root directory ���� ����ΰ� �� �� ���� ������(��ġ�� �ٸ�)�� ����
	MkPathName relativeFilePath = filePath;
	if (!relativeFilePath.ConvertToRootBasisRelativePath())
		return false;

	return m_SearchTable.Exist(relativeFilePath);
}

bool MkFileSystem::GetFileInfo(const MkPathName& filePath, unsigned int& originalSize, unsigned int& dataSize, unsigned int& writtenTime) const
{
	FileBlockIndex fbi;
	if (!_GetFileBlockIndex(filePath, fbi))
		return false;

	return m_ChunkTable[fbi.chunkIndex].GetBlockInfo(fbi.blockIndex, originalSize, dataSize, writtenTime);
}

bool MkFileSystem::GetOriginalFileData(const MkPathName& filePath, MkByteArray& buffer) const
{
	FileBlockIndex fbi;
	if (!_GetFileBlockIndex(filePath, fbi))
		return false;

	return m_ChunkTable[fbi.chunkIndex].GetOriginalFileData(fbi.blockIndex, buffer);
}

bool MkFileSystem::ExtractAvailableFile(const MkPathName& filePath, const MkPathName& destinationDirectoryPath) const
{
	FileBlockIndex fbi;
	if (!_GetFileBlockIndex(filePath, fbi))
		return false;

	MkPathName dest;
	dest.ConvertToRootBasisAbsolutePath(destinationDirectoryPath);
	return m_ChunkTable[fbi.chunkIndex].ExtractAvailableFile(fbi.blockIndex, dest);
}

bool MkFileSystem::ExtractAllAvailableFiles(const MkPathName& destinationDirectoryPath) const
{
	MkPathName dest;
	dest.ConvertToRootBasisAbsolutePath(destinationDirectoryPath);

	MkConstMapLooper<unsigned int, MkFileChunk> looper(m_ChunkTable);
	MK_STL_LOOP(looper)
	{
		if (!looper.GetCurrentField().ExtractAllAvailableFiles(dest))
			return false;
	}
	return true;
}

void MkFileSystem::ExportFileStructure(MkDataNode& node) const
{
	MkConstHashMapLooper<MkHashStr, FileBlockIndex> looper(m_SearchTable);
	MK_STL_LOOP(looper)
	{
		_BuildStructureAndCountFiles(looper.GetCurrentKey(), node);
	}

	looper.SetUp(m_SearchTable);
	MK_STL_LOOP(looper)
	{
		_FillFilesToNode(looper.GetCurrentKey(), node);
	}
}

//------------------------------------------------------------------------------------------------//

bool MkFileSystem::RemoveFile(const MkPathName& filePath)
{
	MkPathName relativeFilePath = filePath;
	if (!relativeFilePath.ConvertToRootBasisRelativePath())
		return false;

	if (!m_SearchTable.Exist(relativeFilePath))
		return false;

	const FileBlockIndex& fbi = m_SearchTable[relativeFilePath];
	bool ok = m_ChunkTable[fbi.chunkIndex].RemoveFile(fbi.blockIndex);
	if (ok)
	{
		m_SearchTable.Erase(relativeFilePath);
	}
	return ok;
}

bool MkFileSystem::UpdateOriginalDirectory(const MkPathName& absolutePathOfBaseDirectory)
{
	// ��� ��ȿ�� �˻�
	MK_CHECK(!m_AbsoluteWorkingDirectoryPath.Empty(), L"MkFileSystem �ʱ�ȭ ����")
		return false;

	MK_CHECK(absolutePathOfBaseDirectory.IsAbsolutePath() && absolutePathOfBaseDirectory.IsDirectoryPath(), absolutePathOfBaseDirectory + L" ��� ����")
		return false;

	// ��� ��ΰ� �������� ������ ���� ������ ���ٰ� ����
	if (!absolutePathOfBaseDirectory.CheckAvailable())
		return true;

	MkPathName absBase = absolutePathOfBaseDirectory;
	absBase.CheckAndAddBackslash();

	// ���͸� �� ��� ���� ���� ũ�� Ž��
	unsigned int totalFileCount = absBase.GetFileCount(true);
	if (totalFileCount == 0)
		return true;

	// ���� ��� ����Ʈ
	MkFilePathListContainer filePathListContainer(absBase);

	// ��� ���� Ž��
	MkArray<MkPathName>& relativeFilePathList = filePathListContainer.GetRelativeFilePathList();
	relativeFilePathList.Reserve(totalFileCount);
	absBase.GetFileList(relativeFilePathList, true, true);
	unsigned int fileCount = relativeFilePathList.GetSize();
	if (fileCount == 0)
		return true;

	// ��� ������ �̹� �����ϰ� ������ ����
	MK_INDEXING_LOOP(relativeFilePathList, i)
	{
		const MkPathName& relativeFilePath = relativeFilePathList[i];
		if (m_SearchTable.Exist(relativeFilePath))
		{
			const FileBlockIndex& fbi = m_SearchTable[relativeFilePath];
			MK_CHECK(m_ChunkTable[fbi.chunkIndex].RemoveFile(fbi.blockIndex), relativeFilePath + L" ��� ���� �� ���� �߻�")
				return false;
			m_SearchTable.Erase(relativeFilePath);
		}
	}

	// ���� ũ�Ⱑ 0�� ��� ����
	// MkArray<> Ư���� �߰� ������ ȿ���������� ������ ���� ���� �������� ������ ����ϰ� �߻��ϴ� ���� �ƴϹǷ� ����
	MkArray<unsigned int> emptyFileList(fileCount);
	for (unsigned int i=0; i<fileCount; ++i)
	{
		if (filePathListContainer.GetAbsoluteOriginalFilePath(i).GetFileSize() == 0)
		{
			emptyFileList.PushBack(i);
		}
	}
	relativeFilePathList.Erase(emptyFileList);
	emptyFileList.Clear();

	// �����ϴ� ûũ �� ���̵���κ��� ũ�Ⱑ ���� ûũ�� �˻�
	MkArray<unsigned int> availableChunks(m_ChunkTable.GetSize());
	MkMapLooper<unsigned int, MkFileChunk> looper(m_ChunkTable);
	MK_STL_LOOP(looper)
	{
		if (looper.GetCurrentField().GetChunkSize() < m_ChunkSizeGuideline)
		{
			availableChunks.PushBack(looper.GetCurrentKey());
		}
	}

	// ��� ûũ�鿡 ���� �߰�
	MK_INDEXING_LOOP(availableChunks, i)
	{
		if (!filePathListContainer.Empty())
		{
			unsigned int chunkIndex = availableChunks[i];
			MkFileChunk& currChunk = m_ChunkTable[chunkIndex];
			unsigned int blockOffset = currChunk.GetBlockCount();

			// ���� ûũ�� ���̵������ ����ϴ� ��ŭ ���� ������
			MkArray<MkPathName> memberFilePathList;
			if (!currChunk.AttachOriginalFiles(L"", filePathListContainer, memberFilePathList, m_ChunkSizeGuideline, m_PercentageForCompressing, false))
				return false;

			// ûũ�� ��ϵ� ���� ��ε��� key�� �˻� ���̺� ����
			_RegisterPathListToSearchTable(memberFilePathList, chunkIndex, blockOffset);
		}
	}
	availableChunks.Clear();

	// ���� ûũ�� ��ϵ��� �ʰ� ���� �ִ� ������ ���� ��� �� ûũ ������ ���
	while (!filePathListContainer.Empty())
	{
		// �߰��� �� �ε����̰ų� ���� �ε��� �˻�
		unsigned int chunkIndex = 0;
		MkMapLooper<unsigned int, MkFileChunk> chunkLooper(m_ChunkTable);
		MK_STL_LOOP(chunkLooper)
		{
			if (chunkIndex == chunkLooper.GetCurrentKey())
			{
				++chunkIndex;
			}
			else
				break;
		}

		// ûũ ����
		if (!_CreateNewChunkFromOriginalFiles(chunkIndex, filePathListContainer))
			return false;
	}

	m_SearchTable.Rehash();
	return true;
}

bool MkFileSystem::OptimizeChunks(unsigned int percentageForOptimizing)
{
	percentageForOptimizing = Clamp<unsigned int>(percentageForOptimizing, 0, 100);

	MkArray<unsigned int> removedChunk(m_ChunkTable.GetSize());

	MkMapLooper<unsigned int, MkFileChunk> looper(m_ChunkTable);
	MK_STL_LOOP(looper)
	{
		// ûũ�� ��ȸ�ϸ� �� ���� ���� ��
		MkFileChunk& currChunk = looper.GetCurrentField();
		if (currChunk.GetBlankPercentage() > percentageForOptimizing)
		{
			MkArray<MkPathName> movedPathList;
			MkArray<unsigned int> newIndexList;
			if (!currChunk.OptimizeChunk(movedPathList, newIndexList)) // ����ȭ ����
				return false;

			// ���� ���� �Ҽ� ûũ�� �״���̰� block index�� ����
			if (currChunk.GetAbsoluteChunkFilePath().CheckAvailable())
			{
				MK_INDEXING_LOOP(movedPathList, i)
				{
					m_SearchTable[movedPathList[i]].blockIndex = newIndexList[i];
				}
			}
			// ���� ûũ������ ���ٸ� ��� �Ҽ� ����� disable ���¸� �ǹ�. ������ ���� ûũ �ε��� ����
			else
			{
				removedChunk.PushBack(looper.GetCurrentKey());
			}
		}
	}

	// �� ûũ ����
	MK_INDEXING_LOOP(removedChunk, i)
	{
		m_ChunkTable.Erase(removedChunk[i]);
	}
	return true;
}

void MkFileSystem::Clear(void)
{
	m_ChunkTable.Clear();
	m_SearchTable.Clear();
}

MkFileSystem::MkFileSystem()
{
	SetChunkSizeGuideline();
	SetPercentageForCompressing();
	SetChunkFileNamingRule();
}

//------------------------------------------------------------------------------------------------//

void MkFileSystem::_GenerateChunkFilePath(MkPathName& chunkFilePath, unsigned int index) const
{
	chunkFilePath = m_AbsoluteWorkingDirectoryPath;
	MkStr indexStr = index;
	chunkFilePath.Reserve(m_AbsoluteWorkingDirectoryPath.GetSize() + m_ChunkFilePrefix.GetSize() + indexStr.GetSize() + 1 + m_ChunkFileExtension.GetSize());
	chunkFilePath += m_ChunkFilePrefix;
	chunkFilePath += indexStr;
	chunkFilePath += L".";
	chunkFilePath += m_ChunkFileExtension;
}

bool MkFileSystem::_GetFileBlockIndex(const MkPathName& filePath, FileBlockIndex& fbi) const
{
	// ����η� ��ȯ. root directory ���� ����ΰ� �� �� ���� ������(��ġ�� �ٸ�)�� ����
	MkPathName relativeFilePath = filePath;
	if (!relativeFilePath.ConvertToRootBasisRelativePath())
		return false;

	if (!m_SearchTable.Exist(relativeFilePath))
		return false;

	fbi = m_SearchTable[relativeFilePath];
	return true;
}

bool MkFileSystem::_CreateNewChunkFromOriginalFiles(unsigned int chunkIndex, MkFilePathListContainer& filePathListContainer)
{
	// ûũ ���� ��θ� ����
	MkPathName chunkFilePath;
	_GenerateChunkFilePath(chunkFilePath, chunkIndex);

	if (MKDEF_SHOW_FILE_PACKING_INFO)
	{
		MK_DEV_PANEL.MsgToLog(L"* ûũ ���� ���� : " + chunkFilePath.GetFileName());
	}

	// ûũ�� ������ ����, ���Ϸ� ���
	MkArray<MkPathName> memberFilePathList;
	MkFileChunk& currChunk = m_ChunkTable.Create(chunkIndex);
	if (!currChunk.AttachOriginalFiles(chunkFilePath, filePathListContainer, memberFilePathList, m_ChunkSizeGuideline, m_PercentageForCompressing, true))
		return false;

	// ûũ�� ��ϵ� ���� ��ε��� key�� �˻� ���̺� ����
	_RegisterPathListToSearchTable(memberFilePathList, chunkIndex, 0);
	return true;
}

void MkFileSystem::_RegisterPathListToSearchTable(const MkArray<MkPathName>& memberFilePathList, unsigned int chunkIndex, unsigned int blockOffset)
{
	MK_INDEXING_LOOP(memberFilePathList, i)
	{
		const MkPathName& currPath = memberFilePathList[i];
		if (!currPath.Empty())
		{
			FileBlockIndex fbi;
			fbi.chunkIndex = chunkIndex;
			fbi.blockIndex = blockOffset + i;
			m_SearchTable.Create(currPath, fbi);
		}
	}
}

const static MkHashStr FILES = L"Files";
const static MkHashStr COUNT = L"Count";

void MkFileSystem::_BuildStructureAndCountFiles(const MkHashStr& key, MkDataNode& node) const
{
	MkStr path = key.GetString();
	MkArray<MkStr> token;
	if (path.Tokenize(token, L"\\") > 0)
	{
		_BuildStructureAndCountFiles(token, 0, token.GetSize() - 1, &node);
	}
}

void MkFileSystem::_BuildStructureAndCountFiles(const MkArray<MkStr>& token, unsigned int currIndex, unsigned int indexOfFile, MkDataNode* node) const
{
	// directory
	if (currIndex < indexOfFile)
	{
		MkHashStr dirName = token[currIndex];
		MkDataNode* nextNode = node->ChildExist(dirName) ? node->GetChildNode(dirName) : node->CreateChildNode(dirName);
		_BuildStructureAndCountFiles(token, currIndex + 1, indexOfFile, nextNode);
	}
	// file
	else // (currIndex == indexOfFile)
	{
		if (node->IsValidKey(COUNT))
		{
			unsigned int cnt;
			node->GetData(COUNT, cnt, 0); // ++cnt
			node->SetData(COUNT, cnt + 1, 0);
		}
		else
		{
			unsigned int cnt = 1;
			node->CreateUnit(COUNT, cnt);
		}
	}
}

void MkFileSystem::_FillFilesToNode(const MkHashStr& key, MkDataNode& node) const
{
	MkStr path = key.GetString();
	MkArray<MkStr> token;
	if (path.Tokenize(token, L"\\") > 0)
	{
		_FillFilesToNode(token, 0, token.GetSize() - 1, &node);
	}
}

void MkFileSystem::_FillFilesToNode(const MkArray<MkStr>& token, unsigned int currIndex, unsigned int indexOfFile, MkDataNode* node) const
{
	// directory
	if (currIndex < indexOfFile)
	{
		_FillFilesToNode(token, currIndex + 1, indexOfFile, node->GetChildNode(token[currIndex]));
	}
	// file
	else // (currIndex == indexOfFile)
	{
		if (node->IsValidKey(FILES))
		{
			unsigned int pos;
			node->GetData(COUNT, pos, 0);
			node->SetData(FILES, token[currIndex], pos);

			++pos;
			if (pos == node->GetDataSize(FILES))
			{
				node->RemoveUnit(COUNT);
			}
			else
			{
				node->SetData(COUNT, pos, 0);
			}
		}
		else
		{
			unsigned int cnt;
			node->GetData(COUNT, cnt, 0);
			node->CreateUnit(FILES, ePDT_Str, cnt);
			node->SetData(FILES, token[currIndex], 0);
			cnt = 1;
			node->SetData(COUNT, cnt, 0);
		}
	}
}

//------------------------------------------------------------------------------------------------//
