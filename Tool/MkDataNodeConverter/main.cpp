
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkLogicThreadUnit.h"

#include "MkCore_MkSystemEnvironment.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"

#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"


// ���� ���ϸ�
#define MKDEF_APP_SETTING_FILE_NAME L"MDC_SETTING.txt"

// ���� �⺻ ��
#define MKDEF_APP_DEF_OUTPUT_TYPE 1 // 0:text, 1:binary
#define MKDEF_APP_DEF_NEW_EXTENSION true
#define MKDEF_APP_DEF_NEW_EXT_VALUE L"mmd" // minikey metadata
#define MKDEF_APP_DEF_MAKE_BACKUP true

// ���� Ű
#define MKDEF_APP_KEY_OUTPUT_TYPE L"DefOutputType" // int
#define MKDEF_APP_KEY_NEW_EXT L"DefNewExtension" // bool
#define MKDEF_APP_KEY_EXT_VALUE L"DefExtensionValue" // str
#define MKDEF_APP_KEY_TARGET_DIR L"DefTargetDirectory" // str(path)
#define MKDEF_APP_KEY_MAKE_BACKUP L"DefBackupEnable" // bool

// control id
#define MKDEF_APP_BTN_FILE_SELECTION_ID 1

#define MKDEF_APP_CB_LOADTYPE_TAG_ID 2
#define MKDEF_APP_CB_LOADTYPE_TEMPLATE_ID 3

#define MKDEF_APP_CB_NEW_EXT_TAG_ID 4
#define MKDEF_APP_CB_NEW_EXTENSION_ID 5
#define MKDEF_APP_EDIT_NEW_EXT_ID 6

#define MKDEF_APP_RADIO_OUTPUT_TYPE_TAG_ID 7
#define MKDEF_APP_RADIO_OUTPUT_TYPE_TXT_ID 8
#define MKDEF_APP_RADIO_OUTPUT_TYPE_BIN_ID 9

#define MKDEF_APP_EDIT_TARGET_DIR_TAG_ID 10
#define MKDEF_APP_EDIT_TARGET_DIR_VALUE_ID 11
#define MKDEF_APP_BTN_FIND_TARGET_DIR_ID 12
#define MKDEF_APP_BTN_OPEN_TARGET_DIR_ID 13

#define MKDEF_APP_CB_MAKE_BACKUP_TAG_ID 14
#define MKDEF_APP_CB_MAKE_BACKUP_ID 15

#define MKDEF_APP_BTN_SAVE_ALL_ID 16


static MkArray<int> gFrameworkEvent(32);


typedef struct __NodeFileInfo
{
	MkPathName srcPath;
	MkDataNode node;
}
_NodeFileInfo;


