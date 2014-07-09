
#include "MkCore_MkCheck.h"
#include "MkCore_MkInputManager.h"
#include "MkCore_MkTimeManager.h"
#include "MkCore_MkDevPanel.h"

#include "MkS2D_MkTexturePool.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkHiddenEditBox.h"
#include "MkS2D_MkRenderer.h"
#include "MkS2D_MkDrawStep.h"
#include "MkS2D_MkSpreadButtonNode.h"
#include "MkS2D_MkEditModeSettingWindow.h"
#include "MkS2D_MkWindowEventManager.h"


#define MKDEF_ARRAY_EXIST(names, target) (names.FindFirstInclusion(MkArraySection(0), target) != MKDEF_ARRAY_ERROR)
#define MKDEF_ARRAY_ERASE(names, target) names.EraseFirstInclusion(MkArraySection(0), target)

#define MKDEF_WIN_MGR_STEP_NAME L"__#WindowMgr"

const static MkHashStr SCENE_RECT_NAME = L"Scene";
const static MkHashStr DARKEN_RECT_NAME = L"Darken";
const static MkHashStr REGION_RECT_NAME = L"Region";
const static MkHashStr DEBUG_RECT_NAME = L"Debug";

const static MkHashStr SETTING_WINDOW_NAME = L"__#Setting";

//------------------------------------------------------------------------------------------------//

void MkWindowEventManager::SetUp(const MkBaseTexturePtr& sceneTexture)
{
	if (m_RootNode == NULL)
	{
		m_RootNode = new MkSceneNode(L"Root");

		// scene layer
		MkSRect* sceneLayer = m_RootNode->CreateSRect(SCENE_RECT_NAME);
		sceneLayer->SetLocalDepth(m_SceneLayerDepth);
		
		// modal ������ ��¶� ȭ�� ��Ӱ� ����� layer ����
		MkSRect* darkenLayer = m_RootNode->CreateSRect(DARKEN_RECT_NAME);
		if (darkenLayer != NULL)
		{
			darkenLayer->SetTexture(MK_WR_PRESET.GetSystemImageFilePath(), MK_WR_PRESET.GetDarkenLayerSubsetName());
		}

		// ���� ǥ�ÿ� layer ����
		MkSRect* regionLayer = m_RootNode->CreateSRect(REGION_RECT_NAME);
		if (regionLayer != NULL)
		{
			regionLayer->SetTexture(MK_WR_PRESET.GetSystemImageFilePath(), MK_WR_PRESET.GetRegionLayerSubsetName());
			regionLayer->SetVisible(false);
		}

		// ������ layer ����
		MkSRect* debugLayer = m_RootNode->CreateSRect(DEBUG_RECT_NAME);
		if (debugLayer != NULL)
		{
			debugLayer->SetTexture(MK_WR_PRESET.GetSystemImageFilePath(), MK_WR_PRESET.GetRegionLayerSubsetName());
			debugLayer->SetVisible(false);
		}

		m_RootNode->Update();
	}

	if (m_DrawStep == NULL)
	{
		m_DrawStep = MK_RENDERER.GetDrawQueue().GetStep(MKDEF_WIN_MGR_STEP_NAME);
		if (m_DrawStep == NULL)
		{
			m_DrawStep = MK_RENDERER.GetDrawQueue().CreateStep(MKDEF_WIN_MGR_STEP_NAME, MKDEF_WINDOW_MGR_DRAW_STEP_PRIORITY);
			m_DrawStep->AddSceneNode(m_RootNode);
		}

		MkSRect* srect = m_RootNode->GetSRect(DARKEN_RECT_NAME);
		if (srect != NULL)
		{
			srect->SetLocalRect(m_DrawStep->GetRegionRect());
			srect->SetVisible(false);
		}
	}

	if (sceneTexture != NULL)
	{
		MkSRect* sceneLayer = m_RootNode->GetSRect(SCENE_RECT_NAME);
		if (sceneLayer != NULL)
		{
			sceneLayer->SetTexture(sceneTexture);
		}
	}

	// ���� ������ ����
	if (!m_WindowTable.Exist(SETTING_WINDOW_NAME))
	{
		MkEditModeSettingWindow* settingWindow = new MkEditModeSettingWindow(SETTING_WINDOW_NAME);
		if (settingWindow != NULL)
		{
			if (settingWindow->SetUp(MK_WR_PRESET.GetDefaultThemeName()))
			{
				RegisterWindow(settingWindow, false);
			}
			else
			{
				MK_DELETE(settingWindow);
			}
		}

		MK_CHECK(settingWindow != NULL, L"edit mode�� setting window ���� ����") {}
	}
}

