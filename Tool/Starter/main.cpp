
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkCmdLine.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"
#include "MkCore_MkPatchStarter.h"


// single thread
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		MkCmdLine newCmd = cmdLine;
		newCmd.AddPair("#AME", "_MkStarter");
		newCmd.UpdateFullStr();

		bool ok = MkPatchStarter::StartLauncher(L"http://210.207.252.151/kuntara", newCmd);
		return false; // 종료
	}

	TestFramework() : MkBaseFramework() {}
	virtual ~TestFramework() {}

protected:
};

class TestApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new TestFramework; }

public:
	TestApplication() : MkWin32Application() {}
	virtual ~TestApplication() {}
};

//------------------------------------------------------------------------------------------------//

// 엔트리 포인트에서의 TestApplication 실행
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"Starter", L"", false, eSWP_None, CW_USEDEFAULT, CW_USEDEFAULT, 100, 20, false, true, NULL, cmdline,
		"#DMK=_MkStarter; #BME=_MkStarter; #BME=_MkLauncher; #BME=_MkGameApp");

	return 0;
}

//------------------------------------------------------------------------------------------------//

