#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkGlobalFunction.h"
#include "MkCore_MkArray.h"


class GameHitPointGage
{
public:

	inline void Initialize(int maxCap)
	{
		m_MaxCap = maxCap;
		m_LastDurability = m_CurrDurability = m_MaxCap;
		m_LastHitPoint = m_CurrHitPoint = m_MaxCap;
	}

	inline void AddMaxCap(int offset)
	{
		if (offset > 0)
		{
			m_MaxCap += offset;
			m_CurrDurability += offset;
			m_CurrHitPoint += offset;
		}
	}

	inline void RemoveMaxCap(int offset)
	{
		if (offset > 0)
		{
			m_MaxCap -= offset;
			if (m_CurrDurability > m_MaxCap)
			{
				m_CurrDurability = m_MaxCap;
			}
			if (m_CurrHitPoint > m_CurrDurability)
			{
				m_CurrHitPoint = m_CurrDurability;
			}
		}
	}

	inline int GetMaxCap(void) const { return m_MaxCap; }

	inline int GetLastDurability(void) const { return m_LastDurability; }
	inline int GetCurrDurability(void) const { return m_CurrDurability; }

	inline int GetLastHitPoint(void) const { return m_LastHitPoint; }
	inline int GetCurrHitPoint(void) const { return m_CurrHitPoint; }

	inline void AddDurability(int value) { m_DurabilityModifier.PushBack(value); }
	inline void AddHitPoint(int value) { m_HitPointModifier.PushBack(value); }

	inline void AddDamage(int value, int damToDurPerc)
	{
		int durDamage = ApplyPercentage<int, int>(value, damToDurPerc);
		int hpDamage = value;

		if (durDamage > 0)
		{
			AddDurability(-durDamage);
			hpDamage -= durDamage;
		}

		if (hpDamage > 0)
		{
			AddHitPoint(-hpDamage);
		}
		
	}

	inline void UpdateGage(void)
	{
		m_LastDurability = m_CurrDurability;
		m_LastHitPoint = m_CurrHitPoint;
		
		if (!m_DurabilityModifier.Empty())
		{
			int addition = static_cast<int>(0);
			MK_INDEXING_LOOP(m_DurabilityModifier, i)
			{
				addition += m_DurabilityModifier[i];
			}

			m_CurrDurability = Clamp<int>(m_CurrDurability + addition, 0, m_MaxCap);

			if (m_CurrHitPoint > m_CurrDurability)
			{
				m_CurrHitPoint = m_CurrDurability;
			}
		}

		if (!m_HitPointModifier.Empty())
		{
			int addition = static_cast<int>(0);
			MK_INDEXING_LOOP(m_HitPointModifier, i)
			{
				addition += m_HitPointModifier[i];
			}

			m_CurrHitPoint = Clamp<int>(m_CurrHitPoint + addition, 0, m_CurrDurability);
		}
	}

	inline void Clear(void)
	{
		m_DurabilityModifier.Clear();
		m_HitPointModifier.Clear();
	}

	GameHitPointGage() {}
	~GameHitPointGage() { Clear(); }

protected:

	int m_MaxCap;

	int m_LastDurability;
	int m_CurrDurability;

	int m_LastHitPoint;
	int m_CurrHitPoint;

	MkArray<int> m_DurabilityModifier;
	MkArray<int> m_HitPointModifier;
};
