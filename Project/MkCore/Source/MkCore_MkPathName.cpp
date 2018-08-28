
#include <ShlObj.h>
#include <atltime.h>
#include "MkCore_MkCheck.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkInterfaceForFileReading.h"
#include "MkCore_MkDataNode.h"

// static value
static MkPathName gModulePath;
static MkPathName gRootPath;
static MkStr gAppName;


//------------------------------------------------------------------------------------------------//
// 디렉토리 순회용 객체
//------------------------------------------------------------------------------------------------//

// 기본 정보 검색
class __DirectoryScouter
{
public:

	// startPath : 탐색을 시작할 디렉토리 경로
	// totalFileSize : 총 파일 크기 반환
	// includeSubDirectory : 하위 폴더 탐색 여부
	// 반환 값은 속한 모든 파일 수
	unsigned int ScoutDirectory(const MkPathName& startPath, unsigned __int64& totalFileSize, bool includeSubDirectory)
	{
		if (startPath.IsDirectoryPath())
		{
			MkPathName targetPath;
			targetPath.ConvertToRootBasisAbsolutePath(startPath);
			if (targetPath.CheckAvailable())
			{
				_ScoutDirectory(targetPath, includeSubDirectory, NULL);
				totalFileSize = m_TotalFileSize;
				return m_TotalFileCount;
			}
		}
		totalFileSize = 0;
		return 0;
	}

	// startPath : 탐색을 시작할 디렉토리 경로
	// node : 정보가 담길 data node
	// 반환 값은 소속 파일이 마지막으로 수정된 시간
	unsigned int ScoutDirectory(const MkPathName& startPath, MkDataNode& node)
	{
		if (startPath.IsDirectoryPath())
		{
			MkPathName targetPath;
			targetPath.ConvertToRootBasisAbsolutePath(startPath);
			if (targetPath.CheckAvailable())
			{
				node.Clear();
				_ScoutDirectory(targetPath, true, &node);
				return m_HighestWrittenTime;
			}
		}
		return 0;
	}

	__DirectoryScouter()
	{
		m_TotalFileCount = 0;
		m_TotalFileSize = 0;
		m_HighestWrittenTime = 0;
	}

protected:
	
	// currentDirectoryPath는 존재하는 디렉토리 절대 경로
	void _ScoutDirectory(const MkPathName& currentDirectoryPath, bool includeSubDirectory, MkDataNode* node)
	{
		// 최초 핸들 얻음
		WIN32_FIND_DATA fd;
		MkStr pathForHandling = currentDirectoryPath;
		pathForHandling += L"*.*";
		HANDLE hFind = FindFirstFile(pathForHandling.GetPtr(), &fd);
		if (hFind == INVALID_HANDLE_VALUE)
			return;

		// 핸들 검사
		do
		{
			// 히든 속성 제외
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0)
			{
				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) // 파일
				{
					unsigned __int64 currSize = static_cast<unsigned __int64>(fd.nFileSizeLow) +
						static_cast<unsigned __int64>(fd.nFileSizeHigh) * (static_cast<unsigned __int64>(MAXDWORD) + static_cast<unsigned __int64>(1));

					m_TotalFileSize += currSize;
					++m_TotalFileCount;

					if (node != NULL)
					{
						MkPathName targetPath = currentDirectoryPath;
						MkPathName fileName = fd.cFileName;
						targetPath += fileName;

						MK_CHECK(fd.nFileSizeHigh == 0, targetPath + L" 파일의 크기가 4G를 넘음. 제외함") {}
						if (fd.nFileSizeHigh == 0)
						{
							unsigned int writtenTime = targetPath.GetWrittenTime();
							if (writtenTime > m_HighestWrittenTime)
							{
								m_HighestWrittenTime = writtenTime;
							}

							if (fileName.__CreateFileStructureInfo(*node, false, fd.nFileSizeLow, 0, writtenTime) != NULL)
							{
								MkPathName::__IncreaseFileCount(*node);
							}
						}
					}
				}
				else if (includeSubDirectory) // 디렉토리
				{
					MkPathName targetName = fd.cFileName;
					if ((targetName != L".") && (targetName != L".."))
					{
						MkDataNode* childNode = NULL;
						if (node != NULL)
						{
							childNode = node->CreateChildNode(targetName);
							MK_CHECK(childNode != NULL, targetName + L" : 같은 이름의 파일이 동일 위치에 존재해 해당 디렉토리 생성 실패") {}
						}

						MkPathName targetPath = currentDirectoryPath;
						targetPath += targetName;
						targetPath += L"\\";
						_ScoutDirectory(targetPath, includeSubDirectory, childNode); // 재귀

						if (childNode != NULL)
						{
							// 해당 자식노드의 하위에 아무런 파일이나 디렉토리가 없으면 삭제
							if ((!childNode->IsValidKey(MkPathName::KeyDirCount)) && (!childNode->IsValidKey(MkPathName::KeyFileCount)))
							{
								childNode->DetachFromParentNode();
								delete childNode;
							}
							else
							{
								MkPathName::__IncreaseDirectoryCount(*node); // 유의미하면 카운터 증가
							}
						}
					}
				}
			}
		}
		while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}

protected:

	unsigned int m_TotalFileCount;
	unsigned __int64 m_TotalFileSize;

	unsigned int m_HighestWrittenTime;
};

// 파일, 디렉토리 삭제
// 디렉토리 속성에 대해서는 (http://benjaminlog.com/entry/directory-readonly-attribute) 참조
class __DirectoryDestroyer
{
public:

	// startPath : 탐색을 시작할 디렉토리 경로
	// deleteAllFiles : true면 포함된 모든 파일까지 삭제. false면 빈 디렉토리만 삭제
	bool DestroyDirectory(const MkPathName& startPath, bool deleteAllFiles = true) const
	{
		if (!startPath.IsDirectoryPath())
			return false;

		MkPathName targetPath;
		targetPath.ConvertToRootBasisAbsolutePath(startPath);
		if (targetPath.CheckAvailable())
		{
			_DestroyDirectory(targetPath, deleteAllFiles);
			return (!targetPath.CheckAvailable());
		}
		return true;
	}

protected:

	// currentDirectoryPath는 존재하는 디렉토리 절대 경로
	void _DestroyDirectory(const MkPathName& currentDirectoryPath, bool deleteAllFiles) const
	{
		// 최초 핸들 얻음
		WIN32_FIND_DATA fd;
		MkStr pathForHandling = currentDirectoryPath;
		pathForHandling += L"*.*";
		HANDLE hFind = FindFirstFile(pathForHandling.GetPtr(), &fd);
		bool noFiles = true;

		if (hFind != INVALID_HANDLE_VALUE)
		{
			// 핸들 검사
			do
			{
				MkPathName targetName = fd.cFileName;
				MkPathName targetPath = currentDirectoryPath;
				targetPath += targetName;
				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) // 파일
				{
					if (deleteAllFiles)
					{
						if (((fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0) || ((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0))
						{
							SetFileAttributes(targetPath.GetPtr(), FILE_ATTRIBUTE_NORMAL);
						}
						DeleteFile(targetPath.GetPtr());
					}
					else
						noFiles = false;
				}
				else // 디렉토리
				{
					if ((targetName != L".") && (targetName != L".."))
					{
						targetPath += L"\\";
						_DestroyDirectory(targetPath, deleteAllFiles); // 재귀
					}
				}
			}
			while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}

		if (noFiles)
		{
			RemoveDirectory(currentDirectoryPath.GetPtr());
		}
	}
};

// 기본 콜렉션. 모든 파일 검색
class __FileCollector
{
public:

