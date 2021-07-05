#pragma once

//------------------------------------------------------------------------------------------------//
// ������ ���� ���α׷� ���, ����
//------------------------------------------------------------------------------------------------//

class MkStr;
class MkPathName;

class MkWindowStartRegister
{
public:
	
	// ������ ���� ���α׷� ���
	// targetFilePath�� ���� Ȥ�� ��� ���丮 ���� �����
	// ex> MkWindowStartRegister::RegisterStartProgram(L"SteamCafe_SrvMonitor", L"SteamCafe_SrvMonitor.exe");
	static bool RegisterStartProgram(const MkStr& key, const MkPathName& targetFilePath);

	// ������ ���� ���α׷� ����
	// ex> MkWindowStartRegister::RemoveStartProgram(L"SteamCafe_SrvMonitor");
	static bool RemoveStartProgram(const MkStr& key);
};
