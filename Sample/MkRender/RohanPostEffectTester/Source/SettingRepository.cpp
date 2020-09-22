
#include "MkCore_MkDataNode.h"

#include "SceneSelectionWindowNode.h"
#include "SettingRepository.h"

#define RDEF_GET_VALUE(values, index, def) (values.IsValidIndex(index) ? values[index] : def)
#define RDEF_SET_VALUE(values, index, data) if (values.IsValidIndex(index)) values[index] = data

//------------------------------------------------------------------------------------------------//

void SettingRepository::SetUp(const SceneSelectionWindowNode* sceneWindowNode)
{
	m_SceneWindowNode = sceneWindowNode;

	// world setting
	MkDataNode node;
	MkPathName filePath;
	filePath.ConvertToRootBasisAbsolutePath(L"WorldSetting.txt");
	if (filePath.CheckAvailable() && node.Load(filePath))
	{
		_CopyEffectSet(node.GetChildNode(L"#Default"), m_DefaultSet);

		MkArray<MkHashStr> keys;
		node.GetChildNodeList(keys);
		unsigned int defPos = keys.FindFirstInclusion(MkArraySection(0), L"#Default");
		if (defPos != MKDEF_ARRAY_ERROR)
		{
			keys.Erase(MkArraySection(defPos, 1));
		}

		MK_INDEXING_LOOP(keys, i)
		{
			MkDataNode* childNode = node.GetChildNode(keys[i]);
			_EffectRegion& region = m_MapData.Create(keys[i]);

			_CopyEffectSet(childNode, region.effectSet);

			childNode->GetData(L"XY", region.xy, 0);

			MkArray<int> rooms;
			childNode->GetData(L"Room", rooms);
			if (rooms.Empty())
			{
				region.room = 0xf; // 1111
			}
			else
			{
				region.room = 0;
				MK_INDEXING_LOOP(rooms, j)
				{
					switch (rooms[j])
					{
					case 1: region.room |= 0x1; break;
					case 2: region.room |= 0x2; break;
					case 3: region.room |= 0x4; break;
					case 4: region.room |= 0x8; break;
					}
				}

				if (region.room == 0)
				{
					region.room = 0xf; // 1111
				}
			}
		}
	}
}

bool SettingRepository::SaveEffectSetting(const MkPathName& filePath) const
{
	MkDataNode node;

	if (!m_SceneKey.Empty())
	{
		node.CreateUnitEx(L"Scene", m_SceneKey);
	}

	MkConstHashMapLooper<MkHashStr, _EffectRegion> looper(m_MapData);
	MK_STL_LOOP(looper)
	{
		MkDataNode* regionNode = node.CreateChildNode(looper.GetCurrentKey());
		const _EffectSet& effSet = looper.GetCurrentField().effectSet;

		// xy
		regionNode->CreateUnit(L"XY", looper.GetCurrentField().xy);

		// room
		regionNode->CreateUnit(L"Room", looper.GetCurrentField().room);

		// color
		if (effSet.color.enable && (effSet.color.value != m_DefaultSet.color.value))
		{
			regionNode->CreateUnit(L"Color", effSet.color.value);
		}

		// fog
		if (effSet.fog.enable && (effSet.fog.value != m_DefaultSet.fog.value))
		{
			regionNode->CreateUnit(L"Fog", effSet.fog.value);
		}

		// hdr
		if (effSet.hdr.enable && (effSet.hdr.value != m_DefaultSet.hdr.value))
		{
			regionNode->CreateUnit(L"HDR", effSet.hdr.value);
		}

		// dof
		if (effSet.dof.enable && (effSet.dof.value != m_DefaultSet.dof.value))
		{
			regionNode->CreateUnit(L"DOF", effSet.dof.value);
		}
	}

	{
		MkDataNode* regionNode = node.CreateChildNode(L"#Default");
		regionNode->CreateUnit(L"Color", m_DefaultSet.color.value);
		regionNode->CreateUnit(L"Fog", m_DefaultSet.fog.value);
		regionNode->CreateUnit(L"HDR", m_DefaultSet.hdr.value);
		regionNode->CreateUnit(L"DOF", m_DefaultSet.dof.value);

		if (m_EdgeDetectionEnable)
		{
			MkArray<float> buffer(4);
			buffer.PushBack(m_EdgeDetectionAmplifier);
			buffer.PushBack(m_EdgeDetectionEdgeColor.x); buffer.PushBack(m_EdgeDetectionEdgeColor.y); buffer.PushBack(m_EdgeDetectionEdgeColor.z);
			regionNode->CreateUnit(L"Edge", buffer);
		}

		if (m_RadialBlurEnable)
		{
			MkArray<float> buffer(4);
			buffer.PushBack(m_RadialBlurPosition.x); buffer.PushBack(m_RadialBlurPosition.y);
			buffer.PushBack(m_RadialBlurStartRange); buffer.PushBack(m_RadialBlurPower);
			regionNode->CreateUnit(L"RBlur", buffer);
		}
	}

	return node.SaveToText(filePath);
}

