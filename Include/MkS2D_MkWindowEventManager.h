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

	// ������ ���
	// (NOTE) ȣ�� �� SetUp()�� �Ϸ� �� �����̾�� ��
	// (NOTE) ��ϵ� windowNode�� �θ� ��尡 ����� ��
	bool RegisterWindow(MkBaseWindowNode* windowNode, bool activate);

	// ������ Ȱ��ȭ ���� ��ȯ
	bool IsOnActivation(const MkHashStr& windowName) const;

	// ������ Ȱ��ȭ �� modal ���� ����
	// modal ���� ������ �ش� �������� ��Ȱ��ȭ(DeactivateWindow(windowName))
	void ActivateWindow(const MkHashStr& windowName, bool modal = false);

	// ������ ��Ȱ��ȭ
	void DeactivateWindow(const MkHashStr& windowName);

	// ������ Ȱ��ȭ ���� ���
	void ToggleWindow(const MkHashStr& windowName);

	// ����
	void Update(void);

	inline void SetEditMode(bool enable) { m_EditMode = enable; }
	inline bool GetEditMode(void) const { return m_EditMode; }

	// ����
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
