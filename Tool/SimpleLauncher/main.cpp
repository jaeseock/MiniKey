
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include <CommCtrl.h>

#include "MkCore_MkCmdLine.h"
#include "MkCore_MkDevPanel.h"

#include "MkCore_MkLayeredWindow.h"

#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkPatchFileGenerator.h"
#include "MkCore_MkPatchStarter.h"
#include "MkCore_MkPatchFileDownloader.h"

#define MKDEF_APP_EDIT_MAIN_ID 1
#define MKDEF_APP_PROG_MAIN_ID 2
#define MKDEF_APP_EDIT_SUB_ID 3
#define MKDEF_APP_PROG_SUB_ID 4

#define MKDEF_APP_BTN_START_ID 5

static bool g_RunGame = false;
static MkLayeredWindow g_LayeredWindow;


// single thread
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		g_LayeredWindow.Clear();

		// patch url 검사
		MkStr patchUrlKey = MKDEF_PATCH_DOWNLOAD_URL_KEY;
		if (cmdLine.HasPair(patchUrlKey))
		{
			m_PatchURL = cmdLine.GetValue(patchUrlKey, 0);
		}

		if (m_PatchURL.Empty())
		{
			::MessageBox(NULL, L"No URL.", L"SimpleLauncher", MB_OK);
			return false; // quit
		}

		// launcher가 app을 실행하므로 복수 실행 예외 처리
		MkCmdLine newCmd = cmdLine;
		newCmd.AddPair(L"#AME", L"_MkGameApp");
		newCmd.GetFullStr(m_Arg);

		// ui 생성
		m_Font = CreateFont
			(12, 0, 0, 0, 0, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH | FF_ROMAN, L"굴림");

		HWND hWnd = m_MainWindow.GetWindowHandle();
		HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE));

		const DWORD staticTextControlStyle = WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY;
		const DWORD buttonControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON;

		int currentY = 5;
		m_MainDescHandle = _CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_MAIN_ID, L"", staticTextControlStyle, MkIntRect(5, currentY, 200, 12));

		currentY += 18;
		m_MainProgHandle = ::CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE, 5, currentY, 400, 15, hWnd, reinterpret_cast<HMENU>(MKDEF_APP_PROG_MAIN_ID), hInstance, NULL);
		::SendMessage(m_MainProgHandle, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
		::SendMessage(m_MainProgHandle, PBM_SETPOS, 0, 0);

		currentY += 30;
		m_SubDescHandle = _CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_SUB_ID, L"", staticTextControlStyle, MkIntRect(5, currentY, 200, 12));

		currentY += 18;
		m_SubProgHandle = ::CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE, 5, currentY, 400, 15, hWnd, reinterpret_cast<HMENU>(MKDEF_APP_PROG_SUB_ID), hInstance, NULL);

		currentY += 30;
		m_RunBtnHandle = _CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_START_ID, L"게임 시작", buttonControlStyle, MkIntRect(5, currentY, 400, 40));

		// 강제로 foreground로 보냄
		HWND frontWnd = ::GetForegroundWindow();
		HWND myWnd = m_MainWindow.GetWindowHandle();
		if ((frontWnd != NULL) && (frontWnd != myWnd))
		{
			DWORD frontThreadID = ::GetWindowThreadProcessId(frontWnd, NULL);
			DWORD myThreadID= ::GetCurrentThreadId();
			if (myThreadID != frontThreadID)
			{
				if (::AttachThreadInput(myThreadID, frontThreadID, TRUE))
				{
					::SetForegroundWindow(myWnd);
					::BringWindowToTop(myWnd);
					::AttachThreadInput(myThreadID, frontThreadID, FALSE);
				}
			}
		}
		
		return true;
	}

	virtual void StartLooping(void)
	{
		MK_DEV_PANEL.ClearLogWindow();

		MK_DEV_PANEL.InsertEmptyLine();
		MK_DEV_PANEL.MsgToLog(L"> Download URL :");
		MK_DEV_PANEL.MsgToLog(L"  " + m_PatchURL);
		MK_DEV_PANEL.InsertEmptyLine();

		m_Patcher.CheckAndDownloadPatchFile(m_PatchURL);
	}

	virtual void Update(void)
	{
		if (g_RunGame)
		{
			if (m_RunBtnHandle != NULL)
			{
				EnableWindow(m_RunBtnHandle, FALSE);
			}

			MkPathName runPath = m_Patcher.GetRunFilePath();
			runPath.OpenFileInVerb(m_Arg);

			g_RunGame = false;
			MkBaseFramework::Close();
		}
		else
		{
			m_Patcher.Update();

			MkPatchFileDownloader::ePatchState currState = m_Patcher.GetCurrentState();
			if (currState == m_PatchState)
			{
				MkStr subDesc;
				switch (currState)
				{
				case MkPatchFileDownloader::eReady:
					break;

				case MkPatchFileDownloader::eDownloadPatchInfoFile:
					break;

				case MkPatchFileDownloader::ePurgeDeleteList:
					break;

				case MkPatchFileDownloader::eFindDownloadTargets:
					break;

				case MkPatchFileDownloader::eDownloadTargetFiles:
					::SendMessage(m_SubProgHandle, PBM_SETPOS, m_Patcher.GetCurrentProgress(), 0);
					subDesc = MkStr(m_Patcher.GetCurrentProgress()) + L" / " + MkStr(m_Patcher.GetMaxProgress());
					break;

				case MkPatchFileDownloader::eUpdateFiles:
					::SendMessage(m_SubProgHandle, PBM_SETPOS, m_Patcher.GetCurrentProgress(), 0);
					subDesc = MkStr(m_Patcher.GetCurrentProgress()) + L" / " + MkStr(m_Patcher.GetMaxProgress());
					break;

				case MkPatchFileDownloader::eOptimizeChunk:
					::SendMessage(m_SubProgHandle, PBM_SETPOS, m_Patcher.GetCurrentProgress(), 0);
					subDesc = MkStr(m_Patcher.GetCurrentProgress()) + L" / " + MkStr(m_Patcher.GetMaxProgress());
					break;

				case MkPatchFileDownloader::eShowSuccessResult:
				case MkPatchFileDownloader::eShowFailedResult:
					break;
				}

				::SetWindowText(m_SubDescHandle, subDesc.GetPtr());
			}
			else
			{
				MkStr mainDesc;
				MkStr subDesc;
				switch (currState)
				{
				case MkPatchFileDownloader::eReady:
					break;

				case MkPatchFileDownloader::eDownloadPatchInfoFile:
					mainDesc = L" 설정 파일 다운";
					break;

				case MkPatchFileDownloader::ePurgeDeleteList:
					mainDesc = L" 파일 삭제";
					::SendMessage(m_MainProgHandle, PBM_SETPOS, 5, 0);
					break;

				case MkPatchFileDownloader::eFindDownloadTargets:
					mainDesc = L" 다운 대상 파일 선별";
					::SendMessage(m_MainProgHandle, PBM_SETPOS, 10, 0);
					break;

				case MkPatchFileDownloader::eDownloadTargetFiles:
					mainDesc = L" 패치 파일 다운로드";
					::SendMessage(m_MainProgHandle, PBM_SETPOS, 20, 0);
					if (m_Patcher.GetMaxProgress() == 0)
					{
						::SendMessage(m_SubProgHandle, PBM_SETRANGE, 0, MAKELPARAM(0, 1));
						::SendMessage(m_SubProgHandle, PBM_SETPOS, 1, 0);
					}
					else
					{
						::SendMessage(m_SubProgHandle, PBM_SETRANGE, 0, MAKELPARAM(0, m_Patcher.GetMaxProgress()));
						subDesc = L"0 / " + MkStr(m_Patcher.GetMaxProgress());
					}
					break;

				case MkPatchFileDownloader::eUpdateFiles:
					mainDesc = L" 패치 파일 적용";
					::SendMessage(m_MainProgHandle, PBM_SETPOS, 70, 0);
					if (m_Patcher.GetMaxProgress() == 0)
					{
						::SendMessage(m_SubProgHandle, PBM_SETRANGE, 0, MAKELPARAM(0, 1));
						::SendMessage(m_SubProgHandle, PBM_SETPOS, 1, 0);
					}
					else
					{
						::SendMessage(m_SubProgHandle, PBM_SETRANGE, 0, MAKELPARAM(0, m_Patcher.GetMaxProgress()));
						subDesc = L"0 / " + MkStr(m_Patcher.GetMaxProgress());
					}
					break;

				case MkPatchFileDownloader::eOptimizeChunk:
					mainDesc = L" 파일 시스템 최적화";
					::SendMessage(m_MainProgHandle, PBM_SETPOS, 80, 0);
					if (m_Patcher.GetMaxProgress() == 0)
					{
						::SendMessage(m_SubProgHandle, PBM_SETRANGE, 0, MAKELPARAM(0, 1));
						::SendMessage(m_SubProgHandle, PBM_SETPOS, 1, 0);
					}
					else
					{
						::SendMessage(m_SubProgHandle, PBM_SETRANGE, 0, MAKELPARAM(0, m_Patcher.GetMaxProgress()));
						subDesc = L"0 / " + MkStr(m_Patcher.GetMaxProgress());
					}
					break;

				case MkPatchFileDownloader::eShowSuccessResult:
					mainDesc = L" 완료";
					::SendMessage(m_MainProgHandle, PBM_SETPOS, 100, 0);
					break;

				case MkPatchFileDownloader::eShowFailedResult:
					mainDesc = L" 실패";
					::SendMessage(m_MainProgHandle, PBM_SETPOS, 100, 0);
					break;
				}

				if (!mainDesc.Empty())
				{
					::SetWindowText(m_MainDescHandle, mainDesc.GetPtr());
				}

				::SetWindowText(m_SubDescHandle, subDesc.GetPtr());

				m_PatchState = currState;
			}
		}
	}

	virtual void Clear(void)
	{
		::DestroyWindow(m_MainDescHandle);
		::DestroyWindow(m_MainProgHandle);
		::DestroyWindow(m_SubDescHandle);
		::DestroyWindow(m_SubProgHandle);
		::DestroyWindow(m_RunBtnHandle);
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
					case MKDEF_APP_BTN_START_ID:
						g_RunGame = true;
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
		m_MainDescHandle = NULL;
		m_MainProgHandle = NULL;
		m_SubDescHandle = NULL;
		m_SubProgHandle = NULL;
		m_RunBtnHandle = NULL;
		m_PatchState = MkPatchFileDownloader::eReady;
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
	HWND m_MainDescHandle;
	HWND m_MainProgHandle;
	HWND m_SubDescHandle;
	HWND m_SubProgHandle;
	HWND m_RunBtnHandle;

	MkStr m_Arg;
	MkStr m_PatchURL;

	MkPatchFileDownloader m_Patcher;
	MkPatchFileDownloader::ePatchState m_PatchState;
};


class TestApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new TestFramework; }

	TestApplication() : MkWin32Application() {}
	virtual ~TestApplication() {}

protected:

	virtual bool _CheckExcution(MkCmdLine& cmdLine, HANDLE& myMutexHandle, const wchar_t* myTitle)
	{
		if (!MkWin32Application::_CheckExcution(cmdLine, myMutexHandle, myTitle))
			return false;

		g_LayeredWindow.SetUp(L"SimpleLauncherIntroImage", L"launcher.png");
		return true;
	}
};

//------------------------------------------------------------------------------------------------//

// 엔트리 포인트에서의 TestApplication 실행
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"SimpleLauncher", L"", true, eSWP_FixedSize, CW_USEDEFAULT, CW_USEDEFAULT, 410, 146, false, true, TestFramework::NewWndProc,
		L"#DMK=_MkLauncher; #BME=_MkStarter; #BME=_MkLauncher; #BME=_MkGameApp");

	return 0;
}

//------------------------------------------------------------------------------------------------//