bool MkWindowEventManager::RegisterWindow(MkBaseWindowNode* windowNode, bool activate)
{
	MK_CHECK((m_DrawStep != NULL) && (m_RootNode != NULL), L"�ʱ�ȭ���� ���� MkWindowEventManager�� window ��� �õ�")
		return false;
	
	MK_CHECK(windowNode != NULL, L"MkWindowEventManager�� NULL window ��� �õ�")
		return false;

	const MkHashStr& name = windowNode->GetNodeName();
	MK_CHECK(!m_WindowTable.Exist(name), L"MkWindowEventManager�� �̹� �����ϴ� " + name.GetString() + L" window ��� �õ�")
		return false;

	MK_CHECK(windowNode->GetParentNode() == NULL, L"MkWindowEventManager�� �θ� �����ϴ� " + name.GetString() + L" window ��� �õ�")
		return false;

	m_RootNode->AttachChildNode(windowNode);
	windowNode->DeclareRootWindow();
	windowNode->Update(); // �ʱ�ȭ

	m_WindowTable.Create(name, windowNode);

	_LastWindowLostFocus();
	_SetFocusToWindowNode(windowNode);
	m_OnActivatingWindows.PushBack(name);

	if (!activate)
	{
		DeactivateWindow(name);
	}

	return true;
}

void MkWindowEventManager::RemoveWindow(const MkHashStr& windowName)
{
	if (m_WindowTable.Exist(windowName))
	{
		MKDEF_ARRAY_ERASE(m_OnActivatingWindows, windowName);
		MKDEF_ARRAY_ERASE(m_WaitForActivatingWindows, windowName);
		MKDEF_ARRAY_ERASE(m_WaitForDeactivatingWindows, windowName);

		if (windowName == m_LastFocusWindow)
		{
			m_FocusLostByClick = true;
			m_LastFocusWindow.Clear();
			m_CurrentTargetWindowNode = NULL;
			_SetRegionLayerVisible(false);
		}

		if ((m_FrontHitWindow != NULL) && (windowName == m_FrontHitWindow->GetRootWindow()->GetNodeName()))
		{
			m_FrontHitWindow = NULL;
		}

		if (windowName == m_ModalWindow)
		{
			m_ModalWindow.Clear();
			_SetDarkenLayerEnable(false);
		}

		if ((!m_OpeningSpreadButtons.Empty()) && (windowName == m_OpeningSpreadButtons[0]->GetRootWindow()->GetNodeName()))
		{
			m_OpeningSpreadButtons.Clear();
		}

		if ((m_CurrentTargetWindowNode != NULL) && (windowName == m_CurrentTargetWindowNode->GetRootWindow()->GetNodeName()))
		{
			m_CurrentTargetWindowNode = NULL;
		}

		m_RootNode->DetachChildNode(windowName);
		delete m_WindowTable[windowName];
		m_WindowTable.Erase(windowName);
	}
}

bool MkWindowEventManager::IsOnActivation(const MkHashStr& windowName) const
{
	return m_WindowTable.Exist(windowName) ? MKDEF_ARRAY_EXIST(m_OnActivatingWindows, windowName) : false;
}

void MkWindowEventManager::ActivateWindow(const MkHashStr& windowName, bool modal)
{
	if (m_WindowTable.Exist(windowName))
	{
		MKDEF_ARRAY_ERASE(m_WaitForDeactivatingWindows, windowName);

		MKDEF_ARRAY_ERASE(m_WaitForActivatingWindows, windowName); // Ȱ��ȭ ��� ����� ������ �߿�
		m_WaitForActivatingWindows.PushBack(windowName);

		if (modal)
		{
			// modal off -> on
			if (m_ModalWindow.Empty())
			{
				_SetDarkenLayerEnable(true);
			}

			m_ModalWindow = windowName;
		}
	}
}

void MkWindowEventManager::DeactivateWindow(const MkHashStr& windowName)
{
	if (m_WindowTable.Exist(windowName))
	{
		MKDEF_ARRAY_ERASE(m_WaitForActivatingWindows, windowName);

		if (MKDEF_ARRAY_EXIST(m_OnActivatingWindows, windowName) &&
			(!MKDEF_ARRAY_EXIST(m_WaitForDeactivatingWindows, windowName)))
		{
			m_WaitForDeactivatingWindows.PushBack(windowName);
		}
	}
}

void MkWindowEventManager::ToggleWindow(const MkHashStr& windowName, bool modal)
{
	if (m_WindowTable.Exist(windowName))
	{
		if (MKDEF_ARRAY_EXIST(m_OnActivatingWindows, windowName))
		{
			DeactivateWindow(windowName);
		}
		else
		{
			ActivateWindow(windowName, modal);
		}
	}
}