	// fileNameList : 검색된 파일 경로들이 담길 버퍼. 가능하면 충분히 Reserve 할 것
	// startPath : 탐색을 시작할 디렉토리 경로
	// includeSubDirectory : 하위 폴더 탐색 여부
	// filter : 초기화 된 필터(opt)
	void GetFileList
		(MkArray<MkPathName>& fileNameList, const MkPathName& startPath,
		bool includeSubDirectory, bool includeZeroSizeFile, const MkPathName::Filter* filter) const
	{
		if (startPath.IsDirectoryPath())
		{
			MkPathName targetPath;
			targetPath.ConvertToRootBasisAbsolutePath(startPath);
			if (targetPath.CheckAvailable())
			{
				_GetFileListInDirectory(fileNameList, targetPath, L"", includeSubDirectory, includeZeroSizeFile, filter);
			}
		}
	}

protected:

	void _GetFileListInDirectory
		(MkArray<MkPathName>& fileNameList, const MkPathName& startPath, const MkPathName& offset,
		bool includeSubDirectory, bool includeZeroSizeFile, const MkPathName::Filter* filter) const
	{
		// 최초 핸들 얻음
		WIN32_FIND_DATA fd;
		MkStr pathForHandling;
		pathForHandling.Reserve(startPath.GetSize() + offset.GetSize() + 3);
		pathForHandling += startPath;
		pathForHandling += offset;
		pathForHandling += L"*.*";
		HANDLE hFind = FindFirstFile(pathForHandling.GetPtr(), &fd);
		if (hFind == INVALID_HANDLE_VALUE)
			return;

		// 핸들 검사
		do
		{
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0) // 히든 속성 제외
			{
				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) // 파일
				{
					if (includeZeroSizeFile || ((fd.nFileSizeLow > 0) || (fd.nFileSizeHigh > 0))) // 용량 검사
					{
						MkPathName fileName = fd.cFileName;
						if ((filter == NULL) ? true : filter->CheckAvailableFile(offset + fileName))
						{
							fileNameList.PushBack(offset + fileName);
						}
					}
				}
				else if (includeSubDirectory) // 디렉토리
				{
					MkPathName targetName = fd.cFileName;
					if ((targetName != L".") && (targetName != L".."))
					{
						MkPathName targetPath = offset;
						targetPath += targetName;
						targetPath += L"\\";

						if ((filter == NULL) ? true : filter->CheckAvailableDirectory(targetPath))
						{
							_GetFileListInDirectory(fileNameList, startPath, targetPath, includeSubDirectory, includeZeroSizeFile, filter); // 재귀
						}
					}
				}
			}
		}
		while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
};

//------------------------------------------------------------------------------------------------//

void MkPathName::SetUp(const wchar_t* rootPath)
{
	if (gModulePath.Empty() || gAppName.Empty())
	{
		wchar_t fullPath[MAX_PATH];
		GetModuleFileName(NULL, fullPath, MAX_PATH);
		MkPathName tmpBuf = fullPath;
		MkStr ext;
		tmpBuf.SplitPath(gModulePath, gAppName, ext);
	}

	if (gRootPath.Empty())
	{
		if (rootPath == NULL)
		{
			gRootPath = gModulePath;
		}
		else
		{
			SetRootDirectory(rootPath);
		}
	}
}

//------------------------------------------------------------------------------------------------//

MkPathName::MkPathName(void) : MkStr() {}
MkPathName::MkPathName(const MkPathName& str) : MkStr(str) { ReplaceKeyword(L"/", L"\\"); }
MkPathName::MkPathName(const MkStr& str) : MkStr(str) { ReplaceKeyword(L"/", L"\\"); }
MkPathName::MkPathName(const MkHashStr& str) : MkStr(str) { ReplaceKeyword(L"/", L"\\"); }
MkPathName::MkPathName(const std::wstring& str) : MkStr(str) { ReplaceKeyword(L"/", L"\\"); }
MkPathName::MkPathName(const wchar_t* wcharArray) : MkStr(wcharArray) { ReplaceKeyword(L"/", L"\\"); }

//------------------------------------------------------------------------------------------------//

bool MkPathName::CheckAvailable() const
{
	return (_waccess(m_Str.GetPtr(), 0) != -1);
}

bool MkPathName::IsDirectoryPath(void) const
{
	unsigned int extPos = _GetExtensionPosition();
	assert(extPos != MKDEF_ARRAY_ERROR);
	return (extPos == 0);
}

bool MkPathName::IsAbsolutePath(void) const
{
	if ((GetSize() > 2) && (m_Str[1] == L':') && (m_Str[2] == L'\\')) // 로컬 드라이버 구분자 검사
		return true;

	return CheckPrefix(L"\\\\"); // 네트워크 경로인지 검사
}

unsigned int MkPathName::GetFileSize(void) const
{
	if (!IsDirectoryPath())
	{
		MkInterfaceForFileReading frInterface;
		if (frInterface.SetUp(*this))
		{
			return frInterface.GetFileSize();
		}
	}
	return 0;
}

MkPathName MkPathName::GetPath(void) const
{
	MkPathName tmpPath;
	MkStr tmpName, tmpExt;
	SplitPath(tmpPath, tmpName, tmpExt);
	return tmpPath;
}

MkPathName MkPathName::GetFileName(bool includeExtension) const
{
	MkPathName fileName;
	if (!IsDirectoryPath())
	{
		MkPathName tmpPath;
		MkStr tmpName, tmpExt;
		SplitPath(tmpPath, tmpName, tmpExt);
		if (includeExtension)
		{
			tmpName.Reserve(tmpName.GetSize() + tmpExt.GetSize() + 1); // IsDirectoryPath() 검사에서 확장자가 존재함을 보장
			tmpName += L".";
			tmpName += tmpExt;
		}
		fileName = tmpName;
	}
	return fileName;
}

MkStr MkPathName::GetFileExtension(void) const
{
	MkStr tmpExt;
	unsigned int extPos = _GetExtensionPosition();
	if ((extPos > 0) && (extPos != MKDEF_ARRAY_ERROR))
	{
		GetSubStr(MkArraySection(extPos), tmpExt);
	}
	return tmpExt;
}

void MkPathName::SplitPath(MkPathName& path, MkStr& name, MkStr& extension) const
{
	wchar_t driveBuf[MAX_PATH];
	wchar_t pathBuf[MAX_PATH];
	wchar_t nameBuf[MAX_PATH];
	wchar_t extensionBuf[MAX_PATH];
	_wsplitpath_s(m_Str.GetPtr(), driveBuf, MAX_PATH, pathBuf, MAX_PATH, nameBuf, MAX_PATH, extensionBuf, MAX_PATH);

	path = driveBuf; // path
	path += MkStr(pathBuf);

	name = nameBuf; // name

	MkStr ext = extensionBuf; // extension
	if (!ext.Empty())
	{
		ext.GetSubStr(MkArraySection(1), extension);
	}
}

//------------------------------------------------------------------------------------------------//

bool MkPathName::Filter::SetUp
(const MkArray<MkPathName>* exceptionFilter, const MkArray<MkPathName>* nameFilter, const MkArray<MkPathName>* extensionFilter, const MkArray<MkStr>* prefixFilter)
{
	m_OnExceptionFilter = (exceptionFilter == NULL) ? false : _ConvertArrayToHashMap(*exceptionFilter, m_ExceptionFilter);
	m_OnNameFilter = (nameFilter == NULL) ? false : _ConvertArrayToHashMap(*nameFilter, m_NameFilter);
	m_OnExtensionFilter = (extensionFilter == NULL) ? false : _ConvertArrayToHashMap(*extensionFilter, m_ExtensionFilter);
	m_OnPrefixFilter = (prefixFilter == NULL) ? false : _ConvertArrayToArray(*prefixFilter, m_PrefixFilter);
	return CheckAvailable();
}

