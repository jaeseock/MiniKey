
#include "MkCore_MkCheck.h"
#include "MkS2D_MkBaseResetableResource.h"
#include "MkS2D_MkResetableResourcePool.h"

//------------------------------------------------------------------------------------------------//

bool MkResetableResourcePool::RegisterResource(MkBaseResetableResource* resource)
{
	MK_CHECK(resource != NULL, L"NULL resource µî·Ï")
		return false;

	ID64 id = MK_PTR_TO_ID64(resource);

	if (m_Pool.Exist(id))
		return false;

	m_Pool.Create(id, resource);
	return true;
}

void MkResetableResourcePool::DeleteResource(MkBaseResetableResource* resource)
{
	if (resource != NULL)
	{
		ID64 id = MK_PTR_TO_ID64(resource);

		if (m_Pool.Exist(id))
		{
			m_Pool.Erase(id);
		}
	}
}

void MkResetableResourcePool::UnloadResources(void)
{
	MkMapLooper<ID64, MkBaseResetableResource*> looper(m_Pool);
	MK_STL_LOOP(looper)
	{
		looper.GetCurrentField()->UnloadResource();
	}
}

void MkResetableResourcePool::ReloadResources(LPDIRECT3DDEVICE9 device)
{
	MkMapLooper<ID64, MkBaseResetableResource*> looper(m_Pool);
	MK_STL_LOOP(looper)
	{
		looper.GetCurrentField()->ReloadResource(device);
	}
}

MkResetableResourcePool::~MkResetableResourcePool()
{
	m_Pool.Clear();
}

//------------------------------------------------------------------------------------------------//
