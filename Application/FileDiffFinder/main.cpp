
//------------------------------------------------------------------------------------------------//
// single/multi-thread rendering application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkCheck.h"
#include "MkCore_MkPageManager.h"
//#include "MkCore_MkInputManager.h"
//#include "MkCore_MkTimeState.h"
#include "MkCore_MkWin32Application.h"
#include "MkCore_MkBaseFramework.h"
//#include "MkCore_MkProfilingManager.h"
//#include "MkCore_MkKeywordFilter.h"
//#include "MkCore_MkFloatOp.h"

#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkSystemEnvironment.h"

#define MKDEF_APP_BTN_EXPORT_ID 1


const static MkHashStr VersionTag = L"Version";

static MkArray<int> gFrameworkEvent(32);

//------------------------------------------------------------------------------------------------//

// TestPage 선언
class TestPage : public MkBasePage
{
protected:

	enum eAppState
	{
		eAS_Init = 0, // 시작
		eAS_LoadSrc, // 원본 정보 분석
		eAS_LoadInfo, // 이전 정보 읽음. 이전 정보가 없으면 원본으로 초기화
		eAS_FindDiffs, // 패치 대상 추출
		eAS_Wait, // 대기
		eAS_ExportDiff // 패치 대상 출력 후 이전 정보를 원본 정보로 갱신
	};

public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		// 설정파일로부터 초기화
		MkDataNode settingNode;
		MK_CHECK(settingNode.Load(L"setting.txt"), L"setting.txt 파일이 없음")
			return false;

		// 원본 경로
		MkPathName targetSrcPath;
		MK_CHECK(settingNode.GetData(L"TargetSrcPath", targetSrcPath, 0), L"setting.txt에 TargetSrcPath로 원본 경로가 지정되어 있지 않음")
			return false;

		MK_CHECK(!targetSrcPath.Empty(), L"setting.txt에 TargetSrcPath가 비어 있음")
			return false;

		m_TargetSrcFullPath.ConvertToRootBasisAbsolutePath(targetSrcPath);

		MK_CHECK(m_TargetSrcFullPath.IsDirectoryPath(), L"원본 경로(" + m_TargetSrcFullPath + L") 는 디렉토리 경로가 아님")
			return false;

		MK_CHECK(m_TargetSrcFullPath.CheckAvailable(), L"원본 경로(" + m_TargetSrcFullPath + L") 가 존재하지 않음")
			return false;

		// 예외 경로/파일명
		MkArray<MkStr> nameFilterList;
		if (settingNode.GetData(L"SkipNameFilter", nameFilterList) && (!nameFilterList.Empty()))
		{
			m_SkipNameFilter.Reserve(nameFilterList.GetSize());

			MK_INDEXING_LOOP(nameFilterList, i)
			{
				m_SkipNameFilter.PushBack(nameFilterList[i]);
			}
		}

		// 예외 파일 확장자
		MkArray<MkStr> extFilterList;
		if (settingNode.GetData(L"SkipExtFilter", extFilterList) && (!extFilterList.Empty()))
		{
			m_SkipExtFilter.Reserve(extFilterList.GetSize());

			MK_INDEXING_LOOP(extFilterList, i)
			{
				m_SkipExtFilter.PushBack(extFilterList[i]);
			}
		}

		// 정보 파일 경로
		MkPathName infoFilePath;
		MK_CHECK(settingNode.GetData(L"InfoFilePath", infoFilePath, 0), L"setting.txt에 InfoFilePath로 정보 파일 경로가 지정되어 있지 않음")
			return false;

		MK_CHECK(!infoFilePath.Empty(), L"setting.txt에 InfoFilePath가 비어 있음")
			return false;

		m_InfoFilePath.ConvertToRootBasisAbsolutePath(infoFilePath);

		// 출력 디렉토리 경로
		MkPathName exDirPath;
		MK_CHECK(settingNode.GetData(L"ExportDirPath", exDirPath, 0), L"setting.txt에 ExportDirPath로 출력 디렉토리 경로가 지정되어 있지 않음")
			return false;

		MK_CHECK(!exDirPath.Empty(), L"setting.txt에 ExportDirPath가 비어 있음")
			return false;

		m_ExportDirPath.ConvertToRootBasisAbsolutePath(exDirPath);

		MK_CHECK(m_ExportDirPath.IsDirectoryPath(), L"setting.txt에 ExportDirPath은 디렉토리 경로가 아님")
			return false;

