#ifndef __MINIKEY_CORE_MKBIASEDDICE_H__
#define __MINIKEY_CORE_MKBIASEDDICE_H__

//------------------------------------------------------------------------------------------------//
// biased ���� ������
// �������� �������� ������ ����Ʈ��ŭ �ٸ�
//
// ������ �߻��� ID�� Ȯ�� ����Ʈ�� �־� ���, ���� �߻��� ������ ��ϵ� ID�� �ϳ���
// ���� ������ �� ������ �ƴ϶� (�ش� ����Ʈ / �� ����Ʈ) ��ŭ�� ������ ���� �Ҵ�
// ex> ������(����Ʈ) -> 1(10), 2(5), 3(35) ������ �� ����Ʈ(10 + 5 + 35 = 50)����
// - 1�� ���� Ȯ���� 10/50 = 20%
// - 2�� ���� Ȯ���� 5/50 = 10%
// - 3�� ���� Ȯ���� 35/50 = 70%
//
// ���� ���� �˰����� ���λ����� "MkCore_MkRandomGenerator.h" ����
//
// ex>
//	unsigned int testCount = 100000; // �׽�Ʈ Ƚ��
//
//	MkBiasedDice<int> dice;
//	dice.RegisterID(0, 5);
//	dice.RegisterID(1, 10);
//	dice.RegisterID(2, 30);
//	dice.RegisterID(3, 40);
//	dice.RegisterID(4, 100);
//	dice.RegisterID(5, 300);
//	dice.UpdatePoints();
//
//	MkArray<unsigned int> hitCount;
//	hitCount.Fill(6, 0);
//
//	for (unsigned int i=0; i<testCount; ++i)
//	{
//		int id;
//		dice.GenerateRandomID(id);
//		++hitCount[id];
//	}
//
//	float pFactor = 100.f / static_cast<float>(testCount);
//	for (unsigned int i=0; i<6; ++i)
//	{
//		float originalHitPercentage = dice.GetHitPercentage(i);
//		float currentHitPercentage = static_cast<float>(hitCount[i]) * pFactor;
//		float diff = fabs(originalHitPercentage - currentHitPercentage);
//		wprintf(L" - ID %d (%f%%) -> %d (%f%%) hits, %f%% different\n", i, originalHitPercentage, hitCount[i], currentHitPercentage, diff);
//	}
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkCore_MkCheck.h"
#include "MkCore_MkRandomGenerator.h"


template <class IDType>
class MkBiasedDice
{
public:

	// �õ� ����(0 ~ 20000)
	inline void SetSeed(unsigned int seed) { m_RandomGenerator.SetSeed(seed); }

	// ID, Ȯ�� ����Ʈ ���
	// (NOTE) ���� ��� �ݿ��� ���ؼ��� UpdatePoints() ȣ�� �ʿ�
	inline void RegisterID(const IDType& id, unsigned int point)
	{
		MK_CHECK(point > 0, L"MkBiasedDice�� ����Ϸ��� ID�� Ȯ�� ����Ʈ�� 0��")
			return;

		MK_CHECK(!m_IdTable.Exist(id), L"MkBiasedDice�� �̹� �����ϴ� ID ��� �õ�")
			return;

		m_IdTable.Create(id, point);
	}

	// ���� ��ϵǾ� �ִ� ID�� ����
	// (NOTE) ���� ��꿡 �ݿ��� ���ؼ��� UpdatePoints() ȣ�� �ʿ�
	inline void RemoveID(const IDType& id) { if (m_IdTable.Exist(id)) { m_IdTable.Erase(id); } }

	// ���� ��ϵǾ� �ִ� ID�� point�� ����
	// (NOTE) ���� ��꿡 �ݿ��� ���ؼ��� UpdatePoints() ȣ�� �ʿ�
	inline void ResetPoint(const IDType& id, unsigned int point) { if (m_IdTable.Exist(id)) { m_IdTable[id] = point; } }

	// ���� ��ϵǾ� �ִ� ID, point�� Ȯ�� ����
	// ����� �Ϸ�ǰų�, ��ϵ� ID�� point�� ��ȭ�� ������ ��� ȣ�� �ʿ�
	inline void UpdatePoints(void)
	{
		if (m_IdTable.Empty())
			return;

		m_PointTable.Clear();
		m_TotalPoint = 0;

		MkMapLooper<IDType, unsigned int> looper(m_IdTable);
		MK_STL_LOOP(looper)
		{
			m_PointTable.Create(m_TotalPoint, looper.GetCurrentKey());
			m_TotalPoint += looper.GetCurrentField();
		}
	}

	// ���� ��ϵǾ� �ִ� ID�� �ϳ��� Ȯ�� ����Ʈ�� ���� �������� ������ id�� �Ҵ�
	// ��ȯ���� ��������
	// (NOTE) UpdatePoints() ȣ�� ���� ��ȿ
	inline bool GenerateRandomID(IDType& id)
	{
		unsigned int memberCount = m_PointTable.GetSize();
		MK_CHECK(memberCount > 0, L"�ʱ�ȭ���� ���� MkBiasedDice���� ���� ���� �õ�")
			return false;

		if (memberCount == 1)
		{
			id = m_PointTable[m_PointTable.GetFirstKey()];
		}
		else
		{
			unsigned int randomNumber = m_RandomGenerator.GetRandomNumber(m_TotalPoint);
			unsigned int lower, upper;
			m_PointTable.GetBoundKey(randomNumber, lower, upper);
			id = m_PointTable[lower];
		}
		return true;
	}

	// �ش� id�� ����Ȯ���� �ۼ�Ʈ�� ��ȯ(0 ~ 100%)
	// (NOTE) UpdatePoints() ȣ�� ���� ��ȿ
	inline float GetHitPercentage(const IDType& id) const
	{
		if ((!m_IdTable.Exist(id)) || (m_TotalPoint == 0))
			return 0.f;

		return (static_cast<float>(m_IdTable[id]) * 100.f / static_cast<float>(m_TotalPoint));
	}

	// ����
	void Clear(void)
	{
		m_TotalPoint = 0;
		m_IdTable.Clear();
		m_PointTable.Clear();
	}

	// ������
	MkBiasedDice() { m_TotalPoint = 0; }
	MkBiasedDice(unsigned int seed) : m_RandomGenerator(seed) { m_TotalPoint = 0; }

protected:

	MkRandomGenerator m_RandomGenerator;

	unsigned int m_TotalPoint;
	MkMap<IDType, unsigned int> m_IdTable;
	MkMap<unsigned int, IDType> m_PointTable;
};

#endif
