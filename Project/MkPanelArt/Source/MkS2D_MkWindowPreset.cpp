
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"

#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkWindowResourceManager.h"


#define MKDEF_THEME_FONT_STATE_KEY L"FontState"

//------------------------------------------------------------------------------------------------//

bool MkWindowPreset::SetUp(const MkDataNode& node)
{
	MkStr defaultThemeName;
	MK_CHECK(node.GetData(L"DefaultThemeName", defaultThemeName, 0) && (!defaultThemeName.Empty()), L"window preset ��忡 DefaultThemeName�� �����Ǿ� ���� ����")
		return false;
	m_DefaultThemeName = defaultThemeName;

	int margin;
	MK_CHECK(node.GetData(L"Margin", margin, 0) && (margin > 0), L"window preset ��忡 Margin�� ���������� �����Ǿ� ���� ����")
		return false;
	m_Margin = static_cast<float>(margin);

	MkStr systemImageFilePath;
	MK_CHECK(node.GetData(L"SystemImageFilePath", systemImageFilePath, 0) && (!systemImageFilePath.Empty()), L"window preset ��忡 SystemImageFilePath�� �����Ǿ� ���� ����")
		return false;
	m_SystemImageFilePath = systemImageFilePath;

	MkStr darkenLayerSubsetName;
	MK_CHECK(node.GetData(L"DarkenLayerSubsetName", darkenLayerSubsetName, 0) && (!darkenLayerSubsetName.Empty()), L"window preset ��忡 DarkenLayerSubsetName�� �����Ǿ� ���� ����")
		return false;
	m_DarkenLayerSubsetName = darkenLayerSubsetName;

	MkStr regionLayerSubsetName;
	MK_CHECK(node.GetData(L"RegionLayerSubsetName", regionLayerSubsetName, 0) && (!regionLayerSubsetName.Empty()), L"window preset ��忡 RegionLayerSubsetName�� �����Ǿ� ���� ����")
		return false;
	m_RegionLayerSubsetName = regionLayerSubsetName;

	MkStr windowIconSampleSubsetName;
	MK_CHECK(node.GetData(L"WindowIconSampleSubsetName", windowIconSampleSubsetName, 0) && (!windowIconSampleSubsetName.Empty()), L"window preset ��忡 WindowIconSampleSubsetName�� �����Ǿ� ���� ����")
		return false;
	m_WindowIconSampleSubsetName = windowIconSampleSubsetName;

	MkStr windowBackgroundSampleSubsetName;
	MK_CHECK(node.GetData(L"WindowBackgroundSampleSubsetName", windowBackgroundSampleSubsetName, 0) && (!windowBackgroundSampleSubsetName.Empty()), L"window preset ��忡 WindowBackgroundSampleSubsetName�� �����Ǿ� ���� ����")
		return false;
	m_WindowBackgroundSampleSubsetName = windowBackgroundSampleSubsetName;

	MkStr windowSelectIconSubsetName;
	MK_CHECK(node.GetData(L"WindowSelectIconSubsetName", windowSelectIconSubsetName, 0) && (!windowSelectIconSubsetName.Empty()), L"window preset ��忡 WindowSelectIconSubsetName�� �����Ǿ� ���� ����")
		return false;
	m_WindowSelectIconSubsetName = windowSelectIconSubsetName;

	MkStr editBoxFontType;
	MK_CHECK(node.GetData(L"EditBoxFontType", editBoxFontType, 0) && (!editBoxFontType.Empty()), L"window preset ��忡 EditBoxFontType�� �����Ǿ� ���� ����")
		return false;
	m_EditBoxFontType = editBoxFontType;

	MkStr editBoxNormalFontState;
	MK_CHECK(node.GetData(L"EditBoxNormalFontState", editBoxNormalFontState, 0) && (!editBoxNormalFontState.Empty()), L"window preset ��忡 EditBoxNormalFontState�� �����Ǿ� ���� ����")
		return false;
	m_EditBoxNormalFontState = editBoxNormalFontState;

	MkStr editBoxSelectionFontState;
	MK_CHECK(node.GetData(L"EditBoxSelectionFontState", editBoxSelectionFontState, 0) && (!editBoxSelectionFontState.Empty()), L"window preset ��忡 EditBoxSelectionFontState�� �����Ǿ� ���� ����")
		return false;
	m_EditBoxSelectionFontState = editBoxSelectionFontState;

	MkStr editBoxCursorFontState;
	MK_CHECK(node.GetData(L"EditBoxCursorFontState", editBoxCursorFontState, 0) && (!editBoxCursorFontState.Empty()), L"window preset ��忡 EditBoxCursorFontState�� �����Ǿ� ���� ����")
		return false;
	m_EditBoxCursorFontState = editBoxCursorFontState;

	MkStr editBoxSelectionSubsetName;
	MK_CHECK(node.GetData(L"EditBoxSelectionSubsetName", editBoxSelectionSubsetName, 0) && (!editBoxSelectionSubsetName.Empty()), L"window preset ��忡 EditBoxSelectionSubsetName�� �����Ǿ� ���� ����")
		return false;
	m_EditBoxSelectionSubsetName = editBoxSelectionSubsetName;

	MkArray<MkStr> noticeButtonSubsetName;
	MK_CHECK(node.GetData(L"NoticeButtonSubsetName", noticeButtonSubsetName) && (!noticeButtonSubsetName.Empty()), L"window preset ��忡 NoticeButtonSubsetName�� �����Ǿ� ���� ����")
		return false;

	m_NoticeButtonSubsetName.Flush();
	m_NoticeButtonSubsetName.Reserve(noticeButtonSubsetName.GetSize());
	MK_INDEXING_LOOP(noticeButtonSubsetName, i)
	{
		m_NoticeButtonSubsetName.PushBack(noticeButtonSubsetName[i]);
	}

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

			if ((!currNode.GetData(MKDEF_THEME_FONT_STATE_KEY, fontBuffer)) || (!_CheckFontData(fontBuffer)))
				return false;

			m_DefaultThemeEnable = true; // �⺻ �׸��� ��� component�� ������ �־�� ��
		}
		else
		{
			bool disable = true;
			for (int j=eS2D_WPC_BackgroundWindow; j<eS2D_WPC_MaxWindowPresetComponent; ++j)
			{
				if (!_LoadDataAndCheck(currNode, static_cast<eS2D_WindowPresetComponent>(j), currThemeName, false, subsetNameBuffer[j]))
					disable = false;
			}

			if ((!currNode.GetData(MKDEF_THEME_FONT_STATE_KEY, fontBuffer)) || (!_CheckFontData(fontBuffer)))
				disable = false;

			if (disable) // Ȯ�� �׸��� component�� �ּ� �ϳ� �̻� ������ �־�� ��
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
			fonts.Reserve(2);
			fonts.PushBack(fontBuffer[0]);
			fonts.PushBack(fontBuffer[1]);
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
	{ L"BackgroundWindow", L"StaticWindow", L"GuideBox", L"ArrowLeft", L"ArrowRight", L"ArrowUp", L"ArrowDown", L"VSlideBar", L"HSlideBar", L"TabFront", // bg
	L"NormalTitle", L"SystemMsgTitle", L"SystemErrorTitle", // title
	L"NormalBtn", L"OKBtn", L"CancelBtn", L"SelectionBtn", L"TabRearBtn", L"RootBtn", L"ListBtn", L"UncheckBtn", L"CheckBtn",
	L"DirLeftBtn", L"DirRightBtn", L"DirUpBtn", L"DirDownBtn", L"VSlideBtn", L"HSlideBtn", L"CloseBtn" };

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
	return (component == eS2D_WPC_None) ? MkHashStr::NullHash : sPresetKeywords[component];
}

