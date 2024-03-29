
#include "MkPE_MkOrthogonalCamera.h"
#include "MkPE_MkBitmapPool.h"
#include "MkPE_MkShaderEffectPool.h"
#include "MkPE_MkPostEffectRenderer.h"

#pragma comment(lib, "d3d9")
#pragma comment(lib, "d3dx9")


//------------------------------------------------------------------------------------------------//

void MkPostEffectRenderer::SetUp(LPDIRECT3DDEVICE9 device, const MkInt2& size, const MkArray<MkStr>& effectFilePathList, bool autoRefresh)
{
	m_Device = device;
	m_ScreenSize = size;

	MK_SHADER_POOL.SetUp(device, effectFilePathList, autoRefresh);
}

void MkPostEffectRenderer::Resize(const MkInt2& size)
{
	if (size != m_ScreenSize)
	{
		m_ScreenSize = size;

		m_RenderTarget.Resize(m_ScreenSize);

		MkMapLooper<int, MkScreen> looper(m_ScreenQueue);
		MK_STL_LOOP(looper)
		{
			looper.GetCurrentField().Resize(m_ScreenSize);
		}
	}
}

bool MkPostEffectRenderer::CreateSourceRenderTarget(MkRenderToTexture::eTargetFormat format)
{
	return m_RenderTarget.SetUp(m_ScreenSize, format);
}

bool MkPostEffectRenderer::CreateSourceRenderTarget(const MkArray<MkRenderToTexture::eTargetFormat>& formats)
{
	return m_RenderTarget.SetUp(m_ScreenSize, formats);
}

MkScreen* MkPostEffectRenderer::CreateScreen(int index)
{
	return _CreateScreen(index);
}

MkScreen* MkPostEffectRenderer::CreateScreen(int index, MkRenderToTexture::eTargetFormat format)
{
	MkScreen* screen = _CreateScreen(index);
	screen->SetUp(m_ScreenSize, format);
	return screen;
}

MkScreen* MkPostEffectRenderer::CreateScreen(int index, const MkInt2& size, MkRenderToTexture::eTargetFormat format)
{
	MkScreen* screen = _CreateScreen(index);
	screen->SetUp(size, format, true);
	return screen;
}

MkScreen* MkPostEffectRenderer::CreateScreen(int index, const MkArray<MkRenderToTexture::eTargetFormat>& formats)
{
	MkScreen* screen = _CreateScreen(index);
	screen->SetUp(m_ScreenSize, formats);
	return screen;
}

MkScreen* MkPostEffectRenderer::CreateScreen(int index, const MkInt2& size, const MkArray<MkRenderToTexture::eTargetFormat>& formats)
{
	MkScreen* screen = _CreateScreen(index);
	screen->SetUp(size, formats, true);
	return screen;
}

MkScreen* MkPostEffectRenderer::GetScreen(int index)
{
	return m_ScreenQueue.Exist(index) ? &m_ScreenQueue[index] : NULL;
}

void MkPostEffectRenderer::ClearAllScreens(void)
{
	m_ScreenQueue.Clear();
}

void MkPostEffectRenderer::Draw(void)
{
	if (m_Device == NULL)
		return;

	MK_SHADER_POOL.CheckAndReload(m_Device, 1000);

	// static
	m_Device->SetRenderState(D3DRS_AMBIENT, 0xffffffff);
	m_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_Device->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	m_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_Device->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
	m_Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	m_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	m_Device->SetTransform(D3DTS_WORLD, &MkD3DType::GetIdentityMatrix());
	
	D3DMATERIAL9 material;
	material.Diffuse.r = 1.f; material.Diffuse.g = 1.f; material.Diffuse.b = 1.f;
	material.Ambient.r = 0.f; material.Ambient.g = 0.f; material.Ambient.b = 0.f; material.Ambient.a = 1.f;
	material.Specular.r = 0.f; material.Specular.g = 0.f; material.Specular.b = 0.f; material.Specular.a = 1.f;
	material.Emissive.r = 0.f; material.Emissive.g = 0.f; material.Emissive.b = 0.f; material.Emissive.a = 1.f;
	material.Power = 1.f;
	m_Device->SetMaterial(&material);

	// 대부분이 알파 오브젝트고 소팅 순서를 완벽하게 정해서 그려주므로 의미 없음
	m_Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS); // z read off
	m_Device->SetRenderState(D3DRS_ZWRITEENABLE, D3DZB_FALSE); // z write off

	m_Device->SetIndices(NULL); // D3DPT_TRIANGLELIST만 사용하므로 index buffer가 필요 없음

	// camera
	MkFloat2 screenSize(static_cast<float>(m_ScreenSize.x), static_cast<float>(m_ScreenSize.y));

	MkOrthogonalCamera camera;
	camera.SetPosition(screenSize / 2.f);
	camera.SetSize(screenSize);
	camera.UpdateViewProjectionMatrix();

	m_Device->SetTransform(D3DTS_VIEW, &camera.GetViewMatrix());
	m_Device->SetTransform(D3DTS_PROJECTION, &camera.GetProjectionMatrix());

	// draw
	if (m_ScreenQueue.Empty())
	{
		m_Device->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_XRGB(0, 0, 0), 1.f, 0);
	}
	else if (SUCCEEDED(m_Device->BeginScene()))
	{
		MkMapLooper<int, MkScreen> looper(m_ScreenQueue);
		MK_STL_LOOP(looper)
		{
			looper.GetCurrentField().__Draw(m_Device, screenSize);
		}

		m_Device->EndScene();
	}
}

void MkPostEffectRenderer::Clear(void)
{
	m_RenderTarget.Clear();
	ClearAllScreens();

	MK_SHADER_POOL.Clear();
	MK_BITMAP_POOL.Clear();
}

void MkPostEffectRenderer::UnloadResource(void)
{
	m_RenderTarget.UnloadResource();

	MkMapLooper<int, MkScreen> looper(m_ScreenQueue);
	MK_STL_LOOP(looper)
	{
		looper.GetCurrentField().UnloadResource();
	}

	MK_SHADER_POOL.UnloadResource();
}

void MkPostEffectRenderer::ReloadResource(void)
{
	m_RenderTarget.ReloadResource();

	MkMapLooper<int, MkScreen> looper(m_ScreenQueue);
	MK_STL_LOOP(looper)
	{
		looper.GetCurrentField().ReloadResource();
	}

	MK_SHADER_POOL.ReloadResource();
}

MkPostEffectRenderer& MkPostEffectRenderer::GetInstance(void)
{
	static MkPostEffectRenderer instance;
	return instance;
}

MkPostEffectRenderer::MkPostEffectRenderer() 
{
	m_Device = NULL;
}

MkScreen* MkPostEffectRenderer::_CreateScreen(int index)
{
	MkScreen* screen = NULL;
	if (m_ScreenQueue.Exist(index))
	{
		screen = &m_ScreenQueue[index];
		screen->Clear();
	}
	else
	{
		screen = &m_ScreenQueue.Create(index);
		screen->SetUp();
	}
	return screen;
}

//------------------------------------------------------------------------------------------------//
