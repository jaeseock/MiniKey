#pragma once


//------------------------------------------------------------------------------------------------//
// 텍스쳐 기본형
// 2D의 경우 많은 설정들을 생략 할 수 있지만 혹시 모르니 남겨 둠
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkType2.h"
#include "MkCore_MkColor.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkSharedPointer.h"

#include "MkPA_MkGlobalDefinition.h"
#include "MkPA_MkD3DDefinition.h"
#include "MkPA_MkImageInfo.h"


class MkPathName;

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

	bool SetUp
		(const MkInt2& size, unsigned int mipLevel, DWORD usage, D3DFORMAT format, // 이미지 데이터에 종속
		eFilterType filterType = ePoint, D3DTEXTUREADDRESS addressMode = D3DTADDRESS_CLAMP, const MkColor& borderColor = MkColor::Black, // 외부 설정
		const MkDataNode* infoNode = NULL);

	// 렌더 스테이트 갱신
	void UpdateRenderState(DWORD objectAlpha) const;

	// 크기 반환
	inline const MkInt2& GetSize(void) const { return m_Size; }

	// alphaRef가 0이면 testing off, 1 ~ 255면 testing on
	inline void SetAlphaTestingRef(unsigned int alphaRef) { m_AlphaTestingRef = Clamp<unsigned int>(alphaRef, 0, 0xff); }
	inline unsigned int GetAlphaTestingRef(void) const { return m_AlphaTestingRef; }

	// min/mag filter
	void SetFilterType(eFilterType filterType);

	inline eFilterType GetFilterType(void) const { return m_FilterType; }
	inline D3DTEXTUREFILTERTYPE GetMinMagFilter(void) const { return m_MinMagFilter; }
	inline unsigned int GetMaxAnisotropic(void) const { return m_MaxAnisotropic; }

	// mipmap filter 반환
	inline bool GetUseMipmap(void) const { return m_UseMipmap; }
	inline D3DTEXTUREFILTERTYPE GetMipmapFilter(void) const { return ((m_UseMipmap) ? D3DTEXF_LINEAR : D3DTEXF_NONE); }

	// address mode 반환
	inline D3DTEXTUREADDRESS GetAddressMode(void) const { return m_AddressMode; }

	// border color 반환
	inline D3DCOLOR GetBorderColor(void) const { return m_BorderColor; }

	// texture 반환
	inline const LPDIRECT3DTEXTURE9 GetTexture(void) const { return m_Texture; }

	// surface 반환
	inline LPDIRECT3DSURFACE9 GetSurface(void) const { return m_Surface; }

	// format 반환
	inline D3DFORMAT GetFormat(void) const { return m_Format; }

	// alpha channel 유무 반환
	inline bool HasAlphaChannel(void) const { return m_HasAlphaChannel; }

	// blend type
	inline void SetBlendType(ePA_MaterialBlendType blendType) { m_BlendType = blendType; }
	inline ePA_MaterialBlendType GetBlendType(void) const { return m_BlendType; }

	// pool용 key
	inline void SetPoolKey(const MkHashStr& key) { m_PoolKey = key; }
	inline const MkHashStr& GetPoolKey(void) const { return m_PoolKey; }

	// image info
	inline const MkImageInfo& GetImageInfo(void) const { return m_Info; }
	inline void ResetDefaultSubsetInfo(const MkInt2& srcSize, const MkInt2& realSize) { m_Info.ResetDefaultSubsetInfo(srcSize, realSize); }

	// 파일로 저장
	bool SaveToPNG(const MkPathName& filePath) const;

	// 픽셀 정보 반환
	bool GetPixelTable(MkArray<MkColor>& buffer);

	// 해당 포맷이 알파 포맷인지 체크
	static bool IsAlphaFormat(D3DFORMAT format);

	// size의 x, y가 1 이상일 경우 2의 n승을 만족하는 최소 크기를 반환
	static MkInt2 GetFormalSize(const MkInt2 srcSize);

	MkBaseTexture();
	virtual ~MkBaseTexture() { _Clear(); }

protected:

	void _Clear(void);

protected:

	MkInt2 m_Size;
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
	ePA_MaterialBlendType m_BlendType;

	// bitmap pool 공유를 위한 key
	MkHashStr m_PoolKey;

	// info
	MkImageInfo m_Info;
};

typedef MkSharedPointer<MkBaseTexture> MkBaseTexturePtr;

//------------------------------------------------------------------------------------------------//
