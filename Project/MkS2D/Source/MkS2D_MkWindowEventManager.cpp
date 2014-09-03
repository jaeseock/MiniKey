
#include "MkCore_MkCheck.h"
#include "MkCore_MkInputManager.h"
#include "MkCore_MkTimeManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkProfilingManager.h"

#include "MkS2D_MkTexturePool.h"
#include "MkS2D_MkCursorManager.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkHiddenEditBox.h"
#include "MkS2D_MkRenderer.h"
#include "MkS2D_MkDrawStep.h"
#include "MkS2D_MkSpreadButtonNode.h"
#include "MkS2D_MkEditModeSettingWindow.h"
#include "MkS2D_MkEditModeTargetWindow.h"
#include "MkS2D_MkSystemWindows.h"
#include "MkS2D_MkWindowEventManager.h"


#define MKDEF_ARRAY_EXIST(names, target) (names.FindFirstInclusion(MkArraySection(0), target) != MKDEF_ARRAY_ERROR)
#define MKDEF_ARRAY_ERASE(names, target) names.EraseFirstInclusion(MkArraySection(0), target)

#define MKDEF_WIN_MGR_STEP_NAME L"__#WindowMgr"

const static MkHashStr SCENE_RECT_NAME = L"Scene";
const static MkHashStr DARKEN_RECT_NAME = L"Darken";
const static MkHashStr REGION_RECT_NAME = L"Region";
const static MkHashStr DEBUG_RECT_NAME = L"Debug";

const static MkHashStr SETTING_WINDOW_NAME = L"__#Setting";
const static MkHashStr TARGET_WINDOW_NAME = L"__#Target";
const static MkHashStr SW_NODENAMEINPUT_WINDOW_NAME = L"__#NNI";
const static MkHashStr SW_WIN_ATTR_WINDOW_NAME = L"__#WinAttr";
const static MkHashStr SW_SRECT_SETTER_WINDOW_NAME = L"__#SRectSetter";

static const MkHashStr PROF_KEY_UPDATE = L"MkWindowEventManager::Update";

//------------------------------------------------------------------------------------------------//

