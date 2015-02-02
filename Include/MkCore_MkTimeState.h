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

	// MkTimeManager.SetUp()�� ������ ī��Ʈ
	// MkTimeManager�� �����ϴ� 49.7�ϰ��� �� �ʼ��� 4,294,080�̰� 60fps�� ��� �ִ� ������ ���� 257,644,800�̹Ƿ� unsigned int ������ ���� ����
	unsigned int frameCount;

	// ���� �����Ӻ��� ���� �����Ӱ� �ð�(sec) : time weight �����
	double elapsed;

	// MkTimeManager.SetUp()�� ��� �� ���� �ð�(sec) : time weight �����Ǿ� ����� ����
	double fullTime;

	// frame count�� ��ǥġ�� ����ġ���� ����
	bool busyFrame;
};

//------------------------------------------------------------------------------------------------//

#endif

