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
	void SetUp(const MkTimeState& currentTimeState, float tickCount = 0.f);

	// �� �������� tick frame�� �ش��ϴ��� ���� ��ȯ
	// (NOTE) tick�� ���� ���(tickCount <= 0.f) �׻� false ��ȯ
	// (NOTE) �ð� ������ �����ϹǷ� ��Ȯ�ϰ� tickCount��ŭ �������� ���� �ƴ�
	//           (tickCount == 2.f��� 1.998f, 1.989���� tick ������ �� ����)
	bool OnTick(const MkTimeState& currentTimeState) const;

	// �� tick �������� ���� �ð��� �ʴ����� ��ȯ
	float GetTickTime(const MkTimeState& currentTimeState) const;

	// �� tick �������� ���� �ð��� (��:��:��:�и���) ������ ��ȯ
	float GetTickTime(const MkTimeState& currentTimeState, unsigned int& hour, unsigned int& minute, unsigned int& second, unsigned int& millisec) const;

	// �� tick �������� ���� ���� ��ȯ(0.f ~ 1.f)
	// (NOTE) tick�� ���� ���(tickCount <= 0.f) �׻� 0.f ��ȯ
	float GetTickRatio(const MkTimeState& currentTimeState) const;

	// tick count ��ȯ
	inline float GetTickCount(void) const { return m_TickCount; }

protected:

	float m_BeginTime;
	float m_TickCount;
};

#endif
