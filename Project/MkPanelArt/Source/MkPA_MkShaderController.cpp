
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkShaderEffectPool.h"
#include "MkPA_MkShaderController.h"


//------------------------------------------------------------------------------------------------//

bool MkShaderController::SetShaderEffect(const MkHashStr& name)
{
	m_ShaderEffect = MK_SHADER_POOL.GetShaderEffect(name);
	return (m_ShaderEffect != NULL);
}

const MkHashStr& MkShaderController::GetShaderEffectName(void) const
{
	return (m_ShaderEffect == NULL) ? MkHashStr::EMPTY : m_ShaderEffect->GetName();
}

void MkShaderController::Clear(void)
{
	m_ShaderEffect = NULL;
	m_AssignPack.Clear();
	m_ShaderTexture0 = NULL;
	m_ShaderTexture1 = NULL;
	m_ShaderTexture2 = NULL;
}

unsigned int MkShaderController::BeginTechnique(void)
{
	unsigned int passCount = 0;
	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
	if (device != NULL)
	{
		// default shader
		if (m_ShaderEffect == NULL)
		{
			device->SetVertexShader(NULL);
			device->SetPixelShader(NULL);
		}
		// run effect
		else
		{
			passCount = m_ShaderEffect->BeginTechnique(m_AssignPack);
		}
	}
	return passCount;
}

void MkShaderController::BeginPass(unsigned int index)
{
	if (m_ShaderEffect != NULL)
	{
		m_ShaderEffect->BeginPass(index);
	}
}

void MkShaderController::EndPass(void)
{
	if (m_ShaderEffect != NULL)
	{
		m_ShaderEffect->EndPass();
	}
}

void MkShaderController::EndTechnique(void)
{
	if (m_ShaderEffect != NULL)
	{
		m_ShaderEffect->EndTechnique();
	}
}

MkShaderController::MkShaderController()
{
	m_ShaderEffect = NULL;

	// 모든 rect의 wvp는 identity
	D3DXMATRIX wvp;
	D3DXMatrixIdentity(&wvp);
	m_AssignPack.SetWorldViewProjection(wvp);
}

//------------------------------------------------------------------------------------------------//
