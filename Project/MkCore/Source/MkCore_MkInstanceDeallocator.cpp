
#include "MkCore_MkInstanceDeallocator.h"

//------------------------------------------------------------------------------------------------//

void MkInstanceDeallocator::RegisterInstance(MkVirtualInstance* instance)
{
	if (instance != NULL)
	{
		m_InstanceList.push_back(instance);
	}
}

void MkInstanceDeallocator::ClearFrontToRear(void)
{
	if (!m_InstanceList.empty())
	{
		std::vector<MkVirtualInstance*>::iterator itr;
		for (itr = m_InstanceList.begin(); itr != m_InstanceList.end(); ++itr)
		{
			if (*itr != NULL)
			{
				delete (*itr);
			}
		}

		m_InstanceList.clear();
	}
}

void MkInstanceDeallocator::ClearRearToFront(void)
{
	if (!m_InstanceList.empty())
	{
		std::vector<MkVirtualInstance*>::reverse_iterator itr;
		for (itr = m_InstanceList.rbegin(); itr != m_InstanceList.rend(); ++itr)
		{
			if (*itr != NULL)
			{
				delete (*itr);
			}
		}

		m_InstanceList.clear();
	}
}

MkInstanceDeallocator::MkInstanceDeallocator(unsigned int initSize)
{
	m_InstanceList.reserve(initSize);
}

//------------------------------------------------------------------------------------------------//
