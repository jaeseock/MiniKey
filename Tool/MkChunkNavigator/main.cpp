
//------------------------------------------------------------------------------------------------//
// single/multi-thread application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkLogicThreadUnit.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkFileSystem.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkDataNode.h"

#define MKDEF_APP_GB_INIT_TAG_ID 1
#define MKDEF_APP_EDIT_CHUNK_DIR_VALUE_ID 2
#define MKDEF_APP_BTN_FIND_CHUNK_DIR_ID 3
#define MKDEF_APP_EDIT_CHUNK_PREFIX_ID 4
#define MKDEF_APP_EDIT_SETTING_FILE_ID 5
#define MKDEF_APP_BTN_LOAD_SYSTEM_DIR_ID 6

#define MKDEF_APP_BG_DIRECTORY_ID 10
#define MKDEF_APP_EDIT_CURRENT_DIR_ID 11
#define MKDEF_APP_BTN_SHOW_DIR_LIST_ID 12
#define MKDEF_APP_BTN_MOVE_TO_PARENT_ID 13
#define MKDEF_APP_BTN_REMOVE_TARGET_DIR_ID 14
#define MKDEF_APP_BTN_UNPACK_TARGET_DIR_ID 15
#define MKDEF_APP_EDIT_TARGET_DIR_ID 16
#define MKDEF_APP_BTN_MOVE_TO_TARGET_DIR_ID 17

#define MKDEF_APP_BG_FILE_ID 20
#define MKDEF_APP_BTN_SHOW_FILE_LIST_ID 21
#define MKDEF_APP_EDIT_TARGET_FILE_ID 22
#define MKDEF_APP_BTN_INFO_TARGET_FILE_ID 23
#define MKDEF_APP_BTN_REMOVE_TARGET_FILE_ID 24
#define MKDEF_APP_BTN_UNPACK_TARGET_FILE_ID 25

#define MKDEF_APP_GB_UPDATE_TAG_ID 30
#define MKDEF_APP_EDIT_UPDATE_DIR_VALUE_ID 31
#define MKDEF_APP_BTN_FIND_UPDATE_DIR_ID 32
#define MKDEF_APP_BTN_UPDATE_DIR_ID 33

#define MKDEF_APP_GB_CHUNK_TAG_ID 40
#define MKDEF_APP_BTN_SHOW_CHUNK_INFO_ID 41
#define MKDEF_APP_BTN_OPT_CHUNK_ID 42

#define MKDEF_APP_BTN_PRINT_STRUCTURE_ID 50


static MkArray<int> gFrameworkEvent(32);


//------------------------------------------------------------------------------------------------//

class TestFramework : public MkBaseFramework
{
public:

	enum eFrameworkState
	{
		eFS_Idle = 0,

		eFS_Loading,
		eFS_Unpacking,
		eFS_Updating,
		eFS_Optimizing
	};

	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		// 콘트롤 생성 준비
		m_Font = CreateFont
			(12, 0, 0, 0, 0, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH | FF_ROMAN, L"굴림");