bool MkPathName::Filter::CheckAvailable(void) const
{
	return (m_OnExceptionFilter || m_OnNameFilter || m_OnExtensionFilter || m_OnPrefixFilter);
}

bool MkPathName::Filter::CheckAvailableFile(const MkPathName& relativePath) const
{
	MkPathName targetPath = relativePath;
	targetPath.ToLower();

	if (m_OnExceptionFilter || m_OnNameFilter)
	{
		MkHashStr hashKey(targetPath);
		if (m_OnExceptionFilter)
		{
			if (m_ExceptionFilter.Exist(hashKey))
				return !m_PassingResult;
		}

		if (m_OnNameFilter)
		{
			if (m_NameFilter.Exist(hashKey))
				return m_PassingResult;
		}
	}

	if (m_OnExtensionFilter)
	{
		if (m_ExtensionFilter.Exist(MkHashStr(targetPath.GetFileExtension())))
			return m_PassingResult;
	}

	if (m_OnPrefixFilter)
	{
		MkPathName targetFile = targetPath.GetFileName(false);
		MK_INDEXING_LOOP(m_PrefixFilter, i)
		{
			if (targetFile.CheckPrefix(m_PrefixFilter[i]))
				return m_PassingResult;
		}
	}

	return !m_PassingResult;
}

bool MkPathName::Filter::CheckAvailableDirectory(const MkPathName& relativePath) const
{
	if (m_OnNameFilter)
	{
		MkPathName targetPath = relativePath;
		targetPath.ToLower();

		if (m_NameFilter.Exist(MkHashStr(targetPath)))
			return m_PassingResult;
	}

	return !m_PassingResult;
}

bool MkPathName::Filter::GetExceptionFilter(MkArray<MkStr>& buffer) const { return _GetKeyList(m_ExceptionFilter, buffer); }
bool MkPathName::Filter::GetNameFilter(MkArray<MkStr>& buffer) const { return _GetKeyList(m_NameFilter, buffer); }
bool MkPathName::Filter::GetExtensionFilter(MkArray<MkStr>& buffer) const { return _GetKeyList(m_ExtensionFilter, buffer); }
bool MkPathName::Filter::GetPrefixFilter(MkArray<MkStr>& buffer) const { buffer = m_PrefixFilter; return !buffer.Empty(); }

void MkPathName::Filter::Clear(void)
{
	m_ExceptionFilter.Clear();
	m_NameFilter.Clear();
	m_ExtensionFilter.Clear();
	m_PrefixFilter.Clear();

	m_OnExceptionFilter = false;
	m_OnNameFilter = false;
	m_OnExtensionFilter = false;
	m_OnPrefixFilter = false;
}

MkPathName::Filter::Filter(bool result)
{
	m_OnExceptionFilter = false;
	m_OnNameFilter = false;
	m_OnExtensionFilter = false;
	m_OnPrefixFilter = false;
	m_PassingResult = result;
}

bool MkPathName::Filter::_ConvertArrayToHashMap(const MkArray<MkPathName>& source, MkHashMap<MkHashStr, int>& target)
{
	bool turnOn = !source.Empty();
	if (turnOn)
	{
		target.Clear();
		MK_INDEXING_LOOP(source, i)
		{
			MkStr srcStr = source[i];
			target.Create(MkHashStr(srcStr.ToLower()), 0);
		}
		target.Rehash();
	}
	return turnOn;
}

bool MkPathName::Filter::_ConvertArrayToArray(const MkArray<MkStr>& source, MkArray<MkStr>& target)
{
	bool turnOn = !source.Empty();
	if (turnOn)
	{
		target.Clear();
		target.Reserve(source.GetSize());
		MK_INDEXING_LOOP(source, i)
		{
			MkStr srcStr = source[i];
			target.PushBack(srcStr.ToLower());
		}
	}
	return turnOn;
}

bool MkPathName::Filter::_GetKeyList(const MkHashMap<MkHashStr, int>& source, MkArray<MkStr>& buffer)
{
	bool ok = !source.Empty();
	if (ok)
	{
		buffer.Clear();
		buffer.Reserve(source.GetSize());
		MkConstHashMapLooper<MkHashStr, int> looper(source);
		MK_STL_LOOP(looper)
		{
			buffer.PushBack(looper.GetCurrentKey().GetString());
		}
	}
	return ok;
}

MkPathName::WhiteFilter::WhiteFilter() : Filter(true) {}
MkPathName::BlackFilter::BlackFilter() : Filter(false) {}

//------------------------------------------------------------------------------------------------//

unsigned int MkPathName::GetFileCount(bool includeSubDirectory) const
{
	__DirectoryScouter scouter;
	unsigned __int64 totalFileSize;
	return scouter.ScoutDirectory(*this, totalFileSize, includeSubDirectory);
}

unsigned int MkPathName::GetFileCountAndTotalSize(unsigned __int64& totalFileSize, bool includeSubDirectory) const
{
	__DirectoryScouter scouter;
	return scouter.ScoutDirectory(*this, totalFileSize, includeSubDirectory);
}

void MkPathName::GetFileList(MkArray<MkPathName>& filePathList, bool includeSubDirectory, bool includeZeroSizeFile, const Filter* filter) const
{
	__FileCollector interfaceInstance;
	interfaceInstance.GetFileList(filePathList, *this, includeSubDirectory, includeZeroSizeFile, filter);
}

void MkPathName::GetWhiteFileList
(MkArray<MkPathName>& filePathList, const MkArray<MkPathName>* nameFilter, const MkArray<MkPathName>* extensionFilter,
 const MkArray<MkStr>* prefixFilter, const MkArray<MkPathName>* exceptionFilter, bool includeSubDirectory, bool includeZeroSizeFile) const
{
	WhiteFilter ft;
	const Filter* filter = ft.SetUp(exceptionFilter, nameFilter, extensionFilter, prefixFilter) ? &ft : NULL;
	GetFileList(filePathList, includeSubDirectory, includeZeroSizeFile, filter);
}

void MkPathName::GetBlackFileList
(MkArray<MkPathName>& filePathList, const MkArray<MkPathName>* nameFilter, const MkArray<MkPathName>* extensionFilter,
 const MkArray<MkStr>* prefixFilter, const MkArray<MkPathName>* exceptionFilter, bool includeSubDirectory, bool includeZeroSizeFile) const
{
	BlackFilter ft;
	const Filter* filter = ft.SetUp(exceptionFilter, nameFilter, extensionFilter, prefixFilter) ? &ft : NULL;
	GetFileList(filePathList, includeSubDirectory, includeZeroSizeFile, filter);
}