void MkWindowEventManager::Update(void)
{
	if ((m_DrawStep == NULL) || (m_RootNode == NULL))
		return;

	MK_EDIT_BOX.Update();

	// normal mode <-> edit mode toggle
	if (MK_INPUT_MGR.GetKeyPushing(VK_CONTROL) && MK_INPUT_MGR.GetKeyPushing(VK_SHIFT))
	{
		if (MK_INPUT_MGR.GetKeyReleased(MKDEF_S2D_TOGGLE_KEY_BETWEEN_NORMAL_AND_EDIT_MODE))
		{
			m_EditMode = !m_EditMode;

			MK_DEV_PANEL.MsgToLog((m_EditMode) ? L"> Toggle to Edit mode" : L"> Toggle to Normal mode");
		}
	}

	// edit mode�� setting window toggle
	if (m_EditMode && MK_INPUT_MGR.GetKeyReleased(MKDEF_S2D_SETTING_WINDOW_TOGGLE_KEY) && m_WindowTable.Exist(SETTING_WINDOW_NAME))
	{
		if (MKDEF_ARRAY_EXIST(m_OnActivatingWindows, SETTING_WINDOW_NAME))
		{
			DeactivateWindow(SETTING_WINDOW_NAME);
		}
		else
		{
			ActivateWindow(SETTING_WINDOW_NAME, true);
		}
	}

	// current cursor state
	MkInt2 currentCursorPoint = MK_INPUT_MGR.GetAbsoluteMousePosition(true); // flip y
	MkFloat2 currentCursorPosition(static_cast<float>(currentCursorPoint.x), static_cast<float>(currentCursorPoint.y));
	bool currentButtonPressed[3] = { MK_INPUT_MGR.GetMouseLeftButtonPressed(), MK_INPUT_MGR.GetMouseMiddleButtonPressed(), MK_INPUT_MGR.GetMouseRightButtonPressed() };
	bool currentButtonPushing[3] = { MK_INPUT_MGR.GetMouseLeftButtonPushing(), MK_INPUT_MGR.GetMouseMiddleButtonPushing(), MK_INPUT_MGR.GetMouseRightButtonPushing() };
	bool currentButtonReleased[3] = { MK_INPUT_MGR.GetMouseLeftButtonReleased(), MK_INPUT_MGR.GetMouseMiddleButtonReleased(), MK_INPUT_MGR.GetMouseRightButtonReleased() };
	bool cursorAvailable = MK_INPUT_MGR.GetMousePointerAvailable();

	// deactivation
	if (!m_WaitForDeactivatingWindows.Empty())
	{
		MK_INDEXING_LOOP(m_WaitForDeactivatingWindows, i)
		{
			MkBaseWindowNode* windowNode = m_WindowTable[m_WaitForDeactivatingWindows[i]];
			windowNode->Deactivate();
			windowNode->SetVisible(false);

			// ��Ȱ��ȭ ��� �� ��� �����찡 �ִٸ� ��޻��� ����
			if ((!m_ModalWindow.Empty()) && (m_WaitForDeactivatingWindows[i] == m_ModalWindow))
			{
				m_ModalWindow.Clear();
				_SetDarkenLayerEnable(false);
			}
		}

		m_OnActivatingWindows.EraseAllTagInclusions(MkArraySection(0), m_WaitForDeactivatingWindows);
		m_WaitForDeactivatingWindows.Clear();
	}

	// activation
	if (!m_WaitForActivatingWindows.Empty())
	{
		MK_INDEXING_LOOP(m_WaitForActivatingWindows, i)
		{
			if (MKDEF_ARRAY_EXIST(m_OnActivatingWindows, m_WaitForActivatingWindows[i]))
			{
				MKDEF_ARRAY_ERASE(m_OnActivatingWindows, m_WaitForActivatingWindows[i]); // ������ ����
			}
			else
			{
				MkBaseWindowNode* windowNode = m_WindowTable[m_WaitForActivatingWindows[i]];
				windowNode->Activate();
				windowNode->SetVisible(true);
			}
		}

		m_OnActivatingWindows.PushBack(m_WaitForActivatingWindows);
		m_WaitForActivatingWindows.Clear();

		// Ȱ��ȭ ��� �� ��� �����찡 ������ �����찡 �ƴ϶�� ������ �������� ��ġ�ϰ� ��(focusing)
		if ((!m_ModalWindow.Empty()) && (m_OnActivatingWindows[m_OnActivatingWindows.GetSize() - 1] != m_ModalWindow))
		{
			MKDEF_ARRAY_ERASE(m_OnActivatingWindows, m_ModalWindow);
			m_OnActivatingWindows.PushBack(m_ModalWindow);
		}

		m_FocusLostByClick = false;
	}

	// Ȱ��ȭ�� �����찡 ������
	if (!m_OnActivatingWindows.Empty())
	{
		unsigned int onFocusIndex = m_OnActivatingWindows.GetSize() - 1;

		// ���� �������� spread button�� �ִٸ� item list world AABR ����
		if (!m_OpeningSpreadButtons.Empty())
		{
			MK_INDEXING_LOOP(m_OpeningSpreadButtons, i)
			{
				m_OpeningSpreadButtons[i]->__UpdateItemRegion();
			}

			//__ShowDebugLayer(m_OpeningSpreadButtons[m_OpeningSpreadButtons.GetSize() - 1]->GetItemWorldAABR(), 1.f);

			// Ŭ�� �̺�Ʈ�� �߻��ߴٸ� spread button ���� �ȿ� �����ϴ��� Ȯ��
			if (currentButtonPressed[0] || currentButtonPressed[1] || currentButtonPressed[2])
			{
				bool hit = false;
				if (m_OpeningSpreadButtons[0]->GetWindowRect().CheckGridIntersection(currentCursorPosition))
				{
					hit = true;
				}
				else
				{
					MK_INDEXING_LOOP(m_OpeningSpreadButtons, i)
					{
						if (m_OpeningSpreadButtons[i]->GetItemWorldAABR().CheckGridIntersection(currentCursorPosition))
						{
							hit = true;
							break;
						}
					}
				}

				if (!hit) // ���� �� Ŭ���̹Ƿ� ��� ����
				{
					MkSpreadButtonNode* oldBtn = m_OpeningSpreadButtons[0];
					oldBtn->CloseAllItems();

					//__HideDebugLayer();
				}
			}
		}

		// ��� �������� �ƴϸ� click event�� ���� ���� ��ȭ �ݿ�
		if (m_ModalWindow.Empty())
		{
			if (currentButtonPressed[0] || currentButtonPressed[1] || currentButtonPressed[2])
			{
				// Ŭ�� �̺�Ʈ�� �߻����� ��� �ں��� ������ ��ȸ�ϸ� focus �����츦 ã�´�
				bool foundFocusWindow = false;
				for (unsigned int i=onFocusIndex; i!=0xffffffff; --i)
				{
					MkBaseWindowNode* windowNode = m_WindowTable[m_OnActivatingWindows[i]];
					if (windowNode->GetVisible() && windowNode->GetWorldAABR().CheckIntersection(currentCursorPosition))
					{
						if (i < onFocusIndex)
						{
							m_OnActivatingWindows.Erase(MkArraySection(i, 1));
							m_OnActivatingWindows.PushBack(windowNode->GetNodeName());
						}

						foundFocusWindow = true;
						break;
					}
				}

				// �ƹ��� �����쵵 ���õ��� �ʾҸ� focus ��ȿȭ
				m_FocusLostByClick = !foundFocusWindow;
				if (m_FocusLostByClick)
				{
					_LastWindowLostFocus();
					m_LastFocusWindow.Clear();

					m_CurrentTargetWindowNode = NULL;
					_SetRegionLayerVisible(false);
				}
			}
		}

		// ������ ���� ���� ���ġ
		m_WindowTable[m_OnActivatingWindows[onFocusIndex]]->SetLocalDepth(m_MinDepthBandwidth);
		if (m_OnActivatingWindows.GetSize() > 1)
		{
			float windowDepthUnit = (m_MaxDepthBandwidth - m_MinDepthBandwidth) / static_cast<float>(onFocusIndex);
			float offset = windowDepthUnit;

			for (unsigned int i=onFocusIndex-1; i!=0xffffffff; --i)
			{
				m_WindowTable[m_OnActivatingWindows[i]]->SetLocalDepth(m_MinDepthBandwidth + offset);
				offset += windowDepthUnit;
			}
		}

		// focus window ����. ��ȿ click�� �����ٸ� Ȱ��ȭ���� ������ �� ���� �������� ��ġ�� ��ȿ �����찡 focus�� ������
		MkBaseWindowNode* onFocusWindowNode = NULL;
		MkBaseWindowNode* nextWindowNode = NULL; // focus window �ٷ� ���� window
		if (!m_FocusLostByClick)
		{
			for (unsigned int i=onFocusIndex; i!=0xffffffff; --i)
			{
				const MkHashStr& currWindowName = m_OnActivatingWindows[i];
				MkBaseWindowNode* windowNode = m_WindowTable[currWindowName];
				if (windowNode->GetVisible())
				{
					if (onFocusWindowNode == NULL)
					{
						if (currWindowName != m_LastFocusWindow)
						{
							_LastWindowLostFocus();
							_SetFocusToWindowNode(windowNode);
						}

						onFocusWindowNode = windowNode;
					}
					else
					{
						nextWindowNode = windowNode;
						break;
					}
				}
			}
		}

		// ���� �������� spread button�� �ִµ� ��Ŀ���� �Ҿ��ٸ� ����
		if (!m_OpeningSpreadButtons.Empty())
		{
			if (onFocusWindowNode != m_OpeningSpreadButtons[0]->GetRootWindow())
			{
				MkSpreadButtonNode* oldBtn = m_OpeningSpreadButtons[0];
				oldBtn->CloseAllItems();
			}
		}

		// ��� ���¸� darken layer ���� ����
		if (!m_ModalWindow.Empty())
		{
			float darkenLayerDepth = m_MaxDepthBandwidth;
			if (nextWindowNode != NULL)
			{
				darkenLayerDepth = (onFocusWindowNode->GetLocalDepth() + nextWindowNode->GetLocalDepth()) * 0.5f;
			}

			MkSRect* srect = m_RootNode->GetSRect(DARKEN_RECT_NAME);
			if (srect != NULL)
			{
				srect->SetLocalDepth(darkenLayerDepth);
			}
		}

		// input event : enable �����϶��� ����
		MkArray<MkInputManager::InputEvent> inputEventList;
		if (MK_INPUT_MGR.GetInputEvent(inputEventList) > 0)
		{
			int inputClientY = MK_INPUT_MGR.GetTargetWindowClientSize().y;
			MK_INDEXING_LOOP(inputEventList, i)
			{
				const MkInputManager::InputEvent& evt = inputEventList[i];
				MkFloat2 cursorPosition(static_cast<float>(evt.arg1), static_cast<float>(inputClientY - evt.arg2));

				// eMouseMove �̺�Ʈ�� ��� ���¸� focus window��, �ƴϸ� ��ü ��ȸ
				if (evt.eventType == MkInputManager::eMouseMove)
				{
					bool inside = (evt.arg0 == 1);
					if (m_ModalWindow.Empty())
					{
						for (unsigned int i=onFocusIndex; i!=0xffffffff; --i)
						{
							MkBaseWindowNode* windowNode = m_WindowTable[m_OnActivatingWindows[i]];
							if (windowNode->GetVisible() && windowNode->GetEnable() && (!windowNode->GetAttribute(MkBaseWindowNode::eIgnoreInputEvent)))
							{
								windowNode->InputEventMouseMove(inside, currentButtonPushing, cursorPosition);
							}
						}
					}
					else if (onFocusWindowNode->GetEnable() && (!onFocusWindowNode->GetAttribute(MkBaseWindowNode::eIgnoreInputEvent)))
					{
						onFocusWindowNode->InputEventMouseMove(inside, currentButtonPushing, cursorPosition);
					}
				}
				// �� �� �̺�Ʈ�� focus window���� ����
				else if ((onFocusWindowNode != NULL) && onFocusWindowNode->GetEnable() && (!onFocusWindowNode->GetAttribute(MkBaseWindowNode::eIgnoreInputEvent)))
				{
					switch (evt.eventType)
					{
					case MkInputManager::eKeyPress: onFocusWindowNode->InputEventKeyPress(evt.arg0); break;
					case MkInputManager::eKeyRelease: onFocusWindowNode->InputEventKeyRelease(evt.arg0); break;
					case MkInputManager::eMousePress: onFocusWindowNode->InputEventMousePress(evt.arg0, cursorPosition); break;
					case MkInputManager::eMouseRelease: onFocusWindowNode->InputEventMouseRelease(evt.arg0, cursorPosition); break;
					case MkInputManager::eMouseDoubleClick: onFocusWindowNode->InputEventMouseDoubleClick(evt.arg0, cursorPosition); break;
					case MkInputManager::eMouseWheelMove: onFocusWindowNode->InputEventMouseWheelMove(evt.arg0, cursorPosition); break;
					}
				}
			}
		}

		if (onFocusWindowNode != NULL)
		{
			onFocusWindowNode->__ConsumeWindowEvent();
		}

		if (cursorAvailable && (onFocusWindowNode != NULL))
		{
			// Ÿ�� ������, �巡�� ������ ����
			if (currentButtonPressed[0] && onFocusWindowNode->GetWorldAABR().CheckIntersection(currentCursorPosition))
			{
				MkBaseWindowNode* frontHit = onFocusWindowNode->__GetFrontHitWindow(currentCursorPosition);
				if (m_AllowDragMovement && (frontHit != NULL)) // �̵� ����϶��� slide bar�� Ÿ���� �� �� �ְ� ��ȯ
				{
					switch (frontHit->GetPresetComponentType())
					{
					case eS2D_WPC_VSlideBar:
					case eS2D_WPC_HSlideBar:
						frontHit = dynamic_cast<MkBaseWindowNode*>(frontHit->GetParentNode());
						break;
					case eS2D_WPC_VSlideButton:
					case eS2D_WPC_HSlideButton:
						frontHit = dynamic_cast<MkBaseWindowNode*>(frontHit->GetParentNode()->GetParentNode());
						break;
					}
				}

				if (frontHit != NULL)
				{
					// edit box ������ ���õǾ����� �˸�. �ٸ� ������ ������� �Է¸�� ���
					MK_EDIT_BOX.BindControl((frontHit->GetNodeType() == eS2D_SNT_EditBoxNode) ? frontHit : NULL);

					if (m_EditMode && (onFocusWindowNode->GetNodeName() != SETTING_WINDOW_NAME))
					{
						m_CurrentTargetWindowNode = frontHit;

						_ShowRegionLayer();
					}
					
					if (m_FrontHitWindow == NULL)
					{
						m_FrontHitWindow = frontHit;
						m_CursorStartPosition = currentCursorPosition;
						m_WindowAABRBegin = m_FrontHitWindow->GetWorldAABR().position;
						m_WindowOffsetToWorldPos = MkFloat2(m_FrontHitWindow->GetWorldPosition().x, m_FrontHitWindow->GetWorldPosition().y) - m_WindowAABRBegin;

						// �ڵ鸵 ����
						if (m_FrontHitWindow->GetAttribute(MkBaseWindowNode::eDragToHandling))
						{
							
						}
					}
				}
			}

			if (m_FrontHitWindow != NULL)
			{
				// �ڵ鸵 �̵�
				if (m_FrontHitWindow->GetAttribute(MkBaseWindowNode::eDragToHandling))
				{
				}
				// �̵����� �Ӽ��� ���� ���¿��� ����Ʈ ���ų� �巡�� �����찡 �̵� �Ӽ��� ������ ���� ��� ������ �̵�
				else if ((!m_FrontHitWindow->GetAttribute(MkBaseWindowNode::eIgnoreMovement)) &&
					((m_EditMode && m_AllowDragMovement) || m_FrontHitWindow->GetAttribute(MkBaseWindowNode::eDragMovement)))
				{
					MkFloat2 offset = currentCursorPosition - m_CursorStartPosition;
					MkFloat2 newPosition = _ConfineMovement(m_FrontHitWindow, m_WindowAABRBegin, m_WindowOffsetToWorldPos, offset);
					m_FrontHitWindow->SetLocalAsWorldPosition(newPosition, false);
				}
			}
		}

		// ��Ŀ������ �����찡 ���ų� Ȱ��ȭ���� edit box �׷��� �ƴ� �ٸ� �����찡 ��Ŀ���� ������ ������ �Է¸�� ���
		if ((onFocusWindowNode == NULL) || (onFocusWindowNode->GetNodeName() != MK_EDIT_BOX.GetRootNodeNameOfBindingControl()))
		{
			MK_EDIT_BOX.BindControl(NULL);
		}
	}
	else
	{
		MK_EDIT_BOX.BindControl(NULL); // Ȱ��ȭ���� �����찡 ��� �Է¸�� ���
	}

	if ((!cursorAvailable) || currentButtonReleased[0])
	{
		m_FrontHitWindow = NULL;
	}

	if (m_EditMode)
	{
		if (m_CurrentTargetWindowNode != NULL)
		{
			// movement by arrow key
			if (!m_CurrentTargetWindowNode->GetAttribute(MkBaseWindowNode::eIgnoreMovement))
			{
				MkFloat2 offset;
				if (MK_INPUT_MGR.GetKeyReleased(VK_LEFT)) offset.x = -1.f;
				if (MK_INPUT_MGR.GetKeyReleased(VK_RIGHT)) offset.x = 1.f;
				if (MK_INPUT_MGR.GetKeyReleased(VK_UP)) offset.y = 1.f;
				if (MK_INPUT_MGR.GetKeyReleased(VK_DOWN)) offset.y = -1.f;

				if (!offset.IsZero())
				{
					const MkFloat2& worldAABRBegin = m_CurrentTargetWindowNode->GetWorldAABR().position;
					MkFloat2 oldPosition = MkFloat2(m_CurrentTargetWindowNode->GetWorldPosition().x, m_CurrentTargetWindowNode->GetWorldPosition().y);
					MkFloat2 newPosition = _ConfineMovement(m_CurrentTargetWindowNode, worldAABRBegin, oldPosition - worldAABRBegin, offset);
					m_CurrentTargetWindowNode->SetLocalAsWorldPosition(newPosition, false);
				}
			}

			_UpdateRegionLayer(m_CurrentTargetWindowNode->GetWindowRect(), m_CurrentTargetWindowNode->GetWorldPosition().z - MKDEF_BASE_WINDOW_DEPTH_GRID * 0.5f);
		}
	}

	// scene graph update
	m_RootNode->Update();

	// debug
	{
		MK_DEV_PANEL.__MsgToDrawingBoard(5, (m_EditMode) ? L"[Edit mode]" : L"[Normal mode]");

		MkStr debugMsg(512);
		debugMsg = L"[Mouse] : ";
		debugMsg += currentCursorPoint;
		debugMsg += L", L(";
		debugMsg += currentButtonPushing[0];
		debugMsg += L") M(";
		debugMsg += currentButtonPushing[1];
		debugMsg += L") R(";
		debugMsg += currentButtonPushing[2];
		debugMsg += L")";
		MK_DEV_PANEL.__MsgToDrawingBoard(6, debugMsg);
	}

	if (m_CurrentTargetWindowNode != NULL)
	{
		MkStr debugMsg(512);
		const MkFloatRect& winRect = m_CurrentTargetWindowNode->GetWindowRect();
		debugMsg = L"[";
		debugMsg += m_CurrentTargetWindowNode->GetNodeName().GetString();
		debugMsg += L"] WR ";
		debugMsg += MkInt2(static_cast<int>(winRect.position.x), static_cast<int>(winRect.position.y));
		debugMsg += L", ";
		debugMsg += MkInt2(static_cast<int>(winRect.size.x), static_cast<int>(winRect.size.y));
		if (winRect.CheckGridIntersection(currentCursorPosition))
		{
			debugMsg += L" -> on cursor";
		}
		MK_DEV_PANEL.__MsgToDrawingBoard(7, debugMsg);

		if (!m_CurrentTargetWindowNode->GetPresetComponentName().Empty())
		{
			debugMsg = L"   >> Component : ";
			debugMsg += m_CurrentTargetWindowNode->GetPresetComponentName().GetString();

			MkArray<MkHashStr> keys;
			m_CurrentTargetWindowNode->GetChildNodeList(keys);
			MK_INDEXING_LOOP(keys, i)
			{
				if (m_CurrentTargetWindowNode->GetChildNode(keys[i])->GetVisible())
				{
					debugMsg += L" >> ";
					debugMsg += keys[i];
					break;
				}
			}
			MK_DEV_PANEL.__MsgToDrawingBoard(8, debugMsg);
		}
	}
}

