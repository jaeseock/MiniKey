
#include "MkCore_MkAccessibilityShortcutKeySetter.h"


//------------------------------------------------------------------------------------------------//

void MkAccessibilityShortcutKeySetter::TurnOffStickyAndFilterKeys(void)
{
	if (m_StickyKeyStack.GetSize() == 0)
	{
		STICKYKEYS sk;
		sk.cbSize = sizeof(STICKYKEYS);
		SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(STICKYKEYS), &sk, 0);
		m_StickyKeyStack.Push(sk);

		sk.dwFlags &= ~(SKF_AVAILABLE | SKF_HOTKEYACTIVE);
		SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &sk, 0);
	}

	if (m_FilterKeyStack.GetSize() == 0)
	{
		FILTERKEYS fk;
		fk.cbSize = sizeof(FILTERKEYS);
		SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(FILTERKEYS), &fk, 0);
		m_FilterKeyStack.Push(fk);

		fk.dwFlags &= ~(FKF_AVAILABLE | FKF_HOTKEYACTIVE);
		SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &fk, 0);
	}
}

void MkAccessibilityShortcutKeySetter::Restore(void)
{
	if (m_StickyKeyStack.GetSize() == 1)
	{
		STICKYKEYS sk;
		m_StickyKeyStack.Pop(sk);
		SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &sk, 0);
	}

	if (m_FilterKeyStack.GetSize() == 1)
	{
		FILTERKEYS fk;
		m_FilterKeyStack.Pop(fk);
		SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &fk, 0);
	}
}

//------------------------------------------------------------------------------------------------//
