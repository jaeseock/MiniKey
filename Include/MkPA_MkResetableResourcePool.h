#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - resetable resource pool
//
// device reset�� reset�� resource �����
// ��ü �������� �������� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkPA_MkD3DDefinition.h"

#define MK_RESETABLE_RESPOOL MkResetableResourcePool::Instance()


//------------------------------------------------------------------------------------------------//

class MkBaseResetableResource;

class MkResetableResourcePool : public MkSingletonPattern<MkResetableResourcePool>
{
public:

	// ���ҽ� ���
	bool RegisterResource(MkBaseResetableResource* resource);

	// ��ϵ� ���ҽ� ����
	void DeleteResource(MkBaseResetableResource* resource);

	// ��ϵ� ��� ���ҽ����� UnloadResource() ȣ��
	void UnloadResources(void);

	// ��ϵ� ��� ���ҽ����� ReloadResource() ȣ��
	void ReloadResources(LPDIRECT3DDEVICE9 device);

	MkResetableResourcePool() : MkSingletonPattern<MkResetableResourcePool>() {}
	~MkResetableResourcePool();

protected:

	MkMap<ID64, MkBaseResetableResource*> m_Pool;
};