void MkPathName::GetIndexFormFileList
(MkMap<unsigned int, MkPathName>& filePathTable, const MkStr& prefix, const MkStr& extension, bool includeZeroSizeFile) const
{
	if (prefix.Empty() || extension.Empty())
		return;

	// 먼저 prefix, extension으로 white filtering
	MkArray<MkPathName> filePathList;
	filePathList.Reserve(256);
	MkArray<MkPathName> extensionFilter;
	extensionFilter.PushBack(extension);
	MkArray<MkStr> prefixFilter;
	prefixFilter.PushBack(prefix);

	GetWhiteFileList(filePathList, NULL, &extensionFilter, &prefixFilter, NULL, false, includeZeroSizeFile);
	if (filePathList.Empty())
		return;

	unsigned int indexPosition = prefix.GetSize();
	MK_INDEXING_LOOP(filePathList, i)
	{
		const MkPathName& filePath = filePathList[i];

		MkPathName path; // 파일경로를 파싱해 이름 얻음
		MkStr name, extension;
		filePath.SplitPath(path, name, extension);

		MkStr indexPart; // 이름에서 index 파트 분리
		name.GetSubStr(MkArraySection(indexPosition), indexPart);
		if (indexPart.IsDigit())
		{
			unsigned int index = indexPart.ToUnsignedInteger();
			if (!filePathTable.Exist(index))
			{
				filePathTable.Create(index, filePath);
			}
		}
	}
}

// 2000년 기준이므로 1970 ~ 2000.1.1 00:00:00에 해당하는 초를 제외
// CTime c2000(2000, 1, 1, 0, 0, 0, 0) >> c2000.GetTime() == 946652400;
#define MKDEF_WRITTEN_TIME_OFFSET (static_cast<__time64_t>(946652400))

