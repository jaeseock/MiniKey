
#include <process.h>
#include "MkCore_MkCheck.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkThreadManager.h"


//------------------------------------------------------------------------------------------------//

bool MkThreadManager::RegisterThreadUnit(MkBaseThreadUnit* threadUnit)
{
	MK_CHECK(threadUnit != NULL, L"NULL thread unit 등록 시도")
		return false;

	const MkStr& threadName = threadUnit->GetThreadName().GetString();
	MK_CHECK(!threadName.Empty(), L"등록될 thread unit에 이름이 없음")
		return false;

	MK_CHECK(!m_Lock, L"실행중인 MkThreadManager에 thread unit 등록 시도")
		return false;

	_ThreadInfo threadInfo;
	threadInfo.handle = reinterpret_cast<HANDLE>
		(_beginthreadex(NULL, 0, __ThreadProc, reinterpret_cast<LPVOID>(threadUnit), CREATE_SUSPENDED, &threadInfo.id));

	MK_CHECK(threadInfo.handle != NULL, threadName + L" thread 생성 실패")
		return false;

	threadInfo.unit = threadUnit;
	MkWrapInCriticalSection(m_CS)
	{
		m_ThreadInfos.Create(threadUnit->GetThreadName(), threadInfo);
	}

	if (MkDevPanel::InstancePtr() != NULL)
	{
		MK_DEV_PANEL.MsgToLog(threadName + L" thread(" + MkStr(threadInfo.id) + L") 등록", false);
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
				case MkBaseThreadUnit::eRunning: // SetUp() 성공
					++runCount;
					break;

				case MkBaseThreadUnit::eEnd: // SetUp() 실패
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
		// 모든 스레드가 종료될 때 까지 대기
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
	// 인자로 온 스레드 실행
	MkBaseThreadUnit* threadObject = reinterpret_cast<MkBaseThreadUnit*>(lpParam);
	MK_CHECK(threadObject != NULL, L"NULL MkBaseThreadUnit로 스레드 실행 시도")
		return 0;

	const MkStr& threadName = threadObject->GetThreadName().GetString();
	unsigned int currentThreadID = static_cast<unsigned int>(GetCurrentThreadId());
	if (MkDevPanel::InstancePtr() != NULL)
	{
		MK_DEV_PANEL.MsgToLog(threadName + L" thread(" + MkStr(currentThreadID) + L") 실행", false);
	}

	// 초기화
	bool ok = threadObject->SetUp();
	MK_CHECK(ok, threadName + L" thread(" + MkStr(currentThreadID) + L") SetUp() 실패") {}

	if (ok)
	{
		threadObject->__ChangeStateToRunning();

		// 진행
		threadObject->Run();
	}

	// 해제
	threadObject->Clear();

	// 종료 선언
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
