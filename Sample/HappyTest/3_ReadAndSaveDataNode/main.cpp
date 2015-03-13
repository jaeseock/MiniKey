
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

// TestPage ����
class TestPage : public MkBasePage
{
public:

	virtual bool SetUp(MkDataNode& sharingNode)
	{
		// ���Ϸκ��� ����
		MkDataNode dataNode;
		dataNode.Load(L"..\\..\\test01.txt");

		// ��� Ž��
		MkDataNode* orcNode = dataNode.GetChildNode(L"MonsterTable")->GetChildNode(L"Orc");

		// �� Ȯ�� : KoreanName(overwrite)
		MkStr koreanName;
		if (orcNode->GetData(L"KoreanName", koreanName, 0))
		{
			MK_DEV_PANEL.MsgToLog(L"");
			MK_DEV_PANEL.MsgToLog(L"KoreanName : " + koreanName);
		}

		// �� Ȯ�� : intelligence(overwrite)
		int intelligence;
		if (orcNode->GetData(L"intelligence", intelligence, 0))
		{
			MK_DEV_PANEL.MsgToLog(L"");
			MK_DEV_PANEL.MsgToLog(L"intelligence : " + MkStr(intelligence));
		}

		// �� Ȯ�� : threeSize(���ø����� ����)
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

		// �� ����
		orcNode->SetData(L"threeSize", static_cast<unsigned int>(200), 1);

		// orc node�� �ؽ�Ʈ ���Ϸ� ����
		orcNode->SaveToText(L"..\\..\\ORC.txt");
		
		// ��� ���� ����
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

// TestFramework ����
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

// TestApplication ����
class TestApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new TestFramework; }

public:
	TestApplication() : MkWin32Application() {}
	virtual ~TestApplication() {}
};

//------------------------------------------------------------------------------------------------//

// ��Ʈ�� ����Ʈ������ TestApplication ����
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"Data node Op.", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