unsigned int MkPathName::GetWrittenTime(void) const
{
	if (!CheckAvailable())
		return 0;

	HANDLE fileHandle = CreateFile(m_Str.GetPtr(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE)
		return 0;

	FILETIME write_time;
	GetFileTime(fileHandle, NULL, NULL, &write_time);
	SYSTEMTIME write_system_time, write_local_time;
	FileTimeToSystemTime(&write_time, &write_system_time);
	SystemTimeToTzSpecificLocalTime(NULL, &write_system_time, &write_local_time);
    CloseHandle(fileHandle);

	if ((write_local_time.wYear < 2000) || (write_local_time.wYear > 2135))
		return 0;

	/*
	// CTime 이라는 것이 있는 줄 모르고 만든 수동계산
	// 값은 정확히 맞는데... 무식하면 몸이 피곤하다는 속담의 산 증거...
	// 만약 atltime을 사용하기 싫다면 이 쪽을 대신 사용 할 것
	// 그레고리력 기준으로 계산
	// http://ko.wikipedia.org/wiki/%EC%9C%A4%EB%85%84
	// 2000 ~ 2136.2 범위이므로 2100년을 제외한 년도는 4로 나누어 떨어지면 윤년이라는 점을 주의할 것
	unsigned int thisYear = static_cast<unsigned int>(write_local_time.wYear) - 2000;
	unsigned int lastLeapYearCount = (thisYear == 0) ? 0 : ((thisYear - 1) / 4 + (((thisYear - 1) < 100) ? 1 : 0)); // 작년까지의 윤년 수(ex> 2011년은 3개(00, 04, 08), 2111년은 25개)
	unsigned int daysTillLastYear = thisYear * 365 + lastLeapYearCount; // 작년까지의 일수
	bool cl = ((thisYear != 100) && ((thisYear % 4) == 0)); // 올해가 윤년이면
	unsigned int daysForLastMonth[12] = {0, 31, cl?60:59, cl?91:90, cl?121:120, cl?152:151, cl?182:181, cl?213:212, cl?244:243, cl?274:273, cl?305:304, cl?335:334};
	unsigned int month = static_cast<unsigned int>(write_local_time.wMonth);
	unsigned int day = static_cast<unsigned int>(write_local_time.wDay);
	unsigned int daysOfThisYear = daysForLastMonth[month - 1] + day - 1; // 어제까지의 올해 일수
	unsigned int secsTillYesterday = (daysTillLastYear + daysOfThisYear) * 86400; // 어제까지의 초수
	unsigned int secsOfToday = // 오늘의 초수
		static_cast<unsigned int>(write_local_time.wHour) * 3600 +
		static_cast<unsigned int>(write_local_time.wMinute) * 60 +
		static_cast<unsigned int>(write_local_time.wSecond);

	return (secsTillYesterday + secsOfToday);
	*/

	// 자동 계산(CTime 이용)
	// CTime의 범위가 1970.1.1 ~ 2038.12.31이라 위 필터링에서 자동으로 걸러지므로 안심하고 사용 가능
	return ConvertWrittenTime
		(static_cast<int>(write_local_time.wYear), static_cast<int>(write_local_time.wMonth), static_cast<int>(write_local_time.wDay),
		static_cast<int>(write_local_time.wHour), static_cast<int>(write_local_time.wMinute), static_cast<int>(write_local_time.wSecond));
}

void MkPathName::ConvertWrittenTime(unsigned int writtenTime, int& year, int& month, int& day, int& hour, int& min, int& sec)
{
	CTime ctime(static_cast<__time64_t>(writtenTime) + MKDEF_WRITTEN_TIME_OFFSET);

	year = ctime.GetYear();
	month = ctime.GetMonth();
	day = ctime.GetDay();
	hour = ctime.GetHour();
	min = ctime.GetMinute();
	sec = ctime.GetSecond();
}

unsigned int MkPathName::ConvertWrittenTime(int year, int month, int day, int hour, int min, int sec)
{
	CTime ctime(year, month, day, hour, min, sec, 0);
	__time64_t currentSecs = ctime.GetTime();
	__time64_t secsFrom2000 = currentSecs - MKDEF_WRITTEN_TIME_OFFSET;
	return static_cast<unsigned int>(secsFrom2000);
}

//------------------------------------------------------------------------------------------------//

bool MkPathName::ChangeFileName(const MkStr& name)
{
	assert(!name.Empty());

	MkPathName tmpPath;
	MkStr tmpName, tmpExt;
	SplitPath(tmpPath, tmpName, tmpExt);
	if (tmpExt.Empty())
		return false;

	*this = tmpPath;
	*this += name;
	*this += L".";
	*this += tmpExt;
	return true;
}

bool MkPathName::ChangeFileExtension(const MkStr& extension)
{
	assert(!extension.Empty());

	MkStr extBuf = GetFileExtension();
	if (extBuf.Empty())
		return false;

	BackSpace(extBuf.GetSize());
	*this += extension;
	return true;
}

void MkPathName::CheckAndAddBackslash(void)
{
	if ((!Empty()) && (!CheckPostfix(L"\\")))
	{
		*this += L"\\";
	}
}

void MkPathName::OptimizePath(void)
{
	bool directoryPath = IsDirectoryPath();

	if (!Exist(L".\\"))
	{
		if (directoryPath)
		{
			CheckAndAddBackslash();
		}
		return;
	}

	// split by backslash
	MkArray<MkStr> tokens;
	Tokenize(tokens, L"\\");
	MkArray<MkStr> directoryBuffer(tokens.GetSize());

	MK_INDEXING_LOOP(tokens, i)
	{
		const MkStr& token = tokens[i];
		if (token.Equals(0, L".."))
		{
			if (directoryBuffer.Empty() || directoryBuffer[directoryBuffer.GetSize() - 1].Equals(0, L".."))
			{
				directoryBuffer.PushBack(L"..");
			}
			else
			{
				directoryBuffer.PopBack();
			}
		}
		else if (token.Equals(0, L"."))
		{
			// skip
		}
		else
		{
			directoryBuffer.PushBack(token);
		}
	}

	// merge directory lists
	Flush();
	unsigned int lastIndex = directoryBuffer.GetSize() - 1;
	MK_INDEXING_LOOP(directoryBuffer, i)
	{
		*this += directoryBuffer[i];
		if (directoryPath || (i != lastIndex))
		{
			*this += L"\\";
		}
	}
}

void MkPathName::ConvertToModuleBasisAbsolutePath(const MkPathName& path) { _UpdateCurrentPath(GetModuleDirectory(), path); }
bool MkPathName::ConvertToModuleBasisRelativePath(void) { return _ConvertToRelativePath(GetModuleDirectory()); }
void MkPathName::ConvertToWorkingBasisAbsolutePath(const MkPathName& path) { _UpdateCurrentPath(GetWorkingDirectory(), path); }
bool MkPathName::ConvertToWorkingBasisRelativePath(void) { return _ConvertToRelativePath(GetWorkingDirectory()); }
void MkPathName::ConvertToRootBasisAbsolutePath(const MkPathName& path) { _UpdateCurrentPath(GetRootDirectory(), path); }
bool MkPathName::ConvertToRootBasisRelativePath(void) { return _ConvertToRelativePath(GetRootDirectory()); }

void MkPathName::ConvertToSystemBasisAbsolutePath(const MkPathName& path, int csidl)
{
	wchar_t buffer[MAX_PATH] = {0, };
	::SHGetSpecialFolderPath(NULL, buffer, csidl, NULL);

	MkPathName systemPath = buffer;
	systemPath.CheckAndAddBackslash();

	_UpdateCurrentPath(systemPath, path);
}

//------------------------------------------------------------------------------------------------//

bool MkPathName::DeleteCurrentFile(void) const
{
	MkPathName tmpPath;
	tmpPath.ConvertToRootBasisAbsolutePath(*this);
	if (tmpPath.CheckAvailable())
	{
		return tmpPath.IsDirectoryPath() ? false : (DeleteFile(tmpPath.GetPtr()) != 0);
	}
	return true;
}

bool MkPathName::DeleteCurrentDirectory(bool deleteAllFiles) const
{
	__DirectoryDestroyer destroyer;
	return destroyer.DestroyDirectory(*this, deleteAllFiles);
}

bool MkPathName::MakeDirectoryPath(void) const
{
	MkPathName tmpPath;
	tmpPath.ConvertToRootBasisAbsolutePath(*this);
	if (!tmpPath.CheckAvailable())
	{
		if (tmpPath.IsDirectoryPath())
		{
			MkArray<MkStr> directories;
			unsigned int pathCount = tmpPath.Tokenize(directories, L"\\");
			assert(pathCount > 0); // 절대경로가 비어 있음
			
			unsigned int index = 0;
			MK_INDEXING_RLOOP(directories, i)
			{
				tmpPath.BackSpace(directories[i].GetSize() + 1);
				if (tmpPath.CheckAvailable())
				{
					index = i;
					break;
				}
			}
			assert(index > 0); // 장치명까지 존재하지 않음
			for (unsigned int i=index; i<pathCount; ++i)
			{
				tmpPath += directories[i] + L"\\";
				if (CreateDirectory(tmpPath.GetPtr(), NULL) == 0)
					return false;
			}
		}
		else
			return false;
	}
	return true;
}

bool MkPathName::RenameCurrentFile(const MkPathName& newFileName)
{
	if (newFileName.Exist(L"\\"))
		return false;

	if (IsDirectoryPath())
		return false;
	
	MkPathName tmpPath;
	tmpPath.ConvertToRootBasisAbsolutePath(*this);
	if (tmpPath.CheckAvailable())
	{
		MkPathName newPath = tmpPath.GetPath();
		newPath += newFileName;
		bool ok = (MoveFile(tmpPath.GetPtr(), newPath.GetPtr()) != 0);
		if (ok)
		{
			*this = GetPath();
			*this += newFileName;
		}
		return ok;
	}
	return false;
}

bool MkPathName::CopyCurrentFile(const MkPathName& newPath, bool failIfExists) const
{
	return _CopyOrMoveCurrentFile(newPath, true, failIfExists);
}

bool MkPathName::MoveCurrentFile(const MkPathName& newPath) const
{
	return _CopyOrMoveCurrentFile(newPath, false, false);
}

bool MkPathName::SetWrittenTime(unsigned int writtenTime) const
{
	int year, month, day, hour, min, sec;
	ConvertWrittenTime(writtenTime, year, month, day, hour, min, sec);
	return SetWrittenTime(year, month, day, hour, min, sec);
}

bool MkPathName::SetWrittenTime(int year, int month, int day, int hour, int min, int sec) const
{
	MkPathName tmpPath;
	tmpPath.ConvertToRootBasisAbsolutePath(*this);
	if (tmpPath.CheckAvailable())
	{
		HANDLE hFile = ::CreateFile
				(tmpPath.GetPtr(), GENERIC_READ | GENERIC_WRITE , FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
		
		if (hFile == INVALID_HANDLE_VALUE)
			return false;

		SYSTEMTIME st;
		st.wYear = year;
		st.wMonth = month;
		st.wDayOfWeek = 0;
		st.wDay = day;
		st.wHour = hour;
		st.wMinute = min;
		st.wSecond = sec;
		st.wMilliseconds = 0;

		FILETIME ft;
		::SystemTimeToFileTime(&st, &ft);
		::LocalFileTimeToFileTime(&ft, &ft);
		bool ok = (::SetFileTime(hFile, NULL, NULL, &ft) != 0);
		::CloseHandle(hFile);
		return ok;
	}
	return false;
}

//------------------------------------------------------------------------------------------------//

const MkStr& MkPathName::GetApplicationName(void) { return gAppName; }
const MkPathName& MkPathName::GetModuleDirectory(void) { return gModulePath; }

MkPathName MkPathName::GetWorkingDirectory(void)
{
	wchar_t fullPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, fullPath);
	MkPathName tmpBuf = fullPath;
	tmpBuf.CheckAndAddBackslash();
	return tmpBuf;
}

void MkPathName::SetWorkingDirectory(const MkPathName& workingDirectory)
{
	assert(workingDirectory.IsDirectoryPath() && workingDirectory.IsAbsolutePath() && workingDirectory.CheckAvailable());
	SetCurrentDirectory(workingDirectory.GetPtr());
}

const MkPathName& MkPathName::GetRootDirectory(void) { return gRootPath; }

void MkPathName::SetRootDirectory(const MkPathName& rootPath)
{
	assert(rootPath.IsDirectoryPath());
	gRootPath.ConvertToModuleBasisAbsolutePath(rootPath);
	gRootPath.MakeDirectoryPath();
}

//------------------------------------------------------------------------------------------------//

void MkPathName::OpenDirectoryInExplorer(void) const
{
	MkPathName tmpPath;
	tmpPath.ConvertToRootBasisAbsolutePath(*this);
	if (tmpPath.CheckAvailable())
	{
		// http://www.digipine.com/1293
		SHELLEXECUTEINFO si;
		ZeroMemory(&si, sizeof(SHELLEXECUTEINFO));
		si.cbSize = sizeof(SHELLEXECUTEINFO);
		si.hwnd = NULL;
		si.lpVerb = L"";
		si.lpFile = L"explorer.exe";
		si.nShow = SW_SHOW;
		si.lpParameters = tmpPath.GetPtr();
		si.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShellExecuteEx(&si);
	}
}

void MkPathName::OpenFileInVerb(const MkStr& argument) const
{
	if (IsDirectoryPath())
		return;

	MkPathName tmpPath;
	tmpPath.ConvertToRootBasisAbsolutePath(*this);
	if (tmpPath.CheckAvailable())
	{
		SetWorkingDirectory(tmpPath.GetPath()); // 워킹 디렉토리 변경

		SHELLEXECUTEINFO si;
		ZeroMemory(&si, sizeof(SHELLEXECUTEINFO));
		si.cbSize = sizeof(SHELLEXECUTEINFO);
		si.hwnd = NULL;
		si.lpVerb = L"open";
		si.lpFile = tmpPath.GetPtr();
		si.nShow = SW_SHOW;
		si.lpParameters = argument.Empty() ? NULL : argument.GetPtr();
		si.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShellExecuteEx(&si);
	}
}

bool MkPathName::ExcuteWindowProcess(const MkStr& processTitle, const MkStr& argument) const { return _ExcuteProcess(processTitle, NORMAL_PRIORITY_CLASS, argument); }
bool MkPathName::ExcuteConsoleProcess(const MkStr& processTitle, const MkStr& argument) const { return _ExcuteProcess(processTitle, CREATE_NEW_CONSOLE, argument); }

//------------------------------------------------------------------------------------------------//

bool MkPathName::GetSingleFilePathFromDialog(HWND owner)
{
	MkArray<MkStr> extensionList;
	return GetSingleFilePathFromDialog(extensionList, owner);
}

bool MkPathName::GetSingleFilePathFromDialog(const MkStr& extension, HWND owner)
{
	MkArray<MkStr> extensionList(1);
	extensionList.PushBack(extension);
	return GetSingleFilePathFromDialog(extensionList, owner);
}

bool MkPathName::GetSingleFilePathFromDialog(const MkArray<MkStr>& extensionList, HWND owner)
{
	MkPathName directoryPath;
	MkArray<MkPathName> fileNameList;
	bool ok = (_GetFilePathFromDialog(directoryPath, fileNameList, extensionList, owner, true, true) == 1);
	if (ok)
	{
		*this = directoryPath;
	}
	return ok;
}

unsigned int MkPathName::GetMultipleFilePathFromDialog(MkPathName& directoryPath, MkArray<MkPathName>& fileNameList, HWND owner)
{
	MkArray<MkStr> extensionList;
	return GetMultipleFilePathFromDialog(directoryPath, fileNameList, extensionList, owner);
}

unsigned int MkPathName::GetMultipleFilePathFromDialog
(MkPathName& directoryPath, MkArray<MkPathName>& fileNameList, const MkStr& extension, HWND owner)
{
	MkArray<MkStr> extensionList(1);
	extensionList.PushBack(extension);
	return GetMultipleFilePathFromDialog(directoryPath, fileNameList, extensionList, owner);
}

unsigned int MkPathName::GetMultipleFilePathFromDialog(MkPathName& directoryPath, MkArray<MkPathName>& fileNameList, const MkArray<MkStr>& extensionList, HWND owner)
{
	return _GetFilePathFromDialog(directoryPath, fileNameList, extensionList, owner, false, true);
}

bool MkPathName::GetSaveFilePathFromDialog(HWND owner)
{
	MkArray<MkStr> extensionList;
	return GetSaveFilePathFromDialog(extensionList, owner);
}

bool MkPathName::GetSaveFilePathFromDialog(const MkStr& extension, HWND owner)
{
	MkArray<MkStr> extensionList(1);
	extensionList.PushBack(extension);
	return GetSaveFilePathFromDialog(extensionList, owner);
}

bool MkPathName::GetSaveFilePathFromDialog(const MkArray<MkStr>& extensionList, HWND owner)
{
	MkPathName directoryPath;
	MkArray<MkPathName> fileNameList;
	bool ok = (_GetFilePathFromDialog(directoryPath, fileNameList, extensionList, owner, true, false) == 1);
	if (ok)
	{
		*this = directoryPath;
	}
	return ok;
}

bool MkPathName::GetDirectoryPathFromDialog(const MkStr& msg, HWND owner, const MkPathName& initialPath)
{
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(BROWSEINFO));
	bi.hwndOwner = owner;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpszTitle = msg;
	if (!initialPath.Empty())
	{
		if (initialPath.IsAbsolutePath() && initialPath.IsDirectoryPath() && initialPath.CheckAvailable())
		{
			bi.lParam = (LPARAM)(LPCSTR)initialPath.GetPtr();
			bi.lpfn = BrowseCallbackProc;
		}
	}

	LPITEMIDLIST itemIdList = SHBrowseForFolder(&bi);
	if (itemIdList != NULL)
	{
		wchar_t szPath[MAX_PATH];
		SHGetPathFromIDList(itemIdList, szPath);
		*this = szPath;
		CheckAndAddBackslash();
		return true;
	}

	Flush();
	return false;
}

