
//------------------------------------------------------------------------------------------------//

#include <windows.h>
#include <CommCtrl.h> // InitCommonControlsEx

#pragma comment (lib, "Comctl32.lib")

#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"

#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkPatchFileGenerator.h"
#include "MkCore_MkPatchFileUploader.h"


#define MKDEF_APP_EDIT_RES_DIR_TAG_ID 1
#define MKDEF_APP_EDIT_RES_DIR_VALUE_ID 2
#define MKDEF_APP_BTN_FIND_RES_DIR_ID 3

#define MKDEF_APP_BTN_PACK_START_ID 4

#define MKDEF_APP_CBOX_UPLOAD_TARGET_ID 5
#define MKDEF_APP_BTN_UPLOAD_START_ID 6

const static MkPathName SettingFileName = L"PatchInfo.mmd";
const static MkHashStr KEY_StartPath = L"StartPath";
const static MkHashStr KEY_LauncherFileName = L"LauncherFileName";
const static MkHashStr KEY_RunFilePath = L"RunFilePath";
const static MkHashStr KEY_TryCompress = L"TryCompress";

const static MkHashStr KEY_ExportNodeName = L"[EXPORT]";
const static MkHashStr KEY_DestURL = L"URL";
const static MkHashStr KEY_DestRemotePath = L"RemotePath";
const static MkHashStr KEY_DestUserName = L"UserName";
const static MkHashStr KEY_DestPassword = L"Password";
const static MkHashStr KEY_PassiveMode = L"PassiveMode";
const static MkHashStr KEY_Warning = L"Warning";

