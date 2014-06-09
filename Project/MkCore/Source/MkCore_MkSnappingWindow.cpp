
#include "MkCore_MkCheck.h"
#include "MkCore_MkSnappingWindow.h"


//------------------------------------------------------------------------------------------------//

bool MkSnappingWindow::SetUpByWindowCreation
(HINSTANCE hInstance, WNDPROC wndProc, HWND hParent, const MkStr& title, eSystemWindowProperty sysWinProp, const MkInt2& position, const MkInt2& clientSize)
{
	bool ok = MkListeningWindow::SetUpByWindowCreation(hInstance, wndProc, hParent, title, sysWinProp, position, clientSize);
	if (ok)
	{
		_SnapToParent();
	}
	return ok;
}

bool MkSnappingWindow::SetUpByOuterWindow(HWND hWnd)
{
	bool ok = MkListeningWindow::SetUpByOuterWindow(hWnd);
	if (ok)
	{
		_SnapToParent();
	}
	return ok;
}

void MkSnappingWindow::ListenPositionUpdate(const MkIntRect& newRect)
{
	SnapWindow(newRect);
}

void MkSnappingWindow::ListenWindowSizeUpdate(const MkIntRect& newRect)
{
	SnapWindow(newRect);
}

void MkSnappingWindow::ListenWindowRectUpdate(const MkIntRect& newRect)
{
	SnapWindow(newRect);
}

void MkSnappingWindow::ListenShowStateUpdate(unsigned int newShowCmd, const MkIntRect& newRect)
{
	if ((newShowCmd == SW_SHOWNORMAL) || (newShowCmd == SW_SHOWMAXIMIZED))
	{
		SnapWindow(newRect);
	}
}

void MkSnappingWindow::ListenWindowRestored(const MkIntRect& newRect)
{
	SnapWindow(newRect);
}

void MkSnappingWindow::SetAlignmentPosition(eRectAlignmentPosition alignmentPosition)
{
	MK_CHECK(IsValidRectAlignmentPosition(alignmentPosition), L"올바른 정렬위치가 아님 : " + MkStr(static_cast<int>(alignmentPosition)))
		return;

	m_AlignmentPosition = alignmentPosition;
	_SnapToParent();
}

void MkSnappingWindow::SnapWindow(const MkIntRect& pivotRect)
{
	MK_CHECK(m_hWnd != NULL, L"초기화되지 않은 윈도우")
		return;

	MkIntRect rect(GetPosition(), GetWindowSize());
	MkInt2 newPosition = pivotRect.GetSnapPosition(rect, m_AlignmentPosition, m_Border, true);
	SetPosition(newPosition);
}

MkSnappingWindow::MkSnappingWindow() : MkListeningWindow()
{
	m_AlignmentPosition = eRAP_NonePosition;
	m_Border = MkInt2(0, 0);
}

//------------------------------------------------------------------------------------------------//

void MkSnappingWindow::_SnapToParent(void)
{
	if ((m_hParent != NULL) && (GetAlignmentPosition() != eRAP_NonePosition))
	{
		MkInt2 parentPos = MkBaseWindow::GetWindowPosition(m_hParent);
		MkInt2 parentSize = MkBaseWindow::GetWindowSize(m_hParent);

		SnapWindow(MkIntRect(parentPos, parentSize));
	}
}

//------------------------------------------------------------------------------------------------//
