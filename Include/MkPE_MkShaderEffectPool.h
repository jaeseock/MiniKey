#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - compiled shader effect(fxo) pool
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"

#include "MkPE_MkD3DDefinition.h"

#define MK_SHADER_POOL MkShaderEffectPool::GetInstance()


class MkShaderEffect;

class MkShaderEffectPool
{
public:
	
	void SetUp(LPDIRECT3DDEVICE9 device, const MkArray<MkStr>& filePathList, bool autoRefresh);

	void CheckAndReload(LPDIRECT3DDEVICE9 device, const MkHashStr& name);
	void CheckAndReload(LPDIRECT3DDEVICE9 device, DWORD refreshTime = 1000); // 1 sec

	MkShaderEffect* GetShaderEffect(const MkHashStr& name) const;

	void Clear(void);

	virtual void UnloadResource(void);
	virtual void ReloadResource(void);

	static MkShaderEffectPool& GetInstance(void);

	MkShaderEffectPool();
	virtual ~MkShaderEffectPool() { Clear(); }

protected:

	DWORD m_RefreshTimeStamp;

	MkHashMap<MkHashStr, MkShaderEffect*> m_Pool;
};