static MkHashStr gTargetExport;

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

			m_SettingNode.GetData(KEY_TryCompress, m_TryCompress, 0);

			if (m_SettingNode.ChildExist(KEY_ExportNodeName))
			{
				const MkDataNode& serviceDestNode = *m_SettingNode.GetChildNode(KEY_ExportNodeName);
				MkArray<MkHashStr> dests;
				serviceDestNode.GetChildNodeList(dests);

				MK_INDEXING_LOOP(dests, i)
				{
					m_OutDirNames.PushBack(dests[i].GetString());
				}
			}
		}

		if (startResPath.Empty())
		{
			startResPath = MkPathName::GetModuleDirectory();
		}

		// ��Ʈ�� ���� �غ�
		INITCOMMONCONTROLSEX icex;
		icex.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&icex);		

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
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_RES_DIR_VALUE_ID, startResPath.GetPtr(), inputControlStyle, MkIntRect(5, currentY, 545, 20));
		
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FIND_RES_DIR_ID, L"����", buttonControlStyle, MkIntRect(555, currentY, 40, 20));

		currentY += 30;
		m_PackingStartHandle = _CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_PACK_START_ID, L"��ġ ���� ����", buttonControlStyle, MkIntRect(5, currentY, 590, 40));

		m_PG.SetParentWindowHandle(m_MainWindow.GetWindowHandle());

		currentY += 70;
		m_TargetComboBox = _CreateControl(hWnd, hInstance, WC_COMBOBOX, MKDEF_APP_CBOX_UPLOAD_TARGET_ID, L"", CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, MkIntRect(5, currentY, 590, 400));
		if (m_TargetComboBox != NULL)
		{
			::SendMessage(m_TargetComboBox, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"[ ���ε� ��� ���� ]"); // default
			::SendMessage(m_TargetComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
		}

		currentY += 30;
		m_UploadStartHandle = _CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_UPLOAD_START_ID, L"��ġ ���ε�", buttonControlStyle, MkIntRect(5, currentY, 590, 40));

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

		if (m_TargetComboBox != NULL)
		{
			const MkArray<MkStr>& destName = m_PG.GetUpdatingDestNames();
			MK_INDEXING_LOOP(destName, i)
			{
				::SendMessage(m_TargetComboBox, CB_ADDSTRING, (WPARAM)0, (LPARAM)destName[i].GetPtr());
			}
		}
	}

	virtual void Update(void)
	{
		if (m_OnPacking == _eStartPatching)
		{
			if (m_PG.GeneratePatchFiles(m_ResRootPath, m_TryCompress))
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

				m_OnPacking = _eUpdatePatching;
			}
			else
			{
				m_OnPacking = _eReady;
				EnableWindow(m_PackingStartHandle, TRUE);
				EnableWindow(m_UploadStartHandle, TRUE);
			}
		}
		else if (m_OnPacking == _eUpdatePatching)
		{
			if (!m_PG.Update())
			{
				m_OnPacking = _eReady;
				EnableWindow(m_PackingStartHandle, TRUE);
				EnableWindow(m_UploadStartHandle, TRUE);
			}
		}
		else if (m_OnPacking == _eStartUploading)
		{
			if (m_PU.StartUploading(m_ExportInfo[0], m_ExportInfo[1], m_ExportInfo[2], m_ExportInfo[3], m_ExportInfo[4], m_ExportInfo[5].ToBool()))
			{
				m_OnPacking = _eUpdateUploading;
			}
			else
			{
				::SendMessage(m_TargetComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
				gTargetExport.Clear();

				m_OnPacking = _eReady;
				EnableWindow(m_PackingStartHandle, TRUE);
				EnableWindow(m_UploadStartHandle, TRUE);
			}
		}
		else if (m_OnPacking == _eUpdateUploading)
		{
			if (!m_PU.Update())
			{
				::SendMessage(m_TargetComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
				gTargetExport.Clear();

				m_OnPacking = _eReady;
				EnableWindow(m_PackingStartHandle, TRUE);
				EnableWindow(m_UploadStartHandle, TRUE);
			}
		}

		if (m_OnPacking == _eReady)
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

				case MKDEF_APP_BTN_PACK_START_ID: // ��ŷ ���� ��ư �̺�Ʈ
					{
						if ((m_ResourceDirHandle != NULL) && (m_PackingStartHandle != NULL))
						{
							m_ResRootPath.Clear();
							if (_GetPathFromEditBox(m_ResourceDirHandle, m_ResRootPath) && (!m_ResRootPath.Empty()))
							{
								m_OnPacking = _eStartPatching;
								EnableWindow(m_PackingStartHandle, FALSE);
								EnableWindow(m_UploadStartHandle, FALSE);
							}
						}
					}
					break;

				case MKDEF_APP_BTN_UPLOAD_START_ID: // ���ε� ���� ��ư �̺�Ʈ
					{
						if ((m_UploadStartHandle != NULL) && (!gTargetExport.Empty()) && m_SettingNode.ChildExist(KEY_ExportNodeName))
						{
							const MkDataNode& exportNode = *m_SettingNode.GetChildNode(KEY_ExportNodeName);
							if (exportNode.ChildExist(gTargetExport))
							{
								const MkDataNode& currDestNode = *exportNode.GetChildNode(gTargetExport);

								m_ExportInfo.Clear();
								m_ExportInfo.Reserve(6);

								EnableWindow(m_PackingStartHandle, FALSE);
								EnableWindow(m_UploadStartHandle, FALSE);

								// warning ����. �ɼ�
								bool proceed = true;
								bool warning = false;
								if (currDestNode.GetData(KEY_Warning, warning, 0) && warning)
								{
									MkStr msg;
									msg.Reserve(256);
									msg += L"[ ";
									msg += gTargetExport.GetString();
									msg += L" ]";
									msg += MkStr::LF;
									msg += MkStr::LF;
									msg += L"��ġ ������ ��¥��¥�������� ���ε� �Ͻðڽ��ϱ�?";

									if (::MessageBox(NULL, msg.GetPtr(), L"Warning!!!", MB_YESNO) == IDNO)
									{
										proceed = false;
									}
								}

								if (proceed)
								{
									bool ok = false;
									do
									{
										// ��ġ ���� ��Ʈ ���. �ݵ�� ����
										const MkArray<MkStr>& destName = m_PG.GetUpdatingDestNames();
										unsigned int index = destName.FindFirstInclusion(MkArraySection(0), gTargetExport.GetString());
										if (index == MKDEF_ARRAY_ERROR)
											break;

										const MkPathName& currDestPath = m_PG.GetUpdatingDestPaths()[index];
										m_ExportInfo.PushBack(currDestPath);

										// url. �ʼ�
										MkStr url;
										if ((!currDestNode.GetData(KEY_DestURL, url, 0)) || url.Empty())
											break;
										m_ExportInfo.PushBack(url);

										// remote path. �ɼ�
										MkStr remotePath;
										currDestNode.GetData(KEY_DestRemotePath, remotePath, 0);
										m_ExportInfo.PushBack(remotePath);

										// user name. �ʼ�
										MkStr userName;
										if ((!currDestNode.GetData(KEY_DestUserName, userName, 0)) || userName.Empty())
											break;
										m_ExportInfo.PushBack(userName);

										// password. �ʼ�
										MkStr password;
										if ((!currDestNode.GetData(KEY_DestPassword, password, 0)) || password.Empty())
											break;
										m_ExportInfo.PushBack(password);

										// passive mode?. �ɼ�
										bool passiveMode = true;
										currDestNode.GetData(KEY_PassiveMode, passiveMode, 0);
										m_ExportInfo.PushBack(MkStr(passiveMode));

										ok = true;

									}
									while (false);

									if (ok)
									{
										m_OnPacking = _eStartUploading;
										
										MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
										MK_DEV_PANEL.MsgToLog(L"> [" + gTargetExport.GetString() + L"] ��� ��� ���ε��� �����մϴ�.");
										MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
										MK_DEV_PANEL.InsertEmptyLine();
									}
									else
									{
										m_ExportInfo.Clear();
										EnableWindow(m_PackingStartHandle, TRUE);
										EnableWindow(m_UploadStartHandle, TRUE);

										MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
										MK_DEV_PANEL.MsgToLog(L"> [" + gTargetExport.GetString() + L"] ��� ����� ������ ��� �ֽ��ϴ�.");
										MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
										MK_DEV_PANEL.InsertEmptyLine();
									}
								}
								else
								{
									EnableWindow(m_PackingStartHandle, TRUE);
									EnableWindow(m_UploadStartHandle, TRUE);
								}
							}
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
					case MKDEF_APP_BTN_UPLOAD_START_ID:
						gFrameworkEvent.PushBack(target);
						break;
					}
				}
				break;

			case CBN_SELCHANGE:
				{
					switch (LOWORD(wParam))
					{
					case MKDEF_APP_CBOX_UPLOAD_TARGET_ID:
						{
							HWND handle = (HWND)lParam;
							DWORD index = ::SendMessage(handle, CB_GETCURSEL, 0, 0);
							if (index > 0)
							{
								wchar_t buffer[512] = {0, };
								::SendMessage(handle, CB_GETLBTEXT, index, (LPARAM)buffer);
								gTargetExport = buffer;
							}
							else
							{
								gTargetExport.Clear();
							}
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
		m_TryCompress = true;
		m_ResourceDirHandle = NULL;
		m_PackingStartHandle = NULL;
		m_TargetComboBox = NULL;
		m_UploadStartHandle = NULL;
		m_OnPacking = _eReady;
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

	enum _eState
	{
		_eReady = 0,

		_eStartPatching,
		_eUpdatePatching,

		_eStartUploading,
		_eUpdateUploading
	};

protected:

	HFONT m_Font;

	MkDataNode m_SettingNode;
	MkPathName m_LauncherFileName;
	MkPathName m_RunFilePath;
	MkPathName m_ResRootPath;

	bool m_TryCompress;

	MkArray<MkStr> m_OutDirNames;
	
	HWND m_ResourceDirHandle;
	HWND m_PackingStartHandle;

	HWND m_TargetComboBox;
	HWND m_UploadStartHandle;

	_eState m_OnPacking;
	MkArray<MkStr> m_ExportInfo;
	
	MkPatchFileGenerator m_PG;
	MkPatchFileUploader m_PU;
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
	application.Run(hI, L"SimplePatchFileGenerator", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 600, 192, false, true, TestFramework::NewWndProc,
		L"#DMK=_MkPatchFileGenerator; #BME=_MkPatchFileGenerator"); // �ߺ� ���� ����

	return 0;
}

//------------------------------------------------------------------------------------------------//

