#pragma once


//------------------------------------------------------------------------------------------------//
// theme 정의 data
// MkWindowThemeFormData의 집합
// component는 각 항목마다 정해진 predefined name과 form type이 존재
// window system의 visual 담당
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkPA_MkWindowThemeFormData.h"


class MkWindowThemeData
{
public:

	// component type 정의
	// (NOTE) 변동시 ComponentTypeName[], ComponentFormType[]에도 반영해 줄 것
	enum eComponentType
	{
		eCT_None = -1,

		//------------------------------------------------------------------------------------------------//
		
		eCT_ShadowBox = 0, // eFT_SingleUnit
		eCT_DefaultBox, // eFT_SingleUnit
		eCT_NoticeBox, // eFT_SingleUnit
		eCT_StaticBox, // eFT_SingleUnit
		eCT_GuideBox, // eFT_SingleUnit

		eCT_Title, // eFT_DualUnit

		eCT_NormalBtn, // eFT_QuadUnit
		eCT_OKBtn, // eFT_QuadUnit
		eCT_CancelBtn, // eFT_QuadUnit

		eCT_YellowZone, // eFT_SingleUnit
		eCT_BlueZone, // eFT_SingleUnit
		eCT_RedOutlineZone, // eFT_SingleUnit

		eCT_YellowSelBtn, // eFT_QuadUnit
		eCT_BlueSelBtn, // eFT_QuadUnit
		eCT_RedOutlineSelBtn, // eFT_QuadUnit

		//------------------------------------------------------------------------------------------------//

		eCT_Max
	};

public:

	// 초기화
	bool SetUp(const MkDataNode& node);

	// 해제
	void Clear(void);

	// theme image file pathname 반환
	inline const MkHashStr& GetImageFilePath(void) const { return m_ImageFilePath; }

	// component form data 반환
	inline const MkWindowThemeFormData* GetFormData(eComponentType compType) const { return m_Components.Exist(compType) ? &m_Components[compType] : NULL; }

	~MkWindowThemeData() { Clear(); }

protected:

	MkHashStr m_ImageFilePath;

	MkMap<eComponentType, MkWindowThemeFormData> m_Components;

public:

	static const MkHashStr DefaultThemeName;

	// eComponentType
	static const MkHashStr ComponentTypeName[eCT_Max];
	static const MkWindowThemeFormData::eFormType ComponentFormType[eCT_Max];
};