		// 콘트롤 생성
		m_Font = CreateFont
			(12, 0, 0, 0, 0, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH | FF_ROMAN, L"굴림");

		HWND hWnd = MkBaseFramework::GetMainWindowPtr()->GetWindowHandle();
		HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE));
		const DWORD buttonControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON;

		m_ExportBtnHandle = _CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_EXPORT_ID, L"패치 출력 후 정보 갱신", buttonControlStyle, MkIntRect(10, 10, 160, 30));
		
		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		switch (m_AppState)
		{
		case eAS_Init:
			{
				m_AppState = eAS_LoadSrc;
				MK_DEV_PANEL.MsgToFreeboard(2, L"[ 원본 분석 ]");

				MK_DEV_PANEL.InsertEmptyLine();
				MK_DEV_PANEL.MsgToLog(L"> FileDiffFinder 시작");
			}
			break;

		case eAS_LoadSrc:
			{
				MkArray<MkStr> prefixFilter;
				MkArray<MkPathName> exceptionFilter;

				m_TargetSrcFullPath.GetBlackFileStructure(m_SrcInfoNode, m_SkipNameFilter, m_SkipExtFilter, prefixFilter, exceptionFilter, true, false);
				MK_DEV_PANEL.MsgToLog((m_SrcInfoNode.GetChildNodeCount() == 0) ? L"> 원본 파일 없음" : L"> 원본 파일 분석 완료");

				m_AppState = eAS_LoadInfo;
				MK_DEV_PANEL.MsgToFreeboard(2, L"[ 이전 정보 로딩 ]");
			}
			break;

		case eAS_LoadInfo:
			{
				// 정보 파일 존재
				if (m_InfoFilePath.CheckAvailable())
				{
					if (m_LastInfoNode.Load(m_InfoFilePath))
					{
						MkStr verStr;
						MK_CHECK(m_LastInfoNode.GetData(VersionTag, verStr, 0), VersionTag.GetString() + L" 정보가 없음. 파일 오염 의심")
						{
							m_AppState = eAS_Wait;
							MK_DEV_PANEL.MsgToFreeboard(2, L"[ 대기 중 ]");
							break;
						}

						MK_DEV_PANEL.MsgToFreeboard(0, L"Version : " + verStr);

						m_AppState = eAS_FindDiffs;
						MK_DEV_PANEL.MsgToFreeboard(2, L"[ 변경 파일 목록 추출 ]");
						MK_DEV_PANEL.MsgToLog(L"> 이전 정보 로딩 성공");
						MK_DEV_PANEL.MsgToLog(L"  " + m_InfoFilePath);
						MK_DEV_PANEL.MsgToLog(L"> 변경 파일 추출 시작");
					}
					else
					{
						MK_CHECK(false, m_InfoFilePath + L" 위치의 이전 정보 로딩 실패. 파일 오염 의심") {}

						MkBaseFramework::Close(); // quit application
					}
				}
				// 정보 파일 없음. 최초 생성
				else
				{
					m_LastInfoNode = m_SrcInfoNode; // 복사

					MkStr verStr; // version 정보
					_GetVersionString(verStr);
					m_LastInfoNode.CreateUnit(VersionTag, verStr);

					m_LastInfoNode.SaveToText(m_InfoFilePath); // 정보 파일 출력

					MK_DEV_PANEL.MsgToLog(L"> 이전 정보 존재하지 않음. 초기화 실행");
					MK_DEV_PANEL.MsgToLog(L"  " + m_InfoFilePath);

					m_AppState = eAS_Wait;
					MK_DEV_PANEL.MsgToFreeboard(0, L"Version : " + verStr);
					MK_DEV_PANEL.MsgToFreeboard(2, L"[ 대기 중 ]");
				}
			}
			break;

		case eAS_FindDiffs:
			{
				_FindDiffs(m_SrcInfoNode, m_LastInfoNode, m_DiffInfoNode);

				MkArray<MkPathName> fileList;
				_GetFileList(m_DiffInfoNode, L"", fileList);

				MK_DEV_PANEL.MsgToLog(L"> ----------------------------------------------------------------------");
				MK_DEV_PANEL.MsgToLog(L"> " + MkStr(fileList.GetSize()) + L" 개의 갱신 파일 확인");
				MK_DEV_PANEL.MsgToLog(L"> ----------------------------------------------------------------------");

				MK_INDEXING_LOOP(fileList, i)
				{
					MK_DEV_PANEL.MsgToLog(L" " + MkStr(i) + L"> " + fileList[i]);
				}
				if (!fileList.Empty())
				{
					MK_DEV_PANEL.MsgToLog(L"> ----------------------------------------------------------------------");
				}

				m_AppState = eAS_Wait;
				MK_DEV_PANEL.MsgToFreeboard(2, L"[ 대기 중 ]");
				
				if (!fileList.Empty())
				{
					_EnableWindow(true);
				}
			}
			break;

		case eAS_Wait:
			break;

		case eAS_ExportDiff:
			{
				do
				{
					MK_CHECK(m_ExportDirPath.MakeDirectoryPath(), m_ExportDirPath + L" 경로가 존재하지 않음")
						break;

					MkStr verStr;
					_GetVersionString(verStr);
					MkPathName targetBasePath = m_ExportDirPath + verStr;

					// diff 설정 파일 출력
					MkPathName targetInfoFilePath = targetBasePath;
					targetInfoFilePath += L".txt";
					m_DiffInfoNode.SaveToText(targetInfoFilePath);
					MK_DEV_PANEL.MsgToLog(L"> 패치 정보 파일 출력");
					MK_DEV_PANEL.MsgToLog(L"  " + targetInfoFilePath);

					// 출력 파일이 저장 될 경로 생성
					MkPathName targetDirPath = targetBasePath;
					targetDirPath.CheckAndAddBackslash();

					// 패치 파일 복사
					_CopyFiles(m_DiffInfoNode, m_TargetSrcFullPath, targetDirPath);
					MK_DEV_PANEL.MsgToLog(L"> 패치 출력 완료");
					MK_DEV_PANEL.MsgToLog(L"  " + targetDirPath);

					// 이전 정보 갱신
					m_LastInfoNode.Clear();
					m_LastInfoNode = m_SrcInfoNode; // 복사
					m_LastInfoNode.CreateUnit(VersionTag, verStr);

					// 정보 파일 출력
					// 이게 끝나면 되돌릴 수 없음. 이미 원본 파일이 변경 된 이후이므로 설정 백업본을 남긴다는 행위는 의미가 없음
					m_LastInfoNode.SaveToText(m_InfoFilePath);
					m_DiffInfoNode.Clear();

					MK_DEV_PANEL.MsgToFreeboard(0, L"Version : " + verStr);
				}
				while (false);

				m_AppState = eAS_Wait;
				MK_DEV_PANEL.MsgToFreeboard(2, L"[ 대기 중 ]");
			}
			break;
		}

		// 버튼 이벤트
		MK_INDEXING_LOOP(gFrameworkEvent, i)
		{
			switch (gFrameworkEvent[i])
			{
			case MKDEF_APP_BTN_EXPORT_ID:
				{
					_EnableWindow(false);
					m_AppState = eAS_ExportDiff;
					MK_DEV_PANEL.MsgToFreeboard(2, L"[ 패치 대상 출력 ]");
				}
				break;
			}
		}
		gFrameworkEvent.Clear();
	}

	TestPage(const MkHashStr& name) : MkBasePage(name)
	{
		m_Font = NULL;
		m_ExportBtnHandle = NULL;
		m_AppState = eAS_Init;
	}

	virtual ~TestPage()
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
			EnableWindow(hControl, FALSE);
		}

		return hControl;
	}

	void _EnableWindow(bool enable)
	{
		if (m_ExportBtnHandle != NULL)
		{
			EnableWindow(m_ExportBtnHandle, (enable) ? TRUE : FALSE);
		}
	}

	void _GetVersionString(MkStr& buffer)
	{
		buffer.Clear();

		MkStr dataStr = MK_SYS_ENV.GetCurrentSystemDate(); // L"연.월.일" -> L"연_월_일"
		dataStr.ReplaceKeyword(L".", L"_");
		MkStr timeStr = MK_SYS_ENV.GetCurrentSystemTime(); // L"시:분:초" -> L"시_분_초"
		timeStr.ReplaceKeyword(L":", L"_");

		buffer.Reserve(dataStr.GetSize() + 1 + timeStr.GetSize());
		buffer += dataStr;
		buffer += L"_";
		buffer += timeStr; // L"연_월_일_시_분_초"
	}

	void _GetFilesAndDirs(const MkDataNode& node, MkArray<MkHashStr>& files, MkArray<MkHashStr>& dirs) const
	{
		MkArray<MkHashStr> keys;
		node.GetChildNodeList(keys);
		files.Reserve(keys.GetSize());
		dirs.Reserve(keys.GetSize());

		MK_INDEXING_LOOP(keys, i)
		{
			const MkDataNode* childNode = node.GetChildNode(keys[i]);
			if (childNode != NULL)
			{
				if (childNode->IsValidKey(MkHashStr::Path)) // Path key는 디렉토리에만 존재
				{
					dirs.PushBack(keys[i]);
				}
				else
				{
					files.PushBack(keys[i]);
				}
			}
		}
	}

	void _CopyChildren(const MkDataNode& from, MkDataNode& to, const MkArray<MkHashStr>& keys) const
	{
		MK_INDEXING_LOOP(keys, i)
		{
			const MkDataNode* childNode = from.GetChildNode(keys[i]);
			if (childNode != NULL)
			{
				MkDataNode* newNode = to.CreateChildNode(keys[i]);
				if (newNode != NULL)
				{
					*newNode = *childNode; // deep copy
				}
			}
		}
	}

	void _FindDiffs(const MkDataNode& srcInfo, const MkDataNode& lastInfo, MkDataNode& diffInfo)
	{
		// 경로 복사
		MkStr path;
		if (srcInfo.GetData(MkHashStr::Path, path, 0))
		{
			diffInfo.CreateUnit(MkHashStr::Path, path);
		}

		// src의 파일, 디렉토리
		MkArray<MkHashStr> srcFileKeys, srcDirKeys;
		_GetFilesAndDirs(srcInfo, srcFileKeys, srcDirKeys);

		// last의 파일, 디렉토리
		MkArray<MkHashStr> lastFileKeys, lastDirKeys;
		_GetFilesAndDirs(lastInfo, lastFileKeys, lastDirKeys);

		// 파일에 대한 차집합, 교집합 검사
		MkArray<MkHashStr> newFKeys, sameFKeys, removedFKeys;
		srcFileKeys.IntersectionTest(lastFileKeys, newFKeys, sameFKeys, removedFKeys);

		// 디렉토리에 대한 차집합, 교집합 검사
		MkArray<MkHashStr> newDKeys, sameDKeys, removedDKeys;
		srcDirKeys.IntersectionTest(lastDirKeys, newDKeys, sameDKeys, removedDKeys);

		// 새로 등장한 파일, 디렉토리는 모두 diff에 추가
		_CopyChildren(srcInfo, diffInfo, newFKeys);
		unsigned int files = newFKeys.GetSize();

		_CopyChildren(srcInfo, diffInfo, newDKeys);
		unsigned int directories = newDKeys.GetSize();

		// 같은 이름의 파일은 size, last written time을 비교해 다르면 diff에 추가
		MkArray<MkHashStr> diffFKeys(sameFKeys.GetSize());

		MK_INDEXING_LOOP(sameFKeys, i)
		{
			const MkHashStr& name = sameFKeys[i];
			const MkDataNode* srcNode = srcInfo.GetChildNode(name);
			const MkDataNode* lastNode = lastInfo.GetChildNode(name);
			if ((srcNode != NULL) && (lastNode != NULL))
			{
				MkArray<unsigned int> srcSize, srcLWT;
				MK_CHECK(srcNode->GetData(MkHashStr::FSize, srcSize), MkHashStr::FSize.GetString() + L" 에러") continue;
				MK_CHECK(srcNode->GetData(MkHashStr::LWT, srcLWT), MkHashStr::LWT.GetString() + L" 에러") continue;

				MkArray<unsigned int> lastSize, lastLWT;
				MK_CHECK(lastNode->GetData(MkHashStr::FSize, lastSize), MkHashStr::FSize.GetString() + L" 에러") continue;
				MK_CHECK(lastNode->GetData(MkHashStr::LWT, lastLWT), MkHashStr::LWT.GetString() + L" 에러") continue;

				if ((srcSize != lastSize) || (srcLWT != lastLWT))
				{
					diffFKeys.PushBack(name);
				}
			}
		}

		_CopyChildren(srcInfo, diffInfo, diffFKeys);
		files += diffFKeys.GetSize();

		// 같은 이름의 디렉토리를 대상으로 재귀 탐색
		MK_INDEXING_LOOP(sameDKeys, i)
		{
			const MkHashStr& name = sameDKeys[i];
			const MkDataNode* srcNode = srcInfo.GetChildNode(name);
			const MkDataNode* lastNode = lastInfo.GetChildNode(name);
			if ((srcNode != NULL) && (lastNode != NULL))
			{
				MkDataNode* newDiffNode = diffInfo.CreateChildNode(name);
				if (newDiffNode != NULL)
				{
					_FindDiffs(*srcNode, *lastNode, *newDiffNode); // 재귀 탐색

					if (newDiffNode->GetChildNodeCount() == 0) // 하위 파일, 유효 디렉토리가 없는 디렉토리는 삭제
					{
						diffInfo.RemoveChildNode(name);
					}
					else
					{
						++directories;
					}
				}
			}
		}

		diffInfo.CreateUnit(MkHashStr::Files, files);
		diffInfo.CreateUnit(MkHashStr::Dirs, directories);
	}

	void _GetFileList(const MkDataNode& node, const MkPathName& pathFromRoot, MkArray<MkPathName>& fileList) const
	{
		MkArray<MkHashStr> files, dirs;
		_GetFilesAndDirs(node, files, dirs);

		// 파일
		if (!files.Empty())
		{
			files.SortInAscendingOrder(); // 정렬
			MK_INDEXING_LOOP(files, i)
			{
				fileList.PushBack(pathFromRoot + files[i].GetString());
			}
		}

		// 디렉토리
		if (!dirs.Empty())
		{
			dirs.SortInAscendingOrder(); // 정렬
			MK_INDEXING_LOOP(dirs, i)
			{
				const MkHashStr& name = dirs[i];
				const MkDataNode* childNode = node.GetChildNode(name);
				if (childNode != NULL)
				{
					MkPathName newPath = pathFromRoot;
					newPath += name.GetString();
					newPath += L"\\";
					_GetFileList(*childNode, newPath, fileList);
				}
			}
		}
	}

	void _CopyFiles(const MkDataNode& node, const MkPathName& srcBasePath, const MkPathName& destBasePath)
	{
		MkPathName localPath;
		if (node.GetData(MkHashStr::Path, localPath, 0))
		{
			// 원본 경로
			MkPathName srcPath = srcBasePath;
			srcPath += localPath;
			srcPath.CheckAndAddBackslash();

			// 대상 경로
			MkPathName destPath = destBasePath;
			destPath += localPath;
			destPath.CheckAndAddBackslash();
			MK_CHECK(destPath.MakeDirectoryPath(), destPath + L" 경로 생성 실패")
				return;

			// 파일, 디렉토리 키 얻음
			MkArray<MkHashStr> files, dirs;
			_GetFilesAndDirs(node, files, dirs);

			// 파일
			MK_INDEXING_LOOP(files, i)
			{
				MkPathName targetSrcPath = srcPath;
				targetSrcPath += files[i].GetString();

				MkPathName targetDestPath = destPath;
				targetDestPath += files[i].GetString();

				MK_CHECK(::CopyFile(targetSrcPath.GetPtr(), targetDestPath.GetPtr(), FALSE) != 0, targetSrcPath + L" -> " + targetDestPath + L" 복사 실패")
					continue;
			}

			// 디렉토리
			MK_INDEXING_LOOP(dirs, i)
			{
				const MkDataNode* childNode = node.GetChildNode(dirs[i]);
				if (childNode != NULL)
				{
					_CopyFiles(*childNode, srcBasePath, destBasePath);
				}
			}
		}
	}

