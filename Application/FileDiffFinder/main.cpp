
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

// TestPage ����
class TestPage : public MkBasePage
{
protected:

	enum eAppState
	{
		eAS_Init = 0, // ����
		eAS_LoadSrc, // ���� ���� �м�
		eAS_LoadInfo, // ���� ���� ����. ���� ������ ������ �������� �ʱ�ȭ
		eAS_FindDiffs, // ��ġ ��� ����
		eAS_Wait, // ���
		eAS_ExportDiff // ��ġ ��� ��� �� ���� ������ ���� ������ ����
	};

public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		// �������Ϸκ��� �ʱ�ȭ
		MkDataNode settingNode;
		MK_CHECK(settingNode.Load(L"setting.txt"), L"setting.txt ������ ����")
			return false;

		// ���� ���
		MkPathName targetSrcPath;
		MK_CHECK(settingNode.GetData(L"TargetSrcPath", targetSrcPath, 0), L"setting.txt�� TargetSrcPath�� ���� ��ΰ� �����Ǿ� ���� ����")
			return false;

		MK_CHECK(!targetSrcPath.Empty(), L"setting.txt�� TargetSrcPath�� ��� ����")
			return false;

		m_TargetSrcFullPath.ConvertToRootBasisAbsolutePath(targetSrcPath);

		MK_CHECK(m_TargetSrcFullPath.IsDirectoryPath(), L"���� ���(" + m_TargetSrcFullPath + L") �� ���丮 ��ΰ� �ƴ�")
			return false;

		MK_CHECK(m_TargetSrcFullPath.CheckAvailable(), L"���� ���(" + m_TargetSrcFullPath + L") �� �������� ����")
			return false;

		// ���� ���/���ϸ�
		MkArray<MkStr> nameFilterList;
		if (settingNode.GetData(L"SkipNameFilter", nameFilterList) && (!nameFilterList.Empty()))
		{
			m_SkipNameFilter.Reserve(nameFilterList.GetSize());

			MK_INDEXING_LOOP(nameFilterList, i)
			{
				m_SkipNameFilter.PushBack(nameFilterList[i]);
			}
		}

		// ���� ���� Ȯ����
		MkArray<MkStr> extFilterList;
		if (settingNode.GetData(L"SkipExtFilter", extFilterList) && (!extFilterList.Empty()))
		{
			m_SkipExtFilter.Reserve(extFilterList.GetSize());

			MK_INDEXING_LOOP(extFilterList, i)
			{
				m_SkipExtFilter.PushBack(extFilterList[i]);
			}
		}

		// ���� ���� ���
		MkPathName infoFilePath;
		MK_CHECK(settingNode.GetData(L"InfoFilePath", infoFilePath, 0), L"setting.txt�� InfoFilePath�� ���� ���� ��ΰ� �����Ǿ� ���� ����")
			return false;

		MK_CHECK(!infoFilePath.Empty(), L"setting.txt�� InfoFilePath�� ��� ����")
			return false;

		m_InfoFilePath.ConvertToRootBasisAbsolutePath(infoFilePath);

		// ��� ���丮 ���
		MkPathName exDirPath;
		MK_CHECK(settingNode.GetData(L"ExportDirPath", exDirPath, 0), L"setting.txt�� ExportDirPath�� ��� ���丮 ��ΰ� �����Ǿ� ���� ����")
			return false;

		MK_CHECK(!exDirPath.Empty(), L"setting.txt�� ExportDirPath�� ��� ����")
			return false;

		m_ExportDirPath.ConvertToRootBasisAbsolutePath(exDirPath);

		MK_CHECK(m_ExportDirPath.IsDirectoryPath(), L"setting.txt�� ExportDirPath�� ���丮 ��ΰ� �ƴ�")
			return false;

		// ��Ʈ�� ����
		m_Font = CreateFont
			(12, 0, 0, 0, 0, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH | FF_ROMAN, L"����");

