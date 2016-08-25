#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - D3D render state
// 2D라는 특성 때문에 많은 설정들이 생략 가능
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkValueDecision.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkPA_MkD3DDefinition.h"


#define MK_RENDER_STATE MkRenderStateSetter::Instance()

//------------------------------------------------------------------------------------------------//

class MkRenderStateSetter : public MkSingletonPattern<MkRenderStateSetter>
{
public:

	// 렌더링 시작 선언
	void Begin(void);

	// fvf
	void UpdateFVF(DWORD fvf);

	// MkBaseTexture
	void UpdateBlendOp(DWORD alphaBlend, DWORD objectAlpha, DWORD srcBlend, DWORD dstBlend, DWORD alphaTest, DWORD alphaRef);

	void UpdateBaseTexture(DWORD sampler,
		LPDIRECT3DTEXTURE9 texture, unsigned int coordinate, _D3DTEXTUREFILTERTYPE minmag, unsigned int maxAnisotropic,
		_D3DTEXTUREFILTERTYPE mipmap, _D3DTEXTUREADDRESS address, D3DCOLOR borderColor);

	void UpdateTextureOperation(DWORD sampler, DWORD cOP, DWORD cA1, DWORD cA2, DWORD aOP, DWORD aA1, DWORD aA2);

	void ClearSampler1(void);

	// public
	void UpdateViewProjectionTransform(const D3DXMATRIX& view, const D3DXMATRIX& projection);

	MkRenderStateSetter();
	virtual ~MkRenderStateSetter() {}

protected:

	typedef struct __TextureSet
	{
		MkValueDecision<LPDIRECT3DTEXTURE9> m_Texture;
		MkValueDecision<unsigned int> m_Coordinate;
		MkValueDecision<_D3DTEXTUREFILTERTYPE> m_MinMagFilter;
		MkValueDecision<unsigned int> m_MaxAnisotropic;
		MkValueDecision<_D3DTEXTUREFILTERTYPE> m_MipmapFilter;
		MkValueDecision<_D3DTEXTUREADDRESS> m_TextureAddress;
		MkValueDecision<D3DCOLOR> m_BorderColor;

		MkValueDecision<DWORD> m_ColorOP;
		MkValueDecision<DWORD> m_ColorA1;
		MkValueDecision<DWORD> m_ColorA2;
		MkValueDecision<DWORD> m_AlphaOP;
		MkValueDecision<DWORD> m_AlphaA1;
		MkValueDecision<DWORD> m_AlphaA2;
	}
	_TextureSet;

protected:

	// transform
	MkValueDecision<D3DXMATRIX> m_ViewMatrix;
	MkValueDecision<D3DXMATRIX> m_ProjectionMatrix;

	// fvf
	MkValueDecision<DWORD> m_FVF;

	// material
	MkValueDecision<DWORD> m_AlphaBlend;
	MkValueDecision<DWORD> m_ObjectAlpha;
	MkValueDecision<DWORD> m_SrdBlend;
	MkValueDecision<DWORD> m_DstBlend;
	MkValueDecision<DWORD> m_AlphaFunc;
	MkValueDecision<DWORD> m_AlphaTest;
	MkValueDecision<DWORD> m_AlphaRef;
	
	// texture
	_TextureSet m_TextureSet[2]; // sampler 0, 1

	// const
	D3DMATERIAL9 m_D3DMaterial;
};
