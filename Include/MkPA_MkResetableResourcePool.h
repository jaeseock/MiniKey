#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - resetable resource pool
//
// device reset시 reset될 resource 저장소
// 객체 해제에는 관여하지 않음
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

	// 리소스 등록
	bool RegisterResource(MkBaseResetableResource* resource);

	// 등록된 리소스 제거
	void DeleteResource(MkBaseResetableResource* resource);

	// 등록된 모든 리소스들의 UnloadResource() 호출
	void UnloadResources(void);

	// 등록된 모든 리소스들의 ReloadResource() 호출
	void ReloadResources(LPDIRECT3DDEVICE9 device);

	MkResetableResourcePool() : MkSingletonPattern<MkResetableResourcePool>() {}
	~MkResetableResourcePool();

protected:

	MkMap<ID64, MkBaseResetableResource*> m_Pool;
};