bool SettingRepository::LoadEffectSetting(const MkPathName& filePath)
{
	MkDataNode node;
	bool ok = node.Load(filePath);
	if (ok)
	{
		{
			MkHashStr sceneKey;
			if ((m_SceneWindowNode != NULL) &&
				node.GetDataEx(L"Scene", sceneKey, 0) &&
				m_SceneWindowNode->IsValidScene(sceneKey))
			{
				m_SceneKey = sceneKey;
			}
		}

		_CopyEffectSet(node.GetChildNode(L"#Default"), m_DefaultSet);

		MkArray<MkHashStr> keys;
		node.GetChildNodeList(keys);
		MK_INDEXING_LOOP(keys, i)
		{
			if (m_MapData.Exist(keys[i]))
			{
				_CopyEffectSet(node.GetChildNode(keys[i]), m_MapData[keys[i]].effectSet);
			}
		}

		const MkDataNode* regionNode = node.GetChildNode(L"#Default");
		if (regionNode != NULL)
		{
			{
				MkArray<float> buffer;
				m_EdgeDetectionEnable = (regionNode->GetData(L"Edge", buffer) && (buffer.GetSize() == 4));
				if (m_EdgeDetectionEnable)
				{
					m_EdgeDetectionAmplifier = buffer[0];
					m_EdgeDetectionEdgeColor = MkVec3(buffer[1], buffer[2], buffer[3]);
				}
			}
			
			{
				MkArray<float> buffer;
				m_RadialBlurEnable = (regionNode->GetData(L"RBlur", buffer) && (buffer.GetSize() == 4));
				if (m_RadialBlurEnable)
				{
					m_RadialBlurPosition = MkVec2(buffer[0], buffer[1]);
					m_RadialBlurStartRange = buffer[2];
					m_RadialBlurPower = buffer[3];
				}
			}
		}
	}
	return ok;
}

bool SettingRepository::GetColorEditEnable(void) const { return _GetTargetEffectSet().color.enable; }
float SettingRepository::GetColorEditBrightness(void) const { return RDEF_GET_VALUE(_GetTargetEffectSet().color.value, 0, 1.f); }
float SettingRepository::GetColorEditSaturation(void) const { return RDEF_GET_VALUE(_GetTargetEffectSet().color.value, 1, 1.f); }
float SettingRepository::GetColorEditContrast(void) const { return RDEF_GET_VALUE(_GetTargetEffectSet().color.value, 2, 1.f); }

void SettingRepository::SetColorEditEnable(bool enable) { _GetTargetEffectSet().color.enable = enable; }
void SettingRepository::SetColorEditBrightness(float value) { RDEF_SET_VALUE(_GetTargetEffectSet().color.value, 0, value); }
void SettingRepository::SetColorEditSaturation(float value) { RDEF_SET_VALUE(_GetTargetEffectSet().color.value, 1, value); }
void SettingRepository::SetColorEditContrast(float value) { RDEF_SET_VALUE(_GetTargetEffectSet().color.value, 2, value); }

void SettingRepository::ResetColorEditValue(void) { ResetColorEditValue(_GetTargetEffectSet().color.value); }
void SettingRepository::ResetColorEditValue(MkArray<float>& values)
{
	values.Clear();
	values.Reserve(3);
	values.PushBack(1.f); // brightness
	values.PushBack(1.f); // saturation
	values.PushBack(1.f); // contrast
}

