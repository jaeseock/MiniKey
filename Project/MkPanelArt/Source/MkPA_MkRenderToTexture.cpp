
#include "MkCore_MkCheck.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkRenderToTexture.h"


//------------------------------------------------------------------------------------------------//

bool MkRenderToTexture::SetUp(const MkInt2& size, eFormat format, int targetIndex)
{
	MK_CHECK(size.IsPositive(), L"render to texture의 size는 0보다 커야 함")
		return false;

	m_Size = size;
	m_RenderToTextureFormat = format;
	m_TargetIndex = targetIndex;

	m_PoolKey.Clear();

	D3DFORMAT d3dFormat;
	if (m_RenderToTextureFormat == eDepth)
	{
		m_HasAlphaChannel = false;
		d3dFormat = D3DFMT_R32F;
	}
	else
	{
		m_HasAlphaChannel = (format == eRGBA);
		d3dFormat = (m_HasAlphaChannel) ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8;
	}

	m_BlendType = (m_HasAlphaChannel) ? ePA_MBT_AlphaChannel : ePA_MBT_Opaque;
	
	if (MkBaseTexture::SetUp(m_Size, 1, D3DUSAGE_RENDERTARGET, d3dFormat, ePoint, D3DTADDRESS_BORDER, (m_HasAlphaChannel) ? MkColor::Empty : MkColor::Black))
	{
		LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
		D3DFORMAT depthStencilFormat = MK_DEVICE_MGR.GetDepthStencilFormat();

		if (device->CreateDepthStencilSurface(static_cast<unsigned int>(m_Size.x), static_cast<unsigned int>(m_Size.y), depthStencilFormat, D3DMULTISAMPLE_NONE, 0, TRUE, &m_DepthBuffer, NULL) == D3D_OK)
		{
			_ClearTarget(device);
			return true;
		}
	}

	_Clear();
	return false;
}

void MkRenderToTexture::ChangeRenderTarget(void)
{
	if (m_DepthBuffer != NULL)
	{
		LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
		device->GetRenderTarget(m_TargetIndex, &m_OldRenderTarget);
		device->GetDepthStencilSurface(&m_OldDepthBuffer);
		device->GetViewport(&m_OldViewport);

		device->SetRenderTarget(m_TargetIndex, m_Surface);
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

void MkRenderToTexture::RestoreRenderTarget(void)
{
	if (m_DepthBuffer != NULL)
	{
		LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
		device->SetRenderTarget(m_TargetIndex, m_OldRenderTarget);
		device->SetDepthStencilSurface(m_OldDepthBuffer);
		device->SetViewport(&m_OldViewport);

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

void MkRenderToTexture::ReloadResource(LPDIRECT3DDEVICE9 device)
{
	unsigned int width = static_cast<unsigned int>(m_Size.x);
	unsigned int height = static_cast<unsigned int>(m_Size.y);

	D3DFORMAT format;
	if (m_RenderToTextureFormat == eDepth)
	{
		format = D3DFMT_R32F;
	}
	else
	{
		format = (m_HasAlphaChannel) ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8;
	}

	unsigned int mipLevel = 1;

	if (D3DXCheckTextureRequirements(device, &width, &height, &mipLevel, D3DUSAGE_RENDERTARGET, &format, D3DPOOL_DEFAULT) == D3D_OK)
	{
		if (D3DXCreateTexture(device, width, height, mipLevel, D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, &m_Texture) == D3D_OK)
		{
			if (m_Texture->GetSurfaceLevel(0, &m_Surface) == D3D_OK)
			{
				D3DFORMAT depthStencilFormat = MK_DEVICE_MGR.GetDepthStencilFormat();
				if (device->CreateDepthStencilSurface(width, height, depthStencilFormat, D3DMULTISAMPLE_NONE, 0, TRUE, &m_DepthBuffer, NULL) == D3D_OK)
				{
					_ClearTarget(device);
					return;
				}
			}
		}
	}

	UnloadResource();
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

void MkRenderToTexture::_ClearTarget(LPDIRECT3DDEVICE9 device)
{
	LPDIRECT3DSURFACE9 oldRenderTarget = NULL;
	LPDIRECT3DSURFACE9 oldDepthBuffer = NULL;
	D3DVIEWPORT9 oldViewport;

	device->GetRenderTarget(m_TargetIndex, &oldRenderTarget);
	device->GetDepthStencilSurface(&oldDepthBuffer);
	device->GetViewport(&oldViewport);

	device->SetRenderTarget(m_TargetIndex, m_Surface);
	device->SetDepthStencilSurface(m_DepthBuffer);

	D3DVIEWPORT9 vp;
	vp.X = 0;
	vp.Y = 0;
	vp.Width = static_cast<DWORD>(m_Size.x);
	vp.Height = static_cast<DWORD>(m_Size.y);
	vp.MinZ = 0.f;
	vp.MaxZ = 1.f;
	device->SetViewport(&vp);

	D3DCOLOR clearColor = D3DCOLOR_ARGB(((m_HasAlphaChannel) ? 0 : 0xff), 0, 0, 0);
	device->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), clearColor, 1.f, 0);

	device->SetRenderTarget(m_TargetIndex, oldRenderTarget);
	device->SetDepthStencilSurface(oldDepthBuffer);
	device->SetViewport(&oldViewport);

	MK_RELEASE(oldRenderTarget);
	MK_RELEASE(oldDepthBuffer);
}

//------------------------------------------------------------------------------------------------//
