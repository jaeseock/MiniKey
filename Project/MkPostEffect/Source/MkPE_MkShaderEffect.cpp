
#include "MkCore_MkCheck.h"
#include "MkCore_MkFileManager.h"
//#include "MkCore_MkDevPanel.h"

//#include "MkPE_MkDeviceManager.h"
#include "MkPE_MkBitmapPool.h"
#include "MkPE_MkDrawCall.h"
#include "MkPE_MkShaderEffect.h"


//------------------------------------------------------------------------------------------------//

bool MkShaderEffect::SetUp(LPDIRECT3DDEVICE9 device, const MkPathName& filePath)
{
	Clear();

	if (device == NULL)
		return false;

	do
	{
		MkPathName absoluteFilePath;
		absoluteFilePath.ConvertToRootBasisAbsolutePath(filePath);

		// file data
		MkByteArray srcData;
		MK_CHECK(MkFileManager::GetFileData(absoluteFilePath, srcData), L"이펙트 파일 로딩 실패 : " + filePath)
			break;

		if (srcData.Empty())
			break;

		// effect
		if (FAILED(D3DXCreateEffect(device, srcData.GetPtr(), srcData.GetSize(), NULL, NULL, 0, NULL, &m_Effect, NULL)))
			break;

		// 여기까지 통과했으면 성공
		m_FxoInfo.path = absoluteFilePath;
		m_FxoInfo.writtenTime = m_FxoInfo.path.GetWrittenTime();

		_UpdateHandle();
		return true;
	}
	while (false);

	Clear();
	return false;
}

void MkShaderEffect::CheckAndReload(LPDIRECT3DDEVICE9 device)
{
	unsigned int writtenTime = m_FxoInfo.path.GetWrittenTime();
	if (writtenTime > 0)
	{
		// fxo 파일 체크해 재로딩. 기본 텍스쳐 포함
		if (writtenTime != m_FxoInfo.writtenTime)
		{
			MkPathName srcPath = m_FxoInfo.path;
			if (SetUp(device, srcPath))
			{
				m_FxoInfo.writtenTime = writtenTime;
			}
		}
		// 텍스쳐만 체크해 재로딩
		else
		{
			for (int i=0; i<MKDEF_MAX_TEXTURE_COUNT; ++i)
			{
				MkBaseTexture* defBitmap = m_Texture[i].defValue;
				if ((defBitmap != NULL) && (!defBitmap->GetPoolKey().Empty()))
				{
					unsigned int writtenTime = m_DefaultMapInfo[i].path.GetWrittenTime();
					if ((writtenTime > 0) && (writtenTime != m_DefaultMapInfo[i].writtenTime))
					{
						MkHashStr key = defBitmap->GetPoolKey();

						MK_BITMAP_POOL.UnloadBitmapTexture(key);
						m_Texture[i].defValue = MK_BITMAP_POOL.GetBitmapTexture(key);

						m_DefaultMapInfo[i].writtenTime = writtenTime;
					}
				}
			}
		}
	}
}

bool MkShaderEffect::IsValidTechnique(const MkHashStr& name) const
{
	return m_Techniques.Exist(name);
}

