
#include "MkCore_MkCheck.h"
#include "MkCore_MkGlobalFunction.h"

#include "MkS2D_MkDisplayManager.h"
#include "MkS2D_MkRenderStateSetter.h"
#include "MkS2D_MkBaseTexture.h"


//------------------------------------------------------------------------------------------------//

bool MkBaseTexture::SetUp
(const MkUInt2& size, unsigned int mipLevel, DWORD usage, D3DFORMAT format,
 eFilterType filterType, D3DTEXTUREADDRESS addressMode, const MkColor& borderColor, const MkDataNode* subsetNode)
{
	LPDIRECT3DDEVICE9 device = MK_DISPLAY_MGR.GetDevice();
	if (device == NULL)
		return false;

	D3DPOOL poolType = ((usage == D3DUSAGE_DYNAMIC) || (usage == D3DUSAGE_RENDERTARGET)) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;

	unsigned int width = size.x;
	unsigned int height = size.y;
	MK_CHECK(D3DXCheckTextureRequirements(device, &width, &height, &mipLevel, 0, &format, poolType) == D3D_OK, L"허용되지 않는 포맷의 텍스쳐")
		return false;

	MK_CHECK(D3DXCreateTexture(device, width, height, mipLevel, usage, format, poolType, &m_Texture) == D3D_OK, L"d3d texture 생성 실패")
		return false;

	MK_CHECK(m_Texture->GetSurfaceLevel(0, &m_Surface) == D3D_OK, L"texture로부터 surface 생성 실패")
	{
		MK_RELEASE(m_Texture);
		return false;
	}

	m_Size = MkUInt2(width, height);
	m_Format = format;
	m_UseMipmap = (mipLevel > 1);

	m_AlphaTestingRef = 1;

	m_HasAlphaChannel = false;
	m_BlendType = eS2D_MBT_Opaque;
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
		m_HasAlphaChannel = true;
		m_BlendType = eS2D_MBT_AlphaChannel;
		break;
    }
	
	SetFilterType(filterType);

	m_AddressMode = addressMode;
	m_BorderColor = static_cast<D3DCOLOR>(borderColor.ConvertToD3DCOLOR());

	m_Subset.SetUp(m_Size, subsetNode);

	return true;
}

void MkBaseTexture::UpdateRenderState(DWORD objectAlpha) const
{
	if ((m_Texture != NULL) && (m_Surface != NULL))
	{
		bool hasObjectAlpha = (objectAlpha < 0xff);

		// blend type. color/alpha operation에서 만들어진 컬러, 알파 값을 어떻게 배경 컬러, 알파와 섞을지 결정
		DWORD alphaBlend, srcBlend, dstBlend;
		switch (m_BlendType)
		{
		case eS2D_MBT_Opaque: // 기본적으로 불투명이지만 objectAlpha가 영향을 미치면 eS2D_MBT_AlphaChannel와 동일
			alphaBlend = (hasObjectAlpha) ? TRUE : FALSE;
			srcBlend = (hasObjectAlpha) ? D3DBLEND_SRCALPHA : D3DBLEND_ONE;
			dstBlend = (hasObjectAlpha) ? D3DBLEND_INVSRCALPHA : D3DBLEND_ZERO;
			break;

		case eS2D_MBT_AlphaChannel: // (srcColor * srcAlpha) + (dstColor * (1.f - srcAlpha))
			alphaBlend = TRUE;
			srcBlend = D3DBLEND_SRCALPHA;
			dstBlend = D3DBLEND_INVSRCALPHA;
			break;

		case eS2D_MBT_ColorAdd: // (srcColor * 1) + (dstColor * 1)
			alphaBlend = TRUE;
			srcBlend = D3DBLEND_ONE;
			dstBlend = D3DBLEND_ONE;
			break;

		case eS2D_MBT_ColorMult: // (srcColor * 0) + (dstColor * srcColor)
			alphaBlend = TRUE;
			srcBlend = D3DBLEND_ZERO;
			dstBlend = D3DBLEND_SRCCOLOR;
			break;
		}

		DWORD alphaTest = (m_AlphaTestingRef == 0) ? FALSE : TRUE;
		DWORD alphaRef = static_cast<DWORD>(m_AlphaTestingRef);
		
		MK_RENDER_STATE.UpdateBlendOp(alphaBlend, objectAlpha, srcBlend, dstBlend, alphaTest, alphaRef);
		
		// color/alpha operation. 해당 재질의 컬러, 알파 값을 만들어냄
		DWORD cOP, cA1, cA2, aOP, aA1, aA2;
		switch (m_BlendType)
		{
		case eS2D_MBT_Opaque: // hasObjectAlpha일 경우 알파값으로 tfactor를 사용
			cOP = D3DTOP_SELECTARG1;
			cA1 = D3DTA_TEXTURE;
			cA2 = D3DTA_CURRENT;
			aOP = (hasObjectAlpha) ? D3DTOP_SELECTARG1 : D3DTOP_DISABLE;
			aA1 = (hasObjectAlpha) ? D3DTA_TFACTOR : D3DTA_TEXTURE;
			aA2 = D3DTA_CURRENT;
			break;

		case eS2D_MBT_AlphaChannel: // hasObjectAlpha일 경우 알파값으로 tfactor와 texture alpha의 곱을 사용
			cOP = D3DTOP_SELECTARG1;
			cA1 = D3DTA_TEXTURE;
			cA2 = D3DTA_CURRENT;
			aOP = (hasObjectAlpha) ? D3DTOP_MODULATE : D3DTOP_SELECTARG1;
			aA1 = D3DTA_TEXTURE;
			aA2 = (hasObjectAlpha) ? D3DTA_TFACTOR : D3DTA_CURRENT;
			break;

		case eS2D_MBT_ColorAdd: // hasObjectAlpha일 경우 텍스쳐 컬러에 미리 tfactor를 곱함
			cOP = (hasObjectAlpha) ? D3DTOP_MODULATE : D3DTOP_SELECTARG1;
			cA1 = D3DTA_TEXTURE;
			cA2 = (hasObjectAlpha) ? (D3DTA_TFACTOR | D3DTA_ALPHAREPLICATE) : D3DTA_CURRENT;
			aOP = D3DTOP_DISABLE;
			aA1 = D3DTA_TEXTURE;
			aA2 = D3DTA_CURRENT;
			break;

		case eS2D_MBT_ColorMult: // hasObjectAlpha일 경우 공식(texColor + (1 - texColor) * (1 - tfactor))이 한 스테이지에 처리되지 못하기 때문에 두 단계로 나누어 처리
			cOP = (hasObjectAlpha) ? D3DTOP_MODULATE : D3DTOP_SELECTARG1; // 1st step == (1 - texColor) * (1 - alpha)
			cA1 = (hasObjectAlpha) ? (D3DTA_TEXTURE | D3DTA_COMPLEMENT) : D3DTA_TEXTURE;
			cA2 = (hasObjectAlpha) ? (D3DTA_TFACTOR | D3DTA_COMPLEMENT | D3DTA_ALPHAREPLICATE) : D3DTA_CURRENT;
			aOP = D3DTOP_DISABLE;
			aA1 = D3DTA_TEXTURE;
			aA2 = D3DTA_CURRENT;
			break;
		}

		MK_RENDER_STATE.UpdateTextureOperation(0, cOP, cA1, cA2, aOP, aA1, aA2);

		// set sampler 0
		MK_RENDER_STATE.UpdateBaseTexture
			(0, GetTexture(), 0, GetMinMagFilter(), GetMaxAnisotropic(), GetMipmapFilter(), GetAddressMode(), GetBorderColor());

		if (hasObjectAlpha && (m_BlendType == eS2D_MBT_ColorMult))
		{
			// 2st step == texColor + result of 1st step
			MK_RENDER_STATE.UpdateTextureOperation(1, D3DTOP_ADD, D3DTA_TEXTURE, D3DTA_CURRENT, aOP, aA1, aA2);

			// set sampler 1
			MK_RENDER_STATE.UpdateBaseTexture
				(1, GetTexture(), 0, GetMinMagFilter(), GetMaxAnisotropic(), GetMipmapFilter(), GetAddressMode(), GetBorderColor());
		}
		else
		{
			// clear sampler 1
			MK_RENDER_STATE.ClearSampler1();
		}
	}
}

