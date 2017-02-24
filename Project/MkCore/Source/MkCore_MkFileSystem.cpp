
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"
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
}

void MkFileSystem::SetFilterForOriginalFiles
(const MkArray<MkPathName>& nameFilter, const MkArray<MkPathName>& extensionFilter, const MkArray<MkStr>& prefixFilter, const MkArray<MkPathName>& exceptionFilter)
{
	m_NameFilter = nameFilter;
	m_ExtensionFilter = extensionFilter;
	m_PrefixFilter = prefixFilter;
	m_ExceptionFilter = exceptionFilter;
}

bool MkFileSystem::SetSystemSetting(const MkPathName& settingFilePath)
{
	MkDataNode node;
	bool ok = node.Load(settingFilePath);
	if (ok)
	{
		unsigned int chunkSizeInMB;
		if (node.GetData(L"ChunkSizeInMB", chunkSizeInMB, 0))
		{
			SetChunkSizeGuideline(chunkSizeInMB);
		}

		unsigned int percentageForCompressing;
		if (node.GetData(L"PercentageForCompressing", percentageForCompressing, 0))
		{
			SetPercentageForCompressing(percentageForCompressing);
		}

		MkStr prefix, extension;
		if (node.GetData(L"Prefix", prefix, 0) && node.GetData(L"Extension", extension, 0))
		{
			SetChunkFileNamingRule(prefix, extension);
		}

		MkArray<MkStr> nameFilter, extensionFilter, exceptionFilter;

		m_NameFilter.Clear();
		if (node.GetData(L"NameFilter", nameFilter))
		{
			m_NameFilter.Reserve(nameFilter.GetSize());
			MK_INDEXING_LOOP(nameFilter, i)
			{
				MkStr& tmpName = nameFilter[i];
				tmpName.ReplaceKeyword(L"/", L"\\"); // 디렉토리명을 문자열로 사용 할 경우 탈출문자(\")로 인해 파싱이 실패하므로 대신 /를 사용
				m_NameFilter.PushBack(tmpName);
			}
		}

		m_ExtensionFilter.Clear();
		if (node.GetData(L"ExtensionFilter", extensionFilter))
		{
			m_ExtensionFilter.Reserve(extensionFilter.GetSize());
			MK_INDEXING_LOOP(extensionFilter, i)
			{
				m_ExtensionFilter.PushBack(extensionFilter[i]);
			}
		}

		m_PrefixFilter.Clear();
		node.GetData(L"PrefixFilter", m_PrefixFilter);

		m_ExceptionFilter.Clear();
		if (node.GetData(L"ExceptionFilter", exceptionFilter))
		{
			m_ExceptionFilter.Reserve(exceptionFilter.GetSize());
			MK_INDEXING_LOOP(exceptionFilter, i)
			{
				m_ExceptionFilter.PushBack(exceptionFilter[i]);
			}
		}
	}
	return ok;
}

//------------------------------------------------------------------------------------------------//

