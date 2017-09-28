
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"

#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkPatchFileGenerator.h"

//#include <ShellAPI.h>
#include "MkCore_MkVisitWebPage.h"



// single thread
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		{
			MkStr sample;
			sample.ReadTextFile(L"create.txt");

			MkStr buffer;
			MkVisitWebPage::Post(L"https://gameinfo.kuntara.co.kr/character/index.html", sample, buffer);
			buffer.WriteToTextFile(L"rep_create.txt");
		}
		{
			MkStr sample;
			sample.ReadTextFile(L"delete.txt");

			MkStr buffer;
			MkVisitWebPage::Post(L"https://gameinfo.kuntara.co.kr/character/chardel.html", sample, buffer);
			buffer.WriteToTextFile(L"rep_delete.txt");
		}

		return true;
	}

	virtual void StartLooping(void)
	{
		//MK_DEV_PANEL.ClearLogWindow();
	}

	virtual void Update(void)
	{
		
	}

	virtual void Clear(void)
	{
		
	}

	TestFramework() : MkBaseFramework()
	{
	}

	virtual ~TestFramework()
	{
	}

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
	application.Run(hI, L"WhiteBoard", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 200, 200);

	return 0;
}

//------------------------------------------------------------------------------------------------//