void MkBaseTexture::SetFilterType(eFilterType filterType)
{
	m_FilterType = filterType;

	switch (m_FilterType)
	{
	case eNone:
		m_MinMagFilter = D3DTEXF_NONE;
		m_MaxAnisotropic = 0;
		break;

	case ePoint:
		m_MinMagFilter = D3DTEXF_POINT;
		m_MaxAnisotropic = 0;
		break;

	case eLinear:
		m_MinMagFilter = D3DTEXF_LINEAR;
		m_MaxAnisotropic = 0;
		break;

	case eLowAnisotropic:
	case eMiddleAnisotropic:
	case eHighAnisotropic:
		{
			unsigned int cap = static_cast<unsigned int>(MK_DISPLAY_MGR.GetCaps().MaxAnisotropy);
			if (cap > 0)
			{
				m_MinMagFilter = D3DTEXF_ANISOTROPIC;
				if (filterType == eLowAnisotropic)
				{
					m_MaxAnisotropic = cap / 4;
				}
				else if (filterType == eMiddleAnisotropic)
				{
					m_MaxAnisotropic = cap / 2;
				}
				else
				{
					m_MaxAnisotropic = cap;
				}

				if (m_MaxAnisotropic == 0)
				{
					m_MaxAnisotropic = 1;
				}
			}
			else
			{
				filterType = eLinear;
				m_MinMagFilter = D3DTEXF_LINEAR;
				m_MaxAnisotropic = 0;
			}
		}
		break;
	}
}

MkBaseTexture::MkBaseTexture()
{
	m_Texture = NULL;
	m_Surface = NULL;
	m_Format = D3DFMT_UNKNOWN;

	m_AlphaTestingRef = 1;
	SetFilterType(eLinear);
	m_AddressMode = D3DTADDRESS_WRAP;
	m_BorderColor = MkColor::Black.ConvertToD3DCOLOR();
	m_UseMipmap = false;
	m_HasAlphaChannel = false;
	m_BlendType = eS2D_MBT_Opaque;
	m_BitmapGroup = 0;
}

void MkBaseTexture::_Clear(void)
{
	MK_RELEASE(m_Surface);
	MK_RELEASE(m_Texture);
}

//------------------------------------------------------------------------------------------------//
