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

	unsigned int frameCount; // MkTimeManager.SetUp()�� ������ ī��Ʈ
	float elapsed; // ���� �����Ӻ��� ���� �����Ӱ� �ð�(sec) : time weight �����
	float fullTime; // MkTimeManager.SetUp()�� ��� �� ���� �ð�(sec) : time weight �����Ǿ� ����� ����
	bool busyFrame; // frame count�� ��ǥġ�� ����ġ���� ����
};

//------------------------------------------------------------------------------------------------//

#endif

