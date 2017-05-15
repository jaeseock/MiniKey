
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
		// ��Ʈ�� ���� �غ�
		m_Font = CreateFont
			(12, 0, 0, 0, 0, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH | FF_ROMAN, L"����");

		HWND hWnd = m_MainWindow.GetWindowHandle();
		HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE));

		const DWORD groupBoxControlStyle = WS_CHILD | WS_VISIBLE | BS_GROUPBOX;
		const DWORD staticTextControlStyle = WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY;
		const DWORD inputControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT;
		const DWORD buttonControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON;

		//------------------------------------------------------------------------------------------------//
		// �ʱ�ȭ ��Ʈ��
		//------------------------------------------------------------------------------------------------//

		int currentY = 10;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_GB_INIT_TAG_ID, L"�ʱ�ȭ", groupBoxControlStyle, MkIntRect(5, currentY, 590, 70));

		currentY += 20;
		m_ChunkDirHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_CHUNK_DIR_VALUE_ID, MkPathName::GetModuleDirectory().GetPtr(), inputControlStyle, MkIntRect(10, currentY, 540, 20));

		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FIND_CHUNK_DIR_ID, L"����", buttonControlStyle, MkIntRect(555, currentY, 35, 20));

		currentY += 25;
		_CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_CHUNK_PREFIX_ID, L"���� ���ϸ�", staticTextControlStyle, MkIntRect(10, currentY + 3, 70, 14));

		m_SettingFileHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_SETTING_FILE_ID, L"PackInfo.mmd", inputControlStyle, MkIntRect(85, currentY, 130, 20));

		m_LoadingBtnHandle = _CreateControl
			(hWnd, hInstance, L"button", MKDEF_APP_BTN_LOAD_SYSTEM_DIR_ID, L"���� �ý��� �ε�", buttonControlStyle, MkIntRect(220, currentY, 370, 20));

		//------------------------------------------------------------------------------------------------//
		// ���丮 ��Ʈ��
		//------------------------------------------------------------------------------------------------//

		currentY += 45;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BG_DIRECTORY_ID, L"���丮", groupBoxControlStyle, MkIntRect(5, currentY, 590, 95));

		currentY += 20;
		m_CurrDirHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_CURRENT_DIR_ID, L"", staticTextControlStyle, MkIntRect(10, currentY, 580, 20));

		currentY += 25;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_SHOW_DIR_LIST_ID, L"���� ���丮 ����Ʈ ���", buttonControlStyle, MkIntRect(10, currentY, 265, 20));
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_MOVE_TO_PARENT_ID, L"������ �̵�", buttonControlStyle, MkIntRect(280, currentY, 100, 20));
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_REMOVE_TARGET_DIR_ID, L"���丮 ����", buttonControlStyle, MkIntRect(385, currentY, 100, 20));
		m_UnpackDirHandle = _CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_UNPACK_TARGET_DIR_ID, L"���丮 Ǯ��", buttonControlStyle, MkIntRect(490, currentY, 100, 20));

		currentY += 25;
		m_TargetDirHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_TARGET_DIR_ID, L"", inputControlStyle, MkIntRect(10, currentY, 475, 20));

		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_MOVE_TO_TARGET_DIR_ID, L"Ÿ������ �̵�", buttonControlStyle, MkIntRect(490, currentY, 100, 20));

		//------------------------------------------------------------------------------------------------//
		// ���� ��Ʈ��
		//------------------------------------------------------------------------------------------------//

		currentY += 45;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BG_FILE_ID, L"����", groupBoxControlStyle, MkIntRect(5, currentY, 590, 70));

		currentY += 20;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_SHOW_FILE_LIST_ID, L"���� ���� ����Ʈ ���", buttonControlStyle, MkIntRect(10, currentY, 265, 20));

		currentY += 25;
		m_TargetFileHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_TARGET_FILE_ID, L"", inputControlStyle, MkIntRect(10, currentY, 265, 20));

		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_INFO_TARGET_FILE_ID, L"���� ����", buttonControlStyle, MkIntRect(280, currentY, 100, 20));
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_REMOVE_TARGET_FILE_ID, L"���� ����", buttonControlStyle, MkIntRect(385, currentY, 100, 20));
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_UNPACK_TARGET_FILE_ID, L"���� Ǯ��", buttonControlStyle, MkIntRect(490, currentY, 100, 20));

		//------------------------------------------------------------------------------------------------//
		// ���� ��Ʈ��
		//------------------------------------------------------------------------------------------------//

		currentY += 45;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_GB_UPDATE_TAG_ID, L"����", groupBoxControlStyle, MkIntRect(5, currentY, 590, 70));

		currentY += 20;
		m_UpdateSrcDirHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_UPDATE_DIR_VALUE_ID, MkPathName::GetModuleDirectory().GetPtr(), inputControlStyle, MkIntRect(10, currentY, 540, 20));

		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FIND_UPDATE_DIR_ID, L"����", buttonControlStyle, MkIntRect(555, currentY, 35, 20));

		currentY += 25;
		m_DoUpdateHandle = _CreateControl
			(hWnd, hInstance, L"button", MKDEF_APP_BTN_UPDATE_DIR_ID, L"���丮�� ����", buttonControlStyle, MkIntRect(10, currentY, 580, 20));

		//------------------------------------------------------------------------------------------------//
		// ûũ ��Ʈ��
		//------------------------------------------------------------------------------------------------//

		currentY += 45;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_GB_CHUNK_TAG_ID, L"ûũ", groupBoxControlStyle, MkIntRect(5, currentY, 590, 45));

		currentY += 20;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_SHOW_CHUNK_INFO_ID, L"���ط�(�� ���� ����) ���", buttonControlStyle, MkIntRect(10, currentY, 285, 20));

		m_OptimizeHandle = _CreateControl
			(hWnd, hInstance, L"button", MKDEF_APP_BTN_OPT_CHUNK_ID, L"����ȭ", buttonControlStyle, MkIntRect(305, currentY, 285, 20));

		//------------------------------------------------------------------------------------------------//
		// ��� ���
		//------------------------------------------------------------------------------------------------//

		currentY += 45;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_PRINT_STRUCTURE_ID, L"���� ������ ���� ���", buttonControlStyle, MkIntRect(5, currentY, 590, 20));
		
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

			// setting file�� ������ �ʱ�ȭ
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

				MessageBox(m_MainWindow.GetWindowHandle(), L"���� ����", L"Success!", MB_OK);
			}
			else
			{
				MessageBox(m_MainWindow.GetWindowHandle(), L"���� ����", L"Error!", MB_OK);
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

			MK_DEV_PANEL.MsgToLog(L"< ����ŷ �Ϸ� >");
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

				MK_DEV_PANEL.MsgToLog(L"< ���� �Ϸ� >");
			}
			else
			{
				MK_DEV_PANEL.MsgToLog(L"< ���� ����. ���� �ý��� �� �ε� �ʿ� >");
			}

			EnableWindow(m_DoUpdateHandle, TRUE);
		}

		if (m_State == eFS_Optimizing)
		{
			m_FileSystem.OptimizeAllChunks(0);

			EnableWindow(m_OptimizeHandle, TRUE);

			MK_DEV_PANEL.MsgToLog(L"< ûũ ����ȭ �Ϸ� >");
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
							if (targetPath.GetDirectoryPathFromDialog(L"ûũ ���ϵ��� �����ϴ� ���� ����", m_MainWindow.GetWindowHandle(), defPath))
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

								MK_DEV_PANEL.MsgToLog(m_SettingFilePath.Empty() ? L"���� ���� �ε� ����" : (settingFile + L" �ε� ����"));
								MK_DEV_PANEL.MsgToLog(L"< ���� �ý��� ���� ��... >");
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
								MK_DEV_PANEL.MsgToLog(L"< ���� ���丮�� �������� ���� >");
							}
							else
							{
								MK_INDEXING_LOOP(listBuffer, i)
								{
									MK_DEV_PANEL.MsgToLog(listBuffer[i].GetString());
								}
								MK_DEV_PANEL.MsgToLog(L"< " + MkStr(listBuffer.GetSize()) + L"���� ���� ���丮 >");
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
								MK_DEV_PANEL.MsgToLog(L"< ��Ʈ ���丮������ ���� Ž�� �Ұ� >");
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

								MK_DEV_PANEL.MsgToLog(L"< ��Ʈ ���丮 ���� ��� ���� �Ϸ� >");
							}
							else
							{
								parent->DetachChildNode(m_CurrentNode->GetNodeName());
								delete m_CurrentNode;
								m_CurrentNode = parent;
								_UpdateCurrentDirectory(m_CurrentNode);

								MK_DEV_PANEL.MsgToLog(L"< ���丮 ���� �Ϸ� �� ������ �̵� >");
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
							if (m_UnpackingFilePath.GetDirectoryPathFromDialog(L"Ǯ�� ���ϵ��� ��ġ �� ���� ����", m_MainWindow.GetWindowHandle(), defPath))
							{
								m_State = eFS_Unpacking;

								MK_DEV_PANEL.MsgToLog(L"< ����ŷ ��... >");
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
								MK_DEV_PANEL.MsgToLog(L"< ��� ���丮�� �������� ���� >");
							}
							else
							{
								m_CurrentNode = targetNode;
								_UpdateCurrentDirectory(m_CurrentNode);
								SetWindowText(m_TargetDirHandle, L"");

								MK_DEV_PANEL.MsgToLog(L"< ��� ���丮�� �̵� �Ϸ� >");
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
								MK_DEV_PANEL.MsgToLog(L"< ���� ������ �������� ���� >");
							}
							else
							{
								MK_INDEXING_LOOP(listBuffer, i)
								{
									MK_DEV_PANEL.MsgToLog(listBuffer[i].GetString());
								}
								MK_DEV_PANEL.MsgToLog(L"< " + MkStr(listBuffer.GetSize()) + L"���� ���� ���� >");
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
								MK_DEV_PANEL.MsgToLog(L"< " + filePath + L" ���� >");
								MK_DEV_PANEL.MsgToLog(L"   - ���� ũ�� : " + MkStr(originalSize) + L" bytes");
								if (originalSize > dataSize)
								{
									MK_DEV_PANEL.MsgToLog(L"   - ����� : " + MkStr(ConvertToPercentage<unsigned int, unsigned int>(dataSize, originalSize)) + L" %");
									MK_DEV_PANEL.MsgToLog(L"   - ���� ũ�� : " + MkStr(dataSize) + L" bytes");
								}
								MK_DEV_PANEL.MsgToLog(L"   - ���� �ð� : " + MkStr(writtenTime));
							}
							else
							{
								MK_DEV_PANEL.MsgToLog(L"< " + filePath + L" ���� �������� ���� >");
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

								MK_DEV_PANEL.MsgToLog(L"< " + filePath + L" ���� ���� �Ϸ� >");
							}
							else
							{
								MK_DEV_PANEL.MsgToLog(L"< " + filePath + L" ���� ���� ���� >");
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

								MK_DEV_PANEL.MsgToLog(L"< " + filePath + L" ���� ����ŷ �Ϸ� >");
							}
							else
							{
								MK_DEV_PANEL.MsgToLog(L"< " + filePath + L" ���� ����ŷ ���� >");
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
							if (targetPath.GetDirectoryPathFromDialog(L"���� �� ���̽� ���丮 ����", m_MainWindow.GetWindowHandle(), defPath))
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

								MK_DEV_PANEL.MsgToLog(L"< ���� ��... >");
							}
						}
					}
					break;

				case MKDEF_APP_BTN_SHOW_CHUNK_INFO_ID:
					{
						MkMap<MkStr, unsigned int> buffer;
						if (m_FileSystem.GetBlankPercentage(buffer))
						{
							MK_DEV_PANEL.MsgToLog(L"< ûũ ���ط� >");

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

							MK_DEV_PANEL.MsgToLog(L"< ûũ ����ȭ ���� >");

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

// ��Ʈ�� ����Ʈ������ TestApplication ����
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"MkFilePacker", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 600, 490, false, false, TestFramework::NewWndProc, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

