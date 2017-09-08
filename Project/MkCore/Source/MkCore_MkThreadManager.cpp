
#include <process.h>
#include "MkCore_MkCheck.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkThreadManager.h"


//------------------------------------------------------------------------------------------------//

bool MkThreadManager::RegisterThreadUnit(MkBaseThreadUnit* threadUnit)
{
	MK_CHECK(threadUnit != NULL, L"NULL thread unit ��� �õ�")
		return false;

	const MkStr& threadName = threadUnit->GetThreadName().GetString();
	MK_CHECK(!threadName.Empty(), L"��ϵ� thread unit�� �̸��� ����")
		return false;

	MK_CHECK(!m_Lock, L"�������� MkThreadManager�� thread unit ��� �õ�")
		return false;

	_ThreadInfo threadInfo;
	threadInfo.handle = reinterpret_cast<HANDLE>
		(_beginthreadex(NULL, 0, __ThreadProc, reinterpret_cast<LPVOID>(threadUnit), CREATE_SUSPENDED, &threadInfo.id));

	MK_CHECK(threadInfo.handle != NULL, threadName + L" thread ���� ����")
		return false;

	threadInfo.unit = threadUnit;
	MkWrapInCriticalSection(m_CS)
	{
		m_ThreadInfos.Create(threadUnit->GetThreadName(), threadInfo);
	}

	if (MkDevPanel::InstancePtr() != NULL)
	{
		MK_DEV_PANEL.MsgToLog(threadName + L" thread(" + MkStr(threadInfo.id) + L") ���", false);
	}
	return true;
}

void MkThreadManager::WakeUpAll(void)
{
	if (!m_Lock)
	{
		m_Lock = true;
		MkWrapInCriticalSection(m_CS)
		{
			MkHashMapLooper<MkHashStr, _ThreadInfo> looper(m_ThreadInfos);
			MK_STL_LOOP(looper)
			{
				ResumeThread(looper.GetCurrentField().handle);
			}
		}
	}
}

bool MkThreadManager::WaitTillAllThreadsAreRunning(void)
{
	if (!m_Lock)
		return false;

	while (true)
	{
		unsigned int runCount = 0, threadCount = 0;
		MkWrapInCriticalSection(m_CS)
		{
			threadCount = m_ThreadInfos.GetSize();
			
			MkHashMapLooper<MkHashStr, _ThreadInfo> looper(m_ThreadInfos);
			MK_STL_LOOP(looper)
			{
				switch (looper.GetCurrentField().unit->__GetThreadState())
				{
				case MkBaseThreadUnit::eRunning: // SetUp() ����
					++runCount;
					break;

				case MkBaseThreadUnit::eEnd: // SetUp() ����
					return false;
				}
			}
		}

		if (runCount < threadCount)
		{
			Sleep(1);
		}
		else // (runCount == threadCount)
			break;
	}

	return true;
}

void MkThreadManager::StopAll(void)
{
	if (!m_Lock)
		return;

	MkArray<HANDLE> handleArray;
	MkWrapInCriticalSection(m_CS)
	{
		if (!m_ThreadInfos.Empty())
		{
			handleArray.Reserve(m_ThreadInfos.GetSize());

			MkHashMapLooper<MkHashStr, _ThreadInfo> looper(m_ThreadInfos);
			MK_STL_LOOP(looper)
			{
				_ThreadInfo& info = looper.GetCurrentField();
				info.unit->__ChangeStateToClosing();
				handleArray.PushBack(info.handle);
			}
		}
	}

	if (!handleArray.Empty())
	{
		// ��� �����尡 ����� �� ���� ���
		WaitForMultipleObjects(handleArray.GetSize(), handleArray.GetPtr(), TRUE, INFINITE);
	}

	MkWrapInCriticalSection(m_CS)
	{
		MkHashMapLooper<MkHashStr, _ThreadInfo> looper(m_ThreadInfos);
		MK_STL_LOOP(looper)
		{
			delete looper.GetCurrentField().unit;
			::CloseHandle(looper.GetCurrentField().handle);
		}
		m_ThreadInfos.Clear();
	}

	m_Lock = false;
}

unsigned int WINAPI MkThreadManager::__ThreadProc(LPVOID lpParam)
{
	// ���ڷ� �� ������ ����
	MkBaseThreadUnit* threadObject = reinterpret_cast<MkBaseThreadUnit*>(lpParam);
	MK_CHECK(threadObject != NULL, L"NULL MkBaseThreadUnit�� ������ ���� �õ�")
		return 0;

	const MkStr& threadName = threadObject->GetThreadName().GetString();
	unsigned int currentThreadID = static_cast<unsigned int>(GetCurrentThreadId());
	if (MkDevPanel::InstancePtr() != NULL)
	{
		MK_DEV_PANEL.MsgToLog(threadName + L" thread(" + MkStr(currentThreadID) + L") ����", false);
	}

	// �ʱ�ȭ
	bool ok = threadObject->SetUp();
	MK_CHECK(ok, threadName + L" thread(" + MkStr(currentThreadID) + L") SetUp() ����") {}

	if (ok)
	{
		threadObject->__ChangeStateToRunning();

		// ����
		threadObject->Run();
	}

	// ����
	threadObject->Clear();

	// ���� ����
	threadObject->__ChangeStateToEnd();

	if (MkDevPanel::InstancePtr() != NULL)
	{
		MK_DEV_PANEL.MsgToLog(threadName + L" thread(" + MkStr(currentThreadID) + L") end", false);
	}

	return 0;
}

MkThreadManager::MkThreadManager()
{
	m_Lock = false;
}

//------------------------------------------------------------------------------------------------//