const MkFloatRect& MkWindowEventManager::GetRegionRect(void) const
{
	const static MkFloatRect nullRect;
	return (m_DrawStep == NULL) ? nullRect : m_DrawStep->GetRegionRect();
}

void MkWindowEventManager::Clear(void)
{
	m_DrawStep = NULL;
	MK_DELETE(m_RootNode);
	m_WindowTable.Clear();
	m_OnActivatingWindows.Clear();
	m_WaitForActivatingWindows.Clear();
	m_WaitForDeactivatingWindows.Clear();
	m_LastFocusWindow.Clear();
	m_FocusLostByClick = false;
	m_FrontHitWindow = NULL;
	m_ModalWindow.Clear();
	m_OpeningSpreadButtons.Clear();
	m_CurrentTargetWindowNode = NULL;
}

MkWindowEventManager::MkWindowEventManager() : MkSingletonPattern<MkWindowEventManager>()
{
	m_MinDepthBandwidth = MKDEF_S2D_MAX_WORLD_DEPTH * 0.1f;
	m_MaxDepthBandwidth = MKDEF_S2D_MAX_WORLD_DEPTH * 0.9f;
	m_SceneLayerDepth = MKDEF_S2D_MAX_WORLD_DEPTH * 0.95f;

	m_DrawStep = NULL;
	m_RootNode = NULL;
	m_EditMode = true;
	m_FocusLostByClick = false;
	m_FrontHitWindow = NULL;
	m_CurrentTargetWindowNode = NULL;

	m_ShowWindowSelection = false;
	m_AllowDragMovement = true;
}

