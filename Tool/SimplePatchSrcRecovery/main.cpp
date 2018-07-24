
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"

#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkPatchFileGenerator.h"
#include "MkCore_MkPatchSrcRecovery.h"

#define MKDEF_APP_EDIT_URL_TAG_ID 1
#define MKDEF_APP_EDIT_URL_VALUE_ID 2

#define MKDEF_APP_EDIT_GEN_DIR_TAG_ID 3
#define MKDEF_APP_EDIT_GEN_DIR_VALUE_ID 4
#define MKDEF_APP_BTN_FIND_GEN_DIR_ID 5

#define MKDEF_APP_EDIT_SRC_DIR_TAG_ID 6
#define MKDEF_APP_EDIT_SRC_DIR_VALUE_ID 7
#define MKDEF_APP_BTN_FIND_SRC_DIR_ID 8

#define MKDEF_APP_BTN_RECOVERY 9


static MkArray<int> gFrameworkEvent(32);

// single thread
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		m_Font = CreateFont
			(12, 0, 0, 0, 0, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH | FF_ROMAN, L"����");

		HWND hWnd = m_MainWindow.GetWindowHandle();
		HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE));

		const DWORD staticTextControlStyle = WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY;
		const DWORD inputControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT;
		const DWORD buttonControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON;

		const int DEF_WIDTH = 160;
		int currentY = 5;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_URL_TAG_ID, L" �ٿ�ε� URL", staticTextControlStyle, MkIntRect(5, currentY, DEF_WIDTH, 12));

		currentY += 13;
		m_DownloadUrlEB = _CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_URL_VALUE_ID, L"", inputControlStyle, MkIntRect(5, currentY, 590, 20));

		currentY += 30;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_GEN_DIR_TAG_ID, L" ��ġ ������ ���丮 ���", staticTextControlStyle, MkIntRect(5, currentY, DEF_WIDTH, 12));

		currentY += 13;
		m_GeneratorPathEB = _CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_GEN_DIR_VALUE_ID, L"", inputControlStyle, MkIntRect(5, currentY, 545, 20));

		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FIND_GEN_DIR_ID, L"����", buttonControlStyle, MkIntRect(555, currentY, 40, 20));

		currentY += 30;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_SRC_DIR_TAG_ID, L" ��ġ ���� ���丮 ���", staticTextControlStyle, MkIntRect(5, currentY, DEF_WIDTH, 12));

		currentY += 13;
		m_SourcePathEB = _CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_SRC_DIR_VALUE_ID, L"", inputControlStyle, MkIntRect(5, currentY, 545, 20));

		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FIND_SRC_DIR_ID, L"����", buttonControlStyle, MkIntRect(555, currentY, 40, 20));

		currentY += 30;
		m_RecoveryBtn = _CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_RECOVERY, L"���� ����", buttonControlStyle, MkIntRect(5, currentY, 590, 40));

		return true;
	}

	virtual void StartLooping(void)
	{
		MK_DEV_PANEL.ClearLogWindow();
	}

	virtual void Update(void)
	{
		m_Recovery.Update();

		switch (m_Recovery.GetMainState())
		{
		case MkPatchSrcRecovery::eReady:
			{
				MK_INDEXING_LOOP(gFrameworkEvent, i)
				{
					switch (gFrameworkEvent[i])
					{
					case MKDEF_APP_BTN_FIND_GEN_DIR_ID: // ��ġ ������ ���丮 ���� �̺�Ʈ. ���̾�α׸� ���� ����
						{
							if (m_GeneratorPathEB != NULL)
							{
								MkPathName defPath;
								if (!_GetPathFromEditBox(m_GeneratorPathEB, defPath))
								{
									defPath = MkPathName::GetModuleDirectory();
								}

								MkPathName targetPath;
								if (targetPath.GetDirectoryPathFromDialog(L"��ġ ������ ���丮�� ����", m_MainWindow.GetWindowHandle(), defPath))
								{
									SetWindowText(m_GeneratorPathEB, targetPath.GetPtr());
								}
							}
						}
						break;

					case MKDEF_APP_BTN_FIND_SRC_DIR_ID: // ��ġ ���� ���丮 ���� �̺�Ʈ. ���̾�α׸� ���� ����
						{
							if (m_SourcePathEB != NULL)
							{
								MkPathName defPath;
								if (!_GetPathFromEditBox(m_SourcePathEB, defPath))
								{
									defPath = MkPathName::GetModuleDirectory();
								}

								MkPathName targetPath;
								if (targetPath.GetDirectoryPathFromDialog(L"��ġ ���� ���丮�� ����", m_MainWindow.GetWindowHandle(), defPath))
								{
									SetWindowText(m_SourcePathEB, targetPath.GetPtr());
								}
							}
						}
						break;

					case MKDEF_APP_BTN_RECOVERY:
						{
							if ((m_DownloadUrlEB != NULL) && (m_GeneratorPathEB != NULL) && (m_SourcePathEB != NULL))
							{
								m_DownloadURL.Clear();
								m_PatchGeneratorDirPath.Clear();
								m_PatchSourceDirPath.Clear();

								if (_GetUrlFromEditBox(m_DownloadUrlEB, m_DownloadURL) &&
									_GetPathFromEditBox(m_GeneratorPathEB, m_PatchGeneratorDirPath) &&
									_GetPathFromEditBox(m_SourcePathEB, m_PatchSourceDirPath))
								{
									m_DownloadURL.RemoveBlank();

									if (m_Recovery.RecoverPatchSource(m_DownloadURL, m_PatchGeneratorDirPath, L"History", L"Patch", m_PatchSourceDirPath))
									{
										if (m_RecoveryBtn != NULL)
										{
											EnableWindow(m_RecoveryBtn, FALSE);
										}
									}
								}
							}
						}
						break;
					}
				}

				gFrameworkEvent.Clear();
			}
			break;

		case MkPatchSrcRecovery::eShowSuccessResult:
			{
				const MkDataNode& historyNode = m_Recovery.GetHistoryNode();

				// patch generator�� info ������ ����
				// Key�� [SimplePatchFileGenerator] ����

				// RunFilePath
				MkDataNode patchGenInfoNode;
				MkStr rfp;
				if (historyNode.GetData(MkPatchFileGenerator::KEY_RunFilePath, rfp, 0))
				{
					patchGenInfoNode.CreateUnit(L"RunFilePath", rfp);
				}

				// LauncherFileName
				MkStr launcherFileName = MkPatchFileGenerator::LauncherFileName.GetString() + L"." + MkDataNode::DefaultFileExtension; // ex> LauncherFile.mmd
				MkPathName launcherFileNameDown;
				MkPatchFileGenerator::ConvertFilePathToDownloadable(launcherFileName, launcherFileNameDown); // ex> LauncherFile.mmd.rp

				MkDataNode launcherNode;
				if (launcherNode.Load(m_PatchGeneratorDirPath + L"Patch\\" + launcherFileNameDown))
				{
					MkStr lfn;
					if (launcherNode.GetData(MkPatchFileGenerator::LauncherFileName, lfn, 0))
					{
						patchGenInfoNode.CreateUnit(L"LauncherFileName", lfn);
					}
				}

				// StartPath
				MkStr sp = m_PatchSourceDirPath;
				sp.BackSpace(1);
				sp += L"/";
				patchGenInfoNode.CreateUnit(L"StartPath", sp);

				// [EXPORT]
				MkDataNode* exportNode = patchGenInfoNode.CreateChildNode(L"[EXPORT]");
				MkDataNode* ftpNode = exportNode->CreateChildNode(L"Target FTP"); // �ӽø�

				MkStr targetURL = m_DownloadURL; // ex> L"http://210.207.252.151/main/SteamQA";
				if (targetURL.CheckPrefix(L"http://"))
				{
					targetURL.PopFront(7); // ex> L"210.207.252.151/main/SteamQA";
				}

				MkStr domain, remotePath;
				unsigned int splitPos = targetURL.GetFirstKeywordPosition(L"/");

				// ex> L"210.207.252.151"
				if (splitPos == MKDEF_ARRAY_ERROR)
				{
					domain = targetURL;
				}
				// ex> L"210.207.252.151/"
				else if ((splitPos + 1) == targetURL.GetSize())
				{
					domain = targetURL;
					domain.BackSpace(1); // L"210.207.252.151
				}
				// ex> L"210.207.252.151/main/SteamQA";
				else
				{
					targetURL.GetSubStr(MkArraySection(0, splitPos), domain); // L"210.207.252.151
					targetURL.GetSubStr(MkArraySection(splitPos + 1), remotePath); // main/SteamQA
				}

				ftpNode->CreateUnit(L"URL", domain);
				ftpNode->CreateUnit(L"RemotePath", remotePath);
				ftpNode->CreateUnit(L"UserName", MkStr::EMPTY);
				ftpNode->CreateUnit(L"Password", MkStr::EMPTY);
				ftpNode->CreateUnit(L"Warning", false);

				// save
				MkPathName piFilePath = m_PatchGeneratorDirPath + L"PatchInfo.mmd";
				patchGenInfoNode.SaveToText(piFilePath);

				MkStr msg;
				msg.Reserve(512);
				msg += L"������ �Ϸ�Ǿ����ϴ�.";
				msg += MkStr::CRLF;
				msg += MkStr::CRLF;
				msg += piFilePath;
				msg += MkStr::CRLF;
				msg += L"���Ͽ� ���ε�� ID/PW�� �Է����ּ���.";

				::MessageBox(NULL, msg, L"SUCCESS", MB_OK);

				m_Recovery.ResetMainState();

				if (m_RecoveryBtn != NULL)
				{
					EnableWindow(m_RecoveryBtn, TRUE);
				}
			}
			break;

		case MkPatchSrcRecovery::eShowFailedResult:
			{
				MkStr msg;
				msg.Reserve(512);
				msg += L"���� ������ ������ �־� �����Ͽ����ϴ�.";
				msg += MkStr::CRLF;
				msg += L"Ȯ���� �ٽ� �õ����ּ���.";

				::MessageBox(NULL, msg, L"FAIL", MB_OK);

				m_Recovery.ResetMainState();

				if (m_RecoveryBtn != NULL)
				{
					EnableWindow(m_RecoveryBtn, TRUE);
				}
			}
			break;
		}
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
					case MKDEF_APP_BTN_FIND_GEN_DIR_ID:
					case MKDEF_APP_BTN_FIND_SRC_DIR_ID:
					case MKDEF_APP_BTN_RECOVERY:
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
		m_DownloadUrlEB = NULL;
		m_GeneratorPathEB = NULL;
		m_SourcePathEB = NULL;
		m_RecoveryBtn = NULL;
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

	bool _GetUrlFromEditBox(HWND editBoxHandle, MkStr& buffer)
	{
		wchar_t pathBuf[MAX_PATH + 1] = {0, };
		GetWindowText(editBoxHandle, pathBuf, MAX_PATH + 1);
		buffer = pathBuf;

		return (!buffer.Empty());
	}

	bool _GetPathFromEditBox(HWND editBoxHandle, MkPathName& buffer)
	{
		wchar_t pathBuf[MAX_PATH + 1] = {0, };
		GetWindowText(editBoxHandle, pathBuf, MAX_PATH + 1);
		buffer = pathBuf;
		buffer.CheckAndAddBackslash();

		return ((!buffer.Empty()) && buffer.IsDirectoryPath());
	}

protected:

	HFONT m_Font;

	HWND m_DownloadUrlEB;
	HWND m_GeneratorPathEB;
	HWND m_SourcePathEB;

	HWND m_RecoveryBtn;

	MkStr m_DownloadURL;
	MkPathName m_PatchGeneratorDirPath;
	MkPathName m_PatchSourceDirPath;

	MkPatchSrcRecovery m_Recovery;
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
	application.Run(hI, L"SimplePatchSrcRecovery", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 600, 178, false, false, TestFramework::NewWndProc,
		L"#DMK=_MkPatchSrcRecovery; #BME=_MkPatchSrcRecovery"); // �ߺ� ���� ����

	return 0;
}

//------------------------------------------------------------------------------------------------//

