
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
// ���丮 ��ȸ�� ��ü
//------------------------------------------------------------------------------------------------//

// �⺻ ���� �˻�
class __DirectoryScouter
{
public:

	// startPath : Ž���� ������ ���丮 ���
	// fileSizeInKB : �� ���� ũ�� ��ȯ(KB)
	// includeSubDirectory : ���� ���� Ž�� ����
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
	
	// currentDirectoryPath�� �����ϴ� ���丮 ���� ���
	void _ScoutDirectory(const MkPathName& currentDirectoryPath, unsigned int& fileCount, unsigned int& fileSize, bool includeSubDirectory) const
	{
		// ���� �ڵ� ����
		WIN32_FIND_DATA fd;
		MkStr pathForHandling = currentDirectoryPath;
		pathForHandling += L"*.*";
		HANDLE hFind = FindFirstFile(pathForHandling.GetPtr(), &fd);
		if (hFind == INVALID_HANDLE_VALUE)
			return;

		// �ڵ� �˻�
		do
		{
			// ���� �Ӽ� ����
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0)
			{
				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) // ����
				{
					++fileCount;
					if (fd.nFileSizeLow > 0)
					{
						fileSize += fd.nFileSizeLow / 1024;
					}
				}
				else if (includeSubDirectory) // ���丮
				{
					MkPathName targetName = fd.cFileName;
					if ((targetName != L".") && (targetName != L".."))
					{
						MkPathName targetPath = currentDirectoryPath;
						targetPath += targetName;
						targetPath += L"\\";
						_ScoutDirectory(targetPath, fileCount, fileSize, includeSubDirectory); // ���
					}
				}
			}
		}
		while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
};

// ����, ���丮 ����
// ���丮 �Ӽ��� ���ؼ��� (http://benjaminlog.com/entry/directory-readonly-attribute) ����
class __DirectoryDestroyer
{
public:

	// startPath : Ž���� ������ ���丮 ���
	// deleteReadOnly : read-only ���� ���� ����
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

	// currentDirectoryPath�� �����ϴ� ���丮 ���� ���
	void _DestroyDirectory(const MkPathName& currentDirectoryPath) const
	{
		// ���� �ڵ� ����
		WIN32_FIND_DATA fd;
		MkStr pathForHandling = currentDirectoryPath;
		pathForHandling += L"*.*";
		HANDLE hFind = FindFirstFile(pathForHandling.GetPtr(), &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			// �ڵ� �˻�
			do
			{
				MkPathName targetName = fd.cFileName;
				MkPathName targetPath = currentDirectoryPath;
				targetPath += targetName;
				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) // ����
				{
					if (((fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0) || ((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0))
					{
						SetFileAttributes(targetPath.GetPtr(), FILE_ATTRIBUTE_NORMAL);
					}
					DeleteFile(targetPath.GetPtr());
				}
				else // ���丮
				{
					if ((targetName != L".") && (targetName != L".."))
					{
						targetPath += L"\\";
						_DestroyDirectory(targetPath); // ���
					}
				}
			}
			while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}

		RemoveDirectory(currentDirectoryPath.GetPtr());
	}
};

// �⺻ �ݷ���. ��� ���� �˻�
class __BaseFileCollection
{
public:

	// fileNameList : �˻��� ���� ��ε��� ��� ����. �����ϸ� ����� Reserve �� ��
	// startPath : Ž���� ������ ���丮 ���
	// includeSubDirectory : ���� ���� Ž�� ����
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
		// ���� �ڵ� ����
		WIN32_FIND_DATA fd;
		MkStr pathForHandling;
		pathForHandling.Reserve(startPath.GetSize() + offset.GetSize() + 3);
		pathForHandling += startPath;
		pathForHandling += offset;
		pathForHandling += L"*.*";
		HANDLE hFind = FindFirstFile(pathForHandling.GetPtr(), &fd);
		if (hFind == INVALID_HANDLE_VALUE)
			return;

		// �ڵ� �˻�
		do
		{
			// ���� �Ӽ� ����
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0)
			{
				MkPathName targetName = fd.cFileName;

				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) // ����
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
				else if (includeSubDirectory) // ���丮
				{
					if ((targetName != L".") && (targetName != L".."))
					{
						MkPathName newOffset;
						newOffset.Reserve(offset.GetSize() + targetName.GetSize() + 1);
						newOffset += offset;
						newOffset += targetName;
						newOffset += L"\\";
						_GetFileListInDirectory(fileNameList, startPath, newOffset, includeSubDirectory, includeZeroSizeFile); // ���
					}
				}
			}
		}
		while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
};

