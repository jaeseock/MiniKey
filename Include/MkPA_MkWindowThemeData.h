#pragma once


//------------------------------------------------------------------------------------------------//
// theme ���� data
// MkWindowThemeFormData�� ����
// component�� �� �׸񸶴� ������ predefined name�� form type�� ����
// window system�� visual ���
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkPA_MkWindowThemeFormData.h"


class MkWindowThemeData
{
public:

	// component type ����
	// (NOTE) ������ ComponentTypeName[], ComponentFormType[]���� �ݿ��� �� ��
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

	// �ʱ�ȭ
	bool SetUp(const MkDataNode& node);

	// ����
	void Clear(void);

	// theme image file pathname ��ȯ
	inline const MkHashStr& GetImageFilePath(void) const { return m_ImageFilePath; }

	// component form data ��ȯ
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
