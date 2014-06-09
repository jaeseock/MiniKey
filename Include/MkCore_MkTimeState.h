#ifndef __MINIKEY_CORE_MKTIMESTATE_H__
#define __MINIKEY_CORE_MKTIMESTATE_H__

//------------------------------------------------------------------------------------------------//

class MkTimeState
{
public:

	MkTimeState()
	{
		frameCount = 0;
		elapsed = 0.f;
		fullTime = 0.f;
		busyFrame = false;
	}

	MkTimeState(unsigned int inFrameCount, float inElapsed, float inFullTime, bool inBusyFrame)
	{
		frameCount = inFrameCount;
		elapsed = inElapsed;
		fullTime = inFullTime;
		busyFrame = inBusyFrame;
	}

public:

	unsigned int frameCount; // MkTimeManager.SetUp()후 프레임 카운트
	float elapsed; // 이전 프레임부터 현재 프레임간 시간(sec) : time weight 적용됨
	float fullTime; // MkTimeManager.SetUp()후 경과 된 현재 시간(sec) : time weight 누적되어 적용된 상태
	bool busyFrame; // frame count가 목표치에 못미치는지 여부
};

//------------------------------------------------------------------------------------------------//

#endif