		HWND hWnd = MkBaseFramework::GetMainWindowPtr()->GetWindowHandle();
		HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hWnd, GWL_HINSTANCE));
		const DWORD buttonControlStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON;

		m_ExportBtnHandle = _CreateControl(hWnd, hInstance, L"button", MKDEF_APP_BTN_EXPORT_ID, L"��ġ ��� �� ���� ����", buttonControlStyle, MkIntRect(10, 10, 160, 30));
		
		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		switch (m_AppState)
		{
		case eAS_Init:
			{
				m_AppState = eAS_LoadSrc;
				MK_DEV_PANEL.MsgToFreeboard(2, L"[ ���� �м� ]");

				MK_DEV_PANEL.InsertEmptyLine();
				MK_DEV_PANEL.MsgToLog(L"> FileDiffFinder ����");
			}
			break;

		case eAS_LoadSrc:
			{
				MkArray<MkStr> prefixFilter;
				MkArray<MkPathName> exceptionFilter;

				m_TargetSrcFullPath.GetBlackFileStructure(m_SrcInfoNode, m_SkipNameFilter, m_SkipExtFilter, prefixFilter, exceptionFilter, true, false);
				MK_DEV_PANEL.MsgToLog((m_SrcInfoNode.GetChildNodeCount() == 0) ? L"> ���� ���� ����" : L"> ���� ���� �м� �Ϸ�");

				m_AppState = eAS_LoadInfo;
				MK_DEV_PANEL.MsgToFreeboard(2, L"[ ���� ���� �ε� ]");
			}
			break;

		case eAS_LoadInfo:
			{
				// ���� ���� ����
				if (m_InfoFilePath.CheckAvailable())
				{
					if (m_LastInfoNode.Load(m_InfoFilePath))
					{
						MkStr verStr;
						MK_CHECK(m_LastInfoNode.GetData(VersionTag, verStr, 0), VersionTag.GetString() + L" ������ ����. ���� ���� �ǽ�")
						{
							m_AppState = eAS_Wait;
							MK_DEV_PANEL.MsgToFreeboard(2, L"[ ��� �� ]");
							break;
						}

						MK_DEV_PANEL.MsgToFreeboard(0, L"Version : " + verStr);

						m_AppState = eAS_FindDiffs;
						MK_DEV_PANEL.MsgToFreeboard(2, L"[ ���� ���� ��� ���� ]");
						MK_DEV_PANEL.MsgToLog(L"> ���� ���� �ε� ����");
						MK_DEV_PANEL.MsgToLog(L"  " + m_InfoFilePath);
						MK_DEV_PANEL.MsgToLog(L"> ���� ���� ���� ����");
					}
					else
					{
						MK_CHECK(false, m_InfoFilePath + L" ��ġ�� ���� ���� �ε� ����. ���� ���� �ǽ�") {}

						MkBaseFramework::Close(); // quit application
					}
				}
				// ���� ���� ����. ���� ����
				else
				{
					m_LastInfoNode = m_SrcInfoNode; // ����

					MkStr verStr; // version ����
					_GetVersionString(verStr);
					m_LastInfoNode.CreateUnit(VersionTag, verStr);

					m_LastInfoNode.SaveToText(m_InfoFilePath); // ���� ���� ���

					MK_DEV_PANEL.MsgToLog(L"> ���� ���� �������� ����. �ʱ�ȭ ����");
					MK_DEV_PANEL.MsgToLog(L"  " + m_InfoFilePath);

					m_AppState = eAS_Wait;
					MK_DEV_PANEL.MsgToFreeboard(0, L"Version : " + verStr);
					MK_DEV_PANEL.MsgToFreeboard(2, L"[ ��� �� ]");
				}
			}
			break;

		case eAS_FindDiffs:
			{
				_FindDiffs(m_SrcInfoNode, m_LastInfoNode, m_DiffInfoNode);

				MkArray<MkPathName> fileList;
				_GetFileList(m_DiffInfoNode, L"", fileList);

				MK_DEV_PANEL.MsgToLog(L"> ----------------------------------------------------------------------");
				MK_DEV_PANEL.MsgToLog(L"> " + MkStr(fileList.GetSize()) + L" ���� ���� ���� Ȯ��");
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
				MK_DEV_PANEL.MsgToFreeboard(2, L"[ ��� �� ]");
				
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
					MK_CHECK(m_ExportDirPath.MakeDirectoryPath(), m_ExportDirPath + L" ��ΰ� �������� ����")
						break;

					MkStr verStr;
					_GetVersionString(verStr);
					MkPathName targetBasePath = m_ExportDirPath + verStr;

					// diff ���� ���� ���
					MkPathName targetInfoFilePath = targetBasePath;
					targetInfoFilePath += L".txt";
					m_DiffInfoNode.SaveToText(targetInfoFilePath);
					MK_DEV_PANEL.MsgToLog(L"> ��ġ ���� ���� ���");
					MK_DEV_PANEL.MsgToLog(L"  " + targetInfoFilePath);

					// ��� ������ ���� �� ��� ����
					MkPathName targetDirPath = targetBasePath;
					targetDirPath.CheckAndAddBackslash();

					// ��ġ ���� ����
					_CopyFiles(m_DiffInfoNode, m_TargetSrcFullPath, targetDirPath);
					MK_DEV_PANEL.MsgToLog(L"> ��ġ ��� �Ϸ�");
					MK_DEV_PANEL.MsgToLog(L"  " + targetDirPath);

					// ���� ���� ����
					m_LastInfoNode.Clear();
					m_LastInfoNode = m_SrcInfoNode; // ����
					m_LastInfoNode.CreateUnit(VersionTag, verStr);

					// ���� ���� ���
					// �̰� ������ �ǵ��� �� ����. �̹� ���� ������ ���� �� �����̹Ƿ� ���� ������� ����ٴ� ������ �ǹ̰� ����
					m_LastInfoNode.SaveToText(m_InfoFilePath);
					m_DiffInfoNode.Clear();

					MK_DEV_PANEL.MsgToFreeboard(0, L"Version : " + verStr);
				}
				while (false);

				m_AppState = eAS_Wait;
				MK_DEV_PANEL.MsgToFreeboard(2, L"[ ��� �� ]");
			}
			break;
		}

		// ��ư �̺�Ʈ
		MK_INDEXING_LOOP(gFrameworkEvent, i)
		{
			switch (gFrameworkEvent[i])
			{
			case MKDEF_APP_BTN_EXPORT_ID:
				{
					_EnableWindow(false);
					m_AppState = eAS_ExportDiff;
					MK_DEV_PANEL.MsgToFreeboard(2, L"[ ��ġ ��� ��� ]");
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

		MkStr dataStr = MK_SYS_ENV.GetCurrentSystemDate(); // L"��.��.��" -> L"��_��_��"
		dataStr.ReplaceKeyword(L".", L"_");
		MkStr timeStr = MK_SYS_ENV.GetCurrentSystemTime(); // L"��:��:��" -> L"��_��_��"
		timeStr.ReplaceKeyword(L":", L"_");

		buffer.Reserve(dataStr.GetSize() + 1 + timeStr.GetSize());
		buffer += dataStr;
		buffer += L"_";
		buffer += timeStr; // L"��_��_��_��_��_��"
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
				if (childNode->IsValidKey(MkHashStr::Path)) // Path key�� ���丮���� ����
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
		// ��� ����
		MkStr path;
		if (srcInfo.GetData(MkHashStr::Path, path, 0))
		{
			diffInfo.CreateUnit(MkHashStr::Path, path);
		}

		// src�� ����, ���丮
		MkArray<MkHashStr> srcFileKeys, srcDirKeys;
		_GetFilesAndDirs(srcInfo, srcFileKeys, srcDirKeys);

		// last�� ����, ���丮
		MkArray<MkHashStr> lastFileKeys, lastDirKeys;
		_GetFilesAndDirs(lastInfo, lastFileKeys, lastDirKeys);

		// ���Ͽ� ���� ������, ������ �˻�
		MkArray<MkHashStr> newFKeys, sameFKeys, removedFKeys;
		srcFileKeys.IntersectionTest(lastFileKeys, newFKeys, sameFKeys, removedFKeys);

		// ���丮�� ���� ������, ������ �˻�
		MkArray<MkHashStr> newDKeys, sameDKeys, removedDKeys;
		srcDirKeys.IntersectionTest(lastDirKeys, newDKeys, sameDKeys, removedDKeys);

		// ���� ������ ����, ���丮�� ��� diff�� �߰�
		_CopyChildren(srcInfo, diffInfo, newFKeys);
		unsigned int files = newFKeys.GetSize();

		_CopyChildren(srcInfo, diffInfo, newDKeys);
		unsigned int directories = newDKeys.GetSize();

		// ���� �̸��� ������ size, last written time�� ���� �ٸ��� diff�� �߰�
		MkArray<MkHashStr> diffFKeys(sameFKeys.GetSize());

		MK_INDEXING_LOOP(sameFKeys, i)
		{
			const MkHashStr& name = sameFKeys[i];
			const MkDataNode* srcNode = srcInfo.GetChildNode(name);
			const MkDataNode* lastNode = lastInfo.GetChildNode(name);
			if ((srcNode != NULL) && (lastNode != NULL))
			{
				MkArray<unsigned int> srcSize, srcLWT;
				MK_CHECK(srcNode->GetData(MkHashStr::FSize, srcSize), MkHashStr::FSize.GetString() + L" ����") continue;
				MK_CHECK(srcNode->GetData(MkHashStr::LWT, srcLWT), MkHashStr::LWT.GetString() + L" ����") continue;

				MkArray<unsigned int> lastSize, lastLWT;
				MK_CHECK(lastNode->GetData(MkHashStr::FSize, lastSize), MkHashStr::FSize.GetString() + L" ����") continue;
				MK_CHECK(lastNode->GetData(MkHashStr::LWT, lastLWT), MkHashStr::LWT.GetString() + L" ����") continue;

				if ((srcSize != lastSize) || (srcLWT != lastLWT))
				{
					diffFKeys.PushBack(name);
				}
			}
		}

		_CopyChildren(srcInfo, diffInfo, diffFKeys);
		files += diffFKeys.GetSize();

		// ���� �̸��� ���丮�� ������� ��� Ž��
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
					_FindDiffs(*srcNode, *lastNode, *newDiffNode); // ��� Ž��

					if (newDiffNode->GetChildNodeCount() == 0) // ���� ����, ��ȿ ���丮�� ���� ���丮�� ����
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

		// ����
		if (!files.Empty())
		{
			files.SortInAscendingOrder(); // ����
			MK_INDEXING_LOOP(files, i)
			{
				fileList.PushBack(pathFromRoot + files[i].GetString());
			}
		}

		// ���丮
		if (!dirs.Empty())
		{
			dirs.SortInAscendingOrder(); // ����
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
			// ���� ���
			MkPathName srcPath = srcBasePath;
			srcPath += localPath;
			srcPath.CheckAndAddBackslash();

			// ��� ���
			MkPathName destPath = destBasePath;
			destPath += localPath;
			destPath.CheckAndAddBackslash();
			MK_CHECK(destPath.MakeDirectoryPath(), destPath + L" ��� ���� ����")
				return;

			// ����, ���丮 Ű ����
			MkArray<MkHashStr> files, dirs;
			_GetFilesAndDirs(node, files, dirs);

			// ����
			MK_INDEXING_LOOP(files, i)
			{
				MkPathName targetSrcPath = srcPath;
				targetSrcPath += files[i].GetString();

				MkPathName targetDestPath = destPath;
				targetDestPath += files[i].GetString();

				MK_CHECK(::CopyFile(targetSrcPath.GetPtr(), targetDestPath.GetPtr(), FALSE) != 0, targetSrcPath + L" -> " + targetDestPath + L" ���� ����")
					continue;
			}

			// ���丮
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

	// ��Ʈ��
	HFONT m_Font;
	HWND m_ExportBtnHandle;

	// ���� ����
	MkPathName m_TargetSrcFullPath; // ���� ���
	MkPathName m_InfoFilePath; // ���� ���� ���
	MkPathName m_ExportDirPath; // ��� ���丮 ���
	MkArray<MkPathName> m_SkipNameFilter; // ���� ����/��θ�
	MkArray<MkPathName> m_SkipExtFilter; // ���� Ȯ����

	// ���� ����
	eAppState m_AppState;

	MkDataNode m_SrcInfoNode; // ���� ����
	MkDataNode m_LastInfoNode; // ���� ����
	MkDataNode m_DiffInfoNode; // ���� ����

};

// TestFramework ����
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

// TestApplication ����
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
	application.Run(hI, L"FillDiffLauncher", L"..\\FileRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 180, 50, false, false, TestFramework::NewWndProc, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

