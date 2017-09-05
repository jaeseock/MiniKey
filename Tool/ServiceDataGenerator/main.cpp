
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include <CommCtrl.h> // InitCommonControlsEx

#pragma comment (lib, "Comctl32.lib")

//#include "MkCore_MkCmdLine.h"
//#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkApplicationRegister.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#define MKDEF_SERVICES_FILENAME L"services.txt"
#define MKDEF_REGISTER_FILENAME L"AppServiceData.mmd"

#define MKDEF_APP_CB_SRV_ID 1
#define MKDEF_APP_BTN_START_ID 2

static MkHashStr gTargetService;
static bool gMakeData = false;


// single thread
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		MkArray<MkHashStr> services;
		if (!m_ServiceData.LoadServices(MKDEF_SERVICES_FILENAME, services))
		{
			MkStr msg = MkStr(MKDEF_SERVICES_FILENAME) + L" 파일 초기화 실패";
			::MessageBox(m_MainWindow.GetWindowHandle(), msg.GetPtr(), L"RegDataGenerator ERROR!", MB_OK);
			return false;
		}
		
		// ui 생성
		INITCOMMONCONTROLSEX icex;
		icex.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&icex);

		m_Font = CreateFont(12, 0, 0, 0, 0, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH | FF_ROMAN, L"굴림");

		m_ServiceComboBox = _CreateControl(WC_COMBOBOX, MKDEF_APP_CB_SRV_ID, L"", CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, MkIntRect(5, 5, 400, 400));

		MK_INDEXING_LOOP(services, i)
		{
			::SendMessage(m_ServiceComboBox, CB_ADDSTRING, (WPARAM)0, (LPARAM)services[i].GetPtr());
		}

		if (services.GetSize() == 1)
		{
			::SendMessage(m_ServiceComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			gTargetService = services[0];
		}

		m_MakeDataBtn = _CreateControl(L"button", MKDEF_APP_BTN_START_ID, L"설정 파일 생성", WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON, MkIntRect(5, 31, 400, 40));
		
		return true;
	}

	virtual void Update(void)
	{
		if (gMakeData)
		{
			if (gTargetService.Empty())
			{
				MK_DEV_PANEL.MsgToLog(L"대상 서비스를 선택하세요.");
			}
			else
			{
				if (m_MakeDataBtn != NULL)
				{
					::EnableWindow(m_MakeDataBtn, FALSE);
					MK_DEV_PANEL.MsgToLog(gTargetService.GetString() + L" 서비스 정보를 생성합니다.");

					if (m_ServiceData.SaveService(gTargetService, MKDEF_REGISTER_FILENAME))
					{
						MkStr msg = MkStr(MKDEF_REGISTER_FILENAME) + L" 파일 저장 성공";
						::MessageBox(m_MainWindow.GetWindowHandle(), msg.GetPtr(), L"RegDataGenerator", MB_OK);
					}
					else
					{
						MkStr msg = MkStr(MKDEF_REGISTER_FILENAME) + L" 파일 저장 실패";
						::MessageBox(m_MainWindow.GetWindowHandle(), msg.GetPtr(), L"RegDataGenerator ERROR!", MB_OK);
					}

					::EnableWindow(m_MakeDataBtn, TRUE);
				}
			}
			gMakeData = false;
		}
	}

	virtual void Clear(void)
	{
		::DestroyWindow(m_ServiceComboBox);
		::DestroyWindow(m_MakeDataBtn);
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
					switch (LOWORD(wParam))
					{
					case MKDEF_APP_BTN_START_ID:
						gMakeData = true;
						break;
					}
				}
				break;

			case CBN_SELCHANGE:
				{
					switch (LOWORD(wParam))
					{
					case MKDEF_APP_CB_SRV_ID:
						{
							HWND handle = (HWND)lParam;
							DWORD index = ::SendMessage(handle, CB_GETCURSEL, 0, 0);
							wchar_t buffer[512] = {0, };
							::SendMessage(handle, CB_GETLBTEXT, index, (LPARAM)buffer);
							gTargetService = buffer;
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
		m_ServiceComboBox = NULL;
		m_MakeDataBtn = NULL;
	}

	virtual ~TestFramework()
	{
		MK_DELETE_OBJECT(m_Font);
	}

protected:

	HWND _CreateControl(const wchar_t* type, unsigned int id, const wchar_t* caption, DWORD style, const MkIntRect& rect)
	{
		HWND hControl = ::CreateWindow
			(type, caption, style, rect.position.x, rect.position.y, rect.size.x, rect.size.y,
			m_MainWindow.GetWindowHandle(), reinterpret_cast<HMENU>(id), ::GetModuleHandle(NULL), NULL);

		if (hControl != NULL)
		{
			::SendMessage(hControl, WM_SETFONT, reinterpret_cast<WPARAM>(m_Font), 0);
		}

		return hControl;
	}

protected:

	HFONT m_Font;
	HWND m_ServiceComboBox;
	HWND m_MakeDataBtn;

	MkApplicationRegister::GenerateServiceData m_ServiceData;
};


class TestApplication : public MkWin32Application
{
public:
	virtual MkBaseFramework* CreateFramework(void) const { return new TestFramework; }

	TestApplication() : MkWin32Application() {}
	virtual ~TestApplication() {}
};

//------------------------------------------------------------------------------------------------//

// 엔트리 포인트에서의 TestApplication 실행
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"RegDataGenerator", L"", true, eSWP_FixedSize, CW_USEDEFAULT, CW_USEDEFAULT, 410, 76, false, true, TestFramework::NewWndProc,
		L"#DMK=_RegDataGenerator; #BME=_RegDataGenerator");

	return 0;
}

//------------------------------------------------------------------------------------------------//

