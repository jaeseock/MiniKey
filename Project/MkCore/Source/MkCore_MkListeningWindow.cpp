
#include "MkCore_MkListeningWindow.h"

#define MKDEF_CAST_CURRENT_RECT(proceed, func_name) \
	if (proceed && (!m_Listener.Empty())) \
	{ \
		MkIntRect rect(GetPosition(), GetWindowSize()); \
		MK_INDEXING_LOOP(m_Listener, i) \
		{ \
			m_Listener[i]->func_name(rect); \
		} \
	}

//------------------------------------------------------------------------------------------------//

unsigned int MkListeningWindow::RegisterWindowUpdateListener(MkWindowUpdateListener* reciever)
{
	if (reciever == NULL)
		return 0xffffffff;

	unsigned int index = m_Listener.GetSize();
	m_Listener.PushBack(reciever);
	return index;
}

void MkListeningWindow::RemoveWindowUpdateListener(unsigned int index)
{
	if (m_Listener.IsValidIndex(index))
	{
		m_Listener.Erase(MkArraySection(index, 1));
	}
}

void MkListeningWindow::ClearAllWindowUpdateListener(void)
{
	m_Listener.Clear();
}

//------------------------------------------------------------------------------------------------//

bool MkListeningWindow::SetUpByWindowCreation
(HINSTANCE hInstance, WNDPROC wndProc, HWND hParent, const MkStr& title, eSystemWindowProperty sysWinProp,
 const MkInt2& position, const MkInt2& clientSize)
{
	bool ok = MkBaseWindow::SetUpByWindowCreation(hInstance, wndProc, hParent, title, sysWinProp, position, clientSize);
	MKDEF_CAST_CURRENT_RECT(ok, ListenWindowRectUpdate);
	return ok;
}

bool MkListeningWindow::SetUpByOuterWindow(HWND hWnd)
{
	bool ok = MkBaseWindow::SetUpByOuterWindow(hWnd);
	MKDEF_CAST_CURRENT_RECT(ok, ListenWindowRectUpdate);
	return ok;
}

bool MkListeningWindow::SetPosition(const MkInt2& position)
{
	bool ok = MkBaseWindow::SetPosition(position);
	MKDEF_CAST_CURRENT_RECT(ok, ListenPositionUpdate);
	return ok;
}

bool MkListeningWindow::SetWindowSize(const MkInt2& size)
{
	bool ok = MkBaseWindow::SetWindowSize(size);
	MKDEF_CAST_CURRENT_RECT(ok, ListenWindowSizeUpdate);
	return ok;
}

bool MkListeningWindow::SetOnTop(void)
{
	bool ok = MkBaseWindow::SetOnTop();
	if (ok && (!m_Listener.Empty()))
	{
		MK_INDEXING_LOOP(m_Listener, i)
		{
			m_Listener[i]->ListenOnTopUpdate();
		}
	}
	return ok;
}

bool MkListeningWindow::SetShowCmd(unsigned int showCmd)
{
	bool ok = MkBaseWindow::SetShowCmd(showCmd);
	if (ok && (!m_Listener.Empty()))
	{
		unsigned int newCmd = GetShowCmd();
		MkIntRect rect(GetPosition(), GetWindowSize());
		MK_INDEXING_LOOP(m_Listener, i)
		{
			m_Listener[i]->ListenShowCmdUpdate(newCmd, rect);
		}
	}
	return ok;
}

void MkListeningWindow::WindowRestored(void)
{
	MkBaseWindow::WindowRestored();
	MKDEF_CAST_CURRENT_RECT(true, ListenWindowRestored);
}

void MkListeningWindow::Clear(void)
{
	MkBaseWindow::Clear();
	ClearAllWindowUpdateListener();
}

//------------------------------------------------------------------------------------------------//
