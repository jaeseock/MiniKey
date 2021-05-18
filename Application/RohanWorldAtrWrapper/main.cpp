
//------------------------------------------------------------------------------------------------//

//#include <windows.h>

//#include "minizip/unzip.h"

//#include "MkCore_MkCheck.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkSystemEnvironment.h"
#include "MkCore_MkZipCompressor.h"
#include "MkCore_MkXORer.h"
//#include "MkCore_MkInputManager.h"
//#include "MkCore_MkFileManager.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkInterfaceForFileReading.h"
#include "MkCore_MkInterfaceForFileWriting.h"
//#include "MkCore_MkInterfaceForZipFileReading.h"

//#include "MkCore_MkFileManager.h"


class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		if (m_TargetDirPath.GetDirectoryPathFromDialog(L"하위에 atr 파일이 존재하는 폴더 지정", m_MainWindow.GetWindowHandle()))
		{
			m_TargetFileList.Reserve(128);

			MkDataNode node;
			m_TargetDirPath.ExportSystemStructure(node);

			_FindAtrFiles(L"", node);
			if (m_TargetFileList.Empty())
			{
				::MessageBox(m_MainWindow.GetWindowHandle(), L"하위에 art 파일이 없음", L"으잉?", MB_OK);
				return false;
			}

			MK_DEV_PANEL.MsgToLog(L"[ atr 파일 루트 ]");
			MK_DEV_PANEL.MsgToLog(L"  - " + m_TargetDirPath);
			MK_DEV_PANEL.InsertEmptyLine();
			
			MK_DEV_PANEL.MsgToLog(L"[ 검출 : " + MkStr(m_TargetFileList.GetSize()) + L" 개 ]");
			MK_INDEXING_LOOP(m_TargetFileList, i)
			{
				MK_DEV_PANEL.MsgToLog(L"  - " + m_TargetFileList[i]);
			}

			// 출력 폴더
			MkStr dateStr = MK_SYS_ENV.GetCurrentSystemDate();
			dateStr.PopFront(2); // L"20" 제거
			dateStr.RemoveKeyword(L".");
			MkStr timeStr = MK_SYS_ENV.GetCurrentSystemTime();
			timeStr.RemoveKeyword(L":");

			MkStr exportDirName = dateStr + L"_" + timeStr;
			m_ExportDirPath.ConvertToRootBasisAbsolutePath(exportDirName);

			m_CurrIndex = 0;
			m_SuccessCount = 0;
			m_State = 0;
		}

		if (m_TargetFileList.Empty())
			return false;

		return true;
	}

	virtual void StartLooping(void)
	{
		
	}

	virtual void Update(void)
	{
		if (m_TargetFileList.Empty() || (m_State < 0))
			return;

		if (m_CurrIndex == m_TargetFileList.GetSize())
		{
			if (m_State == 0)
			{
				if (m_CurrIndex == m_SuccessCount)
				{
					::MessageBox(m_MainWindow.GetWindowHandle(), L"파일 생성 모두 성공. 무결성 체크 시작.", L"굿잡", MB_OK);

					m_CurrIndex = 0;
					m_SuccessCount = 0;
					m_State = 1;
				}
				else
				{
					::MessageBox(m_MainWindow.GetWindowHandle(), L"생성 실패 파일 존재. 로그 참조.", L"으헝", MB_OK);

					m_TargetFileList.Clear();
					return;
				}
			}
			else if (m_State == 1)
			{
				if (m_CurrIndex == m_SuccessCount)
				{
					::MessageBox(m_MainWindow.GetWindowHandle(), L"무결성 체크 모두 성공!!!", L"굿잡", MB_OK);
				}
				else
				{
					::MessageBox(m_MainWindow.GetWindowHandle(), L"무결성 검사 실패 파일 존재. 로그 참조", L"으헝", MB_OK);
				}

				m_TargetFileList.Clear();
				return;
			}
		}

		//
		do
		{
			if (m_State == 0)
			{
				// 읽고
				MkByteArray destBuffer;
				MkInterfaceForFileReading frInterface;

				if (!frInterface.SetUp(m_TargetDirPath + m_TargetFileList[m_CurrIndex]))
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" 파일 열기 실패");
					break;
				}

				if (frInterface.Read(destBuffer, MkArraySection(0)) == MKDEF_ARRAY_ERROR)
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" 파일 읽기 실패");
					break;
				}
				frInterface.Clear();

				// 압축
				MkByteArray compBuffer;
				if (MkZipCompressor::Compress(destBuffer.GetPtr(), destBuffer.GetSize(), compBuffer) == 0)
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" 파일 압축 실패");
					break;
				}
				destBuffer.Clear();

				// XOR
				MkXORer::Convert(compBuffer, L"로한 좀 그만 건드려 이눔들아!!!");

				// 저장
				MkInterfaceForFileWriting fwInterface;
				if (!fwInterface.SetUp(m_ExportDirPath + m_TargetFileList[m_CurrIndex], true, true))
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" 파일 생성 실패");
					break;
				}

				if (fwInterface.Write(compBuffer, MkArraySection(0)) != compBuffer.GetSize())
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" 파일 기록 실패");
					break;
				}
				fwInterface.Clear();
			}
			else if (m_State == 1)
			{
				// 읽고
				MkByteArray destBuffer;
				MkInterfaceForFileReading frInterface;

				if (!frInterface.SetUp(m_ExportDirPath + m_TargetFileList[m_CurrIndex]))
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" 파일 열기 실패");
					break;
				}

				if (frInterface.Read(destBuffer, MkArraySection(0)) == MKDEF_ARRAY_ERROR)
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" 파일 읽기 실패");
					break;
				}
				frInterface.Clear();

				// XOR
				MkXORer::Convert(destBuffer, L"로한 좀 그만 건드려 이눔들아!!!");

				// 압축 해제
				MkByteArray uncompBuffer;
				if (MkZipCompressor::Uncompress(destBuffer.GetPtr(), destBuffer.GetSize(), uncompBuffer) == 0)
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" 파일 압축 실패");
					break;
				}
				destBuffer.Clear();

				// 원본 읽음
				if (!frInterface.SetUp(m_TargetDirPath + m_TargetFileList[m_CurrIndex]))
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" 파일 열기 실패");
					break;
				}

				if (frInterface.Read(destBuffer, MkArraySection(0)) == MKDEF_ARRAY_ERROR)
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" 파일 읽기 실패");
					break;
				}
				frInterface.Clear();

				// 비교
				if (destBuffer != uncompBuffer)
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" 파일이 동일하지 않음");
					break;
				}
			}
			
			++m_SuccessCount;
		}
		while (false);

		++m_CurrIndex;
	}

	virtual void Clear(void)
	{
	}

	TestFramework() : MkBaseFramework()
	{
		m_CurrIndex = 0;
		m_SuccessCount = 0;
		m_State = -1;
	}

	virtual ~TestFramework()
	{
	}

