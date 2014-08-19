
#include <ShlObj.h>
#include <atltime.h>
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkInterfaceForFileReading.h"

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
	// fileSizeInKB : 총 파일 크기 반환(KB)
	// includeSubDirectory : 하위 폴더 탐색 여부
	unsigned int ScoutDirectory(const MkPathName& startPath, unsigned int& fileSizeInKB, bool includeSubDirectory) const
	{
		if (startPath.IsDirectoryPath())
		{
			MkPathName targetPath;
			targetPath.ConvertToRootBasisAbsolutePath(startPath);
			if (targetPath.CheckAvailable())
			{
				unsigned int fileCount = 0;
				unsigned int fileSize = 0;
				_ScoutDirectory(targetPath, fileCount, fileSize, includeSubDirectory);
				fileSizeInKB = fileSize;
				return fileCount;
			}
		}
		fileSizeInKB = 0;
		return 0;
	}

protected:
	
	// currentDirectoryPath는 존재하는 디렉토리 절대 경로
	void _ScoutDirectory(const MkPathName& currentDirectoryPath, unsigned int& fileCount, unsigned int& fileSize, bool includeSubDirectory) const
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
					++fileCount;
					if (fd.nFileSizeLow > 0)
					{
						fileSize += fd.nFileSizeLow / 1024;
					}
				}
				else if (includeSubDirectory) // 디렉토리
				{
					MkPathName targetName = fd.cFileName;
					if ((targetName != L".") && (targetName != L".."))
					{
						MkPathName targetPath = currentDirectoryPath;
						targetPath += targetName;
						targetPath += L"\\";
						_ScoutDirectory(targetPath, fileCount, fileSize, includeSubDirectory); // 재귀
					}
				}
			}
		}
		while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
};

// 파일, 디렉토리 삭제
// 디렉토리 속성에 대해서는 (http://benjaminlog.com/entry/directory-readonly-attribute) 참조
class __DirectoryDestroyer
{
public:

	// startPath : 탐색을 시작할 디렉토리 경로
	// deleteReadOnly : read-only 파일 삭제 여부
	bool DestroyDirectory(const MkPathName& startPath) const
	{
		if (!startPath.IsDirectoryPath())
			return false;

		MkPathName targetPath;
		targetPath.ConvertToRootBasisAbsolutePath(startPath);
		if (targetPath.CheckAvailable())
		{
			_DestroyDirectory(targetPath);
			return (!targetPath.CheckAvailable());
		}
		return true;
	}

protected:

	// currentDirectoryPath는 존재하는 디렉토리 절대 경로
	void _DestroyDirectory(const MkPathName& currentDirectoryPath) const
	{
		// 최초 핸들 얻음
		WIN32_FIND_DATA fd;
		MkStr pathForHandling = currentDirectoryPath;
		pathForHandling += L"*.*";
		HANDLE hFind = FindFirstFile(pathForHandling.GetPtr(), &fd);
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
					if (((fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0) || ((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0))
					{
						SetFileAttributes(targetPath.GetPtr(), FILE_ATTRIBUTE_NORMAL);
					}
					DeleteFile(targetPath.GetPtr());
				}
				else // 디렉토리
				{
					if ((targetName != L".") && (targetName != L".."))
					{
						targetPath += L"\\";
						_DestroyDirectory(targetPath); // 재귀
					}
				}
			}
			while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}

		RemoveDirectory(currentDirectoryPath.GetPtr());
	}
};

// 기본 콜렉션. 모든 파일 검색
class __BaseFileCollection
{
public:

	// fileNameList : 검색된 파일 경로들이 담길 버퍼. 가능하면 충분히 Reserve 할 것
	// startPath : 탐색을 시작할 디렉토리 경로
	// includeSubDirectory : 하위 폴더 탐색 여부
	void GetFileList(MkArray<MkPathName>& fileNameList, const MkPathName& startPath, bool includeSubDirectory, bool includeZeroSizeFile) const
	{
		if (startPath.IsDirectoryPath())
		{
			MkPathName targetPath;
			targetPath.ConvertToRootBasisAbsolutePath(startPath);
			if (targetPath.CheckAvailable())
			{
				_GetFileListInDirectory(fileNameList, targetPath, L"", includeSubDirectory, includeZeroSizeFile);
			}
		}
	}

protected:
	
