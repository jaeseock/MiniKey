
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkFilePathListContainer.h"
#include "MkCore_MkFileSystem.h"


const MkPathName MkFileSystem::DefaultSettingFileName(L"PackInfo.mmd");

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
}

void MkFileSystem::SetFilterForOriginalFiles
(const MkArray<MkPathName>* exceptionFilter, const MkArray<MkPathName>* nameFilter, const MkArray<MkPathName>* extensionFilter, const MkArray<MkStr>* prefixFilter)
{
	m_PathFilter.Clear();
	m_PathFilter.SetUp(exceptionFilter, nameFilter, extensionFilter, prefixFilter);
}

void MkFileSystem::SetSystemSetting(const MkDataNode& node)
{
	// chunk size
	unsigned int chunkSizeInMB;
	if (node.GetData(L"ChunkSizeInMB", chunkSizeInMB, 0))
	{
		SetChunkSizeGuideline(chunkSizeInMB);
	}

	// comp limit
	unsigned int percentageForCompressing;
	if (node.GetData(L"PercentageForCompressing", percentageForCompressing, 0))
	{
		SetPercentageForCompressing(percentageForCompressing);
	}

	// name rule
	MkStr prefix, extension;
	if (node.GetData(L"Prefix", prefix, 0) && node.GetData(L"Extension", extension, 0))
	{
		SetChunkFileNamingRule(prefix, extension);
	}

	// filtering
	MkArray<MkStr> filterBuffer;
	MkArray<MkPathName> exceptionFilter;
	if (node.GetData(L"ExceptionFilter", filterBuffer))
	{
		exceptionFilter.Reserve(filterBuffer.GetSize());
		MK_INDEXING_LOOP(filterBuffer, i)
		{
			exceptionFilter.PushBack(filterBuffer[i]);
		}
		filterBuffer.Clear();
	}

	MkArray<MkPathName> nameFilter;
	if (node.GetData(L"NameFilter", filterBuffer))
	{
		nameFilter.Reserve(filterBuffer.GetSize());
		MK_INDEXING_LOOP(filterBuffer, i)
		{
			MkStr& tmpName = filterBuffer[i];
			tmpName.ReplaceKeyword(L"/", L"\\"); // ���丮���� ���ڿ��� ��� �� ��� Ż�⹮��(\")�� ���� �Ľ��� �����ϹǷ� ��� /�� ���
			nameFilter.PushBack(tmpName);
		}
		filterBuffer.Clear();
	}

	MkArray<MkPathName> extensionFilter;
	if (node.GetData(L"ExtensionFilter", filterBuffer))
	{
		extensionFilter.Reserve(filterBuffer.GetSize());
		MK_INDEXING_LOOP(filterBuffer, i)
		{
			extensionFilter.PushBack(filterBuffer[i]);
		}
		filterBuffer.Clear();
	}

	node.GetData(L"PrefixFilter", filterBuffer);

	SetFilterForOriginalFiles(&exceptionFilter, &nameFilter, &extensionFilter, &filterBuffer);
}

bool MkFileSystem::SetSystemSetting(const MkPathName& settingFilePath)
{
	MkDataNode node;
	bool ok = node.Load(settingFilePath);
	if (ok)
	{
		SetSystemSetting(node);
	}
	return ok;
}

bool MkFileSystem::SetSystemSetting(void)
{
	MkPathName packInfoPath;
	packInfoPath.ConvertToRootBasisAbsolutePath(DefaultSettingFileName);
	return packInfoPath.CheckAvailable() ? SetSystemSetting(DefaultSettingFileName) : false;
}

//------------------------------------------------------------------------------------------------//

