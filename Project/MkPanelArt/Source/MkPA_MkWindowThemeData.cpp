
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"

#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkWindowThemeData.h"


const MkHashStr MkWindowThemeData::DefaultThemeName(L"Default");

const MkHashStr MkWindowThemeData::ComponentTypeName[MkWindowThemeData::eCT_Max] =
{
	L"Default", // eCT_Default
	L"Shadow" // eCT_Shadow
};

const MkWindowThemeFormData::eFormType MkWindowThemeData::ComponentFormType[MkWindowThemeData::eCT_Max] =
{
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_Default
	MkWindowThemeFormData::eFT_SingleUnit // eCT_Shadow
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

//------------------------------------------------------------------------------------------------//
