
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"

#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkWindowResourceManager.h"


#define MKDEF_THEME_FONT_TYPE_AND_STATE_KEY L"FontTypeAndState"

//------------------------------------------------------------------------------------------------//

bool MkWindowPreset::SetUp(const MkDataNode& node)
{
	MkStr defaultThemeName;
	MK_CHECK(node.GetData(L"DefaultThemeName", defaultThemeName, 0) && (!defaultThemeName.Empty()), L"window preset 노드에 DefaultThemeName이 지정되어 있지 않음")
		return false;
	m_DefaultThemeName = defaultThemeName;

	int margin;
	MK_CHECK(node.GetData(L"Margin", margin, 0) && (margin > 0), L"window preset 노드에 Margin이 정상적으로 지정되어 있지 않음")
		return false;
	m_Margin = static_cast<float>(margin);

	MkStr systemImageFilePath;
	MK_CHECK(node.GetData(L"SystemImageFilePath", systemImageFilePath, 0) && (!systemImageFilePath.Empty()), L"window preset 노드에 SystemImageFilePath이 지정되어 있지 않음")
		return false;
	m_SystemImageFilePath = systemImageFilePath;

	MkStr darkenLayerSubsetName;
	MK_CHECK(node.GetData(L"DarkenLayerSubsetName", darkenLayerSubsetName, 0) && (!darkenLayerSubsetName.Empty()), L"window preset 노드에 DarkenLayerSubsetName이 지정되어 있지 않음")
		return false;
	m_DarkenLayerSubsetName = darkenLayerSubsetName;

	MkStr windowIconSampleSubsetName;
	MK_CHECK(node.GetData(L"WindowIconSampleSubsetName", windowIconSampleSubsetName, 0) && (!windowIconSampleSubsetName.Empty()), L"window preset 노드에 WindowIconSampleSubsetName이 지정되어 있지 않음")
		return false;
	m_WindowIconSampleSubsetName = windowIconSampleSubsetName;

	MkStr windowBackgroundSampleSubsetName;
	MK_CHECK(node.GetData(L"WindowBackgroundSampleSubsetName", windowBackgroundSampleSubsetName, 0) && (!windowBackgroundSampleSubsetName.Empty()), L"window preset 노드에 WindowBackgroundSampleSubsetName이 지정되어 있지 않음")
		return false;
	m_WindowBackgroundSampleSubsetName = windowBackgroundSampleSubsetName;

	MkArray<MkHashStr> themeNames;
	node.GetChildNodeList(themeNames);
	MK_INDEXING_LOOP(themeNames, i)
	{
		const MkHashStr& currThemeName = themeNames[i];
		bool isDefaultTheme = (currThemeName == m_DefaultThemeName);
		const MkDataNode& currNode = *node.GetChildNode(currThemeName);

		MkArray<MkHashStr> subsetNameBuffer[eS2D_WPC_MaxWindowPresetComponent];
		MkArray<MkStr> fontBuffer; // font type, highlight font state, normal font state

		if (currThemeName == m_DefaultThemeName)
		{
			for (int j=eS2D_WPC_BackgroundWindow; j<eS2D_WPC_MaxWindowPresetComponent; ++j)
			{
				if (!_LoadDataAndCheck(currNode, static_cast<eS2D_WindowPresetComponent>(j), currThemeName, true, subsetNameBuffer[j]))
					return false;
			}

			if ((!currNode.GetData(MKDEF_THEME_FONT_TYPE_AND_STATE_KEY, fontBuffer)) || (!_CheckFontData(fontBuffer)))
				return false;

			m_DefaultThemeEnable = true; // 기본 테마는 모든 component를 가지고 있어야 함
		}
		else
		{
			bool disable = true;
			for (int j=eS2D_WPC_BackgroundWindow; j<eS2D_WPC_MaxWindowPresetComponent; ++j)
			{
				if (!_LoadDataAndCheck(currNode, static_cast<eS2D_WindowPresetComponent>(j), currThemeName, false, subsetNameBuffer[j]))
					disable = false;
			}

			if ((!currNode.GetData(MKDEF_THEME_FONT_TYPE_AND_STATE_KEY, fontBuffer)) || (!_CheckFontData(fontBuffer)))
				disable = false;

			if (disable) // 확장 테마는 component를 최소 하나 이상 가지고 있어야 함
				continue;
		}

		MkMap<eS2D_WindowPresetComponent, MkArray<MkHashStr> >& currImageSet = m_ThemeImageSubsets.Create(currThemeName);
		for (int j=eS2D_WPC_BackgroundWindow; j<eS2D_WPC_MaxWindowPresetComponent; ++j)
		{
			if (!subsetNameBuffer[j].Empty())
			{
				currImageSet.Create(static_cast<eS2D_WindowPresetComponent>(j), subsetNameBuffer[j]);
			}
		}

		if (_CheckFontData(fontBuffer))
		{
			MkArray<MkHashStr>& fonts = m_ThemeFonts.Create(currThemeName);
			fonts.Reserve(3);
			fonts.PushBack(fontBuffer[0]);
			fonts.PushBack(fontBuffer[1]);
			fonts.PushBack(fontBuffer[2]);
		}

		MK_DEV_PANEL.MsgToLog(L"> window theme : " + currThemeName.GetString() + MkStr((currThemeName == m_DefaultThemeName) ? L" << [DEF]" : L""), true);
	}

	return true;
}

