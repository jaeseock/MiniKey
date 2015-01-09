
#include "MkS2D_MkRenderStateSetter.h"


#define MK_UPDATE_RENDER_FUNCTION(lastest, value, func) \
	lastest.Commit(value); \
	if (lastest.Changed()) m_Device->func(lastest.GetDecision());

#define MK_UPDATE_RENDER_TRANSFORM(target, lastest, value) \
	lastest.Commit(value); \
	if (lastest.Changed()) m_Device->SetTransform(target, &lastest.GetDecision());

#define MK_UPDATE_RENDER_STATE(target, lastest, value) \
	lastest.Commit(value); \
	if (lastest.Changed()) m_Device->SetRenderState(target, lastest.GetDecision());

#define MK_UPDATE_SAMPLER_FUNCTION(lastest, value, func, sampler) \
	lastest.Commit(value); \
	if (lastest.Changed()) m_Device->func(sampler, lastest.GetDecision());

#define MK_UPDATE_TEXTURE_STAGE_STATE(target, lastest, value, sampler) \
	lastest.Commit(value); \
	if (lastest.Changed()) m_Device->SetTextureStageState(sampler, target, lastest.GetDecision());

#define MK_UPDATE_SAMPLER_STATE(target, lastest, value, sampler) \
	lastest.Commit(value); \
	if (lastest.Changed()) m_Device->SetSamplerState(sampler, target, lastest.GetDecision());

#define MK_UPDATE_SAMPLER_STATE_DUAL(target1, target2, lastest, value, sampler) \
	lastest.Commit(value); \
	if (lastest.Changed()) { \
		m_Device->SetSamplerState(sampler, target1, lastest.GetDecision()); \
		m_Device->SetSamplerState(sampler, target2, lastest.GetDecision()); }


//------------------------------------------------------------------------------------------------//

bool MkRenderStateSetter::SetUp(LPDIRECT3DDEVICE9 device)
{
	m_D3DMaterial.Diffuse.r = 1.f; m_D3DMaterial.Diffuse.g = 1.f; m_D3DMaterial.Diffuse.b = 1.f;
	m_D3DMaterial.Ambient.r = 0.f; m_D3DMaterial.Ambient.g = 0.f; m_D3DMaterial.Ambient.b = 0.f; m_D3DMaterial.Ambient.a = 1.f;
	m_D3DMaterial.Specular.r = 0.f; m_D3DMaterial.Specular.g = 0.f; m_D3DMaterial.Specular.b = 0.f; m_D3DMaterial.Specular.a = 1.f;
	m_D3DMaterial.Emissive.r = 0.f; m_D3DMaterial.Emissive.g = 0.f; m_D3DMaterial.Emissive.b = 0.f; m_D3DMaterial.Emissive.a = 1.f;
	m_D3DMaterial.Power = 1.f;

	m_Device = device;
	return (m_Device != NULL);
}

void MkRenderStateSetter::Begin(void)
{
	m_WorldMatrix.SetUp();
	m_ViewMatrix.SetUp();
	m_ProjectionMatrix.SetUp();

	m_FVF.SetUp();
	m_VertexBuffer.SetUp();
	m_Stride.SetUp();
	m_IndexBuffer.SetUp();

	m_AlphaBlend.SetUp();
	m_ObjectAlpha.SetUp();
	m_SrdBlend.SetUp();
	m_DstBlend.SetUp();
	m_AlphaFunc.SetUp();
	m_AlphaTest.SetUp();
	m_AlphaRef.SetUp();
	
	m_TextureSet[0].m_Texture.SetUp();
	m_TextureSet[0].m_Coordinate.SetUp();
	m_TextureSet[0].m_MinMagFilter.SetUp();
	m_TextureSet[0].m_MaxAnisotropic.SetUp();
	m_TextureSet[0].m_MipmapFilter.SetUp();
	m_TextureSet[0].m_TextureAddress.SetUp();
	m_TextureSet[0].m_BorderColor.SetUp();

	m_TextureSet[0].m_ColorOP.SetUp();
	m_TextureSet[0].m_ColorA1.SetUp();
	m_TextureSet[0].m_ColorA2.SetUp();
	m_TextureSet[0].m_AlphaOP.SetUp();
	m_TextureSet[0].m_AlphaA1.SetUp();
	m_TextureSet[0].m_AlphaA2.SetUp();

	m_TextureSet[1].m_Texture.SetUp();
	m_TextureSet[1].m_Coordinate.SetUp();
	m_TextureSet[1].m_MinMagFilter.SetUp();
	m_TextureSet[1].m_MaxAnisotropic.SetUp();
	m_TextureSet[1].m_MipmapFilter.SetUp();
	m_TextureSet[1].m_TextureAddress.SetUp();
	m_TextureSet[1].m_BorderColor.SetUp();

	m_TextureSet[1].m_ColorOP.SetUp();
	m_TextureSet[1].m_ColorA1.SetUp();
	m_TextureSet[1].m_ColorA2.SetUp();
	m_TextureSet[1].m_AlphaOP.SetUp();
	m_TextureSet[1].m_AlphaA1.SetUp();
	m_TextureSet[1].m_AlphaA2.SetUp();
	
	// global
	m_Device->SetRenderState(D3DRS_AMBIENT, 0xffffffff);
	m_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_Device->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	m_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_Device->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
	m_Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	m_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	m_Device->SetMaterial(&m_D3DMaterial);

	// 대부분이 알파 오브젝트고 소팅 순서를 완벽하게 정해서 그려주므로 의미 없음
	m_Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS); // z read off
	m_Device->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_FALSE); // z write off

	m_Device->SetIndices(NULL); // D3DPT_TRIANGLELIST만 사용하므로 index buffer가 필요 없음
	m_Device->SetVertexShader(NULL);
	m_Device->SetPixelShader(NULL);
}

