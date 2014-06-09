
#include "MkCore_MkHashStr.h"
#include "MkCore_MkTimeManager.h"
#include "MkCore_MkProfilingManager.h"
#include "MkCore_MkBaseThreadUnit.h"


//------------------------------------------------------------------------------------------------//

void MkBaseThreadUnit::Run(void)
{
	const MkHashStr profKey = MKDEF_PROFILING_PREFIX_FOR_THREAD + GetThreadName().GetString();
	MkTimeState lastTimeState;
	lastTimeState.frameCount = 0xffffffff;

	// 루핑 진행
	while (m_CurrentState == eRunning)
	{
		// 현재 time state 얻음
		MkTimeState currTimeState;
		MK_TIME_MGR.GetCurrentTimeState(currTimeState);

		// 새로운 프레임이면 수행
		if (currTimeState.frameCount != lastTimeState.frameCount)
		{
			MK_PROF_MGR.Begin(profKey); // profiling start

			Update(currTimeState);

			MK_PROF_MGR.End(profKey); // profiling end

			lastTimeState = currTimeState;
		}
		// 동일 시간이면 대기
		else
		{
			Sleep(1);
		}
	}
}

MkBaseThreadUnit::MkBaseThreadUnit(const MkHashStr& threadName)
{
	m_ThreadName = threadName;
	m_CurrentState = eReady;
}

//------------------------------------------------------------------------------------------------//
