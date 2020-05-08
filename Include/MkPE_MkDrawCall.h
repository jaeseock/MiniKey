#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkRect.h"
#include "MkCore_MkHashStr.h"

#include "MkPE_MkD3DDefinition.h"


class MkPathName;
class MkBaseTexture;

class MkDrawCall
{
public:

	// 알파
	void SetAlpha(float alpha);
	float GetAlpha(void) const;

	// 텍스쳐
	bool SetTexture(int index, const MkPathName& filePath); // 파일 경로로부터
	bool SetTexture(int index, MkBaseTexture* texture); // 외부 텍스쳐로부터
	bool SetTexture(int index, int sourceOutputIndex); // source render target으로부터
	MkBaseTexture* GetTexture(int index) const;

	// shader effect. 비었으면 지정된 effect 없음
	bool SetShaderEffect(const MkHashStr& name);
	inline const MkHashStr& GetShaderEffect(void) const { return m_EffectName; }

	// technique. 비었으면 해당 effect의 기본 테크닉 사용
	// (NOTE) 호출 전 shader effect가 설정 되어 있어야 함
	bool SetTechnique(const MkHashStr& technique);
	inline const MkHashStr& GetTechnique(void) const { return m_Technique; }

	// User Defined Property
	void SetUDP(const MkHashStr& key, const D3DXVECTOR4& value);
	bool GetUDP(const MkHashStr& key, D3DXVECTOR4& buffer) const;

	// 해제
	void Clear(void);

	MkDrawCall();
	~MkDrawCall() { Clear(); }

	void __Draw(LPDIRECT3DDEVICE9 device, const MkFloat2& screenSize);

protected:

	void _FillVertexData(MkFloatRect::ePointName pn, const MkFloat2& size, MkByteArray& buffer) const;
	void _FillVertexData(const MkFloat2& size, MkByteArray& buffer) const;

	typedef struct __UDP_Unit
	{
		MkHashStr key;
		D3DXVECTOR4 value;
	}
	_UDP_Unit;

protected:

	// default
	DWORD m_Alpha;
	MkBaseTexture* m_Texture[MKDEF_MAX_TEXTURE_COUNT];

	// effect
	MkHashStr m_EffectName;
	MkHashStr m_Technique;
	MkArray<_UDP_Unit> m_UDP;
};