void MkWindowEventManager::__SpreadButtonOpened(int index, MkSpreadButtonNode* button)
{
	if ((index >= 0) && (button != NULL))
	{
		unsigned int pos = static_cast<unsigned int>(index);
		int oldSize = m_OpeningSpreadButtons.GetSize();

		if (m_OpeningSpreadButtons.IsValidIndex(pos))
		{
			MkSpreadButtonNode* oldBtn = m_OpeningSpreadButtons[pos];
			oldBtn->CloseAllItems();
		}
		else
		{
			MK_CHECK(pos == oldSize, L"�߸��� spread button ���� ����")
				return;
		}
		m_OpeningSpreadButtons.PushBack(button);
	}
}

void MkWindowEventManager::__SpreadButtonClosed(int index)
{
	if (index >= 0)
	{
		MK_CHECK((static_cast<unsigned int>(index) + 1) == m_OpeningSpreadButtons.GetSize(), L"�߸��� spread button ���� ����")
			return;

		m_OpeningSpreadButtons.PopBack();
	}
}

void MkWindowEventManager::__ShowDebugLayer(const MkFloatRect& rect, float depth)
{
	if ((m_RootNode != NULL) && rect.SizeIsNotZero())
	{
		MkSRect* srect = m_RootNode->GetSRect(DEBUG_RECT_NAME);
		if (srect != NULL)
		{
			srect->SetVisible(true);
			srect->SetLocalRect(rect);
			srect->SetLocalDepth(depth);
		}
	}
}

