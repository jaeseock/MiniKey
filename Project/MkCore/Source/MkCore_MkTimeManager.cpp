
#include <Windows.h>
#include "MkCore_MkProjectDefinition.h"
#include "MkCore_MkFloatOp.h"
#include "MkCore_MkStr.h"
#include "MkCore_MkTimeManager.h"


//------------------------------------------------------------------------------------------------//

void MkTimeManager::SetUp(void)
{
	m_ZeroTime = GetTickCount();
	m_CurrentTimeState = MkTimeState();
}

void MkTimeManager::Update(void)
{
	while (true)
	{
		// 현재 시간에 해당하는 Fixed 프레임 카운트를 구함
		// (NOTE) Windows계열 시간함수들의 정밀도는 별도로 지정하지 않는 이상 15~16 ms임
		DWORD systemTime = GetTickCount() - m_ZeroTime;
		double currentTick = static_cast<double>(systemTime) * MKDEF_FRAME_TIME_FACTOR;

		// 목표 프레임 카운트
		unsigned int targetFrameCount = static_cast<unsigned int>(currentTick);

		// 임시 time state
		MkTimeState ts = m_CurrentTimeState;

		// 목표 프레임까지 도달하기 위해 소모되어야 할 프레임 카운트
		unsigned int lastFrameCount = ts.frameCount;
		unsigned int remainFrame = targetFrameCount - lastFrameCount;

		// 소모 프레임이 남았을 경우 시간 진행
		if (remainFrame > 0)
		{
			// 1 프레임 진행
			ts.frameCount = lastFrameCount + 1;

			// 이전 프레임과 현재 프레임 사이의 경과된 시간 갱신
			ts.elapsed = MKDEF_FRAME_ELAPSED_TIME;

			// 진행시간은 경과시간의 누적
			ts.fullTime += MKDEF_FRAME_ELAPSED_TIME;

			// 잔여 프레임이 남았을 경우 busy frame
			ts.busyFrame = ((remainFrame - 1) > 0);

			// 최종반영
			m_CurrentTimeState = ts;

			break;
		}
		else
		{
			// time slice 양보의 미덕
			// Sleep(0)은 자칫 starvation을 불러 올 수도 있으므로 1을 쓰지만 OS 스케쥴링에 따라 수십 ms 밀릴 수도 있음
			// 이는 전적으로 windows에 종속된 것이므로 기본 시간정밀도(xp 이하 10ms, vista 이상 20ms) 주의
			// timeBeginPeriod/timeEndPeriod는 의미 없음
			Sleep(1);
		}
	}
}

void MkTimeManager::GetCurrentTimeState(MkStr& buffer)
{
	MkTimeState currTimeState;
	GetCurrentTimeState(currTimeState);
	
	unsigned int hour, minute, second, millisec;
	MkFloatOp::ConvertSecondsToClockTime(currTimeState.fullTime, hour, minute, second, millisec);
	MkStr tmp;
	tmp.Reserve(30);
	if (hour < 10)
	{
		tmp += L"0";
	}
	tmp += hour;
	tmp += (minute < 10) ? L":0" : L":";
	tmp += minute;
	tmp += (second < 10) ? L":0" : L":";
	tmp += second;
	if (millisec < 10)
	{
		tmp += L":00";
	}
	else if (millisec < 100)
	{
		tmp += L":0";
	}
	else
	{
		tmp += L":";
	}
	tmp += millisec;
	tmp += L" (";
	tmp += currTimeState.frameCount;
	tmp += L")";

	buffer = tmp;
}

MkTimeManager::MkTimeManager() : MkSingletonPattern<MkTimeManager>()
{
	SetUp();
}

//------------------------------------------------------------------------------------------------//
