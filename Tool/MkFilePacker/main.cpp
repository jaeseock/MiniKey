
//------------------------------------------------------------------------------------------------//
// single/multi-thread application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkLogicThreadUnit.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

//#include "MkCore_MkFileManager.h"
#include "MkCore_MkFileSystem.h"
//#include "MkCore_MkCheck.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkDataNode.h"

#define MKDEF_APP_EDIT_RES_DIR_TAG_ID 1
#define MKDEF_APP_EDIT_RES_DIR_VALUE_ID 2
#define MKDEF_APP_BTN_FIND_RES_DIR_ID 3

#define MKDEF_APP_EDIT_TARGET_DIR_TAG_ID 4
#define MKDEF_APP_EDIT_TARGET_DIR_VALUE_ID 5
#define MKDEF_APP_BTN_FIND_TARGET_DIR_ID 6

#define MKDEF_APP_EDIT_SETTING_FILE_TAG_ID 7
#define MKDEF_APP_EDIT_SETTING_FILE_VALUE_ID 8
#define MKDEF_APP_BTN_FIND_SETTING_FILE_ID 9

#define MKDEF_APP_BTN_PACK_START_ID 10


static MkArray<int> gFrameworkEvent(32);


//------------------------------------------------------------------------------------------------//

class TestFramework : public MkBaseFramework
{
public:

	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		// ��Ʈ�� ���� �غ�
		m_Font = CreateFont
			(12, 0, 0, 0, 0, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH | FF_ROMAN, L"����");

		HWND hWnd = m_MainWindow.GetWindowHandle();
		HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE));

		const DWORD staticTextControlStyle = WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY;
		const DWORD inputControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT;
		const DWORD buttonControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON;

		const int DEF_WIDTH = 115;
		int currentY = 5;

		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_RES_DIR_TAG_ID, L" ���ҽ� ����", staticTextControlStyle, MkIntRect(5, currentY, DEF_WIDTH, 12));

		currentY += 13;
		m_ResourceDirHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_RES_DIR_VALUE_ID, MkPathName::GetModuleDirectory().GetPtr(), inputControlStyle, MkIntRect(5, currentY, 545, 20));
		
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FIND_RES_DIR_ID, L"����", buttonControlStyle, MkIntRect(555, currentY, 40, 20));

		currentY += 30;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_TARGET_DIR_TAG_ID, L" ��� ��� ����", staticTextControlStyle, MkIntRect(5, currentY, DEF_WIDTH, 12));

		currentY += 13;
		m_TargetDirHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_TARGET_DIR_VALUE_ID, MkPathName::GetModuleDirectory().GetPtr(), inputControlStyle, MkIntRect(5, currentY, 545, 20));
		
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FIND_TARGET_DIR_ID, L"����", buttonControlStyle, MkIntRect(555, currentY, 40, 20));

		currentY += 30;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_SETTING_FILE_TAG_ID, L" ���� ���� ���", staticTextControlStyle, MkIntRect(5, currentY, DEF_WIDTH, 12));

		currentY += 13;
		m_SettingFileHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_SETTING_FILE_VALUE_ID, L"", inputControlStyle, MkIntRect(5, currentY, 545, 20));
		
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FIND_SETTING_FILE_ID, L"����", buttonControlStyle, MkIntRect(555, currentY, 40, 20));

		currentY += 30;
		m_PackingStartHandle = _CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_PACK_START_ID, L"��ŷ ����", buttonControlStyle, MkIntRect(5, currentY, 590, 40));

		return true;
	}

	virtual void StartLooping(void)
	{
		MK_DEV_PANEL.ClearLogWindow();
	}

	virtual void Update(void)
	{
		if (m_OnPacking)
		{
			MkFileSystem pack;

			if (!m_SettingPath.Empty())
			{
				MkPathName settingFile;
				settingFile.ConvertToModuleBasisAbsolutePath(m_SettingPath);
				if (settingFile.CheckAvailable())
				{
					pack.SetSystemSetting(settingFile);
				}
			}

			if (pack.SetUpFromOriginalDirectory(m_ResPath, m_TargetPath))
			{
				pack.PrintSystemInfoToDevPanel();
				MessageBox(m_MainWindow.GetWindowHandle(), L"��ŷ ����", L"Success!", MB_OK);
			}
			else
			{
				MessageBox(m_MainWindow.GetWindowHandle(), L"��ŷ ����", L"Error!", MB_OK);
			}

			// ��� ����
			MkDataNode structureNode;
			pack.ExportSystemStructure(structureNode, true);

			MkPathName targetNodePath = L"PackInfo.txt";
			structureNode.SaveToText(targetNodePath);
			MK_DEV_PANEL.MsgToLog(targetNodePath + L" ���Ͽ� ��� ����");
			targetNodePath.OpenFileInVerb();

			pack.Clear();

			m_ResPath.Clear();
			m_TargetPath.Clear();
			m_SettingPath.Clear();

			m_OnPacking = false;
			EnableWindow(m_PackingStartHandle, TRUE);
		}

		if (!m_OnPacking)
		{
			MK_INDEXING_LOOP(gFrameworkEvent, i)
			{
				switch (gFrameworkEvent[i])
				{
				case MKDEF_APP_BTN_FIND_RES_DIR_ID: // ���ҽ� ���� ���� �̺�Ʈ. ���̾�α׸� ���� ����
					{
						if (m_ResourceDirHandle != NULL)
						{
							MkPathName defPath;
							if (!_GetPathFromEditBox(m_ResourceDirHandle, defPath))
							{
								defPath = MkPathName::GetModuleDirectory();
							}

							MkPathName targetPath;
							if (targetPath.GetDirectoryPathFromDialog(L"���ҽ� ������ ����", m_MainWindow.GetWindowHandle(), defPath))
							{
								SetWindowText(m_ResourceDirHandle, targetPath.GetPtr());
							}
						}
					}
					break;

				case MKDEF_APP_BTN_FIND_TARGET_DIR_ID: // Ÿ�� ���� ���� �̺�Ʈ. ���̾�α׸� ���� ����
					{
						if (m_TargetDirHandle != NULL)
						{
							MkPathName defPath;
							if (!_GetPathFromEditBox(m_TargetDirHandle, defPath))
							{
								defPath = MkPathName::GetModuleDirectory();
							}

							// ���� ����
							MkPathName targetPath;
							if (targetPath.GetDirectoryPathFromDialog(L"��� ��� ������ ����", m_MainWindow.GetWindowHandle(), defPath))
							{
								SetWindowText(m_TargetDirHandle, targetPath.GetPtr());
							}
						}
					}
					break;

				case MKDEF_APP_BTN_FIND_SETTING_FILE_ID: // ���� ���� ��ư �̺�Ʈ. ���̾�α׸� ���� ����
					{
						if (m_SettingFileHandle != NULL)
						{
							MkPathName filePath;
							if (filePath.GetSingleFilePathFromDialog(m_MainWindow.GetWindowHandle()))
							{
								SetWindowText(m_SettingFileHandle, filePath.GetPtr());

								MkFileSystem pack;
								pack.SetSystemSetting(filePath);
								pack.PrintSystemInfoToDevPanel(true);
							}
						}
					}
					break;

				case MKDEF_APP_BTN_PACK_START_ID: // ��ŷ ���� ��ư �̺�Ʈ
					{
						if ((m_ResourceDirHandle != NULL) && (m_TargetDirHandle != NULL) && (m_SettingFileHandle != NULL) && (m_PackingStartHandle != NULL))
						{
							if (_GetPathFromEditBox(m_ResourceDirHandle, m_ResPath) && _GetPathFromEditBox(m_TargetDirHandle, m_TargetPath))
							{
								_GetPathFromEditBox(m_SettingFileHandle, m_SettingPath);
								
								MK_DEV_PANEL.MsgToLog(L"//---------------------------------------------------------------//");
								MK_DEV_PANEL.MsgToLog(L"- ���ҽ� ��� : " + m_ResPath);
								MK_DEV_PANEL.MsgToLog(L"- ���� ��� : " + m_TargetPath);
								if (!m_SettingPath.Empty())
								{
									MK_DEV_PANEL.MsgToLog(L"- ���� ���� ��� : " + m_SettingPath);
								}
								MK_DEV_PANEL.MsgToLog(L"//---------------------------------------------------------------//");

								m_OnPacking = true;
								EnableWindow(m_PackingStartHandle, FALSE);
							}
						}
					}
					break;
				}
			}
		}

		gFrameworkEvent.Clear();
	}

	static LRESULT CALLBACK NewWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_COMMAND:
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				{
					int target = static_cast<int>(LOWORD(wParam));
					switch (target)
					{
					case MKDEF_APP_BTN_FIND_RES_DIR_ID:
					case MKDEF_APP_BTN_FIND_TARGET_DIR_ID:
					case MKDEF_APP_BTN_FIND_SETTING_FILE_ID:
					case MKDEF_APP_BTN_PACK_START_ID:
						gFrameworkEvent.PushBack(target);
						break;
					}
				}
				break;
			}
			break;
		}

		return WndProc(hWnd, msg, wParam, lParam);
	}

	TestFramework() : MkBaseFramework()
	{
		m_Font = NULL;
		m_ResourceDirHandle = NULL;
		m_TargetDirHandle = NULL;
		m_SettingFileHandle = NULL;
		m_PackingStartHandle = NULL;

		m_OnPacking = false;
	}

	virtual ~TestFramework()
	{
		MK_DELETE_OBJECT(m_Font);
	}

