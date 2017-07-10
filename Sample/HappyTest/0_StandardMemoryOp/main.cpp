
//------------------------------------------------------------------------------------------------//
// single/multi-thread application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>
#include <string.h>
#include <fstream>

#include "MkCore_MkPageManager.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkVec3.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkDevPanel.h"


//------------------------------------------------------------------------------------------------//

const static int SIZE_COUNT = 10;


// TestPage 선언
class TestPage : public MkBasePage
{
protected:

	typedef struct _FixedSizeDataUnit
	{
		int a;
		float b;
		MkVec3 c;
		wchar_t d[64];
	}
	FixedSizeDataUnit;

public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		const unsigned int sizeOfData = sizeof(FixedSizeDataUnit) * SIZE_COUNT;

		// fixed size data 생성
		FixedSizeDataUnit fsUnit[SIZE_COUNT];
		for (int i=0; i<SIZE_COUNT; ++i)
		{
			float f = static_cast<float>(i);

			FixedSizeDataUnit& target = fsUnit[i];
			target.a = i;
			target.b = f * -10.f;
			target.c = MkVec3(f, f * -2.f, f * 3.f);
			wcscpy_s(target.d, 64, L"잘생겨서 죄송요~");
		}

		// fsUnit -> 파일로 출력
		std::ofstream ostream;
		ostream.open(L"test.data", std::ios::out | std::ios::binary | std::ios::trunc);
		if (ostream.is_open())
		{
			ostream.write(reinterpret_cast<const char*>(fsUnit), sizeOfData);
			ostream.close();
		}

		// fixed size data 버퍼 생성 및 초기화
		FixedSizeDataUnit fsBuffer[SIZE_COUNT];
		ZeroMemory(fsBuffer, sizeOfData);

		// 파일에서 읽기 -> fsBuffer
		std::ifstream rstream;
		rstream.open(L"test.data", std::ios::in | std::ios::binary);
		if (rstream.is_open())
		{
			rstream.read(reinterpret_cast<char*>(fsBuffer), sizeOfData);
			rstream.close();
		}

		MK_DEV_PANEL.MsgToLog(MkStr::EMPTY); // 빈 칸 하나 넣고,

		// 읽어들인 결과 출력. 첫번째와 마지막 unit 대상
		const FixedSizeDataUnit& firstUnit = fsBuffer[0];
		MK_DEV_PANEL.MsgToLog(L"[ First unit ]");
		MK_DEV_PANEL.MsgToLog(L"   a : " + MkStr(firstUnit.a));
		MK_DEV_PANEL.MsgToLog(L"   b : " + MkStr(firstUnit.b));
		MK_DEV_PANEL.MsgToLog(L"   c : " + MkStr(firstUnit.c));
		MK_DEV_PANEL.MsgToLog(L"   d : " + MkStr(firstUnit.d));

		const FixedSizeDataUnit& lastUnit = fsBuffer[SIZE_COUNT - 1];
		MK_DEV_PANEL.MsgToLog(L"[ Last unit ]");
		MK_DEV_PANEL.MsgToLog(L"   a : " + MkStr(lastUnit.a));
		MK_DEV_PANEL.MsgToLog(L"   b : " + MkStr(lastUnit.b));
		MK_DEV_PANEL.MsgToLog(L"   c : " + MkStr(lastUnit.c));
		MK_DEV_PANEL.MsgToLog(L"   d : " + MkStr(lastUnit.d));

		// 출력 폴더 열기
		MkPathName targetDir = MkPathName::GetRootDirectory() + L"..\\..\\"; // project foler
		targetDir.OpenDirectoryInExplorer();

		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		
	}

	virtual void Clear(MkDataNode* sharingNode = NULL) {}

	TestPage(const MkHashStr& name) : MkBasePage(name) {}
	virtual ~TestPage() {}
};

// TestFramework 선언
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		MK_PAGE_MGR.SetUp(new TestPage(L"TestPage"));
		MK_PAGE_MGR.ChangePageDirectly(L"TestPage");

		return true;
	}

	virtual ~TestFramework() {}
};

// TestApplication 선언
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
	application.Run(hI, L"Standard memory operation", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

