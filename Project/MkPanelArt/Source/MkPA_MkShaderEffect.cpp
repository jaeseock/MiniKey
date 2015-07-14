
#include "MkCore_MkCheck.h"
#include "MkCore_MkPathName.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkShaderEffectAssignPack.h"
#include "MkPA_MkShaderEffect.h"


const MkHashStr MkShaderEffect::SemanticKey[eS_Max] =
{
	L"WORLDVIEWPROJECTION", // eS_WorldViewProjection
	L"ALPHA", // eS_Alpha
	L"DIFFUSETEX", // eS_DiffuseTexture
	L"SHADERTEX0", // eS_ShaderTexture0
	L"SHADERTEX1", // eS_ShaderTexture1
	L"SHADERTEX2", // eS_ShaderTexture2
	L"DTEXSIZE", // eS_DiffuseTexSize
	L"STEXSIZE0", // eS_ShaderTexSize0
	L"STEXSIZE1", // eS_ShaderTexSize1
	L"STEXSIZE2", // eS_ShaderTexSize2
	L"UDP0", // eS_UDP0
	L"UDP1", // eS_UDP1
	L"UDP2", // eS_UDP2
	L"UDP3" // eS_UDP3
};

const MkShaderEffect::eParamType MkShaderEffect::SemanticParamType[eS_Max] =
{
	ePT_Matrix, // eS_WorldViewProjection
	ePT_Float, // eS_Alpha
	ePT_Texture, // eS_DiffuseTexture
	ePT_Texture, // eS_ShaderTexture0
	ePT_Texture, // eS_ShaderTexture1
	ePT_Texture, // eS_ShaderTexture2
	ePT_Float2, // eS_DiffuseTexSize
	ePT_Float2, // eS_ShaderTexSize0
	ePT_Float2, // eS_ShaderTexSize1
	ePT_Float2, // eS_ShaderTexSize2
	ePT_Float4, // eS_UDP0
	ePT_Float4, // eS_UDP1
	ePT_Float4, // eS_UDP2
	ePT_Float4 // eS_UDP3
};

//------------------------------------------------------------------------------------------------//

bool MkShaderEffect::SetUp(const MkHashStr& name, const MkPathName& filePath)
{
	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
	if (device == NULL)
		return false;

	// name
	m_Name = name;

	// effect setup
	LPD3DXBUFFER msg = NULL;
	bool ok = SUCCEEDED(D3DXCreateEffectFromFile(device, filePath, NULL, NULL, 0, NULL, &m_Effect, &msg));
	if (ok)
	{
		// parameter 정보를 조사해 정의된 semantic이 존재하면 등록
		D3DXEFFECT_DESC effectDesc;
		m_Effect->GetDesc(&effectDesc);
		for (unsigned int i=0; i<effectDesc.Parameters; ++i)
		{
			// param 정보 추출
			D3DXHANDLE paramHandle = m_Effect->GetParameter(NULL, i);
			D3DXPARAMETER_DESC paramDesc;
			m_Effect->GetParameterDesc(paramHandle, &paramDesc);

			// semantic key가 존재하면 진행
			MkHashStr semanticKey;
			if (paramDesc.Semantic != NULL)
			{
				MkStr buffer;
				buffer.ImportMultiByteString(paramDesc.Semantic);
				semanticKey = buffer;
			}
			if (!semanticKey.Empty())
			{
				// param name
				MkStr paramName;
				paramName.ImportMultiByteString(paramDesc.Name);

				// semantic
				eSemantic semantic = _GetSemantic(semanticKey);

				// 정의되지 않은 semantic 이면 무시
				MK_CHECK(semantic != eS_None, m_Name.GetString() + L" effect의 " + paramName + L" param에 정의되지 않은 " + semanticKey.GetString() + L" semantic 적용. param 무시됨")
					continue;

				// param type
				eParamType paramType = _GetParamType(paramDesc);

				// param type이 일치하지 않는 semantic 이면 무시
				MK_CHECK(paramType == SemanticParamType[semantic], m_Name.GetString() + L" effect의 " + paramName + L" param에 적용된 " + semanticKey.GetString() + L" semantic의 허용 type이 다름. param 무시됨")
					continue;

				// semantic 중복은 허용하지 않음
				MK_CHECK(!m_SemanticParameters.Exist(semantic), m_Name.GetString() + L" effect의 " + paramName + L" param에 " + semanticKey.GetString() + L" semantic 중복 적용. param 무시됨")
					continue;

				// 등록
				m_SemanticParameters.Create(semantic) = paramHandle;

				// annotation으로부터 default 값 읽음
				_UpdateDefaultFromAnnotation(semantic, paramHandle);
			}
		}

		// 최초 technique을 default로 설정
		if (effectDesc.Techniques > 0)
		{
			D3DXHANDLE techniqueHandle = m_Effect->GetTechnique(0);
			m_Effect->SetTechnique(techniqueHandle);
		}

		m_SemanticParameters.GetKeyList(m_OwnedSemanticList);
	}
	else if (msg != NULL)
	{
		MkStr errorMsg;
		errorMsg.ImportMultiByteString(reinterpret_cast<const char*>(msg->GetBufferPointer()));
		MK_CHECK(false, m_Name.GetString() + L" effect 초기화 오류 : " + errorMsg) {}
	}

	MK_RELEASE(msg);
	return ok;
}

