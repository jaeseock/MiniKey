
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

#define MKDEF_APP_EDIT_CHUNK_SIZE_TAG_ID 7
#define MKDEF_APP_EDIT_CHUNK_SIZE_VALUE_ID 8

#define MKDEF_APP_EDIT_COMP_ENABLE_TAG_ID 9
#define MKDEF_APP_EDIT_COMP_ENABLE_VALUE_ID 10

#define MKDEF_APP_EDIT_CHUNK_PREFIX_TAG_ID 11
#define MKDEF_APP_EDIT_CHUNK_PREFIX_VALUE_ID 12

#define MKDEF_APP_EDIT_CHUNK_EXTENSION_TAG_ID 13
#define MKDEF_APP_EDIT_CHUNK_EXTENSION_VALUE_ID 14

#define MKDEF_APP_BTN_FILTER_FILE_ID 15

#define MKDEF_APP_BTN_PACK_START_ID 16

#define MKDEF_APP_FILTER_KEY_NAME L"Name"
#define MKDEF_APP_FILTER_KEY_EXTENSION L"Extension"
#define MKDEF_APP_FILTER_KEY_PREFIX L"Prefix"
#define MKDEF_APP_FILTER_KEY_EXCEPTION L"Exception"


static MkArray<int> gFrameworkEvent(32);


//------------------------------------------------------------------------------------------------//