void MkWindowEventManager::__HideDebugLayer(void)
{
	if (m_RootNode != NULL)
	{
		MkSRect* srect = m_RootNode->GetSRect(DEBUG_RECT_NAME);
		if (srect != NULL)
		{
			srect->SetVisible(false);
		}
	}
}

void MkWindowEventManager::_LastWindowLostFocus(void)
{
	if (!m_LastFocusWindow.Empty())
	{
		m_WindowTable[m_LastFocusWindow]->LostFocus();
	}
}

void MkWindowEventManager::_SetFocusToWindowNode(MkBaseWindowNode* targetNode)
{
	targetNode->OnFocus();
	m_LastFocusWindow = targetNode->GetNodeName();
}

void MkWindowEventManager::_SetDarkenLayerEnable(bool enable)
{
	if (m_RootNode != NULL)
	{
		MkSRect* srect = m_RootNode->GetSRect(DARKEN_RECT_NAME);
		if (srect != NULL)
		{
			srect->SetVisible(enable);
		}
	}
}

MkFloat2 MkWindowEventManager::_ConfineMovement(MkBaseWindowNode* targetNode, const MkFloat2& posBegin, const MkFloat2& toWorld, const MkFloat2& offset) const
{
	if (targetNode->GetAttribute(MkBaseWindowNode::eConfinedToScreen))
	{
		return (m_DrawStep->GetRegionRect().Confine(MkFloatRect(posBegin + offset, targetNode->GetWorldAABR().size)) + toWorld);
	}
	else if (targetNode->GetAttribute(MkBaseWindowNode::eConfinedToParent))
	{
		MkBaseWindowNode* anchorNode = targetNode->GetAncestorWindowNode();
		const MkFloatRect& anchorRect = (anchorNode == NULL) ? m_DrawStep->GetRegionRect() : anchorNode->GetWorldAABR();
		return (anchorRect.Confine(MkFloatRect(posBegin + offset, targetNode->GetWorldAABR().size)) + toWorld);
	}
	return (posBegin + offset + toWorld);
}

