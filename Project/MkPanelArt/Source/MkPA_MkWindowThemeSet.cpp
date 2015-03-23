
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
	MK_CHECK(node.GetChildNodeList(themeKeyList) > 0, node.GetNodeName().GetString() + L" 노드에 등록된 theme가 하나도 없음")
		return false;

	MK_CHECK(themeKeyList.FindFirstInclusion(MkArraySection(0), MkWindowThemeData::DefaultThemeName) != MKDEF_ARRAY_ERROR, node.GetNodeName().GetString() + L" 노드에 " + MkWindowThemeData::DefaultThemeName.GetString() + L" theme가 없음")
		return false;

	MK_INDEXING_LOOP(themeKeyList, i)
	{
		const MkHashStr& currThemeName = themeKeyList[i];
		MkWindowThemeData& td = m_Themes.Create(currThemeName);
		if (!td.SetUp(*node.GetChildNode(currThemeName)))
			return false;
	}

	MK_DEV_PANEL.MsgToLog(L"", false);
	return true;
}

const MkWindowThemeFormData* MkWindowThemeSet::GetFormData
(const MkHashStr& themeName, MkWindowThemeData::eComponentType compType, const MkHashStr& formName)
{
	if (m_Themes.Empty() || (compType == MkWindowThemeData::eCT_None))
		return NULL;

	const MkWindowThemeFormData* dataPtr = _GetFormData(themeName, compType, formName);
	if (dataPtr == NULL)
	{
		dataPtr = _GetFormData(MkWindowThemeData::DefaultThemeName, compType, formName);
	}
	return dataPtr;
}

float MkWindowThemeSet::GetFrameSize(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType) const
{
	if (m_Themes.Empty())
		return 0.f;

	float size = 0.f;
	if (m_Themes.Exist(themeName))
	{
		size = m_Themes[themeName].GetFrameSize(frameType);
	}
	return (size == 0.f) ? m_Themes[MkWindowThemeData::DefaultThemeName].GetFrameSize(frameType) : size;
}

const MkArray<MkHashStr>& MkWindowThemeSet::GetCaptionTextNode(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType) const
{
	if (m_Themes.Empty())
		return MkHashStr::EMPTY_ARRAY;

	if (m_Themes.Exist(themeName))
	{
		const MkArray<MkHashStr>& name = m_Themes[themeName].GetCaptionTextNode(frameType);
		return name.Empty() ? m_Themes[MkWindowThemeData::DefaultThemeName].GetCaptionTextNode(frameType) : name;
	}
	return m_Themes[MkWindowThemeData::DefaultThemeName].GetCaptionTextNode(frameType);
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
			if (refCounter == 1) // bitmap pool에 물려 있는 곳 말고는 참조된 곳이 없다는 의미
			{
				MK_BITMAP_POOL.UnloadBitmapTexture(imageFilePath);
				m_UsedThemes.Erase(MkArraySection(i, 1));

				MK_DEV_PANEL.MsgToLog(L"theme 자원 해제 : " + targetTheme.GetString(), false);
			}
		}
	}
}

void MkWindowThemeSet::Clear(void)
{
	UnloadUnusedThemeImage();

	m_Themes.Clear();
	m_UsedThemes.Clear();
}

const MkWindowThemeFormData* MkWindowThemeSet::_GetFormData
(const MkHashStr& themeName, MkWindowThemeData::eComponentType compType, const MkHashStr& formName)
{
	const MkWindowThemeFormData* dataPtr = NULL;
	if (m_Themes.Exist(themeName))
	{
		dataPtr = m_Themes[themeName].GetFormData(compType, formName);
		if ((dataPtr != NULL) && (!m_UsedThemes.Exist(MkArraySection(0), themeName)))
		{
			m_UsedThemes.PushBack(themeName);
		}
	}
	return dataPtr;
}

//------------------------------------------------------------------------------------------------//
