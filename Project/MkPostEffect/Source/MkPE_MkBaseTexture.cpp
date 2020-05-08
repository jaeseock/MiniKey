
#include "MkCore_MkCheck.h"
#include "MkCore_MkGlobalFunction.h"
#include "MkCore_MkPathName.h"

#include "MkPE_MkPostEffectRenderer.h"
//#include "MkPA_MkRenderStateSetter.h"
#include "MkPE_MkBaseTexture.h"


//------------------------------------------------------------------------------------------------//

bool MkBaseTexture::SetUp(const MkInt2& size, DWORD usage, D3DFORMAT format, eMinMagFilterType filterType)
{
	_Clear();

	do
	{
		LPDIRECT3DDEVICE9 device = MK_POST_EFFECT.GetDevice();
		if (device == NULL)
			break;

		D3DPOOL poolType = ((usage == D3DUSAGE_DYNAMIC) || (usage == D3DUSAGE_RENDERTARGET)) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;

		unsigned int width = static_cast<unsigned int>(size.x);
		unsigned int height = static_cast<unsigned int>(size.y);
		unsigned int mipLevel = 1;
		MK_CHECK(D3DXCheckTextureRequirements(device, &width, &height, &mipLevel, 0, &format, poolType) == D3D_OK, L"허용되지 않는 포맷의 텍스쳐")
			break;

		MK_CHECK(D3DXCreateTexture(device, width, height, mipLevel, usage, format, poolType, &m_Texture) == D3D_OK, L"d3d texture 생성 실패")
			break;

		MK_CHECK(m_Texture->GetSurfaceLevel(0, &m_Surface) == D3D_OK, L"texture로부터 surface 생성 실패")
			break;

		m_Size = MkInt2(static_cast<int>(width), static_cast<int>(height));
		
		m_HasAlphaChannel = IsAlphaFormat(format);
		m_BlendType = (m_HasAlphaChannel) ? eTBT_AlphaChannel : eTBT_Opaque;
		
		SetMinMagFilter(filterType);
		return true;
	}
	while (false);

	_Clear();
	return false;
}

