
#include <Windows.h>
#include <crtdbg.h>

#include "MkCore_MkProjectDefinition.h"
#include "MkCore_MkStr.h"
#include "MkCore_MkLogManager.h"
#include "MkCore_MkCheck.h"


//------------------------------------------------------------------------------------------------//

bool MkCheck::ExceptionForDebug(const wchar_t* expression, const wchar_t* message)
{
	MkStr msg;
	msg.Reserve(1024);
	msg += L"EXPRESSION : ";
	msg += expression;
	msg += L"\nMESSAGE : ";
	msg += message;

#if (MKDEF_CHECK_EXCEPTION_BREAK)
	int rlt = MessageBox(NULL, msg.GetPtr(), L"Oops!!! Break?", MB_YESNO);
	if (rlt == IDYES)
	{
		// break
		_CrtDbgBreak();
	}
	else if (rlt == IDNO)
#else
	int rlt = MessageBox(NULL, msg.GetPtr(), L"Oops!!!", MB_OK);
	if (rlt == IDOK)
#endif
	{
		// log 기록 후 진행
		if (MkLogManager::InstancePtr() != NULL)
		{
			MK_LOG_MGR.Msg(L"<Error> " + msg, true);
		}
	}

	return true; // excute action
}

bool MkCheck::ExceptionForRelease(const char* function, long lineNum, const wchar_t* expression, const wchar_t* message)
{
	MkStr funcBuf;
	funcBuf.ImportMultiByteString(std::string(function));

	MkStr msg;
	msg.Reserve(1024);
	msg += L"FUNCTION : ";
	msg += funcBuf;
	msg += L" (";
	msg += MkStr(lineNum);
	msg += L")\nEXPRESSION : ";
	msg += expression;
	msg += L"\nMESSAGE : ";
	msg += message;

//#if (MKDEF_CHECK_EXCEPTION_BREAK)
//	int rlt = MessageBox(NULL, msg.GetPtr(), L"Oops!!! Break?", MB_YESNO);
//	if (rlt == IDYES)
//	{
//		if (MkLogManager::InstancePtr() != NULL)
//		{
//			MK_LOG_MGR.CreateCrashPage(msg); // crash log
//		{
//		throw; // 강제예외
//	}
//	else if (rlt == IDNO)
//#else
	int rlt = MessageBox(NULL, msg.GetPtr(), L"Oops!!!", MB_OK);
	if (rlt == IDOK)
//#endif
	{
		// log 기록 후 진행
		if (MkLogManager::InstancePtr() != NULL)
		{
			MK_LOG_MGR.Msg(L"<Error> " + msg, true);
		}
	}
	
	return true; // excute action
}

//------------------------------------------------------------------------------------------------//

