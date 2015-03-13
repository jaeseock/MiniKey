
//------------------------------------------------------------------------------------------------//
// single/multi-thread application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>
#include <string.h>
#include <fstream>

#include "MkCore_MkPageManager.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"


//------------------------------------------------------------------------------------------------//

// TestPage 선언
class TestPage : public MkBasePage
{
public:

	virtual bool SetUp(MkDataNode& sharingNode)
	{
		// 파일로부터 읽음
		MkDataNode dataNode;
		dataNode.Load(L"..\\..\\test01.txt");

		// 노드 탐색
		MkDataNode* orcNode = dataNode.GetChildNode(L"MonsterTable")->GetChildNode(L"Orc");

		// 값 확인 : KoreanName(overwrite)
		MkStr koreanName;
		if (orcNode->GetData(L"KoreanName", koreanName, 0))
		{
			MK_DEV_PANEL.MsgToLog(L"");
			MK_DEV_PANEL.MsgToLog(L"KoreanName : " + koreanName);
		}

		// 값 확인 : intelligence(overwrite)
		int intelligence;
		if (orcNode->GetData(L"intelligence", intelligence, 0))
		{
			MK_DEV_PANEL.MsgToLog(L"");
			MK_DEV_PANEL.MsgToLog(L"intelligence : " + MkStr(intelligence));
		}

		// 값 확인 : threeSize(템플릿에만 존재)
		MkArray<unsigned int> threeSize;
		if (orcNode->GetData(L"threeSize", threeSize))
		{
			MK_DEV_PANEL.MsgToLog(L"");
			MK_DEV_PANEL.MsgToLog(L"threeSize :");

			MK_INDEXING_LOOP(threeSize, i)
			{
				MK_DEV_PANEL.MsgToLog(L"   - " + MkStr(threeSize[i]));
			}
		}

		// 값 수정
		orcNode->SetData(L"threeSize", static_cast<unsigned int>(200), 1);

		// orc node만 텍스트 파일로 저장
		orcNode->SaveToText(L"..\\..\\ORC.txt");
		
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
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg)
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
	application.Run(hI, L"Data node Op.", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

