#pragma once


//------------------------------------------------------------------------------------------------//
// shader effect
// �ϳ��� fxo�� ����
// �ϳ� Ȥ�� ������ technique�� ������ ������ object�� �� �� �ϳ��� technique�� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashStr.h"
//#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
//#include "MkCore_MkValueDecision.h"
#include "MkCore_MkPathName.h"

#include "MkPE_MkD3DDefinition.h"


class MkBaseTexture;
class MkDrawCall;

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
		// �⺻ ���� ���õǸ� MkDrawCall�� ���� ����
		eS_Alpha = 0, // ALPHA

		// screen size(ePT_Float2)
		eS_ScreenSize,

		// texture(ePT_Texture)
		// ������ MKDEF_MAX_TEXTURE_COUNT�� ���� ����
		// �ʱ��� ���� ����(ex> texture diffuseTex : TEXTURE0 <string path = "map\bg.png";>;)
		eS_Texture0, // TEXTURE0
		eS_Texture1, // TEXTURE1
		eS_Texture2, // TEXTURE2
		eS_Texture3, // TEXTURE3
		eS_Texture4, // TEXTURE4
		eS_Texture5, // TEXTURE5
		eS_Texture6, // TEXTURE6
		eS_Texture7, // TEXTURE7

		// user defined property(ePT_Float, ePT_Float2, ePT_Float3, ePT_Float4)
		// ex> float4 colorFactor : UDP = float4(1.f, 0.5f, 0.2f, 1.f);
		eS_UDP // UDP
	};

	// �ʱ�ȭ
	bool SetUp(LPDIRECT3DDEVICE9 device, const MkPathName& filePath);

	// fxo, default texture ���� ������ �˻��� �ʿ�� ���ʱ�ȭ
	void CheckAndReload(LPDIRECT3DDEVICE9 device);

	// ��� ������ technique���� ��ȯ
	bool IsValidTechnique(const MkHashStr& name) const;

	// draw
	unsigned int BeginTechnique(const MkDrawCall& drawCall, const MkFloat2& screenSize);
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

	void _UpdateHandle(void);

	template<class DataType>
	class _ParameterInfo
	{
	public:
		D3DXHANDLE handle;
		eParamType paramType;
		DataType defValue;

		void SetUp(D3DXHANDLE h, eParamType pt, DataType dv);
		_ParameterInfo();
	};

	typedef struct __ResourceInfo
	{
		MkPathName path;
		unsigned int writtenTime;
	}
	_ResourceInfo;

protected:

	// resource files
	_ResourceInfo m_FxoInfo;
	_ResourceInfo m_DefaultMapInfo[MKDEF_MAX_TEXTURE_COUNT];

	LPD3DXEFFECT m_Effect; // ����Ʈ ��ü

	MkHashMap<MkHashStr, D3DXHANDLE> m_Techniques; // ���� ��ũ��
	D3DXHANDLE m_DefaultTechnique;
	
	// parameter
	_ParameterInfo<float> m_Alpha;
	_ParameterInfo<MkFloat2> m_ScreenSize;
	_ParameterInfo<MkBaseTexture*> m_Texture[MKDEF_MAX_TEXTURE_COUNT];
	MkHashMap< MkHashStr, _ParameterInfo<D3DXVECTOR4> > m_UDP;
};
