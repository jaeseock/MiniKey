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

	// 초기화
	bool SetUp(const MkDataNode& node);

	// 해당 window type image set 반환
	const MkArray<MkHashStr>& GetWindowTypeImageSet(const MkHashStr& themeName, eS2D_WindowPresetComponent component) const;

	// 해제
	void Clear(void);

	// eS2D_WindowPresetComponent keyword 반환
	static const MkHashStr& GetWindowPresetComponentKeyword(eS2D_WindowPresetComponent component);

	// eS2D_TitleState keyword 반환
	static const MkHashStr& GetTitleStateKeyword(eS2D_TitleState state);

	// eS2D_WindowState keyword 반환
	static const MkHashStr& GetWindowStateKeyword(eS2D_WindowState state);

	MkWindowPreset();
	~MkWindowPreset() { Clear(); }

protected:

	bool _LoadDataAndCheck(const MkDataNode& node, const MkHashStr& key, const MkHashStr& themeName, unsigned int size, bool defaultTheme, MkArray<MkHashStr>& buffer) const;

protected:

	MkHashStr m_DefaultThemeName;
	bool m_DefaultThemeEnable;

	MkHashMap<MkHashStr, MkMap<eS2D_WindowPresetComponent, MkArray<MkHashStr> > > m_Themes;
};
