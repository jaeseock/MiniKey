
#include "MkCore_MkCheck.h"
#include "MkPE_MkPostEffectRenderer.h"
#include "MkPE_MkRenderToTexture.h"


//------------------------------------------------------------------------------------------------//

bool MkRenderToTexture::SetUp(const MkInt2& size, eTargetFormat format, int targetIndex)
{
	m_TargetFormat = format;
	m_TargetIndex = targetIndex;

	m_HasAlphaChannel = (m_TargetFormat == eTF_Depth) ? false : (m_TargetFormat == eTF_RGBA);
	m_BlendType = (m_HasAlphaChannel) ? eTBT_AlphaChannel : eTBT_Opaque;

	m_PoolKey.Clear();

	return Resize(size);
}

bool MkRenderToTexture::Resize(const MkInt2& size)
{
	MK_CHECK(size.IsPositive(), L"render to texture의 size는 0보다 커야 함")
		return false;

	m_Size = size;
	return _CreateTexture();
}

void MkRenderToTexture::ChangeRenderTarget(void)
{
	if (m_Surface != NULL)
	{
		LPDIRECT3DDEVICE9 device = MK_POST_EFFECT.GetDevice();
		device->GetRenderTarget(m_TargetIndex, &m_OldRenderTarget);
		if (m_TargetIndex == 0)
		{
			device->GetDepthStencilSurface(&m_OldDepthBuffer);
			device->GetViewport(&m_OldViewport);
		}

		device->SetRenderTarget(m_TargetIndex, m_Surface);
		if (m_TargetIndex == 0)
		{
			device->SetDepthStencilSurface(m_DepthBuffer);

			D3DVIEWPORT9 vp;
			vp.X = 0;
			vp.Y = 0;
			vp.Width = static_cast<DWORD>(m_Size.x);
			vp.Height = static_cast<DWORD>(m_Size.y);
			vp.MinZ = 0.f;
			vp.MaxZ = 1.f;
			device->SetViewport(&vp);
		}
	}
}

void MkRenderToTexture::RestoreRenderTarget(void)
{
	if (m_Surface != NULL)
	{
		LPDIRECT3DDEVICE9 device = MK_POST_EFFECT.GetDevice();
		device->SetRenderTarget(m_TargetIndex, m_OldRenderTarget);
		if (m_TargetIndex == 0)
		{
			device->SetDepthStencilSurface(m_OldDepthBuffer);
			device->SetViewport(&m_OldViewport);
		}

		MK_RELEASE(m_OldRenderTarget);
		MK_RELEASE(m_OldDepthBuffer);
	}
}

void MkRenderToTexture::UnloadResource(void)
{
	_Clear();

	MK_RELEASE(m_DepthBuffer);
	MK_RELEASE(m_OldRenderTarget);
	MK_RELEASE(m_OldDepthBuffer);
}

void MkRenderToTexture::ReloadResource(void)
{
	LPDIRECT3DDEVICE9 device = MK_POST_EFFECT.GetDevice();
	if (device == NULL)
		return;

	_CreateTexture();
}

MkRenderToTexture::MkRenderToTexture() : MkBaseTexture()
{
	m_TargetIndex = 0;
	m_DepthBuffer = NULL;
	m_OldRenderTarget = NULL;
	m_OldDepthBuffer = NULL;
}

MkRenderToTexture::~MkRenderToTexture()
{
	MK_RELEASE(m_DepthBuffer);
	MK_RELEASE(m_OldRenderTarget);
	MK_RELEASE(m_OldDepthBuffer);
}

bool MkRenderToTexture::_CreateTexture(void)
{
	UnloadResource();

	D3DFORMAT d3dFormat = (m_TargetFormat == eTF_Depth) ? D3DFMT_R32F : ((m_HasAlphaChannel) ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8);

	if (MkBaseTexture::SetUp(m_Size, D3DUSAGE_RENDERTARGET, d3dFormat, eMMFT_Point))
	{
		if (m_TargetIndex == 0)
		{
			LPDIRECT3DDEVICE9 device = MK_POST_EFFECT.GetDevice();
			if (device->CreateDepthStencilSurface(static_cast<unsigned int>(m_Size.x), static_cast<unsigned int>(m_Size.y), D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE, &m_DepthBuffer, NULL) == D3D_OK)
			{
				_ClearTarget(device);
				return true;
			}
		}
		else
			return true;
	}

	UnloadResource();
	return false;
}

void MkRenderToTexture::_ClearTarget(LPDIRECT3DDEVICE9 device)
{
	ChangeRenderTarget();

	D3DCOLOR clearColor = D3DCOLOR_ARGB(((m_HasAlphaChannel) ? 0 : 0xff), 0, 0, 0);
	device->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), clearColor, 1.f, 0);

	RestoreRenderTarget();
}

//------------------------------------------------------------------------------------------------//