	virtual bool _CheckAvailable(const MkPathName& name) const { return true; }

	void _GetFileListInDirectory
		(MkArray<MkPathName>& fileNameList, const MkPathName& startPath, const MkPathName& offset, bool includeSubDirectory, bool includeZeroSizeFile) const
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
			// 히든 속성 제외
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0)
			{
				MkPathName targetName = fd.cFileName;

				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) // 파일
				{
					if (_CheckAvailable(targetName))
					{
						if (includeZeroSizeFile || ((fd.nFileSizeLow > 0) || (fd.nFileSizeHigh > 0)))
						{
							MkPathName targetPath = offset;
							targetPath += targetName;
							fileNameList.PushBack(targetPath);
						}
					}
				}
				else if (includeSubDirectory) // 디렉토리
				{
					if ((targetName != L".") && (targetName != L".."))
					{
						MkPathName newOffset;
						newOffset.Reserve(offset.GetSize() + targetName.GetSize() + 1);
						newOffset += offset;
						newOffset += targetName;
						newOffset += L"\\";
						_GetFileListInDirectory(fileNameList, startPath, newOffset, includeSubDirectory, includeZeroSizeFile); // 재귀
					}
				}
			}
		}
		while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
};

// 필터링 콜렉션
class __FileCollectionWithFiltering : public __BaseFileCollection
{
public:

	void GetFileList
		(MkArray<MkPathName>& fileNameList, const MkPathName& startPath,
		const MkArray<MkPathName>& exceptionFilter, const MkArray<MkPathName>& nameFilter,
		const MkArray<MkPathName>& extensionFilter, const MkArray<MkStr>& prefixFilter,
		bool includeSubDirectory, bool includeZeroSizeFile)
	{
		m_OnExceptionFilter = _ConvertArrayToHashMap(exceptionFilter, m_ExceptionFilter);
		m_OnNameFilter = _ConvertArrayToHashMap(nameFilter, m_NameFilter);
		m_OnExtensionFilter = _ConvertArrayToHashMap(extensionFilter, m_ExtensionFilter);
		m_OnPrefixFilter = _ConvertArrayToArray(prefixFilter, m_PrefixFilter);

		__BaseFileCollection::GetFileList(fileNameList, startPath, includeSubDirectory, includeZeroSizeFile);
	}

	__FileCollectionWithFiltering(const bool& passingResult) : __BaseFileCollection()
	{
		m_OnExceptionFilter = false;
		m_OnNameFilter = false;
		m_OnExtensionFilter = false;
		m_OnPrefixFilter = false;
		m_SkipFiltering = true;
		m_PassingResult = passingResult;
	}

protected:

	// 필터링 대상 파일 수에 비례하여 검색 횟수가 늘어나므로 HashMap<>을 사용해 검색 효율 증대
	bool _ConvertArrayToHashMap(const MkArray<MkPathName>& source, MkHashMap<MkHashStr, int>& target)
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
			m_SkipFiltering = false;
		}
		return turnOn;
	}

	bool _ConvertArrayToArray(const MkArray<MkStr>& source, MkArray<MkStr>& target)
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
			m_SkipFiltering = false;
		}
		return turnOn;
	}

	virtual bool _CheckAvailable(const MkPathName& name) const
	{
		if (m_SkipFiltering)
			return !m_PassingResult;

		MkPathName tmpName = name;
		tmpName.ToLower();

		if (m_OnExceptionFilter || m_OnNameFilter)
		{
			MkHashStr hashName(tmpName);
			if (m_OnExceptionFilter)
			{
				if (m_ExceptionFilter.Exist(hashName))
					return !m_PassingResult;
			}

			if (m_OnNameFilter)
			{
				if (m_NameFilter.Exist(hashName))
					return m_PassingResult;
			}
		}

		if (m_OnExtensionFilter)
		{
			if (m_ExtensionFilter.Exist(MkHashStr(tmpName.GetFileExtension())))
				return m_PassingResult;
		}

		if (m_OnPrefixFilter)
		{
			MK_INDEXING_LOOP(m_PrefixFilter, i)
			{
				if (tmpName.CheckPrefix(m_PrefixFilter[i]))
					return m_PassingResult;
			}
		}

		return !m_PassingResult;
	}

