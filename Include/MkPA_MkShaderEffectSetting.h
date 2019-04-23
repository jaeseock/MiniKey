#pragma once


//------------------------------------------------------------------------------------------------//
// object별 shader effect 설정
// - technique
// - alpha
// - texture0, 1, 2, 3으로 총 4개까지 설정 가능
// - user defined property(shader code에 선언된 변수명으로 설정)
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashStr.h"
#include "MkCore_MkHashMap.h"

#include "MkPA_MkD3DDefinition.h"


class MkBaseTexture;

class MkShaderEffectSetting
{
public:

	inline void SetTechnique(const MkHashStr& name) { m_Technique = name; }
	inline const MkHashStr& GetTechnique(void) const { return m_Technique; }

	inline void SetAlpha(float alpha) { m_Alpha = alpha; }
	inline float GetAlpha(void) const { return m_Alpha; }

	inline void SetTexture0(MkBaseTexture* texture) { m_Texture0 = texture; }
	inline MkBaseTexture* GetTexture0(void) const { return m_Texture0; }

	inline void SetTexture1(MkBaseTexture* texture) { m_Texture1 = texture; }
	inline MkBaseTexture* GetTexture1(void) const { return m_Texture1; }

	inline void SetTexture2(MkBaseTexture* texture) { m_Texture2 = texture; }
	inline MkBaseTexture* GetTexture2(void) const { return m_Texture2; }

	inline void SetTexture3(MkBaseTexture* texture) { m_Texture3 = texture; }
	inline MkBaseTexture* GetTexture3(void) const { return m_Texture3; }

	inline void CreateUDP(const MkHashStr& name, const D3DXVECTOR4& value) { if (m_UDP.Exist(name)) { m_UDP[name] = value; } else { m_UDP.Create(name, value); } }
	inline bool SetUDP(const MkHashStr& name, const D3DXVECTOR4& value) { bool ok = m_UDP.Exist(name); if (ok) { m_UDP[name] = value; } return ok; }
	inline bool GetUDP(const MkHashStr& name, D3DXVECTOR4& buffer) const { bool ok = m_UDP.Exist(name); if (ok) { buffer = m_UDP[name]; } return ok; }
	inline const D3DXVECTOR4& GetUDP(const MkHashStr& name) const { return m_UDP[name]; } // 예외처리 안함
	inline const MkHashMap<MkHashStr, D3DXVECTOR4>& GetUDP(void) const { return m_UDP; }

	MkShaderEffectSetting() {}
	~MkShaderEffectSetting() {}

protected:

	MkHashStr m_Technique;
	float m_Alpha;
	MkBaseTexture* m_Texture0;
	MkBaseTexture* m_Texture1;
	MkBaseTexture* m_Texture2;
	MkBaseTexture* m_Texture3;
	MkHashMap<MkHashStr, D3DXVECTOR4> m_UDP;
};