const MkArray<MkHashStr>& MkWindowPreset::GetWindowTypeImageSet(const MkHashStr& themeName, eS2D_WindowPresetComponent component) const
{
	if (m_ThemeImageSubsets.Exist(themeName))
	{
		const MkMap<eS2D_WindowPresetComponent, MkArray<MkHashStr> >& currTheme = m_ThemeImageSubsets[themeName];
		if (currTheme.Exist(component))
		{
			return currTheme[component];
		}
	}
	
	return (m_DefaultThemeEnable) ? m_ThemeImageSubsets[m_DefaultThemeName][component] : __GetNullList();
}

void MkWindowPreset::Clear(void)
{
	m_DefaultThemeName.Clear();
	m_DefaultThemeEnable = false;
	m_ThemeImageSubsets.Clear();
	m_ThemeFonts.Clear();
}

const static MkHashStr sPresetKeywords[eS2D_WPC_MaxWindowPresetComponent] =
	{ L"BackgroundWindow", L"StaticWindow", L"GuideBox", L"ArrowLeft", L"ArrowRight", L"ArrowUp", L"ArrowDown",
	L"TitleWindow", L"NegativeButton", L"PossitiveButton", L"RootButton", L"ListButton", L"CancelIcon" };

eS2D_WindowPresetComponent MkWindowPreset::GetWindowPresetComponentEnum(const MkHashStr& keyword)
{
	static MkHashMap<MkHashStr, eS2D_WindowPresetComponent> sKeyMap;
	if (sKeyMap.Empty())
	{
		for (int i=eS2D_WPC_BackgroundWindow; i<eS2D_WPC_MaxWindowPresetComponent; ++i)
		{
			sKeyMap.Create(sPresetKeywords[i], static_cast<eS2D_WindowPresetComponent>(i));
		}
		sKeyMap.Rehash();
	}
	return sKeyMap.Exist(keyword) ? sKeyMap[keyword] : eS2D_WPC_None;
}

const MkHashStr& MkWindowPreset::GetWindowPresetComponentKeyword(eS2D_WindowPresetComponent component)
{
	return sPresetKeywords[component];
}

const MkHashStr& MkWindowPreset::GetBackgroundStateKeyword(eS2D_BackgroundState state)
{
	const static MkHashStr sKeywords[eS2D_BS_MaxBackgroundState] = { L"BG" };
	return sKeywords[state];
}

