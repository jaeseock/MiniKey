
#include "MkPA_MkBaseTexture.h"
#include "MkPA_MkShaderEffectAssignPack.h"


//------------------------------------------------------------------------------------------------//

void MkShaderEffectAssignPack::Clear(void)
{
	ClearWorldViewProjection();
	SetAlpha(1.f);

	SetDiffuseTexture(NULL);
	SetShaderTexture0(NULL);
	SetShaderTexture1(NULL);
	SetShaderTexture2(NULL);

	D3DXVECTOR4 empty(0.f, 0.f, 0.f, 0.f);
	SetUDP0(empty);
	SetUDP1(empty);
	SetUDP2(empty);
	SetUDP3(empty);
}

void MkShaderEffectAssignPack::SetWorldViewProjection(const D3DXMATRIX& wvp)
{
	m_Assign[MkShaderEffect::eS_WorldViewProjection] = true;
	m_WorldViewProjection = wvp;
}

void MkShaderEffectAssignPack::ClearWorldViewProjection(void)
{
	m_Assign[MkShaderEffect::eS_WorldViewProjection] = false;
	D3DXMatrixIdentity(&m_WorldViewProjection);
}

void MkShaderEffectAssignPack::SetAlpha(float alpha)
{
	m_Assign[MkShaderEffect::eS_Alpha] = (alpha < 1.f);
	m_Alpha = alpha;
}

void MkShaderEffectAssignPack::SetDiffuseTexture(MkBaseTexture* tex)
{
	_SetTexture(MkShaderEffect::eS_DiffuseTexture, tex);
}

LPDIRECT3DTEXTURE9 MkShaderEffectAssignPack::GetDiffuseTexture(void) const
{
	return _GetTexture(m_DiffuseTexture);
}

void MkShaderEffectAssignPack::SetShaderTexture0(MkBaseTexture* tex)
{
	_SetTexture(MkShaderEffect::eS_ShaderTexture0, tex);
}

LPDIRECT3DTEXTURE9 MkShaderEffectAssignPack::GetShaderTexture0(void) const
{
	return _GetTexture(m_ShaderTexture0);
}

void MkShaderEffectAssignPack::SetShaderTexture1(MkBaseTexture* tex)
{
	_SetTexture(MkShaderEffect::eS_ShaderTexture1, tex);
}

LPDIRECT3DTEXTURE9 MkShaderEffectAssignPack::GetShaderTexture1(void) const
{
	return _GetTexture(m_ShaderTexture1);
}

void MkShaderEffectAssignPack::SetShaderTexture2(MkBaseTexture* tex) { _SetTexture(MkShaderEffect::eS_ShaderTexture2, tex); }
LPDIRECT3DTEXTURE9 MkShaderEffectAssignPack::GetShaderTexture2(void) const { return _GetTexture(m_ShaderTexture2); }

void MkShaderEffectAssignPack::SetUDP0(const D3DXVECTOR4& value)
{
	m_Assign[MkShaderEffect::eS_UDP0] = ((value.x != 0.f) || (value.y != 0.f) || (value.z != 0.f) || (value.w != 0.f));
	m_UDP0 = value;
}

void MkShaderEffectAssignPack::SetUDP1(const D3DXVECTOR4& value)
{
	m_Assign[MkShaderEffect::eS_UDP1] = ((value.x != 0.f) || (value.y != 0.f) || (value.z != 0.f) || (value.w != 0.f));
	m_UDP1 = value;
}

void MkShaderEffectAssignPack::SetUDP2(const D3DXVECTOR4& value)
{
	m_Assign[MkShaderEffect::eS_UDP2] = ((value.x != 0.f) || (value.y != 0.f) || (value.z != 0.f) || (value.w != 0.f));
	m_UDP2 = value;
}

void MkShaderEffectAssignPack::SetUDP3(const D3DXVECTOR4& value)
{
	m_Assign[MkShaderEffect::eS_UDP3] = ((value.x != 0.f) || (value.y != 0.f) || (value.z != 0.f) || (value.w != 0.f));
	m_UDP3 = value;
}

void MkShaderEffectAssignPack::_SetTexture(MkShaderEffect::eSemantic semantic, MkBaseTexture* tex)
{
	switch (semantic)
	{
	case MkShaderEffect::eS_DiffuseTexture:
		m_DiffuseTexture = tex;
		m_Assign[MkShaderEffect::eS_DiffuseTexSize] = m_Assign[semantic] = (tex != NULL);
		m_DiffuseTexSize.x = (tex == NULL) ? 0.f : static_cast<float>(tex->GetSize().x);
		m_DiffuseTexSize.y = (tex == NULL) ? 0.f : static_cast<float>(tex->GetSize().y);
		break;

	case MkShaderEffect::eS_ShaderTexture0:
		m_ShaderTexture0 = tex;
		m_Assign[MkShaderEffect::eS_ShaderTexSize0] = m_Assign[semantic] = (tex != NULL);
		m_ShaderTexSize0.x = (tex == NULL) ? 0.f : static_cast<float>(tex->GetSize().x);
		m_ShaderTexSize0.y = (tex == NULL) ? 0.f : static_cast<float>(tex->GetSize().y);
		break;

	case MkShaderEffect::eS_ShaderTexture1:
		m_ShaderTexture1 = tex;
		m_Assign[MkShaderEffect::eS_ShaderTexSize1] = m_Assign[semantic] = (tex != NULL);
		m_ShaderTexSize1.x = (tex == NULL) ? 0.f : static_cast<float>(tex->GetSize().x);
		m_ShaderTexSize1.y = (tex == NULL) ? 0.f : static_cast<float>(tex->GetSize().y);
		break;

	case MkShaderEffect::eS_ShaderTexture2:
		m_ShaderTexture2 = tex;
		m_Assign[MkShaderEffect::eS_ShaderTexSize2] = m_Assign[semantic] = (tex != NULL);
		m_ShaderTexSize2.x = (tex == NULL) ? 0.f : static_cast<float>(tex->GetSize().x);
		m_ShaderTexSize2.y = (tex == NULL) ? 0.f : static_cast<float>(tex->GetSize().y);
		break;
	}
}

LPDIRECT3DTEXTURE9 MkShaderEffectAssignPack::_GetTexture(const MkBaseTexture* tex) const
{
	return (tex == NULL) ? NULL : tex->GetTexture();
}

//------------------------------------------------------------------------------------------------//