class TestFramework : public MkBaseFramework
{
public:

	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg)
	{
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
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_RES_DIR_VALUE_ID, MkPathName::GetModuleDirectory().GetPtr(), inputControlStyle, MkIntRect(5, currentY, 545, 20));
		
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FIND_RES_DIR_ID, L"변경", buttonControlStyle, MkIntRect(555, currentY, 40, 20));

		currentY += 30;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_TARGET_DIR_TAG_ID, L" 출력 대상 폴더", staticTextControlStyle, MkIntRect(5, currentY, DEF_WIDTH, 12));

		currentY += 13;
		m_TargetDirHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_TARGET_DIR_VALUE_ID, MkPathName::GetModuleDirectory().GetPtr(), inputControlStyle, MkIntRect(5, currentY, 545, 20));
		
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FIND_TARGET_DIR_ID, L"변경", buttonControlStyle, MkIntRect(555, currentY, 40, 20));

		unsigned int currentX = 5;
		currentY += 30;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_CHUNK_SIZE_TAG_ID, L" 청크 기준 크기(MB)", staticTextControlStyle, MkIntRect(currentX, currentY, DEF_WIDTH, 12));
		currentX += DEF_WIDTH + 5;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_COMP_ENABLE_TAG_ID, L" 압축 경계(%)", staticTextControlStyle, MkIntRect(currentX, currentY, DEF_WIDTH, 12));
		currentX += DEF_WIDTH + 5;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_CHUNK_PREFIX_TAG_ID, L" 청크 파일 prefix", staticTextControlStyle, MkIntRect(currentX, currentY, DEF_WIDTH, 12));
		currentX += DEF_WIDTH + 5;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_CHUNK_EXTENSION_TAG_ID, L" 청크 파일 확장자", staticTextControlStyle, MkIntRect(currentX, currentY, DEF_WIDTH, 12));

		currentX = 5;
		currentY += 13;
		m_ChunkSizeHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_CHUNK_SIZE_VALUE_ID, L"512", inputControlStyle, MkIntRect(currentX, currentY, DEF_WIDTH, 20));
		currentX += DEF_WIDTH + 5;
		m_CompressingHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_COMP_ENABLE_VALUE_ID, L"70", inputControlStyle, MkIntRect(currentX, currentY, DEF_WIDTH, 20));
		currentX += DEF_WIDTH + 5;
		m_ChunkPrefixHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_CHUNK_PREFIX_VALUE_ID, L"MK_PACK_", inputControlStyle, MkIntRect(currentX, currentY, DEF_WIDTH, 20));
		currentX += DEF_WIDTH + 5;
		m_ChunkExtensionHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_CHUNK_EXTENSION_VALUE_ID, L"mcf", inputControlStyle, MkIntRect(currentX, currentY, DEF_WIDTH, 20));

		currentX += DEF_WIDTH + 5;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FILTER_FILE_ID, L"필터 파일 지정", buttonControlStyle, MkIntRect(currentX, currentY, 110, 20));

		currentY += 30;
		m_PackingStartHandle = _CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_PACK_START_ID, L"패킹 시작", buttonControlStyle, MkIntRect(5, currentY, 590, 40));
		
		return true;
	}

	virtual void StartLooping(void)
	{
		MK_DEV_PANEL.ClearLogWindow();

		MK_DEV_PANEL.MsgToFreeboard(1, L"청크 기준 크기 : ");
		MK_DEV_PANEL.MsgToFreeboard(2, L"   - 하나의 청크 파일 최소 크기. 기준점이므로 정확하지는 않음");

		MK_DEV_PANEL.MsgToFreeboard(4, L"압축 경계 (0 ~ 100) : ");
		MK_DEV_PANEL.MsgToFreeboard(5, L"   - 0이면 전혀 압축을 하지 않고 100 이상이면 대부분 압축");
	}

	virtual void Update(void)
	{
		if (m_OnPacking)
		{
			MkFileSystem pack;
			pack.SetChunkSizeGuideline(m_ChunkSize);
			pack.SetPercentageForCompressing(m_Comp);
			pack.SetChunkFileNamingRule(m_PrefixStr, m_ExtStr);

			if (pack.SetUpFromOriginalDirectory(m_ResPath, m_NameFilter, m_ExtensionFilter, m_PrefixFilter, m_ExceptionFilter, m_TargetPath))
			{
				MK_DEV_PANEL.MsgToLog(L"//---------------------------------------------------------------//");
				MK_DEV_PANEL.MsgToLog(L"   청크 파일 수 : " + MkStr(pack.GetTotalChunkCount()));
				MK_DEV_PANEL.MsgToLog(L"   모든 파일 수 : " + MkStr(pack.GetTotalFileCount()));
				MK_DEV_PANEL.MsgToLog(L"//---------------------------------------------------------------//");

				MessageBox(m_MainWindow.GetWindowHandle(), L"패킹 성공", L"Success!", MB_OK);
			}
			else
			{
				MessageBox(m_MainWindow.GetWindowHandle(), L"패킹 실패", L"Error!", MB_OK);
			}

			pack.Clear();

			m_OnPacking = false;
			EnableWindow(m_PackingStartHandle, TRUE);
		}

		if (!m_OnPacking)
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

				case MKDEF_APP_BTN_FIND_TARGET_DIR_ID: // 타겟 폴더 선택 이벤트. 다이얼로그를 열어 선택
					{
						if (m_TargetDirHandle != NULL)
						{
							MkPathName defPath;
							if (!_GetPathFromEditBox(m_TargetDirHandle, defPath))
							{
								defPath = MkPathName::GetModuleDirectory();
							}

							// 폴더 선택
							MkPathName targetPath;
							if (targetPath.GetDirectoryPathFromDialog(L"출력 대상 폴더를 선택", m_MainWindow.GetWindowHandle(), defPath))
							{
								SetWindowText(m_TargetDirHandle, targetPath.GetPtr());
							}
						}
					}
					break;

				case MKDEF_APP_BTN_FILTER_FILE_ID: // 필터 파일 버튼 이벤트
					{
						MkPathName filePath;
						if (filePath.GetSingleFilePathFromDialog(m_MainWindow.GetWindowHandle()))
						{
							MkDataNode node;
							if (node.Load(filePath))
							{
								MkArray<MkStr> nameFilter, extensionFilter, exceptionFilter;

								m_NameFilter.Clear();
								m_ExtensionFilter.Clear();
								m_PrefixFilter.Clear();
								m_ExceptionFilter.Clear();

								if (node.GetData(MKDEF_APP_FILTER_KEY_NAME, nameFilter))
								{
									m_NameFilter.Reserve(nameFilter.GetSize());
									MK_INDEXING_LOOP(nameFilter, i)
									{
										m_NameFilter.PushBack(nameFilter[i]);

										if (i == 0)
										{
											MK_DEV_PANEL.MsgToLog(L"Filter : Name");
										}
										MK_DEV_PANEL.MsgToLog(L"   - " + nameFilter[i]);
									}
								}

								if (node.GetData(MKDEF_APP_FILTER_KEY_EXTENSION, extensionFilter))
								{
									m_ExtensionFilter.Reserve(extensionFilter.GetSize());
									MK_INDEXING_LOOP(extensionFilter, i)
									{
										m_ExtensionFilter.PushBack(extensionFilter[i]);

										if (i == 0)
										{
											MK_DEV_PANEL.MsgToLog(L"Filter : Extension");
										}
										MK_DEV_PANEL.MsgToLog(L"   - " + extensionFilter[i]);
									}
								}

								if (node.GetData(MKDEF_APP_FILTER_KEY_PREFIX, m_PrefixFilter))
								{
									MK_INDEXING_LOOP(m_PrefixFilter, i)
									{
										if (i == 0)
										{
											MK_DEV_PANEL.MsgToLog(L"Filter : Prefix");
										}
										MK_DEV_PANEL.MsgToLog(L"   - " + m_PrefixFilter[i]);
									}
								}

								if (node.GetData(MKDEF_APP_FILTER_KEY_EXCEPTION, exceptionFilter))
								{
									m_ExceptionFilter.Reserve(exceptionFilter.GetSize());
									MK_INDEXING_LOOP(exceptionFilter, i)
									{
										m_ExceptionFilter.PushBack(exceptionFilter[i]);

										if (i == 0)
										{
											MK_DEV_PANEL.MsgToLog(L"Filter : Exception");
										}
										MK_DEV_PANEL.MsgToLog(L"   - " + exceptionFilter[i]);
									}
								}
							}
						}
					}
					break;

				case MKDEF_APP_BTN_PACK_START_ID: // 패킹 시작 버튼 이벤트
					{
						if ((m_ResourceDirHandle != NULL) && (m_TargetDirHandle != NULL) &&
							(m_ChunkSizeHandle != NULL) && (m_CompressingHandle != NULL) && (m_ChunkPrefixHandle != NULL) && (m_ChunkExtensionHandle != NULL) && (m_PackingStartHandle != NULL))
						{
							if (_GetPathFromEditBox(m_ResourceDirHandle, m_ResPath) && _GetPathFromEditBox(m_TargetDirHandle, m_TargetPath) &&
								_GetUIntFromEditBox(m_ChunkSizeHandle, m_ChunkSize) && _GetUIntFromEditBox(m_CompressingHandle, m_Comp) &&
								_GetStringFromEditBox(m_ChunkPrefixHandle, m_PrefixStr) && _GetStringFromEditBox(m_ChunkExtensionHandle, m_ExtStr))
							{
								MK_DEV_PANEL.MsgToLog(L"//---------------------------------------------------------------//");
								MK_DEV_PANEL.MsgToLog(L"- 리소스 경로 : " + m_ResPath);
								MK_DEV_PANEL.MsgToLog(L"- 저장 경로 : " + m_TargetPath);
								MK_DEV_PANEL.MsgToLog(L"- 기준 청크 크기 : " + MkStr(m_ChunkSize) + L"MB");
								MK_DEV_PANEL.MsgToLog(L"- 압축 경계 : " + MkStr(m_Comp) + L"%");
								MK_DEV_PANEL.MsgToLog(L"- 청크 파일명 규칙 : " + m_PrefixStr + L"(n)." + m_ExtStr);
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
					case MKDEF_APP_BTN_FILTER_FILE_ID:
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
		m_ChunkSizeHandle = NULL;
		m_CompressingHandle = NULL;
		m_ChunkPrefixHandle = NULL;
		m_ChunkExtensionHandle = NULL;
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

	bool _GetStringFromEditBox(HWND editBoxHandle, MkStr& buffer)
	{
		wchar_t strBuf[256] = {0, };
		GetWindowText(editBoxHandle, strBuf, 256);
		buffer = strBuf;

		return (!buffer.Empty());
	}

	bool _GetUIntFromEditBox(HWND editBoxHandle, unsigned int& buffer)
	{
		MkStr buf;
		if (_GetStringFromEditBox(editBoxHandle, buf))
		{
			buf.RemoveBlank();
			if (buf.IsDigit())
			{
				buffer = buf.ToUnsignedInteger();
				return true;
			}
		}
		return false;
	}

protected:

	HFONT m_Font;

	HWND m_ResourceDirHandle;
	HWND m_TargetDirHandle;

	HWND m_ChunkSizeHandle;
	HWND m_CompressingHandle;
	HWND m_ChunkPrefixHandle;
	HWND m_ChunkExtensionHandle;

	HWND m_PackingStartHandle;

	MkArray<MkPathName> m_NameFilter;
	MkArray<MkPathName> m_ExtensionFilter;
	MkArray<MkStr> m_PrefixFilter;
	MkArray<MkPathName> m_ExceptionFilter;

	bool m_OnPacking;
	unsigned int m_ChunkSize;
	unsigned int m_Comp;
	MkStr m_PrefixStr;
	MkStr m_ExtStr;
	MkPathName m_ResPath;
	MkPathName m_TargetPath;
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

// 엔트리 포인트에서의 TestApplication 실행
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"MkFilePacker", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 600, 180, false, false, TestFramework::NewWndProc, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

