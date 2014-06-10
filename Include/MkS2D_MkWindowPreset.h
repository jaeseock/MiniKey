#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"

#include "MkS2D_MkGlobalDefinition.h"


class MkDataNode;

class MkWindowPreset
{
public:

	// �ʱ�ȭ
	bool SetUp(const MkDataNode& node);

	// �ش� window type image set ��ȯ
	const MkArray<MkHashStr>& GetWindowTypeImageSet(const MkHashStr& themeName, eS2D_WindowPresetComponent component) const;

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

	// state�� �ش��ϴ� keyword ��ȯ
	static const MkHashStr& GetWindowStateKeyword(eS2D_WindowState state);

	MkWindowPreset();
	~MkWindowPreset() { Clear(); }

protected:

	bool _LoadDataAndCheck(const MkDataNode& node, eS2D_WindowPresetComponent component, const MkHashStr& themeName, bool defaultTheme, MkArray<MkHashStr>& buffer) const;

protected:

	MkHashStr m_DefaultThemeName;
	bool m_DefaultThemeEnable;

	MkHashMap<MkHashStr, MkMap<eS2D_WindowPresetComponent, MkArray<MkHashStr> > > m_Themes;
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
	static const MkHashStr& GetKeyword(DataType state) { return MkHashStr::NullHash; }
};

template <> int MkWindowPresetStateInterface<eS2D_BackgroundState>::GetBegin(void) { return eS2D_BS_DefaultState; }
template <> int MkWindowPresetStateInterface<eS2D_TitleState>::GetBegin(void) { return eS2D_TS_OnFocusState; }
template <> int MkWindowPresetStateInterface<eS2D_WindowState>::GetBegin(void) { return eS2D_WS_DefaultState; }

template <> int MkWindowPresetStateInterface<eS2D_BackgroundState>::GetEnd(void) { return eS2D_BS_MaxBackgroundState; }
template <> int MkWindowPresetStateInterface<eS2D_TitleState>::GetEnd(void) { return eS2D_TS_MaxTitleState; }
template <> int MkWindowPresetStateInterface<eS2D_WindowState>::GetEnd(void) { return eS2D_WS_MaxWindowState; }

template <> const MkHashStr& MkWindowPresetStateInterface<eS2D_BackgroundState>::GetKeyword(eS2D_BackgroundState state) { return MkWindowPreset::GetBackgroundStateKeyword(state); }
template <> const MkHashStr& MkWindowPresetStateInterface<eS2D_TitleState>::GetKeyword(eS2D_TitleState state) { return MkWindowPreset::GetTitleStateKeyword(state); }
template <> const MkHashStr& MkWindowPresetStateInterface<eS2D_WindowState>::GetKeyword(eS2D_WindowState state) { return MkWindowPreset::GetWindowStateKeyword(state); }

//------------------------------------------------------------------------------------------------//
