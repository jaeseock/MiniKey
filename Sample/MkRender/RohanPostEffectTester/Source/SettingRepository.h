#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkVec2.h"
#include "MkCore_MkVec3.h"

#define SREPO SettingRepository::GetInstance()


class SceneSelectionWindowNode;

class SettingRepository
{
public:

	void SetUp(const SceneSelectionWindowNode* sceneWindowNode);

	bool SaveEffectSetting(const MkPathName& filePath) const;
	bool LoadEffectSetting(const MkPathName& filePath);

	// region
	inline const MkHashStr& GetTargetRegion(void) const { return m_TargetRegion; }
	inline void SetTargetRegion(const MkHashStr& key) { m_TargetRegion = key; }

	// scene key
	inline const MkHashStr& GetSceneKey(void) const { return m_SceneKey; }
	inline void SetSceneKey(const MkHashStr& key) { m_SceneKey = key; }

	// color edit
	bool GetColorEditEnable(void) const;
	float GetColorEditBrightness(void) const;
	float GetColorEditSaturation(void) const;
	float GetColorEditContrast(void) const;
	
	void SetColorEditEnable(bool enable);
	void SetColorEditBrightness(float value);
	void SetColorEditSaturation(float value);
	void SetColorEditContrast(float value);
	
	void ResetColorEditValue(void);
	void ResetColorEditValue(MkArray<float>& values);

	// depth fog
	bool GetDepthFogEnable(void) const;
	float GetDepthFogGraphBegin(void) const;
	float GetDepthFogGraphEnd(void) const;
	float GetDepthFogMixerB(void) const;
	float GetDepthFogMixerM(void) const;
	float GetDepthFogMixerA(void) const;
	float GetDepthFogColorR(void) const;
	float GetDepthFogColorG(void) const;
	float GetDepthFogColorB(void) const;
	float GetDepthFogColorA(void) const;

	void SetDepthFogEnable(bool enable);
	void SetDepthFogGraphBegin(float value);
	void SetDepthFogGraphEnd(float value);
	void SetDepthFogMixerB(float value);
	void SetDepthFogMixerM(float value);
	void SetDepthFogMixerA(float value);
	void SetDepthFogColorR(float value);
	void SetDepthFogColorG(float value);
	void SetDepthFogColorB(float value);
	void SetDepthFogColorA(float value);

	void ResetDepthFogValue(void);
	void ResetDepthFogValue(MkArray<float>& values);

	// edge detection
	inline bool GetEdgeDetectionEnable(void) const { return m_EdgeDetectionEnable; }
	inline float GetEdgeDetectionAmplifier(void) const { return m_EdgeDetectionAmplifier; }
	inline float GetEdgeDetectionDebugBlend(void) const { return m_EdgeDetectionDebugBlend; }
	inline const MkVec3& GetEdgeDetectionEdgeColor(void) const { return m_EdgeDetectionEdgeColor; }

	inline void SetEdgeDetectionEnable(bool enable) { m_EdgeDetectionEnable = enable; }
	inline void SetEdgeDetectionAmplifier(float value) { m_EdgeDetectionAmplifier = value; }
	inline void SetEdgeDetectionDebugBlend(float value) { m_EdgeDetectionDebugBlend = value; }
	inline void SetEdgeDetectionEdgeColor(const MkVec3& color) { m_EdgeDetectionEdgeColor = color; }

	void ResetEdgeDetectionValue(void);

	// hdr
	bool GetFakeHDREnable(void) const;
	float GetFakeHDRPowerHigh(void) const;
	float GetFakeHDRPowerLow(void) const;

	void SetFakeHDREnable(bool enable);
	void SetFakeHDRPowerHigh(float value);
	void SetFakeHDRPowerLow(float value);

	void ResetFakeHDRValue(void);
	void ResetFakeHDRValue(MkArray<float>& values);

	// depth of field
	bool GetDepthOfFieldEnable(void) const;
	float GetDepthOfFieldPosition(void) const;
	float GetDepthOfFieldRange(void) const;
	inline float GetDepthOfFieldDebugBlend(void) const { return m_DepthOfFieldDebugBlend; }

	void SetDepthOfFieldEnable(bool enable);
	void SetDepthOfFieldPosition(float value);
	void SetDepthOfFieldRange(float value);
	inline void SetDepthOfFieldDebugBlend(float value) { m_DepthOfFieldDebugBlend = value; }

	void ResetDepthOfFieldValue(void);
	void ResetDepthOfFieldValue(MkArray<float>& values);

	// radial blur
	inline bool GetRadialBlurEnable(void) const { return m_RadialBlurEnable; }
	inline const MkVec2& GetRadialBlurPosition(void) const { return m_RadialBlurPosition; }
	inline float GetRadialBlurStartRange(void) const { return m_RadialBlurStartRange; }
	inline float GetRadialBlurPower(void) const { return m_RadialBlurPower; }

	inline void SetRadialBlurEnable(bool enable) { m_RadialBlurEnable = enable; }
	inline void SetRadialBlurPosition(const MkVec2& value) { m_RadialBlurPosition = value; }
	inline void SetRadialBlurStartRange(float value) { m_RadialBlurStartRange = value; }
	inline void SetRadialBlurPower(float value) { m_RadialBlurPower = value; }

	void ResetRadialBlurValue(void);

	static SettingRepository& GetInstance(void);

	SettingRepository();
	~SettingRepository() {}

protected:

	typedef struct __EffectUnit
	{
		bool enable;
		MkArray<float> value;
	}
	_EffectUnit;

	typedef struct __EffectSet
	{
		_EffectUnit color;
		_EffectUnit fog;
		_EffectUnit hdr;
		_EffectUnit dof;
	}
	_EffectSet;

	typedef struct __EffectRegion
	{
		_EffectSet effectSet;
		MkInt2 xy;
		int room;
	}
	_EffectRegion;

protected:

	void _CopyEffectUnit(const MkDataNode& node, const MkHashStr& key, unsigned int validSize, const _EffectUnit& defaultUnit, _EffectUnit& effUnit);
	void _CopyEffectSet(const MkDataNode* node, _EffectSet& effSet);

	const _EffectSet& _GetTargetEffectSet(void) const;
	_EffectSet& _GetTargetEffectSet(void);
	
protected:

	const SceneSelectionWindowNode* m_SceneWindowNode;

	_EffectSet m_DefaultSet;
	MkHashMap<MkHashStr, _EffectRegion> m_MapData;

	MkHashStr m_TargetRegion;

	MkHashStr m_SceneKey;

	bool m_EdgeDetectionEnable;
	float m_EdgeDetectionAmplifier;
	float m_EdgeDetectionDebugBlend;
	MkVec3 m_EdgeDetectionEdgeColor;

	bool m_DepthOfFieldEnable;
	MkArray<float> m_DepthOfFieldValue;
	float m_DepthOfFieldDebugBlend;

	bool m_RadialBlurEnable;
	MkVec2 m_RadialBlurPosition;
	float m_RadialBlurStartRange;
	float m_RadialBlurPower;
};