void MkWindowEventManager::_ShowRegionLayer(void)
{
	if (m_ShowWindowSelection && _SetRegionLayerVisible(true))
	{
		MkTimeState timeState;
		MK_TIME_MGR.GetCurrentTimeState(timeState);
		m_RegionLayerCounter.SetUp(timeState, MKDEF_WINDOW_MGR_REGION_LAYER_LIFETIME);
	}
}

void MkWindowEventManager::_UpdateRegionLayer(const MkFloatRect& rect, float depth)
{
	if ((m_RootNode != NULL) && rect.SizeIsNotZero())
	{
		MkSRect* srect = m_RootNode->GetSRect(REGION_RECT_NAME);
		if ((srect != NULL) && (srect->GetVisible()))
		{
			MkTimeState timeState;
			MK_TIME_MGR.GetCurrentTimeState(timeState);

			if (m_RegionLayerCounter.OnTick(timeState))
			{
				srect->SetVisible(false);
			}
			else
			{
				srect->SetLocalRect(rect);
				srect->SetLocalDepth(depth);
				srect->SetObjectAlpha(1.f - m_RegionLayerCounter.GetTickRatio(timeState));
			}
		}
	}
}

bool MkWindowEventManager::_SetRegionLayerVisible(bool enable)
{
	if (m_RootNode != NULL)
	{
		MkSRect* srect = m_RootNode->GetSRect(REGION_RECT_NAME);
		if (srect != NULL)
		{
			srect->SetVisible(enable);
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------//
