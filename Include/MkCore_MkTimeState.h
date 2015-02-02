#ifndef __MINIKEY_CORE_MKTIMESTATE_H__
#define __MINIKEY_CORE_MKTIMESTATE_H__

//------------------------------------------------------------------------------------------------//

class MkTimeState
{
public:

	MkTimeState()
	{
		frameCount = 0;
		elapsed = 0.;
		fullTime = 0.;
		busyFrame = false;
	}

	MkTimeState(unsigned int inFrameCount, double inElapsed, double inFullTime, bool inBusyFrame)
	{
		frameCount = inFrameCount;
		elapsed = inElapsed;
		fullTime = inFullTime;
		busyFrame = inBusyFrame;
	}

public:

	// MkTimeManager.SetUp()후 프레임 카운트
	// MkTimeManager가 보장하는 49.7일간의 총 초수는 4,294,080이고 60fps의 경우 최대 프레임 수는 257,644,800이므로 unsigned int 범위를 넘지 않음
	unsigned int frameCount;

	// 이전 프레임부터 현재 프레임간 시간(sec) : time weight 적용됨
	double elapsed;

	// MkTimeManager.SetUp()후 경과 된 현재 시간(sec) : time weight 누적되어 적용된 상태
	double fullTime;

	// frame count가 목표치에 못미치는지 여부
	bool busyFrame;
};

//------------------------------------------------------------------------------------------------//

#endif

