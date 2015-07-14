#pragma once


//------------------------------------------------------------------------------------------------//
// shader effect(fx)
// �ǽð� �Ǽ���(2D ó��)�� �ش�ǹǷ� ������ ũ�� ���� �� ����
// - vs�� ���������� ���� ���� �⺻���� wvp����� uv�� ������ �ǹǷ� �⺻ �ϳ��� ����� ����
// - ps�� ����
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkD3DDefinition.h"
#include "MkCore_MkMap.h"
#include "MkCore_MkHashStr.h"


class MkPathName;
class MkBaseTexture;
class MkShaderEffectAssignPack;

class MkShaderEffect
{
public:

	//------------------------------------------------------------------------------------------------//
	// ���� ���� Ű����
	// (NOTE) ����� MkShaderEffect ������ ���� MkShaderEffectAssignPack���� �ݿ� �� ��
	//------------------------------------------------------------------------------------------------//

	enum eSemantic
	{
		eS_None = -1,

		// transform(ePT_Matrix)
		eS_WorldViewProjection = 0, // WORLDVIEWPROJECTION

		// alpha(ePT_Float)
		eS_Alpha, // ALPHA

		// texture(ePT_Texture). default ����(ex> texture diffuseTex : TEXTURE0 <string def = "map\bg.png";>;)
		eS_DiffuseTexture, // DIFFUSETEX
		eS_ShaderTexture0, // SHADERTEX0
		eS_ShaderTexture1, // SHADERTEX1
		eS_ShaderTexture2, // SHADERTEX2

		// texture size(ePT_Float2)
		eS_DiffuseTexSize, // DTEXSIZE
		eS_ShaderTexSize0, // STEXSIZE0
		eS_ShaderTexSize1, // STEXSIZE1
		eS_ShaderTexSize2, // STEXSIZE2

		// user defined property(ePT_Float4). default ����(ex> float4 colorFactor : UDP <string def = "1, 0.5, 0.2, 1";>;)
		eS_UDP0, // UDP0
		eS_UDP1, // UDP1
		eS_UDP2, // UDP2
		eS_UDP3, // UDP3

		eS_Max
	};

	//------------------------------------------------------------------------------------------------//

	// fxo ���Ϸ� �ʱ�ȭ
	bool SetUp(const MkHashStr& name, const MkPathName& filePath);

	// effect �̸� ��ȯ
	inline const MkHashStr& GetName(void) const { return m_Name; }

	// ����
	void Clear(void);

	// draw
	unsigned int BeginTechnique(const MkShaderEffectAssignPack& objectPack);
	void BeginPass(unsigned int index);
	void EndPass(void);
	void EndTechnique(void);

	// device lost
	void UnloadResource(void);
	void ReloadResource(void);

public:

	MkShaderEffect();
	~MkShaderEffect() { Clear(); }

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

	bool _GetAnnotation(D3DXHANDLE paramHandle, const char* name, MkStr& buffer) const; // name-string�� ���
	MkBaseTexture* _GetDefaultTexture(D3DXHANDLE paramHandle) const;
	bool _GetDefaultVector4(D3DXHANDLE paramHandle, D3DXVECTOR4& buffer) const;
	void _UpdateDefaultFromAnnotation(eSemantic semantic, D3DXHANDLE paramHandle);

	void _ApplyAssignPack(eSemantic semantic, const MkShaderEffectAssignPack& objectPack);

protected:

	MkHashStr m_Name;

	LPD3DXEFFECT m_Effect;

	// semantic
	MkMap<eSemantic, D3DXHANDLE> m_SemanticParameters;
	MkArray<eSemantic> m_OwnedSemanticList;

	// default value
	MkShaderEffectAssignPack* m_DefaultPack;

public:

	static const MkHashStr SemanticKey[eS_Max];
	static const eParamType SemanticParamType[eS_Max];
};