bool MkFileSystem::SetUpFromOriginalDirectory(const MkPathName& absolutePathOfBaseDirectory, const MkPathName& workingDirectoryPath)
{
	// 경로 유효성 검사
	MK_CHECK(absolutePathOfBaseDirectory.IsAbsolutePath() && absolutePathOfBaseDirectory.IsDirectoryPath() && absolutePathOfBaseDirectory.CheckAvailable(), absolutePathOfBaseDirectory + L" 경로 오류")
		return false;

	MkPathName absBase = absolutePathOfBaseDirectory;
	absBase.CheckAndAddBackslash();

	m_AbsoluteWorkingDirectoryPath.ConvertToRootBasisAbsolutePath(workingDirectoryPath);
	MK_CHECK(m_AbsoluteWorkingDirectoryPath.IsDirectoryPath(), m_AbsoluteWorkingDirectoryPath + L" 경로 오류")
		return false;

	m_ChunkTable.Clear();
	m_SearchTable.Clear();

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
	absBase.GetBlackFileList(relativeFilePathList, m_NameFilter, m_ExtensionFilter, m_PrefixFilter, m_ExceptionFilter, true, false);
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

	m_ChunkTable.Clear();
	m_SearchTable.Clear();

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

	relativeFilePath.ToLower();
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

void MkFileSystem::PrintSystemInfoToDevPanel(bool printFilterInfo) const
{
	if ((MkDevPanel::InstancePtr() != NULL) && MK_DEV_PANEL.IsEnable())
	{
		MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
		MK_DEV_PANEL.MsgToLog(L"[ MkFileSystem 정보 ]");
		MK_DEV_PANEL.MsgToLog(L"   크기 가이드라인 : " + MkStr(m_ChunkSizeGuideline / 1048576) + L" MB");
		MK_DEV_PANEL.MsgToLog(L"   압축 경계 : " + MkStr(m_PercentageForCompressing) + L" %");
		MK_DEV_PANEL.MsgToLog(L"   청크 파일 형식 : " + m_ChunkFilePrefix + L"(N)." + m_ChunkFileExtension);
		if (GetTotalChunkCount() > 0)
		{
			MK_DEV_PANEL.MsgToLog(L"   청크 파일 수 : " + MkStr(GetTotalChunkCount()));
			
			MkMap<MkStr, unsigned int> blankBuffer;
			if (GetBlankPercentage(blankBuffer))
			{
				MK_DEV_PANEL.MsgToLog(L"   청크 공극률");

				MkMapLooper<MkStr, unsigned int> looper(blankBuffer);
				MK_STL_LOOP(looper)
				{
					MK_DEV_PANEL.MsgToLog(L"   - " + looper.GetCurrentKey() + L" : " + MkStr(looper.GetCurrentField()) + L" %");
				}
			}

			MK_DEV_PANEL.MsgToLog(L"   모든 파일 수 : " + MkStr(GetTotalFileCount()));
		}

		if (printFilterInfo)
		{
			if (!m_NameFilter.Empty())
			{
				MK_DEV_PANEL.MsgToLog(L"   파일/디렉토리 필터");
				MK_INDEXING_LOOP(m_NameFilter, i)
				{
					MK_DEV_PANEL.MsgToLog(L"   - " + m_NameFilter[i]);
				}
			}

			if (!m_ExtensionFilter.Empty())
			{
				MK_DEV_PANEL.MsgToLog(L"   파일 확장자 필터");
				MK_INDEXING_LOOP(m_ExtensionFilter, i)
				{
					MK_DEV_PANEL.MsgToLog(L"   - " + m_ExtensionFilter[i]);
				}
			}

			if (!m_PrefixFilter.Empty())
			{
				MK_DEV_PANEL.MsgToLog(L"   파일 prefix 필터");
				MK_INDEXING_LOOP(m_PrefixFilter, i)
				{
					MK_DEV_PANEL.MsgToLog(L"   - " + m_PrefixFilter[i]);
				}
			}

			if (!m_ExceptionFilter.Empty())
			{
				MK_DEV_PANEL.MsgToLog(L"   파일 필터링 예외");
				MK_INDEXING_LOOP(m_ExceptionFilter, i)
				{
					MK_DEV_PANEL.MsgToLog(L"   - " + m_ExceptionFilter[i]);
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

bool MkFileSystem::UpdateFromOriginalDirectory(const MkPathName& absolutePathOfBaseDirectory, bool removeOrgFilesAfterUpdating)
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
	absBase.GetBlackFileList(relativeFilePathList, m_NameFilter, m_ExtensionFilter, m_PrefixFilter, m_ExceptionFilter, true, true);
	unsigned int fileCount = relativeFilePathList.GetSize();
	if (fileCount == 0)
		return true;

	// 경로 복사본
	MkFilePathListContainer copyListContainer = filePathListContainer;

	// 대상 파일이 이미 존재하고 있으면 삭제
	MK_INDEXING_LOOP(relativeFilePathList, i)
	{
		MkPathName currPath = relativeFilePathList[i];
		currPath.ToLower();
		MkHashStr relativeFilePathKey = currPath;

		if (m_SearchTable.Exist(relativeFilePathKey))
		{
			const FileBlockIndex& fbi = m_SearchTable[relativeFilePathKey];
			MK_CHECK(m_ChunkTable[fbi.chunkIndex].RemoveFile(fbi.blockIndex), relativeFilePathKey.GetString() + L" 블록 삭제 중 오류 발생")
				return false;
			m_SearchTable.Erase(relativeFilePathKey);
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

	// 모두 성공적으로 완료되었으면 원본 파일 삭제
	if (removeOrgFilesAfterUpdating)
	{
		for (unsigned int i=0; i<fileCount; ++i)
		{
			copyListContainer.GetAbsoluteOriginalFilePath(i).DeleteCurrentFile(); // 파일 삭제
		}

		copyListContainer.GetAbsolutePathOfBaseDirectory().DeleteCurrentDirectory(false); // 빈 폴더 삭제
	}
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
					MkPathName movedPath = movedPathList[i];
					movedPath.ToLower();
					m_SearchTable[movedPath].blockIndex = newIndexList[i];
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

	m_NameFilter.Clear();
	m_ExtensionFilter.Clear();
	m_PrefixFilter.Clear();
	m_ExceptionFilter.Clear();
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

	relativeFilePath.ToLower();
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
		MK_CHECK(m_ChunkTable[fbi.chunkIndex].GetBlockInfo(fbi.blockIndex, node, includeChunkInfo), token[depth] + L" file node 생성 실패")
			return;
		
		_IncreaseUnitCount(node, MkPathName::KEY_FILE_COUNT);
	}
	// directory
	else
	{
		MkHashStr currName = token[depth];
		MkDataNode* childNode = node.GetChildNode(currName);
		if (childNode == NULL)
		{
			childNode = node.CreateChildNode(currName);
			MK_CHECK(childNode != NULL, currName.GetString() + L" node 생성 실패")
				return;

			if (token.GetSize() >= (depth + 2)) // child node가 directory일 경우
			{
				_IncreaseUnitCount(node, MkPathName::KEY_DIR_COUNT);
			}
		}

		_ExportSystemStructure(*childNode, token, depth + 1, fbi, includeChunkInfo);
	}
}

void MkFileSystem::_IncreaseUnitCount(MkDataNode& node, const MkHashStr& key) const
{
	if (node.IsValidKey(key))
	{
		unsigned int cnt;
		node.GetData(key, cnt, 0); // ++cnt
		node.SetData(key, cnt + 1, 0);
	}
	else
	{
		node.CreateUnit(key, static_cast<unsigned int>(1));
	}
}

//------------------------------------------------------------------------------------------------//
