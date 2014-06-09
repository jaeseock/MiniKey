
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
	// 경로 유효성 검사
	MK_CHECK(absolutePathOfBaseDirectory.IsAbsolutePath() && absolutePathOfBaseDirectory.IsDirectoryPath() && absolutePathOfBaseDirectory.CheckAvailable(), absolutePathOfBaseDirectory + L" 경로 오류")
		return false;

	MkPathName absBase = absolutePathOfBaseDirectory;
	absBase.CheckAndAddBackslash();

	m_AbsoluteWorkingDirectoryPath.ConvertToRootBasisAbsolutePath(workingDirectoryPath);
	MK_CHECK(m_AbsoluteWorkingDirectoryPath.IsDirectoryPath(), m_AbsoluteWorkingDirectoryPath + L" 경로 오류")
		return false;

	Clear();

	// 이전 청크 파일들이 남아 있다면 삭제
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
		if (!m_AbsoluteWorkingDirectoryPath.MakeDirectoryPath()) // 출력 경로 보장
			return false;
	}

	// 필터링 전 모든 파일 수와 크기 탐색
	unsigned int totalFileCount = absBase.GetFileCount(true);
	if (totalFileCount == 0)
		return true;

	// 파일 경로 리스트
	MkFilePathListContainer filePathListContainer(absBase);

	// 대상 파일 탐색. 한 폴더 내의 모든 파일은 유일한 경로명을 가지고 있으므로 중복 여부 없음
	MkArray<MkPathName>& relativeFilePathList = filePathListContainer.GetRelativeFilePathList();
	relativeFilePathList.Reserve(totalFileCount);
	absBase.GetBlackFileList(relativeFilePathList, nameFilter, extensionFilter, prefixFilter, exceptionFilter, true, false);
	if (relativeFilePathList.Empty())
		return true;

	// 청크 구성
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
	MK_CHECK(m_AbsoluteWorkingDirectoryPath.IsDirectoryPath() && m_AbsoluteWorkingDirectoryPath.CheckAvailable(), m_AbsoluteWorkingDirectoryPath + L" 경로 오류")
		return false;

	Clear();

	// 이전 청크 파일들 검색
	MkMap<unsigned int, MkPathName> filePathTable;
	m_AbsoluteWorkingDirectoryPath.GetIndexFormFileList(filePathTable, m_ChunkFilePrefix, m_ChunkFileExtension, true);
	if (filePathTable.Empty())
		return true;

	MkMapLooper<unsigned int, MkPathName> looper(filePathTable);
	MK_STL_LOOP(looper)
	{
		unsigned int chunkIndex = looper.GetCurrentKey();

		// 청크 파일 경로명 생성
		MkPathName chunkFilePath;
		_GenerateChunkFilePath(chunkFilePath, chunkIndex);

		// 청크를 생성해 구성
		MkArray<MkPathName> memberFilePathList;
		if (!m_ChunkTable.Create(chunkIndex).SetUpFromChunkFile(chunkFilePath, memberFilePathList))
			return false;

		// 청크에 등록된 파일 경로들을 key로 검색 테이블 구성
		if (chunkFilePath.CheckAvailable())
		{
			_RegisterPathListToSearchTable(memberFilePathList, chunkIndex, 0);
		}
		// 만약 청크파일이 없다면 모든 소속 블록이 disable 상태를 의미. 청크 삭제
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
	// 상대경로로 변환. root directory 기준 상대경로가 될 수 없는 절대경로(장치가 다름)면 실패
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
	// 경로 유효성 검사
	MK_CHECK(!m_AbsoluteWorkingDirectoryPath.Empty(), L"MkFileSystem 초기화 선행")
		return false;

	MK_CHECK(absolutePathOfBaseDirectory.IsAbsolutePath() && absolutePathOfBaseDirectory.IsDirectoryPath(), absolutePathOfBaseDirectory + L" 경로 오류")
		return false;

	// 대상 경로가 존재하지 않으면 갱신 파일이 없다고 간주
	if (!absolutePathOfBaseDirectory.CheckAvailable())
		return true;

	MkPathName absBase = absolutePathOfBaseDirectory;
	absBase.CheckAndAddBackslash();

	// 필터링 전 모든 파일 수와 크기 탐색
	unsigned int totalFileCount = absBase.GetFileCount(true);
	if (totalFileCount == 0)
		return true;

	// 파일 경로 리스트
	MkFilePathListContainer filePathListContainer(absBase);

	// 대상 파일 탐색
	MkArray<MkPathName>& relativeFilePathList = filePathListContainer.GetRelativeFilePathList();
	relativeFilePathList.Reserve(totalFileCount);
	absBase.GetFileList(relativeFilePathList, true, true);
	unsigned int fileCount = relativeFilePathList.GetSize();
	if (fileCount == 0)
		return true;

	// 대상 파일이 이미 존재하고 있으면 삭제
	MK_INDEXING_LOOP(relativeFilePathList, i)
	{
		const MkPathName& relativeFilePath = relativeFilePathList[i];
		if (m_SearchTable.Exist(relativeFilePath))
		{
			const FileBlockIndex& fbi = m_SearchTable[relativeFilePath];
			MK_CHECK(m_ChunkTable[fbi.chunkIndex].RemoveFile(fbi.blockIndex), relativeFilePath + L" 블록 삭제 중 오류 발생")
				return false;
			m_SearchTable.Erase(relativeFilePath);
		}
	}

	// 파일 크기가 0인 경로 제외
	// MkArray<> 특성상 중간 삭제가 효율적이지는 않지만 파일 갱신 과정에서 삭제는 빈번하게 발생하는 일이 아니므로 무시
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

	// 존재하는 청크 중 가이드라인보다 크기가 작은 청크들 검색
	MkArray<unsigned int> availableChunks(m_ChunkTable.GetSize());
	MkMapLooper<unsigned int, MkFileChunk> looper(m_ChunkTable);
	MK_STL_LOOP(looper)
	{
		if (looper.GetCurrentField().GetChunkSize() < m_ChunkSizeGuideline)
		{
			availableChunks.PushBack(looper.GetCurrentKey());
		}
	}

	// 대상 청크들에 파일 추가
	MK_INDEXING_LOOP(availableChunks, i)
	{
		if (!filePathListContainer.Empty())
		{
			unsigned int chunkIndex = availableChunks[i];
			MkFileChunk& currChunk = m_ChunkTable[chunkIndex];
			unsigned int blockOffset = currChunk.GetBlockCount();

			// 기존 청크에 가이드라인이 허락하는 만큼 파일 덧붙힘
			MkArray<MkPathName> memberFilePathList;
			if (!currChunk.AttachOriginalFiles(L"", filePathListContainer, memberFilePathList, m_ChunkSizeGuideline, m_PercentageForCompressing, false))
				return false;

			// 청크에 등록된 파일 경로들을 key로 검색 테이블 구성
			_RegisterPathListToSearchTable(memberFilePathList, chunkIndex, blockOffset);
		}
	}
	availableChunks.Clear();

	// 기존 청크에 등록되지 않고 남아 있는 파일이 있을 경우 새 청크 생성해 등록
	while (!filePathListContainer.Empty())
	{
		// 중간에 빈 인덱스이거나 다음 인덱스 검색
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

		// 청크 생성
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
		// 청크를 순회하면 빈 공간 비율 비교
		MkFileChunk& currChunk = looper.GetCurrentField();
		if (currChunk.GetBlankPercentage() > percentageForOptimizing)
		{
			MkArray<MkPathName> movedPathList;
			MkArray<unsigned int> newIndexList;
			if (!currChunk.OptimizeChunk(movedPathList, newIndexList)) // 최적화 실행
				return false;

			// 개별 파일 소속 청크는 그대로이고 block index만 변경
			if (currChunk.GetAbsoluteChunkFilePath().CheckAvailable())
			{
				MK_INDEXING_LOOP(movedPathList, i)
				{
					m_SearchTable[movedPathList[i]].blockIndex = newIndexList[i];
				}
			}
			// 만약 청크파일이 없다면 모든 소속 블록이 disable 상태를 의미. 삭제를 위해 청크 인덱스 저장
			else
			{
				removedChunk.PushBack(looper.GetCurrentKey());
			}
		}
	}

	// 빈 청크 삭제
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
	// 상대경로로 변환. root directory 기준 상대경로가 될 수 없는 절대경로(장치가 다름)면 실패
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
	// 청크 파일 경로명 생성
	MkPathName chunkFilePath;
	_GenerateChunkFilePath(chunkFilePath, chunkIndex);

	if (MKDEF_SHOW_FILE_PACKING_INFO)
	{
		MK_DEV_PANEL.MsgToLog(L"* 청크 파일 생성 : " + chunkFilePath.GetFileName());
	}

	// 청크를 생성해 구성, 파일로 출력
	MkArray<MkPathName> memberFilePathList;
	MkFileChunk& currChunk = m_ChunkTable.Create(chunkIndex);
	if (!currChunk.AttachOriginalFiles(chunkFilePath, filePathListContainer, memberFilePathList, m_ChunkSizeGuideline, m_PercentageForCompressing, true))
		return false;

	// 청크에 등록된 파일 경로들을 key로 검색 테이블 구성
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