const static MkHashStr sTitleKeywords[eS2D_TS_MaxTitleState] = { L"OnFocus", L"LostFocus" };

const MkHashStr& MkWindowPreset::GetTitleStateKeyword(eS2D_TitleState state)
{
	return sTitleKeywords[state];
}

const MkArray<MkHashStr>& MkWindowPreset::GetTitleStateKeywordList(void)
{
	static MkArray<MkHashStr> sKeyList(eS2D_TS_MaxTitleState);
	if (sKeyList.Empty())
	{
		for (int i=eS2D_TS_OnFocusState; i<eS2D_TS_MaxTitleState; ++i)
		{
			sKeyList.PushBack(sTitleKeywords[i]);
		}
	}
	return sKeyList;
}

const static MkHashStr sWindowKeywords[eS2D_WS_MaxWindowState] = { L"Default", L"OnCursor", L"OnClick", L"Disable" };

const MkHashStr& MkWindowPreset::GetWindowStateKeyword(eS2D_WindowState state)
{
	return sWindowKeywords[state];
}

const MkArray<MkHashStr>& MkWindowPreset::GetWindowStateKeywordList(void)
{
	static MkArray<MkHashStr> sKeyList(eS2D_WS_MaxWindowState);
	if (sKeyList.Empty())
	{
		for (int i=eS2D_WS_DefaultState; i<eS2D_WS_MaxWindowState; ++i)
		{
			sKeyList.PushBack(sWindowKeywords[i]);
		}
	}
	return sKeyList;
}

const MkArray<MkHashStr>& MkWindowPreset::__GetNullList(void)
{
	const static MkArray<MkHashStr> Null;
	return Null;
}

MkWindowPreset::MkWindowPreset()
{
	m_DefaultThemeEnable = false;
	m_Margin = 0.f;
}

bool MkWindowPreset::_LoadDataAndCheck
(const MkDataNode& node, eS2D_WindowPresetComponent component, const MkHashStr& themeName, bool defaultTheme, MkArray<MkHashStr>& buffer) const
{
	MkArray<MkStr> strBuf;
	const MkHashStr& key = GetWindowPresetComponentKeyword(component);
	bool ok = node.GetData(key, strBuf);
	if (defaultTheme)
	{
		MK_CHECK(ok, L"기본 테마에 " + key.GetString() + L" 항목이 없음")
			return false;
	}

	if (ok)
	{
		MK_CHECK(strBuf.GetSize() == GetWindowPresetComponentSize(component), themeName.GetString() + L" 테마에 " + key.GetString() + L" 항목의 크기가 맞지 않음")
			return false;

		MK_INDEXING_LOOP(strBuf, i)
		{
			MK_CHECK(MK_WR_IMAGE_SET.Exist(strBuf[i]), themeName.GetString() + L" 테마의 " + key.GetString() + L" 항목에 존재하지 않는 image subset이 있음")
				return false;
		}

		buffer.Reserve(strBuf.GetSize());
		MK_INDEXING_LOOP(strBuf, i)
		{
			buffer.PushBack(strBuf[i]);
		}
		return true;
	}

	return false;
}

bool MkWindowPreset::_CheckFontData(const MkArray<MkStr>& buffer) const
{
	return ((buffer.GetSize() == 3) && MK_FONT_MGR.CheckAvailableFontType(buffer[0]) &&
		MK_FONT_MGR.CheckAvailableFontState(buffer[1]) && MK_FONT_MGR.CheckAvailableFontState(buffer[2]));
}

const MkHashStr& MkWindowPreset::_GetThemeFont(const MkHashStr& themeName, unsigned int index) const
{
	if (m_ThemeFonts.Exist(themeName))
	{
		return m_ThemeFonts[themeName][index];
	}
	if (m_DefaultThemeEnable)
	{
		return m_ThemeFonts[m_DefaultThemeName][index];
	}
	return MkHashStr::NullHash;
}

//------------------------------------------------------------------------------------------------//
