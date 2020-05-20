
#include "MkCore_MkDataNode.h"

#include "SceneSelectionWindowNode.h"
#include "SettingRepository.h"


//------------------------------------------------------------------------------------------------//

void SettingRepository::SetUp(const SceneSelectionWindowNode* sceneWindowNode)
{
	m_SceneWindowNode = sceneWindowNode;
}

bool SettingRepository::SaveEffectSetting(const MkPathName& filePath) const
{
	MkDataNode node;

	if (!m_SceneKey.Empty())
	{
		node.CreateUnitEx(L"Scene", m_SceneKey);
	}

	if (m_ColorEditEnable)
	{
		MkDataNode* effNode = node.CreateChildNode(L"ColorEdit");
		effNode->CreateUnit(L"Contrast", m_ColorEditContrast);
		effNode->CreateUnit(L"Saturation", m_ColorEditSaturation);
		effNode->CreateUnit(L"Brightness", m_ColorEditBrightness);
		effNode->CreateUnit(L"ColorBalance", MkVec3(m_ColorEditColorBalanceR, m_ColorEditColorBalanceG, m_ColorEditColorBalanceB));
	}

	if (m_DepthFogEnable)
	{
		MkDataNode* effNode = node.CreateChildNode(L"DepthFog");
		effNode->CreateUnit(L"Mixer", m_DepthFogMixer);
		effNode->CreateUnitEx(L"MultMap", m_DepthFogMultMap);
		effNode->CreateUnitEx(L"AddMap", m_DepthFogAddMap);
	}

	if (m_EdgeDetectionEnable)
	{
		MkDataNode* effNode = node.CreateChildNode(L"EdgeDetection");
		effNode->CreateUnit(L"Amplifier", m_EdgeDetectionAmplifier);
		effNode->CreateUnit(L"EdgeColor", m_EdgeDetectionEdgeColor);
	}

	if (m_FakeHDREnable)
	{
		MkDataNode* effNode = node.CreateChildNode(L"FakeHDR");
		effNode->CreateUnit(L"PowerHigh", m_FakeHDRPowerHigh);
		effNode->CreateUnit(L"PowerLow", m_FakeHDRPowerLow);
	}

	if (m_DepthOfFieldEnable)
	{
		MkDataNode* effNode = node.CreateChildNode(L"DepthOfField");
		effNode->CreateUnit(L"Position", m_DepthOfFieldPosition);
		effNode->CreateUnit(L"Range", m_DepthOfFieldRange);
	}

	if (m_RadialBlurEnable)
	{
		MkDataNode* effNode = node.CreateChildNode(L"RadialBlur");
		effNode->CreateUnit(L"Position", MkVec2(m_RadialBlurPosition.x, m_RadialBlurPosition.y));
		effNode->CreateUnit(L"StartRange", m_RadialBlurStartRange);
		effNode->CreateUnit(L"Power", m_RadialBlurPower);
	}

	return node.SaveToText(filePath);
}