// ���͸� �ݷ���
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

	// ���͸� ��� ���� ���� ����Ͽ� �˻� Ƚ���� �þ�Ƿ� HashMap<>�� ����� �˻� ȿ�� ����
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

// white colletion : ���͸� ����� ���ϸ� �˻�
class __WhiteFileCollection : public __FileCollectionWithFiltering
{
public:
	__WhiteFileCollection() : __FileCollectionWithFiltering(true) {}
};

// black colletion : ���͸� ��� �� �� ���ϸ� �˻�
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
	if ((GetSize() > 2) && (m_Str[1] == L':') && (m_Str[2] == L'\\')) // ���� ����̹� ������ �˻�
		return true;

	return CheckPrefix(L"\\\\"); // ��Ʈ��ũ ������� �˻�
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
			tmpName.Reserve(tmpName.GetSize() + tmpExt.GetSize() + 1); // IsDirectoryPath() �˻翡�� Ȯ���ڰ� �������� ����
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

	// ���� prefix, extension���� white filtering
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

		MkPathName path; // ���ϰ�θ� �Ľ��� �̸� ����
		MkStr name, extension;
		filePath.SplitPath(path, name, extension);

		MkStr indexPart; // �̸����� index ��Ʈ �и�
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
	// CTime �̶�� ���� �ִ� �� �𸣰� ���� �������
	// ���� ��Ȯ�� �´µ�... �����ϸ� ���� �ǰ��ϴٴ� �Ӵ��� �� ����...
	// ���� atltime�� ����ϱ� �ȴٸ� �� ���� ��� ��� �� ��
	// �׷����� �������� ���
	// http://ko.wikipedia.org/wiki/%EC%9C%A4%EB%85%84
	// 2000 ~ 2136.2 ���������� 2100���� ������ �⵵�� 4�� ������ �������� �����̶�� ���� ������ ��
	unsigned int thisYear = static_cast<unsigned int>(write_local_time.wYear) - 2000;
	unsigned int lastLeapYearCount = (thisYear == 0) ? 0 : ((thisYear - 1) / 4 + (((thisYear - 1) < 100) ? 1 : 0)); // �۳������ ���� ��(ex> 2011���� 3��(00, 04, 08), 2111���� 25��)
	unsigned int daysTillLastYear = thisYear * 365 + lastLeapYearCount; // �۳������ �ϼ�
	bool cl = ((thisYear != 100) && ((thisYear % 4) == 0)); // ���ذ� �����̸�
	unsigned int daysForLastMonth[12] = {0, 31, cl?60:59, cl?91:90, cl?121:120, cl?152:151, cl?182:181, cl?213:212, cl?244:243, cl?274:273, cl?305:304, cl?335:334};
	unsigned int month = static_cast<unsigned int>(write_local_time.wMonth);
	unsigned int day = static_cast<unsigned int>(write_local_time.wDay);
	unsigned int daysOfThisYear = daysForLastMonth[month - 1] + day - 1; // ���������� ���� �ϼ�
	unsigned int secsTillYesterday = (daysTillLastYear + daysOfThisYear) * 86400; // ���������� �ʼ�
	unsigned int secsOfToday = // ������ �ʼ�
		static_cast<unsigned int>(write_local_time.wHour) * 3600 +
		static_cast<unsigned int>(write_local_time.wMinute) * 60 +
		static_cast<unsigned int>(write_local_time.wSecond);

	return (secsTillYesterday + secsOfToday);
	*/

	// �ڵ� ���(CTime �̿�)
	// CTime�� ������ 1970.1.1 ~ 2038.12.31�̶� �� ���͸����� �ڵ����� �ɷ����Ƿ� �Ƚ��ϰ� ��� ����
	CTime ctime(write_local_time.wYear, write_local_time.wMonth, write_local_time.wDay,
		write_local_time.wHour, write_local_time.wMinute, write_local_time.wSecond, 0);

	__time64_t currentSecs = ctime.GetTime();

	// 2000�� ���������� 1970 ~ 2000.1.1 00:00:00�� �ش��ϴ� �ʸ� ����
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
			assert(pathCount > 0); // �����ΰ� ��� ����
			
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
			assert(index > 0); // ��ġ����� �������� ����
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
		return 0; // ��θ�. ex> "abc\\"

	if (lastChar == L'.')
		return MKDEF_ARRAY_ERROR; // '.'���� ����, error. ex> "abc."

	if (size == 1)
		return 0; // ��θ�. ex> "a"

	for (unsigned int i = size-2; i >= 0; --i)
	{
		const wchar_t& currChar = m_Str[i];
		if (currChar == L'\\')
			return 0; // ��θ�. ex> "abc\\def"

		if (currChar == L'.')
		{
			if (i == 0)
				return MKDEF_ARRAY_ERROR; // ���ϸ� ����, error. ex> ".ext"

			const wchar_t& nameChar = m_Str[i-1];
			if (nameChar == L'\\')
				return MKDEF_ARRAY_ERROR; // ���ϸ� ����, error. ex> "\\.ext"

			if (nameChar == L'.')
				return MKDEF_ARRAY_ERROR; // dot ������ �������� ����, error. ex> "..ext"
			
			return (i + 1); // Ȯ���� ��ġ ��ȯ
		}
	}
	return 0; // ��θ�. ex> "abc"
}