protected:

	void _FindAtrFiles(const MkPathName& parentDir, const MkDataNode& node)
	{
		// 현재 폴더 경로
		MkPathName thisDir = parentDir + node.GetNodeName().GetString();
		thisDir.CheckAndAddBackslash();

		// atr 파일 검색
		MkArray<MkHashStr> subFiles;
		MkPathName::__GetSubFiles(node, subFiles);
		MK_INDEXING_LOOP(subFiles, i)
		{
			MkPathName filePath = subFiles[i].GetString();
			MkStr ext = filePath.GetFileExtension();
			ext.ToLower();
			if (ext.Equals(0, L"atr"))
			{
				m_TargetFileList.PushBack(thisDir + filePath);
			}
		}

		// 하위 폴더 검색
		MkArray<MkHashStr> subDirs;
		MkPathName::__GetSubDirectories(node, subDirs);
		MK_INDEXING_LOOP(subDirs, i)
		{
			const MkStr& dirName = subDirs[i];
			if ((dirName.GetSize() > 2) && (dirName.GetAt(1) == L'-'))
			{
				_FindAtrFiles(thisDir, *node.GetChildNode(subDirs[i]));
			}
		}
	}

protected:

	MkPathName m_TargetDirPath;
	MkArray<MkPathName> m_TargetFileList;

	MkPathName m_ExportDirPath;

	unsigned int m_CurrIndex;
	unsigned int m_SuccessCount;

	int m_State;
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
	application.Run(hI, L"RohanWorldAtrWrapper", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 200, 200);

	return 0;
}

//------------------------------------------------------------------------------------------------//

