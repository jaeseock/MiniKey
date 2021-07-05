
#include <windows.h>
#include <tlhelp32.h>
#include "MkCore_MkStr.h"
#include "MkCore_MkWindowProcessOp.h"

bool MkWindowProcessOp::Exist(const MkStr& processName)
{
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
	bool ok = false;

	do
	{
		hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
			break;

		PROCESSENTRY32 processEntry32;
		processEntry32.dwSize = sizeof(PROCESSENTRY32);

		if (!::Process32First(hProcessSnap, &processEntry32))
			break;

		while (::Process32Next(hProcessSnap, &processEntry32))
		{
			if (processName.Equals(0, MkStr(processEntry32.szExeFile)))
			{
				ok = true;
				break;
			}
		}
	}
	while (false);

	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hProcessSnap);
	}
	return ok;
}

int MkWindowProcessOp::Count(const MkStr& processName)
{
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
	int result = -1;

	do
	{
		hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
			break;

		PROCESSENTRY32 processEntry32;
		processEntry32.dwSize = sizeof(PROCESSENTRY32);

		if (!::Process32First(hProcessSnap, &processEntry32))
			break;

		result = 0;
		while (::Process32Next(hProcessSnap, &processEntry32))
		{
			if (processName.Equals(0, MkStr(processEntry32.szExeFile)))
			{
				++result;
			}
		}
	}
	while (false);

	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hProcessSnap);
	}
	return result;
}

int MkWindowProcessOp::Kill(const MkStr& processName)
{
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
	int result = -1;

	do
	{
		hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
			break;

		PROCESSENTRY32 processEntry32;
		processEntry32.dwSize = sizeof(PROCESSENTRY32);

		if (!::Process32First(hProcessSnap, &processEntry32))
			break;

		result = 0;
		while (::Process32Next(hProcessSnap, &processEntry32))
		{
			if (processName.Equals(0, MkStr(processEntry32.szExeFile)))
			{
				HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, processEntry32.th32ProcessID);
				if (hProcess != NULL)
				{
					if (::TerminateProcess(hProcess, 1) == TRUE)
					{
						++result;
					}

					CloseHandle(hProcess);
				}
			}
		}
	}
	while (false);

	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hProcessSnap);
	}
	return result;
}
