
//------------------------------------------------------------------------------------------------//
// single/multi-thread application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkPageManager.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkVec3.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkDevPanel.h"

#include "MkCore_MkInterfaceForDataWriting.h"
#include "MkCore_MkInterfaceForDataReading.h"
#include "MkCore_MkInterfaceForFileWriting.h"
#include "MkCore_MkInterfaceForFileReading.h"


//------------------------------------------------------------------------------------------------//

const static int SIZE_COUNT = 10;


// TestPage ����
class TestPage : public MkBasePage
{
public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		// [ data ���� ���� ]
		//------------------------------------------------------------------------------------------------//
		// int a;
		// float b;
		// MkVec3 c;
		// wchar_t d[64];
		//------------------------------------------------------------------------------------------------//

		// srcArray ����
		MkInterfaceForDataWriting dwInterface;
		dwInterface.SetInputSize(ePDT_Int, SIZE_COUNT);
		dwInterface.SetInputSize(ePDT_Float, SIZE_COUNT);
		dwInterface.SetInputSize(ePDT_Vec3, SIZE_COUNT);
		dwInterface.SetInputSize(ePDT_Str, SIZE_COUNT);
		dwInterface.UpdateInputSize();

		for (int i=0; i<SIZE_COUNT; ++i)
		{
			float f = static_cast<float>(i);

			dwInterface.Write(static_cast<int>(i));
			dwInterface.Write(static_cast<float>(f * -10.f));
			dwInterface.Write(MkVec3(f, f * -2.f, f * 3.f));
			dwInterface.Write(MkStr(L"�߻��ܼ� �˼ۿ�~"));
		}

		MkByteArray srcArray;
		dwInterface.Flush(srcArray);
		dwInterface.Clear();

		// srcArray -> ���Ϸ� ���
		MkInterfaceForFileWriting fwInterface;
		fwInterface.SetUp(L"test.data", true, true);
		fwInterface.Write(srcArray, MkArraySection(0));
		fwInterface.Clear();

		srcArray.Clear();
		
		// ������ destBuffer�� ����
		MkByteArray destBuffer;
		MkInterfaceForFileReading frInterface;
		frInterface.SetUp(L"test.data");
		frInterface.Read(destBuffer, MkArraySection(0));
		frInterface.Clear();

		MK_DEV_PANEL.MsgToLog(MkStr::EMPTY); // �� ĭ �ϳ� �ְ�,

		// �о���� ��� ���. ù��°�� ������ unit ���
		MkInterfaceForDataReading drInterface;
		drInterface.SetUp(destBuffer, 0);

		for (int i=0; i<SIZE_COUNT; ++i)
		{
			int a = 0;
			drInterface.Read(a);

			float b = 0.f;
			drInterface.Read(b);

			MkVec3 c = MkVec3::Zero;
			drInterface.Read(c);

			MkStr d;
			drInterface.Read(d);

			if (i == 0)
			{
				MK_DEV_PANEL.MsgToLog(L"[ First unit ]");
				MK_DEV_PANEL.MsgToLog(L"   a : " + MkStr(a));
				MK_DEV_PANEL.MsgToLog(L"   b : " + MkStr(b));
				MK_DEV_PANEL.MsgToLog(L"   c : " + MkStr(c));
				MK_DEV_PANEL.MsgToLog(L"   d : " + MkStr(d));
			}
			else if (i == (SIZE_COUNT - 1))
			{
				MK_DEV_PANEL.MsgToLog(L"[ Last unit ]");
				MK_DEV_PANEL.MsgToLog(L"   a : " + MkStr(a));
				MK_DEV_PANEL.MsgToLog(L"   b : " + MkStr(b));
				MK_DEV_PANEL.MsgToLog(L"   c : " + MkStr(c));
				MK_DEV_PANEL.MsgToLog(L"   d : " + MkStr(d));
			}
		}

		destBuffer.Clear();

		// ��� ���� ����
		MkPathName::GetRootDirectory().OpenDirectoryInExplorer();

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
	application.Run(hI, L"Standard memory operation", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