protected:

	MkHashMap<MkHashStr, int> m_ExceptionFilter;
	MkHashMap<MkHashStr, int> m_NameFilter;
	MkHashMap<MkHashStr, int> m_ExtensionFilter;
	MkArray<MkStr> m_PrefixFilter;

	bool m_OnExceptionFilter;
	bool m_OnNameFilter;
	bool m_OnExtensionFilter;
	bool m_OnPrefixFilter;
	bool m_SkipFiltering;

	bool m_PassingResult;
};

// white colletion : 필터링 통과한 파일만 검색
class __WhiteFileCollection : public __FileCollectionWithFiltering
{
public:
	__WhiteFileCollection() : __FileCollectionWithFiltering(true) {}
};

// black colletion : 필터링 통과 못 한 파일만 검색
class __BlackFileCollection : public __FileCollectionWithFiltering
{
public:
	__BlackFileCollection() : __FileCollectionWithFiltering(false) {}
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
MkPathName::MkPathName(const MkPathName& str) : MkStr(str) {}
MkPathName::MkPathName(const MkStr& str) : MkStr(str) {}
MkPathName::MkPathName(const MkHashStr& str) : MkStr(str) {}
MkPathName::MkPathName(const std::wstring& str) : MkStr(str) {}
MkPathName::MkPathName(const wchar_t* wcharArray) : MkStr(wcharArray) {}

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

unsigned int MkPathName::GetFileCount(bool includeSubDirectory) const
{
	__DirectoryScouter scouter;
	unsigned int fileSize;
	return scouter.ScoutDirectory(*this, fileSize, includeSubDirectory);
}

unsigned int MkPathName::GetFileCountAndSize(unsigned int& fileSizeInKB, bool includeSubDirectory) const
{
	__DirectoryScouter scouter;
	return scouter.ScoutDirectory(*this, fileSizeInKB, includeSubDirectory);
}

void MkPathName::GetFileList(MkArray<MkPathName>& filePathList, bool includeSubDirectory, bool includeZeroSizeFile) const
{
	__BaseFileCollection interfaceInstance;
	interfaceInstance.GetFileList(filePathList, *this, includeSubDirectory, includeZeroSizeFile);
}

void MkPathName::GetWhiteFileList
(MkArray<MkPathName>& filePathList, const MkArray<MkPathName>& nameFilter, const MkArray<MkPathName>& extensionFilter,
 const MkArray<MkStr>& prefixFilter, const MkArray<MkPathName>& exceptionFilter, bool includeSubDirectory, bool includeZeroSizeFile) const
{
	__WhiteFileCollection interfaceInstance;
	interfaceInstance.GetFileList(filePathList, *this, exceptionFilter, nameFilter, extensionFilter, prefixFilter, includeSubDirectory, includeZeroSizeFile);
}

void MkPathName::GetBlackFileList
(MkArray<MkPathName>& filePathList, const MkArray<MkPathName>& nameFilter, const MkArray<MkPathName>& extensionFilter,
 const MkArray<MkStr>& prefixFilter, const MkArray<MkPathName>& exceptionFilter, bool includeSubDirectory, bool includeZeroSizeFile) const
{
	__BlackFileCollection interfaceInstance;
	interfaceInstance.GetFileList(filePathList, *this, exceptionFilter, nameFilter, extensionFilter, prefixFilter, includeSubDirectory, includeZeroSizeFile);
}

void MkPathName::GetIndexFormFileList
(MkMap<unsigned int, MkPathName>& filePathTable, const MkStr& prefix, const MkStr& extension, bool includeZeroSizeFile) const
{
	if (prefix.Empty() || extension.Empty())
		return;

	// 먼저 prefix, extension으로 white filtering
	MkArray<MkPathName> filePathList;
	filePathList.Reserve(256);
	MkArray<MkPathName> emptyFilter;
	MkArray<MkPathName> extensionFilter;
	extensionFilter.PushBack(extension);
	MkArray<MkStr> prefixFilter;
	prefixFilter.PushBack(prefix);
	GetWhiteFileList(filePathList, emptyFilter, extensionFilter, prefixFilter, emptyFilter, false, includeZeroSizeFile);
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

unsigned int MkPathName::GetWrittenTime(void) const
{
	if (!CheckAvailable())
		return MKDEF_ARRAY_ERROR;

	HANDLE fileHandle = CreateFile(m_Str.GetPtr(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE)
		return MKDEF_ARRAY_ERROR;

	FILETIME create_time, access_time, write_time;
	GetFileTime(fileHandle, &create_time, &access_time, &write_time);
	SYSTEMTIME write_system_time, write_local_time;
	FileTimeToSystemTime(&write_time, &write_system_time);
	SystemTimeToTzSpecificLocalTime(NULL, &write_system_time, &write_local_time);
    CloseHandle(fileHandle);

	if ((write_local_time.wYear < 2000) || (write_local_time.wYear > 2135))
		return MKDEF_ARRAY_ERROR;

	/*
	// CTime 이라는 것이 있는 줄 모르고 만든 수동계산
	// 값은 정확히 맞는데... 무식하면 몸이 피곤하다는 속담의 산 증거...
	// 만약 atltime을 사용하기 싫다면 이 쪽을 대신 사용 할 것
	// 그레고리력 기준으로 계산
	// http://ko.wikipedia.org/wiki/%EC%9C%A4%EB%85%84
	// 2000 ~ 2136.2 범위임으로 2100년을 제외한 년도는 4로 나누어 떨어지면 윤년이라는 점을 주의할 것
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
	CTime ctime(write_local_time.wYear, write_local_time.wMonth, write_local_time.wDay,
		write_local_time.wHour, write_local_time.wMinute, write_local_time.wSecond, 0);

	__time64_t currentSecs = ctime.GetTime();

	// 2000년 기준임으로 1970 ~ 2000.1.1 00:00:00에 해당하는 초를 제외
	// CTime c2000(2000, 1, 1, 0, 0, 0, 0) >> c2000.GetTime() == 946652400;
	__time64_t secsFrom2000 = currentSecs - static_cast<__time64_t>(946652400);
	return static_cast<unsigned int>(secsFrom2000);
}

//------------------------------------------------------------------------------------------------//

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

bool MkPathName::DeleteCurrentDirectory(void) const
{
	__DirectoryDestroyer destroyer;
	return destroyer.DestroyDirectory(*this);
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

bool MkPathName::MoveCurrentFile(const MkPathName& newPath)
{
	if (newPath.Empty() || newPath.IsDirectoryPath())
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

			if (!targetPath.CheckAvailable())
			{
				if (!targetPath.MakeDirectoryPath())
					return false;
			}

			targetPath += tmpPath.GetFileName();
		}

		bool ok = (MoveFile(tmpPath.GetPtr(), targetPath.GetPtr()) != 0);
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

unsigned int MkPathName::_GetExtensionPosition(void) const
{
	if (Empty())
		return 0;

	unsigned int size = GetSize();
	const wchar_t& lastChar = m_Str[size - 1];
	if (lastChar == L'\\')
		return 0; // 경로명. ex> "abc\\"

	if (lastChar == L'.')
		return MKDEF_ARRAY_ERROR; // '.'으로 종료, error. ex> "abc."

	if (size == 1)
		return 0; // 경로명. ex> "a"

	for (unsigned int i = size-2; i >= 0; --i)
	{
		const wchar_t& currChar = m_Str[i];
		if (currChar == L'\\')
			return 0; // 경로명. ex> "abc\\def"

		if (currChar == L'.')
		{
			if (i == 0)
				return MKDEF_ARRAY_ERROR; // 파일명 없음, error. ex> ".ext"

			const wchar_t& nameChar = m_Str[i-1];
			if (nameChar == L'\\')
				return MKDEF_ARRAY_ERROR; // 파일명 없음, error. ex> "\\.ext"

			if (nameChar == L'.')
				return MKDEF_ARRAY_ERROR; // dot 연속은 인정하지 않음, error. ex> "..ext"
			
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
		argBuf += L" ";
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

//------------------------------------------------------------------------------------------------//

