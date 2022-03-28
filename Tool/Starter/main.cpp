
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkCmdLine.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkWin32Application.h"
#include "MkCore_MkPatchStarter.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkTailData.h"


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

		// URL
		MkStr url;
		MkByteArray tailDataBuffer;
		if (MkTailData::GetTailData(tailDataBuffer))
		{
			MkDataNode tailDataNode;
			if (tailDataNode.Load(tailDataBuffer))
			{
				tailDataNode.GetData(L"URL", url, 0);
			}
		}

		if (url.Empty())
		{
			//url = L"http://210.207.252.151/kuntara/korea";
			//url = L"https://intpatch-rohan2m.playwith.co.kr/devclient";
			//url = L"http://intpatch-sealm.playwith.co.kr/dev_win_patch";
			//url = L"http://intpatch-sealm.playwith.co.kr/pp_win_patch";
			::MessageBox(NULL, L"No URL", L"Starter", MB_OK);
			return false;
		}

		// CMD
		MkCmdLine newCmd = cmdLine;
		newCmd.AddPair(L"#AME", L"_MkStarter");

		MkStr cmdPass;
		newCmd.GetFullStr(cmdPass);

		MkPatchStarter::StartLauncher(url, cmdPass.GetPtr());
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

