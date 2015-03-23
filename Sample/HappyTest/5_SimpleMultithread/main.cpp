
//------------------------------------------------------------------------------------------------//
// single/multi-thread application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkLogicThreadUnit.h"

#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

// for test
#include "MkCore_MkPathName.h"
#include "MkCore_MkInputManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkLockable.h"
#include "MkCore_MkProfilingManager.h"
#include "MkCore_MkThreadManager.h"
#include "MkCore_MkSystemEnvironment.h"

//------------------------------------------------------------------------------------------------//
// multi-thread�� ��� �ּ� ó��
//------------------------------------------------------------------------------------------------//

#define SAMPLEDEF_SINGLE_THREAD

//------------------------------------------------------------------------------------------------//

class MkTestThreadUnit : public MkBaseThreadUnit
{
public:

	void SetTargetDir(const MkPathName& targetDir)
	{
		m_TargetDir = targetDir;
	}

	void AddFilePath(const MkPathName& filePath)
	{
		MkArray<MkPathName> filePathList = m_FilePathList;

		filePathList.PushBack(filePath);

		m_FilePathList = filePathList;
	}

	bool IsComplete(void) { return m_Done; }

	virtual void Update(const MkTimeState& timeState)
	{
		MkPathName targetDir = m_TargetDir;
		MkArray<MkPathName> filePathList = m_FilePathList;

		// ����!!!
		if ((!targetDir.Empty()) && (!filePathList.Empty()))
		{
			MK_INDEXING_LOOP(filePathList, i)
			{
				MkStr buffer;
				if (buffer.ReadTextFile(targetDir + filePathList[i]))
				{
					unsigned int cnt = 0;
					MK_INDEXING_LOOP(buffer, j)
					{
						if ((buffer[j] == L'a') || (buffer[j] == L'A'))
						{
							++cnt;
						}
					}

					//MK_DEV_PANEL.MsgToLog(filePathList[i] + L" : " + MkStr(cnt), false);
				}
			}

			filePathList.Clear();
			m_FilePathList = filePathList;

			m_Done = true;
		}
	}

	MkTestThreadUnit(const MkHashStr& threadName) : MkBaseThreadUnit(threadName)
	{
		m_Done = false;
	}

	virtual ~MkTestThreadUnit() { Clear(); }

protected:

	MkLockable<MkPathName> m_TargetDir;
	MkLockable< MkArray<MkPathName> > m_FilePathList;

	MkLockable<bool> m_Done;
};

//------------------------------------------------------------------------------------------------//

// single-thread������ TestFramework ����
#ifdef SAMPLEDEF_SINGLE_THREAD

class TestFramework : public MkBaseFramework
{
public:

