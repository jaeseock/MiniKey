#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - compiled shader effect(fxo) pool
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"

#include "MkPA_MkBaseResetableResource.h"


#define MK_SHADER_POOL MkShaderEffectPool::Instance()


class MkShaderEffect;

class MkShaderEffectPool : public MkBaseResetableResource, public MkSingletonPattern<MkShaderEffectPool>
{
public:
	
	void SetUp(const MkArray<MkStr>& filePathList);

	MkShaderEffect* GetShaderEffect(const MkHashStr& name) const;

	void Clear(void);

	//void SetImageRectVertexDeclaration(void);

	//------------------------------------------------------------------------------------------------//
	// MkBaseResetableResource
	//------------------------------------------------------------------------------------------------//

	virtual void UnloadResource(void);
	virtual void ReloadResource(LPDIRECT3DDEVICE9 device);

	MkShaderEffectPool();
	virtual ~MkShaderEffectPool() { Clear(); }

protected:

	MkHashMap<MkHashStr, MkShaderEffect*> m_Pool;

	// for 2d effect
	//LPDIRECT3DVERTEXDECLARATION9 m_VertexDecl;
};