void MkPathName::_UpdateCurrentPath(const MkPathName& basePath, const MkPathName& sourcePath)
{
	if (sourcePath.Empty())
	{
		*this = basePath; // basePath�� �׻� ����ȭ�Ǿ� �����Ƿ� �� �̻��� �۾� ���ʿ�
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

		OptimizePath(); // sourcePath�� ����ȭ �ʿ��ϹǷ� ������ ��ȯ �� ����ȭ
	}
}

bool MkPathName::_CheckOnSameDevice(const MkPathName& basePath, const MkPathName& targetPath) const
{
	const wchar_t& b0 = basePath.GetAt(0);
	const wchar_t& b1 = basePath.GetAt(1);
	if ((b0 == targetPath.GetAt(0)) && (b1 == targetPath.GetAt(1))) // ó�� �� ���ڰ� ��ġ
	{
		// ��Ʈ��ũ ����� ��� ��ġ�� Ȯ��
		if ((b0 == L'\\') && (b1 == L'\\')) 
		{
			// ��ġ�� ���� Ȯ��
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
			return false; // ��ġ�� ũ�Ⱑ �ٸ�
		}

		// ������ ��� ����̺�� ��ġ�� ������ true
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
			PopFront(basePath.GetSize()); // ������ basePath ����
		}
		else
		{
			if (_CheckOnSameDevice(basePath, *this))
			{
				MkArray<MkStr> baseTokens; // basePath ����
				unsigned int baseCount = basePath.Tokenize(baseTokens, L"\\");

				MkArray<MkStr> currTokens; // ������ ����
				unsigned int currCount = Tokenize(currTokens, L"\\");

				unsigned int index = 1; // ��ġ���� ��ġ�ϹǷ� ���� ���(��ġ��)�� ����
				while ((index < baseCount) && (index < currCount)) // �� ��ΰ� ���ʷ� �޶����� ���� ����
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

				// ������ basePath ������ �ƴѵ��� ��� ������ ��ġ�ϴ� ���� �� ��ΰ� Ȯ���� ���� ���ϸ��� ���̽�
				// ex> basePath == L"d:\\a\\b\\", currPath == L"d:\\a\\b
				if (baseCount == index)
				{
					assert(baseCount > 1); // upstair Ž���� �����Ϸ��� �ּ��� ��ġ�� ����, ������ 2�� �̻��̾�� ��

					Flush();
					*this += L"..\\";
					*this += currTokens[currCount - 1];
				}
				else
				{
					unsigned int upstairCount = baseCount - index; // ���� ���� ��
					if (upstairCount > index) // upstairCount��ŭ L"..\\"�� ���� ���̹Ƿ� ������ �������� �߰��� ������ ������ Ȯ���� ������ ����
					{
						Reserve(GetSize() + upstairCount * 3);
					}

					bool directoryPath = IsDirectoryPath();

					Flush(); // ����
					AddRepetition(L"..\\", upstairCount); // upstairCount��ŭ L"..\\"�� ����

					if (currCount > index) // �ܿ� ������ ����
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

	// ��ŷ ���丮 ����
	SetWorkingDirectory(tmpPath.GetPath());

	// ���ڰ� �����ϸ� �߰�
	MkStr argBuf;
	if (!argument.Empty())
	{
		argBuf.Reserve(tmpPath.GetSize() + 1 + argument.GetSize());
		argBuf += tmpPath;
		argBuf += L" ";
		argBuf += argument;
	}

	// ����
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
		extFilter.Reserve(extensionList.GetSize() * 128); // ext �ϳ��� 128�̸� ���
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

			// ���� ����̰� Ȯ���ڰ� �����ϴ� ��� ��ȿ�� �˻�
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

			resultPath.ToLower(0); // ����̺�� �빮��->�ҹ���
			directoryPath = resultPath;
			fileCount = resultCount;
		}
		else
		{
			MkPathName path = pathBuf;
			path.ToLower(0); // ����̺�� �빮��->�ҹ���
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