bool SettingRepository::LoadEffectSetting(const MkPathName& filePath)
{
	MkDataNode setting;
	bool ok = setting.Load(filePath);
	if (ok)
	{
		{
			MkHashStr sceneKey;
			if ((m_SceneWindowNode != NULL) &&
				setting.GetDataEx(L"Scene", sceneKey, 0) &&
				m_SceneWindowNode->IsValidScene(sceneKey))
			{
				m_SceneKey = sceneKey;
			}
		}
		{
			const MkDataNode* effNode = setting.GetChildNode(L"ColorEdit");
			m_ColorEditEnable = (effNode != NULL);
			if (m_ColorEditEnable)
			{
				effNode->GetData(L"Contrast", m_ColorEditContrast, 0);
				effNode->GetData(L"Saturation", m_ColorEditSaturation, 0);
				effNode->GetData(L"Brightness", m_ColorEditBrightness, 0);
				MkVec3 buffer;
				effNode->GetData(L"ColorBalance", buffer, 0);
				m_ColorEditColorBalanceR = buffer.x;
				m_ColorEditColorBalanceG = buffer.y;
				m_ColorEditColorBalanceB = buffer.z;
			}
		}
		{
			const MkDataNode* effNode = setting.GetChildNode(L"DepthFog");
			m_DepthFogEnable = (effNode != NULL);
			if (m_DepthFogEnable)
			{
				effNode->GetData(L"Mixer", m_DepthFogMixer, 0);
				effNode->GetDataEx(L"MultMap", m_DepthFogMultMap, 0);
				effNode->GetDataEx(L"AddMap", m_DepthFogAddMap, 0);
			}
		}
		{
			const MkDataNode* effNode = setting.GetChildNode(L"EdgeDetection");
			m_EdgeDetectionEnable = (effNode != NULL);
			if (m_EdgeDetectionEnable)
			{
				effNode->GetData(L"Amplifier", m_EdgeDetectionAmplifier, 0);
				effNode->GetData(L"EdgeColor", m_EdgeDetectionEdgeColor, 0);
			}
		}
		{
			const MkDataNode* effNode = setting.GetChildNode(L"FakeHDR");
			m_FakeHDREnable = (effNode != NULL);
			if (m_FakeHDREnable)
			{
				effNode->GetData(L"PowerHigh", m_FakeHDRPowerHigh, 0);
				effNode->GetData(L"PowerLow", m_FakeHDRPowerLow, 0);
			}
		}
		{
			const MkDataNode* effNode = setting.GetChildNode(L"DepthOfField");
			m_DepthOfFieldEnable = (effNode != NULL);
			if (m_DepthOfFieldEnable)
			{
				effNode->GetData(L"Position", m_DepthOfFieldPosition, 0);
				effNode->GetData(L"Range", m_DepthOfFieldRange, 0);
			}
		}
		{
			const MkDataNode* effNode = setting.GetChildNode(L"RadialBlur");
			m_RadialBlurEnable = (effNode != NULL);
			if (m_RadialBlurEnable)
			{
				MkVec2 buffer;
				effNode->GetData(L"Position", buffer, 0);
				m_RadialBlurPosition.x = buffer.x;
				m_RadialBlurPosition.y = buffer.y;
				effNode->GetData(L"StartRange", m_RadialBlurStartRange, 0);
				effNode->GetData(L"Power", m_RadialBlurPower, 0);
			}
		}
	}
	return ok;
}

void SettingRepository::ResetColorEditValue(void)
{
	m_ColorEditContrast = 1.f;
	m_ColorEditSaturation = 1.f;
	m_ColorEditBrightness = 1.f;
	m_ColorEditColorBalanceR = 0.f;
	m_ColorEditColorBalanceG = 0.f;
	m_ColorEditColorBalanceB = 0.f;
}

void SettingRepository::ResetDepthFogValue(void)
{
	m_DepthFogMixer = 0.5f;
	m_DepthFogMultMap = L"mult_00";
	m_DepthFogAddMap = L"add_00";
}

void SettingRepository::ResetEdgeDetectionValue(void)
{
	m_EdgeDetectionAmplifier = 8.f;
	m_EdgeDetectionEdgeColor.Clear(); // black
}

void SettingRepository::ResetFakeHDRValue(void)
{
	m_FakeHDRPowerHigh = 0.8f;
	m_FakeHDRPowerLow = 0.3f;
}

void SettingRepository::ResetDepthOfFieldValue(void)
{
	m_DepthOfFieldPosition = 0.0625f;
	m_DepthOfFieldRange = 20.f;
}

void SettingRepository::ResetRadialBlurValue(void)
{
	m_RadialBlurPosition = MkFloat2(0.5f, 0.5f);
	m_RadialBlurStartRange = 0.1f;
	m_RadialBlurPower = 0.01f;
}

SettingRepository& SettingRepository::GetInstance(void)
{
	static SettingRepository instance;
	return instance;
}

SettingRepository::SettingRepository()
{
	m_SceneWindowNode = NULL;

	m_ColorEditEnable = false;
	ResetColorEditValue();
	
	m_DepthFogEnable = false;
	ResetDepthFogValue();

	m_EdgeDetectionEnable = false;
	ResetEdgeDetectionValue();
	m_EdgeDetectionDebugBlend = 1.f;

	m_FakeHDREnable = false;
	ResetFakeHDRValue();
	
	m_DepthOfFieldEnable = false;
	ResetDepthOfFieldValue();
	m_DepthOfFieldDebugBlend = 0.f;

	m_RadialBlurEnable = false;
	ResetRadialBlurValue();
}

//------------------------------------------------------------------------------------------------//