void MkShaderEffect::Clear(void)
{
	m_Name.Clear();
	MK_RELEASE(m_Effect);
	m_SemanticParameters.Clear();
	m_OwnedSemanticList.Clear();
	MK_DELETE(m_DefaultPack);
}

unsigned int MkShaderEffect::BeginTechnique(const MkShaderEffectAssignPack& objectPack)
{
	if (m_Effect != NULL)
	{
		// 소유한 semantic 대상으로 parameter setting
		MK_INDEXING_LOOP(m_OwnedSemanticList, i)
		{
			_ApplyAssignPack(m_OwnedSemanticList[i], objectPack);
		}

		// commit
		m_Effect->CommitChanges();

		// run
		unsigned int passCount = 0;
		m_Effect->Begin(&passCount, D3DXFX_DONOTSAVESTATE);
		return passCount;
	}
	return 0;
}

void MkShaderEffect::BeginPass(unsigned int index)
{
	if (m_Effect != NULL)
	{
		m_Effect->BeginPass(index);
	}
}

void MkShaderEffect::EndPass(void)
{
	if (m_Effect != NULL)
	{
		m_Effect->EndPass();
	}
}

void MkShaderEffect::EndTechnique(void)
{
	if (m_Effect != NULL)
	{
		m_Effect->End();
	}
}

void MkShaderEffect::UnloadResource(void)
{
	if (m_Effect != NULL)
	{
		m_Effect->OnLostDevice();
	}
}

void MkShaderEffect::ReloadResource(void)
{
	if (m_Effect != NULL)
	{
		m_Effect->OnResetDevice();
	}
}

MkShaderEffect::MkShaderEffect()
{
	m_Effect = NULL;
	m_DefaultPack = NULL;
}

//------------------------------------------------------------------------------------------------//

MkShaderEffect::eSemantic MkShaderEffect::_GetSemantic(const MkHashStr& key)
{
	for (int i=0; i<eS_Max; ++i)
	{
		if (key == SemanticKey[i])
		{
			return static_cast<eSemantic>(i);
		}
	}
	return eS_None;
}

MkShaderEffect::eParamType MkShaderEffect::_GetParamType(const D3DXPARAMETER_DESC& desc)
{
	eParamType paramType = ePT_None;
	switch (desc.Type)
	{
	case D3DXPT_INT:
		{
			if ((desc.Rows == 1) && (desc.Columns == 1))
			{
				paramType = ePT_Integer;
			}
		}
		break;

	case D3DXPT_FLOAT:
		{
			if (desc.Rows == 1)
			{
				switch (desc.Columns)
				{
				case 1: paramType = ePT_Float; break;
				case 2: paramType = ePT_Float2; break;
				case 3: paramType = ePT_Float3; break;
				case 4: paramType = ePT_Float4; break;
				}
			}
			else if ((desc.Rows == 4) && (desc.Columns == 4))
			{
				paramType = ePT_Matrix;
			}
		}
		break;

	case D3DXPT_TEXTURE:
		paramType = ePT_Texture;
		break;
	}

	return paramType;
}

bool MkShaderEffect::_GetAnnotation(D3DXHANDLE paramHandle, const char* name, MkStr& buffer) const
{
	D3DXHANDLE annotHandle = m_Effect->GetAnnotationByName(paramHandle, name);
	if (annotHandle != NULL)
	{
		const char* str = NULL;
		m_Effect->GetString(annotHandle, reinterpret_cast<LPCSTR*>(&str));
		if (str != NULL)
		{
			buffer.ImportMultiByteString(str);
			return true;
		}
	}
	return false;
}

MkBaseTexture* MkShaderEffect::_GetDefaultTexture(D3DXHANDLE paramHandle) const
{
	MkStr msg;
	if (_GetAnnotation(paramHandle, "def", msg) && (!msg.Empty()))
	{
		MkBaseTexture* texture = MK_BITMAP_POOL.GetBitmapTexture(MkHashStr(msg));
		MK_CHECK(texture != NULL, m_Name.GetString() + L" effect에서 default로 지정 된 " + msg + L" texture가 존재하지 않음") {}
		return texture;
	}
	return NULL;
}

bool MkShaderEffect::_GetDefaultVector4(D3DXHANDLE paramHandle, D3DXVECTOR4& buffer) const
{
	MkStr msg;
	if (_GetAnnotation(paramHandle, "def", msg))
	{
		msg.RemoveBlank();

		MkArray<MkStr> tokens;
		unsigned int tokenSize = msg.Tokenize(tokens, L",");
		if (tokenSize > 0)
		{
			buffer = D3DXVECTOR4(tokens[0].ToFloat(), 0.f, 0.f, 0.f);
			if (tokenSize > 1)
			{
				buffer.y = tokens[1].ToFloat();
				if (tokenSize > 2)
				{
					buffer.z = tokens[2].ToFloat();
					if (tokenSize > 3)
					{
						buffer.w = tokens[3].ToFloat();
					}
				}
			}
			return true;
		}
	}
	return false;
}

