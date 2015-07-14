#pragma once


//------------------------------------------------------------------------------------------------//
// shader effect assign pack
// - effect에 기본 값 설정용
// - object마다 고유 값 할당용
//
// MkShaderEffect::eSemantic에 직결 됨
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkShaderEffect.h"


class MkBaseTexture;

class MkShaderEffectAssignPack
{
public:

	inline bool GetAssigned(MkShaderEffect::eSemantic semantic) const { return m_Assign[semantic]; }
	void Clear(void);

	// eS_WorldViewProjection. default identity
	void SetWorldViewProjection(const D3DXMATRIX& wvp);
	inline const D3DXMATRIX& GetWorldViewProjection(void) const { return m_WorldViewProjection; }
	void ClearWorldViewProjection(void);

	// eS_Alpha
	void SetAlpha(float alpha);
	inline float GetAlpha(void) const { return m_Alpha; }

	// eS_DiffuseTexture
	void SetDiffuseTexture(MkBaseTexture* tex);
	LPDIRECT3DTEXTURE9 GetDiffuseTexture(void) const;

	// eS_ShaderTexture0
	void SetShaderTexture0(MkBaseTexture* tex);
	LPDIRECT3DTEXTURE9 GetShaderTexture0(void) const;

	// eS_ShaderTexture1
	void SetShaderTexture1(MkBaseTexture* tex);
	LPDIRECT3DTEXTURE9 GetShaderTexture1(void) const;

	// eS_ShaderTexture2
	void SetShaderTexture2(MkBaseTexture* tex);
	LPDIRECT3DTEXTURE9 GetShaderTexture2(void) const;

	// eS_DiffuseTexSize, eS_ShaderTexSize0 ~ 2
	inline const D3DXVECTOR4& GetDiffuseTexSize(void) const { return m_DiffuseTexSize; }
	inline const D3DXVECTOR4& GetShaderTexSize0(void) const { return m_ShaderTexSize0; }
	inline const D3DXVECTOR4& GetShaderTexSize1(void) const { return m_ShaderTexSize1; }
	inline const D3DXVECTOR4& GetShaderTexSize2(void) const { return m_ShaderTexSize2; }

	// eS_UDP0
	void SetUDP0(const D3DXVECTOR4& value);
	inline const D3DXVECTOR4& GetUDP0(void) const { return m_UDP0; }

	// eS_UDP1
	void SetUDP1(const D3DXVECTOR4& value);
	inline const D3DXVECTOR4& GetUDP1(void) const { return m_UDP1; }

	// eS_UDP2
	void SetUDP2(const D3DXVECTOR4& value);
	inline const D3DXVECTOR4& GetUDP2(void) const { return m_UDP2; }

	// eS_UDP3
	void SetUDP3(const D3DXVECTOR4& value);
	inline const D3DXVECTOR4& GetUDP3(void) const { return m_UDP3; }

	MkShaderEffectAssignPack() { Clear(); }
	~MkShaderEffectAssignPack() {}

protected:

	void _SetTexture(MkShaderEffect::eSemantic semantic, MkBaseTexture* tex);
	LPDIRECT3DTEXTURE9 _GetTexture(const MkBaseTexture* tex) const;

protected:

	bool m_Assign[MkShaderEffect::eS_Max];

	D3DXMATRIX m_WorldViewProjection; // eS_WorldViewProjection

	float m_Alpha; // eS_Alpha

	MkBaseTexture* m_DiffuseTexture; // eS_DiffuseTexture
	MkBaseTexture* m_ShaderTexture0; // eS_ShaderTexture0
	MkBaseTexture* m_ShaderTexture1; // eS_ShaderTexture1
	MkBaseTexture* m_ShaderTexture2; // eS_ShaderTexture2

	D3DXVECTOR4 m_DiffuseTexSize; // eS_DiffuseTexSize
	D3DXVECTOR4 m_ShaderTexSize0; // eS_ShaderTexSize0
	D3DXVECTOR4 m_ShaderTexSize1; // eS_ShaderTexSize1
	D3DXVECTOR4 m_ShaderTexSize2; // eS_ShaderTexSize2

	D3DXVECTOR4 m_UDP0; // eS_UDP0
	D3DXVECTOR4 m_UDP1; // eS_UDP1
	D3DXVECTOR4 m_UDP2; // eS_UDP2
	D3DXVECTOR4 m_UDP3; // eS_UDP3
};
