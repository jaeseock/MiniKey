
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"

#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkWindowThemeSet.h"


//------------------------------------------------------------------------------------------------//

bool MkWindowThemeSet::SetUp(const MkDataNode& node)
{
	MK_DEV_PANEL.MsgToLog(L"< Window theme >", false);

	MkArray<MkHashStr> themeKeyList;
	MK_CHECK(node.GetChildNodeList(themeKeyList) > 0, node.GetNodeName().GetString() + L" ��忡 ��ϵ� theme�� �ϳ��� ����")
		return false;

	MK_CHECK(themeKeyList.FindFirstInclusion(MkArraySection(0), MkWindowThemeData::DefaultThemeName) != MKDEF_ARRAY_ERROR, node.GetNodeName().GetString() + L" ��忡 " + MkWindowThemeData::DefaultThemeName.GetString() + L" theme�� ����")
		return false;

	MK_INDEXING_LOOP(themeKeyList, i)
	{
		const MkHashStr& currThemeName = themeKeyList[i];
		MkWindowThemeData& td = m_Themes.Create(currThemeName);
		if (!td.SetUp(*node.GetChildNode(currThemeName)))
			return false;
	}

	SetCurrentTheme(MkWindowThemeData::DefaultThemeName);

	MK_DEV_PANEL.MsgToLog(L"", false);
	return true;
}

bool MkWindowThemeSet::SetCurrentTheme(const MkHashStr& themeName)
{
	if (m_Themes.Empty()) // �ʱ�ȭ���� �ʾ���
		return false;

	if (themeName == m_CurrentTheme)
		return true;

	bool ok = m_Themes.Exist(themeName);
	if (ok)
	{
		// ���� theme�� ���� ���� ��� ��Ͽ� �߰�
		if (m_Themes.Exist(m_CurrentTheme))
		{
			m_UsedThemes.PushBack(m_CurrentTheme);
		}

		m_CurrentTheme = themeName;

		MK_DEV_PANEL.MsgToLog(L"theme ��� ���� : " + m_CurrentTheme.GetString(), false);
	}
	return ok;
}

const MkHashStr& MkWindowThemeSet::GetImageFilePath(void) const
{
	MK_CHECK(!m_Themes.Empty(), L"�ʱ�ȭ���� ���� theme ���� ȣ��")
		return MkHashStr::EMPTY;

	const MkHashStr& targetThemeName = m_Themes.Exist(m_CurrentTheme) ? m_CurrentTheme : MkWindowThemeData::DefaultThemeName;
	return m_Themes[targetThemeName].GetImageFilePath();
}

const MkWindowThemeFormData* MkWindowThemeSet::GetFormData(MkWindowThemeData::eComponentType compType) const
{
	if (compType == MkWindowThemeData::eCT_None)
		return NULL;

	MK_CHECK(!m_Themes.Empty(), L"�ʱ�ȭ���� ���� theme ���� ȣ��")
		return NULL;

	const MkWindowThemeFormData* dataPtr = NULL;
	if (m_Themes.Exist(m_CurrentTheme))
	{
		dataPtr = m_Themes[m_CurrentTheme].GetFormData(compType);
	}

	return (dataPtr == NULL) ? m_Themes[MkWindowThemeData::DefaultThemeName].GetFormData(compType) : dataPtr;
}

void MkWindowThemeSet::UnloadUnusedThemeImage(void)
{
	if (!m_UsedThemes.Empty())
	{
		MkArray<MkHashStr> usedThemes = m_UsedThemes;
		MK_INDEXING_LOOP(usedThemes, i)
		{
			const MkHashStr& targetTheme = usedThemes[i];
			const MkHashStr& imageFilePath = m_Themes[targetTheme].GetImageFilePath();

			MkBaseTexture* texture = MK_BITMAP_POOL.GetBitmapTexture(imageFilePath);
			unsigned int refCounter = MK_SHARED_PTR_MGR.GetReferenceCounter(MK_PTR_TO_ID64(texture));
			if (refCounter == 1) // bitmap pool�� ���� �ִ� �� ����� ������ ���� ���ٴ� �ǹ�
			{
				MK_BITMAP_POOL.UnloadBitmapTexture(imageFilePath);
				m_UsedThemes.Erase(MkArraySection(i, 1));

				MK_DEV_PANEL.MsgToLog(L"theme �ڿ� ���� : " + targetTheme.GetString(), false);
			}
		}
	}
}

void MkWindowThemeSet::Clear(void)
{
	UnloadUnusedThemeImage();

	m_Themes.Clear();
	m_CurrentTheme.Clear();
	m_UsedThemes.Clear();
}

//------------------------------------------------------------------------------------------------//
