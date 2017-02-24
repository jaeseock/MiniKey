
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkLogicThreadUnit.h"

#include "MkCore_MkSystemEnvironment.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"

#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"


// 세팅 파일명
#define MKDEF_APP_SETTING_FILE_NAME L"MDC_SETTING.txt"

// 설정 기본 값
#define MKDEF_APP_DEF_OUTPUT_TYPE 1 // 0:text, 1:binary
#define MKDEF_APP_DEF_NEW_EXTENSION true
#define MKDEF_APP_DEF_NEW_EXT_VALUE L"mmd" // minikey metadata
#define MKDEF_APP_DEF_MAKE_BACKUP true

// 설정 키
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
		// 세팅 파일 로드
		MkPathName settingFile;
		settingFile.ConvertToRootBasisAbsolutePath(MKDEF_APP_SETTING_FILE_NAME);
		if (settingFile.CheckAvailable())
		{
			m_SettingNode.Load(settingFile);
		}

		// 세팅 파일 여부 상관없이 무결성을 보장해야 함
		
		// 출력 타입 읽어들이고 없으면 복구
		int outputTypeBuf = MKDEF_APP_DEF_OUTPUT_TYPE;
		if (!m_SettingNode.GetData(MKDEF_APP_KEY_OUTPUT_TYPE, outputTypeBuf, 0))
		{
			m_SettingNode.CreateUnit(MKDEF_APP_KEY_OUTPUT_TYPE, outputTypeBuf);
		}

		// 확장자 형식 여부 읽어들이고 없으면 복구
		bool newExt = MKDEF_APP_DEF_NEW_EXTENSION;
		if (!m_SettingNode.GetData(MKDEF_APP_KEY_NEW_EXT, newExt, 0))
		{
			m_SettingNode.CreateUnit(MKDEF_APP_KEY_NEW_EXT, newExt);
		}

		// 확장자 값 읽어들이고 없으면 복구
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

		// 출력 경로명 읽어들이고 없으면 복구
		MkStr targetPathBuf = MkPathName::GetRootDirectory();
		if (!m_SettingNode.GetData(MKDEF_APP_KEY_TARGET_DIR, targetPathBuf, 0))
		{
			m_SettingNode.CreateUnit(MKDEF_APP_KEY_TARGET_DIR, targetPathBuf);
		}
		else
		{
			MkPathName tmpPath;
			tmpPath.ConvertToRootBasisAbsolutePath(targetPathBuf); // 상대 경로가 들어있으면 여기서 절대 경로로 바뀜
			if (!tmpPath.CheckAvailable()) // 존재하지 않는 경로면 초기화
			{
				tmpPath = MkPathName::GetRootDirectory();
			}
			
			targetPathBuf = tmpPath;
			m_SettingNode.SetData(MKDEF_APP_KEY_TARGET_DIR, targetPathBuf, 0); // 절대 경로 값 유지
		}

		// 백업 유지 여부 읽어들이고 없으면 복구
		bool makeBackupBuf = MKDEF_APP_DEF_MAKE_BACKUP;
		if (!m_SettingNode.GetData(MKDEF_APP_KEY_MAKE_BACKUP, makeBackupBuf, 0))
		{
			m_SettingNode.CreateUnit(MKDEF_APP_KEY_MAKE_BACKUP, makeBackupBuf);
		}

		// 콘트롤 생성 준비
		m_Font = CreateFont
			(12, 0, 0, 0, 0, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH | FF_ROMAN, L"굴림");

		HWND hWnd = m_MainWindow.GetWindowHandle();
		HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE));

		const DWORD checkboxControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_AUTOCHECKBOX;
		const DWORD radioControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_AUTORADIOBUTTON;
		const DWORD staticTextControlStyle = WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY;
		const DWORD inputControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT;
		const DWORD buttonControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON;

		int currentY = 5;

		// 콘트롤 생성 : 입력 버튼
		_CreateControl
			(hWnd, hInstance, L"button", MKDEF_APP_BTN_FILE_SELECTION_ID, L"여기로 파일 드래그 or 버튼 눌러 직접 선택",
			buttonControlStyle, MkIntRect(5, currentY, 285, 135));

		// 콘트롤 생성 : 입력 형태
		currentY = 150;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_CB_LOADTYPE_TAG_ID, L" 입력 형태", staticTextControlStyle, MkIntRect(5, currentY, 140, 12));
		currentY += 13;
		m_LoadTemplateOnlyHandle = _CreateControl
			(hWnd, hInstance, L"button", MKDEF_APP_CB_LOADTYPE_TEMPLATE_ID, L"템플릿만 읽음", checkboxControlStyle, MkIntRect(5, currentY, 285, 20));
		m_LoadTemplateOnly = true;
		SendMessage(m_LoadTemplateOnlyHandle, BM_SETCHECK, BST_CHECKED, 0);

		// 콘트롤 생성 : 확장자 형식
		currentY += 30;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_CB_NEW_EXT_TAG_ID, L" 저장시 확장자 적용", staticTextControlStyle, MkIntRect(5, currentY, 140, 12));
		currentY += 13;
		m_NewExtensionHandle = _CreateControl
			(hWnd, hInstance, L"button", MKDEF_APP_CB_NEW_EXTENSION_ID, L"새 확장자 적용", checkboxControlStyle, MkIntRect(5, currentY, 140, 20));
		SendMessage(m_NewExtensionHandle, BM_SETCHECK, (newExt) ? BST_CHECKED : BST_UNCHECKED, 0);
		m_NewExtValueHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_NEW_EXT_ID, extVal.GetPtr(), inputControlStyle, MkIntRect(150, currentY, 140, 20));
		EnableWindow(m_NewExtValueHandle, (newExt) ? TRUE : FALSE);

		// 콘트롤 생성 : 출력 형태
		currentY += 30;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_RADIO_OUTPUT_TYPE_TAG_ID, L" 출력 형태", staticTextControlStyle, MkIntRect(5, currentY, 140, 12));
		currentY += 13;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_RADIO_OUTPUT_TYPE_TXT_ID, L"텍스트 파일로 출력", radioControlStyle, MkIntRect(5, currentY, 140, 20));
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_RADIO_OUTPUT_TYPE_BIN_ID, L"이진 파일로 출력", radioControlStyle, MkIntRect(150, currentY, 140, 20));
		_SetOutputType(outputTypeBuf);

		// 콘트롤 생성 : 출력 대상 폴더
		currentY += 30;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_TARGET_DIR_TAG_ID, L" 출력 대상 폴더", staticTextControlStyle, MkIntRect(5, currentY, 140, 12));
		currentY += 13;
		m_TargetDirHandle = _CreateControl
			(hWnd, hInstance, L"edit", MKDEF_APP_EDIT_TARGET_DIR_VALUE_ID, targetPathBuf.GetPtr(), inputControlStyle, MkIntRect(5, currentY, 285, 20));
		currentY += 21;
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_FIND_TARGET_DIR_ID, L"출력 대상 폴더 변경", buttonControlStyle, MkIntRect(5, currentY, 140, 20));
		_CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_OPEN_TARGET_DIR_ID, L"출력 대상 폴더 열기", buttonControlStyle, MkIntRect(150, currentY, 140, 20));

		// 콘트롤 생성 : 백업 유지여부
		currentY += 30;
		_CreateControl(hWnd, hInstance, L"edit", MKDEF_APP_CB_MAKE_BACKUP_TAG_ID, L" 백업 유지 여부", staticTextControlStyle, MkIntRect(5, currentY, 140, 12));
		currentY += 13;
		m_MakeBackupHandle = _CreateControl
			(hWnd, hInstance, L"button", MKDEF_APP_CB_MAKE_BACKUP_ID, L"자동으로 유지", checkboxControlStyle, MkIntRect(5, currentY, 285, 20));
		SendMessage(m_MakeBackupHandle, BM_SETCHECK, (makeBackupBuf) ? BST_CHECKED : BST_UNCHECKED, 0);

		// 콘트롤 생성 : 저장
		currentY += 30;
		m_SaveButtonHandle = _CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_SAVE_ALL_ID, L"모두 저장", buttonControlStyle, MkIntRect(5, currentY, 285, 30));
		_UpdateSaveButton();

		// dev panel에 출력
		MK_DEV_PANEL.MsgToFreeboard(0, L" < 입력 형태 > ");
		MK_DEV_PANEL.MsgToFreeboard(1, L"  - " + MkStr((m_LoadTemplateOnly) ? L"템플릿만 읽음(출력 대상이 아님)" :  L"모든 템플릿과 데이터 읽음"));

		MK_DEV_PANEL.MsgToFreeboard(3, L" < 저장시 확장자 적용 > ");
		MK_DEV_PANEL.MsgToFreeboard(4, L"  - " + MkStr((newExt) ? L"새 확장자 적용" :  L"원본 파일명 그대로 사용"));

		MK_DEV_PANEL.MsgToFreeboard(6, L" < 출력 형태 > ");
		MK_DEV_PANEL.MsgToFreeboard(7, L"  - " + MkStr((outputTypeBuf == 0) ? L"텍스트 파일로 출력" :  L"이진 파일로 출력"));

		MK_DEV_PANEL.MsgToFreeboard(9, L" < 기존 파일 존재시 백업 유지 여부 >");
		MK_DEV_PANEL.MsgToFreeboard(10, L"  - " + MkStr((makeBackupBuf) ? L"자동으로 유지" :  L"덮어쓰기"));

		MK_DEV_PANEL.MsgToFreeboard(12, L" < 출력 준비된 파일 >");
		
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

			case MKDEF_APP_CB_LOADTYPE_TEMPLATE_ID: // 입력 형태 체크박스 이벤트
				{
					m_LoadTemplateOnly = (SendMessage(m_LoadTemplateOnlyHandle, BM_GETCHECK, 0, 0) == BST_CHECKED);

					MK_DEV_PANEL.MsgToFreeboard(1, L"  - " + MkStr((m_LoadTemplateOnly) ? L"템플릿만 읽음(출력 대상이 아님)" :  L"모든 템플릿과 데이터 읽음"));
				}
				break;

			case MKDEF_APP_CB_NEW_EXTENSION_ID: // 확장자 형식 체크박스 이벤트
				{
					bool newExt = (SendMessage(m_NewExtensionHandle, BM_GETCHECK, 0, 0) == BST_CHECKED);
					m_SettingNode.SetData(MKDEF_APP_KEY_NEW_EXT, newExt, 0);
					EnableWindow(m_NewExtValueHandle, (newExt) ? TRUE : FALSE);

					MK_DEV_PANEL.MsgToFreeboard(4, L"  - " + MkStr((newExt) ? L"새 확장자 적용" :  L"원본 파일명 그대로 사용"));
				}
				break;

			case MKDEF_APP_RADIO_OUTPUT_TYPE_TXT_ID: _SetOutputType(0); break; // 출력 형태 라디오 버튼 이벤트
			case MKDEF_APP_RADIO_OUTPUT_TYPE_BIN_ID: _SetOutputType(1); break;

			case MKDEF_APP_BTN_FIND_TARGET_DIR_ID: // 출력 대상 폴더 선택 이벤트. 다이얼로그를 열어 선택
				{
					MkStr tmpBuf;
					MkPathName defPath;
					if (m_SettingNode.GetData(MKDEF_APP_KEY_TARGET_DIR, tmpBuf, 0))
					{
						defPath = tmpBuf;
					}

					MkPathName targetPath;
					if (targetPath.GetDirectoryPathFromDialog(L"출력 대상 폴더를 선택", m_MainWindow.GetWindowHandle(), defPath))
					{
						m_SettingNode.SetData(MKDEF_APP_KEY_TARGET_DIR, targetPath, 0);

						if (m_TargetDirHandle != NULL)
						{
							SetWindowText(m_TargetDirHandle, targetPath.GetPtr());
						}
					}
				}
				break;

			case MKDEF_APP_BTN_OPEN_TARGET_DIR_ID: // 출력 대상 폴더 열기 이벤트
				{
					MkStr tmpBuf;
					if (m_SettingNode.GetData(MKDEF_APP_KEY_TARGET_DIR, tmpBuf, 0))
					{
						MkPathName targetPath = tmpBuf;
						targetPath.OpenDirectoryInExplorer();
					}
				}
				break;

			case MKDEF_APP_CB_MAKE_BACKUP_ID: // 백업 유지 여부 체크박스 이벤트
				{
					bool makeBackupBuf = (SendMessage(m_MakeBackupHandle, BM_GETCHECK, 0, 0) == BST_CHECKED);
					m_SettingNode.SetData(MKDEF_APP_KEY_MAKE_BACKUP, makeBackupBuf, 0);

					MK_DEV_PANEL.MsgToFreeboard(10, L"  - " + MkStr((makeBackupBuf) ? L"자동으로 유지" :  L"덮어쓰기"));
				}
				break;

			case MKDEF_APP_BTN_SAVE_ALL_ID: // 모두 저장 버튼 이벤트. 버튼이 활성화되었다는 뜻은 최소한 하나 이상 출력 할 파일이 존재함을 의미
				{
					// edit box로부터 대상 경로를 읽어들임
					wchar_t pathBuf[MAX_PATH + 1] = {0, };
					GetWindowText(m_TargetDirHandle, pathBuf, MAX_PATH + 1);
					MkPathName targetPath = pathBuf;
					if (targetPath.Empty() || (!targetPath.IsDirectoryPath()) || (!targetPath.CheckAvailable()))
					{
						MessageBox(m_MainWindow.GetWindowHandle(), L"출력 대상 디렉토리 경로가 올바르지 않습니다.", L"Error", MB_OK);
					}
					else
					{
						// 출력 대상 디렉토리 갱신
						targetPath.CheckAndAddBackslash();
						m_SettingNode.SetData(MKDEF_APP_KEY_TARGET_DIR, targetPath, 0);

						// 출력 형태
						bool newExt, makeBackupBuf;
						int outputTypeBuf;
						if (m_SettingNode.GetData(MKDEF_APP_KEY_NEW_EXT, newExt, 0) &&
							m_SettingNode.GetData(MKDEF_APP_KEY_OUTPUT_TYPE, outputTypeBuf, 0) &&
							m_SettingNode.GetData(MKDEF_APP_KEY_MAKE_BACKUP, makeBackupBuf, 0))
						{
							// 확장자 읽어서 저장
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

							// 백업 유지면 time stamp로 보관 폴더 경로 생성
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

							MK_DEV_PANEL.MsgToLog(L"< 출력 시작 >----------------------------------------");

							// 파일별 출력
							bool backupApplied = false;
							MK_INDEXING_LOOP(m_NodeInfo, j)
							{
								const MkPathName& srcPath = m_NodeInfo[j].srcPath;
								if (!srcPath.Empty())
								{
									// 파일 경로 생성. 설정에 따라 확장자 변경
									MkPathName fileName = srcPath.GetFileName();
									if (newExt)
									{
										fileName.ChangeFileExtension(extVal);
									}
									MkPathName newPath = targetPath + fileName;

									// 기존 파일 존재여부 체크해 설정에 따라 백업본 생성
									bool makeBackupFile = (makeBackupBuf && newPath.CheckAvailable());
									if (makeBackupFile)
									{
										backupPath.MakeDirectoryPath();
										newPath.MoveCurrentFile(backupPath + fileName);
										backupApplied = true;
									}

									// 설정된 형태로 파일 출력
									MkStr msg = L"   - 출력";
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

									msg += L" 완료 : ";
									msg += fileName;

									if (makeBackupFile)
									{
										msg += L" [백업]";
									}
									MK_DEV_PANEL.MsgToLog(msg);
								}
							}

							if (backupApplied)
							{
								MK_DEV_PANEL.MsgToLog(L"   * 백업 폴더 : " + backupFolderName);
							}

							MK_DEV_PANEL.MsgToLog(L"< 출력 종료 >" + MkStr::CR);

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
			// 템플릿만 추가. 출력 대상 파일에는 영향 없음
			if (m_LoadTemplateOnly)
			{
				MK_DEV_PANEL.MsgToLog(L"< 템플릿 추가 시작 >---------------------------------");

				MK_INDEXING_LOOP(draggedFilePathList, i)
				{
					const MkPathName& srcPath = draggedFilePathList[i];
					if (MkDataNode::RegisterTemplate(srcPath))
					{
						MK_DEV_PANEL.MsgToLog(L"   - " + srcPath.GetFileName());
					}
				}

				MK_DEV_PANEL.MsgToLog(L"< 템플릿 추가 종료 >" + MkStr::CR);
			}
			// 출력 대상 추가
			else
			{
				MK_DEV_PANEL.MsgToLog(L"< 데이터 추가 시작 >---------------------------------");

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

				MK_DEV_PANEL.MsgToLog(L"< 데이터 추가 종료 >" + MkStr::CR);
				
				_UpdateSaveButton();
			}		

			draggedFilePathList.Clear();
		}
	}

	virtual void Clear(void)
	{
		// 저장하기 전 출력 경로명 마지막의 '\' 제거
		MkStr tmpStr;
		m_SettingNode.GetData(MKDEF_APP_KEY_TARGET_DIR, tmpStr, 0);
		tmpStr.BackSpace(1);
		m_SettingNode.SetData(MKDEF_APP_KEY_TARGET_DIR, tmpStr, 0);

		// 저장
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

		MK_DEV_PANEL.MsgToFreeboard(7, L"  - " + MkStr((type == 0) ? L"텍스트 파일로 출력" :  L"이진 파일로 출력"));
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

		MK_DEV_PANEL.MsgToFreeboard(13, L"  - " + MkStr(cnt) + L" 개");

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

// 엔트리 포인트에서의 TestApplication 실행
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"MkDataNodeConverter", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 295, 420, false, true, TestFramework::NewWndProc, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

