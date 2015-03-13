
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"

#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkWindowThemeData.h"


const MkHashStr MkWindowThemeData::DefaultThemeName(L"Default");


const MkHashStr MkWindowThemeData::ComponentTypeName[MkWindowThemeData::eCT_Max] =
{
	MK_VALUE_TO_STRING(eCT_ShadowBox),
	MK_VALUE_TO_STRING(eCT_DefaultBox),
	MK_VALUE_TO_STRING(eCT_NoticeBox),
	MK_VALUE_TO_STRING(eCT_StaticBox),
	MK_VALUE_TO_STRING(eCT_GuideBox),
	MK_VALUE_TO_STRING(eCT_Title),
	MK_VALUE_TO_STRING(eCT_NormalBtn),
	MK_VALUE_TO_STRING(eCT_OKBtn),
	MK_VALUE_TO_STRING(eCT_CancelBtn),
	MK_VALUE_TO_STRING(eCT_FlatBtn),
	MK_VALUE_TO_STRING(eCT_GuideBtn),
	MK_VALUE_TO_STRING(eCT_GuideLine),
	MK_VALUE_TO_STRING(eCT_YellowZone),
	MK_VALUE_TO_STRING(eCT_BlueZone),
	MK_VALUE_TO_STRING(eCT_RedOutlineZone),
	MK_VALUE_TO_STRING(eCT_YellowSelBtn),
	MK_VALUE_TO_STRING(eCT_BlueSelBtn),
	MK_VALUE_TO_STRING(eCT_RedOutlineSelBtn),
	MK_VALUE_TO_STRING(eCT_RedLEDBtn16),
	MK_VALUE_TO_STRING(eCT_GreenLEDBtn16),
	MK_VALUE_TO_STRING(eCT_BlueLEDBtn16),
	MK_VALUE_TO_STRING(eCT_RedLEDBtn22),
	MK_VALUE_TO_STRING(eCT_GreenLEDBtn22),
	MK_VALUE_TO_STRING(eCT_BlueLEDBtn22)
};

const MkWindowThemeFormData::eFormType MkWindowThemeData::ComponentFormType[MkWindowThemeData::eCT_Max] =
{
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_ShadowBox
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_DefaultBox
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_NoticeBox
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_StaticBox
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_GuideBox
	MkWindowThemeFormData::eFT_DualUnit, // eCT_Title
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_NormalBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_OKBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_CancelBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_FlatBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_GuideBtn
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_GuideLine
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_YellowZone
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_BlueZone
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_RedOutlineZone
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_YellowSelBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_BlueSelBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_RedOutlineSelBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_RedLEDBtn16
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_GreenLEDBtn16
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_BlueLEDBtn16
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_RedLEDBtn22
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_GreenLEDBtn22
	MkWindowThemeFormData::eFT_QuadUnit // eCT_BlueLEDBtn22
};

//------------------------------------------------------------------------------------------------//

bool MkWindowThemeData::SetUp(const MkDataNode& node)
{
	MkStr imagePath;
	MK_CHECK(node.GetData(L"ImagePath", imagePath, 0) && (!imagePath.Empty()), node.GetNodeName().GetString() + L" theme node에 ImagePath 경로가 없음")
		return false;

	m_ImageFilePath = imagePath;
	MkBaseTexture* texture = MK_BITMAP_POOL.GetBitmapTexture(m_ImageFilePath);
	MK_CHECK(texture != NULL, L"잘못된 이미지 경로 : " + imagePath)
		return false;

	bool notDefaultTheme = (node.GetNodeName() != DefaultThemeName);

	for (unsigned int i=0; i<eCT_Max; ++i)
	{
		const MkHashStr& componentName = ComponentTypeName[i];
		bool hasComponent = node.ChildExist(componentName);

		// default theme는 모든 component를 다 가지고 있어야 함
		MK_CHECK(notDefaultTheme || hasComponent, node.GetNodeName().GetString() + L" theme node에 " + componentName.GetString() + L" component가 없음")
			return false;

		if (hasComponent)
		{
			eComponentType compType = static_cast<eComponentType>(i);
			MkWindowThemeFormData& fd = m_Components.Create(compType);
			MK_CHECK(fd.SetUp(&m_ImageFilePath, texture, *node.GetChildNode(componentName)), node.GetNodeName().GetString() + L" theme node의 " + componentName.GetString() + L" component 구성 실패")
				return false;

			// setup이 완료되었으면 form type 점검
			MK_CHECK(fd.GetFormType() == ComponentFormType[i], node.GetNodeName().GetString() + L" theme node의 " + componentName.GetString() + L" component form type이 잘못되었음")
				return false;
		}
	}

	// 설정이 끝났으니 사용 될 때까지 texture 해제
	MK_BITMAP_POOL.UnloadBitmapTexture(m_ImageFilePath);

	MkStr compMsg = (m_Components.GetSize() == eCT_Max) ? L"FULL" : MkStr(m_Components.GetSize());
	MK_DEV_PANEL.MsgToLog(L"   - theme name : " + node.GetNodeName().GetString() + L" (" + compMsg + L" component)", false);
	return true;
}

void MkWindowThemeData::Clear(void)
{
	m_ImageFilePath.Clear();
	m_Components.Clear();
}

MkWindowThemeFormData::eFormType MkWindowThemeData::GetFormTypeOfComponent(eComponentType compType)
{
	return (compType == eCT_None) ? MkWindowThemeFormData::eFT_None : MkWindowThemeData::ComponentFormType[compType];
}

//------------------------------------------------------------------------------------------------//
