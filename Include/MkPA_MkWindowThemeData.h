#pragma once


//------------------------------------------------------------------------------------------------//
// theme ���� data
// MkWindowThemeFormData�� ����
// component�� �� �׸񸶴� ������ predefined name�� form type�� ����
// window system�� visual ���
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
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
		// regular component
		//------------------------------------------------------------------------------------------------//
		
		// flexible
		eCT_ShadowBox = 0, // eFT_SingleUnit
		eCT_DefaultBox, // eFT_SingleUnit
		eCT_NoticeBox, // eFT_SingleUnit
		eCT_StaticBox, // eFT_SingleUnit
		eCT_GuideBox, // eFT_SingleUnit

		eCT_Title, // eFT_DualUnit

		eCT_NormalBtn, // eFT_QuadUnit
		eCT_OKBtn, // eFT_QuadUnit
		eCT_CancelBtn, // eFT_QuadUnit

		eCT_CheckBoxBtn, // eFT_QuadUnit
		eCT_FlatBtn, // eFT_QuadUnit
		eCT_GuideBtn, // eFT_QuadUnit

		eCT_GuideLine, // eFT_SingleUnit

		eCT_YellowZone, // eFT_SingleUnit
		eCT_BlueZone, // eFT_SingleUnit
		eCT_RedOutlineZone, // eFT_SingleUnit

		eCT_YellowSelBtn, // eFT_QuadUnit
		eCT_BlueSelBtn, // eFT_QuadUnit
		eCT_RedOutlineSelBtn, // eFT_QuadUnit

		// static. follow frame size
		eCT_RedLEDBtnSmall, // eFT_QuadUnit
		eCT_RedLEDBtnMedium, // eFT_QuadUnit
		eCT_RedLEDBtnLarge, // eFT_QuadUnit

		eCT_GreenLEDBtnSmall, // eFT_QuadUnit
		eCT_GreenLEDBtnMedium, // eFT_QuadUnit
		eCT_GreenLEDBtnLarge, // eFT_QuadUnit

		eCT_BlueLEDBtnSmall, // eFT_QuadUnit
		eCT_BlueLEDBtnMedium, // eFT_QuadUnit
		eCT_BlueLEDBtnLarge, // eFT_QuadUnit

		eCT_NoticeIconSmall, // eFT_SingleUnit
		eCT_NoticeIconMedium, // eFT_SingleUnit
		eCT_NoticeIconLarge, // eFT_SingleUnit

		eCT_InformationIconSmall, // eFT_SingleUnit
		eCT_InformationIconMedium, // eFT_SingleUnit
		eCT_InformationIconLarge, // eFT_SingleUnit

		eCT_WarningIconSmall, // eFT_SingleUnit
		eCT_WarningIconMedium, // eFT_SingleUnit
		eCT_WarningIconLarge, // eFT_SingleUnit

		eCT_WindowIconSmall, // eFT_SingleUnit
		eCT_WindowIconMedium, // eFT_SingleUnit
		eCT_WindowIconLarge, // eFT_SingleUnit

		eCT_EditModeIconSmall, // eFT_SingleUnit
		eCT_EditModeIconMedium, // eFT_SingleUnit
		eCT_EditModeIconLarge, // eFT_SingleUnit

		eCT_LeftArrowIconSmall, // eFT_SingleUnit
		eCT_LeftArrowIconMedium, // eFT_SingleUnit
		eCT_LeftArrowIconLarge, // eFT_SingleUnit

		eCT_RightArrowIconSmall, // eFT_SingleUnit
		eCT_RightArrowIconMedium, // eFT_SingleUnit
		eCT_RightArrowIconLarge, // eFT_SingleUnit

		eCT_UpArrowIconSmall, // eFT_SingleUnit
		eCT_UpArrowIconMedium, // eFT_SingleUnit
		eCT_UpArrowIconLarge, // eFT_SingleUnit

		eCT_DownArrowIconSmall, // eFT_SingleUnit
		eCT_DownArrowIconMedium, // eFT_SingleUnit
		eCT_DownArrowIconLarge, // eFT_SingleUnit

		eCT_CheckMarkIconSmall, // eFT_SingleUnit
		eCT_CheckMarkIconMedium, // eFT_SingleUnit
		eCT_CheckMarkIconLarge, // eFT_SingleUnit

		eCT_RegularMax,

		//------------------------------------------------------------------------------------------------//
		// custom component
		//------------------------------------------------------------------------------------------------//

		eCT_CustomForm = eCT_RegularMax
	};

	// frame type ����. size ����
	enum eFrameType
	{
		eFT_None = -1,

		eFT_Small = 0,
		eFT_Medium,
		eFT_Large,

		eFT_Max
	};

	// LED type ����
	enum eLEDType
	{
		eLED_Red = 0,
		eLED_Green,
		eLED_Blue
	};

	// icon type ����
	// eIT_Default ~ eIT_CheckMark�� system icon, eIT_CustomTag ���� tag ����
	enum eIconType
	{
		eIT_None = -1,

		eIT_Default = 0,
		eIT_Notice,
		eIT_Information,
		eIT_Warning,
		eIT_EditMode,

		eIT_LeftArrow,
		eIT_RightArrow,
		eIT_UpArrow,
		eIT_DownArrow,

		eIT_CheckMark,

		eIT_CustomTag
	};

public:

	// �ʱ�ȭ
	bool SetUp(const MkDataNode& node);

	// ����
	void Clear(void);

	// setting ��ȯ
	inline const MkHashStr& GetImageFilePath(void) const { return m_ImageFilePath; }
	float GetFrameSize(eFrameType frameType) const;
	const MkArray<MkHashStr>& GetCaptionTextNode(eFrameType frameType) const;
	const MkArray<MkHashStr>& GetEditTextNode(eFrameType frameType) const;

	// component form data ��ȯ
	const MkWindowThemeFormData* GetFormData(eComponentType compType, const MkHashStr& formName) const;

	// frame type�� �´� LED button ��ȯ
	static eComponentType GetLEDButtonComponent(eFrameType frameType, eLEDType LEDType);

	// frame type�� �´� system icon ��ȯ
	static eComponentType GetSystemIconComponent(eFrameType frameType, eIconType iconType);

	MkWindowThemeData();
	~MkWindowThemeData() { Clear(); }

protected:

	// setting
	MkHashStr m_ImageFilePath;
	float m_FrameSize[eFT_Max];
	MkArray<MkHashStr> m_CaptionTextNode[eFT_Max];
	MkArray<MkHashStr> m_EditTextNode[eFT_Max];

	// forms
	MkMap<eComponentType, MkWindowThemeFormData> m_RegularComponents;
	MkHashMap<MkHashStr, MkWindowThemeFormData>* m_CustomForms;

public:

	static const MkHashStr DefaultThemeName;

	// eComponentType
	static const MkHashStr ComponentTypeName[eCT_RegularMax];
	static const MkWindowThemeFormData::eFormType ComponentFormType[eCT_RegularMax];

	// eCT_CustomForm
	static const MkHashStr CustomFormName;

	// eFrameType
	static const MkHashStr FrameTypeName[eFT_Max];
};
