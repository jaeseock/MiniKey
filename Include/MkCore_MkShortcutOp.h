#pragma once

//------------------------------------------------------------------------------------------------//
// shortcut operation
//
// (NOTE) �翬�ϰԵ� ������ ������ �־�� ��
//        ������Ʈ ���� �Ӽ� -> ��Ŀ -> �޴��佺Ʈ ���� -> UAC ���� ���� : ������ ���� �䱸(requireAdministrator)
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkPathName.h"


class MkShortcutOperator
{
public:
	
	// shortcut ����
	static bool CreateShortcut
		(const MkPathName& locationPath, // shortcut file�� ��ġ �� ����, Ȥ�� module directory��� ��� ���
		const MkPathName& targetPath, // shortcut Ŭ���� ����� ���(���� ���� ���, url ��)
		const MkStr& desc = MkStr::EMPTY, // ����
		const MkPathName& iconFilePath = MkStr::EMPTY); // ������ ������ ����, Ȥ�� module directory��� ��� ���

	//------------------------------------------------------------------------------------------------//
	// ����ȭ��
	//------------------------------------------------------------------------------------------------//

	// ����ȭ�鿡 shortcut ����
	// ex> MkShortcutOperator::CreateShortcutToCommonDesktopDirectory(L"��Ʒ� ����", L"https://www.daum.net/", L"�׽�Ʈ �����̶���", L"C:\\origin.ico");
	static bool CreateShortcutToCommonDesktopDirectory
		(const MkPathName& shortcutFilePath, // shortcut file ���. Ȯ���ڰ� ������ lnk ���Ϸ� �ν�. �߰� ��ΰ� ������ �˾Ƽ� ����
		const MkPathName& targetPath, // shortcut Ŭ���� ����� ���(���� ���� ���, url ��)
		const MkStr& desc = MkStr::EMPTY, // ����
		const MkPathName& iconFilePath = MkStr::EMPTY); // ������ ������ ����, Ȥ�� module directory��� ��� ���

	// ����ȭ�鿡 ������ shortcut ����
	static bool DeleteShortcutInCommonDesktopDirectory
		(const MkPathName& shortcutFilePath); // shortcut file ���. Ȯ���ڰ� ������ lnk ���Ϸ� �ν�

	// ���� ���α׷��� ������ directory ����
	static bool DeleteDirectoryInCommonDesktopDirectory
		(const MkPathName& dirPath); // directory ���

	//------------------------------------------------------------------------------------------------//
	// ���� ���α׷�
	//------------------------------------------------------------------------------------------------//

	// ���� ���α׷��� shortcut ����
	// ex> MkShortcutOperator::CreateShortcutToCommonPrograms(L"�����\\gogo\\���� ��Ÿ��", L"https://www.daum.net/", L"�ͽ� ��", L"C:\\origin.ico");
	//     MkShortcutOperator::CreateShortcutToCommonPrograms(L"�����\\���� - ����", L"C:\\uninstaller.exe", L"���˷�", L"C:\\uninstaller.exe");
	static bool CreateShortcutToCommonPrograms
		(const MkPathName& shortcutFilePath, // shortcut file ���. Ȯ���ڰ� ������ lnk ���Ϸ� �ν�. �߰� ��ΰ� ������ �˾Ƽ� ����
		const MkPathName& targetPath, // shortcut Ŭ���� ����� ���(������ ���� ���, url ��)
		const MkStr& desc = MkStr::EMPTY, // ����
		const MkPathName& iconFilePath = MkStr::EMPTY); // ������ ������ ����, Ȥ�� module directory��� ��� ���

	// ���� ���α׷��� ������ shortcut ����
	// ex> MkShortcutOperator::DeleteShortcutInCommonPrograms(L"�����\\gogo\\���� ��Ÿ��");
	static bool DeleteShortcutInCommonPrograms
		(const MkPathName& shortcutFilePath); // shortcut file ���. Ȯ���ڰ� ������ lnk ���Ϸ� �ν�

	// ���� ���α׷��� ������ directory ����
	// ex> MkShortcutOperator::DeleteDirectoryInCommonPrograms(L"�����");
	static bool DeleteDirectoryInCommonPrograms
		(const MkPathName& dirPath); // directory ���
};
