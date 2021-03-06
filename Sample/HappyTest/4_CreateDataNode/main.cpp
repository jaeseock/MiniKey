
//------------------------------------------------------------------------------------------------//
// single/multi-thread application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>
#include <string.h>
#include <fstream>

#include "MkCore_MkPageManager.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkSystemEnvironment.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"


//------------------------------------------------------------------------------------------------//

// TestPage 선언
class TestPage : public MkBasePage
{
public:

	virtual bool SetUp(MkDataNode& sharingNode)
	{
		// 객체 생성
		MkDataNode dataNode;

		// "Today" unit pair 생성
		MkStr today = MK_SYS_ENV.GetCurrentSystemDate();
		dataNode.CreateUnit(L"Today", today);

		// "TimeStamp" unit pair 생성
		MkStr timeStamp = MK_SYS_ENV.GetCurrentSystemTime();
		dataNode.CreateUnit(L"TimeStamp", timeStamp);

		// "UserName" unit pair 생성
		const MkStr& userName = MK_SYS_ENV.GetCurrentLogOnUserName();
		dataNode.CreateUnit(L"UserName", userName);

		// oops, "UserName" 변경
		dataNode.SetData(L"UserName", MkStr(L"大和撫子(やまとなでしこ)"), 0);

		// 하위 "개념" 노드 생성
		{
			MkDataNode* childNode = dataNode.CreateChildNode(L"개념");

			// "Exist" unit pair 생성
			childNode->CreateUnit(L"Exist", false);

			// "IQperLevel" unit pair 생성
			MkArray<int> IQperLevel;
			IQperLevel.PushBack(10);
			IQperLevel.PushBack(20);
			IQperLevel.PushBack(-1);
			IQperLevel.PushBack(40);
			childNode->CreateUnit(L"IQperLevel", IQperLevel);

			// aa.txt 파일을 읽어 내용을 "aa" unit으로 추가
			MkStr aa;
			aa.ReadTextFile(L"..\\..\\aa.txt");
			childNode->CreateUnit(L"aa", aa);
		}

		// "Desc" unit pair 생성
		dataNode.CreateUnit(L"Desc", ePDT_Str, 4);
		dataNode.SetData(L"Desc", MkStr(L"잘생긴게\\n"), 0);
		dataNode.SetData(L"Desc", MkStr(L"\\t죄엡~ 니이~ 드아으아"), 1);
		dataNode.SetData(L"Desc", MkStr(L"죄송요 (*´ω｀*)"), 3);

		// 청크 파일로 저장
		dataNode.SaveToBinary(L"..\\..\\test.bin");
		dataNode.Clear();

		// 새 data node를 선언하고 청크 파일을 읽음
		MkDataNode nodeBuffer;
		nodeBuffer.Load(L"..\\..\\test.bin");

		// "개념" 노드의 aa를 log로 출력
		MkStr aa;
		nodeBuffer.GetChildNode(L"개념")->GetData(L"aa", aa, 0);
		MK_DEV_PANEL.MsgToLog(aa);

		// 그대로 text로 저장
		nodeBuffer.SaveToText(L"..\\..\\test.txt");
		
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
	application.Run(hI, L"Data node Op.", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, false, false, NULL);

	return 0;
}

//------------------------------------------------------------------------------------------------//

