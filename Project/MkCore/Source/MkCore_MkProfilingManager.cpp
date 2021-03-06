
#include "MkCore_MkProjectDefinition.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkProfilingManager.h"


//------------------------------------------------------------------------------------------------//

void MkProfilingManager::Begin(const MkHashStr& key)
{
	if (IsEnable())
	{
		MkWrapInCriticalSection(m_CS_Units)
		{
			if (m_Units.Exist(key))
			{
				m_Units[key].Begin();
			}
			else
			{
				_ProfilerUnit& unit = m_Units.Create(key);
				unit.SetUp(m_SystemFrequenceFactor);
				unit.Begin();
				m_Units.Rehash();
			}
		}
	}
}

void MkProfilingManager::End(const MkHashStr& key, bool flushImmediately)
{
	if (IsEnable())
	{
		MkWrapInCriticalSection(m_CS_Units)
		{
			if (m_Units.Exist(key))
			{
				m_Units[key].End(flushImmediately);
			}
		}
	}
}

bool MkProfilingManager::GetUpdated(const MkHashStr& key)
{
	if (IsEnable())
	{
		MkWrapInCriticalSection(m_CS_Units)
		{
			if (m_Units.Exist(key))
			{
				return m_Units[key].GetUpdated();
			}
		}
	}
	return false;
}

__int64 MkProfilingManager::GetEverageTick(const MkHashStr& key)
{
	if (IsEnable())
	{
		MkWrapInCriticalSection(m_CS_Units)
		{
			if (m_Units.Exist(key))
			{
				return m_Units[key].GetEverageTick();
			}
		}
	}
	return 0;
}

unsigned int MkProfilingManager::GetEverageTime(const MkHashStr& key)
{
	if (IsEnable())
	{
		MkWrapInCriticalSection(m_CS_Units)
		{
			if (m_Units.Exist(key))
			{
				return m_Units[key].GetEverageTime();
			}
		}
	}
	return 0;
}

void MkProfilingManager::GetEverageTimeStr(const MkHashStr& key, MkStr& buffer)
{
	__int64 microSec = GetEverageTime(key);
	float percentInFrame = static_cast<float>(microSec) * static_cast<float>(MKDEF_FRAME_COUNTS_PER_SEC) / 10000.f;
	__int64 sec = 0, milliSec = 0;
	if (microSec >= 1000000)
	{
		sec = microSec / 1000000;
		microSec -= sec * 1000000;
	}
	if (microSec >= 1000)
	{
		milliSec = microSec / 1000;
		microSec -= milliSec * 1000;
	}

	MkStr msg;
	msg.Reserve(key.GetSize() + 50);
	msg += key;
	msg += L" : ";
	if (sec > 0)
	{
		msg += sec;
		msg += L"(s) ";
	}
	if (milliSec > 0)
	{
		msg += milliSec;
		msg += L"(ms) ";
	}
	if ((microSec > 0) || ((sec == 0) && (milliSec == 0)))
	{
		msg += microSec;
		msg += L"(µs)";
	}
	else
	{
		msg.BackSpace(1);
	}
	
	msg += L", ";
	msg += percentInFrame;
	msg += L"%";
	buffer = msg;
}

unsigned int MkProfilingManager::GetKeyList(MkArray<MkHashStr>& buffer)
{
	if (IsEnable())
	{
		MkScopedCriticalSection(m_CS_Units);
		return m_Units.GetKeyList(buffer);
	}
	return 0;
}

MkProfilingManager::MkProfilingManager() : MkSwitchedSingletonPattern<MkProfilingManager>()
{
	m_Enable = _CheckSystemSupport();
}

MkProfilingManager::MkProfilingManager(bool enable) : MkSwitchedSingletonPattern<MkProfilingManager>(enable)
{
	m_Enable = (enable) ? _CheckSystemSupport() : false;
}

//------------------------------------------------------------------------------------------------//

void MkProfilingManager::_ProfilerUnit::SetUp(double systemFrequenceFactor)
{
	m_SystemFrequenceFactor = systemFrequenceFactor;
	m_BeginSet = false;
	m_Updated = true;
	m_TickPool.Reserve(MKDEF_FRAME_COUNTS_PER_SEC);
	m_EverageTick = -1;
	m_EverageTime = 0xffffffff;
}

void MkProfilingManager::_ProfilerUnit::GetClock(unsigned __int64& container)
{
	static unsigned __int32 lowPart, highPart;

	_asm _emit 0x0f
	_asm _emit 0x31
	_asm mov lowPart, eax
	_asm mov highPart, edx

	container = highPart;
	container <<= 32;
	container |= lowPart;
}

void MkProfilingManager::_ProfilerUnit::Begin(void)
{
	m_BeginSet = true;
	QueryPerformanceCounter(&m_StartTick);
}

void MkProfilingManager::_ProfilerUnit::End(bool flushImmediately)
{
	if (!m_BeginSet)
		return;

	// tick
	QueryPerformanceCounter(&m_EndTick);
	m_BeginSet = false;

	// Begin과 End 사이 틱을 보관
	m_TickPool.PushBack(m_EndTick.QuadPart - m_StartTick.QuadPart);

	// 카운터 체크
	unsigned int currPoolSize = m_TickPool.GetSize();
	if (flushImmediately || (currPoolSize >= MKDEF_FRAME_COUNTS_PER_SEC))
	{
		// 기록된 tick의 합
		__int64 sumOfTick = 0;
		MK_INDEXING_LOOP(m_TickPool, i)
		{
			sumOfTick += m_TickPool[i];
		}

		// 평균 틱
		double dTick = static_cast<double>(sumOfTick) / static_cast<double>(currPoolSize);
		__int64 result = static_cast<__int64>(dTick);
		m_Updated = (result != m_EverageTick);
		m_EverageTick = result;
		m_EverageTime = static_cast<unsigned int>(dTick * m_SystemFrequenceFactor);

		m_TickPool.Flush();
	}
	else
	{
		m_Updated = false;
	}
}

//------------------------------------------------------------------------------------------------//

bool MkProfilingManager::_CheckSystemSupport(void)
{
	LARGE_INTEGER freqTick;
	bool ok = (QueryPerformanceFrequency(&freqTick) == TRUE);
	if (ok)
	{
		m_SystemFrequenceFactor = static_cast<double>(1000000) / static_cast<double>(freqTick.QuadPart);
	}
	return ok;
}

//------------------------------------------------------------------------------------------------//

MkFocusProfiler::MkFocusProfiler(const MkHashStr& key)
{
	m_Key = key;
	m_FlushAndLog = false;
	MK_PROF_MGR.Begin(m_Key);
}

MkFocusProfiler::MkFocusProfiler(const MkHashStr& key, bool flushAndLog)
{
	m_Key = key;
	m_FlushAndLog = flushAndLog;
	MK_PROF_MGR.Begin(m_Key);
}

MkFocusProfiler::~MkFocusProfiler()
{
	MK_PROF_MGR.End(m_Key, m_FlushAndLog);

	if (m_FlushAndLog)
	{
		MkStr sbuf;
		MK_PROF_MGR.GetEverageTimeStr(m_Key, sbuf);
		MK_DEV_PANEL.MsgToLog(sbuf);
	}
}

//------------------------------------------------------------------------------------------------//

