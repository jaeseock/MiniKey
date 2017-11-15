
#include "MkCore_MkPathName.h"
#include "MkCore_MkCodeDefinitionConverter.h"

// [빌드 이벤트] -> [빌드 전 이벤트] -> [명령줄]
// $(ProjectDir)MkCodeDefinitionConverter.exe $(ProjectDir)codedef.txt

int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	MkStr::SetUp();
	MkPathName::SetUp();

	const wchar_t* cmdLine = ::GetCommandLine();
	int lineCount = 0;
	LPWSTR* cmdLines = ::CommandLineToArgvW(cmdLine, &lineCount);

	MkArray<MkStr> args(2);
	if (lineCount > 1)
	{
		for (int i=1; i<lineCount; ++i)
		{
			args.PushBack(cmdLines[i]);
		}
	}

	// args[0] : 원본 data node 파일명
	// (opt) args[1] : 대상 서비스명
	if (args.GetSize() > 0)
	{
		MkCodeDefinitionConverter::Convert(args[0], (args.GetSize() == 2) ? args[1] : MkStr::EMPTY);
	}
	return 0;
}

