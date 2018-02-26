
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkDevPanel.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"


// single thread
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		MkStr ansi;
		ansi.ReadTextFile(L"D:\\Solutions\\MiniKey\\Tool\\MMD Viewer\\bin\\FileRoot\\tt_ansi.txt");
		ansi.WriteToTextFile(L"D:\\Solutions\\MiniKey\\Tool\\MMD Viewer\\bin\\FileRoot\\tt_utf8.txt", true, false);

		return true;
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
	application.Run(hI, L"WhiteBoard", L"..\\ResRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 200, 200);

	return 0;
}

//------------------------------------------------------------------------------------------------//

