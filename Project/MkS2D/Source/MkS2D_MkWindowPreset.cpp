
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

		MkArray<MkHashStr> baseWindowSN, windowTitleSN, negativeButtonSN, possitiveButtonSN, cancelIconSN;
		if (currThemeName == m_DefaultThemeName)
		{
			if (!_LoadDataAndCheck(currNode, GetWindowPresetComponentKeyword(eS2D_WPC_BaseWindow), currThemeName, 1, true, baseWindowSN))
				return false;

			if (!_LoadDataAndCheck(currNode, GetWindowPresetComponentKeyword(eS2D_WPC_WindowTitle), currThemeName, eS2D_TS_MaxTitleState, true, windowTitleSN))
				return false;

			if (!_LoadDataAndCheck(currNode, GetWindowPresetComponentKeyword(eS2D_WPC_NegativeButton), currThemeName, eS2D_WS_MaxWindowState, true, negativeButtonSN))
				return false;

			if (!_LoadDataAndCheck(currNode, GetWindowPresetComponentKeyword(eS2D_WPC_PossitiveButton), currThemeName, eS2D_WS_MaxWindowState, true, possitiveButtonSN))
				return false;

			if (!_LoadDataAndCheck(currNode, GetWindowPresetComponentKeyword(eS2D_WPC_CancelIcon), currThemeName, eS2D_WS_MaxWindowState, true, cancelIconSN))
				return false;

			m_DefaultThemeEnable = true;
		}
		else
		{
			bool skip = true;
			if (!_LoadDataAndCheck(currNode, GetWindowPresetComponentKeyword(eS2D_WPC_BaseWindow), currThemeName, 1, false, baseWindowSN))
				skip = false;

			if (!_LoadDataAndCheck(currNode, GetWindowPresetComponentKeyword(eS2D_WPC_WindowTitle), currThemeName, eS2D_TS_MaxTitleState, false, windowTitleSN))
				skip = false;

			if (!_LoadDataAndCheck(currNode, GetWindowPresetComponentKeyword(eS2D_WPC_NegativeButton), currThemeName, eS2D_WS_MaxWindowState, false, negativeButtonSN))
				skip = false;

			if (!_LoadDataAndCheck(currNode, GetWindowPresetComponentKeyword(eS2D_WPC_PossitiveButton), currThemeName, eS2D_WS_MaxWindowState, false, possitiveButtonSN))
				skip = false;

			if (!_LoadDataAndCheck(currNode, GetWindowPresetComponentKeyword(eS2D_WPC_CancelIcon), currThemeName, eS2D_WS_MaxWindowState, false, cancelIconSN))
				skip = false;

			if (skip) // 하나라도 값을 만족하면 테마 성립
				continue;
		}

		MkMap<eS2D_WindowPresetComponent, MkArray<MkHashStr> >& currImageSet = m_Themes.Create(currThemeName);

		if (!baseWindowSN.Empty())
		{
			currImageSet.Create(eS2D_WPC_BaseWindow, baseWindowSN);
		}
		if (!windowTitleSN.Empty())
		{
			currImageSet.Create(eS2D_WPC_WindowTitle, windowTitleSN);
		}
		if (!negativeButtonSN.Empty())
		{
			currImageSet.Create(eS2D_WPC_NegativeButton, negativeButtonSN);
		}
		if (!possitiveButtonSN.Empty())
		{
			currImageSet.Create(eS2D_WPC_PossitiveButton, possitiveButtonSN);
		}
		if (!cancelIconSN.Empty())
		{
			currImageSet.Create(eS2D_WPC_CancelIcon, cancelIconSN);
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

const MkHashStr& MkWindowPreset::GetWindowPresetComponentKeyword(eS2D_WindowPresetComponent component)
{
	const static MkHashStr sKeywords[eS2D_WPC_MaxWindowPresetComponent] = { L"BaseWindow", L"WindowTitle", L"NegativeButton", L"PossitiveButton", L"CancelIcon" };
	return sKeywords[component];
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
(const MkDataNode& node, const MkHashStr& key, const MkHashStr& themeName, unsigned int size, bool defaultTheme, MkArray<MkHashStr>& buffer) const
{
	MkArray<MkStr> strBuf;
	bool ok = node.GetData(key, strBuf);
	if (defaultTheme)
	{
		MK_CHECK(ok, L"기본 테마에 " + key.GetString() + L" 항목이 없음")
			return false;
	}

	if (ok)
	{
		MK_CHECK(strBuf.GetSize() == size, themeName.GetString() + L" 테마에 " + key.GetString() + L" 항목의 크기가 맞지 않음")
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
