
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
		//..
		int tval = 0;
#if DEF_SEAL_TYPE
		tval = 1;
#endif

		int i = 0;
#if (DEF_TEST_INDEX == 1)
		i = 1;
#elif (DEF_TEST_INDEX == 2)
		i = 2;
#endif

		bool ok = DEF_TEST_SWITCH;
		const wchar_t* name = DEF_TEST_STR;

		int k = 0;
#ifdef DEF_TEST_HEIGHT
		k = 1;
#endif
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
	application.Run(hI, L"Code switch sample", L"..\\ResRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 200, 200);

	return 0;
}

//------------------------------------------------------------------------------------------------//

