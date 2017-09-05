#pragma once

//------------------------------------------------------------------------------------------------//
// shortcut operation
//
// (NOTE) 당연하게도 관리자 권한이 있어야 함
//        프로젝트 구성 속성 -> 링커 -> 메니페스트 도구 -> UAC 실행 수준 : 관리자 권한 요구(requireAdministrator)
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkPathName.h"


class MkShortcutOperator
{
public:
	
	// shortcut 생성
	static bool CreateShortcut
		(const MkPathName& locationPath, // shortcut file이 위치 할 절대, 혹은 module directory기반 상대 경로
		const MkPathName& targetPath, // shortcut 클릭시 실행될 대상(파일 절대 경로, url 등)
		const MkStr& desc = MkStr::EMPTY, // 설명
		const MkPathName& iconFilePath = MkStr::EMPTY); // 아이콘 파일의 절대, 혹은 module directory기반 상대 경로

	//------------------------------------------------------------------------------------------------//
	// 바탕화면
	//------------------------------------------------------------------------------------------------//

	// 바탕화면에 shortcut 생성
	// ex> MkShortcutOperator::CreateShortcutToCommonDesktopDirectory(L"브아로 가기", L"https://www.daum.net/", L"테스트 숏컷이랑께", L"C:\\origin.ico");
	static bool CreateShortcutToCommonDesktopDirectory
		(const MkPathName& shortcutFilePath, // shortcut file 경로. 확장자가 없으면 lnk 파일로 인식. 중간 경로가 없으면 알아서 생성
		const MkPathName& targetPath, // shortcut 클릭시 실행될 대상(파일 절대 경로, url 등)
		const MkStr& desc = MkStr::EMPTY, // 설명
		const MkPathName& iconFilePath = MkStr::EMPTY); // 아이콘 파일의 절대, 혹은 module directory기반 상대 경로

	// 바탕화면에 생성된 shortcut 삭제
	static bool DeleteShortcutInCommonDesktopDirectory
		(const MkPathName& shortcutFilePath); // shortcut file 경로. 확장자가 없으면 lnk 파일로 인식

	// 시작 프로그램에 생성된 directory 삭제
	static bool DeleteDirectoryInCommonDesktopDirectory
		(const MkPathName& dirPath); // directory 경로

	//------------------------------------------------------------------------------------------------//
	// 시작 프로그램
	//------------------------------------------------------------------------------------------------//

	// 시작 프로그램에 shortcut 생성
	// ex> MkShortcutOperator::CreateShortcutToCommonPrograms(L"꿍따라\\gogo\\게임 스타토", L"https://www.daum.net/", L"와썹 요", L"C:\\origin.ico");
	//     MkShortcutOperator::CreateShortcutToCommonPrograms(L"꿍따라\\빠이 - 빠이", L"C:\\uninstaller.exe", L"나알려", L"C:\\uninstaller.exe");
	static bool CreateShortcutToCommonPrograms
		(const MkPathName& shortcutFilePath, // shortcut file 경로. 확장자가 없으면 lnk 파일로 인식. 중간 경로가 없으면 알아서 생성
		const MkPathName& targetPath, // shortcut 클릭시 실행될 대상(파일의 절대 경로, url 등)
		const MkStr& desc = MkStr::EMPTY, // 설명
		const MkPathName& iconFilePath = MkStr::EMPTY); // 아이콘 파일의 절대, 혹은 module directory기반 상대 경로

	// 시작 프로그램에 생성된 shortcut 삭제
	// ex> MkShortcutOperator::DeleteShortcutInCommonPrograms(L"꿍따라\\gogo\\게임 스타토");
	static bool DeleteShortcutInCommonPrograms
		(const MkPathName& shortcutFilePath); // shortcut file 경로. 확장자가 없으면 lnk 파일로 인식

	// 시작 프로그램에 생성된 directory 삭제
	// ex> MkShortcutOperator::DeleteDirectoryInCommonPrograms(L"꿍따라");
	static bool DeleteDirectoryInCommonPrograms
		(const MkPathName& dirPath); // directory 경로
};