void MkBaseTexture::UpdateRenderState(DWORD objectAlpha) const
{
	LPDIRECT3DDEVICE9 device = MK_POST_EFFECT.GetDevice();
	if ((device != NULL) && (m_Texture != NULL) && (m_Surface != NULL))
	{
		bool hasObjectAlpha = (objectAlpha < 0xff);

		// color/alpha operation. 해당 재질의 컬러, 알파 값을 만들어냄
		DWORD cOP, cA1, cA2, aOP, aA1, aA2;
		switch (m_BlendType)
		{
		case eTBT_Opaque: // hasObjectAlpha일 경우 알파값으로 tfactor를 사용
			cOP = D3DTOP_SELECTARG1;
			cA1 = D3DTA_TEXTURE;
			cA2 = D3DTA_CURRENT;
			aOP = (hasObjectAlpha) ? D3DTOP_SELECTARG1 : D3DTOP_DISABLE;
			aA1 = (hasObjectAlpha) ? D3DTA_TFACTOR : D3DTA_TEXTURE;
			aA2 = D3DTA_CURRENT;
			break;

		case eTBT_AlphaChannel: // hasObjectAlpha일 경우 알파값으로 tfactor와 texture alpha의 곱을 사용
			cOP = D3DTOP_SELECTARG1;
			cA1 = D3DTA_TEXTURE;
			cA2 = D3DTA_CURRENT;
			aOP = (hasObjectAlpha) ? D3DTOP_MODULATE : D3DTOP_SELECTARG1;
			aA1 = D3DTA_TEXTURE;
			aA2 = (hasObjectAlpha) ? D3DTA_TFACTOR : D3DTA_CURRENT;
			break;

		case eTBT_ColorAdd: // hasObjectAlpha일 경우 텍스쳐 컬러에 미리 tfactor를 곱함
			cOP = (hasObjectAlpha) ? D3DTOP_MODULATE : D3DTOP_SELECTARG1;
			cA1 = D3DTA_TEXTURE;
			cA2 = (hasObjectAlpha) ? (D3DTA_TFACTOR | D3DTA_ALPHAREPLICATE) : D3DTA_CURRENT;
			aOP = D3DTOP_DISABLE;
			aA1 = D3DTA_TEXTURE;
			aA2 = D3DTA_CURRENT;
			break;

		case eTBT_ColorMult: // hasObjectAlpha일 경우 공식(texColor + (1 - texColor) * (1 - tfactor))이 한 스테이지에 처리되지 못하기 때문에 두 단계로 나누어 처리
			cOP = (hasObjectAlpha) ? D3DTOP_MODULATE : D3DTOP_SELECTARG1; // 1st step == (1 - texColor) * (1 - alpha)
			cA1 = (hasObjectAlpha) ? (D3DTA_TEXTURE | D3DTA_COMPLEMENT) : D3DTA_TEXTURE;
			cA2 = (hasObjectAlpha) ? (D3DTA_TFACTOR | D3DTA_COMPLEMENT | D3DTA_ALPHAREPLICATE) : D3DTA_CURRENT;
			aOP = D3DTOP_DISABLE;
			aA1 = D3DTA_TEXTURE;
			aA2 = D3DTA_CURRENT;
			break;
		}

		_UpdateTextureOperation(device, 0, cOP, cA1, cA2, aOP, aA1, aA2);

		// blend type. color/alpha operation에서 만들어진 컬러, 알파 값을 어떻게 배경 컬러, 알파와 섞을지 결정
		DWORD alphaBlend, srcBlend, dstBlend;
		switch (m_BlendType)
		{
		case eTBT_Opaque: // 기본적으로 불투명이지만 objectAlpha가 영향을 미치면 ePA_MBT_AlphaChannel와 동일
			alphaBlend = (hasObjectAlpha) ? TRUE : FALSE;
			srcBlend = (hasObjectAlpha) ? D3DBLEND_SRCALPHA : D3DBLEND_ONE;
			dstBlend = (hasObjectAlpha) ? D3DBLEND_INVSRCALPHA : D3DBLEND_ZERO;
			break;

		case eTBT_AlphaChannel: // (srcColor * srcAlpha) + (dstColor * (1.f - srcAlpha))
			alphaBlend = TRUE;
			srcBlend = D3DBLEND_SRCALPHA;
			dstBlend = D3DBLEND_INVSRCALPHA;
			break;

		case eTBT_ColorAdd: // (srcColor * 1) + (dstColor * 1)
			alphaBlend = TRUE;
			srcBlend = D3DBLEND_ONE;
			dstBlend = D3DBLEND_ONE;
			break;

		case eTBT_ColorMult: // (srcColor * 0) + (dstColor * srcColor)
			alphaBlend = TRUE;
			srcBlend = D3DBLEND_ZERO;
			dstBlend = D3DBLEND_SRCCOLOR;
			break;
		}

		_UpdateBlendOp(device, alphaBlend, objectAlpha, srcBlend, dstBlend, FALSE, 0);

		D3DTEXTUREFILTERTYPE mmFilterType;
		switch (m_MinMagFilter)
		{
		case eMMFT_Point: mmFilterType = D3DTEXF_POINT; break;
		case eMMFT_Linear: mmFilterType = D3DTEXF_LINEAR; break;
		default : mmFilterType = D3DTEXF_NONE; break;
		}

		// set sampler 0
		_UpdateBaseTexture(device, 0, GetTexture(), 0, mmFilterType, 0, D3DTEXF_NONE, D3DTADDRESS_BORDER, 0xff000000);

		if (hasObjectAlpha && (m_BlendType == eTBT_ColorMult))
		{
			// 2st step == texColor + result of 1st step
			_UpdateTextureOperation(device, 1, D3DTOP_ADD, D3DTA_TEXTURE, D3DTA_CURRENT, aOP, aA1, aA2);

			// set sampler 1
			_UpdateBaseTexture(device, 1, GetTexture(), 0, mmFilterType, 0, D3DTEXF_NONE, D3DTADDRESS_BORDER, 0xff000000);
		}
		else
		{
			// clear sampler 1
			device->SetTexture(1, NULL);
			device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		}
	}
}