unsigned int MkShaderEffect::BeginTechnique(const MkDrawCall& drawCall, const MkFloat2& screenSize)
{
	if (m_Effect != NULL)
	{
		// technique. drawCall 우선
		{
			const MkHashStr& value = drawCall.GetTechnique();
			m_Effect->SetTechnique((value.Empty() || (!IsValidTechnique(value))) ? m_DefaultTechnique : m_Techniques[value]);
		}

		// 선언이 존재하는 semantic만 반영. 값은 drawCall 우선

		// alpha
		if (m_Alpha.handle != NULL)
		{
			m_Effect->SetFloat(m_Alpha.handle, drawCall.GetAlpha());
		}

		// screen size
		if (m_ScreenSize.handle != NULL)
		{
			m_Effect->SetFloatArray(m_ScreenSize.handle, &screenSize.x, 2);
		}

		// texture
		for (int i=0; i<MKDEF_MAX_TEXTURE_COUNT; ++i)
		{
			if (m_Texture[i].handle != NULL)
			{
				MkBaseTexture* dcTex = drawCall.GetTexture(i);
				MkBaseTexture* effTex = (dcTex == NULL) ? m_Texture[i].defValue : dcTex;
				if (effTex != NULL)
				{
					m_Effect->SetTexture(m_Texture[i].handle, effTex->GetTexture());
				}
			}
		}

		MkHashMapLooper< MkHashStr, _ParameterInfo<D3DXVECTOR4> > looper(m_UDP);
		MK_STL_LOOP(looper)
		{
			_ParameterInfo<D3DXVECTOR4>& param = looper.GetCurrentField();

			D3DXVECTOR4 value = param.defValue;
			drawCall.GetUDP(looper.GetCurrentKey(), value);

			switch (param.paramType)
			{
			case ePT_Float:
				m_Effect->SetFloat(param.handle, value.x);
				break;
			case ePT_Float2:
				m_Effect->SetFloatArray(param.handle, &value.x, 2);
				break;
			case ePT_Float3:
				m_Effect->SetFloatArray(param.handle, &value.x, 3);
				break;
			case ePT_Float4:
				m_Effect->SetVector(param.handle, &value);
				break;
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
	m_FxoInfo.path.Clear();
	m_Techniques.Clear();
	m_DefaultTechnique = NULL;

	m_Alpha.SetUp(NULL, ePT_None, 1.f);
	for (int i=0; i<MKDEF_MAX_TEXTURE_COUNT; ++i)
	{
		MkBaseTexture* defBitmap = m_Texture[i].defValue;
		if ((defBitmap != NULL) && (!defBitmap->GetPoolKey().Empty()))
		{
			MK_BITMAP_POOL.UnloadBitmapTexture(defBitmap->GetPoolKey());
		}

		m_Texture[i].SetUp(NULL, ePT_None, NULL);

		m_DefaultMapInfo[i].path.Clear();
	}
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
		_UpdateHandle();
	}
}


MkShaderEffect::MkShaderEffect()
{
	m_Effect = NULL;

	for (int i=0; i<MKDEF_MAX_TEXTURE_COUNT; ++i)
	{
		m_Texture[i].SetUp(NULL, ePT_None, NULL);
	}
}

//------------------------------------------------------------------------------------------------//

static MkHashMap<MkHashStr, MkShaderEffect::eSemantic> SemanticKeys;

MkShaderEffect::eSemantic MkShaderEffect::_GetSemantic(const MkHashStr& key)
{
	if (SemanticKeys.Empty())
	{
		SemanticKeys.Create(L"ALPHA", eS_Alpha);
		SemanticKeys.Create(L"SCRSIZE", eS_ScreenSize);
		SemanticKeys.Create(L"TEXTURE0", eS_Texture0);
		SemanticKeys.Create(L"TEXTURE1", eS_Texture1);
		SemanticKeys.Create(L"TEXTURE2", eS_Texture2);
		SemanticKeys.Create(L"TEXTURE3", eS_Texture3);
		SemanticKeys.Create(L"TEXTURE4", eS_Texture4);
		SemanticKeys.Create(L"TEXTURE5", eS_Texture5);
		SemanticKeys.Create(L"TEXTURE6", eS_Texture6);
		SemanticKeys.Create(L"TEXTURE7", eS_Texture7);
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

void MkShaderEffect::_UpdateHandle(void)
{
	if (m_Effect == NULL)
		return;

	D3DXEFFECT_DESC effectDesc;
	m_Effect->GetDesc(&effectDesc);

	if (effectDesc.Techniques == 0)
		return;

	// technique 정보를 조사해 등록
	for (unsigned int i=0; i<effectDesc.Techniques; ++i)
	{
		D3DXHANDLE techniqueHandle = m_Effect->GetTechnique(i);

		D3DXTECHNIQUE_DESC techDesc;
		m_Effect->GetTechniqueDesc(techniqueHandle, &techDesc);
		MkStr techName;
		techName.ImportMultiByteString(techDesc.Name);

		MkHashStr currName = techName;
		if (m_Techniques.Exist(currName))
		{
			m_Techniques[currName] = techniqueHandle;
		}
		else
		{
			m_Techniques.Create(currName, techniqueHandle);
		}

		if (i == 0) // default는 첫번째 테크닉
		{
			m_Effect->SetTechnique(techniqueHandle);
			m_DefaultTechnique = techniqueHandle;
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
				((semantic == eS_ScreenSize) && (paramType != ePT_Float2)) ||
				(((semantic == eS_Texture0) || (semantic == eS_Texture1) || (semantic == eS_Texture2) || (semantic == eS_Texture3)) && (paramType != ePT_Texture)) ||
				((semantic == eS_UDP) && (paramType != ePT_Float) && (paramType != ePT_Float2) && (paramType != ePT_Float3) && (paramType != ePT_Float4)))
				continue;

			switch (semantic)
			{
			case eS_Alpha:
				m_Alpha.SetUp(paramHandle, paramType, 1.f);
				break;

			case eS_ScreenSize:
				m_ScreenSize.SetUp(paramHandle, paramType, MkFloat2::Zero);
				break;

			case eS_Texture0:
			case eS_Texture1:
			case eS_Texture2:
			case eS_Texture3:
			case eS_Texture4:
			case eS_Texture5:
			case eS_Texture6:
			case eS_Texture7:
				{
					unsigned int texIndex;
					switch (semantic)
					{
					case eS_Texture0: texIndex = 0; break;
					case eS_Texture1: texIndex = 1; break;
					case eS_Texture2: texIndex = 2; break;
					case eS_Texture3: texIndex = 3; break;
					case eS_Texture4: texIndex = 4; break;
					case eS_Texture5: texIndex = 5; break;
					case eS_Texture6: texIndex = 6; break;
					case eS_Texture7: texIndex = 7; break;
					}

					MkBaseTexture* texture = m_Texture[texIndex].defValue;
					if (texture == NULL)
					{
						MkStr annotation;
						if (_GetStringAnnotation(paramHandle, "path", annotation))
						{
							MkPathName pathBuf = annotation;
							if ((!pathBuf.Empty()) && (!pathBuf.IsDirectoryPath()) && MkFileManager::CheckAvailable(pathBuf))
							{
								texture = MK_BITMAP_POOL.GetBitmapTexture(MkHashStr(pathBuf));

								if (texture != NULL)
								{
									m_DefaultMapInfo[texIndex].path.ConvertToRootBasisAbsolutePath(pathBuf);
									m_DefaultMapInfo[texIndex].writtenTime = m_DefaultMapInfo[texIndex].path.GetWrittenTime();
								}
							}
						}
					}
					
					m_Texture[texIndex].SetUp(paramHandle, paramType, texture);
				}
				break;

			case eS_UDP:
				{
					MkStr paramName;
					paramName.ImportMultiByteString(paramDesc.Name);
					MkHashStr udpKey = paramName;
					
					if (m_UDP.Exist(udpKey))
					{
						m_UDP[udpKey].SetUp(paramHandle, paramType, m_UDP[udpKey].defValue);
					}
					else
					{
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
				}
				break;
			}
		}
	}
}

//------------------------------------------------------------------------------------------------//

template<class DataType>
void MkShaderEffect::_ParameterInfo<DataType>::SetUp(D3DXHANDLE h, eParamType pt, DataType dv)
{
	handle = h;
	paramType = pt;
	defValue = dv;
}

template<class DataType>
MkShaderEffect::_ParameterInfo<DataType>::_ParameterInfo()
{
	handle = NULL;
	paramType = ePT_None;
}

//------------------------------------------------------------------------------------------------//