//------------------------------------------------------------------------------------------------//

unsigned int MkPathName::ExportSystemStructure(MkDataNode& node) const
{
	__DirectoryScouter scouter;
	return scouter.ScoutDirectory(*this, node);
}

void MkPathName::__GetSubDirectories(const MkDataNode& node, MkArray<MkHashStr>& subDirs)
{
	_GetSubNodesByTag(node, subDirs, KeyDirCount, KeyWrittenTime, false); // node에서 KeyDirCount 갯수만큼 KeyWrittenTime가 없는 child node
}

void MkPathName::__GetSubFiles(const MkDataNode& node, MkArray<MkHashStr>& subFiles)
{
	_GetSubNodesByTag(node, subFiles, KeyFileCount, KeyWrittenTime, true); // node에서 KeyFileCount 갯수만큼 KeyWrittenTime가 있는 child node
}

void MkPathName::__IncreaseDirectoryCount(MkDataNode& node) { _IncreaseUnitCountByTag(node, KeyDirCount); }
void MkPathName::__IncreaseFileCount(MkDataNode& node) { _IncreaseUnitCountByTag(node, KeyFileCount); }

unsigned int MkPathName::__CountTotalFiles(const MkDataNode& node)
{
	unsigned int files = 0;
	node.GetData(KeyFileCount, files, 0);

	MkArray<MkHashStr> subDirs;
	__GetSubDirectories(node, subDirs);
	MK_INDEXING_LOOP(subDirs, i)
	{
		files += __CountTotalFiles(*node.GetChildNode(subDirs[i]));
	}
	return files;
}

bool MkPathName::__CheckFileDifference(const MkDataNode& lastFileNode, MkDataNode& currFileNode)
{
	MkArray<unsigned int> size0(2);
	unsigned int wt0 = 0;
	if (!lastFileNode.GetData(KeyFileSize, size0))
	{
		size0.PushBack(0);
	}
	lastFileNode.GetData(KeyWrittenTime, wt0, 0);

	unsigned int size1 = 0, wt1 = 0;
	currFileNode.GetData(KeyFileSize, size1, 0);
	currFileNode.GetData(KeyWrittenTime, wt1, 0);

	bool diff = ((size0[0] != size1) || (wt0 != wt1)); // 다른지만 비교하지 대소판단은 하지 않음
	if ((!diff) && (size0.GetSize() == 2))
	{
		currFileNode.SetData(KeyFileSize, size0); // 복사
	}
	return diff;
}