bool MkFileSystem::SetUpFromOriginalDirectory(const MkPathName& absolutePathOfBaseDirectory, const MkPathName& workingDirectoryPath)
{
	// ��� ��ȿ�� �˻�
	MK_CHECK(absolutePathOfBaseDirectory.IsAbsolutePath() && absolutePathOfBaseDirectory.IsDirectoryPath() && absolutePathOfBaseDirectory.CheckAvailable(), absolutePathOfBaseDirectory + L" ��� ����")
		return false;

	MkPathName absBase = absolutePathOfBaseDirectory;
	absBase.CheckAndAddBackslash();

	m_AbsoluteWorkingDirectoryPath.ConvertToRootBasisAbsolutePath(workingDirectoryPath);
	MK_CHECK(m_AbsoluteWorkingDirectoryPath.IsDirectoryPath(), m_AbsoluteWorkingDirectoryPath + L" ��� ����")
		return false;

	m_ChunkTable.Clear();
	m_SearchTable.Clear();

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
	absBase.GetFileList(relativeFilePathList, true, false, m_PathFilter.CheckAvailable() ? &m_PathFilter : NULL);
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

	m_ChunkTable.Clear();
	m_SearchTable.Clear();

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

bool MkFileSystem::GetBlankChunks(unsigned int percentageForOptimizing, MkArray<unsigned int>& chunkIndice) const
{
	if (m_ChunkTable.Empty())
		return false;

	percentageForOptimizing = Clamp<unsigned int>(percentageForOptimizing, 0, 100);

	chunkIndice.Clear();
	chunkIndice.Reserve(m_ChunkTable.GetSize());

	MkConstMapLooper<unsigned int, MkFileChunk> looper(m_ChunkTable);
	MK_STL_LOOP(looper)
	{
		const MkFileChunk& currChunk = looper.GetCurrentField();
		if (currChunk.GetBlankPercentage() > percentageForOptimizing)
		{
			chunkIndice.PushBack(looper.GetCurrentKey());
		}
	}
	return (!chunkIndice.Empty());
}

bool MkFileSystem::CheckAvailable(const MkPathName& filePath) const
{
	// ����η� ��ȯ. root directory ���� ����ΰ� �� �� ���� ������(��ġ�� �ٸ�)�� ����
	MkPathName relativeFilePath = filePath;
	if (!relativeFilePath.ConvertToRootBasisRelativePath())
		return false;

	relativeFilePath.ToLower();
	return m_SearchTable.Exist(relativeFilePath);
}

bool MkFileSystem::CheckFileFilter(const MkPathName& fileOrDirPath) const
{
	// ����η� ��ȯ. root directory ���� ����ΰ� �� �� ���� ������(��ġ�� �ٸ�)�� ����
	MkPathName relativeFilePath = fileOrDirPath;
	if (!relativeFilePath.ConvertToRootBasisRelativePath())
		return false;

	if (m_PathFilter.CheckAvailable()) // ���Ͱ� �����ϸ�
	{
		return relativeFilePath.IsDirectoryPath() ? m_PathFilter.CheckAvailableDirectory(relativeFilePath) : m_PathFilter.CheckAvailableFile(relativeFilePath);
	}
	return true;
}

bool MkFileSystem::GetFileInfo(const MkPathName& filePath, unsigned int& originalSize, unsigned int& dataSize, unsigned int& writtenTime) const
{
	FileBlockIndex fbi;
	if (!_GetFileBlockIndex(filePath, fbi))
		return false;

	return m_ChunkTable[fbi.chunkIndex].GetBlockInfo(fbi.blockIndex, originalSize, dataSize, writtenTime);
}

int MkFileSystem::GetFileDifference(const MkPathName& filePath, unsigned int originalSize, unsigned int writtenTime) const
{
	FileBlockIndex fbi;
	if (_GetFileBlockIndex(filePath, fbi))
	{
		unsigned int os, ds, wt;
		if (m_ChunkTable[fbi.chunkIndex].GetBlockInfo(fbi.blockIndex, os, ds, wt))
			return ((os != originalSize) || (wt != writtenTime)) ? 1 : 0;
	}
	return -1;
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

void MkFileSystem::PrintSystemInfoToDevPanel(bool printFilterInfo) const
{
	if ((MkDevPanel::InstancePtr() != NULL) && MK_DEV_PANEL.IsEnable())
	{
		MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
		MK_DEV_PANEL.MsgToLog(L"[ MkFileSystem ���� ]");
		MK_DEV_PANEL.MsgToLog(L"   ũ�� ���̵���� : " + MkStr(m_ChunkSizeGuideline / 1048576) + L" MB");
		MK_DEV_PANEL.MsgToLog(L"   ���� ��� : " + MkStr(m_PercentageForCompressing) + L" %");
		MK_DEV_PANEL.MsgToLog(L"   ûũ ���� ���� : " + m_ChunkFilePrefix + L"(N)." + m_ChunkFileExtension);
		if (GetTotalChunkCount() > 0)
		{
			MK_DEV_PANEL.MsgToLog(L"   ûũ ���� �� : " + MkStr(GetTotalChunkCount()));
			
			MkMap<MkStr, unsigned int> blankBuffer;
			if (GetBlankPercentage(blankBuffer))
			{
				MK_DEV_PANEL.MsgToLog(L"   ûũ ���ط�");

				MkMapLooper<MkStr, unsigned int> looper(blankBuffer);
				MK_STL_LOOP(looper)
				{
					MK_DEV_PANEL.MsgToLog(L"   - " + looper.GetCurrentKey() + L" : " + MkStr(looper.GetCurrentField()) + L" %");
				}
			}

			MK_DEV_PANEL.MsgToLog(L"   ��� ���� �� : " + MkStr(GetTotalFileCount()));
		}

		if (printFilterInfo)
		{
			MkArray<MkStr> buffer;
			if (m_PathFilter.GetNameFilter(buffer))
			{
				MK_DEV_PANEL.MsgToLog(L"   ����/���丮 ����");
				MK_INDEXING_LOOP(buffer, i)
				{
					MK_DEV_PANEL.MsgToLog(L"   - " + buffer[i]);
				}
				buffer.Clear();
			}

			if (m_PathFilter.GetExtensionFilter(buffer))
			{
				MK_DEV_PANEL.MsgToLog(L"   ���� Ȯ���� ����");
				MK_INDEXING_LOOP(buffer, i)
				{
					MK_DEV_PANEL.MsgToLog(L"   - " + buffer[i]);
				}
				buffer.Clear();
			}

			if (m_PathFilter.GetPrefixFilter(buffer))
			{
				MK_DEV_PANEL.MsgToLog(L"   ���� prefix ����");
				MK_INDEXING_LOOP(buffer, i)
				{
					MK_DEV_PANEL.MsgToLog(L"   - " + buffer[i]);
				}
				buffer.Clear();
			}

			if (m_PathFilter.GetExceptionFilter(buffer))
			{
				MK_DEV_PANEL.MsgToLog(L"   ���� ���͸� ����");
				MK_INDEXING_LOOP(buffer, i)
				{
					MK_DEV_PANEL.MsgToLog(L"   - " + buffer[i]);
				}
			}
		}

		MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
	}
}

void MkFileSystem::ExportSystemStructure(MkDataNode& node, bool includeChunkInfo) const
{
	MkConstHashMapLooper<MkHashStr, FileBlockIndex> looper(m_SearchTable);
	MK_STL_LOOP(looper)
	{
		const MkStr& path = looper.GetCurrentKey().GetString();
		MkArray<MkStr> token;
		if (path.Tokenize(token, L"\\") > 0)
		{
			_ExportSystemStructure(node, token, 0, m_SearchTable[looper.GetCurrentKey()], includeChunkInfo);
		}
	}
}

//------------------------------------------------------------------------------------------------//

bool MkFileSystem::RemoveFile(const MkPathName& filePath)
{
	MkPathName relativeFilePath = filePath;
	if (!relativeFilePath.ConvertToRootBasisRelativePath())
		return false;

	relativeFilePath.ToLower();
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

bool MkFileSystem::UpdateFromOriginalFile(const MkPathName& absolutePathOfBaseDirectory, const MkPathName& relativePathOfFile)
{
	// ��� ��ȿ�� �˻�
	MK_CHECK(!m_AbsoluteWorkingDirectoryPath.Empty(), L"MkFileSystem �ʱ�ȭ ����")
		return false;

	MK_CHECK(absolutePathOfBaseDirectory.IsAbsolutePath() && absolutePathOfBaseDirectory.IsDirectoryPath(), absolutePathOfBaseDirectory + L" ��� ����")
		return false;

	MkPathName absSrcFileDir = absolutePathOfBaseDirectory;
	absSrcFileDir.CheckAndAddBackslash();

	MkPathName absSrcFilePath = absSrcFileDir + relativePathOfFile;
	MK_CHECK(!absSrcFilePath.IsDirectoryPath(), absSrcFilePath + L" �� ���� ����̾�� ��")
		return false;

	MK_CHECK(absSrcFilePath.CheckAvailable(), absSrcFilePath + L" ������ �������� ����")
		return false;

	// ��� ������ �̹� �����ϰ� ������ ����
	MkPathName lowerRelFilePath = relativePathOfFile;
	lowerRelFilePath.ToLower();
	MkHashStr relativeFilePathKey = lowerRelFilePath;
	if (m_SearchTable.Exist(relativeFilePathKey))
	{
		const FileBlockIndex& fbi = m_SearchTable[relativeFilePathKey];
		MK_CHECK(m_ChunkTable[fbi.chunkIndex].RemoveFile(fbi.blockIndex), relativeFilePathKey.GetString() + L" ��� ���� �� ���� �߻�")
			return false;
		m_SearchTable.Erase(relativeFilePathKey);
	}

	// ���� ũ�Ⱑ 0�̸� ���⼭ ����
	if (absSrcFilePath.GetFileSize() == 0)
		return true;

	// ���� ��� ����Ʈ
	MkFilePathListContainer filePathListContainer(absSrcFileDir);
	filePathListContainer.GetRelativeFilePathList().PushBack(relativePathOfFile);

	// �����ϴ� ûũ �� ���̵���κ��� ũ�Ⱑ ���� ûũ �ϳ��� �˻�
	unsigned int chunkIndex = m_ChunkTable.GetSize();
	MkMapLooper<unsigned int, MkFileChunk> looper(m_ChunkTable);
	MK_STL_LOOP(looper)
	{
		if (looper.GetCurrentField().GetChunkSize() < m_ChunkSizeGuideline)
		{
			chunkIndex = looper.GetCurrentKey();
			break;
		}
	}

	// ���� Ȱ���� ûũ�� �����Ƿ� ���� ����
	if (chunkIndex == m_ChunkTable.GetSize())
	{
		if (!_UpdateFilesToNewChunk(filePathListContainer))
			return false;
	}
	// ���� ûũ�� ����
	else
	{
		MkArray<unsigned int> availableChunks;
		availableChunks.PushBack(chunkIndex);
		if (!_UpdateFilesToAvailableChunk(availableChunks, filePathListContainer))
			return false;
	}
	return true;
}

bool MkFileSystem::UpdateFromOriginalDirectory(const MkPathName& absolutePathOfBaseDirectory, bool removeOrgFilesAfterUpdating)
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
	absBase.GetFileList(relativeFilePathList, true, true, m_PathFilter.CheckAvailable() ? &m_PathFilter : NULL);
	unsigned int fileCount = relativeFilePathList.GetSize();
	if (fileCount == 0)
		return true;

	// ��� ���纻
	MkFilePathListContainer copyListContainer = filePathListContainer;

	// ��� ������ �̹� �����ϰ� ������ ����
	MK_INDEXING_LOOP(relativeFilePathList, i)
	{
		MkPathName currPath = relativeFilePathList[i];
		currPath.ToLower();
		MkHashStr relativeFilePathKey = currPath;

		if (m_SearchTable.Exist(relativeFilePathKey))
		{
			const FileBlockIndex& fbi = m_SearchTable[relativeFilePathKey];
			MK_CHECK(m_ChunkTable[fbi.chunkIndex].RemoveFile(fbi.blockIndex), relativeFilePathKey.GetString() + L" ��� ���� �� ���� �߻�")
				return false;
			m_SearchTable.Erase(relativeFilePathKey);
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
	if (!_UpdateFilesToAvailableChunk(availableChunks, filePathListContainer))
		return false;
	availableChunks.Clear();

	// ���� ûũ�� ��ϵ��� �ʰ� ���� �ִ� ������ ���� ��� �� ûũ ������ ���
	if (!_UpdateFilesToNewChunk(filePathListContainer))
		return false;
	m_SearchTable.Rehash();

	// ��� ���������� �Ϸ�Ǿ����� ���� ���� ����
	if (removeOrgFilesAfterUpdating)
	{
		for (unsigned int i=0; i<fileCount; ++i)
		{
			copyListContainer.GetAbsoluteOriginalFilePath(i).DeleteCurrentFile(); // ���� ����
		}

		copyListContainer.GetAbsolutePathOfBaseDirectory().DeleteCurrentDirectory(false); // �� ���� ����
	}
	return true;
}

bool MkFileSystem::OptimizeAllChunks(unsigned int percentageForOptimizing)
{
	MkArray<unsigned int> targetChunks;
	if (GetBlankChunks(percentageForOptimizing, targetChunks))
	{
		MK_INDEXING_LOOP(targetChunks, i)
		{
			if (!OptimizeChunk(targetChunks[i]))
				return false;
		}
	}
	return true;
}

bool MkFileSystem::OptimizeChunk(unsigned int chunkIndex)
{
	if (m_ChunkTable.Exist(chunkIndex))
	{
		MkFileChunk& currChunk = m_ChunkTable[chunkIndex];
		MkArray<MkPathName> movedPathList;
		MkArray<unsigned int> newIndexList;
		if (!currChunk.OptimizeChunk(movedPathList, newIndexList)) // ����ȭ ����
			return false;

		// ���� ���� �Ҽ� ûũ�� �״���̰� block index�� ����
		if (currChunk.GetAbsoluteChunkFilePath().CheckAvailable())
		{
			MK_INDEXING_LOOP(movedPathList, i)
			{
				MkPathName movedPath = movedPathList[i];
				movedPath.ToLower();
				m_SearchTable[movedPath].blockIndex = newIndexList[i];
			}
		}
		// ���� ûũ������ ���ٸ� ��� �Ҽ� ����� disable ���¸� �ǹ�. �� ûũ ����
		else
		{
			m_ChunkTable.Erase(chunkIndex);
		}
		return true;
	}
	return false;
}

void MkFileSystem::Clear(void)
{
	m_ChunkTable.Clear();
	m_SearchTable.Clear();
	m_PathFilter.Clear();
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

	relativeFilePath.ToLower();
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
		if (!memberFilePathList[i].Empty())
		{
			FileBlockIndex fbi;
			fbi.chunkIndex = chunkIndex;
			fbi.blockIndex = blockOffset + i;
			MkPathName currPath = memberFilePathList[i];
			currPath.ToLower();
			m_SearchTable.Create(currPath, fbi);
		}
	}
}

void MkFileSystem::_ExportSystemStructure(MkDataNode& node, const MkArray<MkStr>& token, unsigned int depth, const FileBlockIndex& fbi, bool includeChunkInfo) const
{
	// file
	if (token.GetSize() == (depth + 1))
	{
		MK_CHECK(m_ChunkTable[fbi.chunkIndex].GetBlockInfo(fbi.blockIndex, node, includeChunkInfo), token[depth] + L" file node ���� ����")
			return;
		
		MkPathName::__IncreaseFileCount(node);
	}
	// directory
	else
	{
		MkHashStr currName = token[depth];
		MkDataNode* childNode = node.GetChildNode(currName);
		if (childNode == NULL)
		{
			childNode = node.CreateChildNode(currName);
			MK_CHECK(childNode != NULL, currName.GetString() + L" node ���� ����")
				return;

			if (token.GetSize() >= (depth + 2)) // child node�� directory�� ���
			{
				MkPathName::__IncreaseDirectoryCount(node);
			}
		}

		_ExportSystemStructure(*childNode, token, depth + 1, fbi, includeChunkInfo);
	}
}

bool MkFileSystem::_UpdateFilesToAvailableChunk(const MkArray<unsigned int>& availableChunks, MkFilePathListContainer& filePathListContainer)
{
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
	return true;
}

bool MkFileSystem::_UpdateFilesToNewChunk(MkFilePathListContainer& filePathListContainer)
{
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
	return true;
}

//------------------------------------------------------------------------------------------------//
