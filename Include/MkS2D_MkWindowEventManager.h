#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkRect.h"
#include "MkCore_MkTimeCounter.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkBaseTexture.h"


#define MK_WIN_EVENT_MGR MkWindowEventManager::Instance()


class MkSceneNode;
class MkBaseSystemWindow;
class MkNodeNameInputListener;
class MkSRectInfoListener;

class MkDrawStep;
class MkSceneNode;
class MkBaseWindowNode;
class MkSpreadButtonNode;

class MkWindowEventManager : public MkSingletonPattern<MkWindowEventManager>
{
public:

	void SetUp(const MkBaseTexturePtr& sceneTexture);

	// 윈도우 등록
	// (NOTE) 호출 전 SetUp()이 완료 된 상태이어야 함
	// (NOTE) 등록될 windowNode는 부모 노드가 없어야 함
	bool RegisterWindow(MkBaseWindowNode* windowNode, bool activate);

	// 등록된 윈도우 삭제
	void RemoveWindow(const MkHashStr& windowName);

	// 윈도우 활성화 여부 반환
	bool IsOnActivation(const MkHashStr& windowName) const;

	// 윈도우 활성화 및 modal 상태 지정
	// modal 상태 해제는 해당 윈도우의 비활성화(DeactivateWindow(windowName))
	void ActivateWindow(const MkHashStr& windowName, bool modal = false);

	// 윈도우 비활성화
	void DeactivateWindow(const MkHashStr& windowName);

	// 윈도우 활성화 여부 토글
	void ToggleWindow(const MkHashStr& windowName, bool modal = false);

	// 갱신
	void Update(void);

	// 에디트 모드 설정
	inline void SetEditMode(bool enable) { m_EditMode = enable; }
	inline bool GetEditMode(void) const { return m_EditMode; }

	// 현재 지정상태인 윈도우 반환
	inline MkBaseWindowNode* GetFrontHitWindow(void) const { return m_FrontHitWindow; }

	// 그리기 영역 반환
	const MkFloatRect& GetRegionRect(void) const;

	// 지정 윈도우 설정
	void SetTargetWindowNode(MkBaseWindowNode* targetWindow);

	// 지정 윈도우를 저장
	inline void SaveCurrentTargetWindowNode(const MkPathName& filePath) { m_SaveCurrentTargetWindowNodePath = filePath; }

	// system window
	void OpenNodeNameInputSystemWindow(MkSceneNode* targetNode, MkNodeNameInputListener* owner = NULL);
	void OpenWindowAttributeSystemWindow(MkBaseWindowNode* targetWindow);
	void OpenSRectSetterSystemWindow(MkSRectInfoListener* owner, MkSceneNode* targetNode, const MkHashStr& rectName, int inputType);

	// 해제
	void Clear(void);

	MkWindowEventManager();
	virtual ~MkWindowEventManager() { Clear(); }

public:

	void __SpreadButtonOpened(int index, MkSpreadButtonNode* button);
	void __SpreadButtonClosed(int index);

	void __ShowDebugLayer(const MkFloatRect& rect, float depth);
	void __HideDebugLayer(void);

	void __RootWindowNameChanged(const MkHashStr& oldName, const MkHashStr& newName);

	// setting
	inline void __SetShowWindowSelection(bool enable) { m_ShowWindowSelection = enable; }
	inline bool __GetShowWindowSelection(void) { return m_ShowWindowSelection; }
	inline void __SetAllowDragMovement(bool enable) { m_AllowDragMovement = enable; }
	inline bool __GetAllowDragMovement(void) { return m_AllowDragMovement; }

protected:

	void _CreateSystemWindow(MkBaseSystemWindow* systemWindow);

	void _LastWindowLostFocus(void);
	void _SetFocusToWindowNode(MkBaseWindowNode* targetNode);

	void _SetDarkenLayerEnable(bool enable);

	MkFloat2 _ConfineMovement(MkBaseWindowNode* targetNode, const MkFloat2& posBegin, const MkFloat2& toWorld, const MkFloat2& offset) const;

	// 영역지정용 레이어 설정
	void _ShowRegionLayer(void);
	void _UpdateRegionLayer(const MkFloatRect& rect, float depth);
	bool _SetRegionLayerVisible(bool enable);

	void _PassTargetWindow(bool ignoreActivationCheck = false);

	void _ChangeWindowName(const MkHashStr& oldName, const MkHashStr& newName, MkArray<MkHashStr>& targetList);

protected:

	float m_MinDepthBandwidth;
	float m_MaxDepthBandwidth;
	float m_SceneLayerDepth;

	MkDrawStep* m_DrawStep;
	MkSceneNode* m_RootNode;

	MkTimeCounter m_RegionLayerCounter;

	bool m_EditMode;

	MkHashMap<MkHashStr, MkBaseWindowNode*> m_WindowTable;

	MkArray<MkHashStr> m_OnActivatingWindows;
	MkArray<MkHashStr> m_WaitForActivatingWindows;
	MkArray<MkHashStr> m_WaitForDeactivatingWindows;

	MkHashStr m_LastFocusWindow;
	bool m_FocusLostByClick;

	// front window
	MkBaseWindowNode* m_FrontHitWindow;
	MkFloat2 m_CursorStartPosition;
	MkFloat2 m_WindowAABRBegin;
	MkFloat2 m_WindowOffsetToWorldPos;

	// modal
	MkHashStr m_ModalWindow;

	// spread button
	MkArray<MkSpreadButtonNode*> m_OpeningSpreadButtons;

	// setting
	bool m_ShowWindowSelection;
	bool m_AllowDragMovement;

	// windows for edit mode
	MkBaseWindowNode* m_CurrentTargetWindowNode;

	// save target
	MkPathName m_SaveCurrentTargetWindowNodePath;
};