MkDataNode* MkPathName::__CreateFileStructureInfo
(MkDataNode& node, bool compressed, unsigned int origSize, unsigned int compSize, unsigned int writtenTime)
{
	if (Empty() || IsDirectoryPath())
		return NULL;

	MkDataNode* fileNode = node.CreateChildNode(*this);
	MK_CHECK(fileNode != NULL, *this + L" : 같은 이름의 디렉토리가 동일 위치에 존재해 해당 파일 정보 생성 실패")
		return NULL;

	do
	{
		// 크기 정보
		// compressed(false) : 원래 크기만 기록
		// compressed(true) : 원래 크기, 압축 후 크기 순으로 기록
		MkArray<unsigned int> sizeBuf(2);
		sizeBuf.PushBack(origSize);

		if (compressed)
		{
			sizeBuf.PushBack(compSize);
		}

		if (!fileNode->CreateUnit(KeyFileSize, sizeBuf))
			break;

		// 수정 시간
		if (!fileNode->CreateUnit(KeyWrittenTime, writtenTime))
			break;

		return fileNode;
	}
	while (false);

	delete fileNode;
	return NULL;
}

//------------------------------------------------------------------------------------------------//

unsigned int MkPathName::_GetExtensionPosition(void) const
{
	if (Empty())
		return 0;

	unsigned int size = GetSize();
	const wchar_t& lastChar = m_Str[size - 1];
	if ((lastChar == L'\\') || (lastChar == L'/'))
		return 0; // 경로명. ex> "abc\\"

	if (lastChar == L'.')
		return MKDEF_ARRAY_ERROR; // '.'으로 종료, error. ex> "abc."

	if (size == 1)
		return 0; // 경로명. ex> "a"

	for (unsigned int i = size-2; i != 0xffffffff; --i)
	{
		const wchar_t& currChar = m_Str[i];
		if ((currChar == L'\\') || (currChar == L'/'))
			return 0; // 경로명. ex> "abc\\def"

		if (currChar == L'.')
		{
			if (i == 0)
				return MKDEF_ARRAY_ERROR; // 파일명 없음, error. ex> ".ext"

			const wchar_t& nameChar = m_Str[i-1];
			if ((nameChar == L'\\') || (nameChar == L'/'))
				return MKDEF_ARRAY_ERROR; // 파일명 없음, error. ex> "\\.ext"

			return (i + 1); // 확장자 위치 반환
		}
	}
	return 0; // 경로명. ex> "abc"
}

void MkPathName::_UpdateCurrentPath(const MkPathName& basePath, const MkPathName& sourcePath)
{
	if (sourcePath.Empty())
	{
		*this = basePath; // basePath는 항상 최적화되어 있으므로 더 이상의 작업 불필요
	}
	else
	{
		if (sourcePath.IsAbsolutePath())
		{
			*this = sourcePath;
		}
		else
		{
			*this = basePath;
			*this += sourcePath;
		}

		OptimizePath(); // sourcePath에 최적화 필요하므로 절대경로 변환 후 최적화
	}
}

bool MkPathName::_CheckOnSameDevice(const MkPathName& basePath, const MkPathName& targetPath) const
{
	const wchar_t& b0 = basePath.GetAt(0);
	const wchar_t& b1 = basePath.GetAt(1);
	if ((b0 == targetPath.GetAt(0)) && (b1 == targetPath.GetAt(1))) // 처음 두 문자가 일치
	{
		// 네트워크 경로인 경우 장치명 확인
		if ((b0 == L'\\') && (b1 == L'\\')) 
		{
			// 장치명 존재 확인
			assert((basePath.GetSize() > 2) && (targetPath.GetSize() > 2) && (basePath.GetAt(2) != L'\\') && (targetPath.GetAt(2) != L'\\'));

			unsigned int bEndPos = basePath.GetFirstKeywordPosition(MkArraySection(2), L"\\");
			if (bEndPos == MKDEF_ARRAY_ERROR)
				bEndPos = basePath.GetSize();

			unsigned int tEndPos = targetPath.GetFirstKeywordPosition(MkArraySection(2), L"\\");
			if (tEndPos == MKDEF_ARRAY_ERROR)
				tEndPos = targetPath.GetSize();

			if (bEndPos == tEndPos)
			{
				unsigned int size = bEndPos - 2;
				MkStr bDeviceName;
				basePath.GetSubStr(MkArraySection(2, size), bDeviceName);
				MkStr tDeviceName;
				targetPath.GetSubStr(MkArraySection(2, size), tDeviceName);
				return (bDeviceName == tDeviceName);
			}
			return false; // 장치명 크기가 다름
		}

		// 로컬의 경우 드라이브명 일치로 무조건 true
		return true;
	}
	return false;
}

bool MkPathName::_ConvertToRelativePath(const MkPathName& basePath)
{
	OptimizePath();
	if (IsAbsolutePath())
	{
		if (CheckPrefix(basePath))
		{
			PopFront(basePath.GetSize()); // 완전한 basePath 하위
		}
		else
		{
			if (_CheckOnSameDevice(basePath, *this))
			{
				MkArray<MkStr> baseTokens; // basePath 분해
				unsigned int baseCount = basePath.Tokenize(baseTokens, L"\\");

				MkArray<MkStr> currTokens; // 현재경로 분해
				unsigned int currCount = Tokenize(currTokens, L"\\");

				unsigned int index = 1; // 장치명이 일치하므로 최초 경로(장치명)는 무시
				while ((index < baseCount) && (index < currCount)) // 두 경로간 최초로 달라지는 지점 검출
				{
					const MkStr& baseToken = baseTokens[index];
					const MkStr& currToken = currTokens[index];
					if (baseToken == currToken)
					{
						++index;
					}
					else
						break;
				}

				// 완전한 basePath 하위가 아닌데도 모든 구간이 일치하는 경우는 현 경로가 확장자 없는 파일명인 케이스
				// ex> basePath == L"d:\\a\\b\\", currPath == L"d:\\a\\b
				if (baseCount == index)
				{
					assert(baseCount > 1); // upstair 탐색이 가능하려면 최소한 장치명 포함, 구간이 2개 이상이어야 함

					Flush();
					*this += L"..\\";
					*this += currTokens[currCount - 1];
				}
				else
				{
					unsigned int upstairCount = baseCount - index; // 남은 구간 수
					if (upstairCount > index) // upstairCount만큼 L"..\\"을 더할 것이므로 지워질 구간보다 추가될 구간이 많으면 확률상 예약이 유리
					{
						Reserve(GetSize() + upstairCount * 3);
					}

					bool directoryPath = IsDirectoryPath();

					Flush(); // 비우고
					AddRepetition(L"..\\", upstairCount); // upstairCount만큼 L"..\\"을 더함

					if (currCount > index) // 잔여 구간을 더함
					{
						unsigned int lastIndex = currCount - 1;
						for (unsigned int i=index; i<currCount; ++i)
						{
							*this += currTokens[i];
							if (directoryPath || (i != lastIndex))
							{
								*this += L"\\";
							}
						}
					}
				}
			}
			else
				return false;
		}
	}
	return true;
}

bool MkPathName::_CopyOrMoveCurrentFile(const MkPathName& newPath, bool copy, bool failIfExists) const
{
	if (Empty() || IsDirectoryPath() || newPath.Empty())
		return false;

	MkPathName tmpPath;
	tmpPath.ConvertToRootBasisAbsolutePath(*this);
	if (tmpPath.CheckAvailable())
	{
		MkPathName targetPath;
		if (newPath.IsAbsolutePath())
		{
			targetPath = newPath;
		}
		else
		{
			targetPath = tmpPath.GetPath() + newPath;
			targetPath.OptimizePath();
		}

		if (targetPath.IsDirectoryPath())
		{
			targetPath.CheckAndAddBackslash();
			if (!targetPath.MakeDirectoryPath())
				return false;

			targetPath += GetFileName();
		}
		else if (!targetPath.GetPath().MakeDirectoryPath())
			return false;

		if (copy)
		{
			return (::CopyFile(tmpPath.GetPtr(), targetPath.GetPtr(), (failIfExists) ? TRUE : FALSE) != 0);
		}
		return (::MoveFile(tmpPath.GetPtr(), targetPath.GetPtr()) != 0);
	}
	return false;
}

