#ifndef __MINIKEY_CORE_MKBIASEDDICE_H__
#define __MINIKEY_CORE_MKBIASEDDICE_H__

//------------------------------------------------------------------------------------------------//
// biased 랜덤 생성기
// 난수값의 분포도가 배정된 포인트만큼 다름
//
// 난수로 발생될 ID를 확률 포인트를 주어 등록, 이후 발생된 난수는 등록된 ID중 하나임
// 난수 생성시 고른 분포가 아니라 (해당 포인트 / 총 포인트) 만큼의 배율에 따라서 할당
// ex> 난수값(포인트) -> 1(10), 2(5), 3(35) 배정시 총 포인트(10 + 5 + 35 = 50)에서
// - 1이 나올 확률은 10/50 = 20%
// - 2가 나올 확률은 5/50 = 10%
// - 3이 나올 확률은 35/50 = 70%
//
// 랜덤 생성 알고리즘의 세부사항은 "MkCore_MkRandomGenerator.h" 참조
//
// ex>
//	unsigned int testCount = 100000; // 테스트 횟수
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

	// 시드 설정(0 ~ 20000)
	inline void SetSeed(unsigned int seed) { m_RandomGenerator.SetSeed(seed); }

	// ID, 확률 포인트 등록
	// (NOTE) 난수 계산 반영을 위해서는 UpdatePoints() 호출 필요
	inline void RegisterID(const IDType& id, unsigned int point)
	{
		MK_CHECK(point > 0, L"MkBiasedDice에 등록하려는 ID의 확률 포인트가 0임")
			return;

		MK_CHECK(!m_IdTable.Exist(id), L"MkBiasedDice에 이미 존재하는 ID 등록 시도")
			return;

		m_IdTable.Create(id, point);
	}

	// 현재 등록되어 있는 ID를 삭제
	// (NOTE) 난수 계산에 반영을 위해서는 UpdatePoints() 호출 필요
	inline void RemoveID(const IDType& id) { if (m_IdTable.Exist(id)) { m_IdTable.Erase(id); } }

	// 현재 등록되어 있는 ID의 point를 변경
	// (NOTE) 난수 계산에 반영을 위해서는 UpdatePoints() 호출 필요
	inline void ResetPoint(const IDType& id, unsigned int point) { if (m_IdTable.Exist(id)) { m_IdTable[id] = point; } }

	// 현재 등록되어 있는 ID, point로 확률 갱신
	// 등록이 완료되거나, 등록된 ID나 point에 변화가 생겼을 경우 호출 필요
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

	// 현재 등록되어 있는 ID중 하나를 확률 포인트에 따라 랜덤으로 선택해 id에 할당
	// 반환값은 성공여부
	// (NOTE) UpdatePoints() 호출 이후 유효
	inline bool GenerateRandomID(IDType& id)
	{
		unsigned int memberCount = m_PointTable.GetSize();
		MK_CHECK(memberCount > 0, L"초기화되지 않은 MkBiasedDice에서 난수 생성 시도")
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

	// 해당 id의 선택확률을 퍼센트로 반환(0 ~ 100%)
	// (NOTE) UpdatePoints() 호출 이후 유효
	inline float GetHitPercentage(const IDType& id) const
	{
		if ((!m_IdTable.Exist(id)) || (m_TotalPoint == 0))
			return 0.f;

		return (static_cast<float>(m_IdTable[id]) * 100.f / static_cast<float>(m_TotalPoint));
	}

	// 해제
	void Clear(void)
	{
		m_TotalPoint = 0;
		m_IdTable.Clear();
		m_PointTable.Clear();
	}

	// 생성자
	MkBiasedDice() { m_TotalPoint = 0; }
	MkBiasedDice(unsigned int seed) : m_RandomGenerator(seed) { m_TotalPoint = 0; }

protected:

	MkRandomGenerator m_RandomGenerator;

	unsigned int m_TotalPoint;
	MkMap<IDType, unsigned int> m_IdTable;
	MkMap<unsigned int, IDType> m_PointTable;
};

#endif
