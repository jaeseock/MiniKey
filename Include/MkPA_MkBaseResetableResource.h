#pragma once


//------------------------------------------------------------------------------------------------//
// resetable resource 원형
// device reset시 자동화된 unload-reload를 거치는 인터페이스
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkD3DDefinition.h"


class MkBaseResetableResource
{
public:

	virtual void UnloadResource(void) {}
	virtual void ReloadResource(LPDIRECT3DDEVICE9 device) {}

	MkBaseResetableResource() {}
	virtual ~MkBaseResetableResource() {}
};
