#pragma once

//------------------------------------------------------------------------------------------------//
// modifier owner bandwidth
// - 0 ~ 0xff : mage(255)
// - 0x100 ~ 0x1ff : troop(255)
// - 0x200 ~ 0xfff : reserved
// - 0x1000 ~ 0xffff : enchant(61439)
// - 0x10000 ~ 0xffffffff : reserved
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkGlobalFunction.h"
#include "MkCore_MkMap.h"


template <class TargetClass>
class GameStateType
{
public:

	inline void Initialize(const TargetClass& defValue, const TargetClass& minCap, const TargetClass& maxCap)
	{
		SetUp(defValue);
		m_MinCap = minCap;
		m_MaxCap = maxCap;
	}

	inline void SetUp(const TargetClass& defValue)
	{
		m_Addition = static_cast<TargetClass>(0);
		m_Final = m_Default = defValue;
		m_Modified = false;
	}

	inline const TargetClass& GetMinCap(void) const { return m_MinCap; }
	inline const TargetClass& GetMaxCap(void) const { return m_MaxCap; }

	inline const TargetClass& GetAddition(void) const { return m_Addition; }
	inline const TargetClass& GetFinal(void) const { return m_Final; }

	inline void AttachModifier(unsigned int modifierOwnerID, const TargetClass& value)
	{
		TargetClass& mod = m_Modifier.Exist(modifierOwnerID) ? m_Modifier[modifierOwnerID] : m_Modifier.Create(modifierOwnerID);
		mod = value;
		m_Modified = true;
	}

	inline void DetachModifier(unsigned int modifierOwnerID)
	{
		if (m_Modifier.Exist(modifierOwnerID))
		{
			m_Modifier.Erase(modifierOwnerID);
			m_Modified = true;
		}
	}

	inline void UpdateState(void)
	{
		if (m_Modified)
		{
			m_Final = m_Default;
			m_Addition = static_cast<TargetClass>(0);

			if (!m_Modifier.Empty())
			{
				MkConstMapLooper<unsigned int, TargetClass> looper(m_Modifier);
				MK_STL_LOOP(looper)
				{
					m_Addition += looper.GetCurrentField();
				}

				m_Final = Clamp<TargetClass>(m_Final + m_Addition, m_MinCap, m_MaxCap);
			}

			m_Modified = false;
		}
	}

	GameStateType() {}
	~GameStateType() {}

protected:

	TargetClass m_Default;
	TargetClass m_MinCap;
	TargetClass m_MaxCap;

	MkMap<unsigned int, TargetClass> m_Modifier; // modifier owner ID, value
	TargetClass m_Addition;
	bool m_Modified;

	TargetClass m_Final;
};

typedef GameStateType<int> IntStateType;