const static MkHashStr sBackgroundKeywords[eS2D_BS_MaxBackgroundState] = { L"__#BG" };

const MkHashStr& MkWindowPreset::GetBackgroundStateKeyword(eS2D_BackgroundState state)
{
	return sBackgroundKeywords[state];
}

const MkArray<MkHashStr>& MkWindowPreset::GetBackgroundStateKeywordList(void)
{
	static MkArray<MkHashStr> sKeyList(eS2D_BS_MaxBackgroundState);
	if (sKeyList.Empty())
	{
		for (int i=eS2D_BS_DefaultState; i<eS2D_BS_MaxBackgroundState; ++i)
		{
			sKeyList.PushBack(sBackgroundKeywords[i]);
		}
	}
	return sKeyList;
}

const static MkHashStr sTitleKeywords[eS2D_TS_MaxTitleState] = { L"__#OnFocus", L"__#LostFocus" };

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

const static MkHashStr sWindowKeywords[eS2D_WS_MaxWindowState] = { L"__#Default", L"__#OnCursor", L"__#OnClick", L"__#Disable" };

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
		MK_CHECK(ok, L"�⺻ �׸��� " + key.GetString() + L" �׸��� ����")
			return false;
	}

	if (ok)
	{
		MK_CHECK(strBuf.GetSize() == GetWindowPresetComponentSize(component), themeName.GetString() + L" �׸��� " + key.GetString() + L" �׸��� ũ�Ⱑ ���� ����")
			return false;

		MK_INDEXING_LOOP(strBuf, i)
		{
			MK_CHECK(MK_WR_IMAGE_SET.Exist(strBuf[i]), themeName.GetString() + L" �׸��� " + key.GetString() + L" �׸� �������� �ʴ� image subset�� ����")
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
	return ((buffer.GetSize() == 2) && MK_FONT_MGR.CheckAvailableFontState(buffer[0]) && MK_FONT_MGR.CheckAvailableFontState(buffer[1]));
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