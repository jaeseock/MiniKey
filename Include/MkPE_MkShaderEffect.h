#pragma once


//------------------------------------------------------------------------------------------------//
// shader effect
// 하나의 fxo에 대응
// 하나 혹은 복수의 technique를 가지고 있으며 object는 이 중 하나의 technique에 대응
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
	// 문맥 정보 키워드. 3d가 아닌 2d에는 필요한 정보가 많지 않다
	// https://docs.microsoft.com/en-us/windows/desktop/direct3dhlsl/dx-graphics-hlsl-semantics
	//------------------------------------------------------------------------------------------------//

	enum eSemantic
	{
		eS_None = -1,

		// alpha(ePT_Float)
		// 기본 값은 무시되며 MkDrawCall의 값을 따름
		eS_Alpha = 0, // ALPHA

		// screen size(ePT_Float2)
		eS_ScreenSize,

		// texture(ePT_Texture)
		// 갯수는 MKDEF_MAX_TEXTURE_COUNT에 의해 결정
		// 초기경로 지정 가능(ex> texture diffuseTex : TEXTURE0 <string path = "map\bg.png";>;)
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

	// 초기화
	bool SetUp(LPDIRECT3DDEVICE9 device, const MkPathName& filePath);

	// fxo, default texture 파일 변경을 검사해 필요시 재초기화
	void CheckAndReload(LPDIRECT3DDEVICE9 device);

	// 사용 가능한 technique여부 반환
	bool IsValidTechnique(const MkHashStr& name) const;

	// draw
	unsigned int BeginTechnique(const MkDrawCall& drawCall, const MkFloat2& screenSize);
	void BeginPass(unsigned int index);
	void EndPass(void);
	void EndTechnique(void);

	// 해제
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

	LPD3DXEFFECT m_Effect; // 이펙트 객체

	MkHashMap<MkHashStr, D3DXHANDLE> m_Techniques; // 보유 테크닉
	D3DXHANDLE m_DefaultTechnique;
	
	// parameter
	_ParameterInfo<float> m_Alpha;
	_ParameterInfo<MkFloat2> m_ScreenSize;
	_ParameterInfo<MkBaseTexture*> m_Texture[MKDEF_MAX_TEXTURE_COUNT];
	MkHashMap< MkHashStr, _ParameterInfo<D3DXVECTOR4> > m_UDP;
};
