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

	// ����
	void SetAlpha(float alpha);
	float GetAlpha(void) const;

	// �ؽ���
	bool SetTexture(int index, const MkPathName& filePath); // ���� ��ηκ���
	bool SetTexture(int index, MkBaseTexture* texture); // �ܺ� �ؽ��ķκ���
	bool SetTexture(int index, int sourceOutputIndex); // source render target���κ���
	MkBaseTexture* GetTexture(int index) const;

	// shader effect. ������� ������ effect ����
	bool SetShaderEffect(const MkHashStr& name);
	inline const MkHashStr& GetShaderEffect(void) const { return m_EffectName; }

	// technique. ������� �ش� effect�� �⺻ ��ũ�� ���
	// (NOTE) ȣ�� �� shader effect�� ���� �Ǿ� �־�� ��
	bool SetTechnique(const MkHashStr& technique);
	inline const MkHashStr& GetTechnique(void) const { return m_Technique; }

	// User Defined Property
	void SetUDP(const MkHashStr& key, const D3DXVECTOR4& value);
	bool GetUDP(const MkHashStr& key, D3DXVECTOR4& buffer) const;

	// ����
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