void MkRenderStateSetter::UpdateGeometryBuffer(DWORD fvf, LPDIRECT3DVERTEXBUFFER9 vertexBuffer, unsigned int stride)
{
	if (m_Device != NULL)
	{
		MK_UPDATE_RENDER_FUNCTION(m_FVF, fvf, SetFVF);

		//m_VertexBuffer.Commit(vertexBuffer);
		//m_Stride.Commit(stride);
		//if (m_VertexBuffer.Changed() || m_Stride.Changed())
		//{
		//	m_Device->SetStreamSource(0, m_VertexBuffer.GetDecision(), 0, m_Stride.GetDecision());
		//}
	}
}

void MkRenderStateSetter::UpdateBlendOp(DWORD alphaBlend, DWORD objectAlpha, DWORD srcBlend, DWORD dstBlend, DWORD alphaTest, DWORD alphaRef)
{
	if (m_Device != NULL)
	{
		MK_UPDATE_RENDER_STATE(D3DRS_ALPHABLENDENABLE, m_AlphaBlend, alphaBlend);

		if (m_AlphaBlend.GetDecision() == TRUE)
		{
			m_ObjectAlpha.Commit(objectAlpha);
			if (m_ObjectAlpha.Changed())
			{
				m_Device->SetRenderState(D3DRS_TEXTUREFACTOR, (m_ObjectAlpha.GetDecision() << 24) + 0x00ffffff);
			}

			MK_UPDATE_RENDER_STATE(D3DRS_SRCBLEND, m_SrdBlend, srcBlend);
			MK_UPDATE_RENDER_STATE(D3DRS_DESTBLEND, m_DstBlend, dstBlend);

			MK_UPDATE_RENDER_STATE(D3DRS_ALPHATESTENABLE, m_AlphaTest, alphaTest);
			if (m_AlphaTest.GetDecision() == TRUE)
			{
				MK_UPDATE_RENDER_STATE(D3DRS_ALPHAFUNC, m_AlphaFunc, D3DCMP_GREATEREQUAL);
				MK_UPDATE_RENDER_STATE(D3DRS_ALPHAREF, m_AlphaRef, alphaRef);
			}
			else
			{
				MK_UPDATE_RENDER_STATE(D3DRS_ALPHAFUNC, m_AlphaFunc, D3DCMP_ALWAYS);
			}
		}
	}
}

