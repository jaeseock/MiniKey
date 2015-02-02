#ifndef __MINIKEY_CORE_MKTIMECOUNTER_H__
#define __MINIKEY_CORE_MKTIMECOUNTER_H__

//------------------------------------------------------------------------------------------------//
// clock & stopwatch(tick)
// MkTimeManager ���
// (NOTE) MkTimeManager�� ���µǸ� ��ȿ�� ���
//------------------------------------------------------------------------------------------------//

class MkTimeState;

class MkTimeCounter
{
public:

	// �ʱ�ȭ
	// tickCount : ī��Ʈ �ð�(�ʴ���). 0������ ��� tick ����
	void SetUp(const MkTimeState& currentTimeState, double tickCount = 0.);

	// �� �������� tick frame�� �ش��ϴ��� ���� ��ȯ
	// (NOTE) tick�� ���� ���(tickCount <= 0.) �׻� false ��ȯ
	// (NOTE) �ð� ������ �����ϹǷ� ��Ȯ�ϰ� tickCount��ŭ �������� ���� �ƴ�
	//           (tickCount == 2.��� 1.998, 1.989���� tick ������ �� ����)
	bool OnTick(const MkTimeState& currentTimeState) const;

	// �� tick �������� ���� �ð��� �ʴ����� ��ȯ
	double GetTickTime(const MkTimeState& currentTimeState) const;

	// �� tick �������� ���� �ð��� (��:��:��:�и���) ������ ��ȯ
	double GetTickTime(const MkTimeState& currentTimeState, unsigned int& hour, unsigned int& minute, unsigned int& second, unsigned int& millisec) const;

	// �� tick �������� ���� ���� ��ȯ(0.f ~ 1.f)
	// (NOTE) tick�� ���� ���(tickCount <= 0.) �׻� 0.f ��ȯ
	float GetTickRatio(const MkTimeState& currentTimeState) const;

	// tick count ��ȯ
	inline double GetTickCount(void) const { return m_TickCount; }

protected:

	double m_BeginTime;
	double m_TickCount;
};

#endif