	// Ȯ�� ����
	virtual void Update(void)
	{
		static const MkHashStr S_PROF_KEY = L"Single thread";
		static const MkHashStr M_PROF_KEY = L"Multi thread";

		// single-thread�� �׽�Ʈ
		if (MK_INPUT_MGR.GetKeyReleased(L'1'))
		{
			// ��θ� �޾�
			MkPathName targetPath;
			if (targetPath.GetDirectoryPathFromDialog(L"���ҽ� ������ ����", m_MainWindow.GetWindowHandle(), MkPathName::GetModuleDirectory()))
			{
				MkArray<MkPathName> filePathList;
				targetPath.GetFileList(filePathList);

				if (!filePathList.Empty())
				{
					// profiling
					MK_PROF_MGR.Begin(S_PROF_KEY);

					// �˻� ����!!!
					MK_INDEXING_LOOP(filePathList, i)
					{
						const MkPathName& filePath = filePathList[i];
						MkStr ext = filePath.GetFileExtension();
						if ((ext == L"cpp") || (ext == L"h") || (ext == L"txt"))
						{
							MkStr buffer;
							if (buffer.ReadTextFile(targetPath + filePath))
							{
								unsigned int cnt = 0;
								MK_INDEXING_LOOP(buffer, j)
								{
									if ((buffer[j] == L'a') || (buffer[j] == L'A'))
									{
										++cnt;
									}
								}

								//MK_DEV_PANEL.MsgToLog(filePath + L" : " + MkStr(cnt), false);
							}
						}
					}

					// ��� ���
					MK_PROF_MGR.End(S_PROF_KEY, true);
					MkStr profBuf;
					MK_PROF_MGR.GetEverageTimeStr(S_PROF_KEY, profBuf);

					MK_DEV_PANEL.MsgToLog(L"//--------------------------------------------------------//", false);
					MK_DEV_PANEL.MsgToLog(L"// �׽�Ʈ ��~ : " + profBuf, false);
					MK_DEV_PANEL.MsgToLog(L"//--------------------------------------------------------//", false);
				}
			}
		}

		// multi-thread�� �׽�Ʈ
		if (MK_INPUT_MGR.GetKeyReleased(L'2'))
		{
			MkArray<MkPathName> textFilePathList;

			// ��θ� �޾�
			MkPathName targetPath;
			if (targetPath.GetDirectoryPathFromDialog(L"���ҽ� ������ ����", m_MainWindow.GetWindowHandle(), MkPathName::GetModuleDirectory()))
			{
				MkArray<MkPathName> filePathList;
				targetPath.GetFileList(filePathList);

				if (!filePathList.Empty())
				{
					textFilePathList.Reserve(filePathList.GetSize());

					MK_INDEXING_LOOP(filePathList, i)
					{
						const MkPathName& filePath = filePathList[i];
						MkStr ext = filePath.GetFileExtension();
						if ((ext == L"cpp") || (ext == L"h") || (ext == L"txt"))
						{
							textFilePathList.PushBack(filePath);
						}
					}
				}
			}

			// �˻� ����?
			if (!textFilePathList.Empty())
			{
				unsigned int THREAD_COUNT = MK_SYS_ENV.GetNumberOfProcessors() + 1;

				// thread unit ����. ���۽� ��� ����
				m_ThreadUnitBuffer.Reserve(THREAD_COUNT);

				for (unsigned int i=0; i<THREAD_COUNT; ++i)
				{
					MkTestThreadUnit* threadUnit = new MkTestThreadUnit(L"Test_" + MkStr(i));
					m_ThreadUnitBuffer.PushBack(threadUnit);

					threadUnit->SetTargetDir(targetPath);

					m_TestThreadMgr.RegisterThreadUnit(m_ThreadUnitBuffer[i]);
				}

				// �ϰŸ� �й�
				MK_INDEXING_LOOP(textFilePathList, i)
				{
					unsigned int index = i % THREAD_COUNT;
					MkTestThreadUnit* worker = m_ThreadUnitBuffer[index];

					worker->AddFilePath(textFilePathList[i]);
				}

				// ��ϵ� ��� ������ ���� ����
				// ��� unit�� SetUp �ǽ�
				m_TestThreadMgr.WakeUpAll();

				// ��� unit�� SetUp�� �Ϸ�� �� ���� ���. �����ϸ� ����
				m_TestThreadMgr.WaitTillAllThreadsAreRunning();

				// profiling
				MK_PROF_MGR.Begin(M_PROF_KEY);
			}
		}

		// milti-thread �˻�
		if (!m_ThreadUnitBuffer.Empty())
		{
			unsigned int completeCnt = 0;

			MK_INDEXING_LOOP(m_ThreadUnitBuffer, i)
			{
				if (m_ThreadUnitBuffer[i]->IsComplete())
					++completeCnt;
			}

			if (completeCnt == m_ThreadUnitBuffer.GetSize())
			{
				// ��� ���
				MK_PROF_MGR.End(M_PROF_KEY, true);
				MkStr profBuf;
				MK_PROF_MGR.GetEverageTimeStr(M_PROF_KEY, profBuf);

				MK_DEV_PANEL.MsgToLog(L"//--------------------------------------------------------//", false);
				MK_DEV_PANEL.MsgToLog(L"// �׽�Ʈ ��~ : " + profBuf, false);
				MK_DEV_PANEL.MsgToLog(L"//--------------------------------------------------------//", false);

				// ������ ���� ��� ����
				m_TestThreadMgr.StopAll();
				m_ThreadUnitBuffer.Clear();
			}
		}
	}

	virtual ~TestFramework() {}

protected:

	MkThreadManager m_TestThreadMgr;

	MkArray<MkTestThreadUnit*> m_ThreadUnitBuffer;
};

#else

// multi-thread������ TestThreadUnit ����
class TestThreadUnit : public MkLogicThreadUnit
{
public:

	TestThreadUnit(const MkHashStr& threadName) : MkLogicThreadUnit(threadName) {}
	virtual ~TestThreadUnit() {}
};

// multi-thread������ TestFramework ����
class TestFramework : public MkBaseFramework
{
public:
	virtual MkBaseThreadUnit* CreateLogicThreadUnit(const MkHashStr& threadName) const { return new TestThreadUnit(threadName); }
	virtual ~TestFramework() {}
};

#endif

// TestApplication ����(����)
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
	application.Run(hI, L"win32 empty project", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

