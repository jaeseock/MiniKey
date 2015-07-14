
#include "MkCore_MkCheck.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkDevPanel.h"

#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkShaderEffect.h"
#include "MkPA_MkShaderEffectPool.h"


//------------------------------------------------------------------------------------------------//

void MkShaderEffectPool::SetUp(const MkPathName& shaderDirectory)
{
	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
	MK_CHECK(device != NULL, L"NULL device여서 MkShaderEffectPool 초기화 실패")
		return;

	// 경로 점검
	MkPathName dirPath;
	dirPath.ConvertToRootBasisAbsolutePath(shaderDirectory);
	dirPath.CheckAndAddBackslash();
	MK_CHECK(dirPath.IsDirectoryPath() && dirPath.CheckAvailable(), dirPath + L" 경로가 존재하지 않아 MkShaderEffectPool 초기화 실패")
		return;

	// fxo 파일 검색
	MkArray<MkPathName> filePathList;
	MkArray<MkPathName> emptyFilter;
	MkArray<MkStr> prefixFilter;
	MkArray<MkPathName> extensionFilter;
	extensionFilter.PushBack(L"fxo");
	dirPath.GetWhiteFileList(filePathList, emptyFilter, extensionFilter, prefixFilter, emptyFilter, false, false);

	MK_INDEXING_LOOP(filePathList, i)
	{
		MkHashStr effectName = filePathList[i].GetFileName(false);
		MkPathName currFilePath = dirPath + filePathList[i];
		unsigned int fileSize = currFilePath.GetFileSize();
		unsigned int writtenTime = currFilePath.GetWrittenTime();
		
		if (m_Pool.Exist(effectName))
		{
			_EffectData& ed = m_Pool[effectName];
			if ((ed.fileSize != fileSize) || (ed.fileWrittenTime != writtenTime)) // 크기/수정일시 변경 확인
			{
				ed.effect->Clear();
				if (ed.effect->SetUp(effectName, currFilePath))
				{
					ed.fileSize = fileSize;
					ed.fileWrittenTime = writtenTime;

					MK_DEV_PANEL.MsgToLog(effectName.GetString() + L" effect 갱신");
				}
				else
				{
					delete ed.effect;
					m_Pool.Erase(effectName);
				}
			}
		}
		else
		{
			MkShaderEffect* effect = new MkShaderEffect;
			MK_CHECK(effect != NULL, effectName.GetString() + L" effect 객체 생성 실패")
				continue;

			if (effect->SetUp(effectName, currFilePath))
			{
				_EffectData& ed = m_Pool.Create(effectName);
				ed.effect = effect;
				ed.fileSize = fileSize;
				ed.fileWrittenTime = writtenTime;

				MK_DEV_PANEL.MsgToLog(effectName.GetString() + L" effect 생성");
			}
			else
			{
				delete effect;
			}
		}
	}

	if (m_VertexDecl == NULL)
	{
		D3DVERTEXELEMENT9 vertexElement[MAX_FVF_DECL_SIZE];
		D3DXDeclaratorFromFVF(MKDEF_PANEL_FVF, vertexElement);
		device->CreateVertexDeclaration(vertexElement, &m_VertexDecl);
	}
}

void MkShaderEffectPool::Clear(void)
{
	MkHashMapLooper<MkHashStr, _EffectData> looper(m_Pool);
	MK_STL_LOOP(looper)
	{
		delete looper.GetCurrentField().effect;
	}
	m_Pool.Clear();

	MK_RELEASE(m_VertexDecl);
}

void MkShaderEffectPool::SetImageRectVertexDeclaration(void)
{
	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
	if ((device != NULL) && (m_VertexDecl != NULL))
	{
		device->SetVertexDeclaration(m_VertexDecl);
	}
}

MkShaderEffect* MkShaderEffectPool::GetShaderEffect(const MkHashStr& name) const
{
	return m_Pool.Exist(name) ? m_Pool[name].effect : NULL;
}

void MkShaderEffectPool::UnloadResource(void)
{
	MkHashMapLooper<MkHashStr, _EffectData> looper(m_Pool);
	MK_STL_LOOP(looper)
	{
		looper.GetCurrentField().effect->UnloadResource();
	}
}

void MkShaderEffectPool::ReloadResource(LPDIRECT3DDEVICE9 device)
{
	MkHashMapLooper<MkHashStr, _EffectData> looper(m_Pool);
	MK_STL_LOOP(looper)
	{
		looper.GetCurrentField().effect->ReloadResource();
	}
}

MkShaderEffectPool::MkShaderEffectPool() : MkBaseResetableResource(), MkSingletonPattern<MkShaderEffectPool>()
{
	m_VertexDecl = NULL;
}

//------------------------------------------------------------------------------------------------//