protected:

	HWND _CreateControl(HWND hWnd, HINSTANCE hInstance, const wchar_t* type, unsigned int id, const wchar_t* caption, DWORD style, const MkIntRect& rect)
	{
		HWND hControl = CreateWindow
			(type, caption, style, rect.position.x, rect.position.y, rect.size.x, rect.size.y, hWnd, reinterpret_cast<HMENU>(id), hInstance, NULL);

		if (hControl != NULL)
		{
			SendMessage(hControl, WM_SETFONT, reinterpret_cast<WPARAM>(m_Font), 0);
		}

		return hControl;
	}

	bool _GetPathFromEditBox(HWND editBoxHandle, MkPathName& buffer)
	{
		wchar_t pathBuf[MAX_PATH + 1] = {0, };
		GetWindowText(editBoxHandle, pathBuf, MAX_PATH + 1);
		buffer = pathBuf;

		return ((!buffer.Empty()) && buffer.IsDirectoryPath() && buffer.CheckAvailable());
	}

protected:

	HFONT m_Font;

	HWND m_ResourceDirHandle;
	HWND m_TargetDirHandle;
	HWND m_SettingFileHandle;
	HWND m_PackingStartHandle;

	bool m_OnPacking;
	unsigned int m_ChunkSize;
	unsigned int m_Comp;
	MkStr m_PrefixStr;
	MkStr m_ExtStr;
	MkPathName m_ResPath;
	MkPathName m_TargetPath;
	MkPathName m_SettingPath;
};


// TestApplication
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
	application.Run(hI, L"MkFilePacker", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 600, 180, false, false, TestFramework::NewWndProc, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

