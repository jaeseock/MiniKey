#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkRect.h"

#include "MkS2D_MkProjectDefinition.h"


#define MK_WIN_EVENT_MGR MkWindowEventManager::Instance()


class MkBaseWindowNode;

class MkWindowEventManager : public MkSingletonPattern<MkWindowEventManager>
{
public:

	void SetDepthBandwidth(float minDepthBandwidth = MKDEF_S2D_MIN_WINDOW_DEPTH_BANDWIDTH, float maxDepthBandwidth = MKDEF_S2D_MAX_WINDOW_DEPTH_BANDWIDTH);

	bool RegisterWindow(MkBaseWindowNode* windowNode, bool activate);

	bool IsOnActivation(const MkHashStr& windowName) const;
	void ActivateWindow(const MkHashStr& windowName);
	void DeactivateWindow(const MkHashStr& windowName);
	void ToggleWindow(const MkHashStr& windowName);

	void Update(const MkFloat2& screenSize);

	// «ÿ¡¶
	void Clear(void);

	MkWindowEventManager();
	virtual ~MkWindowEventManager() { Clear(); }

protected:

	void _LastWindowLostFocus(void);
	void _SetFocusToWindowNode(MkBaseWindowNode* targetNode);

	MkFloat2 _ConfineMovement(MkBaseWindowNode* targetNode, const MkFloat2& screenSize, const MkFloat2& posBegin, const MkFloat2& toWorld, const MkFloat2& offset) const;

protected:

	float m_MinDepthBandwidth;
	float m_MaxDepthBandwidth;

	MkHashMap<MkHashStr, MkBaseWindowNode*> m_WindowTable;

	MkArray<MkHashStr> m_OnActivatingWindows;
	MkArray<MkHashStr> m_WaitForActivatingWindows;
	MkArray<MkHashStr> m_WaitForDeactivatingWindows;

	MkHashStr m_LastFocusWindow;
	bool m_FocusLostByClick;

	// cursor
	bool m_CursorIsDragging;
	MkBaseWindowNode* m_DraggingWindow;
	MkFloat2 m_CursorStartPosition;
	MkFloat2 m_WindowAABRBegin;
	MkFloat2 m_WindowOffsetToWorldPos;

	MkBaseWindowNode* m_CurrentTargetWindowNode;
};
