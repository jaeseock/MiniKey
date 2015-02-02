#ifndef __MINIKEY_CORE_MKTIMECOUNTER_H__
#define __MINIKEY_CORE_MKTIMECOUNTER_H__

//------------------------------------------------------------------------------------------------//
// clock & stopwatch(tick)
// MkTimeManager 기반
// (NOTE) MkTimeManager가 리셋되면 유효성 상실
//------------------------------------------------------------------------------------------------//

class MkTimeState;

class MkTimeCounter
{
public:

	// 초기화
	// tickCount : 카운트 시간(초단위). 0이하일 경우 tick 없음
	void SetUp(const MkTimeState& currentTimeState, double tickCount = 0.);

	// 현 프레임이 tick frame에 해당하는지 여부 반환
	// (NOTE) tick이 없을 경우(tickCount <= 0.) 항상 false 반환
	// (NOTE) 시간 오차가 존재하므로 정확하게 tickCount만큼 끊어지는 것이 아님
	//           (tickCount == 2.라면 1.998, 1.989에도 tick 인정될 수 있음)
	bool OnTick(const MkTimeState& currentTimeState) const;

	// 현 tick 내에서의 진행 시간을 초단위로 반환
	double GetTickTime(const MkTimeState& currentTimeState) const;

	// 현 tick 내에서의 진행 시간을 (시:분:초:밀리초) 단위로 변환
	double GetTickTime(const MkTimeState& currentTimeState, unsigned int& hour, unsigned int& minute, unsigned int& second, unsigned int& millisec) const;

	// 현 tick 내에서의 진행 비율 반환(0.f ~ 1.f)
	// (NOTE) tick이 없을 경우(tickCount <= 0.) 항상 0.f 반환
	float GetTickRatio(const MkTimeState& currentTimeState) const;

	// tick count 반환
	inline double GetTickCount(void) const { return m_TickCount; }

protected:

	double m_BeginTime;
	double m_TickCount;
};

#endif
