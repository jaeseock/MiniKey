
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"

#include "MkS2D_MkWindowResourceManager.h"


//------------------------------------------------------------------------------------------------//

bool MkWindowPreset::SetUp(const MkDataNode& node)
{
	MkStr defaultThemeName;
	MK_CHECK(node.GetData(L"DefaultThemeName", defaultThemeName, 0) && (!defaultThemeName.Empty()), L"window preset 노드에 DefaultThemeName이 지정되어 있지 않음")
		return false;

	m_DefaultThemeName = defaultThemeName;

	MkArray<MkHashStr> themeNames;
	node.GetChildNodeList(themeNames);
	MK_INDEXING_LOOP(themeNames, i)
	{
		const MkHashStr& currThemeName = themeNames[i];
		bool isDefaultTheme = (currThemeName == m_DefaultThemeName);
		const MkDataNode& currNode = *node.GetChildNode(currThemeName);

		MkArray<MkHashStr> subsetNameBuffer[eS2D_WPC_MaxWindowPresetComponent];
		if (currThemeName == m_DefaultThemeName)
		{
			for (int j=eS2D_WPC_BackgroundWindow; j<eS2D_WPC_MaxWindowPresetComponent; ++j)
			{
				if (!_LoadDataAndCheck(currNode, static_cast<eS2D_WindowPresetComponent>(j), currThemeName, true, subsetNameBuffer[j]))
					return false;
			}

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

			if (disable) // 확장 테마는 component를 최소 하나 이상 가지고 있어야 함
				continue;
		}

		MkMap<eS2D_WindowPresetComponent, MkArray<MkHashStr> >& currImageSet = m_Themes.Create(currThemeName);
		for (int j=eS2D_WPC_BackgroundWindow; j<eS2D_WPC_MaxWindowPresetComponent; ++j)
		{
			if (!subsetNameBuffer[j].Empty())
			{
				currImageSet.Create(static_cast<eS2D_WindowPresetComponent>(j), subsetNameBuffer[j]);
			}
		}

		MK_DEV_PANEL.MsgToLog(L"> window theme : " + currThemeName.GetString() + MkStr((currThemeName == m_DefaultThemeName) ? L" << [DEF]" : L""), true);
	}

	return true;
}

const MkArray<MkHashStr>& MkWindowPreset::GetWindowTypeImageSet(const MkHashStr& themeName, eS2D_WindowPresetComponent component) const
{
	const MkMap<eS2D_WindowPresetComponent, MkArray<MkHashStr> >* currThemePtr;
	const MkMap<eS2D_WindowPresetComponent, MkArray<MkHashStr> >& defTheme = m_Themes[m_DefaultThemeName];
	if (m_Themes.Exist(themeName))
	{
		currThemePtr = &m_Themes[themeName];
	}
	else
	{
		currThemePtr = (m_DefaultThemeEnable) ? &defTheme : NULL;
	}

	if (currThemePtr == NULL)
	{
		const static MkArray<MkHashStr> Null;
		return Null;
	}

	return currThemePtr->Exist(component) ? (*currThemePtr)[component] : defTheme[component];
}

void MkWindowPreset::Clear(void)
{
	m_DefaultThemeName.Clear();
	m_DefaultThemeEnable = false;
	m_Themes.Clear();
}

const static MkHashStr sPresetKeywords[eS2D_WPC_MaxWindowPresetComponent] = { L"BackgroundWindow", L"TitleWindow", L"NegativeButton", L"PossitiveButton", L"CancelIcon" };

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
	const static MkHashStr sKeywords[eS2D_BS_MaxBackgroundState] = { L"Default" };
	return sKeywords[state];
}

const MkHashStr& MkWindowPreset::GetTitleStateKeyword(eS2D_TitleState state)
{
	const static MkHashStr sKeywords[eS2D_TS_MaxTitleState] = { L"OnFocus", L"LostFocus" };
	return sKeywords[state];
}

const MkHashStr& MkWindowPreset::GetWindowStateKeyword(eS2D_WindowState state)
{
	const static MkHashStr sKeywords[eS2D_WS_MaxWindowState] = { L"Default", L"OnCursor", L"OnClick", L"Disable" };
	return sKeywords[state];
}

MkWindowPreset::MkWindowPreset()
{
	m_DefaultThemeEnable = false;
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

//------------------------------------------------------------------------------------------------//
