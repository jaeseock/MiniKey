#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"


#define MK_WIN_EVENT_MGR MkWindowEventManager::Instance()


class MkBaseWindowNode;

class MkWindowEventManager : public MkSingletonPattern<MkWindowEventManager>
{
public:

	bool RegisterWindow(MkBaseWindowNode* windowNode, bool activate);

	bool IsOnActivation(const MkHashStr& windowName) const;
	void ActivateWindow(const MkHashStr& windowName);
	void DeactivateWindow(const MkHashStr& windowName);
	void ToggleWindow(const MkHashStr& windowName);

	void Update(void);

	// «ÿ¡¶
	void Clear(void);

	MkWindowEventManager() : MkSingletonPattern<MkWindowEventManager>() {};
	virtual ~MkWindowEventManager() { Clear(); }

protected:

	MkHashMap<MkHashStr, MkBaseWindowNode*> m_WindowTable;

	MkArray<MkHashStr> m_OnActivatingWindows;
	MkArray<MkHashStr> m_WaitForActivatingWindows;
	MkArray<MkHashStr> m_WaitForDeactivatingWindows;

	MkHashStr m_LastFocusWindow;
};
