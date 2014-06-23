
#include "MkCore_MkCheck.h"
#include "MkCore_MkInputManager.h"
#include "MkCore_MkDevPanel.h"

#include "MkS2D_MkTexturePool.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkDrawStep.h"
#include "MkS2D_MkBaseWindowNode.h"
#include "MkS2D_MkWindowEventManager.h"


#define MKDEF_ARRAY_EXIST(names, target) (names.FindFirstInclusion(MkArraySection(0), target) != MKDEF_ARRAY_ERROR)
#define MKDEF_ARRAY_ERASE(names, target) names.EraseFirstInclusion(MkArraySection(0), target)

const static MkHashStr DARKEN_RECT_NAME = L"Darken";

//------------------------------------------------------------------------------------------------//

void MkWindowEventManager::SetUp(MkDrawStep* drawStep)
{
	m_DrawStep = drawStep;

	if (m_RootNode == NULL)
	{
		m_RootNode = new MkSceneNode(L"Root");
		MkSRect* srect = m_RootNode->CreateSRect(DARKEN_RECT_NAME);

		// modal 윈도우 출력때 화면 어둡게 만드는 레이어 생성
		MkBaseTexturePtr texture;
		MK_TEXTURE_POOL.GetBitmapTexture(MK_WR_PRESET.GetSystemImageFilePath(), texture, 0);

		MK_CHECK(texture != NULL, L"MkWindowEventManager의 Darken layer 생성 실패") {}
		if (texture != NULL)
		{
			srect->SetTexture(texture, MK_WR_PRESET.GetDarkenLayerSubsetName());
		}
		else
		{
			m_RootNode->DeleteSRect(DARKEN_RECT_NAME);
		}

		m_RootNode->Update();
	}

	if ((m_RootNode != NULL) && (m_DrawStep != NULL))
	{
		m_DrawStep->AddSceneNode(m_RootNode);

		MkSRect* srect = m_RootNode->GetSRect(DARKEN_RECT_NAME);
		if (srect != NULL)
		{
			srect->SetLocalRect(m_DrawStep->GetRegionRect());
			srect->SetVisible(false);
		}
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
	windowNode->Update(); // 초기화

	m_WindowTable.Create(name, windowNode);

	// 포커스 전환
	_LastWindowLostFocus();
	_SetFocusToWindowNode(windowNode);

	if (activate)
	{
		m_OnActivatingWindows.PushBack(name);
	}

	return true;
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

void MkWindowEventManager::ToggleWindow(const MkHashStr& windowName)
{
	if (m_WindowTable.Exist(windowName))
	{
		if (MKDEF_ARRAY_EXIST(m_OnActivatingWindows, windowName))
		{
			DeactivateWindow(windowName);
		}
		else
		{
			ActivateWindow(windowName);
		}
	}
}

void MkWindowEventManager::Update(void)
{
	if ((m_DrawStep == NULL) || (m_RootNode == NULL))
		return;

	const MkFloat2& screenSize = m_DrawStep->GetRegionRect().size;

	if (MK_INPUT_MGR.GetKeyPushing(VK_CONTROL) && MK_INPUT_MGR.GetKeyPushing(VK_SHIFT))
	{
		if (MK_INPUT_MGR.GetKeyReleased(MKDEF_S2D_TOGGLE_KEY_BETWEEN_NORMAL_AND_EDIT_MODE))
		{
			m_EditMode = !m_EditMode;

			MK_DEV_PANEL.MsgToLog((m_EditMode) ? L"> Toggle to Edit mode" : L"> Toggle to Normal mode");
		}
	}

	// current cursor state
	MkInt2 currentCursorPoint = MK_INPUT_MGR.GetAbsoluteMousePosition(true); // flip y
	MkFloat2 currentCursorPosition(static_cast<float>(currentCursorPoint.x), static_cast<float>(currentCursorPoint.y));
	bool currentButtonPressed[3] = { MK_INPUT_MGR.GetMouseLeftButtonPressed(), MK_INPUT_MGR.GetMouseMiddleButtonPressed(), MK_INPUT_MGR.GetMouseRightButtonPressed() };
	bool currentButtonPushing[3] = { MK_INPUT_MGR.GetMouseLeftButtonPushing(), MK_INPUT_MGR.GetMouseMiddleButtonPushing(), MK_INPUT_MGR.GetMouseRightButtonPushing() };

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
					if (windowNode->__CheckInputTarget(currentCursorPosition))
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

					m_CurrentTargetWindowNode = NULL;
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
				if (windowNode->__CheckFocusingTarget())
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

		// 모달 상태면 darken layer 깊이 조정
		if (!m_ModalWindow.Empty())
		{
			if ((onFocusWindowNode != NULL) && (onFocusWindowNode->GetNodeName() == m_ModalWindow))
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
			// 있어서는 안되는 일이기는 하지만, 모달 윈도우를 지정한 다음 바로 disable 시켜버리면 포커스를 갖지 못할 경우도 있음
			else
			{
				m_ModalWindow.Clear();
				_SetDarkenLayerEnable(false);
			}
		}

		// input event
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
							if (windowNode->__CheckFocusingTarget())
							{
								windowNode->InputEventMouseMove(inside, currentButtonPushing, cursorPosition);
							}
						}
					}
					else
					{
						onFocusWindowNode->InputEventMouseMove(inside, currentButtonPushing, cursorPosition);
					}
				}
				// 그 외 이벤트는 focus window에만 적용
				else if (onFocusWindowNode != NULL)
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
			// 드래그 이동 및 타겟 윈도우(에디터 모드용) 선택
			if (currentButtonPressed[0] && onFocusWindowNode->GetWorldAABR().CheckIntersection(currentCursorPosition))
			{
				MkPairArray<float, MkBaseWindowNode*> hitWindows(8);
				onFocusWindowNode->__GetHitWindows(currentCursorPosition, hitWindows);
				if (!hitWindows.Empty())
				{
					hitWindows.SortInAscendingOrder();

					if (m_EditMode)
					{
						m_CurrentTargetWindowNode = hitWindows.GetFieldAt(0);
					}

					if (!m_CursorIsDragging)
					{
						MkBaseWindowNode* draggingWindow = NULL;
						if (m_EditMode)
						{
							draggingWindow = m_CurrentTargetWindowNode;
						}
						else
						{
							MK_INDEXING_LOOP(hitWindows, i)
							{
								if (hitWindows.GetFieldAt(0)->GetAttribute(MkBaseWindowNode::eDragMovement))
								{
									draggingWindow = hitWindows.GetFieldAt(0);
									break;
								}
							}
						}

						if (draggingWindow != NULL)
						{
							m_CursorIsDragging = true;
							m_DraggingWindow = draggingWindow;
							m_CursorStartPosition = currentCursorPosition;
							m_WindowAABRBegin = m_DraggingWindow->GetWorldAABR().position;
							m_WindowOffsetToWorldPos = MkFloat2(m_DraggingWindow->GetWorldPosition().x, m_DraggingWindow->GetWorldPosition().y) - m_WindowAABRBegin;
						}
					}
				}
			}
		}
	}

	// cursor operation
	if (MK_INPUT_MGR.GetMousePointerAvailable())
	{
		if (m_CursorIsDragging) // drag movement
		{
			MkFloat2 offset = currentCursorPosition - m_CursorStartPosition;
			MkFloat2 newPosition = _ConfineMovement(m_DraggingWindow, screenSize, m_WindowAABRBegin, m_WindowOffsetToWorldPos, offset);
			m_DraggingWindow->SetLocalAsWorldPosition(newPosition, false);
		}
	}
	else
	{
		m_CursorIsDragging = false;
	}

	if (MK_INPUT_MGR.GetMouseLeftButtonReleased())
	{
		m_CursorIsDragging = false;
	}

	if (m_EditMode)
	{
		// movement by arrow key
		if (m_CurrentTargetWindowNode != NULL)
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
				MkFloat2 newPosition = _ConfineMovement(m_CurrentTargetWindowNode, screenSize, worldAABRBegin, oldPosition - worldAABRBegin, offset);
				m_CurrentTargetWindowNode->SetLocalAsWorldPosition(newPosition, false);
			}
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
		if (winRect.CheckIntersection(currentCursorPosition))
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
	m_CursorIsDragging = false;
	m_ModalWindow.Clear();
	m_CurrentTargetWindowNode = NULL;
}

