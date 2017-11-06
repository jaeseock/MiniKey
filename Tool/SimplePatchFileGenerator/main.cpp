
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"

#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkPatchFileGenerator.h"


#define MKDEF_APP_EDIT_RES_DIR_TAG_ID 1
#define MKDEF_APP_EDIT_RES_DIR_VALUE_ID 2
#define MKDEF_APP_BTN_FIND_RES_DIR_ID 3

#define MKDEF_APP_BTN_PACK_START_ID 4

const static MkPathName SettingFileName = L"PatchInfo.mmd";
const static MkHashStr KEY_StartPath = L"StartPath";
const static MkHashStr KEY_LauncherFileName = L"LauncherFileName";
const static MkHashStr KEY_RunFilePath = L"RunFilePath";
const static MkHashStr KEY_OutDirNames = L"OutDirNames";

static MkArray<int> gFrameworkEvent(32);


// single thread
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		MkStr startResPath;

		MkPathName settingFile;
		settingFile.ConvertToRootBasisAbsolutePath(SettingFileName);
		if (settingFile.CheckAvailable() &&
			m_SettingNode.Load(settingFile))
		{
			m_SettingNode.GetData(KEY_StartPath, startResPath, 0);
			startResPath.ReplaceKeyword(L"/", L"\\");

			MkStr tmpLFN;
			m_SettingNode.GetData(KEY_LauncherFileName, tmpLFN, 0);
			m_LauncherFileName = tmpLFN;

			MkStr tmpRFP;
			m_SettingNode.GetData(KEY_RunFilePath, tmpRFP, 0);
			m_RunFilePath = tmpRFP;

			m_SettingNode.GetData(KEY_OutDirNames, m_OutDirNames);
		}

		if (startResPath.Empty())
		{
			startResPath = MkPathName::GetModuleDirectory();
		}
		

		// 콘트롤 생성 준비
		m_Font = CreateFont
			(12, 0, 0, 0, 0, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH | FF_ROMAN, L"굴림");

		HWND hWnd = m_MainWindow.GetWindowHandle();
		HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE));

		const DWORD staticTextControlStyle = WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY;
		const DWORD inputControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT;
		const DWORD buttonControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON;

		const int DEF_WIDTH = 115;
		int currentY = 5;

		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_RES_DIR_TAG_ID, L" 리소스 폴더", staticTextControlStyle, MkIntRect(5, currentY, DEF_WIDTH, 12));

		currentY += 13;
		m_ResourceDirHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_RES_DIR_VALUE_ID, startResPath.GetPtr(), inputControlStyle, MkIntRect(5, currentY, 545, 20));
		
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FIND_RES_DIR_ID, L"변경", buttonControlStyle, MkIntRect(555, currentY, 40, 20));

		currentY += 30;
		m_PackingStartHandle = _CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_PACK_START_ID, L"패치 파일 생성", buttonControlStyle, MkIntRect(5, currentY, 590, 40));

		m_PG.SetParentWindowHandle(m_MainWindow.GetWindowHandle());

		return true;
	}

	virtual void StartLooping(void)
	{
		MK_DEV_PANEL.ClearLogWindow();

		m_PG.SetLauncherFileName(m_LauncherFileName);
		m_PG.SetRunFilePath(m_RunFilePath);
		m_PG.SetHistoryDirectoryPath(L"History");
		m_PG.SetPatchRootDirectoryPath(L"Patch");
		m_PG.SetUpdatingRootDirectoryPath(L"Update", m_OutDirNames);
	}

	virtual void Update(void)
	{
		if (m_OnPacking == 1)
		{
			if (m_PG.GeneratePatchFiles(m_ResRootPath))
			{
				m_ResRootPath[m_ResRootPath.GetSize() - 1] = L'/'; // L'\\' -> '/'

				if (m_SettingNode.IsValidKey(KEY_StartPath))
				{
					m_SettingNode.SetData(KEY_StartPath, m_ResRootPath, 0);
				}
				else
				{
					m_SettingNode.CreateUnit(KEY_StartPath, m_ResRootPath);
				}

				MkPathName settingFile;
				settingFile.ConvertToRootBasisAbsolutePath(SettingFileName);
				m_SettingNode.SaveToText(settingFile);

				m_OnPacking = 2;
			}
			else
			{
				m_OnPacking = 0;
				EnableWindow(m_PackingStartHandle, TRUE);
			}
		}
		else if (m_OnPacking == 2)
		{
			if (!m_PG.Update())
			{
				m_OnPacking = 0;
				EnableWindow(m_PackingStartHandle, TRUE);
			}
		}

		if (m_OnPacking == 0)
		{
			MK_INDEXING_LOOP(gFrameworkEvent, i)
			{
				switch (gFrameworkEvent[i])
				{
				case MKDEF_APP_BTN_FIND_RES_DIR_ID: // 리소스 폴더 선택 이벤트. 다이얼로그를 열어 선택
					{
						if (m_ResourceDirHandle != NULL)
						{
							MkPathName defPath;
							if (!_GetPathFromEditBox(m_ResourceDirHandle, defPath))
							{
								defPath = MkPathName::GetModuleDirectory();
							}

							MkPathName targetPath;
							if (targetPath.GetDirectoryPathFromDialog(L"리소스 폴더를 선택", m_MainWindow.GetWindowHandle(), defPath))
							{
								SetWindowText(m_ResourceDirHandle, targetPath.GetPtr());
							}
						}
					}
					break;

				case MKDEF_APP_BTN_PACK_START_ID: // 패킹 시작 버튼 이벤트
					{
						if ((m_ResourceDirHandle != NULL) && (m_PackingStartHandle != NULL))
						{
							m_ResRootPath.Clear();
							if (_GetPathFromEditBox(m_ResourceDirHandle, m_ResRootPath) && (!m_ResRootPath.Empty()))
							{
								m_OnPacking = 1;
								EnableWindow(m_PackingStartHandle, FALSE);
							}
							
							/*
							if (_GetPathFromEditBox(m_ResourceDirHandle, resPath) &&
								m_PG.GeneratePatchFiles(resPath))
							{
								resPath[resPath.GetSize() - 1] = L'/'; // L'\\' -> '/'

								if (m_SettingNode.IsValidKey(KEY_StartPath))
								{
									m_SettingNode.SetData(KEY_StartPath, resPath, 0);
								}
								else
								{
									m_SettingNode.CreateUnit(KEY_StartPath, resPath);
								}

								MkPathName settingFile;
								settingFile.ConvertToRootBasisAbsolutePath(SettingFileName);
								m_SettingNode.SaveToText(settingFile);

								m_OnPacking = true;
								EnableWindow(m_PackingStartHandle, FALSE);
							}
							*/
						}
					}
					break;
				}
			}
		}

		gFrameworkEvent.Clear();
	}

	virtual void Clear(void)
	{
		
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
		m_PackingStartHandle = NULL;
		m_OnPacking = 0;
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
		buffer.CheckAndAddBackslash();

		return ((!buffer.Empty()) && buffer.IsDirectoryPath() && buffer.CheckAvailable());
	}

protected:

	HFONT m_Font;

	MkDataNode m_SettingNode;
	MkPathName m_LauncherFileName;
	MkPathName m_RunFilePath;
	MkPathName m_ResRootPath;

	MkArray<MkStr> m_OutDirNames;

	HWND m_ResourceDirHandle;
	HWND m_PackingStartHandle;

	int m_OnPacking;
	
	MkPatchFileGenerator m_PG;
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
	application.Run(hI, L"SimplePatchFileGenerator", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 600, 92, false, true, TestFramework::NewWndProc,
		L"#DMK=_MkPatchFileGenerator; #BME=_MkPatchFileGenerator"); // 중복 실행 금지

	return 0;
}

//------------------------------------------------------------------------------------------------//

