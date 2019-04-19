#pragma once


//------------------------------------------------------------------------------------------------//
// shader effect
// �ϳ��� fxo�� ����
// �ϳ� Ȥ�� ������ technique�� ������ ������ object�� �� �� �ϳ��� technique�� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashStr.h"
#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkValueDecision.h"

#include "MkPA_MkD3DDefinition.h"


class MkPathName;
class MkBaseTexture;
class MkShaderEffectSetting;

class MkShaderEffect
{
public:

	//------------------------------------------------------------------------------------------------//
	// ���� ���� Ű����. 3d�� �ƴ� 2d���� �ʿ��� ������ ���� �ʴ�
	// https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx-graphics-hlsl-semantics
	//------------------------------------------------------------------------------------------------//

	enum eSemantic
	{
		eS_None = -1,

		// alpha(ePT_Float)
		eS_Alpha = 0, // ALPHA

		// texture(ePT_Texture)
		// �ʱ��� ���� ����(ex> texture diffuseTex : TEXTURE0 <string path = "map\bg.png";>;)
		eS_Texture0, // TEXTURE0
		eS_Texture1, // TEXTURE1
		eS_Texture2, // TEXTURE2
		eS_Texture3, // TEXTURE3

		// user defined property(ePT_Float, ePT_Float2, ePT_Float3, ePT_Float4)
		// ex> float4 colorFactor : UDP = float4(1.f, 0.5f, 0.2f, 1.f);
		eS_UDP // UDP
	};

	// �ʱ�ȭ
	bool SetUp(const MkPathName& filePath);

	// ��� ������ technique���� ��ȯ
	bool IsValidTechnique(const MkHashStr& name) const;

	// per object
	MkShaderEffectSetting* CreateEffectSetting(void) const;

	// draw
	unsigned int BeginTechnique(const MkShaderEffectSetting* objectSetting);
	void BeginPass(unsigned int index);
	void EndPass(void);
	void EndTechnique(void);

	// ����
	void Clear(void);

	// device lost
	void UnloadResource(void);
	void ReloadResource(void);
	
	MkShaderEffect();
	virtual ~MkShaderEffect() { Clear(); }

protected:

	// parameter type
	enum eParamType
	{
		ePT_None = -1,
		ePT_Integer = 0,
		ePT_Float,
		ePT_Float2,
		ePT_Float3,
		ePT_Float4,
		ePT_Matrix,
		ePT_Texture
	};

	static eSemantic _GetSemantic(const MkHashStr& key);
	static eParamType _GetParamType(const D3DXPARAMETER_DESC& desc);

	bool _GetStringAnnotation(D3DXHANDLE paramHandle, const char* name, MkStr& buffer) const;

	template<class DataType>
	class _ParameterInfo
	{
	public:
		D3DXHANDLE handle;
		eParamType paramType;
		DataType defValue;
		MkValueDecision<DataType> currValue;

		void SetUp(D3DXHANDLE h, eParamType pt, DataType dv);
		_ParameterInfo();
	};

protected:

	MkHashMap<MkHashStr, D3DXHANDLE> m_Techniques; // ���� ��ũ��

	LPD3DXEFFECT m_Effect; // ����Ʈ ��ü

	_ParameterInfo<MkHashStr> m_Technique;
	_ParameterInfo<float> m_Alpha;
	_ParameterInfo<MkBaseTexture*> m_Texture0;
	_ParameterInfo<MkBaseTexture*> m_Texture1;
	_ParameterInfo<MkBaseTexture*> m_Texture2;
	_ParameterInfo<MkBaseTexture*> m_Texture3;
	MkHashMap< MkHashStr, _ParameterInfo<D3DXVECTOR4> > m_UDP;
};
