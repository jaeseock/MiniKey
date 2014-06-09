#ifndef __MINIKEY_CORE_MKACCESSIBILITYSHORTCUTKEYSETTER_H__
#define __MINIKEY_CORE_MKACCESSIBILITYSHORTCUTKEYSETTER_H__

//------------------------------------------------------------------------------------------------//
// Accessibility Shortcut Key 중 StickyKey 와 FilterKey의 사용을 잠금
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include "MkCore_MkStackPattern.h"


class MkAccessibilityShortcutKeySetter
{
public:

	// 고정키와 필터키 꺼버림
	void TurnOffStickyAndFilterKeys(void);

	// 원 설정 복구
	void Restore(void);

	~MkAccessibilityShortcutKeySetter() { Restore(); }

protected:

	MkStackPattern<STICKYKEYS> m_StickyKeyStack;
	MkStackPattern<FILTERKEYS> m_FilterKeyStack;
};

#endif
