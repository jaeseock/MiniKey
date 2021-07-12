
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkCmdLine.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkWin32Application.h"
#include "MkCore_MkPatchStarter.h"


class TestApplication : public MkWin32Application
{
public:
	TestApplication() : MkWin32Application() {}
	virtual ~TestApplication() {}

protected:

	virtual bool _CheckExcution(MkCmdLine& cmdLine, HANDLE& myMutexHandle, const wchar_t* myTitle)
	{
		if (!MkWin32Application::_CheckExcution(cmdLine, myMutexHandle, myTitle))
			return false;

		MkPathName::SetUp();

		MkCmdLine newCmd = cmdLine;
		newCmd.AddPair(L"#AME", L"_MkStarter");

		MkStr cmdPass;
		newCmd.GetFullStr(cmdPass);

		//MkPatchStarter::StartLauncher(L"http://210.207.252.151/kuntara/korea", cmdPass.GetPtr());
		MkPatchStarter::StartLauncher(L"https://intpatch-rohan2m.playwith.co.kr/devclient", cmdPass.GetPtr());
		return false; // 종료
	}
};

//------------------------------------------------------------------------------------------------//

// 엔트리 포인트에서의 TestApplication 실행
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"Starter", L"", false, eSWP_None, CW_USEDEFAULT, CW_USEDEFAULT, 100, 20, false, true, NULL,
		L"#DMK=_MkStarter; #BME=_MkStarter; #BME=_MkLauncher; #BME=_MkGameApp");

	return 0;
}

//------------------------------------------------------------------------------------------------//

