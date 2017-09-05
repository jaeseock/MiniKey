
#include <windows.h>
#include <shlguid.h>
#include <shobjidl.h>
#include <shlobj.h>
#include "MkCore_MkShortcutOp.h"


//------------------------------------------------------------------------------------------------//

bool MkShortcutOperator::CreateShortcut
(const MkPathName& locationPath, const MkPathName& targetPath, const MkStr& desc, const MkPathName& iconFilePath)
{
	if (locationPath.Empty() || targetPath.Empty())
		return false;

	if (::CoInitialize(NULL) != S_OK)
		return false;

	bool rlt = false;
	do
	{
		IShellLink* slInterface = NULL;
		if (FAILED(::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&slInterface)))
			break;

		slInterface->SetPath(targetPath.GetPtr());
		slInterface->SetDescription(desc.Empty() ? NULL : desc.GetPtr());

		if (!iconFilePath.Empty())
		{
			MkPathName iconFullPath;
			iconFullPath.ConvertToModuleBasisAbsolutePath(iconFilePath);
			if (iconFullPath.CheckAvailable())
			{
				slInterface->SetIconLocation(iconFullPath.GetPtr(), 0);
			}
		}

		IPersistFile* pfInterface = NULL;
		if (SUCCEEDED(slInterface->QueryInterface(IID_IPersistFile, (LPVOID*)&pfInterface)))
		{
			MkPathName locationFullPath;
			locationFullPath.ConvertToModuleBasisAbsolutePath(locationPath);
			rlt = SUCCEEDED(pfInterface->Save(locationFullPath.GetPtr(), TRUE));
			pfInterface->Release();
		}
		slInterface->Release();
	}
	while (false);

	::CoUninitialize();
	return rlt;
}

//------------------------------------------------------------------------------------------------//

static void __MakeLocationPath(const MkPathName& shortcutFilePath, int csidl, MkPathName& locationPath)
{
	MkPathName fpCopy = (shortcutFilePath.GetFileExtension() == MkStr::EMPTY) ? (shortcutFilePath + L".lnk") : shortcutFilePath;
	locationPath.ConvertToSystemBasisAbsolutePath(fpCopy, csidl);
}

static bool __CreateShortcutToSystemDirectory
(const MkPathName& shortcutFilePath, const MkPathName& targetPath, const MkStr& desc, const MkPathName& iconFilePath, int csidl)
{
	if (shortcutFilePath.Empty() || targetPath.Empty())
		return false;

	MkPathName locationPath;
	__MakeLocationPath(shortcutFilePath, csidl, locationPath);

	if (!locationPath.GetPath().MakeDirectoryPath())
		return false;

	return MkShortcutOperator::CreateShortcut(locationPath, targetPath, desc, iconFilePath);
}

static bool __DeleteShortcutInSystemDirectory(const MkPathName& shortcutFilePath, int csidl)
{
	if (shortcutFilePath.Empty())
		return false;

	MkPathName locationPath;
	__MakeLocationPath(shortcutFilePath, csidl, locationPath);
	return locationPath.DeleteCurrentFile();
}

static bool __DeleteDirectoryInSystemDirectory(const MkPathName& dirPath, int csidl)
{
	if (dirPath.Empty() || (!dirPath.IsDirectoryPath()))
		return false;

	MkPathName locationPath;
	locationPath.ConvertToSystemBasisAbsolutePath(dirPath, csidl);
	return locationPath.DeleteCurrentDirectory(true);
}

//------------------------------------------------------------------------------------------------//

bool MkShortcutOperator::CreateShortcutToCommonDesktopDirectory
(const MkPathName& shortcutFilePath, const MkPathName& targetPath, const MkStr& desc, const MkPathName& iconFilePath)
{
	return __CreateShortcutToSystemDirectory(shortcutFilePath, targetPath, desc, iconFilePath, CSIDL_COMMON_DESKTOPDIRECTORY);
}

bool MkShortcutOperator::DeleteShortcutInCommonDesktopDirectory(const MkPathName& shortcutFilePath)
{
	return __DeleteShortcutInSystemDirectory(shortcutFilePath, CSIDL_COMMON_DESKTOPDIRECTORY);
}

bool MkShortcutOperator::DeleteDirectoryInCommonDesktopDirectory(const MkPathName& dirPath)
{
	return __DeleteDirectoryInSystemDirectory(dirPath, CSIDL_COMMON_DESKTOPDIRECTORY);
}

bool MkShortcutOperator::CreateShortcutToCommonPrograms
(const MkPathName& shortcutFilePath, const MkPathName& targetPath, const MkStr& desc, const MkPathName& iconFilePath)
{
	return __CreateShortcutToSystemDirectory(shortcutFilePath, targetPath, desc, iconFilePath, CSIDL_COMMON_PROGRAMS);
}

bool MkShortcutOperator::DeleteShortcutInCommonPrograms(const MkPathName& shortcutFilePath)
{
	return __DeleteShortcutInSystemDirectory(shortcutFilePath, CSIDL_COMMON_PROGRAMS);
}

bool MkShortcutOperator::DeleteDirectoryInCommonPrograms(const MkPathName& dirPath)
{
	return __DeleteDirectoryInSystemDirectory(dirPath, CSIDL_COMMON_PROGRAMS);
}

//------------------------------------------------------------------------------------------------//
