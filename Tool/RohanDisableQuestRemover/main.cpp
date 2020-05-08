
//------------------------------------------------------------------------------------------------//

//#include <windows.h>

#include "MkCore_MkExcelFileInterface.h"

#include "MkCore_MkDevPanel.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

// steam -----------------------------------------------------

// single thread
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		return true; // EXIT_SUCCESS
	}

	virtual void StartLooping(void)
	{
	}

	virtual void ConsumeDraggedFiles(MkArray<MkPathName>& draggedFilePathList)
	{
		if (draggedFilePathList.GetSize() == 1)
		{
			MkExcelFileInterface excel;
			if (excel.SetUp(draggedFilePathList[0]))
			{
				if (excel.SetActiveSheet(0))
				{
					unsigned int y = 1;
					int emptyCellCount = 0;

					int disableQuestCount = 0;
					int deletionCount = 0;

					while (emptyCellCount < 100)
					{
						MkStr enable;
						if (excel.GetData(y, 3, enable) && (!enable.Empty()))
						{
							if (!enable.ToBool())
							{
								++disableQuestCount;

								for (int x=4; x<10; ++x)
								{
									MkStr txtBuf;
									if (excel.GetData(y, x, txtBuf) && (!txtBuf.Empty()))
									{
										MkPathName fileName = txtBuf;
										fileName.DeleteCurrentFile();
										++deletionCount;
									}
								}
							}

							emptyCellCount = 0;
						}
						else
						{
							++emptyCellCount;
						}

						++y;
					}

					MK_DEV_PANEL.MsgToLog(L"��ȿ ����Ʈ : " + MkStr(disableQuestCount) + L" ��");
					MK_DEV_PANEL.MsgToLog(L"���� �ؽ�Ʈ : " + MkStr(deletionCount) + L" ��");
				}
				else
					::MessageBox(m_MainWindow.GetWindowHandle(), L"��ȿ�� ��Ʈ�� �����ϴ�", L"ERROR!!!", MB_OK);
			}
			else
				::MessageBox(m_MainWindow.GetWindowHandle(), L"�������� ���� ������ �ƴ�", L"ERROR!!!", MB_OK);
		}
		else if (draggedFilePathList.GetSize() > 1)
		{
			::MessageBox(m_MainWindow.GetWindowHandle(), L"�ϳ��� ���ϸ� �־��ּ���~", L"ERROR!!!", MB_OK);
		}

		draggedFilePathList.Clear();
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

// ��Ʈ�� ����Ʈ������ TestApplication ����
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"RohanDisableQuestRemover", L"..\\", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 200, 200, false, true);

	return 0;
}

//------------------------------------------------------------------------------------------------//

