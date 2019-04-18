
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
	MK_CHECK(device != NULL, L"NULL device���� MkShaderEffectPool �ʱ�ȭ ����")
		return;

	MK_INDEXING_LOOP(filePathList, i)
	{
		MkPathName currPath = filePathList[i];
		MkHashStr name = currPath.GetFileName(false);
		if (!m_Pool.Exist(name))
		{
			MkShaderEffect* effect = new MkShaderEffect;
			MK_CHECK(effect != NULL, name.GetString() + L" effect ��ü ���� ����")
				continue;

			if (effect->SetUp(currPath))
			{
				m_Pool.Create(name, effect);

				MK_DEV_PANEL.MsgToLog(name.GetString() + L" effect ����");
			}
			else
			{
				delete effect;
			}
		}
	}

	//if (m_VertexDecl == NULL)
	//{
	//	D3DVERTEXELEMENT9 vertexElement[MAX_FVF_DECL_SIZE];
	//	D3DXDeclaratorFromFVF(MKDEF_PANEL_FVF, vertexElement);
	//	device->CreateVertexDeclaration(vertexElement, &m_VertexDecl);
	//}
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

	//MK_RELEASE(m_VertexDecl);
}

//void MkShaderEffectPool::SetImageRectVertexDeclaration(void)
//{
//	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
//	if ((device != NULL) && (m_VertexDecl != NULL))
//	{
//		device->SetVertexDeclaration(m_VertexDecl);
//	}
//}

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

MkShaderEffectPool::MkShaderEffectPool() : MkBaseResetableResource(), MkSingletonPattern<MkShaderEffectPool>()
{
	//m_VertexDecl = NULL;
}

//------------------------------------------------------------------------------------------------//