bool MkPathName::_ExcuteProcess(const MkStr& processTitle, DWORD flag, const MkStr& argument) const
{
	MkStr extension = GetFileExtension();
	if (extension.Empty())
		return false;

	extension.ToLower();
	if (extension != L"exe")
		return false;

	MkPathName tmpPath;
	tmpPath.ConvertToRootBasisAbsolutePath(*this);
	if (!tmpPath.CheckAvailable())
		return false;

	// 워킹 디렉토리 변경
	SetWorkingDirectory(tmpPath.GetPath());

	// 인자가 존재하면 추가
	MkStr argBuf;
	if (!argument.Empty())
	{
		argBuf.Reserve(tmpPath.GetSize() + 1 + argument.GetSize());
		argBuf += tmpPath;
		argBuf += MkStr::SPACE;
		argBuf += argument;
	}

	// 실행
	STARTUPINFO si = {0, };
	PROCESS_INFORMATION pi;
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USEPOSITION;
	si.dwY = si.dwX = CW_USEDEFAULT;
	si.lpTitle = (LPWSTR)processTitle.GetPtr();
	return (CreateProcess(tmpPath.GetPtr(), (argBuf.Empty()) ? NULL : argBuf.GetPtr(), NULL, NULL, FALSE, flag, NULL, NULL, &si, &pi) != 0);
}

int CALLBACK MkPathName::BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM dwData)
{
	switch (uMsg)
	{
    case BFFM_INITIALIZED:
		SendMessage(hWnd, BFFM_SETSELECTION, TRUE, dwData);
		break;
	}
	return 0;
}

unsigned int MkPathName::_GetFilePathFromDialog
(MkPathName& directoryPath, MkArray<MkPathName>& fileNameList, const MkArray<MkStr>& extensionList, HWND owner, bool singleSelection, bool forOpen)
{
	unsigned int allocSize = (singleSelection || (!forOpen)) ? MAX_PATH : (1024 * 32);
	wchar_t* pathBuf = new wchar_t[allocSize];
	ZeroMemory(pathBuf, sizeof(wchar_t) * allocSize);
	wchar_t* tmpFilter = NULL;

	OPENFILENAME OFN;
	ZeroMemory(&OFN, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = owner;
	OFN.lpstrFile = pathBuf;
	if (forOpen)
	{
		OFN.Flags = (singleSelection) ? 0 : (OFN_EXPLORER | OFN_ALLOWMULTISELECT);
	}
	else
	{
		OFN.Flags = (OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT);
	}
	OFN.nMaxFile = 2048;

	if (extensionList.Empty())
	{
		OFN.lpstrFilter = L"All Files\0*.*\0";
	}
	else
	{
		MkStr extFilter;
		extFilter.Reserve(extensionList.GetSize() * 128); // ext 하나당 128이면 충분
		if (forOpen)
		{
			extFilter += L"Files (";
			MK_INDEXING_LOOP(extensionList, i)
			{
				if (i > 0)
				{
					extFilter += L", ";
				}
				extFilter += extensionList[i];
			}
			extFilter += L")|";

			MK_INDEXING_LOOP(extensionList, i)
			{
				if (i > 0)
				{
					extFilter += L";";
				}
				extFilter += L"*.";
				extFilter += extensionList[i];
			}
			extFilter += L"|";
		}
		else
		{
			MK_INDEXING_LOOP(extensionList, i)
			{
				extFilter += extensionList[i];
				extFilter += L" Files|*.";
				extFilter += extensionList[i];
				extFilter += L"|";
			}
		}

		unsigned int bufSize = extFilter.GetSize() + 1;
		tmpFilter = new wchar_t[bufSize];
		ZeroMemory(tmpFilter, sizeof(wchar_t) * bufSize);
		wcscpy_s(tmpFilter, bufSize, extFilter.GetPtr());
		for (unsigned int i=0; i<extFilter.GetSize(); ++i)
		{
			if (tmpFilter[i] == '|')
			{
				tmpFilter[i] = NULL;
			}
		}

		OFN.lpstrFilter = tmpFilter;
	}

	unsigned int fileCount = 0;
	if (((forOpen) ? GetOpenFileName(&OFN) : GetSaveFileName(&OFN)) != 0)
	{
		if (singleSelection || (!forOpen))
		{
			MkPathName resultPath = pathBuf;
			unsigned int resultCount = 1;

			// 저장 모드이고 확장자가 존재하는 경우 유효성 검사
			if ((!forOpen) && (!extensionList.Empty()))
			{
				if (resultPath.IsDirectoryPath()) // ex> L"C:\abc\filename"
				{
					resultPath += L".";
					resultPath += extensionList[OFN.nFilterIndex - 1];
				}
				else
				{
					MkStr ext = resultPath.GetFileExtension();
					if (ext.Empty())
					{
						resultCount = 0;
					}
					else
					{
						ext.ToLower();

						const MkStr& targetExt = extensionList[OFN.nFilterIndex - 1];
						MkStr targetLower = targetExt;
						targetLower.ToLower();
						
						if (ext != targetLower)
						{
							resultPath.ChangeFileExtension(targetExt);
						}
					}
				}
			}

			resultPath.ToLower(0); // 드라이브명 대문자->소문자
			directoryPath = resultPath;
			fileCount = resultCount;
		}
		else
		{
			MkPathName path = pathBuf;
			path.ToLower(0); // 드라이브명 대문자->소문자
			unsigned int offset = path.GetSize() + 1;

			while ((offset < allocSize) && (pathBuf[offset] != NULL))
			{
				offset += wcslen(&pathBuf[offset]) + 1;
				++fileCount;
			}

			if (fileCount == 0)
			{
				directoryPath = path.GetPath();
				fileNameList.Reserve(1);
				fileNameList.PushBack(path.GetFileName());
				fileCount = 1;
			}
			else
			{
				directoryPath = path;
				directoryPath.CheckAndAddBackslash();

				fileNameList.Reserve(fileCount);

				offset = path.GetSize() + 1;
				while ((offset < allocSize) && (pathBuf[offset] != NULL))
				{
					MkPathName fileName = &pathBuf[offset];
					fileNameList.PushBack(fileName);
					offset += fileName.GetSize() + 1;
				}
			}
		}
	}

	MK_DELETE_ARRAY(pathBuf);
	MK_DELETE_ARRAY(tmpFilter);

	return fileCount;
}

void MkPathName::_GetSubNodesByTag(const MkDataNode& node, MkArray<MkHashStr>& subNodes, const MkHashStr& countTag, const MkHashStr& typeTag, bool exist)
{
	unsigned int count = 0;
	node.GetData(countTag, count, 0);
	if (count > 0)
	{
		subNodes.Reserve(count);

		MkArray<MkHashStr> keys;
		node.GetChildNodeList(keys);
		MK_INDEXING_LOOP(keys, i)
		{
			const MkHashStr& currKey = keys[i];
			bool hasTag = node.GetChildNode(currKey)->IsValidKey(typeTag);
			if ((exist) ? (hasTag) : (!hasTag))
			{
				subNodes.PushBack(currKey);
			}
		}
	}
}

void MkPathName::_IncreaseUnitCountByTag(MkDataNode& node, const MkHashStr& key)
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

