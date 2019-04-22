
#include "MkCore_MkCheck.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkDevPanel.h"

#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkShaderEffect.h"
#include "MkPA_MkShaderEffectPool.h"


//------------------------------------------------------------------------------------------------//

void MkShaderEffectPool::SetUp(const MkArray<MkStr>& filePathList)
{
	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
	MK_CHECK(device != NULL, L"NULL device여서 MkShaderEffectPool 초기화 실패")
		return;

	MK_INDEXING_LOOP(filePathList, i)
	{
		MkPathName currPath = filePathList[i];
		MkHashStr name = currPath.GetFileName(false);
		if (!m_Pool.Exist(name))
		{
			MkShaderEffect* effect = new MkShaderEffect;
			MK_CHECK(effect != NULL, name.GetString() + L" effect 객체 생성 실패")
				continue;

			if (effect->SetUp(currPath))
			{
				m_Pool.Create(name, effect);
			}
			else
			{
				delete effect;
			}
		}
	}

	if (!m_Pool.Empty())
	{
		MK_DEV_PANEL.InsertEmptyLine();
	}
}

MkShaderEffect* MkShaderEffectPool::GetShaderEffect(const MkHashStr& name) const
{
	return m_Pool.Exist(name) ? m_Pool[name] : NULL;
}

void MkShaderEffectPool::Clear(void)
{
	MkHashMapLooper<MkHashStr, MkShaderEffect*> looper(m_Pool);
	MK_STL_LOOP(looper)
	{
		delete looper.GetCurrentField();
	}
	m_Pool.Clear();
}

void MkShaderEffectPool::UnloadResource(void)
{
	MkHashMapLooper<MkHashStr, MkShaderEffect*> looper(m_Pool);
	MK_STL_LOOP(looper)
	{
		looper.GetCurrentField()->UnloadResource();
	}
}

void MkShaderEffectPool::ReloadResource(LPDIRECT3DDEVICE9 device)
{
	MkHashMapLooper<MkHashStr, MkShaderEffect*> looper(m_Pool);
	MK_STL_LOOP(looper)
	{
		looper.GetCurrentField()->ReloadResource();
	}
}

//------------------------------------------------------------------------------------------------//