MkWindowEventManager::MkWindowEventManager() : MkSingletonPattern<MkWindowEventManager>()
{
	m_MinDepthBandwidth = MKDEF_S2D_MAX_WORLD_DEPTH * 0.1f;
	m_MaxDepthBandwidth = MKDEF_S2D_MAX_WORLD_DEPTH * 0.9f;

	m_DrawStep = NULL;
	m_RootNode = NULL;
	m_EditMode = true;
	m_FocusLostByClick = false;
	m_CursorIsDragging = false;
	m_CurrentTargetWindowNode = NULL;
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

MkFloat2 MkWindowEventManager::_ConfineMovement
(MkBaseWindowNode* targetNode, const MkFloat2& screenSize, const MkFloat2& posBegin, const MkFloat2& toWorld, const MkFloat2& offset) const
{
	if (targetNode->GetAttribute(MkBaseWindowNode::eConfinedToScreen))
	{
		return (MkFloatRect(MkFloat2(0.f, 0.f), screenSize).Confine(MkFloatRect(posBegin + offset, targetNode->GetWorldAABR().size)) + toWorld);
	}
	else if (targetNode->GetAttribute(MkBaseWindowNode::eConfinedToParent))
	{
		MkBaseWindowNode* anchorNode = targetNode->GetAncestorWindowNode();
		if (anchorNode == NULL)
		{
			return (MkFloatRect(MkFloat2(0.f, 0.f), screenSize).Confine(MkFloatRect(posBegin + offset, targetNode->GetWorldAABR().size)) + toWorld);
		}
		else
		{
			return (anchorNode->GetWorldAABR().Confine(MkFloatRect(posBegin + offset, targetNode->GetWorldAABR().size)) + toWorld);
		}
	}
	return (posBegin + offset + toWorld);
}

//------------------------------------------------------------------------------------------------//
