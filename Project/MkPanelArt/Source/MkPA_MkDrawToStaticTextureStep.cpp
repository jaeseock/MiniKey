
#include "MkCore_MkCheck.h"

#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkRenderTarget.h"
#include "MkPA_MkDrawToStaticTextureStep.h"


//------------------------------------------------------------------------------------------------//

bool MkDrawToStaticTextureStep::Draw(void)
{
	if ((!m_GenerateTexture) || (m_TextureSize.x <= 0) || (m_TextureSize.y <= 0))
		return false;

	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
	if (device == NULL)
		return false;

	// 텍스쳐가 없으면 생성
	_CreateTexture();

	// 임시 렌더타겟 생성
	MkRenderTarget sourceRT;
	sourceRT.SetBackgroundColor(m_BackgroundColor);

	if (sourceRT.SetUp(MkRenderTarget::eTexture, 1, m_TextureSize, (m_AlphaType == eAlphaChannel) ? MkRenderToTexture::eRGBA : MkRenderToTexture::eRGB) &&
		sourceRT.__BeginScene())
	{
		sourceRT.__Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER);

		bool ok = _DrawContents();

		sourceRT.__EndScene();

		if (ok && (D3DXLoadSurfaceFromSurface
			(m_TargetTexture->GetSurface(), NULL, NULL, sourceRT.GetTargetTexture(0)->GetSurface(), NULL, NULL, D3DX_FILTER_POINT,
			(m_AlphaType == eAlphaKey) ? m_AlphaKey : 0) == D3D_OK))
		{
			m_GenerateTexture = false;
			return true;
		}
	}
	return false;
}

void MkDrawToStaticTextureStep::Clear(void)
{
	m_TargetTexture = NULL;
	m_GenerateTexture = false;
}

MkDrawToStaticTextureStep::MkDrawToStaticTextureStep() : MkDrawStepInterface()
{
	m_GenerateTexture = false;
	m_AlphaType = eAlphaChannel;
	m_BackgroundColor = m_AlphaKey = 0xff000000; // pure black
}

void MkDrawToStaticTextureStep::_SetNoneAlphaArguments(const MkInt2& size, DWORD bgColor)
{
	m_TextureSize = size;
	m_AlphaType = eNone;
	m_BackgroundColor = bgColor;
}

void MkDrawToStaticTextureStep::_SetAlphaChannelArguments(const MkInt2& size, DWORD bgColor)
{
	m_TextureSize = size;
	m_AlphaType = eAlphaChannel;
	m_BackgroundColor = bgColor;
}

void MkDrawToStaticTextureStep::_SetAlphaKeyArguments(const MkInt2& size, DWORD alphaKey)
{
	m_TextureSize = size;
	m_AlphaType = eAlphaKey;
	m_AlphaKey = m_BackgroundColor = alphaKey;
}

bool MkDrawToStaticTextureStep::_CreateTexture(void)
{
	if (m_TargetTexture == NULL)
	{
		MkBaseTexture* texture = new MkBaseTexture;
		MK_CHECK(texture != NULL, L"MkBaseTexture alloc 실패")
			return false;

		MK_CHECK(texture->SetUp(m_TextureSize, 1, 0, (m_AlphaType == eNone) ? D3DFMT_X8R8G8B8 : D3DFMT_A8R8G8B8), L"MkBaseTexture 초기화 실패")
		{
			delete texture;
			return false;
		}

		m_TargetTexture = texture;
	}
	return true;
}

//------------------------------------------------------------------------------------------------//