void MkShaderEffect::_UpdateDefaultFromAnnotation(eSemantic semantic, D3DXHANDLE paramHandle)
{
	switch (semantic)
	{
	case eS_DiffuseTexture:
	case eS_ShaderTexture0:
	case eS_ShaderTexture1:
	case eS_ShaderTexture2:
		{
			MkBaseTexture* def = _GetDefaultTexture(paramHandle);
			if (def != NULL)
			{
				if (m_DefaultPack == NULL)
				{
					m_DefaultPack = new MkShaderEffectAssignPack;
				}
				if (m_DefaultPack != NULL)
				{
					switch (semantic)
					{
					case eS_DiffuseTexture: m_DefaultPack->SetDiffuseTexture(def); break;
					case eS_ShaderTexture0: m_DefaultPack->SetShaderTexture0(def); break;
					case eS_ShaderTexture1: m_DefaultPack->SetShaderTexture1(def); break;
					case eS_ShaderTexture2: m_DefaultPack->SetShaderTexture2(def); break;
					}
				}
			}
		}
		break;

	case eS_UDP0:
	case eS_UDP1:
	case eS_UDP2:
	case eS_UDP3:
		{
			D3DXVECTOR4 def;
			if (_GetDefaultVector4(paramHandle, def))
			{
				if (m_DefaultPack == NULL)
				{
					m_DefaultPack = new MkShaderEffectAssignPack;
				}
				if (m_DefaultPack != NULL)
				{
					switch (semantic)
					{
					case eS_UDP0: m_DefaultPack->SetUDP0(def); break;
					case eS_UDP1: m_DefaultPack->SetUDP1(def); break;
					case eS_UDP2: m_DefaultPack->SetUDP2(def); break;
					case eS_UDP3: m_DefaultPack->SetUDP3(def); break;
					}
				}
			}
		}
		break;
	}
}

void MkShaderEffect::_ApplyAssignPack(eSemantic semantic, const MkShaderEffectAssignPack& objectPack)
{
	// object에 먼저 반영되어 있으면 최우선
	const MkShaderEffectAssignPack* targetPack = objectPack.GetAssigned(semantic) ? &objectPack : NULL;

	// object에는 없는데 자체 default pack이 존재하고 값도 반영되어 있으면
	if ((targetPack == NULL) && (m_DefaultPack != NULL))
	{
		targetPack = m_DefaultPack->GetAssigned(semantic) ? m_DefaultPack : NULL;
	}

	// 값 할당
	if (targetPack != NULL)
	{
		switch (semantic)
		{
		case eS_WorldViewProjection:
			m_Effect->SetMatrix(m_SemanticParameters[semantic], &targetPack->GetWorldViewProjection());
			break;
		case eS_Alpha:
			m_Effect->SetFloat(m_SemanticParameters[semantic], targetPack->GetAlpha());
			break;
		case eS_DiffuseTexture:
			m_Effect->SetTexture(m_SemanticParameters[semantic], targetPack->GetDiffuseTexture());
			break;
		case eS_ShaderTexture0:
			m_Effect->SetTexture(m_SemanticParameters[semantic], targetPack->GetShaderTexture0());
			break;
		case eS_ShaderTexture1:
			m_Effect->SetTexture(m_SemanticParameters[semantic], targetPack->GetShaderTexture1());
			break;
		case eS_ShaderTexture2:
			m_Effect->SetTexture(m_SemanticParameters[semantic], targetPack->GetShaderTexture2());
			break;
		case eS_DiffuseTexSize:
			m_Effect->SetVector(m_SemanticParameters[semantic], &targetPack->GetDiffuseTexSize());
			break;
		case eS_ShaderTexSize0:
			m_Effect->SetVector(m_SemanticParameters[semantic], &targetPack->GetShaderTexSize0());
			break;
		case eS_ShaderTexSize1:
			m_Effect->SetVector(m_SemanticParameters[semantic], &targetPack->GetShaderTexSize1());
			break;
		case eS_ShaderTexSize2:
			m_Effect->SetVector(m_SemanticParameters[semantic], &targetPack->GetShaderTexSize2());
			break;
		case eS_UDP0:
			m_Effect->SetVector(m_SemanticParameters[semantic], &targetPack->GetUDP0());
			break;
		case eS_UDP1:
			m_Effect->SetVector(m_SemanticParameters[semantic], &targetPack->GetUDP1());
			break;
		case eS_UDP2:
			m_Effect->SetVector(m_SemanticParameters[semantic], &targetPack->GetUDP2());
			break;
		case eS_UDP3:
			m_Effect->SetVector(m_SemanticParameters[semantic], &targetPack->GetUDP3());
			break;
		}
	}
}

//------------------------------------------------------------------------------------------------//
