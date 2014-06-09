
#include "MkCore_MkTimeState.h"
#include "MkCore_MkFloatOp.h"
#include "MkCore_MkTimeCounter.h"


//------------------------------------------------------------------------------------------------//

void MkTimeCounter::SetUp(const MkTimeState& currentTimeState, float tickCount)
{
	m_BeginTime = currentTimeState.fullTime;
	m_TickCount = tickCount;
}

bool MkTimeCounter::OnTick(const MkTimeState& currentTimeState) const
{
	if ((m_TickCount <= 0.f) || (currentTimeState.fullTime == m_BeginTime))
		return false;

	float currentTime = currentTimeState.fullTime - m_BeginTime;
	float tickEnd = MkFloatOp::SnapToUpperBound(currentTime, m_TickCount);
	float tickBegin = tickEnd - currentTimeState.elapsed;
	return ((currentTime > tickBegin) && (currentTime <= tickEnd));
}

float MkTimeCounter::GetTickTime(const MkTimeState& currentTimeState) const
{
	float currentTime = currentTimeState.fullTime - m_BeginTime;
	return (m_TickCount <= 0.f) ? currentTime : MkFloatOp::GetRemainder(currentTime, m_TickCount);
}

float MkTimeCounter::GetTickTime
(const MkTimeState& currentTimeState, unsigned int& hour, unsigned int& minute, unsigned int& second, unsigned int& millisec) const
{
	float tickTime = GetTickTime(currentTimeState);
	MkFloatOp::ConvertSecondsToClockTime(tickTime, hour, minute, second, millisec);
	return tickTime;
}

float MkTimeCounter::GetTickRatio(const MkTimeState& currentTimeState) const
{
	if (m_TickCount <= 0.f)
		return 0.f;

	float currentTime = currentTimeState.fullTime - m_BeginTime;
	float remainder = MkFloatOp::GetRemainder(currentTime, m_TickCount);
	return (remainder / m_TickCount);
}

//------------------------------------------------------------------------------------------------//