bool MkBaseTexture::SaveToPNG(const MkPathName& filePath) const
{
	MkPathName fullPath;
	fullPath.ConvertToRootBasisAbsolutePath(filePath);
	return ((m_Surface != NULL) && (D3DXSaveSurfaceToFile(fullPath, D3DXIFF_PNG, m_Surface, NULL, NULL) == D3D_OK));
}

bool MkBaseTexture::GetPixelTable(MkArray<D3DCOLOR>& buffer)
{
	if (m_Texture == NULL)
		return false;

	unsigned int totalPixels = static_cast<unsigned int>(m_Size.x * m_Size.y);
	if (totalPixels == 0)
		return false;

	D3DLOCKED_RECT rect;
	if (m_Texture->LockRect(0, &rect, NULL, D3DLOCK_DISCARD) != D3D_OK)
		return false;

	if (((rect.Pitch / m_Size.x) == sizeof(DWORD)) && ((rect.Pitch % m_Size.x) == 0))
	{
		buffer.Reserve(totalPixels);

		DWORD* pColor = reinterpret_cast<DWORD*>(rect.pBits);
		unsigned int index = 0;
		while (index < totalPixels)
		{
			D3DXCOLOR c = pColor[index];
			buffer.PushBack(
				(static_cast<unsigned int>(c.a * 255.f) << 24) |
				(static_cast<unsigned int>(c.r * 255.f) << 16) |
				(static_cast<unsigned int>(c.g * 255.f) << 8) |
				static_cast<unsigned int>(c.b * 255.f)
				);

			++index;
		}
	}
	m_Texture->UnlockRect(0);
	return (buffer.GetSize() == totalPixels);
}

bool MkBaseTexture::IsAlphaFormat(D3DFORMAT format)
{
	switch (format)
	{
	case D3DFMT_A8R8G8B8:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A4R4G4B4:
	case D3DFMT_A8:
	case D3DFMT_A8R3G3B2:
	case D3DFMT_A2B10G10R10:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_A2R10G10B10:
	case D3DFMT_A16B16G16R16:
	case D3DFMT_A8P8:
	case D3DFMT_A8L8:
	case D3DFMT_A4L4:
	//case D3DFMT_DXT1: 알파 없다고 간주. 1bit 알파가 존재하면 컬러가 4bit에서 3bit으로 바뀜(큰 화질 저하)
	//case D3DFMT_DXT2: DXT1 + premultified alpha
	case D3DFMT_DXT3: // 알파 4bit
	//case D3DFMT_DXT4: DXT3 + premultified alpha
	case D3DFMT_DXT5: // 알파 4bit를 8bit로 보간
		return true;
    }
	return false;
}

MkInt2 MkBaseTexture::GetFormalSize(const MkInt2 srcSize)
{
	MkInt2 targetSize = srcSize;
	if (targetSize.IsPositive())
	{
		MkUInt2 uiSize(static_cast<unsigned int>(targetSize.x), static_cast<unsigned int>(targetSize.y));

		unsigned int width = 1;
		while (true)
		{
			if (width >= uiSize.x)
				break;
			width = width << 1; // * 2;
		}

		unsigned int height = 1;
		while (true)
		{
			if (height >= uiSize.y)
				break;
			height = height << 1; // * 2;
		}

		targetSize = MkInt2(static_cast<int>(width), static_cast<int>(height));
	}
	return targetSize;
}