		HWND hWnd = m_MainWindow.GetWindowHandle();
		HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE));

		const DWORD groupBoxControlStyle = WS_CHILD | WS_VISIBLE | BS_GROUPBOX;
		const DWORD staticTextControlStyle = WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY;
		const DWORD inputControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT;
		const DWORD buttonControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON;

		//------------------------------------------------------------------------------------------------//
		// 초기화 콘트롤
		//------------------------------------------------------------------------------------------------//

		int currentY = 10;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_GB_INIT_TAG_ID, L"초기화", groupBoxControlStyle, MkIntRect(5, currentY, 590, 70));

		currentY += 20;
		m_ChunkDirHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_CHUNK_DIR_VALUE_ID, MkPathName::GetModuleDirectory().GetPtr(), inputControlStyle, MkIntRect(10, currentY, 540, 20));

		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FIND_CHUNK_DIR_ID, L"변경", buttonControlStyle, MkIntRect(555, currentY, 35, 20));

		currentY += 25;
		_CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_CHUNK_PREFIX_ID, L"설정 파일명", staticTextControlStyle, MkIntRect(10, currentY + 3, 70, 14));

		m_SettingFileHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_SETTING_FILE_ID, L"PackInfo.mmd", inputControlStyle, MkIntRect(85, currentY, 130, 20));

		m_LoadingBtnHandle = _CreateControl
			(hWnd, hInstance, L"button", MKDEF_APP_BTN_LOAD_SYSTEM_DIR_ID, L"파일 시스템 로드", buttonControlStyle, MkIntRect(220, currentY, 370, 20));

		//------------------------------------------------------------------------------------------------//
		// 디렉토리 콘트롤
		//------------------------------------------------------------------------------------------------//

		currentY += 45;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BG_DIRECTORY_ID, L"디렉토리", groupBoxControlStyle, MkIntRect(5, currentY, 590, 95));

		currentY += 20;
		m_CurrDirHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_CURRENT_DIR_ID, L"", staticTextControlStyle, MkIntRect(10, currentY, 580, 20));

		currentY += 25;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_SHOW_DIR_LIST_ID, L"하위 디렉토리 리스트 출력", buttonControlStyle, MkIntRect(10, currentY, 265, 20));
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_MOVE_TO_PARENT_ID, L"상위로 이동", buttonControlStyle, MkIntRect(280, currentY, 100, 20));
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_REMOVE_TARGET_DIR_ID, L"디렉토리 삭제", buttonControlStyle, MkIntRect(385, currentY, 100, 20));
		m_UnpackDirHandle = _CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_UNPACK_TARGET_DIR_ID, L"디렉토리 풀기", buttonControlStyle, MkIntRect(490, currentY, 100, 20));

		currentY += 25;
		m_TargetDirHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_TARGET_DIR_ID, L"", inputControlStyle, MkIntRect(10, currentY, 475, 20));

		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_MOVE_TO_TARGET_DIR_ID, L"타겟으로 이동", buttonControlStyle, MkIntRect(490, currentY, 100, 20));

		//------------------------------------------------------------------------------------------------//
		// 파일 콘트롤
		//------------------------------------------------------------------------------------------------//

		currentY += 45;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BG_FILE_ID, L"파일", groupBoxControlStyle, MkIntRect(5, currentY, 590, 70));

		currentY += 20;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_SHOW_FILE_LIST_ID, L"하위 파일 리스트 출력", buttonControlStyle, MkIntRect(10, currentY, 265, 20));

		currentY += 25;
		m_TargetFileHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_TARGET_FILE_ID, L"", inputControlStyle, MkIntRect(10, currentY, 265, 20));

		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_INFO_TARGET_FILE_ID, L"파일 정보", buttonControlStyle, MkIntRect(280, currentY, 100, 20));
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_REMOVE_TARGET_FILE_ID, L"파일 삭제", buttonControlStyle, MkIntRect(385, currentY, 100, 20));
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_UNPACK_TARGET_FILE_ID, L"파일 풀기", buttonControlStyle, MkIntRect(490, currentY, 100, 20));

		//------------------------------------------------------------------------------------------------//
		// 갱신 콘트롤
		//------------------------------------------------------------------------------------------------//

		currentY += 45;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_GB_UPDATE_TAG_ID, L"갱신", groupBoxControlStyle, MkIntRect(5, currentY, 590, 70));

		currentY += 20;
		m_UpdateSrcDirHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_UPDATE_DIR_VALUE_ID, MkPathName::GetModuleDirectory().GetPtr(), inputControlStyle, MkIntRect(10, currentY, 540, 20));

		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FIND_UPDATE_DIR_ID, L"변경", buttonControlStyle, MkIntRect(555, currentY, 35, 20));

		currentY += 25;
		m_DoUpdateHandle = _CreateControl
			(hWnd, hInstance, L"button", MKDEF_APP_BTN_UPDATE_DIR_ID, L"디렉토리로 갱신", buttonControlStyle, MkIntRect(10, currentY, 580, 20));

		//------------------------------------------------------------------------------------------------//
		// 청크 콘트롤
		//------------------------------------------------------------------------------------------------//

		currentY += 45;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_GB_CHUNK_TAG_ID, L"청크", groupBoxControlStyle, MkIntRect(5, currentY, 590, 45));

		currentY += 20;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_SHOW_CHUNK_INFO_ID, L"공극률(빈 공간 비율) 출력", buttonControlStyle, MkIntRect(10, currentY, 285, 20));

		m_OptimizeHandle = _CreateControl
			(hWnd, hInstance, L"button", MKDEF_APP_BTN_OPT_CHUNK_ID, L"최적화", buttonControlStyle, MkIntRect(305, currentY, 285, 20));

		//------------------------------------------------------------------------------------------------//
		// 노드 출력
		//------------------------------------------------------------------------------------------------//

		currentY += 45;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_PRINT_STRUCTURE_ID, L"파일 구조를 노드로 출력", buttonControlStyle, MkIntRect(5, currentY, 590, 20));
		
		return true;
	}

	virtual void StartLooping(void)
	{
		MK_DEV_PANEL.ClearLogWindow();
	}

	virtual void Update(void)
	{
		if (m_State == eFS_Loading)
		{
			m_SystemStructure.Clear();
			m_CurrentNode = NULL;

			// setting file이 있으면 초기화
			if (!m_SettingFilePath.Empty())
			{
				m_FileSystem.SetSystemSetting(m_SettingFilePath);
			}

			if (m_FileSystem.SetUpFromChunkFiles(m_ChunkFilePath))
			{
				m_FileSystem.PrintSystemInfoToDevPanel(true);

				m_FileSystem.ExportSystemStructure(m_SystemStructure, true);
				m_CurrentNode = &m_SystemStructure;
				_UpdateCurrentDirectory(m_CurrentNode);

				MessageBox(m_MainWindow.GetWindowHandle(), L"구성 성공", L"Success!", MB_OK);
			}
			else
			{
				MessageBox(m_MainWindow.GetWindowHandle(), L"구성 실패", L"Error!", MB_OK);
			}

			EnableWindow(m_LoadingBtnHandle, TRUE);
		}

		if (m_State == eFS_Unpacking)
		{
			_UnpackDirectory(m_CurrentNode);

			MkPathName relativePath;
			_GetCurrentDirectoryPath(m_CurrentNode, relativePath);
			MkPathName fullPath = m_UnpackingFilePath + relativePath;
			fullPath.OpenDirectoryInExplorer();

			EnableWindow(m_UnpackDirHandle, TRUE);

			MK_DEV_PANEL.MsgToLog(L"< 언패킹 완료 >");
		}

		if (m_State == eFS_Updating)
		{
			m_SystemStructure.Clear();
			m_CurrentNode = NULL;

			if (m_FileSystem.UpdateFromOriginalDirectory(m_UpdateSrcPath))
			{
				m_FileSystem.ExportSystemStructure(m_SystemStructure, true);

				m_CurrentNode = &m_SystemStructure;
				_UpdateCurrentDirectory(m_CurrentNode);

				MK_DEV_PANEL.MsgToLog(L"< 갱신 완료 >");
			}
			else
			{
				MK_DEV_PANEL.MsgToLog(L"< 갱신 실패. 파일 시스템 재 로딩 필요 >");
			}

			EnableWindow(m_DoUpdateHandle, TRUE);
		}

		if (m_State == eFS_Optimizing)
		{
			m_FileSystem.OptimizeAllChunks(0);

			EnableWindow(m_OptimizeHandle, TRUE);

			MK_DEV_PANEL.MsgToLog(L"< 청크 최적화 완료 >");
		}

		if (m_State != eFS_Idle)
		{
			m_State = eFS_Idle;
		}
		else
		{
			MK_INDEXING_LOOP(gFrameworkEvent, i)
			{
				switch (gFrameworkEvent[i])
				{
				case MKDEF_APP_BTN_FIND_CHUNK_DIR_ID:
					{
						if (m_ChunkDirHandle != NULL)
						{
							MkPathName defPath;
							if (!_GetPathFromEditBox(m_ChunkDirHandle, defPath))
							{
								defPath = MkPathName::GetModuleDirectory();
							}

							MkPathName targetPath;
							if (targetPath.GetDirectoryPathFromDialog(L"청크 파일들이 존재하는 폴더 선택", m_MainWindow.GetWindowHandle(), defPath))
							{
								SetWindowText(m_ChunkDirHandle, targetPath.GetPtr());
							}
						}
					}
					break;

				case MKDEF_APP_BTN_LOAD_SYSTEM_DIR_ID:
					{
						if ((m_State == eFS_Idle) && (m_ChunkDirHandle != NULL) && (m_SettingFileHandle != NULL))
						{
							MkPathName chunkPath;
							MkStr prefix, settingFile;
							if (_GetPathFromEditBox(m_ChunkDirHandle, chunkPath) && _GetStringFromEditBox(m_SettingFileHandle, settingFile))
							{
								EnableWindow(m_LoadingBtnHandle, FALSE);
								
								m_ChunkFilePath = chunkPath;
								m_ChunkFilePath.CheckAndAddBackslash();

								if (!settingFile.Empty())
								{
									m_SettingFilePath = m_ChunkFilePath;
									m_SettingFilePath += settingFile;
									if (!m_SettingFilePath.CheckAvailable())
									{
										m_SettingFilePath.Clear();
									}
								}

								m_State = eFS_Loading;

								MK_DEV_PANEL.MsgToLog(m_SettingFilePath.Empty() ? L"설정 파일 로딩 실패" : (settingFile + L" 로딩 성공"));
								MK_DEV_PANEL.MsgToLog(L"< 파일 시스템 구성 중... >");
							}
						}
					}
					break;

				case MKDEF_APP_BTN_SHOW_DIR_LIST_ID:
					{
						if (m_CurrentNode != NULL)
						{
							MkArray<MkHashStr> listBuffer;
							MkPathName::__GetSubDirectories(*m_CurrentNode, listBuffer);
							
							if (listBuffer.Empty())
							{
								MK_DEV_PANEL.MsgToLog(L"< 하위 디렉토리가 존재하지 않음 >");
							}
							else
							{
								MK_INDEXING_LOOP(listBuffer, i)
								{
									MK_DEV_PANEL.MsgToLog(listBuffer[i].GetString());
								}
								MK_DEV_PANEL.MsgToLog(L"< " + MkStr(listBuffer.GetSize()) + L"개의 하위 디렉토리 >");
							}
						}
					}
					break;

				case MKDEF_APP_BTN_MOVE_TO_PARENT_ID:
					{
						if (m_CurrentNode != NULL)
						{
							MkDataNode* parent = m_CurrentNode->GetParentNode();
							if (parent == NULL)
							{
								MK_DEV_PANEL.MsgToLog(L"< 루트 디렉토리에서는 상위 탐색 불가 >");
							}
							else
							{
								m_CurrentNode = parent;
								_UpdateCurrentDirectory(m_CurrentNode);
							}
						}
					}
					break;

				case MKDEF_APP_BTN_REMOVE_TARGET_DIR_ID:
					{
						if (m_CurrentNode != NULL)
						{
							_RemoveDirectory(m_CurrentNode);
							
							MkDataNode* parent = m_CurrentNode->GetParentNode();
							if (parent == NULL)
							{
								m_CurrentNode->Clear();

								MK_DEV_PANEL.MsgToLog(L"< 루트 디렉토리 이하 모두 삭제 완료 >");
							}
							else
							{
								parent->DetachChildNode(m_CurrentNode->GetNodeName());
								delete m_CurrentNode;
								m_CurrentNode = parent;
								_UpdateCurrentDirectory(m_CurrentNode);

								MK_DEV_PANEL.MsgToLog(L"< 디렉토리 삭제 완료 후 상위로 이동 >");
							}
						}
					}
					break;

				case MKDEF_APP_BTN_UNPACK_TARGET_DIR_ID:
					{
						if ((m_State == eFS_Idle) && (m_CurrentNode != NULL))
						{
							EnableWindow(m_UnpackDirHandle, FALSE);

							MkPathName defPath;
							if (!_GetPathFromEditBox(m_ChunkDirHandle, defPath))
							{
								defPath = MkPathName::GetModuleDirectory();
							}

							m_UnpackingFilePath.Clear();
							if (m_UnpackingFilePath.GetDirectoryPathFromDialog(L"풀린 파일들이 위치 할 폴더 선택", m_MainWindow.GetWindowHandle(), defPath))
							{
								m_State = eFS_Unpacking;

								MK_DEV_PANEL.MsgToLog(L"< 언패킹 중... >");
							}
							else
							{
								EnableWindow(m_UnpackDirHandle, TRUE);
							}
						}
					}
					break;

				case MKDEF_APP_BTN_MOVE_TO_TARGET_DIR_ID:
					{
						MkStr dirName;
						if ((m_TargetDirHandle != NULL) && (m_CurrentNode != NULL) && _GetStringFromEditBox(m_TargetDirHandle, dirName))
						{
							MkDataNode* targetNode = m_CurrentNode->GetChildNode(dirName);
							if (targetNode == NULL)
							{
								MK_DEV_PANEL.MsgToLog(L"< 대상 디렉토리는 존재하지 않음 >");
							}
							else
							{
								m_CurrentNode = targetNode;
								_UpdateCurrentDirectory(m_CurrentNode);
								SetWindowText(m_TargetDirHandle, L"");

								MK_DEV_PANEL.MsgToLog(L"< 대상 디렉토리로 이동 완료 >");
							}
						}
					}
					break;

				case MKDEF_APP_BTN_SHOW_FILE_LIST_ID:
					{
						if (m_CurrentNode != NULL)
						{
							MkArray<MkHashStr> listBuffer;
							MkPathName::__GetSubFiles(*m_CurrentNode, listBuffer);
							
							if (listBuffer.Empty())
							{
								MK_DEV_PANEL.MsgToLog(L"< 하위 파일이 존재하지 않음 >");
							}
							else
							{
								MK_INDEXING_LOOP(listBuffer, i)
								{
									MK_DEV_PANEL.MsgToLog(listBuffer[i].GetString());
								}
								MK_DEV_PANEL.MsgToLog(L"< " + MkStr(listBuffer.GetSize()) + L"개의 하위 파일 >");
							}
						}
					}
					break;

				case MKDEF_APP_BTN_INFO_TARGET_FILE_ID:
					{
						MkStr fileName;
						if ((m_TargetFileHandle != NULL) && (m_CurrentNode != NULL) && _GetStringFromEditBox(m_TargetFileHandle, fileName))
						{
							MkPathName relativePath;
							_GetCurrentDirectoryPath(m_CurrentNode, relativePath);

							MkPathName filePath = relativePath + fileName;
							unsigned int originalSize, dataSize, writtenTime;
							if (m_FileSystem.GetFileInfo(filePath, originalSize, dataSize, writtenTime))
							{
								MK_DEV_PANEL.MsgToLog(L"< " + filePath + L" 정보 >");
								MK_DEV_PANEL.MsgToLog(L"   - 원본 크기 : " + MkStr(originalSize) + L" bytes");
								if (originalSize > dataSize)
								{
									MK_DEV_PANEL.MsgToLog(L"   - 압축률 : " + MkStr(ConvertToPercentage<unsigned int, unsigned int>(dataSize, originalSize)) + L" %");
									MK_DEV_PANEL.MsgToLog(L"   - 실제 크기 : " + MkStr(dataSize) + L" bytes");
								}
								MK_DEV_PANEL.MsgToLog(L"   - 수정 시간 : " + MkStr(writtenTime));
							}
							else
							{
								MK_DEV_PANEL.MsgToLog(L"< " + filePath + L" 파일 존재하지 않음 >");
							}
						}
					}
					break;

				case MKDEF_APP_BTN_REMOVE_TARGET_FILE_ID:
					{
						MkStr fileName;
						if ((m_TargetFileHandle != NULL) && (m_CurrentNode != NULL) && _GetStringFromEditBox(m_TargetFileHandle, fileName))
						{
							MkPathName relativePath;
							_GetCurrentDirectoryPath(m_CurrentNode, relativePath);

							MkPathName filePath = relativePath + fileName;
							if (m_FileSystem.RemoveFile(filePath))
							{
								MkArray<MkStr> files;
								m_CurrentNode->GetData(L"Files", files);
								files.EraseFirstInclusion(MkArraySection(0), fileName);
								m_CurrentNode->SetData(L"Files", files);

								MK_DEV_PANEL.MsgToLog(L"< " + filePath + L" 파일 삭제 완료 >");
							}
							else
							{
								MK_DEV_PANEL.MsgToLog(L"< " + filePath + L" 파일 삭제 실패 >");
							}
						}
					}
					break;

				case MKDEF_APP_BTN_UNPACK_TARGET_FILE_ID:
					{
						MkStr fileName;
						if ((m_TargetFileHandle != NULL) && (m_CurrentNode != NULL) && _GetStringFromEditBox(m_TargetFileHandle, fileName))
						{
							MkPathName relativePath;
							_GetCurrentDirectoryPath(m_CurrentNode, relativePath);
							MkPathName filePath = relativePath + fileName;

							if (m_FileSystem.ExtractAvailableFile(filePath, m_ChunkFilePath))
							{
								MkPathName fullPath = m_ChunkFilePath + relativePath;
								fullPath.OpenDirectoryInExplorer();

								MK_DEV_PANEL.MsgToLog(L"< " + filePath + L" 파일 언패킹 완료 >");
							}
							else
							{
								MK_DEV_PANEL.MsgToLog(L"< " + filePath + L" 파일 언패킹 실패 >");
							}
						}
					}
					break;

				case MKDEF_APP_BTN_FIND_UPDATE_DIR_ID:
					{
						if (m_UpdateSrcDirHandle != NULL)
						{
							MkPathName defPath;
							if (!_GetPathFromEditBox(m_UpdateSrcDirHandle, defPath))
							{
								defPath = MkPathName::GetModuleDirectory();
							}

							MkPathName targetPath;
							if (targetPath.GetDirectoryPathFromDialog(L"갱신 할 베이스 디렉토리 선택", m_MainWindow.GetWindowHandle(), defPath))
							{
								SetWindowText(m_UpdateSrcDirHandle, targetPath.GetPtr());
							}
						}
					}
					break;

				case MKDEF_APP_BTN_UPDATE_DIR_ID:
					{
						if ((m_State == eFS_Idle) && (m_UpdateSrcDirHandle != NULL) && (m_DoUpdateHandle != NULL))
						{
							MkPathName srcPath;
							if (_GetPathFromEditBox(m_UpdateSrcDirHandle, srcPath))
							{
								EnableWindow(m_DoUpdateHandle, FALSE);
								
								m_UpdateSrcPath = srcPath;
								m_UpdateSrcPath.CheckAndAddBackslash();

								m_State = eFS_Updating;

								MK_DEV_PANEL.MsgToLog(L"< 갱신 중... >");
							}
						}
					}
					break;

				case MKDEF_APP_BTN_SHOW_CHUNK_INFO_ID:
					{
						MkMap<MkStr, unsigned int> buffer;
						if (m_FileSystem.GetBlankPercentage(buffer))
						{
							MK_DEV_PANEL.MsgToLog(L"< 청크 공극률 >");

							MkMapLooper<MkStr, unsigned int> looper(buffer);
							MK_STL_LOOP(looper)
							{
								MK_DEV_PANEL.MsgToLog(looper.GetCurrentKey() + L" : " + MkStr(looper.GetCurrentField()) + L" %");
							}
						}
					}
					break;

				case MKDEF_APP_BTN_OPT_CHUNK_ID:
					{
						if ((m_State == eFS_Idle) && (m_OptimizeHandle != NULL))
						{
							EnableWindow(m_OptimizeHandle, FALSE);

							MK_DEV_PANEL.MsgToLog(L"< 청크 최적화 시작 >");

							m_State = eFS_Optimizing;
						}
					}
					break;

				case MKDEF_APP_BTN_PRINT_STRUCTURE_ID:
					{
						if (m_CurrentNode != NULL)
						{
							MkPathName filePath = L"PackInfo.txt";
							m_SystemStructure.SaveToText(filePath);
							filePath.OpenFileInVerb();
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
					case MKDEF_APP_BTN_FIND_CHUNK_DIR_ID:
					case MKDEF_APP_BTN_LOAD_SYSTEM_DIR_ID:
					case MKDEF_APP_BTN_SHOW_DIR_LIST_ID:
					case MKDEF_APP_BTN_MOVE_TO_PARENT_ID:
					case MKDEF_APP_BTN_REMOVE_TARGET_DIR_ID:
					case MKDEF_APP_BTN_UNPACK_TARGET_DIR_ID:
					case MKDEF_APP_BTN_MOVE_TO_TARGET_DIR_ID:
					case MKDEF_APP_BTN_SHOW_FILE_LIST_ID:
					case MKDEF_APP_BTN_INFO_TARGET_FILE_ID:
					case MKDEF_APP_BTN_REMOVE_TARGET_FILE_ID:
					case MKDEF_APP_BTN_UNPACK_TARGET_FILE_ID:
					case MKDEF_APP_BTN_FIND_UPDATE_DIR_ID:
					case MKDEF_APP_BTN_UPDATE_DIR_ID:
					case MKDEF_APP_BTN_SHOW_CHUNK_INFO_ID:
					case MKDEF_APP_BTN_OPT_CHUNK_ID:
					case MKDEF_APP_BTN_PRINT_STRUCTURE_ID:
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

	virtual void Clear(void)
	{
		MkBaseFramework::Clear();

		m_FileSystem.Clear();
		m_SystemStructure.Clear();
	}

	TestFramework() : MkBaseFramework()
	{
		m_Font = NULL;

		m_ChunkDirHandle = NULL;
		m_SettingFileHandle = NULL;
		m_LoadingBtnHandle = NULL;

		m_CurrDirHandle = NULL;
		m_TargetDirHandle = NULL;
		m_UnpackDirHandle = NULL;

		m_TargetFileHandle = NULL;

		m_UpdateSrcDirHandle = NULL;
		m_DoUpdateHandle = NULL;

		m_OptimizeHandle = NULL;
		
		m_CurrentNode = NULL;
		m_State = eFS_Idle;
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

	void _GetCurrentDirectoryPath(MkDataNode* targetNode, MkPathName& buffer)
	{
		buffer.Clear();

		if (targetNode != NULL)
		{
			MkDataNode* target = targetNode;
			unsigned int depth = targetNode->GetDepthFromRootNode();
			if (depth > 0)
			{
				MkDeque<MkStr> nameList;

				while (depth > 0)
				{
					nameList.PushFront(target->GetNodeName().GetString());
					target = target->GetParentNode();
					--depth;
				}

				MkStr fullPath;
				MK_INDEXING_LOOP(nameList, i)
				{
					fullPath += nameList[i];
					fullPath += L"\\";
				}

				buffer = fullPath;
			}
		}
	}

	void _UpdateCurrentDirectory(MkDataNode* targetNode)
	{
		if (m_CurrDirHandle != NULL)
		{
			MkPathName buffer;
			_GetCurrentDirectoryPath(targetNode, buffer);

			SetWindowText(m_CurrDirHandle, buffer.GetPtr());
		}
	}

	void _RemoveDirectory(MkDataNode* targetNode)
	{
		if (targetNode != NULL)
		{
			// directory
			MkArray<MkHashStr> listBuffer;
			MkPathName::__GetSubDirectories(*targetNode, listBuffer);
			MK_INDEXING_LOOP(listBuffer, i)
			{
				_RemoveDirectory(targetNode->GetChildNode(listBuffer[i]));
			}
			listBuffer.Clear();

			// files
			MkPathName::__GetSubFiles(*targetNode, listBuffer);
			if (!listBuffer.Empty())
			{
				MkPathName relativePath;
				_GetCurrentDirectoryPath(targetNode, relativePath);

				MK_INDEXING_LOOP(listBuffer, i)
				{
					MkPathName filePath = relativePath + listBuffer[i];
					m_FileSystem.RemoveFile(filePath);
				}
			}
		}
	}

	void _UnpackDirectory(MkDataNode* targetNode)
	{
		if (targetNode != NULL)
		{
			// directory
			MkArray<MkHashStr> listBuffer;
			MkPathName::__GetSubDirectories(*targetNode, listBuffer);
			MK_INDEXING_LOOP(listBuffer, i)
			{
				_UnpackDirectory(targetNode->GetChildNode(listBuffer[i]));
			}
			listBuffer.Clear();

			// files
			MkPathName::__GetSubFiles(*targetNode, listBuffer);
			if (!listBuffer.Empty())
			{
				MkPathName relativePath;
				_GetCurrentDirectoryPath(targetNode, relativePath);

				MK_INDEXING_LOOP(listBuffer, i)
				{
					MkPathName filePath = relativePath + listBuffer[i];
					m_FileSystem.ExtractAvailableFile(filePath, m_UnpackingFilePath);
				}
			}
		}
	}

protected:

	HFONT m_Font;

	HWND m_ChunkDirHandle;
	HWND m_SettingFileHandle;
	HWND m_LoadingBtnHandle;

	HWND m_CurrDirHandle;
	HWND m_TargetDirHandle;
	HWND m_UnpackDirHandle;

	HWND m_TargetFileHandle;

	HWND m_UpdateSrcDirHandle;
	HWND m_DoUpdateHandle;

	HWND m_OptimizeHandle;

	MkFileSystem m_FileSystem;
	MkDataNode m_SystemStructure;
	MkDataNode* m_CurrentNode;

	eFrameworkState m_State;

	MkPathName m_ChunkFilePath;
	MkPathName m_SettingFilePath;
	MkPathName m_UnpackingFilePath;
	MkPathName m_UpdateSrcPath;
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
	application.Run(hI, L"MkFilePacker", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 600, 490, false, false, TestFramework::NewWndProc, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

