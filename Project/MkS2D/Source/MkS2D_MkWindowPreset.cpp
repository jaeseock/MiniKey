
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"

#include "MkS2D_MkWindowResourceManager.h"


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

	MkStr windowIconSampleSubsetName;
	MK_CHECK(node.GetData(L"WindowIconSampleSubsetName", windowIconSampleSubsetName, 0) && (!windowIconSampleSubsetName.Empty()), L"window preset ��忡 WindowIconSampleSubsetName�� �����Ǿ� ���� ����")
		return false;
	m_WindowIconSampleSubsetName = windowIconSampleSubsetName;

	MkStr windowBackgroundSampleSubsetName;
	MK_CHECK(node.GetData(L"WindowBackgroundSampleSubsetName", windowBackgroundSampleSubsetName, 0) && (!windowBackgroundSampleSubsetName.Empty()), L"window preset ��忡 WindowBackgroundSampleSubsetName�� �����Ǿ� ���� ����")
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
		if (currThemeName == m_DefaultThemeName)
		{
			for (int j=eS2D_WPC_BackgroundWindow; j<eS2D_WPC_MaxWindowPresetComponent; ++j)
			{
				if (!_LoadDataAndCheck(currNode, static_cast<eS2D_WindowPresetComponent>(j), currThemeName, true, subsetNameBuffer[j]))
					return false;
			}

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

			if (disable) // Ȯ�� �׸��� component�� �ּ� �ϳ� �̻� ������ �־�� ��
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
		return __GetNullList();
	}

	return currThemePtr->Exist(component) ? (*currThemePtr)[component] : defTheme[component];
}

void MkWindowPreset::Clear(void)
{
	m_DefaultThemeName.Clear();
	m_DefaultThemeEnable = false;
	m_Themes.Clear();
}

const static MkHashStr sPresetKeywords[eS2D_WPC_MaxWindowPresetComponent] =
	{ L"BackgroundWindow", L"TitleWindow", L"NegativeButton", L"PossitiveButton", L"ListButton", L"CancelIcon" };

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

//------------------------------------------------------------------------------------------------//
