#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkType2.h"
#include "MkCore_MkColor.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkSharedPointer.h"

#include "MkS2D_MkGlobalDefinition.h"
#include "MkS2D_MkD3DDefinition.h"
#include "MkS2D_MkImageSubset.h"


class MkBaseTexture
{
public:

	enum eFilterType
	{
		eNone = 0,
		ePoint,
		eLinear,
		eLowAnisotropic, // 25% of max
		eMiddleAnisotropic, // 50% of max
		eHighAnisotropic // 100% of max
	};

public:

	// ���� ������ texture & surface ����
	bool SetUp
		(const MkUInt2& size, unsigned int mipLevel, DWORD usage, D3DFORMAT format, // �̹��� �����Ϳ� ����
		eFilterType filterType = ePoint, D3DTEXTUREADDRESS addressMode = D3DTADDRESS_WRAP, const MkColor& borderColor = MkColor::Black, // �ܺ� ����
		const MkDataNode* subsetNode = NULL); // subset

	// ���� ������Ʈ ����
	void UpdateRenderState(DWORD objectAlpha) const;

	// ũ�� ��ȯ
	inline const MkUInt2& GetSize(void) const { return m_Size; }

	// alphaRef�� 0�̸� testing off, 1 ~ 255�� testing on
	inline void SetAlphaTestingRef(unsigned int alphaRef) { m_AlphaTestingRef = Clamp<unsigned int>(alphaRef, 0, 0xff); }
	inline unsigned int GetAlphaTestingRef(void) const { return m_AlphaTestingRef; }

	// min/mag filter
	void SetFilterType(eFilterType filterType);

	inline eFilterType GetFilterType(void) const { return m_FilterType; }
	inline D3DTEXTUREFILTERTYPE GetMinMagFilter(void) const { return m_MinMagFilter; }
	inline unsigned int GetMaxAnisotropic(void) const { return m_MaxAnisotropic; }

	// mipmap filter ��ȯ
	inline bool GetUseMipmap(void) const { return m_UseMipmap; }
	inline D3DTEXTUREFILTERTYPE GetMipmapFilter(void) const { return ((m_UseMipmap) ? D3DTEXF_LINEAR : D3DTEXF_NONE); }

	// address mode ��ȯ
	inline D3DTEXTUREADDRESS GetAddressMode(void) const { return m_AddressMode; }

	// border color ��ȯ
	inline D3DCOLOR GetBorderColor(void) const { return m_BorderColor; }

	// texture ��ȯ
	inline const LPDIRECT3DTEXTURE9 GetTexture(void) const { return m_Texture; }

	// surface ��ȯ
	inline LPDIRECT3DSURFACE9 GetSurface(void) const { return m_Surface; }

	// format ��ȯ
	inline D3DFORMAT GetFormat(void) const { return m_Format; }

	// alpha channel ���� ��ȯ
	inline bool HasAlphaChannel(void) const { return m_HasAlphaChannel; }

	// blend type
	inline void SetBlendType(eS2D_MaterialBlendType blendType) { m_BlendType = blendType; }
	inline eS2D_MaterialBlendType GetBlendType(void) const { return m_BlendType; }

	// pool�� key
	inline void SetPoolKey(const MkHashStr& key) { m_PoolKey = key; }
	inline const MkHashStr& GetPoolKey(void) const { return m_PoolKey; }

	// image subset
	const MkFloat2& GetSubsetSize(const MkHashStr& name) const { return m_Subset.GetSubsetSize(name); }
	inline void GetSubsetInfo(const MkHashStr& name, MkFloat2& localSize, MkFloat2 (&uv)[MkFloatRect::eMaxPointName]) const { m_Subset.GetSubsetInfo(name, localSize, uv); }

	// bitmap group
	inline void SetBitmapGroup(unsigned int group) { m_BitmapGroup = group; }
	inline unsigned int GetBitmapGroup(void) const { return m_BitmapGroup; }

	MkBaseTexture();
	virtual ~MkBaseTexture() { _Clear(); }

protected:

	void _Clear(void);

protected:

	MkUInt2 m_Size;
	D3DFORMAT m_Format;

	LPDIRECT3DTEXTURE9 m_Texture;
	LPDIRECT3DSURFACE9 m_Surface;

	// alpha test
	unsigned int m_AlphaTestingRef;

	// min/mag filter
	eFilterType m_FilterType;
	D3DTEXTUREFILTERTYPE m_MinMagFilter;
	unsigned int m_MaxAnisotropic;

	// address mode
	D3DTEXTUREADDRESS m_AddressMode;
	D3DCOLOR m_BorderColor;

	// mipmap enable
	bool m_UseMipmap;

	// alpha channel
	bool m_HasAlphaChannel;

	// blend type
	eS2D_MaterialBlendType m_BlendType;

	// bitmap pool ������ ���� key
	MkHashStr m_PoolKey;

	// subset
	MkImageSubset m_Subset;
	
	// bitmap group
	unsigned int m_BitmapGroup;
};

typedef MkSharedPointer<MkBaseTexture> MkBaseTexturePtr;

//------------------------------------------------------------------------------------------------//
