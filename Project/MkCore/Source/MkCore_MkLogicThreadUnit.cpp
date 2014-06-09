
#include "MkCore_MkPageManager.h"
#include "MkCore_MkTimeManager.h"
#include "MkCore_MkProfilingManager.h"
#include "MkCore_MkLogicThreadUnit.h"


//------------------------------------------------------------------------------------------------//

void MkLogicThreadUnit::Run(void)
{
	const MkHashStr profKey = MKDEF_PROFILING_PREFIX_FOR_THREAD + GetThreadName().GetString();
	
	// ���� ����
	while (m_CurrentState == eRunning)
	{
		// ������ ����
		MK_TIME_MGR.Update();

		MK_PROF_MGR.Begin(profKey); // profiling start

		MkTimeState currTimeState;
		MK_TIME_MGR.GetCurrentTimeState(currTimeState);
		Update(currTimeState);

		MK_PROF_MGR.End(profKey); // profiling end
	}
}

void MkLogicThreadUnit::Update(const MkTimeState& timeState)
{
	// page ����
	MK_PAGE_MGR.__Update();
}

void MkLogicThreadUnit::Clear(void)
{
	// ��� ������ ����
	MK_PAGE_MGR.__Clear();
}

//------------------------------------------------------------------------------------------------//