void MkWindowEventManager::SetUp(const MkBaseTexturePtr& sceneTexture)
{
	if (m_RootNode == NULL)
	{
		m_RootNode = new MkSceneNode(L"Root");

		// scene layer
		MkSRect* sceneLayer = m_RootNode->CreateSRect(SCENE_RECT_NAME);
		sceneLayer->SetLocalDepth(m_SceneLayerDepth);
		
		// modal 윈도우 출력때 화면 어둡게 만드는 layer 생성
		MkSRect* darkenLayer = m_RootNode->CreateSRect(DARKEN_RECT_NAME);
		if (darkenLayer != NULL)
		{
			darkenLayer->SetTexture(MK_WR_PRESET.GetSystemImageFilePath(), MK_WR_PRESET.GetDarkenLayerSubsetName());
		}

		// 영역 표시용 layer 생성
		MkSRect* regionLayer = m_RootNode->CreateSRect(REGION_RECT_NAME);
		if (regionLayer != NULL)
		{
			regionLayer->SetTexture(MK_WR_PRESET.GetSystemImageFilePath(), MK_WR_PRESET.GetRegionLayerSubsetName());
			regionLayer->SetVisible(false);
		}

		// 디버깅용 layer 생성
		MkSRect* debugLayer = m_RootNode->CreateSRect(DEBUG_RECT_NAME);
		if (debugLayer != NULL)
		{
			debugLayer->SetTexture(MK_WR_PRESET.GetSystemImageFilePath(), MK_WR_PRESET.GetRegionLayerSubsetName());
			debugLayer->SetVisible(false);
		}

		m_RootNode->UpdateAll();
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

	// 설정 윈도우 생성
	if (!m_WindowTable.Exist(SETTING_WINDOW_NAME))
	{
		_CreateSystemWindow(new MkEditModeSettingWindow(SETTING_WINDOW_NAME));
	}

	// 노드 선택 윈도우 생성
	if (!m_WindowTable.Exist(TARGET_WINDOW_NAME))
	{
		_CreateSystemWindow(new MkEditModeTargetWindow(TARGET_WINDOW_NAME));
	}

	// 노드 이름 입력 윈도우 생성
	if (!m_WindowTable.Exist(SW_NODENAMEINPUT_WINDOW_NAME))
	{
		_CreateSystemWindow(new MkNodeNameInputSystemWindow(SW_NODENAMEINPUT_WINDOW_NAME));
	}

	// 윈도우 attribute 입력 윈도우 생성
	if (!m_WindowTable.Exist(SW_WIN_ATTR_WINDOW_NAME))
	{
		_CreateSystemWindow(new MkWindowAttributeSystemWindow(SW_WIN_ATTR_WINDOW_NAME));
	}

	// SRect setter 윈도우 생성
	if (!m_WindowTable.Exist(SW_SRECT_SETTER_WINDOW_NAME))
	{
		_CreateSystemWindow(new MkSRectSetterSystemWindow(SW_SRECT_SETTER_WINDOW_NAME));
	}
}

bool MkWindowEventManager::RegisterWindow(MkBaseWindowNode* windowNode, bool activate)
{
	MK_CHECK((m_DrawStep != NULL) && (m_RootNode != NULL), L"초기화되지 않은 MkWindowEventManager에 window 등록 시도")
		return false;
	
	MK_CHECK(windowNode != NULL, L"MkWindowEventManager에 NULL window 등록 시도")
		return false;

	const MkHashStr& name = windowNode->GetNodeName();
	MK_CHECK(!m_WindowTable.Exist(name), L"MkWindowEventManager에 이미 존재하는 " + name.GetString() + L" window 등록 시도")
		return false;

	MK_CHECK(windowNode->GetParentNode() == NULL, L"MkWindowEventManager에 부모가 존재하는 " + name.GetString() + L" window 등록 시도")
		return false;

	m_RootNode->AttachChildNode(windowNode);
	windowNode->DeclareRootWindow();
	
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
			SetTargetWindowNode(NULL);
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

		MKDEF_ARRAY_ERASE(m_WaitForActivatingWindows, windowName); // 활성화 대기 목록은 순서가 중요
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

	MkFocusProfiler focusProfiler(PROF_KEY_UPDATE);

	// normal mode <-> edit mode toggle
	if (MK_INPUT_MGR.GetKeyPushing(VK_CONTROL) && MK_INPUT_MGR.GetKeyPushing(VK_SHIFT))
	{
		if (MK_INPUT_MGR.GetKeyReleased(MKDEF_S2D_TOGGLE_KEY_BETWEEN_NORMAL_AND_EDIT_MODE))
		{
			m_EditMode = !m_EditMode;

			if (!m_EditMode)
			{
				DeactivateWindow(SETTING_WINDOW_NAME);
				DeactivateWindow(TARGET_WINDOW_NAME);
				DeactivateWindow(SW_NODENAMEINPUT_WINDOW_NAME);
				DeactivateWindow(SW_WIN_ATTR_WINDOW_NAME);
				DeactivateWindow(SW_SRECT_SETTER_WINDOW_NAME);

				__HideDebugLayer();
				_SetRegionLayerVisible(false);
			}

			MK_DEV_PANEL.MsgToLog((m_EditMode) ? L"> Toggle to Edit mode" : L"> Toggle to Normal mode");
		}
	}

	if (m_EditMode)
	{
		// setting window toggle
		if (MK_INPUT_MGR.GetKeyReleased(MKDEF_S2D_SETTING_WINDOW_TOGGLE_KEY) &&
			(m_ModalWindow.Empty() || (m_ModalWindow == SETTING_WINDOW_NAME)) && m_WindowTable.Exist(SETTING_WINDOW_NAME))
		{
			if (IsOnActivation(SETTING_WINDOW_NAME))
			{
				DeactivateWindow(SETTING_WINDOW_NAME);
			}
			else
			{
				ActivateWindow(SETTING_WINDOW_NAME, true);
			}
		}

		if (MK_INPUT_MGR.GetKeyReleased(MKDEF_S2D_NODE_SEL_WINDOW_TOGGLE_KEY) && m_ModalWindow.Empty() && m_WindowTable.Exist(TARGET_WINDOW_NAME))
		{
			if (IsOnActivation(TARGET_WINDOW_NAME))
			{
				DeactivateWindow(TARGET_WINDOW_NAME);
			}
			else
			{
				_PassTargetWindow(true);
				ActivateWindow(TARGET_WINDOW_NAME, false);
			}
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

			// 비활성화 목록 중 모달 윈도우가 있다면 모달상태 해제
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
				MKDEF_ARRAY_ERASE(m_OnActivatingWindows, m_WaitForActivatingWindows[i]); // 순서만 변동
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

		// 활성화 목록 중 모달 윈도우가 마지막 윈도우가 아니라면 강제로 마지막에 위치하게 함(focusing)
		if ((!m_ModalWindow.Empty()) && (m_OnActivatingWindows[m_OnActivatingWindows.GetSize() - 1] != m_ModalWindow))
		{
			MKDEF_ARRAY_ERASE(m_OnActivatingWindows, m_ModalWindow);
			m_OnActivatingWindows.PushBack(m_ModalWindow);
		}

		m_FocusLostByClick = false;
	}

	// 활성화된 윈도우가 있으면
	if (!m_OnActivatingWindows.Empty())
	{
		unsigned int onFocusIndex = m_OnActivatingWindows.GetSize() - 1;

		// 현재 전개중인 spread button이 있다면 item list world AABR 갱신
		if (!m_OpeningSpreadButtons.Empty())
		{
			MK_INDEXING_LOOP(m_OpeningSpreadButtons, i)
			{
				m_OpeningSpreadButtons[i]->__UpdateItemRegion();
			}

			//__ShowDebugLayer(m_OpeningSpreadButtons[m_OpeningSpreadButtons.GetSize() - 1]->GetItemWorldAABR(), 1.f);

			// 클릭 이벤트가 발생했다면 spread button 영역 안에 존재하는지 확인
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

				if (!hit) // 영역 외 클릭이므로 모두 닫음
				{
					MkSpreadButtonNode* oldBtn = m_OpeningSpreadButtons[0];
					oldBtn->CloseAllItems();

					//__HideDebugLayer();
				}
			}
		}

		// 모달 상태중이 아니면 click event로 인한 순서 변화 반영
		if (m_ModalWindow.Empty())
		{
			if (currentButtonPressed[0] || currentButtonPressed[1] || currentButtonPressed[2])
			{
				// 클릭 이벤트가 발생했을 경우 뒤부터 앞으로 순회하며 focus 윈도우를 찾는다
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

				// 아무런 윈도우도 선택되지 않았면 focus 무효화
				m_FocusLostByClick = !foundFocusWindow;
				if (m_FocusLostByClick)
				{
					_LastWindowLostFocus();
					m_LastFocusWindow.Clear();

					SetTargetWindowNode(NULL);
				}
			}
		}

		// 순서에 따른 깊이 재배치
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

		// focus window 선택. 무효 click이 없었다면 활성화중인 윈도우 중 가장 마지막에 위치한 유효 윈도우가 focus를 가진다
		MkBaseWindowNode* onFocusWindowNode = NULL;
		MkBaseWindowNode* nextWindowNode = NULL; // focus window 바로 뒤의 window
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

		// 현재 전개중인 spread button이 있는데 포커스를 잃었다면 닫음
		if (!m_OpeningSpreadButtons.Empty())
		{
			if (onFocusWindowNode != m_OpeningSpreadButtons[0]->GetRootWindow())
			{
				MkSpreadButtonNode* oldBtn = m_OpeningSpreadButtons[0];
				oldBtn->CloseAllItems();
			}
		}

		// 모달 상태면
		if (!m_ModalWindow.Empty())
		{
			// darken layer 깊이 조정
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

			// 커서가 윈도우 안에 없으면
			if ((onFocusWindowNode != NULL) && (!onFocusWindowNode->GetWorldAABR().CheckGridIntersection(currentCursorPosition)))
			{
				MK_CURSOR_MGR.SetDisableCursor();
			}
		}

		// input event : enable 상태일때만 전달
		MkArray<MkInputManager::InputEvent> inputEventList;
		if (MK_INPUT_MGR.GetInputEvent(inputEventList) > 0)
		{
			int inputClientY = MK_INPUT_MGR.GetTargetWindowClientSize().y;
			MK_INDEXING_LOOP(inputEventList, i)
			{
				const MkInputManager::InputEvent& evt = inputEventList[i];
				MkFloat2 cursorPosition(static_cast<float>(evt.arg1), static_cast<float>(inputClientY - evt.arg2));

				// eMouseMove 이벤트는 모달 상태면 focus window만, 아니면 전체 순회
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
				// 그 외 이벤트는 focus window에만 적용
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

		if (cursorAvailable && (onFocusWindowNode != NULL))
		{
			// 타겟 윈도우, 드래그 윈도우 선택
			if (currentButtonPressed[0] && onFocusWindowNode->GetWorldAABR().CheckIntersection(currentCursorPosition))
			{
				MkBaseWindowNode* frontHit = onFocusWindowNode->__GetFrontHitWindow(currentCursorPosition);

				// 이동 모드일때만 slide bar를 타게팅 할 수 있게 변환
				if (m_EditMode && m_AllowDragMovement && (frontHit != NULL) && (!onFocusWindowNode->GetAttribute(MkBaseWindowNode::eSystemWindow)))
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
					// edit box 영역이 선택되었으면 알림. 다른 종류의 윈도우면 입력모드 취소
					MK_EDIT_BOX.BindControl((frontHit->GetNodeType() == eS2D_SNT_EditBoxNode) ? frontHit : NULL);

					if (m_EditMode && (!onFocusWindowNode->GetAttribute(MkBaseWindowNode::eSystemWindow)))
					{
						SetTargetWindowNode(frontHit);
					}
					
					if (m_FrontHitWindow == NULL)
					{
						m_FrontHitWindow = frontHit;
						m_CursorStartPosition = currentCursorPosition;
						m_WindowAABRBegin = m_FrontHitWindow->GetWorldAABR().position;
						m_WindowOffsetToWorldPos = MkFloat2(m_FrontHitWindow->GetWorldPosition().x, m_FrontHitWindow->GetWorldPosition().y) - m_WindowAABRBegin;

						m_FrontHitWindow->StartDragMovement(m_FrontHitWindow);

						// 핸들링 시작
						if (m_FrontHitWindow->GetAttribute(MkBaseWindowNode::eAllowItemDrag))
						{
							
						}
					}
				}
			}

			if (m_FrontHitWindow != NULL)
			{
				// 핸들링 이동
				if (m_FrontHitWindow->GetAttribute(MkBaseWindowNode::eAllowItemDrag))
				{
				}
				// 이동금지 속성이 없는 상태에서 에디트 모드거나 드래깅 윈도우가 이동 속성을 가지고 있을 경우 윈도우 이동
				else if (!m_FrontHitWindow->GetAttribute(MkBaseWindowNode::eIgnoreMovement))
				{
					bool onMove = (m_EditMode && m_AllowDragMovement && (!m_FrontHitWindow->GetRootWindow()->GetAttribute(MkBaseWindowNode::eSystemWindow)));
					if (!onMove)
					{
						onMove = m_FrontHitWindow->GetAttribute(MkBaseWindowNode::eDragMovement);
					}

					if (onMove)
					{
						MkFloat2 offset = currentCursorPosition - m_CursorStartPosition;
						if (m_FrontHitWindow->ConfirmDragMovement(m_FrontHitWindow, offset))
						{
							MkFloat2 newPosition = _ConfineMovement(m_FrontHitWindow, m_WindowAABRBegin, m_WindowOffsetToWorldPos, offset);
							m_FrontHitWindow->SetLocalAsWorldPosition(newPosition, false);
						}
					}
				}
			}

			// notify window event
			if (onFocusWindowNode != NULL)
			{
				onFocusWindowNode->__ConsumeWindowEvent();
			}
		}

		// 포커싱중인 윈도우가 없거나 활성화중인 edit box 그룹이 아닌 다른 윈도우가 포커스를 가지고 있으면 입력모드 취소
		if ((onFocusWindowNode == NULL) || (onFocusWindowNode->GetNodeName() != MK_EDIT_BOX.GetRootNodeNameOfBindingControl()))
		{
			MK_EDIT_BOX.BindControl(NULL);
		}
	}
	else
	{
		MK_EDIT_BOX.BindControl(NULL); // 활성화중인 윈도우가 없어도 입력모드 취소
	}

	// cursor
	MK_CURSOR_MGR.CommitCurrentCursor();

	// edit box
	MK_EDIT_BOX.Update();

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
				MkInt2 movement;
				if (MK_INPUT_MGR.GetKeyReleased(VK_LEFT)) movement.x = -m_KeyMovementOffset;
				if (MK_INPUT_MGR.GetKeyReleased(VK_RIGHT)) movement.x = m_KeyMovementOffset;
				if (MK_INPUT_MGR.GetKeyReleased(VK_UP)) movement.y = m_KeyMovementOffset;
				if (MK_INPUT_MGR.GetKeyReleased(VK_DOWN)) movement.y = -m_KeyMovementOffset;

				if (MK_INPUT_MGR.GetKeyPushing(VK_SHIFT))
				{
					movement.x *= m_KeyMovementFactor;
					movement.y *= m_KeyMovementFactor;
				}

				if (!movement.IsZero())
				{
					MkFloat2 offset(static_cast<float>(movement.x), static_cast<float>(movement.y));
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
	m_RootNode->UpdateAll();

	// save current target node
	if ((!m_SaveCurrentTargetWindowNodePath.Empty()) && (m_CurrentTargetWindowNode != NULL))
	{
		MkDataNode node;
		m_CurrentTargetWindowNode->Save(node);

		MkStr ext = m_SaveCurrentTargetWindowNodePath.GetFileExtension();
		ext.ToLower();
		
		if (ext == MKDEF_S2D_SCENE_FILE_EXT_BINARY)
		{
			node.SaveToBinary(m_SaveCurrentTargetWindowNodePath);
		}
		else // MKDEF_S2D_SCENE_FILE_EXT_TEXT
		{
			node.SaveToText(m_SaveCurrentTargetWindowNodePath);
		}
		
		m_SaveCurrentTargetWindowNodePath.Clear();
	}

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

void MkWindowEventManager::SetTargetWindowNode(MkBaseWindowNode* targetWindow)
{
	m_CurrentTargetWindowNode = targetWindow;

	if (m_CurrentTargetWindowNode == NULL)
	{
		_SetRegionLayerVisible(false);
	}
	else
	{
		_ShowRegionLayer();
	}

	_PassTargetWindow();
}

void MkWindowEventManager::OpenNodeNameInputSystemWindow(MkSceneNode* targetNode, MkNodeNameInputListener* owner)
{
	if (m_WindowTable.Exist(SW_NODENAMEINPUT_WINDOW_NAME) && m_ModalWindow.Empty())
	{
		MkNodeNameInputSystemWindow* sysWindow = dynamic_cast<MkNodeNameInputSystemWindow*>(m_WindowTable[SW_NODENAMEINPUT_WINDOW_NAME]);
		if (sysWindow != NULL)
		{
			sysWindow->SetUp(targetNode, owner);
		}
	}
}

void MkWindowEventManager::OpenWindowAttributeSystemWindow(MkBaseWindowNode* targetWindow)
{
	if (m_WindowTable.Exist(SW_WIN_ATTR_WINDOW_NAME) && m_ModalWindow.Empty())
	{
		MkWindowAttributeSystemWindow* sysWindow = dynamic_cast<MkWindowAttributeSystemWindow*>(m_WindowTable[SW_WIN_ATTR_WINDOW_NAME]);
		if (sysWindow != NULL)
		{
			sysWindow->SetUp(targetWindow);
		}
	}
}

void MkWindowEventManager::OpenSRectSetterSystemWindow(MkSRectInfoListener* owner, MkSceneNode* targetNode, const MkHashStr& rectName, int inputType, const MkStr& comment)
{
	if (m_WindowTable.Exist(SW_SRECT_SETTER_WINDOW_NAME) && m_ModalWindow.Empty())
	{
		MkSRectSetterSystemWindow* sysWindow = dynamic_cast<MkSRectSetterSystemWindow*>(m_WindowTable[SW_SRECT_SETTER_WINDOW_NAME]);
		if (sysWindow != NULL)
		{
			sysWindow->SetUp(owner, targetNode, rectName, static_cast<MkSRectSetterSystemWindow::eInputType>(inputType), comment);
		}
	}
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
	m_SaveCurrentTargetWindowNodePath.Clear();
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

	m_ShowWindowSelection = true;
	m_AllowDragMovement = true;
	m_KeyMovementOffset = 1;
	m_KeyMovementFactor = 10;
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
			MK_CHECK(pos == oldSize, L"잘못된 spread button 전개 순서")
				return;
		}
		m_OpeningSpreadButtons.PushBack(button);
	}
}

void MkWindowEventManager::__SpreadButtonClosed(int index)
{
	if (index >= 0)
	{
		MK_CHECK((static_cast<unsigned int>(index) + 1) == m_OpeningSpreadButtons.GetSize(), L"잘못된 spread button 해제 순서")
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

void MkWindowEventManager::__RootWindowNameChanged(const MkHashStr& oldName, const MkHashStr& newName)
{
	if (m_WindowTable.Exist(oldName))
	{
		MkBaseWindowNode* targetWindow = m_WindowTable[oldName];
		m_WindowTable.Erase(oldName);
		m_WindowTable.Create(newName, targetWindow);

		_ChangeWindowName(oldName, newName, m_OnActivatingWindows);
		_ChangeWindowName(oldName, newName, m_WaitForActivatingWindows);
		_ChangeWindowName(oldName, newName, m_WaitForDeactivatingWindows);

		if (oldName == m_LastFocusWindow)
		{
			m_LastFocusWindow = newName;
		}

		if (oldName == m_ModalWindow)
		{
			m_ModalWindow = newName;
		}
	}
}

void MkWindowEventManager::_CreateSystemWindow(MkBaseSystemWindow* systemWindow)
{
	if (systemWindow != NULL)
	{
		if (systemWindow->Initialize())
		{
			RegisterWindow(systemWindow, false);
		}
		else
		{
			MK_DELETE(systemWindow);
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

void MkWindowEventManager::_PassTargetWindow(bool ignoreActivationCheck)
{
	if (ignoreActivationCheck || IsOnActivation(TARGET_WINDOW_NAME))
	{
		MkEditModeTargetWindow* targetWindow = dynamic_cast<MkEditModeTargetWindow*>(m_WindowTable[TARGET_WINDOW_NAME]);
		if (targetWindow != NULL)
		{
			targetWindow->SetTargetNode(m_CurrentTargetWindowNode);
		}
	}
}

void MkWindowEventManager::_ChangeWindowName(const MkHashStr& oldName, const MkHashStr& newName, MkArray<MkHashStr>& targetList)
{
	unsigned int pos = targetList.FindFirstInclusion(MkArraySection(0), oldName);
	if (pos != MKDEF_ARRAY_ERROR)
	{
		targetList[pos] = newName;
	}
}

//------------------------------------------------------------------------------------------------//
