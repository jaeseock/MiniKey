#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - shader effect(fx) pool
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"

#include "MkPA_MkBaseResetableResource.h"


#define MK_SHADER_POOL MkShaderEffectPool::Instance()


class MkPathName;
class MkShaderEffect;

class MkShaderEffectPool : public MkBaseResetableResource, public MkSingletonPattern<MkShaderEffectPool>
{
public:
	
	void SetUp(const MkPathName& shaderDirectory);

	void Clear(void);

	void SetImageRectVertexDeclaration(void);

	MkShaderEffect* GetShaderEffect(const MkHashStr& name) const;

	//------------------------------------------------------------------------------------------------//
	// MkBaseResetableResource
	//------------------------------------------------------------------------------------------------//

	virtual void UnloadResource(void);
	virtual void ReloadResource(LPDIRECT3DDEVICE9 device);

	MkShaderEffectPool();
	virtual ~MkShaderEffectPool() { Clear(); }

protected:

	typedef struct __EffectData
	{
		MkShaderEffect* effect;
		unsigned int fileSize;
		unsigned int fileWrittenTime;
	}
	_EffectData;

protected:

	MkHashMap<MkHashStr, _EffectData> m_Pool;

	// for 2d effect
	LPDIRECT3DVERTEXDECLARATION9 m_VertexDecl;
};