MkBaseTexture::MkBaseTexture()
{
	m_Texture = NULL;
	m_Surface = NULL;

	SetMinMagFilter(eMMFT_None);
	m_HasAlphaChannel = false;
	m_BlendType = eTBT_Opaque;
}

void MkBaseTexture::_Clear(void)
{
	MK_RELEASE(m_Surface);
	MK_RELEASE(m_Texture);
}

void MkBaseTexture::_UpdateBlendOp(LPDIRECT3DDEVICE9 device, DWORD alphaBlend, DWORD objectAlpha, DWORD srcBlend, DWORD dstBlend, DWORD alphaTest, DWORD alphaRef) const
{
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, alphaBlend);
	if (alphaBlend == TRUE)
	{
		device->SetRenderState(D3DRS_TEXTUREFACTOR, (objectAlpha << 24) + 0x00ffffff);
		device->SetRenderState(D3DRS_SRCBLEND, srcBlend);
		device->SetRenderState(D3DRS_DESTBLEND, dstBlend);

		device->SetRenderState(D3DRS_ALPHATESTENABLE, alphaTest);
		if (alphaTest == TRUE)
		{
			device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			device->SetRenderState(D3DRS_ALPHAREF, alphaRef);
		}
		else
		{
			device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
		}
	}
}

void MkBaseTexture::_UpdateBaseTexture
(LPDIRECT3DDEVICE9 device, DWORD sampler, LPDIRECT3DTEXTURE9 texture, unsigned int coordinate, _D3DTEXTUREFILTERTYPE minmag, unsigned int maxAnisotropic,
 _D3DTEXTUREFILTERTYPE mipmap, _D3DTEXTUREADDRESS address, D3DCOLOR borderColor) const
{
	device->SetTexture(sampler, texture);
	if (texture != NULL)
	{
		device->SetTextureStageState(sampler, D3DTSS_TEXCOORDINDEX, coordinate);

		device->SetSamplerState(sampler, D3DSAMP_MINFILTER, minmag);
		device->SetSamplerState(sampler, D3DSAMP_MAGFILTER, minmag);
		if (minmag == D3DTEXF_ANISOTROPIC)
		{
			device->SetSamplerState(sampler, D3DSAMP_MAXANISOTROPY, maxAnisotropic);
		}

		device->SetSamplerState(sampler, D3DSAMP_MIPFILTER, mipmap);

		device->SetSamplerState(sampler, D3DSAMP_ADDRESSU, address);
		device->SetSamplerState(sampler, D3DSAMP_ADDRESSV, address);
		if (address == D3DTADDRESS_BORDER)
		{
			device->SetSamplerState(sampler, D3DSAMP_BORDERCOLOR, borderColor);
		}
	}
}

void MkBaseTexture::_UpdateTextureOperation(LPDIRECT3DDEVICE9 device, DWORD sampler, DWORD cOP, DWORD cA1, DWORD cA2, DWORD aOP, DWORD aA1, DWORD aA2) const
{
	device->SetTextureStageState(sampler, D3DTSS_COLOROP, cOP);
	if (cOP != D3DTOP_DISABLE)
	{
		if (cOP != D3DTOP_SELECTARG1)
		{
			device->SetTextureStageState(sampler, D3DTSS_COLORARG2, cA2);
		}
		if (cOP != D3DTOP_SELECTARG2)
		{
			device->SetTextureStageState(sampler, D3DTSS_COLORARG1, cA1);
		}
	}

	device->SetTextureStageState(sampler, D3DTSS_ALPHAOP, aOP);
	if (aOP != D3DTOP_DISABLE)
	{
		if (aOP != D3DTOP_SELECTARG1)
		{
			device->SetTextureStageState(sampler, D3DTSS_ALPHAARG2, aA2);
		}
		if (aOP != D3DTOP_SELECTARG2)
		{
			device->SetTextureStageState(sampler, D3DTSS_ALPHAARG1, aA1);
		}
	}
}

//------------------------------------------------------------------------------------------------//
