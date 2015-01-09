#pragma once


//------------------------------------------------------------------------------------------------//
// resetable resource ����
// device reset�� �ڵ�ȭ�� unload-reload�� ��ġ�� �������̽�
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
