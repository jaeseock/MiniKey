#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkRect.h"

#include "MkS2D_MkProjectDefinition.h"


#define MK_WIN_EVENT_MGR MkWindowEventManager::Instance()


class MkSceneNode;
class MkBaseWindowNode;
class MkDrawStep;

class MkWindowEventManager : public MkSingletonPattern<MkWindowEventManager>
{
public:

	void SetUp(MkDrawStep* drawStep);

	// 윈도우 등록
	// (NOTE) 호출 전 SetUp()이 완료 된 상태이어야 함
	// (NOTE) 등록될 windowNode는 부모 노드가 없어야 함
	bool RegisterWindow(MkBaseWindowNode* windowNode, bool activate);

	// 윈도우 활성화 여부 반환
	bool IsOnActivation(const MkHashStr& windowName) const;

	// 윈도우 활성화 및 modal 상태 지정
	// modal 상태 해제는 해당 윈도우의 비활성화(DeactivateWindow(windowName))
	void ActivateWindow(const MkHashStr& windowName, bool modal = false);

	// 윈도우 비활성화
	void DeactivateWindow(const MkHashStr& windowName);

	// 윈도우 활성화 여부 토글
	void ToggleWindow(const MkHashStr& windowName);

	// 갱신
	void Update(void);

	inline void SetEditMode(bool enable) { m_EditMode = enable; }
	inline bool GetEditMode(void) const { return m_EditMode; }

	// 해제
	void Clear(void);

	MkWindowEventManager();
	virtual ~MkWindowEventManager() { Clear(); }

protected:

	void _LastWindowLostFocus(void);
	void _SetFocusToWindowNode(MkBaseWindowNode* targetNode);

	void _SetDarkenLayerEnable(bool enable);

	MkFloat2 _ConfineMovement(MkBaseWindowNode* targetNode, const MkFloat2& screenSize, const MkFloat2& posBegin, const MkFloat2& toWorld, const MkFloat2& offset) const;

protected:

	float m_MinDepthBandwidth;
	float m_MaxDepthBandwidth;

	MkDrawStep* m_DrawStep;
	MkSceneNode* m_RootNode;

	bool m_EditMode;

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

	// modal
	MkHashStr m_ModalWindow;

	MkBaseWindowNode* m_CurrentTargetWindowNode;
};
