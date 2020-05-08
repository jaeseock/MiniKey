
#include "MkCore_MkCheck.h"
#include "MkCore_MkPathName.h"

#include "MkPE_MkShaderEffect.h"
#include "MkPE_MkShaderEffectPool.h"


//------------------------------------------------------------------------------------------------//

void MkShaderEffectPool::SetUp(LPDIRECT3DDEVICE9 device, const MkArray<MkStr>& filePathList, bool autoRefresh)
{
	MK_CHECK(device != NULL, L"NULL device여서 MkShaderEffectPool 초기화 실패")
		return;

	m_RefreshTimeStamp = (autoRefresh) ? ::timeGetTime() : 0;

	MK_INDEXING_LOOP(filePathList, i)
	{
		MkPathName currPath = filePathList[i];
		MkHashStr name = currPath.GetFileName(false);
		if (!m_Pool.Exist(name))
		{
			MkShaderEffect* effect = new MkShaderEffect;
			MK_CHECK(effect != NULL, name.GetString() + L" effect 객체 생성 실패")
				continue;

			if (effect->SetUp(device, currPath))
			{
				m_Pool.Create(name, effect);
			}
			else
			{
				delete effect;
			}
		}
	}
}

void MkShaderEffectPool::CheckAndReload(LPDIRECT3DDEVICE9 device, const MkHashStr& name)
{
	MkShaderEffect* effect = GetShaderEffect(name);
	if (effect != NULL)
	{
		effect->CheckAndReload(device);
	}
}

void MkShaderEffectPool::CheckAndReload(LPDIRECT3DDEVICE9 device, DWORD refreshTime)
{
	if (m_RefreshTimeStamp > 0)
	{
		DWORD currTime = ::timeGetTime();
		if ((currTime - m_RefreshTimeStamp) > refreshTime)
		{
			MkHashMapLooper<MkHashStr, MkShaderEffect*> looper(m_Pool);
			MK_STL_LOOP(looper)
			{
				looper.GetCurrentField()->CheckAndReload(device);
			}

			m_RefreshTimeStamp = currTime;
		}
	}
}

MkShaderEffect* MkShaderEffectPool::GetShaderEffect(const MkHashStr& name) const
{
	return name.Empty() ? NULL : (m_Pool.Exist(name) ? m_Pool[name] : NULL);
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

void MkShaderEffectPool::ReloadResource(void)
{
	MkHashMapLooper<MkHashStr, MkShaderEffect*> looper(m_Pool);
	MK_STL_LOOP(looper)
	{
		looper.GetCurrentField()->ReloadResource();
	}
}

MkShaderEffectPool& MkShaderEffectPool::GetInstance(void)
{
	static MkShaderEffectPool instance;
	return instance;
}

MkShaderEffectPool::MkShaderEffectPool()
{
	m_RefreshTimeStamp = 0;
}

//------------------------------------------------------------------------------------------------//