// single thread
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg)
	{
		// ���� ���� �ε�
		MkPathName settingFile;
		settingFile.ConvertToRootBasisAbsolutePath(MKDEF_APP_SETTING_FILE_NAME);
		if (settingFile.CheckAvailable())
		{
			m_SettingNode.Load(settingFile);
		}

		// ���� ���� ���� ������� ���Ἲ�� �����ؾ� ��
		
		// ��� Ÿ�� �о���̰� ������ ����
		int outputTypeBuf = MKDEF_APP_DEF_OUTPUT_TYPE;
		if (!m_SettingNode.GetData(MKDEF_APP_KEY_OUTPUT_TYPE, outputTypeBuf, 0))
		{
			m_SettingNode.CreateUnit(MKDEF_APP_KEY_OUTPUT_TYPE, outputTypeBuf);
		}

		// Ȯ���� ���� ���� �о���̰� ������ ����
		bool newExt = MKDEF_APP_DEF_NEW_EXTENSION;
		if (!m_SettingNode.GetData(MKDEF_APP_KEY_NEW_EXT, newExt, 0))
		{
			m_SettingNode.CreateUnit(MKDEF_APP_KEY_NEW_EXT, newExt);
		}

		// Ȯ���� �� �о���̰� ������ ����
		MkStr extVal = MKDEF_APP_DEF_NEW_EXT_VALUE;
		if (!m_SettingNode.GetData(MKDEF_APP_KEY_EXT_VALUE, extVal, 0))
		{
			m_SettingNode.CreateUnit(MKDEF_APP_KEY_EXT_VALUE, extVal);
		}
		else
		{
			extVal.RemoveBlank();
			if (extVal.Empty())
			{
				extVal = MKDEF_APP_DEF_NEW_EXT_VALUE;
			}
			m_SettingNode.SetData(MKDEF_APP_KEY_EXT_VALUE, extVal, 0);
		}

		// ��� ��θ� �о���̰� ������ ����
		MkStr targetPathBuf = MkPathName::GetRootDirectory();
		if (!m_SettingNode.GetData(MKDEF_APP_KEY_TARGET_DIR, targetPathBuf, 0))
		{
			m_SettingNode.CreateUnit(MKDEF_APP_KEY_TARGET_DIR, targetPathBuf);
		}
		else
		{
			MkPathName tmpPath;
			tmpPath.ConvertToRootBasisAbsolutePath(targetPathBuf); // ��� ��ΰ� ��������� ���⼭ ���� ��η� �ٲ�
			if (!tmpPath.CheckAvailable()) // �������� �ʴ� ��θ� �ʱ�ȭ
			{
				tmpPath = MkPathName::GetRootDirectory();
			}
			
			targetPathBuf = tmpPath;
			m_SettingNode.SetData(MKDEF_APP_KEY_TARGET_DIR, targetPathBuf, 0); // ���� ��� �� ����
		}

		// ��� ���� ���� �о���̰� ������ ����
		bool makeBackupBuf = MKDEF_APP_DEF_MAKE_BACKUP;
		if (!m_SettingNode.GetData(MKDEF_APP_KEY_MAKE_BACKUP, makeBackupBuf, 0))
		{
			m_SettingNode.CreateUnit(MKDEF_APP_KEY_MAKE_BACKUP, makeBackupBuf);
		}

		// ��Ʈ�� ���� �غ�
		m_Font = CreateFont
			(12, 0, 0, 0, 0, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH | FF_ROMAN, L"����");

		HWND hWnd = m_MainWindow.GetWindowHandle();
		HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE));

		const DWORD checkboxControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_AUTOCHECKBOX;
		const DWORD radioControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_AUTORADIOBUTTON;
		const DWORD staticTextControlStyle = WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY;
		const DWORD inputControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT;
		const DWORD buttonControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON;

		int currentY = 5;

		// ��Ʈ�� ���� : �Է� ��ư
		_CreateControl
			(hWnd, hInstance, L"button", MKDEF_APP_BTN_FILE_SELECTION_ID, L"����� ���� �巡�� or ��ư ���� ���� ����",
			buttonControlStyle, MkIntRect(5, currentY, 285, 135));

		// ��Ʈ�� ���� : �Է� ����
		currentY = 150;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_CB_LOADTYPE_TAG_ID, L" �Է� ����", staticTextControlStyle, MkIntRect(5, currentY, 140, 12));
		currentY += 13;
		m_LoadTemplateOnlyHandle = _CreateControl
			(hWnd, hInstance, L"button", MKDEF_APP_CB_LOADTYPE_TEMPLATE_ID, L"���ø��� ����", checkboxControlStyle, MkIntRect(5, currentY, 285, 20));
		m_LoadTemplateOnly = true;
		SendMessage(m_LoadTemplateOnlyHandle, BM_SETCHECK, BST_CHECKED, 0);

		// ��Ʈ�� ���� : Ȯ���� ����
		currentY += 30;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_CB_NEW_EXT_TAG_ID, L" ����� Ȯ���� ����", staticTextControlStyle, MkIntRect(5, currentY, 140, 12));
		currentY += 13;
		m_NewExtensionHandle = _CreateControl
			(hWnd, hInstance, L"button", MKDEF_APP_CB_NEW_EXTENSION_ID, L"�� Ȯ���� ����", checkboxControlStyle, MkIntRect(5, currentY, 140, 20));
		SendMessage(m_NewExtensionHandle, BM_SETCHECK, (newExt) ? BST_CHECKED : BST_UNCHECKED, 0);
		m_NewExtValueHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_NEW_EXT_ID, extVal.GetPtr(), inputControlStyle, MkIntRect(150, currentY, 140, 20));
		EnableWindow(m_NewExtValueHandle, (newExt) ? TRUE : FALSE);

		// ��Ʈ�� ���� : ��� ����
		currentY += 30;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_RADIO_OUTPUT_TYPE_TAG_ID, L" ��� ����", staticTextControlStyle, MkIntRect(5, currentY, 140, 12));
		currentY += 13;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_RADIO_OUTPUT_TYPE_TXT_ID, L"�ؽ�Ʈ ���Ϸ� ���", radioControlStyle, MkIntRect(5, currentY, 140, 20));
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_RADIO_OUTPUT_TYPE_BIN_ID, L"���� ���Ϸ� ���", radioControlStyle, MkIntRect(150, currentY, 140, 20));
		_SetOutputType(outputTypeBuf);

		// ��Ʈ�� ���� : ��� ��� ����
		currentY += 30;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_TARGET_DIR_TAG_ID, L" ��� ��� ����", staticTextControlStyle, MkIntRect(5, currentY, 140, 12));
		currentY += 13;
		m_TargetDirHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_TARGET_DIR_VALUE_ID, targetPathBuf.GetPtr(), inputControlStyle, MkIntRect(5, currentY, 285, 20));
		currentY += 21;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FIND_TARGET_DIR_ID, L"��� ��� ���� ����", buttonControlStyle, MkIntRect(5, currentY, 140, 20));
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_OPEN_TARGET_DIR_ID, L"��� ��� ���� ����", buttonControlStyle, MkIntRect(150, currentY, 140, 20));

		// ��Ʈ�� ���� : ��� ��������
		currentY += 30;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_CB_MAKE_BACKUP_TAG_ID, L" ��� ���� ����", staticTextControlStyle, MkIntRect(5, currentY, 140, 12));
		currentY += 13;
		m_MakeBackupHandle = _CreateControl
			(hWnd, hInstance, L"button", MKDEF_APP_CB_MAKE_BACKUP_ID, L"�ڵ����� ����", checkboxControlStyle, MkIntRect(5, currentY, 285, 20));
		SendMessage(m_MakeBackupHandle, BM_SETCHECK, (makeBackupBuf) ? BST_CHECKED : BST_UNCHECKED, 0);

		// ��Ʈ�� ���� : ����
		currentY += 30;
		m_SaveButtonHandle = _CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_SAVE_ALL_ID, L"��� ����", buttonControlStyle, MkIntRect(5, currentY, 285, 30));
		_UpdateSaveButton();

		// dev panel�� ���
		MK_DEV_PANEL.MsgToFreeboard(0, L" < �Է� ���� > ");
		MK_DEV_PANEL.MsgToFreeboard(1, L"  - " + MkStr((m_LoadTemplateOnly) ? L"���ø��� ����(��� ����� �ƴ�)" :  L"��� ���ø��� ������ ����"));

		MK_DEV_PANEL.MsgToFreeboard(3, L" < ����� Ȯ���� ���� > ");
		MK_DEV_PANEL.MsgToFreeboard(4, L"  - " + MkStr((newExt) ? L"�� Ȯ���� ����" :  L"���� ���ϸ� �״�� ���"));

		MK_DEV_PANEL.MsgToFreeboard(6, L" < ��� ���� > ");
		MK_DEV_PANEL.MsgToFreeboard(7, L"  - " + MkStr((outputTypeBuf == 0) ? L"�ؽ�Ʈ ���Ϸ� ���" :  L"���� ���Ϸ� ���"));

		MK_DEV_PANEL.MsgToFreeboard(9, L" < ���� ���� ����� ��� ���� ���� >");
		MK_DEV_PANEL.MsgToFreeboard(10, L"  - " + MkStr((makeBackupBuf) ? L"�ڵ����� ����" :  L"�����"));

		MK_DEV_PANEL.MsgToFreeboard(12, L" < ��� �غ�� ���� >");
		
		return true;
	}

	virtual void StartLooping(void)
	{
		MK_DEV_PANEL.ClearLogWindow();
	}

	virtual void Update(void)
	{
		MK_INDEXING_LOOP(gFrameworkEvent, i)
		{
			switch (gFrameworkEvent[i])
			{
			case MKDEF_APP_BTN_FILE_SELECTION_ID:
				{
					MkPathName dirPath;
					MkArray<MkPathName> fileNameList;
					if (MkPathName::GetMultipleFilePathFromDialog(dirPath, fileNameList, m_MainWindow.GetWindowHandle()) > 0)
					{
						MkArray<MkPathName> draggedFilePathList(fileNameList.GetSize());
						MK_INDEXING_LOOP(fileNameList, j)
						{
							draggedFilePathList.PushBack(dirPath + fileNameList[j]);
						}

						ConsumeDraggedFiles(draggedFilePathList);
					}
				}
				break;

			case MKDEF_APP_CB_LOADTYPE_TEMPLATE_ID: // �Է� ���� üũ�ڽ� �̺�Ʈ
				{
					m_LoadTemplateOnly = (SendMessage(m_LoadTemplateOnlyHandle, BM_GETCHECK, 0, 0) == BST_CHECKED);

					MK_DEV_PANEL.MsgToFreeboard(1, L"  - " + MkStr((m_LoadTemplateOnly) ? L"���ø��� ����(��� ����� �ƴ�)" :  L"��� ���ø��� ������ ����"));
				}
				break;

			case MKDEF_APP_CB_NEW_EXTENSION_ID: // Ȯ���� ���� üũ�ڽ� �̺�Ʈ
				{
					bool newExt = (SendMessage(m_NewExtensionHandle, BM_GETCHECK, 0, 0) == BST_CHECKED);
					m_SettingNode.SetData(MKDEF_APP_KEY_NEW_EXT, newExt, 0);
					EnableWindow(m_NewExtValueHandle, (newExt) ? TRUE : FALSE);

					MK_DEV_PANEL.MsgToFreeboard(4, L"  - " + MkStr((newExt) ? L"�� Ȯ���� ����" :  L"���� ���ϸ� �״�� ���"));
				}
				break;

			case MKDEF_APP_RADIO_OUTPUT_TYPE_TXT_ID: _SetOutputType(0); break; // ��� ���� ���� ��ư �̺�Ʈ
			case MKDEF_APP_RADIO_OUTPUT_TYPE_BIN_ID: _SetOutputType(1); break;

			case MKDEF_APP_BTN_FIND_TARGET_DIR_ID: // ��� ��� ���� ���� �̺�Ʈ. ���̾�α׸� ���� ����
				{
					MkStr tmpBuf;
					MkPathName defPath;
					if (m_SettingNode.GetData(MKDEF_APP_KEY_TARGET_DIR, tmpBuf, 0))
					{
						defPath = tmpBuf;
					}

					MkPathName targetPath;
					if (targetPath.GetDirectoryPathFromDialog(L"��� ��� ������ ����", m_MainWindow.GetWindowHandle(), defPath))
					{
						m_SettingNode.SetData(MKDEF_APP_KEY_TARGET_DIR, targetPath, 0);

						if (m_TargetDirHandle != NULL)
						{
							SetWindowText(m_TargetDirHandle, targetPath.GetPtr());
						}
					}
				}
				break;

			case MKDEF_APP_BTN_OPEN_TARGET_DIR_ID: // ��� ��� ���� ���� �̺�Ʈ
				{
					MkStr tmpBuf;
					if (m_SettingNode.GetData(MKDEF_APP_KEY_TARGET_DIR, tmpBuf, 0))
					{
						MkPathName targetPath = tmpBuf;
						targetPath.OpenDirectoryInExplorer();
					}
				}
				break;

			case MKDEF_APP_CB_MAKE_BACKUP_ID: // ��� ���� ���� üũ�ڽ� �̺�Ʈ
				{
					bool makeBackupBuf = (SendMessage(m_MakeBackupHandle, BM_GETCHECK, 0, 0) == BST_CHECKED);
					m_SettingNode.SetData(MKDEF_APP_KEY_MAKE_BACKUP, makeBackupBuf, 0);

					MK_DEV_PANEL.MsgToFreeboard(10, L"  - " + MkStr((makeBackupBuf) ? L"�ڵ����� ����" :  L"�����"));
				}
				break;

			case MKDEF_APP_BTN_SAVE_ALL_ID: // ��� ���� ��ư �̺�Ʈ. ��ư�� Ȱ��ȭ�Ǿ��ٴ� ���� �ּ��� �ϳ� �̻� ��� �� ������ �������� �ǹ�
				{
					// edit box�κ��� ��� ��θ� �о����
					wchar_t pathBuf[MAX_PATH + 1] = {0, };
					GetWindowText(m_TargetDirHandle, pathBuf, MAX_PATH + 1);
					MkPathName targetPath = pathBuf;
					if (targetPath.Empty() || (!targetPath.IsDirectoryPath()) || (!targetPath.CheckAvailable()))
					{
						MessageBox(m_MainWindow.GetWindowHandle(), L"��� ��� ���丮 ��ΰ� �ùٸ��� �ʽ��ϴ�.", L"Error", MB_OK);
					}
					else
					{
						// ��� ��� ���丮 ����
						targetPath.CheckAndAddBackslash();
						m_SettingNode.SetData(MKDEF_APP_KEY_TARGET_DIR, targetPath, 0);

						// ��� ����
						bool newExt, makeBackupBuf;
						int outputTypeBuf;
						if (m_SettingNode.GetData(MKDEF_APP_KEY_NEW_EXT, newExt, 0) &&
							m_SettingNode.GetData(MKDEF_APP_KEY_OUTPUT_TYPE, outputTypeBuf, 0) &&
							m_SettingNode.GetData(MKDEF_APP_KEY_MAKE_BACKUP, makeBackupBuf, 0))
						{
							// Ȯ���� �о ����
							wchar_t extBuf[MAX_PATH + 1] = {0, };
							GetWindowText(m_NewExtValueHandle, extBuf, MAX_PATH + 1);
							MkStr extVal = extBuf;
							extVal.RemoveBlank();
							if (extVal.Empty())
							{
								extVal = MKDEF_APP_DEF_NEW_EXT_VALUE;
							}
							m_SettingNode.SetData(MKDEF_APP_KEY_EXT_VALUE, extVal, 0);
							SetWindowText(m_NewExtValueHandle, extVal.GetPtr());

							// ��� ������ time stamp�� ���� ���� ��� ����
							MkStr backupFolderName;
							MkPathName backupPath;
							if (makeBackupBuf)
							{
								MkStr dateBuf = MK_SYS_ENV.GetCurrentSystemDate(); // ...\2014.12.14
								dateBuf.RemoveKeyword(L"."); // ex> // "2014.12.14" -> "20141214"
								MkStr timeBuf = MK_SYS_ENV.GetCurrentSystemTime();
								timeBuf.RemoveKeyword(L":"); // ex> "14:08:50" -> "140850"

								backupFolderName.Reserve(dateBuf.GetSize() + timeBuf.GetSize() + 1);
								backupFolderName += dateBuf;
								backupFolderName += L"_";
								backupFolderName += timeBuf; // ex> 20141214_140850

								backupPath += targetPath;
								backupPath += backupFolderName;
								backupPath.CheckAndAddBackslash();
							}

							MK_DEV_PANEL.MsgToLog(L"< ��� ���� >----------------------------------------");

							// ���Ϻ� ���
							bool backupApplied = false;
							MK_INDEXING_LOOP(m_NodeInfo, j)
							{
								const MkPathName& srcPath = m_NodeInfo[j].srcPath;
								if (!srcPath.Empty())
								{
									// ���� ��� ����. ������ ���� Ȯ���� ����
									MkPathName fileName = srcPath.GetFileName();
									if (newExt)
									{
										fileName.ChangeFileExtension(extVal);
									}
									MkPathName newPath = targetPath + fileName;

									// ���� ���� ���翩�� üũ�� ������ ���� ����� ����
									bool makeBackupFile = (makeBackupBuf && newPath.CheckAvailable());
									if (makeBackupFile)
									{
										backupPath.MakeDirectoryPath();
										newPath.MoveCurrentFile(backupPath + fileName);
										backupApplied = true;
									}

									// ������ ���·� ���� ���
									MkStr msg = L"   - ���";
									if (outputTypeBuf == 0)
									{
										m_NodeInfo[i].node.SaveToText(newPath);
										msg += L"(txt)";
									}
									else if (outputTypeBuf == 1)
									{
										m_NodeInfo[i].node.SaveToBinary(newPath);
										msg += L"(bin)";
									}

									msg += L" �Ϸ� : ";
									msg += fileName;

									if (makeBackupFile)
									{
										msg += L" [���]";
									}
									MK_DEV_PANEL.MsgToLog(msg);
								}
							}

							if (backupApplied)
							{
								MK_DEV_PANEL.MsgToLog(L"   * ��� ���� : " + backupFolderName);
							}

							MK_DEV_PANEL.MsgToLog(L"< ��� ���� >" + MkStr::CR);

							m_NodeInfo.Clear();
							_UpdateSaveButton();
						}
					}
				}
				break;
			}
		}
		gFrameworkEvent.Clear();
	}

	virtual void ConsumeDraggedFiles(MkArray<MkPathName>& draggedFilePathList)
	{
		if (!draggedFilePathList.Empty())
		{
			// ���ø��� �߰�. ��� ��� ���Ͽ��� ���� ����
			if (m_LoadTemplateOnly)
			{
				MK_DEV_PANEL.MsgToLog(L"< ���ø� �߰� ���� >---------------------------------");

				MK_INDEXING_LOOP(draggedFilePathList, i)
				{
					const MkPathName& srcPath = draggedFilePathList[i];
					if (MkDataNode::RegisterTemplate(srcPath))
					{
						MK_DEV_PANEL.MsgToLog(L"   - " + srcPath.GetFileName());
					}
				}

				MK_DEV_PANEL.MsgToLog(L"< ���ø� �߰� ���� >" + MkStr::CR);
			}
			// ��� ��� �߰�
			else
			{
				MK_DEV_PANEL.MsgToLog(L"< ������ �߰� ���� >---------------------------------");

				m_NodeInfo.Reserve(m_NodeInfo.GetSize() + draggedFilePathList.GetSize());

				MK_INDEXING_LOOP(draggedFilePathList, i)
				{
					const MkPathName& srcPath = draggedFilePathList[i];

					_NodeFileInfo& fi = m_NodeInfo.PushBack();
					if (fi.node.Load(srcPath))
					{
						fi.srcPath = srcPath;
						MK_DEV_PANEL.MsgToLog(L"   - " + srcPath.GetFileName());
					}
				}

				MK_DEV_PANEL.MsgToLog(L"< ������ �߰� ���� >" + MkStr::CR);
				
				_UpdateSaveButton();
			}		

			draggedFilePathList.Clear();
		}
	}

	virtual void Clear(void)
	{
		// �����ϱ� �� ��� ��θ� �������� '\' ����
		MkStr tmpStr;
		m_SettingNode.GetData(MKDEF_APP_KEY_TARGET_DIR, tmpStr, 0);
		tmpStr.BackSpace(1);
		m_SettingNode.SetData(MKDEF_APP_KEY_TARGET_DIR, tmpStr, 0);

		// ����
		m_SettingNode.SaveToText(MKDEF_APP_SETTING_FILE_NAME);
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
					case MKDEF_APP_BTN_FILE_SELECTION_ID:
					case MKDEF_APP_CB_LOADTYPE_TEMPLATE_ID:
					case MKDEF_APP_CB_NEW_EXTENSION_ID:
					case MKDEF_APP_RADIO_OUTPUT_TYPE_TXT_ID:
					case MKDEF_APP_RADIO_OUTPUT_TYPE_BIN_ID:
					case MKDEF_APP_BTN_FIND_TARGET_DIR_ID:
					case MKDEF_APP_BTN_OPEN_TARGET_DIR_ID:
					case MKDEF_APP_CB_MAKE_BACKUP_ID:
					case MKDEF_APP_BTN_SAVE_ALL_ID:
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
		m_LoadTemplateOnly = true;
		m_LoadTemplateOnlyHandle = NULL;
		m_NewExtensionHandle = NULL;
		m_NewExtValueHandle = NULL;
		m_TargetDirHandle = NULL;
		m_MakeBackupHandle = NULL;
		m_SaveButtonHandle = NULL;
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

	void _SetOutputType(int type)
	{
		m_SettingNode.SetData(MKDEF_APP_KEY_OUTPUT_TYPE, type, 0);
		CheckRadioButton(m_MainWindow.GetWindowHandle(), MKDEF_APP_RADIO_OUTPUT_TYPE_TXT_ID, MKDEF_APP_RADIO_OUTPUT_TYPE_BIN_ID, MKDEF_APP_RADIO_OUTPUT_TYPE_TXT_ID + type);

		MK_DEV_PANEL.MsgToFreeboard(7, L"  - " + MkStr((type == 0) ? L"�ؽ�Ʈ ���Ϸ� ���" :  L"���� ���Ϸ� ���"));
	}

	void _UpdateSaveButton(void)
	{
		unsigned int cnt = 0;
		MK_INDEXING_LOOP(m_NodeInfo, i)
		{
			if (!m_NodeInfo[i].srcPath.Empty())
			{
				++cnt;
			}
		}

		MK_DEV_PANEL.MsgToFreeboard(13, L"  - " + MkStr(cnt) + L" ��");

		if (m_SaveButtonHandle != NULL)
		{
			EnableWindow(m_SaveButtonHandle, (cnt > 0) ? TRUE : FALSE);
		}
	}


protected:

	HFONT m_Font;

	bool m_LoadTemplateOnly;
	HWND m_LoadTemplateOnlyHandle;

	HWND m_NewExtensionHandle;
	HWND m_NewExtValueHandle;

	HWND m_TargetDirHandle;

	HWND m_MakeBackupHandle;

	HWND m_SaveButtonHandle;

	MkDataNode m_SettingNode;

	MkArray<_NodeFileInfo> m_NodeInfo;
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
	application.Run(hI, L"MkDataNodeConverter", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 295, 420, false, true, TestFramework::NewWndProc, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