bool SettingRepository::GetDepthFogEnable(void) const { return _GetTargetEffectSet().fog.enable; }
float SettingRepository::GetDepthFogGraphBegin(void) const { return RDEF_GET_VALUE(_GetTargetEffectSet().fog.value, 0, 0.25f); }
float SettingRepository::GetDepthFogGraphEnd(void) const { return RDEF_GET_VALUE(_GetTargetEffectSet().fog.value, 1, 0.75f); }
float SettingRepository::GetDepthFogMixerB(void) const { return RDEF_GET_VALUE(_GetTargetEffectSet().fog.value, 2, 0.f); }
float SettingRepository::GetDepthFogMixerM(void) const { return RDEF_GET_VALUE(_GetTargetEffectSet().fog.value, 3, 0.f); }
float SettingRepository::GetDepthFogMixerA(void) const { return RDEF_GET_VALUE(_GetTargetEffectSet().fog.value, 4, 0.f); }
float SettingRepository::GetDepthFogColorR(void) const { return RDEF_GET_VALUE(_GetTargetEffectSet().fog.value, 5, 1.f); }
float SettingRepository::GetDepthFogColorG(void) const { return RDEF_GET_VALUE(_GetTargetEffectSet().fog.value, 6, 1.f); }
float SettingRepository::GetDepthFogColorB(void) const { return RDEF_GET_VALUE(_GetTargetEffectSet().fog.value, 7, 1.f); }
float SettingRepository::GetDepthFogColorA(void) const { return RDEF_GET_VALUE(_GetTargetEffectSet().fog.value, 8, 0.3f); }

void SettingRepository::SetDepthFogEnable(bool enable) { _GetTargetEffectSet().fog.enable = enable; }
void SettingRepository::SetDepthFogGraphBegin(float value) { RDEF_SET_VALUE(_GetTargetEffectSet().fog.value, 0, value); }
void SettingRepository::SetDepthFogGraphEnd(float value) { RDEF_SET_VALUE(_GetTargetEffectSet().fog.value, 1, value); }
void SettingRepository::SetDepthFogMixerB(float value) { RDEF_SET_VALUE(_GetTargetEffectSet().fog.value, 2, value); }
void SettingRepository::SetDepthFogMixerM(float value) { RDEF_SET_VALUE(_GetTargetEffectSet().fog.value, 3, value); }
void SettingRepository::SetDepthFogMixerA(float value) { RDEF_SET_VALUE(_GetTargetEffectSet().fog.value, 4, value); }
void SettingRepository::SetDepthFogColorR(float value) { RDEF_SET_VALUE(_GetTargetEffectSet().fog.value, 5, value); }
void SettingRepository::SetDepthFogColorG(float value) { RDEF_SET_VALUE(_GetTargetEffectSet().fog.value, 6, value); }
void SettingRepository::SetDepthFogColorB(float value) { RDEF_SET_VALUE(_GetTargetEffectSet().fog.value, 7, value); }
void SettingRepository::SetDepthFogColorA(float value) { RDEF_SET_VALUE(_GetTargetEffectSet().fog.value, 8, value); }

void SettingRepository::ResetDepthFogValue(void) { ResetDepthFogValue(_GetTargetEffectSet().fog.value); }
void SettingRepository::ResetDepthFogValue(MkArray<float>& values)
{
	values.Clear();
	values.Reserve(9);
	values.PushBack(0.25f); // graph begin
	values.PushBack(0.75f); // graph end
	values.PushBack(1.f); // mixer blend
	values.PushBack(0.f); // mixer mult
	values.PushBack(0.f); // mixer add
	values.PushBack(1.f); // color r
	values.PushBack(1.f); // color g
	values.PushBack(1.f); // color b
	values.PushBack(0.3f); // color a
}

void SettingRepository::ResetEdgeDetectionValue(void)
{
	m_EdgeDetectionAmplifier = 8.f;
	m_EdgeDetectionEdgeColor.Clear(); // black
}

bool SettingRepository::GetFakeHDREnable(void) const { return _GetTargetEffectSet().hdr.enable; }
float SettingRepository::GetFakeHDRPowerHigh(void) const { return RDEF_GET_VALUE(_GetTargetEffectSet().hdr.value, 0, 0.8f); }
float SettingRepository::GetFakeHDRPowerLow(void) const { return RDEF_GET_VALUE(_GetTargetEffectSet().hdr.value, 1, 0.3f); }

