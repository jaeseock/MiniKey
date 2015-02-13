
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
	MK_CHECK(node.GetData(L"ImagePath", imagePath, 0) && (!imagePath.Empty()), node.GetNodeName().GetString() + L" theme node�� ImagePath ��ΰ� ����")
		return false;

	m_ImageFilePath = imagePath;
	MkBaseTexture* texture = MK_BITMAP_POOL.GetBitmapTexture(m_ImageFilePath);
	MK_CHECK(texture != NULL, L"�߸��� �̹��� ��� : " + imagePath)
		return false;

	bool notDefaultTheme = (node.GetNodeName() != DefaultThemeName);

	for (unsigned int i=0; i<eCT_Max; ++i)
	{
		const MkHashStr& componentName = ComponentTypeName[i];
		bool hasComponent = node.ChildExist(componentName);

		// default theme�� ��� component�� �� ������ �־�� ��
		MK_CHECK(notDefaultTheme || hasComponent, node.GetNodeName().GetString() + L" theme node�� " + componentName.GetString() + L" component�� ����")
			return false;

		if (hasComponent)
		{
			eComponentType compType = static_cast<eComponentType>(i);
			MkWindowThemeFormData& fd = m_Components.Create(compType);
			MK_CHECK(fd.SetUp(&m_ImageFilePath, texture, *node.GetChildNode(componentName)), node.GetNodeName().GetString() + L" theme node�� " + componentName.GetString() + L" component ���� ����")
				return false;

			// setup�� �Ϸ�Ǿ����� form type ����
			MK_CHECK(fd.GetFormType() == ComponentFormType[i], node.GetNodeName().GetString() + L" theme node�� " + componentName.GetString() + L" component form type�� �߸��Ǿ���")
				return false;
		}
	}

	// ������ �������� ��� �� ������ texture ����
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