protected:

	// 콘트롤
	HFONT m_Font;
	HWND m_ExportBtnHandle;

	// 설정 정보
	MkPathName m_TargetSrcFullPath; // 원본 경로
	MkPathName m_InfoFilePath; // 정보 파일 경로
	MkPathName m_ExportDirPath; // 출력 디렉토리 경로
	MkArray<MkPathName> m_SkipNameFilter; // 예외 파일/경로명
	MkArray<MkPathName> m_SkipExtFilter; // 예외 확장자

	// 진행 상태
	eAppState m_AppState;

	MkDataNode m_SrcInfoNode; // 원본 정보
	MkDataNode m_LastInfoNode; // 이전 정보
	MkDataNode m_DiffInfoNode; // 갱신 정보

};

// TestFramework 선언
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg)
	{
		MK_PAGE_MGR.SetUp(new TestPage(L"Root"));
		MK_PAGE_MGR.ChangePageDirectly(L"Root");
		
		return MkBaseFramework::SetUp(clientWidth, clientHeight, fullScreen, arg);
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
					case MKDEF_APP_BTN_EXPORT_ID:
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

	virtual ~TestFramework() {}
};

// TestApplication 선언
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
	application.Run(hI, L"FillDiffLauncher", L"..\\FileRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 180, 50, false, false, TestFramework::NewWndProc, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

