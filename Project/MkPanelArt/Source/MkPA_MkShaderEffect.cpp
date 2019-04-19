
#include "MkCore_MkCheck.h"
#include "MkCore_MkFileManager.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkShaderEffectSetting.h"
#include "MkPA_MkShaderEffect.h"


#define MACRO_UPDATE_TEXTURE(index) \
	if (m_Texture##index.handle != NULL) \
	{ \
		MkBaseTexture* value = (objectSetting == NULL) ? m_Texture##index.defValue : objectSetting->GetTexture##index(); \
		if (m_Texture##index.currValue.Commit(value)) \
		{ \
			m_Effect->SetTexture(m_Texture##index.handle, (value == NULL) ? NULL : value->GetTexture()); \
		} \
	}

//------------------------------------------------------------------------------------------------//

bool MkShaderEffect::SetUp(const MkPathName& filePath)
{
	Clear();

	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
	if (device == NULL)
		return false;

	do
	{
		// file data
		MkByteArray srcData;
		MK_CHECK(MkFileManager::GetFileData(filePath, srcData), L"이펙트 파일 로딩 실패 : " + filePath)
			break;

		if (srcData.Empty())
			break;

		// effect
		if (FAILED(D3DXCreateEffect(device, srcData.GetPtr(), srcData.GetSize(), NULL, NULL, 0, NULL, &m_Effect, NULL)))
			break;

		D3DXEFFECT_DESC effectDesc;
		m_Effect->GetDesc(&effectDesc);

		if (effectDesc.Techniques == 0)
			break;

		// 여기를 통과하면 성공
		// technique 정보를 조사해 등록
		for (unsigned int i=0; i<effectDesc.Techniques; ++i)
		{
			D3DXHANDLE techniqueHandle = m_Effect->GetTechnique(i);

			D3DXTECHNIQUE_DESC techDesc;
			m_Effect->GetTechniqueDesc(techniqueHandle, &techDesc);
			MkStr techName;
			techName.ImportMultiByteString(techDesc.Name);

			m_Techniques.Create(techName, techniqueHandle);

			if (i == 0) // default는 첫번째 테크닉
			{
				m_Effect->SetTechnique(techniqueHandle);
				m_Technique.SetUp(NULL, ePT_None, techName);
			}
		}

		// parameter 정보를 조사해 정의된 semantic이 존재하면 등록
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
				// semantic
				eSemantic semantic = _GetSemantic(semanticKey);

				// 정의되지 않은 semantic 이면 무시
				if (semantic == eS_None)
					continue;

				// param type
				eParamType paramType = _GetParamType(paramDesc);

				// param type이 일치하지 않는 semantic 이면 무시
				if (((semantic == eS_Alpha) && (paramType != ePT_Float)) ||
					(((semantic == eS_Texture0) || (semantic == eS_Texture1) || (semantic == eS_Texture2) || (semantic == eS_Texture3)) && (paramType != ePT_Texture)) ||
					((semantic == eS_UDP) && (paramType != ePT_Float) && (paramType != ePT_Float2) && (paramType != ePT_Float3) && (paramType != ePT_Float4)))
					continue;

				switch (semantic)
				{
				case eS_Alpha:
					m_Alpha.SetUp(paramHandle, paramType, 1.f);
					break;

				case eS_Texture0:
				case eS_Texture1:
				case eS_Texture2:
				case eS_Texture3:
					{
						MkBaseTexture* texture = NULL;
						MkPathName pathBuf;
						if (_GetStringAnnotation(paramHandle, "path", pathBuf) && (!pathBuf.Empty()) && (!pathBuf.IsDirectoryPath()) && MkFileManager::CheckAvailable(pathBuf))
						{
							texture = MK_BITMAP_POOL.GetBitmapTexture(MkHashStr(pathBuf));
						}

						switch (semantic)
						{
						case eS_Texture0: m_Texture0.SetUp(paramHandle, paramType, texture); break;
						case eS_Texture1: m_Texture1.SetUp(paramHandle, paramType, texture); break;
						case eS_Texture2: m_Texture2.SetUp(paramHandle, paramType, texture); break;
						case eS_Texture3: m_Texture3.SetUp(paramHandle, paramType, texture); break;
						}
					}
					break;

				case eS_UDP:
					{
						MkStr paramName;
						paramName.ImportMultiByteString(paramDesc.Name);
						MkHashStr udpKey = paramName;

						D3DXVECTOR4 defValue(0.f, 0.f, 0.f, 0.f);
						switch (paramType)
						{
						case ePT_Float: m_Effect->GetFloat(paramHandle, &defValue.x); break;
						case ePT_Float2: m_Effect->GetFloatArray(paramHandle, &defValue.x, 2); break;
						case ePT_Float3: m_Effect->GetFloatArray(paramHandle, &defValue.x, 3); break;
						case ePT_Float4: m_Effect->GetVector(paramHandle, &defValue); break;
						}

						m_UDP.Create(udpKey).SetUp(paramHandle, paramType, defValue);
					}
					break;
				}
			}
		}
		return true;
	}
	while (false);

	Clear();
	return false;
}

bool MkShaderEffect::IsValidTechnique(const MkHashStr& name) const
{
	return m_Techniques.Exist(name);
}

MkShaderEffectSetting* MkShaderEffect::CreateEffectSetting(void) const
{
	if (m_Effect == NULL)
		return NULL;

	MkShaderEffectSetting* instance = new MkShaderEffectSetting;
	if (instance == NULL)
		return NULL;

	instance->SetTechnique(m_Technique.defValue);
	
	if (m_Alpha.handle != NULL)
	{
		instance->SetAlpha(m_Alpha.defValue);
	}
	if (m_Texture0.handle != NULL)
	{
		instance->SetTexture0(m_Texture0.defValue);
	}
	if (m_Texture1.handle != NULL)
	{
		instance->SetTexture1(m_Texture1.defValue);
	}
	if (m_Texture2.handle != NULL)
	{
		instance->SetTexture2(m_Texture2.defValue);
	}
	if (m_Texture3.handle != NULL)
	{
		instance->SetTexture3(m_Texture3.defValue);
	}
	if (!m_UDP.Empty())
	{
		MkConstHashMapLooper< MkHashStr, _ParameterInfo<D3DXVECTOR4> > looper(m_UDP);
		MK_STL_LOOP(looper)
		{
			instance->SetUDP(looper.GetCurrentKey(), looper.GetCurrentField().defValue);
		}
	}
	return instance;
}

unsigned int MkShaderEffect::BeginTechnique(const MkShaderEffectSetting* objectSetting)
{
	if (m_Effect != NULL)
	{
		// technique. objectSetting 우선
		{
			const MkHashStr& value = (objectSetting == NULL) ? m_Technique.defValue : objectSetting->GetTechnique();
			if (m_Technique.currValue.Commit(value))
			{
				m_Effect->SetTechnique(m_Techniques[value]);
			}
		}
		
		// semantic 반영. objectSetting 우선
		if (m_Alpha.handle != NULL)
		{
			float value = (objectSetting == NULL) ? m_Alpha.defValue : objectSetting->GetAlpha();
			if (m_Alpha.currValue.Commit(value))
			{
				m_Effect->SetFloat(m_Alpha.handle, value);
			}
		}

		MACRO_UPDATE_TEXTURE(0);
		MACRO_UPDATE_TEXTURE(1);
		MACRO_UPDATE_TEXTURE(2);
		MACRO_UPDATE_TEXTURE(3);
		
		if (!m_UDP.Empty())
		{
			MkHashMapLooper< MkHashStr, _ParameterInfo<D3DXVECTOR4> > looper(m_UDP);
			MK_STL_LOOP(looper)
			{
				_ParameterInfo<D3DXVECTOR4>& udpParam = looper.GetCurrentField();
				const D3DXVECTOR4& value = (objectSetting == NULL) ? udpParam.defValue : objectSetting->GetUDP(looper.GetCurrentKey());
				if (udpParam.currValue.Commit(value))
				{
					switch (udpParam.paramType)
					{
					case ePT_Float:
						m_Effect->SetFloat(udpParam.handle, value.x);
						break;
					case ePT_Float2:
						m_Effect->SetFloatArray(udpParam.handle, &value.x, 2);
						break;
					case ePT_Float3:
						m_Effect->SetFloatArray(udpParam.handle, &value.x, 3);
						break;
					case ePT_Float4:
						m_Effect->SetVector(udpParam.handle, &value);
						break;
					}
				}
			}
		}

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

void MkShaderEffect::Clear(void)
{
	m_Techniques.Clear();
	
	m_Technique.SetUp(NULL, ePT_None, MkHashStr::EMPTY);
	m_Alpha.SetUp(NULL, ePT_None, 1.f);
	m_Texture0.SetUp(NULL, ePT_None, NULL);
	m_Texture1.SetUp(NULL, ePT_None, NULL);
	m_Texture2.SetUp(NULL, ePT_None, NULL);
	m_Texture3.SetUp(NULL, ePT_None, NULL);
	m_UDP.Clear();

	MK_RELEASE(m_Effect);
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
}

//------------------------------------------------------------------------------------------------//

static MkHashMap<MkHashStr, MkShaderEffect::eSemantic> SemanticKeys;

MkShaderEffect::eSemantic MkShaderEffect::_GetSemantic(const MkHashStr& key)
{
	if (SemanticKeys.Empty())
	{
		SemanticKeys.Create(L"ALPHA", eS_Alpha);
		SemanticKeys.Create(L"TEXTURE0", eS_Texture0);
		SemanticKeys.Create(L"TEXTURE1", eS_Texture1);
		SemanticKeys.Create(L"TEXTURE2", eS_Texture2);
		SemanticKeys.Create(L"TEXTURE3", eS_Texture3);
		SemanticKeys.Create(L"UDP", eS_UDP);
	}

	return SemanticKeys.Exist(key) ? SemanticKeys[key] : eS_None;
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

bool MkShaderEffect::_GetStringAnnotation(D3DXHANDLE paramHandle, const char* name, MkStr& buffer) const
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

//------------------------------------------------------------------------------------------------//

template<class DataType>
void MkShaderEffect::_ParameterInfo<DataType>::SetUp(D3DXHANDLE h, eParamType pt, DataType dv)
{
	handle = h;
	paramType = pt;
	defValue = dv;
	currValue.SetUp(defValue);
}

template<class DataType>
MkShaderEffect::_ParameterInfo<DataType>::_ParameterInfo()
{
	handle = NULL;
	currValue.SetUp();
}

//------------------------------------------------------------------------------------------------//