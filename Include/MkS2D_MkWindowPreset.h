#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkPathName.h"

#include "MkS2D_MkGlobalDefinition.h"


class MkDataNode;

class MkWindowPreset
{
public:

	// �ʱ�ȭ
	bool SetUp(const MkDataNode& node);

	// �ش� �׸��� image set ��ȯ
	const MkArray<MkHashStr>& GetWindowTypeImageSet(const MkHashStr& themeName, eS2D_WindowPresetComponent component) const;

	// �ش� �׸��� font state ��ȯ
	inline const MkHashStr& GetHighlightThemeFontState(const MkHashStr& themeName) const { return _GetThemeFont(themeName, 0); }
	inline const MkHashStr& GetNormalThemeFontState(const MkHashStr& themeName) const { return _GetThemeFont(themeName, 1); }

	// �⺻ �׸��� ��ȯ
	inline const MkHashStr& GetDefaultThemeName(void) const { return (m_DefaultThemeEnable) ? m_DefaultThemeName : MkHashStr::NullHash; }

	// margin �� ��ȯ
	inline float GetMargin(void) const { return m_Margin; }

	// �ý��� �̹��� ���� ��ȯ
	inline const MkPathName& GetSystemImageFilePath(void) const { return m_SystemImageFilePath; }
	inline const MkHashStr& GetDarkenLayerSubsetName(void) const { return m_DarkenLayerSubsetName; }
	inline const MkHashStr& GetRegionLayerSubsetName(void) const { return m_RegionLayerSubsetName; }
	inline const MkHashStr& GetWindowIconSampleSubsetName(void) const { return m_WindowIconSampleSubsetName; }
	inline const MkHashStr& GetWindowBackgroundSampleSubsetName(void) const { return m_WindowBackgroundSampleSubsetName; }

	// ����
	void Clear(void);

	// keyword�� �ش��ϴ� eS2D_WindowPresetComponent ��ȯ
	static eS2D_WindowPresetComponent GetWindowPresetComponentEnum(const MkHashStr& keyword);

	// component�� �ش��ϴ� keyword ��ȯ
	static const MkHashStr& GetWindowPresetComponentKeyword(eS2D_WindowPresetComponent component);

	// state�� �ش��ϴ� keyword ��ȯ
	static const MkHashStr& GetBackgroundStateKeyword(eS2D_BackgroundState state);

	// state�� �ش��ϴ� keyword ��ȯ
	static const MkHashStr& GetTitleStateKeyword(eS2D_TitleState state);
	static const MkArray<MkHashStr>& GetTitleStateKeywordList(void);

	// state�� �ش��ϴ� keyword ��ȯ
	static const MkHashStr& GetWindowStateKeyword(eS2D_WindowState state);
	static const MkArray<MkHashStr>& GetWindowStateKeywordList(void);

	static const MkArray<MkHashStr>& __GetNullList(void);

	MkWindowPreset();
	~MkWindowPreset() { Clear(); }

protected:

	bool _LoadDataAndCheck(const MkDataNode& node, eS2D_WindowPresetComponent component, const MkHashStr& themeName, bool defaultTheme, MkArray<MkHashStr>& buffer) const;
	bool _CheckFontData(const MkArray<MkStr>& buffer) const;
	const MkHashStr& _GetThemeFont(const MkHashStr& themeName, unsigned int index) const;

protected:

	MkHashStr m_DefaultThemeName;
	bool m_DefaultThemeEnable;

	float m_Margin;
	MkPathName m_SystemImageFilePath;
	MkHashStr m_DarkenLayerSubsetName;
	MkHashStr m_RegionLayerSubsetName;
	MkHashStr m_WindowIconSampleSubsetName;
	MkHashStr m_WindowBackgroundSampleSubsetName;

	MkHashMap<MkHashStr, MkMap<eS2D_WindowPresetComponent, MkArray<MkHashStr> > > m_ThemeImageSubsets;
	MkHashMap<MkHashStr, MkArray<MkHashStr> > m_ThemeFonts;
};


//------------------------------------------------------------------------------------------------//
// �Լ� Ư��ȭ�� ���� state ���� ���� ��ȯ
// ���ø�ȭ ���� ������ ���� ���� case by case�� �ʹ� �������µ��� ���� Ȯ��� �������� �������� ��
//------------------------------------------------------------------------------------------------//

template <class DataType>
class MkWindowPresetStateInterface
{
public:
	static int GetBegin(void) { return 0; }
	static int GetEnd(void) { return 0; }
	static int GetHighlight(void) { return 0; }
	static const MkHashStr& GetKeyword(DataType state) { return MkHashStr::NullHash; }
	static const MkArray<MkHashStr>& GetKeywordList(void) { return MkWindowPreset::__GetNullList(); }
};

template <> int MkWindowPresetStateInterface<eS2D_BackgroundState>::GetBegin(void) { return eS2D_BS_DefaultState; }
template <> int MkWindowPresetStateInterface<eS2D_TitleState>::GetBegin(void) { return eS2D_TS_OnFocusState; }
template <> int MkWindowPresetStateInterface<eS2D_WindowState>::GetBegin(void) { return eS2D_WS_DefaultState; }

template <> int MkWindowPresetStateInterface<eS2D_BackgroundState>::GetEnd(void) { return eS2D_BS_MaxBackgroundState; }
template <> int MkWindowPresetStateInterface<eS2D_TitleState>::GetEnd(void) { return eS2D_TS_MaxTitleState; }
template <> int MkWindowPresetStateInterface<eS2D_WindowState>::GetEnd(void) { return eS2D_WS_MaxWindowState; }

template <> int MkWindowPresetStateInterface<eS2D_TitleState>::GetHighlight(void) { return eS2D_TS_OnFocusState; }
template <> int MkWindowPresetStateInterface<eS2D_WindowState>::GetHighlight(void) { return eS2D_WS_OnCursorState; }

template <> const MkHashStr& MkWindowPresetStateInterface<eS2D_BackgroundState>::GetKeyword(eS2D_BackgroundState state) { return MkWindowPreset::GetBackgroundStateKeyword(state); }
template <> const MkHashStr& MkWindowPresetStateInterface<eS2D_TitleState>::GetKeyword(eS2D_TitleState state) { return MkWindowPreset::GetTitleStateKeyword(state); }
template <> const MkHashStr& MkWindowPresetStateInterface<eS2D_WindowState>::GetKeyword(eS2D_WindowState state) { return MkWindowPreset::GetWindowStateKeyword(state); }

template <> const MkArray<MkHashStr>& MkWindowPresetStateInterface<eS2D_TitleState>::GetKeywordList(void) { return MkWindowPreset::GetTitleStateKeywordList(); }
template <> const MkArray<MkHashStr>& MkWindowPresetStateInterface<eS2D_WindowState>::GetKeywordList(void) { return MkWindowPreset::GetWindowStateKeywordList(); }

//------------------------------------------------------------------------------------------------//
