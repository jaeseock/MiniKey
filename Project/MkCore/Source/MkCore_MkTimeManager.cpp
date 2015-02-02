
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
		// ���� �ð��� �ش��ϴ� Fixed ������ ī��Ʈ�� ����
		// (NOTE) Windows�迭 �ð��Լ����� ���е��� ������ �������� �ʴ� �̻� 15~16 ms��
		DWORD systemTime = GetTickCount() - m_ZeroTime;
		double currentTick = static_cast<double>(systemTime) * MKDEF_FRAME_TIME_FACTOR;

		// ��ǥ ������ ī��Ʈ
		unsigned int targetFrameCount = static_cast<unsigned int>(currentTick);

		// �ӽ� time state
		MkTimeState ts = m_CurrentTimeState;

		// ��ǥ �����ӱ��� �����ϱ� ���� �Ҹ�Ǿ�� �� ������ ī��Ʈ
		unsigned int lastFrameCount = ts.frameCount;
		unsigned int remainFrame = targetFrameCount - lastFrameCount;

		// �Ҹ� �������� ������ ��� �ð� ����
		if (remainFrame > 0)
		{
			// 1 ������ ����
			ts.frameCount = lastFrameCount + 1;

			// ���� �����Ӱ� ���� ������ ������ ����� �ð� ����
			ts.elapsed = MKDEF_FRAME_ELAPSED_TIME;

			// ����ð��� ����ð��� ����
			ts.fullTime += MKDEF_FRAME_ELAPSED_TIME;

			// �ܿ� �������� ������ ��� busy frame
			ts.busyFrame = ((remainFrame - 1) > 0);

			// �����ݿ�
			m_CurrentTimeState = ts;

			break;
		}
		else
		{
			// time slice �纸�� �̴�
			// Sleep(0)�� ��ĩ starvation�� �ҷ� �� ���� �����Ƿ� 1�� ������ OS �����층�� ���� ���� ms �и� ���� ����
			// �̴� �������� windows�� ���ӵ� ���̹Ƿ� �⺻ �ð����е�(xp ���� 10ms, vista �̻� 20ms) ����
			// timeBeginPeriod/timeEndPeriod�� �ǹ� ����
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
