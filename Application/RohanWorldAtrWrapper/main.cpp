
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
		if (m_TargetDirPath.GetDirectoryPathFromDialog(L"������ atr ������ �����ϴ� ���� ����", m_MainWindow.GetWindowHandle()))
		{
			m_TargetFileList.Reserve(128);

			MkDataNode node;
			m_TargetDirPath.ExportSystemStructure(node);

			_FindAtrFiles(L"", node);
			if (m_TargetFileList.Empty())
			{
				::MessageBox(m_MainWindow.GetWindowHandle(), L"������ art ������ ����", L"����?", MB_OK);
				return false;
			}

			MK_DEV_PANEL.MsgToLog(L"[ atr ���� ��Ʈ ]");
			MK_DEV_PANEL.MsgToLog(L"  - " + m_TargetDirPath);
			MK_DEV_PANEL.InsertEmptyLine();
			
			MK_DEV_PANEL.MsgToLog(L"[ ���� : " + MkStr(m_TargetFileList.GetSize()) + L" �� ]");
			MK_INDEXING_LOOP(m_TargetFileList, i)
			{
				MK_DEV_PANEL.MsgToLog(L"  - " + m_TargetFileList[i]);
			}

			// ��� ����
			MkStr dateStr = MK_SYS_ENV.GetCurrentSystemDate();
			dateStr.PopFront(2); // L"20" ����
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
					::MessageBox(m_MainWindow.GetWindowHandle(), L"���� ���� ��� ����. ���Ἲ üũ ����.", L"����", MB_OK);

					m_CurrIndex = 0;
					m_SuccessCount = 0;
					m_State = 1;
				}
				else
				{
					::MessageBox(m_MainWindow.GetWindowHandle(), L"���� ���� ���� ����. �α� ����.", L"����", MB_OK);

					m_TargetFileList.Clear();
					return;
				}
			}
			else if (m_State == 1)
			{
				if (m_CurrIndex == m_SuccessCount)
				{
					::MessageBox(m_MainWindow.GetWindowHandle(), L"���Ἲ üũ ��� ����!!!", L"����", MB_OK);
				}
				else
				{
					::MessageBox(m_MainWindow.GetWindowHandle(), L"���Ἲ �˻� ���� ���� ����. �α� ����", L"����", MB_OK);
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
				// �а�
				MkByteArray destBuffer;
				MkInterfaceForFileReading frInterface;

				if (!frInterface.SetUp(m_TargetDirPath + m_TargetFileList[m_CurrIndex]))
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" ���� ���� ����");
					break;
				}

				if (frInterface.Read(destBuffer, MkArraySection(0)) == MKDEF_ARRAY_ERROR)
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" ���� �б� ����");
					break;
				}
				frInterface.Clear();

				// ����
				MkByteArray compBuffer;
				if (MkZipCompressor::Compress(destBuffer.GetPtr(), destBuffer.GetSize(), compBuffer) == 0)
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" ���� ���� ����");
					break;
				}
				destBuffer.Clear();

				// XOR
				MkXORer::Convert(compBuffer, L"���� �� �׸� �ǵ�� �̴����!!!");

				// ����
				MkInterfaceForFileWriting fwInterface;
				if (!fwInterface.SetUp(m_ExportDirPath + m_TargetFileList[m_CurrIndex], true, true))
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" ���� ���� ����");
					break;
				}

				if (fwInterface.Write(compBuffer, MkArraySection(0)) != compBuffer.GetSize())
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" ���� ��� ����");
					break;
				}
				fwInterface.Clear();
			}
			else if (m_State == 1)
			{
				// �а�
				MkByteArray destBuffer;
				MkInterfaceForFileReading frInterface;

				if (!frInterface.SetUp(m_ExportDirPath + m_TargetFileList[m_CurrIndex]))
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" ���� ���� ����");
					break;
				}

				if (frInterface.Read(destBuffer, MkArraySection(0)) == MKDEF_ARRAY_ERROR)
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" ���� �б� ����");
					break;
				}
				frInterface.Clear();

				// XOR
				MkXORer::Convert(destBuffer, L"���� �� �׸� �ǵ�� �̴����!!!");

				// ���� ����
				MkByteArray uncompBuffer;
				if (MkZipCompressor::Uncompress(destBuffer.GetPtr(), destBuffer.GetSize(), uncompBuffer) == 0)
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" ���� ���� ����");
					break;
				}
				destBuffer.Clear();

				// ���� ����
				if (!frInterface.SetUp(m_TargetDirPath + m_TargetFileList[m_CurrIndex]))
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" ���� ���� ����");
					break;
				}

				if (frInterface.Read(destBuffer, MkArraySection(0)) == MKDEF_ARRAY_ERROR)
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" ���� �б� ����");
					break;
				}
				frInterface.Clear();

				// ��
				if (destBuffer != uncompBuffer)
				{
					MK_DEV_PANEL.MsgToLog(m_TargetFileList[m_CurrIndex] + L" ������ �������� ����");
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
		// ���� ���� ���
		MkPathName thisDir = parentDir + node.GetNodeName().GetString();
		thisDir.CheckAndAddBackslash();

		// atr ���� �˻�
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

		// ���� ���� �˻�
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

// ��Ʈ�� ����Ʈ������ TestApplication ����
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"RohanWorldAtrWrapper", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 200, 200);

	return 0;
}

//------------------------------------------------------------------------------------------------//

