
#include <Windows.h>
#include <crtdbg.h>
#include "MkCore_MkStr.h"
#include "MkCore_MkLogManager.h"
#include "MkCore_MkCheck.h"

// ���� �߻� �� ���� ���� �߰�. ����Ǿ� ���� ������ �α׿� ��ϸ� ����� ��ŵ
#define MKDEF_CHECK_EXCEPTION_BREAK

//------------------------------------------------------------------------------------------------//

bool MkCheck::ExceptionForDebug(const wchar_t* message)
{
#ifdef MKDEF_CHECK_EXCEPTION_BREAK
	int rlt = MessageBox(NULL, message, L"Oops!!! Break?", MB_YESNO);
	if (rlt == IDYES)
	{
		// break
		_CrtDbgBreak();
	}
	else if (rlt == IDNO)
#else
	int rlt = MessageBox(NULL, message, L"Oops!!!", MB_OK);
	if (rlt == IDOK)
#endif
	{
		// log ��� �� ����
		MK_LOG_MGR.Msg(L"<Error> " + MkStr(message), true);
	}

	return true; // always true
}

bool MkCheck::ExceptionForRelease(const char* function, long lineNum, const wchar_t* message)
{
	// message
	MkStr funcBuf;
	funcBuf.ImportMultiByteString(std::string(function));
	MkStr msgBuf = message;
	MkStr buffer;
	buffer.Reserve(50 + funcBuf.GetSize() + msgBuf.GetSize());
	buffer += L"<Error> ";
	buffer += funcBuf;
	buffer += L" (";
	buffer += MkStr(lineNum);
	buffer += L") : ";
	buffer += message;

#ifdef MKDEF_CHECK_EXCEPTION_BREAK
	int rlt = MessageBox(NULL, message, L"Oops!!! Break?", MB_YESNO);
	if (rlt == IDYES)
	{
		// crash log
		MK_LOG_MGR.CreateCrashPage(buffer);

		// ��������
		throw;
	}
	else if (rlt == IDNO)
#else
	int rlt = MessageBox(NULL, message, L"Oops!!!", MB_OK);
	if (rlt == IDOK)
#endif
	{
		// log ��� �� ����
		MK_LOG_MGR.Msg(buffer, true);
	}
	
	return true; // always true
}

//------------------------------------------------------------------------------------------------//