void MkRenderStateSetter::UpdateBaseTexture
(DWORD sampler, LPDIRECT3DTEXTURE9 texture, unsigned int coordinate, _D3DTEXTUREFILTERTYPE minmag, unsigned int maxAnisotropic,
 _D3DTEXTUREFILTERTYPE mipmap, _D3DTEXTUREADDRESS address, D3DCOLOR borderColor)
{
	if (m_Device != NULL)
	{
		_TextureSet& currTexSet = m_TextureSet[sampler];

		MK_UPDATE_SAMPLER_FUNCTION(currTexSet.m_Texture, texture, SetTexture, sampler);

		if (currTexSet.m_Texture.GetDecision() != NULL)
		{
			MK_UPDATE_TEXTURE_STAGE_STATE(D3DTSS_TEXCOORDINDEX, currTexSet.m_Coordinate, coordinate, sampler);

			MK_UPDATE_SAMPLER_STATE_DUAL(D3DSAMP_MINFILTER, D3DSAMP_MAGFILTER, currTexSet.m_MinMagFilter, minmag, sampler);
			if (currTexSet.m_MinMagFilter.GetDecision() == D3DTEXF_ANISOTROPIC)
			{
				MK_UPDATE_SAMPLER_STATE(D3DSAMP_MAXANISOTROPY, currTexSet.m_MaxAnisotropic, maxAnisotropic, sampler);
			}

			MK_UPDATE_SAMPLER_STATE(D3DSAMP_MIPFILTER, currTexSet.m_MipmapFilter, mipmap, sampler);

			MK_UPDATE_SAMPLER_STATE_DUAL(D3DSAMP_ADDRESSU, D3DSAMP_ADDRESSV, currTexSet.m_TextureAddress, address, sampler);
			if (currTexSet.m_TextureAddress.GetDecision() == D3DTADDRESS_BORDER)
			{
				MK_UPDATE_SAMPLER_STATE(D3DSAMP_BORDERCOLOR, currTexSet.m_BorderColor, borderColor, sampler);
			}
		}
	}
}

void MkRenderStateSetter::UpdateTextureOperation(DWORD sampler, DWORD cOP, DWORD cA1, DWORD cA2, DWORD aOP, DWORD aA1, DWORD aA2)
{
	if (m_Device != NULL)
	{
		_TextureSet& currTexSet = m_TextureSet[sampler];

		MK_UPDATE_TEXTURE_STAGE_STATE(D3DTSS_COLOROP, currTexSet.m_ColorOP, cOP, sampler);
		const DWORD& cop = currTexSet.m_ColorOP.GetDecision();
		if (cop != D3DTOP_DISABLE)
		{
			if (cop != D3DTOP_SELECTARG1)
			{
				MK_UPDATE_TEXTURE_STAGE_STATE(D3DTSS_COLORARG2, currTexSet.m_ColorA2, cA2, sampler);
			}
			if (cop != D3DTOP_SELECTARG2)
			{
				MK_UPDATE_TEXTURE_STAGE_STATE(D3DTSS_COLORARG1, currTexSet.m_ColorA1, cA1, sampler);
			}
		}

		MK_UPDATE_TEXTURE_STAGE_STATE(D3DTSS_ALPHAOP, currTexSet.m_AlphaOP, aOP, sampler);
		const DWORD& aop = currTexSet.m_AlphaOP.GetDecision();
		if (aop != D3DTOP_DISABLE)
		{
			if (aop != D3DTOP_SELECTARG1)
			{
				MK_UPDATE_TEXTURE_STAGE_STATE(D3DTSS_ALPHAARG2, currTexSet.m_AlphaA2, aA2, sampler);
			}
			if (aop != D3DTOP_SELECTARG2)
			{
				MK_UPDATE_TEXTURE_STAGE_STATE(D3DTSS_ALPHAARG1, currTexSet.m_AlphaA1, aA1, sampler);
			}
		}
	}
}

void MkRenderStateSetter::ClearSampler1(void)
{
	_TextureSet& currTexSet = m_TextureSet[1];
	MK_UPDATE_SAMPLER_FUNCTION(currTexSet.m_Texture, NULL, SetTexture, 1);
	MK_UPDATE_TEXTURE_STAGE_STATE(D3DTSS_COLOROP, currTexSet.m_ColorOP, D3DTOP_DISABLE, 1);
}

void MkRenderStateSetter::ClearWorldTransform(void)
{
	MK_UPDATE_RENDER_TRANSFORM(D3DTS_WORLD, m_WorldMatrix, MkD3DType::GetIdentityMatrix());
}

void MkRenderStateSetter::UpdateViewProjectionTransform(const D3DXMATRIX& view, const D3DXMATRIX& projection)
{
	MK_UPDATE_RENDER_TRANSFORM(D3DTS_VIEW, m_ViewMatrix, view);
	MK_UPDATE_RENDER_TRANSFORM(D3DTS_PROJECTION, m_ProjectionMatrix, projection);
}

//------------------------------------------------------------------------------------------------//