void SettingRepository::SetFakeHDREnable(bool enable) { _GetTargetEffectSet().hdr.enable = enable; }
void SettingRepository::SetFakeHDRPowerHigh(float value) { RDEF_SET_VALUE(_GetTargetEffectSet().hdr.value, 0, value); }
void SettingRepository::SetFakeHDRPowerLow(float value) { RDEF_SET_VALUE(_GetTargetEffectSet().hdr.value, 1, value); }

void SettingRepository::ResetFakeHDRValue(void) { ResetFakeHDRValue(_GetTargetEffectSet().hdr.value); }
void SettingRepository::ResetFakeHDRValue(MkArray<float>& values)
{
	values.Clear();
	values.Reserve(2);
	values.PushBack(0.8f); // power high
	values.PushBack(0.3f); // power low
}

bool SettingRepository::GetDepthOfFieldEnable(void) const { return _GetTargetEffectSet().dof.enable; }
float SettingRepository::GetDepthOfFieldPosition(void) const { return RDEF_GET_VALUE(_GetTargetEffectSet().dof.value, 0, 0.0625f); }
float SettingRepository::GetDepthOfFieldRange(void) const { return RDEF_GET_VALUE(_GetTargetEffectSet().dof.value, 1, 20.f); }

void SettingRepository::SetDepthOfFieldEnable(bool enable) { _GetTargetEffectSet().dof.enable = enable; }
void SettingRepository::SetDepthOfFieldPosition(float value) { RDEF_SET_VALUE(_GetTargetEffectSet().dof.value, 0, value); }
void SettingRepository::SetDepthOfFieldRange(float value) { RDEF_SET_VALUE(_GetTargetEffectSet().dof.value, 1, value); }

void SettingRepository::ResetDepthOfFieldValue(void) { ResetDepthOfFieldValue(_GetTargetEffectSet().dof.value); }
void SettingRepository::ResetDepthOfFieldValue(MkArray<float>& values)
{
	values.Clear();
	values.Reserve(2);
	values.PushBack(0.0625f); // position
	values.PushBack(20.f); // range
}

void SettingRepository::ResetRadialBlurValue(void)
{
	m_RadialBlurPosition = MkVec2(0.5f, 0.5f);
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

	m_EdgeDetectionDebugBlend = 1.f;
	m_DepthOfFieldDebugBlend = 0.f;

	m_DefaultSet.color.enable = true;
	ResetColorEditValue(m_DefaultSet.color.value);

	m_DefaultSet.fog.enable = true;
	ResetDepthFogValue(m_DefaultSet.fog.value);

	m_DefaultSet.hdr.enable = true;
	ResetFakeHDRValue(m_DefaultSet.hdr.value);

	m_DefaultSet.dof.enable = true;
	ResetDepthOfFieldValue(m_DefaultSet.dof.value);

	m_TargetRegion = L"#Default";
}

//------------------------------------------------------------------------------------------------//

void SettingRepository::_CopyEffectUnit(const MkDataNode& node, const MkHashStr& key, unsigned int validSize, const _EffectUnit& defaultUnit, _EffectUnit& effUnit)
{
	effUnit.enable = (node.GetData(key, effUnit.value) && (effUnit.value.GetSize() == validSize));
	if (!effUnit.enable)
	{
		effUnit.value = defaultUnit.value;
	}
}

void SettingRepository::_CopyEffectSet(const MkDataNode* node, _EffectSet& effSet)
{
	if (node != NULL)
	{
		_CopyEffectUnit(*node, L"Color", 3, m_DefaultSet.color, effSet.color);
		_CopyEffectUnit(*node, L"Fog", 9, m_DefaultSet.fog, effSet.fog);
		_CopyEffectUnit(*node, L"HDR", 2, m_DefaultSet.hdr, effSet.hdr);
		_CopyEffectUnit(*node, L"DOF", 2, m_DefaultSet.dof, effSet.dof);
	}
}

const SettingRepository::_EffectSet& SettingRepository::_GetTargetEffectSet(void) const
{
	return m_MapData.Exist(m_TargetRegion) ? m_MapData[m_TargetRegion].effectSet : m_DefaultSet;
}

SettingRepository::_EffectSet& SettingRepository::_GetTargetEffectSet(void)
{
	return m_MapData.Exist(m_TargetRegion) ? m_MapData[m_TargetRegion].effectSet : m_DefaultSet;
}

//------------------------------------------------------------------------------------------------//
