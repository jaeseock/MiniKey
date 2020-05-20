#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashStr.h"
#include "MkCore_MkVec3.h"

#define SREPO SettingRepository::GetInstance()


class SceneSelectionWindowNode;

class SettingRepository
{
public:

	void SetUp(const SceneSelectionWindowNode* sceneWindowNode);

	bool SaveEffectSetting(const MkPathName& filePath) const;

	void LoadEffectSetting(const MkDataNode& setting);
	bool LoadEffectSetting(const MkPathName& filePath);
	
	// scene key
	inline const MkHashStr& GetSceneKey(void) const { return m_SceneKey; }
	inline void SetSceneKey(const MkHashStr& key) { m_SceneKey = key; }

	// color edit
	inline bool GetColorEditEnable(void) const { return m_ColorEditEnable; }
	inline float GetColorEditBrightness(void) const { return m_ColorEditBrightness; }
	inline float GetColorEditSaturation(void) const { return m_ColorEditSaturation; }
	inline float GetColorEditContrast(void) const { return m_ColorEditContrast; }
	inline float GetColorEditColorBalanceR(void) const { return m_ColorEditColorBalanceR; }
	inline float GetColorEditColorBalanceG(void) const { return m_ColorEditColorBalanceG; }
	inline float GetColorEditColorBalanceB(void) const { return m_ColorEditColorBalanceB; }

	inline void SetColorEditEnable(bool enable) { m_ColorEditEnable = enable; }
	inline void SetColorEditBrightness(float value) { m_ColorEditBrightness = value; }
	inline void SetColorEditSaturation(float value) { m_ColorEditSaturation = value; }
	inline void SetColorEditContrast(float value) { m_ColorEditContrast = value; }
	inline void SetColorEditColorBalanceR(float r) { m_ColorEditColorBalanceR = r; }
	inline void SetColorEditColorBalanceG(float g) { m_ColorEditColorBalanceG = g; }
	inline void SetColorEditColorBalanceB(float b) { m_ColorEditColorBalanceB = b; }

	void ResetColorEditValue(void);

	// depth fog
	inline bool GetDepthFogEnable(void) const { return m_DepthFogEnable; }
	inline float GetDepthFogMixer(void) const { return m_DepthFogMixer; }
	inline const MkHashStr& GetDepthFogMultMap(void) const { return m_DepthFogMultMap; }
	inline const MkHashStr& GetDepthFogAddMap(void) const { return m_DepthFogAddMap; }

	inline void SetDepthFogEnable(bool enable) { m_DepthFogEnable = enable; }
	inline void SetDepthFogMixer(float value) { m_DepthFogMixer = value; }
	inline void SetDepthFogMultMap(const MkHashStr& mapPath) { m_DepthFogMultMap = mapPath; }
	inline void SetDepthFogAddMap(const MkHashStr& mapPath) { m_DepthFogAddMap = mapPath; }

	void ResetDepthFogValue(void);

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
	inline bool GetFakeHDREnable(void) const { return m_FakeHDREnable; }
	inline float GetFakeHDRPowerHigh(void) const { return m_FakeHDRPowerHigh; }
	inline float GetFakeHDRPowerLow(void) const { return m_FakeHDRPowerLow; }

	inline void SetFakeHDREnable(bool enable) { m_FakeHDREnable = enable; }
	inline void SetFakeHDRPowerHigh(float value) { m_FakeHDRPowerHigh = value; }
	inline void SetFakeHDRPowerLow(float value) { m_FakeHDRPowerLow = value; }

	void ResetFakeHDRValue(void);

	// depth of field
	inline bool GetDepthOfFieldEnable(void) const { return m_DepthOfFieldEnable; }
	inline float GetDepthOfFieldPosition(void) const { return m_DepthOfFieldPosition; }
	inline float GetDepthOfFieldRange(void) const { return m_DepthOfFieldRange; }
	inline float GetDepthOfFieldDebugBlend(void) const { return m_DepthOfFieldDebugBlend; }

	inline void SetDepthOfFieldEnable(bool enable) { m_DepthOfFieldEnable = enable; }
	inline void SetDepthOfFieldPosition(float value) { m_DepthOfFieldPosition = value; }
	inline void SetDepthOfFieldRange(float value) { m_DepthOfFieldRange = value; }
	inline void SetDepthOfFieldDebugBlend(float value) { m_DepthOfFieldDebugBlend = value; }

	void ResetDepthOfFieldValue(void);

	// radial blur
	inline bool GetRadialBlurEnable(void) const { return m_RadialBlurEnable; }
	inline const MkFloat2& GetRadialBlurPosition(void) const { return m_RadialBlurPosition; }
	inline float GetRadialBlurStartRange(void) const { return m_RadialBlurStartRange; }
	inline float GetRadialBlurPower(void) const { return m_RadialBlurPower; }

	inline void SetRadialBlurEnable(bool enable) { m_RadialBlurEnable = enable; }
	inline void SetRadialBlurPosition(const MkFloat2& value) { m_RadialBlurPosition = value; }
	inline void SetRadialBlurStartRange(float value) { m_RadialBlurStartRange = value; }
	inline void SetRadialBlurPower(float value) { m_RadialBlurPower = value; }

	void ResetRadialBlurValue(void);

	static SettingRepository& GetInstance(void);

	SettingRepository();
	~SettingRepository() {}
	
protected:

	const SceneSelectionWindowNode* m_SceneWindowNode;

	MkHashStr m_SceneKey;

	bool m_ColorEditEnable;
	float m_ColorEditBrightness;
	float m_ColorEditSaturation;
	float m_ColorEditContrast;
	float m_ColorEditColorBalanceR;
	float m_ColorEditColorBalanceG;
	float m_ColorEditColorBalanceB;

	bool m_DepthFogEnable;
	float m_DepthFogMixer;
	MkHashStr m_DepthFogMultMap;
	MkHashStr m_DepthFogAddMap;

	bool m_EdgeDetectionEnable;
	float m_EdgeDetectionAmplifier;
	float m_EdgeDetectionDebugBlend;
	MkVec3 m_EdgeDetectionEdgeColor;

	bool m_FakeHDREnable;
	float m_FakeHDRPowerHigh;
	float m_FakeHDRPowerLow;

	bool m_DepthOfFieldEnable;
	float m_DepthOfFieldPosition;
	float m_DepthOfFieldRange;
	float m_DepthOfFieldDebugBlend;

	bool m_RadialBlurEnable;
	MkFloat2 m_RadialBlurPosition;
	float m_RadialBlurStartRange;
	float m_RadialBlurPower;
};
