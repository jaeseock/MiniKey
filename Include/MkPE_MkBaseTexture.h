#pragma once


//------------------------------------------------------------------------------------------------//
// 텍스쳐 기본형. 초간단
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkType2.h"
#include "MkCore_MkHashStr.h"

#include "MkPE_MkD3DDefinition.h"


class MkPathName;

class MkBaseTexture
{
public:

	enum eMinMagFilterType
	{
		eMMFT_None = 0,
		eMMFT_Point,
		eMMFT_Linear
	};

	enum eTextureBlendType
	{
		eTBT_Opaque = 0,
		eTBT_AlphaChannel,
		eTBT_ColorAdd,
		eTBT_ColorMult
	};

public:

	bool SetUp(const MkInt2& size, DWORD usage, D3DFORMAT format, eMinMagFilterType filterType);

	// 렌더 스테이트 갱신
	void UpdateRenderState(DWORD objectAlpha) const;

	// 크기 반환
	inline const MkInt2& GetSize(void) const { return m_Size; }

	// min/mag filter 설정/반환
	inline void SetMinMagFilter(eMinMagFilterType filterType) { m_MinMagFilter = filterType; }
	inline eMinMagFilterType GetMinMagFilter(void) const { return m_MinMagFilter; }

	// alpha channel 유무 반환
	inline bool HasAlphaChannel(void) const { return m_HasAlphaChannel; }

	// blend type 설정/반환
	inline void SetBlendType(eTextureBlendType blendType) { m_BlendType = blendType; }
	inline eTextureBlendType GetBlendType(void) const { return m_BlendType; }

	// pool용 key 설정/반환
	inline void SetPoolKey(const MkHashStr& key) { m_PoolKey = key; }
	inline const MkHashStr& GetPoolKey(void) const { return m_PoolKey; }

	// texture 반환
	inline const LPDIRECT3DTEXTURE9 GetTexture(void) const { return m_Texture; }

	// surface 반환
	inline LPDIRECT3DSURFACE9 GetSurface(void) const { return m_Surface; }

	// 이미지를 PNG 파일로 저장
	bool SaveToPNG(const MkPathName& filePath) const;

	//  픽셀 정보 반환
	// ex>
	//	MkArray<D3DCOLOR> buffer;
	//	texture->GetPixelTable(buffer);
	//
	//	for (unsigned int y=0; y<img_y; ++y)
	//	{
	//		for (unsigned int x=0; x<img_x; ++x)
	//		{
	//			MkColor c(buffer[y * img_x + x]);
	//		}
	//	}
	bool GetPixelTable(MkArray<D3DCOLOR>& buffer);

	// 해당 포맷이 알파 포맷인지 체크
	static bool IsAlphaFormat(D3DFORMAT format);

	// size의 x, y가 1 이상일 경우 2의 n승을 만족하는 최소 크기를 반환
	static MkInt2 GetFormalSize(const MkInt2 srcSize);

	MkBaseTexture();
	virtual ~MkBaseTexture() { _Clear(); }

protected:

	void _Clear(void);

	void _UpdateBlendOp(LPDIRECT3DDEVICE9 device, DWORD alphaBlend, DWORD objectAlpha, DWORD srcBlend, DWORD dstBlend, DWORD alphaTest, DWORD alphaRef) const;

	void _UpdateBaseTexture(LPDIRECT3DDEVICE9 device, DWORD sampler,
		LPDIRECT3DTEXTURE9 texture, unsigned int coordinate, _D3DTEXTUREFILTERTYPE minmag, unsigned int maxAnisotropic,
		_D3DTEXTUREFILTERTYPE mipmap, _D3DTEXTUREADDRESS address, D3DCOLOR borderColor) const;

	void _UpdateTextureOperation(LPDIRECT3DDEVICE9 device, DWORD sampler, DWORD cOP, DWORD cA1, DWORD cA2, DWORD aOP, DWORD aA1, DWORD aA2) const;

protected:

	LPDIRECT3DTEXTURE9 m_Texture;
	LPDIRECT3DSURFACE9 m_Surface;

	MkInt2 m_Size;
	eMinMagFilterType m_MinMagFilter;

	bool m_HasAlphaChannel;
	eTextureBlendType m_BlendType;

	// bitmap pool 공유를 위한 key
	MkHashStr m_PoolKey;
};

//------------------------------------------------------------------------------------------------//
