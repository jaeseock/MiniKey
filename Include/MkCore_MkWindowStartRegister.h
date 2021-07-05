#pragma once

//------------------------------------------------------------------------------------------------//
// 윈도우 시작 프로그램 등록, 삭제
//------------------------------------------------------------------------------------------------//

class MkStr;
class MkPathName;

class MkWindowStartRegister
{
public:
	
	// 윈도우 시작 프로그램 등록
	// targetFilePath는 절대 혹은 모듈 디렉토리 기준 상대경로
	// ex> MkWindowStartRegister::RegisterStartProgram(L"SteamCafe_SrvMonitor", L"SteamCafe_SrvMonitor.exe");
	static bool RegisterStartProgram(const MkStr& key, const MkPathName& targetFilePath);

	// 윈도우 시작 프로그램 삭제
	// ex> MkWindowStartRegister::RemoveStartProgram(L"SteamCafe_SrvMonitor");
	static bool RemoveStartProgram(const MkStr& key);
};
