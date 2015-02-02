
#include "MkCore_MkTimeState.h"
#include "MkCore_MkFloatOp.h"
#include "MkCore_MkTimeCounter.h"


//------------------------------------------------------------------------------------------------//

void MkTimeCounter::SetUp(const MkTimeState& currentTimeState, double tickCount)
{
	m_BeginTime = currentTimeState.fullTime;
	m_TickCount = tickCount;
}

bool MkTimeCounter::OnTick(const MkTimeState& currentTimeState) const
{
	if ((m_TickCount <= 0.) || (currentTimeState.fullTime == m_BeginTime))
		return false;

	double currentTime = currentTimeState.fullTime - m_BeginTime;
	double tickEnd = MkFloatOp::SnapToUpperBound(currentTime, m_TickCount);
	double tickBegin = tickEnd - currentTimeState.elapsed;
	return ((currentTime > tickBegin) && (currentTime <= tickEnd));
}

double MkTimeCounter::GetTickTime(const MkTimeState& currentTimeState) const
{
	double currentTime = currentTimeState.fullTime - m_BeginTime;
	return (m_TickCount <= 0.) ? currentTime : MkFloatOp::GetRemainder(currentTime, m_TickCount);
}

double MkTimeCounter::GetTickTime
(const MkTimeState& currentTimeState, unsigned int& hour, unsigned int& minute, unsigned int& second, unsigned int& millisec) const
{
	double tickTime = GetTickTime(currentTimeState);
	MkFloatOp::ConvertSecondsToClockTime(tickTime, hour, minute, second, millisec);
	return tickTime;
}

float MkTimeCounter::GetTickRatio(const MkTimeState& currentTimeState) const
{
	if (m_TickCount <= 0.)
		return 0.f;

	double currentTime = currentTimeState.fullTime - m_BeginTime;
	double remainder = MkFloatOp::GetRemainder(currentTime, m_TickCount);
	return static_cast<float>(remainder / m_TickCount);
}

//------------------------------------------------------------------------------------------------//
