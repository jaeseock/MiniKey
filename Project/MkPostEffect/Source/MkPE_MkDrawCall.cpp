
#include "MkPE_MkBitmapPool.h"
#include "MkPE_MkShaderEffect.h"
#include "MkPE_MkShaderEffectPool.h"
#include "MkPE_MkPostEffectRenderer.h"
#include "MkPE_MkDrawCall.h"


//------------------------------------------------------------------------------------------------//

void MkDrawCall::SetAlpha(float alpha)
{
	m_Alpha = static_cast<DWORD>(Clamp<float>(alpha, 0.f, 1.f) * 255.f);
}

float MkDrawCall::GetAlpha(void) const
{
	return (static_cast<float>(m_Alpha) / 255.f);
}

bool MkDrawCall::SetTexture(int index, const MkPathName& filePath)
{
	if ((index < 0) || (index >= MKDEF_MAX_TEXTURE_COUNT))
		return false;

	MkBaseTexture* texture = MK_BITMAP_POOL.GetBitmapTexture(filePath);
	if (texture == NULL)
		return false;

	return SetTexture(index, texture);
}

bool MkDrawCall::SetTexture(int index, MkBaseTexture* texture)
{
	if ((index < 0) || (index >= MKDEF_MAX_TEXTURE_COUNT))
		return false;

	m_Texture[index] = texture;
	return true;
}

bool MkDrawCall::SetTexture(int index, int sourceOutputIndex)
{
	if ((index < 0) || (index >= MKDEF_MAX_TEXTURE_COUNT) || (sourceOutputIndex < 0) || (sourceOutputIndex >= MKDEF_MAX_TEXTURE_COUNT))
		return false;

	const MkRenderToTexture* texture = MK_POST_EFFECT.GetSourceRenderTarget().GetTargetTexture(sourceOutputIndex);
	if (texture == NULL)
		return false;

	return SetTexture(index, const_cast<MkRenderToTexture*>(texture));
}

MkBaseTexture* MkDrawCall::GetTexture(int index) const
{
	return ((index < 0) || (index >= MKDEF_MAX_TEXTURE_COUNT)) ? NULL : m_Texture[index];
}

bool MkDrawCall::SetShaderEffect(const MkHashStr& name)
{
	if (m_EffectName == name)
		return true;

	bool validEffect = (MK_SHADER_POOL.GetShaderEffect(name) != NULL);
	if (validEffect)
	{
		m_EffectName = name;
		m_Technique.Clear(); // default technique
	}
	return validEffect;
}

bool MkDrawCall::SetTechnique(const MkHashStr& technique)
{
	MkShaderEffect* effect = MK_SHADER_POOL.GetShaderEffect(m_EffectName);
	if (effect == NULL)
		return false;

	if (technique.Empty())
	{
		m_Technique.Clear();
		return true;
	}

	if (effect->IsValidTechnique(technique))
	{
		m_Technique = technique;
		return true;
	}
	return false;
}

void MkDrawCall::SetUDP(const MkHashStr& key, const D3DXVECTOR4& value)
{
	MK_INDEXING_LOOP(m_UDP, i)
	{
		_UDP_Unit& udp = m_UDP[i];
		if (udp.key == key)
		{
			udp.value = value;
			return;
		}
	}

	_UDP_Unit& udp = m_UDP.PushBack();
	udp.key = key;
	udp.value = value;
}

bool MkDrawCall::GetUDP(const MkHashStr& key, D3DXVECTOR4& buffer) const
{
	MK_INDEXING_LOOP(m_UDP, i)
	{
		const _UDP_Unit& udp = m_UDP[i];
		if (udp.key == key)
		{
			buffer = udp.value;
			return true;
		}
	}
	return false;
}

void MkDrawCall::Clear(void)
{
	m_Alpha = 0xff;

	for (int i=0; i<MKDEF_MAX_TEXTURE_COUNT; ++i)
	{
		m_Texture[i] = NULL;
	}

	m_EffectName.Clear();
	m_Technique.Clear();
	m_UDP.Clear();
}

MkDrawCall::MkDrawCall()
{
	Clear();
}

void MkDrawCall::__Draw(LPDIRECT3DDEVICE9 device, const MkFloat2& screenSize)
{
	if (device != NULL)
	{
		// 공통
		device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0));

		unsigned int stride = sizeof(float) * 5;
		MkByteArray vertexData(stride * 6);
		_FillVertexData(screenSize, vertexData);
		
		// shader effect가 존재하지 않으면 기본 렌더링
		MkShaderEffect* effect = MK_SHADER_POOL.GetShaderEffect(m_EffectName);
		if (effect == NULL)
		{
			device->SetVertexShader(NULL);
			device->SetPixelShader(NULL);

			if (m_Texture[0] != NULL)
			{
				m_Texture[0]->UpdateRenderState(m_Alpha);
			}

			device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, vertexData.GetPtr(), stride);
		}
		// shader effect가 존재하면 해당 이펙트로 렌더링
		else
		{
			unsigned int passCount = effect->BeginTechnique(*this, screenSize);
			for (unsigned int i=0; i<passCount; ++i)
			{
				effect->BeginPass(i);
				device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, vertexData.GetPtr(), stride);
				effect->EndPass();
			}
			effect->EndTechnique();
		}
	}
}

void MkDrawCall::_FillVertexData(MkFloatRect::ePointName pn, const MkFloat2& size, MkByteArray& buffer) const
{
	MkArray<float> data;
	data.Fill(5);

	switch (pn)
	{
	case MkFloatRect::eLeftTop:
		data[0] = 0.f;
		data[1] = size.y;
		data[3] = 0.f;
		data[4] = 0.f;
		break;

	case MkFloatRect::eRightTop:
		data[0] = size.x;
		data[1] = size.y;
		data[3] = 1.f;
		data[4] = 0.f;
		break;

	case MkFloatRect::eLeftBottom:
		data[0] = 0.f;
		data[1] = 0.f;
		data[3] = 0.f;
		data[4] = 1.f;
		break;

	case MkFloatRect::eRightBottom:
		data[0] = size.x;
		data[1] = 0.f;
		data[3] = 1.f;
		data[4] = 1.f;
		break;
	}

	data[2] = 10.f;

	MkByteArrayHelper<float>::PushBack(buffer, data);
}

void MkDrawCall::_FillVertexData(const MkFloat2& size, MkByteArray& buffer) const
{
	// xyz, uv
	// 0 --- 1
	// |  /  |
	// 2 --- 3
	// seq : 0, 1, 2, 2, 1, 3

	_FillVertexData(MkFloatRect::eLeftTop, size, buffer); // 0
	_FillVertexData(MkFloatRect::eRightTop, size, buffer); // 1
	_FillVertexData(MkFloatRect::eLeftBottom, size, buffer); // 2
	_FillVertexData(MkFloatRect::eLeftBottom, size, buffer); // 2
	_FillVertexData(MkFloatRect::eRightTop, size, buffer); // 1
	_FillVertexData(MkFloatRect::eRightBottom, size, buffer); // 3
}

//------------------------------------------------------------------------------------------------//
