
#include "MkCore_MkStr.h"
#include "MkCore_MkClipboard.h"


//------------------------------------------------------------------------------------------------//

bool MkClipboard::SetText(HWND hWnd, const MkStr& msg)
{
	unsigned int memorySize = (msg.GetSize() + 1) * sizeof(wchar_t);
	HANDLE memoryHandle = ::GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, memorySize);
	char* dest = reinterpret_cast<char*>(::GlobalLock(memoryHandle));
	if (dest != NULL)
	{
		char* src = reinterpret_cast<char*>(const_cast<wchar_t*>(msg.GetPtr()));
		memcpy_s(dest, memorySize, src, memorySize);
		::GlobalUnlock(memoryHandle);

		if (::OpenClipboard(hWnd))
		{
			::EmptyClipboard();
			::SetClipboardData(CF_UNICODETEXT, memoryHandle);
			::CloseClipboard();
			return true;
		}
	}
	return false;
}

bool MkClipboard::GetText(HWND hWnd, MkStr& buffer)
{
	unsigned int priorityList = CF_UNICODETEXT;
	if (::GetPriorityClipboardFormat(&priorityList, 1) == CF_UNICODETEXT)
	{
		if (::OpenClipboard(hWnd))
		{
			HANDLE dataHandle = ::GetClipboardData(CF_UNICODETEXT);
			bool ok = (dataHandle != NULL);
			if (ok)
			{
				buffer = reinterpret_cast<wchar_t*>(::GlobalLock(dataHandle));
				::GlobalUnlock(dataHandle);
			}
			::CloseClipboard();
			return ok;
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------//
