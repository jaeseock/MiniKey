#ifndef __MINIKEY_CORE_MKACCESSIBILITYSHORTCUTKEYSETTER_H__
#define __MINIKEY_CORE_MKACCESSIBILITYSHORTCUTKEYSETTER_H__

//------------------------------------------------------------------------------------------------//
// Accessibility Shortcut Key �� StickyKey �� FilterKey�� ����� ���
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include "MkCore_MkStackPattern.h"


class MkAccessibilityShortcutKeySetter
{
public:

	// ����Ű�� ����Ű ������
	void TurnOffStickyAndFilterKeys(void);

	// �� ���� ����
	void Restore(void);

	~MkAccessibilityShortcutKeySetter() { Restore(); }

protected:

	MkStackPattern<STICKYKEYS> m_StickyKeyStack;
	MkStackPattern<FILTERKEYS> m_FilterKeyStack;
};

#endif
