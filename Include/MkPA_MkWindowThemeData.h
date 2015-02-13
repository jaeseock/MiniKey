#pragma once


//------------------------------------------------------------------------------------------------//
// theme ���� data
// MkWindowThemeFormData�� ����
// component�� �� �׸񸶴� ������ predefined name�� form type�� ����
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
		
		eCT_Default = 0, // L"Default", eFT_SingleUnit
		eCT_Shadow, // L"Shadow", eFT_SingleUnit

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
