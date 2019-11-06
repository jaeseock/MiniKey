
//------------------------------------------------------------------------------------------------//

#include <windows.h>
#include <CommCtrl.h> // InitCommonControlsEx

#include "minizip/zip.h"
#include "minizip/iowin32.h"

#include "MkCore_MkDevPanel.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkInterfaceForFileReading.h"
#include "MkCore_MkInterfaceForFileWriting.h"
#include "MkCore_MkInterfaceForZipFileReading.h"

#pragma comment (lib, "Comctl32.lib")

#define MKDEF_APP_LB_FILE_ID 1
#define MKDEF_APP_BTN_EXTRACT_TARGET_ID 2
#define MKDEF_APP_BTN_EXTRACT_ALL_ID 3
#define MKDEF_APP_BTN_PACK_FOLDER_ID 4

static MkPathName gPackFilePath;
static MkStr gTargetFileName;

static int gCommand = 0;


// single thread
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		INITCOMMONCONTROLSEX icex;
		icex.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&icex);

		m_Font = CreateFont(12, 0, 0, 0, 0, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH | FF_ROMAN, L"굴림");

		m_FileListBox = _CreateControl(WC_LISTBOX, MKDEF_APP_LB_FILE_ID, L"", LBS_STANDARD | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, MkIntRect(10, 10, 470, 758));

		m_ExtractTargetBtn = _CreateControl(L"button", MKDEF_APP_BTN_EXTRACT_TARGET_ID, L"대상 파일 추출", WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON, MkIntRect(490, 10, 140, 40));
		m_ExtractAllBtn = _CreateControl(L"button", MKDEF_APP_BTN_EXTRACT_ALL_ID, L"모든 파일 추출", WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON, MkIntRect(490, 60, 140, 40));
		m_PackFolderBtn = _CreateControl(L"button", MKDEF_APP_BTN_PACK_FOLDER_ID, L"지정된 폴더를 패킹", WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON, MkIntRect(490, 110, 140, 40));

		_UpdateAppTitle(MkPathName::EMPTY);
		
		::EnableWindow(m_ExtractTargetBtn, FALSE);
		::EnableWindow(m_ExtractAllBtn, FALSE);

		return true; // EXIT_SUCCESS
	}

	virtual void StartLooping(void)
	{
		
	}

	virtual void ConsumeDraggedFiles(MkArray<MkPathName>& draggedFilePathList)
	{
		// SPAK 드래그시 읽어서 리스트박스에 등록
		if (draggedFilePathList.GetSize() == 1)
		{
			if (m_FileListBox != NULL)
			{
				::SendMessage(m_FileListBox, LB_RESETCONTENT, 0, 0);
			}

			gPackFilePath.Clear();
			gTargetFileName.Clear();

			MkInterfaceForZipFileReading zipInterface;
			if (zipInterface.SetUp(draggedFilePathList[0], false))
			{
				MkArray<MkHashStr> keys;
				if (zipInterface.GetFileList(keys) > 0)
				{
					MK_INDEXING_LOOP(keys, i)
					{
						::SendMessage(m_FileListBox, LB_ADDSTRING, 0,(LPARAM)keys[i].GetPtr());
					}

					_UpdateAppTitle(draggedFilePathList[0]);
					gPackFilePath = draggedFilePathList[0];
				}
			}

			if (m_ExtractAllBtn != NULL)
			{
				::EnableWindow(m_ExtractAllBtn, gPackFilePath.Empty() ? FALSE : TRUE);
			}
		}

		draggedFilePathList.Clear();
	}

	virtual void Update(void)
	{
		if (m_ExtractTargetBtn != NULL)
		{
			EnableWindow(m_ExtractTargetBtn, gTargetFileName.Empty() ? FALSE : TRUE);
		}

		// 대상 파일 추출
		if ((gCommand == MKDEF_APP_BTN_EXTRACT_TARGET_ID) && (!gPackFilePath.Empty()) && (!gTargetFileName.Empty()))
		{
			MkPathName filePath;
			if (filePath.GetDirectoryPathFromDialog(L"대상 파일을 풀 폴더 지정", m_MainWindow.GetWindowHandle(), gPackFilePath.GetPath()))
			{
				MkInterfaceForZipFileReading zipInterface;
				if (zipInterface.SetUp(gPackFilePath, false))
				{
					MkByteArray byteArray;
					if (_ReadFile(zipInterface, gTargetFileName, byteArray))
					{
						filePath += gTargetFileName;

						MkInterfaceForFileWriting fwInterface;
						fwInterface.SetUp(filePath, true, true);
						fwInterface.Write(byteArray, MkArraySection(0));

						::MessageBox(m_MainWindow.GetWindowHandle(), filePath.GetPtr(), L"대상 파일 추출 완료", MB_OK);
					}
				}
			}
		}
		// 모든 파일 추출
		else if ((gCommand == MKDEF_APP_BTN_EXTRACT_ALL_ID) && (!gPackFilePath.Empty()))
		{
			MkPathName dirPath;
			if (dirPath.GetDirectoryPathFromDialog(L"모든 파일을 풀 폴더 지정", m_MainWindow.GetWindowHandle(), gPackFilePath.GetPath()))
			{
				MkInterfaceForZipFileReading zipInterface;
				if (zipInterface.SetUp(gPackFilePath, false))
				{
					MkArray<MkHashStr> keys;
					if (zipInterface.GetFileList(keys) > 0)
					{
						MK_INDEXING_LOOP(keys, i)
						{
							MkByteArray byteArray;
							if (_ReadFile(zipInterface, keys[i], byteArray))
							{
								MkPathName filePath = dirPath;
								filePath += keys[i];

								MkInterfaceForFileWriting fwInterface;
								fwInterface.SetUp(filePath, true, true);
								fwInterface.Write(byteArray, MkArraySection(0));
							}
						}

						::MessageBox(m_MainWindow.GetWindowHandle(), dirPath.GetPtr(), L"모든 파일 추출 완료", MB_OK);
					}
				}
			}
		}
		// 폴더 패킹
		else if (gCommand == MKDEF_APP_BTN_PACK_FOLDER_ID)
		{
			MkPathName filePath;
			if (filePath.GetSaveFilePathFromDialog(L"SPAK", m_MainWindow.GetWindowHandle()))
			{
				MkPathName srcDirPath;
				if (srcDirPath.GetDirectoryPathFromDialog(L"패킹 할 파일들이 든 폴더 지정", m_MainWindow.GetWindowHandle(), filePath.GetPath()))
				{
					MkArray<MkPathName> filePathList;
					srcDirPath.GetFileList(filePathList, false, false);
					if (filePathList.GetSize() > 0)
					{
						if (filePath.CheckAvailable())
						{
							MkStr name = filePath.GetFileName(false);
							name += L"_";
							name += filePath.GetWrittenTime();
							name += L".";
							name += filePath.GetFileExtension();

							MkPathName backupFilePath = filePath;
							backupFilePath.RenameCurrentFile(name);
						}

						zlib_filefunc_def ffunc;
						fill_win32_filefunc(&ffunc);

						// 파일명을 const char* 타입으로 받지만, zipOpen2에서 파일을 생성하는 CreateFile은 CreateFileW 이기에 유니코드를 그냥 캐스팅해서 넣어 주어야 한다
						zipFile packFile = zipOpen2(reinterpret_cast<const char*>(filePath.GetPtr()), APPEND_STATUS_CREATE, NULL, &ffunc);

						MK_INDEXING_LOOP(filePathList, i)
						{
							const MkPathName& currFileName = filePathList[i];
							MkPathName srcFilePath = srcDirPath;
							srcFilePath += currFileName;

							MkByteArray srcData;
							MkInterfaceForFileReading frInterface;
							frInterface.SetUp(srcFilePath);
							frInterface.Read(srcData, MkArraySection(0));
							frInterface.Clear();

							zip_fileinfo zi = {0, };
							WIN32_FIND_DATA ff32;
							HANDLE fSrcFile = ::FindFirstFile(srcFilePath.GetPtr(), &ff32);
							if (fSrcFile != INVALID_HANDLE_VALUE)
							{
								FILETIME ftime;
								SYSTEMTIME stime;
								::FileTimeToLocalFileTime(&(ff32.ftLastWriteTime), &ftime);
								::FileTimeToSystemTime(&ftime, &stime);

								zi.tmz_date.tm_hour = stime.wHour;
								zi.tmz_date.tm_mday = stime.wDay;
								zi.tmz_date.tm_min = stime.wMinute;
								zi.tmz_date.tm_mon = stime.wMonth;
								zi.tmz_date.tm_sec = stime.wSecond;
								zi.tmz_date.tm_year = stime.wYear;

								::FindClose(fSrcFile);
							}

							std::string bufFileName;
							currFileName.ExportMultiByteString(bufFileName);
							if (zipOpenNewFileInZip3(packFile, bufFileName.c_str(), &zi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, 8, 0,
								-MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, "!@####ca3c6854*@#@", 0) == ZIP_OK)
							{
								zipWriteInFileInZip(packFile, srcData.GetPtr(), srcData.GetSize());
								zipCloseFileInZip(packFile);
							}
						}
						
						if (packFile != NULL)
						{
							zipClose(packFile, "Seal Online Zip v3");

							MkStr msg = srcDirPath;
							msg += MkStr::CRLF;
							msg += L"->";
							msg += MkStr::CRLF;
							msg += filePath;
							::MessageBox(m_MainWindow.GetWindowHandle(), msg.GetPtr(), L"패킹 완료", MB_OK);
						}
					}
				}
			}
		}

		gCommand = 0;
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
					switch (LOWORD(wParam))
					{
					case MKDEF_APP_BTN_EXTRACT_TARGET_ID:
						{
							if (gCommand == 0)
							{
								gCommand = MKDEF_APP_BTN_EXTRACT_TARGET_ID;
							}
						}
						break;

					case MKDEF_APP_BTN_EXTRACT_ALL_ID:
						{
							if (gCommand == 0)
							{
								gCommand = MKDEF_APP_BTN_EXTRACT_ALL_ID;
							}
						}
						break;

					case MKDEF_APP_BTN_PACK_FOLDER_ID:
						{
							if (gCommand == 0)
							{
								gCommand = MKDEF_APP_BTN_PACK_FOLDER_ID;
							}
						}
						break;
					}
				}
				break;

			case LBN_SELCHANGE:
				{
					switch (LOWORD(wParam))
					{
					case MKDEF_APP_LB_FILE_ID:
						{
							HWND handle = (HWND)lParam;
							DWORD index = ::SendMessage(handle, LB_GETCURSEL, 0, 0);
							wchar_t tmp[512] = {0, };
							::SendMessage(handle, LB_GETTEXT, index, (LPARAM)tmp);
							gTargetFileName = tmp;
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
		m_FileListBox = NULL;
		m_ExtractTargetBtn = NULL;
		m_ExtractAllBtn = NULL;
		m_PackFolderBtn = NULL;
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

	void _UpdateAppTitle(const MkPathName& targetPath)
	{
		MkPathName filePath;
		MkStr fileName, fileExt;
		targetPath.SplitPath(filePath, fileName, fileExt);

		MkStr msg;
		if (fileName.Empty())
		{
			msg = L"SealSPAK Operator";
		}
		else
		{
			msg.Reserve(512);
			msg += fileName;
			msg += L".";
			msg += fileExt;
			msg += L" (";

			if (filePath.GetSize() <= 128)
			{
				msg += filePath;
			}
			else
			{
				MkStr tmp;
				filePath.GetSubStr(MkArraySection(filePath.GetSize() - 127), tmp);
				msg += L"...";
				msg += tmp;
			}

			msg += L")";
		}

		::SetWindowTextW(m_MainWindow.GetWindowHandle(), msg.GetPtr());
	}

	bool _ReadFile(MkInterfaceForZipFileReading& zipInterface, const MkPathName& filePath, MkByteArray& destBuffer)
	{
		if (zipInterface.Read(filePath, L"!@####ca3c6854*@#@", destBuffer) > 0) // v3
			return true;
		if (zipInterface.Read(filePath, L"#$#$&*edd775ec!!@", destBuffer) > 0) // v2
			return true;
		if (zipInterface.Read(filePath, L"^^&!@FE6FCE45&&*", destBuffer) > 0) // v1
			return true;
		if (zipInterface.Read(filePath, L"#!#0d984d3fd&&!!", destBuffer) > 0) // v0
			return true;

		return false;
	}

protected:

	HFONT m_Font;
	HWND m_FileListBox;
	HWND m_ExtractTargetBtn;
	HWND m_ExtractAllBtn;
	HWND m_PackFolderBtn;
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
	application.Run(hI, L"SPAK Op.", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 640, 766, false, true, TestFramework::NewWndProc);

	return 0;
}

//------------------------------------------------------------------------------------------------//

