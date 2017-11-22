
//------------------------------------------------------------------------------------------------//

#include <windows.h>
#include <CommCtrl.h> // InitCommonControlsEx

#pragma comment (lib, "Comctl32.lib")

#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"

#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkPatchFileGenerator.h"
#include "MkCore_MkPatchFileExtractor.h"


#define MKDEF_APP_EDIT_START_TAG_ID 1
#define MKDEF_APP_CBOX_START_ID 2
#define MKDEF_APP_EDIT_END_TAG_ID 3
#define MKDEF_APP_CBOX_END_ID 4
#define MKDEF_APP_BTN_FIND_ID 5
#define MKDEF_APP_BTN_EXTRACT_ID 6

//#define MKDEF_APP_BTN_PACK_START_ID 4
//#define MKDEF_APP_CBOX_UPLOAD_TARGET_ID 5

const static MkPathName SettingFileName = L"PatchInfo.mmd";
const static MkHashStr KEY_StartPath = L"StartPath";

static MkArray<int> gFrameworkEvent(32);

// single thread
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		// 콘트롤 생성 준비
		INITCOMMONCONTROLSEX icex;
		icex.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&icex);		

		m_Font = CreateFont
			(12, 0, 0, 0, 0, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH | FF_ROMAN, L"굴림");

		HWND hWnd = m_MainWindow.GetWindowHandle();
		HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE));

		const DWORD staticTextControlStyle = WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY;
		const DWORD buttonControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON;

		const int DEF_WIDTH = 115;
		int currentY = 5;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_START_TAG_ID, L" 여기부터~", staticTextControlStyle, MkIntRect(5, currentY, DEF_WIDTH, 12));

		currentY += 13;
		m_StartComboBox = _CreateControl
			(hWnd, hInstance, WC_COMBOBOX, MKDEF_APP_CBOX_START_ID, L"", CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, MkIntRect(5, currentY, 590, 400));

		currentY += 30;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_START_TAG_ID, L" 여기까지!", staticTextControlStyle, MkIntRect(5, currentY, DEF_WIDTH, 12));

		currentY += 13;
		m_EndComboBox = _CreateControl
			(hWnd, hInstance, WC_COMBOBOX, MKDEF_APP_CBOX_END_ID, L"", CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, MkIntRect(5, currentY, 590, 400));

		currentY += 40;
		m_FindBtn = _CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FIND_ID, L"변경점 탐색", buttonControlStyle, MkIntRect(5, currentY, 590, 30));
		if (m_FindBtn != NULL)
		{
			EnableWindow(m_FindBtn, FALSE);
		}

		currentY += 40;
		m_ExtractBtn = _CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_EXTRACT_ID, L"파일 추출", buttonControlStyle, MkIntRect(5, currentY, 590, 30));
		if (m_ExtractBtn != NULL)
		{
			EnableWindow(m_ExtractBtn, FALSE);
		}

		return true;
	}

	virtual void StartLooping(void)
	{
		MK_DEV_PANEL.ClearLogWindow();

		MkStr startResPath;
		MkDataNode settingNode;

		MkPathName settingFile;
		settingFile.ConvertToRootBasisAbsolutePath(SettingFileName);
		if (settingFile.CheckAvailable() &&
			settingNode.Load(settingFile))
		{
			settingNode.GetData(KEY_StartPath, startResPath, 0);
			startResPath.ReplaceKeyword(L"/", L"\\");
			settingNode.Clear();
		}

		if (startResPath.Empty())
		{
			startResPath = MkPathName::GetModuleDirectory();
		}

		if (m_PE.SetDirectoryPath(L"History", startResPath) && (m_StartComboBox != NULL))
		{
			const MkArray<unsigned int>& historyList = m_PE.GetHistoryList();
			if (!historyList.Empty())
			{
				MK_INDEXING_LOOP(historyList, i)
				{
					::SendMessage(m_StartComboBox, CB_ADDSTRING, (WPARAM)0, (LPARAM)MkPatchFileGenerator::ConvertWrittenTimeToStr(historyList[i]).GetPtr());
				}
				::SendMessage(m_StartComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			}
		}
	}

	virtual void Update(void)
	{
		MK_INDEXING_LOOP(gFrameworkEvent, i)
		{
			switch (gFrameworkEvent[i])
			{
			case MKDEF_APP_CBOX_START_ID:
				{
					++i;
					int index = gFrameworkEvent[i];
					if (m_EndComboBox != NULL)
					{
						::SendMessage(m_EndComboBox, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);

						const MkArray<unsigned int>& historyList = m_PE.GetHistoryList();

						MkArray<unsigned int> destList;
						if (historyList.GetSubArray(MkArraySection(0, index + 1), destList))
						{
							MK_INDEXING_LOOP(destList, j)
							{
								::SendMessage(m_EndComboBox, CB_ADDSTRING, (WPARAM)0, (LPARAM)MkPatchFileGenerator::ConvertWrittenTimeToStr(destList[j]).GetPtr());
							}
							::SendMessage(m_EndComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

							EnableWindow(m_FindBtn, TRUE);
							EnableWindow(m_ExtractBtn, FALSE);
						}
					}
				}
				break;

			case MKDEF_APP_CBOX_END_ID:
				{
					++i;
					EnableWindow(m_FindBtn, TRUE);
					EnableWindow(m_ExtractBtn, FALSE);
				}
				break;

			case MKDEF_APP_BTN_FIND_ID: // 탐색 시작 버튼 이벤트
				{
					if ((m_StartComboBox != NULL) && (m_EndComboBox != NULL))
					{
						DWORD startIndex = ::SendMessage(m_StartComboBox, CB_GETCURSEL, 0, 0);
						DWORD endIndex = ::SendMessage(m_EndComboBox, CB_GETCURSEL, 0, 0);

						bool ok = m_PE.MakeUpdateHistory(static_cast<unsigned int>(startIndex), static_cast<unsigned int>(endIndex));
						EnableWindow(m_ExtractBtn, (ok) ? TRUE : FALSE);
					}
				}
				break;

			case MKDEF_APP_BTN_EXTRACT_ID: // 추출 시작 버튼 이벤트
				{
					m_PE.Extract(L"ExtractionDest");

					EnableWindow(m_FindBtn, FALSE);
					EnableWindow(m_ExtractBtn, FALSE);
				}
				break;
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
					case MKDEF_APP_BTN_FIND_ID:
					case MKDEF_APP_BTN_EXTRACT_ID:
						gFrameworkEvent.PushBack(target);
						break;
					}
				}
				break;

			case CBN_SELCHANGE:
				{
					int target = static_cast<int>(LOWORD(wParam));
					switch (target)
					{
					case MKDEF_APP_CBOX_START_ID:
					case MKDEF_APP_CBOX_END_ID:
						{
							HWND handle = (HWND)lParam;
							DWORD index = ::SendMessage(handle, CB_GETCURSEL, 0, 0);
							gFrameworkEvent.PushBack(target);
							gFrameworkEvent.PushBack(static_cast<int>(index));
						}
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
		m_StartComboBox = NULL;
		m_EndComboBox = NULL;
		m_FindBtn = NULL;
		m_ExtractBtn = NULL;
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

protected:

	HFONT m_Font;
	
	HWND m_StartComboBox;
	HWND m_EndComboBox;

	HWND m_FindBtn;
	HWND m_ExtractBtn;

	MkPatchFileExtractor m_PE;
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
	application.Run(hI, L"SimplePatchFileExtractor", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 600, 175, false, false, TestFramework::NewWndProc,
		L"#DMK=_MkPatchFileExtractor; #BME=_MkPatchFileExtractor"); // 중복 실행 금지

	return 0;
}

//------------------------------------------------------------------------------------------------//

