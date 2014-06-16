
#include "MkCore_MkCheck.h"
#include "MkCore_MkInputManager.h"

#include "MkCore_MkDevPanel.h"

#include "MkS2D_MkBaseWindowNode.h"
#include "MkS2D_MkWindowEventManager.h"


#define MKDEF_ARRAY_EXIST(names, target) (names.FindFirstInclusion(MkArraySection(0), target) != MKDEF_ARRAY_ERROR)
#define MKDEF_ARRAY_ERASE(names, target) names.EraseFirstInclusion(MkArraySection(0), target)

//------------------------------------------------------------------------------------------------//

void MkWindowEventManager::SetDepthBandwidth(float minDepthBandwidth, float maxDepthBandwidth)
{
	m_MinDepthBandwidth = minDepthBandwidth;
	m_MaxDepthBandwidth = maxDepthBandwidth;

	MK_CHECK(minDepthBandwidth < maxDepthBandwidth, L"MkWindowEventManager의 깊이영역 설정 오류. min : " + MkStr(minDepthBandwidth) + L", max : " + MkStr(m_MaxDepthBandwidth))
	{
		SetDepthBandwidth();
	}
}

bool MkWindowEventManager::RegisterWindow(MkBaseWindowNode* windowNode, bool activate)
{
	MK_CHECK(windowNode != NULL, L"MkWindowEventManager에 NULL window 등록 시도")
		return false;

	const MkHashStr& name = windowNode->GetNodeName();
	MK_CHECK(!m_WindowTable.Exist(name), L"MkWindowEventManager에 이미 존재하는 " + name.GetString() + L" window 등록 시도")
		return false;

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

void MkWindowEventManager::ActivateWindow(const MkHashStr& windowName)
{
	if (m_WindowTable.Exist(windowName))
	{
		MKDEF_ARRAY_ERASE(m_WaitForDeactivatingWindows, windowName);

		MKDEF_ARRAY_ERASE(m_WaitForActivatingWindows, windowName); // 활성화 대기 목록은 순서가 중요
		m_WaitForActivatingWindows.PushBack(windowName);
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

void MkWindowEventManager::Update(const MkFloat2& screenSize)
{
	// cursor operation
	if (MK_INPUT_MGR.GetMousePointerAvailable())
	{
		if (m_CursorIsDragging)
		{
			MkInt2 cp = MK_INPUT_MGR.GetAbsoluteMousePosition(true); // flip y
			MkFloat2 currentCursorPos(static_cast<float>(cp.x), static_cast<float>(cp.y));
			MkFloat2 offset = currentCursorPos - m_CursorStartPosition;
			MkFloat2 newPosition = m_WindowStartPosition + offset;

			// attribute : eConfinedToRect
			if (m_DraggingWindow->GetAttribute(MkBaseWindowNode::eConfinedToRect))
			{
				MkBaseWindowNode* pivotNode = m_DraggingWindow->GetAncestorWindowNode();
				MkFloatRect screenRect;
				screenRect.size = (pivotNode == NULL) ? screenSize : pivotNode->GetClientRect().size;
				newPosition = screenRect.Confine(MkFloatRect(newPosition, m_DraggingWindow->GetWorldAABR().size));
			}

			m_DraggingWindow->SetLocalPosition(newPosition);
		}
	}
	else
	{
		m_CursorIsDragging = false;
		m_CurrentTargetWindowNode = NULL;
	}

	// deactivation
	if (!m_WaitForDeactivatingWindows.Empty())
	{
		MK_INDEXING_LOOP(m_WaitForDeactivatingWindows, i)
		{
			MkBaseWindowNode* windowNode = m_WindowTable[m_WaitForDeactivatingWindows[i]];
			windowNode->Deactivate();
			windowNode->SetVisible(false);
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
	}

	if (!m_OnActivatingWindows.Empty())
	{
		int inputClientY = MK_INPUT_MGR.GetTargetWindowClientSize().y;

		// click event로 인한 순서 변화 반영
		unsigned int onFocusIndex = m_OnActivatingWindows.GetSize() - 1;

		MkArray<MkInputManager::InputEvent> inputEventList;
		if (MK_INPUT_MGR.GetInputEvent(inputEventList) > 0)
		{
			MK_INDEXING_LOOP(inputEventList, i)
			{
				const MkInputManager::InputEvent& evt = inputEventList[i];
				if ((evt.eventType == MkInputManager::eMousePress) || (evt.eventType == MkInputManager::eMouseDoubleClick))
				{
					MkFloat2 cursorPosition(static_cast<float>(evt.arg1), static_cast<float>(inputClientY - evt.arg2)); // flip y
					bool foundFocusWindow = false;

					// 클릭 이벤트가 발생했을 경우 뒤부터 앞으로 순회하며 focus 윈도우를 찾는다
					for (unsigned int j=onFocusIndex; j!=0xffffffff; --j)
					{
						MkBaseWindowNode* windowNode = m_WindowTable[m_OnActivatingWindows[j]];
						if (windowNode->__CheckInputTarget(cursorPosition))
						{
							if (j < onFocusIndex)
							{
								m_OnActivatingWindows.Erase(MkArraySection(j, 1));
								m_OnActivatingWindows.PushBack(windowNode->GetNodeName());
							}

							foundFocusWindow = true;
							break;
						}
					}

					m_FocusLostByClick = !foundFocusWindow;
					if (m_FocusLostByClick)
					{
						_LastWindowLostFocus();
						m_LastFocusWindow.Clear();

						m_CurrentTargetWindowNode = NULL;
					}
				}
			}
		}

		// 순서에 따른 깊이 재배치
		m_WindowTable[m_OnActivatingWindows[onFocusIndex]]->SetLocalDepth(m_MinDepthBandwidth);

		if (m_OnActivatingWindows.GetSize() > 1)
		{
			float windowDepthUnit = (m_MaxDepthBandwidth - m_MinDepthBandwidth) / static_cast<float>(m_OnActivatingWindows.GetSize() - 1);
			float offset = windowDepthUnit;

			for (unsigned int i=onFocusIndex-1; i!=0xffffffff; --i)
			{
				m_WindowTable[m_OnActivatingWindows[i]]->SetLocalDepth(m_MinDepthBandwidth + offset);
				offset += windowDepthUnit;
			}
		}

		MkBaseWindowNode* onFocusWindowNode = NULL;

		// window focus. 무효 click이 없었다면 활성화중인 윈도우 중 가장 마지막에 위치한 유효 윈도우가 focus를 가진다
		if (!m_FocusLostByClick)
		{
			for (unsigned int i=onFocusIndex; i!=0xffffffff; --i)
			{
				const MkHashStr& currWindowName = m_OnActivatingWindows[i];
				MkBaseWindowNode* windowNode = m_WindowTable[currWindowName];
				if (windowNode->__CheckFocusingTarget())
				{
					if (currWindowName != m_LastFocusWindow)
					{
						_LastWindowLostFocus();
						_SetFocusToWindowNode(windowNode);
					}

					onFocusWindowNode = windowNode;
					break;
				}
			}
		}

		// input event
		MK_INDEXING_LOOP(inputEventList, i)
		{
			const MkInputManager::InputEvent& evt = inputEventList[i];
			switch (evt.eventType)
			{
			case MkInputManager::eKeyPress:
				{
					if (onFocusWindowNode != NULL)
					{
						onFocusWindowNode->InputEventKeyPress(evt.arg0);
					}
				}
				break;
			case MkInputManager::eKeyRelease:
				{
					if (onFocusWindowNode != NULL)
					{
						onFocusWindowNode->InputEventKeyRelease(evt.arg0);
					}
				}
				break;
			case MkInputManager::eMousePress:
				{
					if (onFocusWindowNode != NULL)
					{
						onFocusWindowNode->InputEventMousePress(evt.arg0, MkFloat2(static_cast<float>(evt.arg1), static_cast<float>(inputClientY - evt.arg2)));
					}
				}
				break;
			case MkInputManager::eMouseRelease:
				{
					if (onFocusWindowNode != NULL)
					{
						onFocusWindowNode->InputEventMouseRelease(evt.arg0, MkFloat2(static_cast<float>(evt.arg1), static_cast<float>(inputClientY - evt.arg2)));
					}
				}
				break;
			case MkInputManager::eMouseDoubleClick:
				{
					if (onFocusWindowNode != NULL)
					{
						onFocusWindowNode->InputEventMouseDoubleClick(evt.arg0, MkFloat2(static_cast<float>(evt.arg1), static_cast<float>(inputClientY - evt.arg2)));
					}
				}
				break;
			case MkInputManager::eMouseWheelMove:
				{
					if (onFocusWindowNode != NULL)
					{
						onFocusWindowNode->InputEventMouseWheelMove(evt.arg0, MkFloat2(static_cast<float>(evt.arg1), static_cast<float>(inputClientY - evt.arg2)));
					}
				}
				break;
			case MkInputManager::eMouseMove:
				{
					bool inside = (evt.arg0 == 1);
					MkFloat2 cursorPosition(static_cast<float>(evt.arg1), static_cast<float>(inputClientY - evt.arg2));

					for (unsigned int i=onFocusIndex; i!=0xffffffff; --i)
					{
						MkBaseWindowNode* windowNode = m_WindowTable[m_OnActivatingWindows[i]];
						if (windowNode->__CheckFocusingTarget())
						{
							windowNode->InputEventMouseMove(inside, cursorPosition);
						}
					}
				}
				break;
			}
		}

		// click target
		if (m_CurrentTargetWindowNode != NULL)
		{
			MkInt2 cp = MK_INPUT_MGR.GetAbsoluteMousePosition(true); // flip y
			MkFloat2 currentCursorPos(static_cast<float>(cp.x), static_cast<float>(cp.y));
			if (m_CurrentTargetWindowNode->GetWindowRect().CheckIntersection(currentCursorPos))
			{
				eS2D_WindowPresetComponent component = m_CurrentTargetWindowNode->GetWindowPresetComponentType();
				if ((component >= eS2D_WPC_WindowStateTypeBegin) && (component < eS2D_WPC_WindowStateTypeEnd))
				{
					eS2D_WindowState windowState = MK_INPUT_MGR.GetMouseLeftButtonPushing() ? eS2D_WS_OnClickState : eS2D_WS_OnCursorState;
					m_CurrentTargetWindowNode->SetComponentState(component, windowState);
				}
			}
		}

		if (MK_INPUT_MGR.GetMouseLeftButtonReleased())
		{
			m_CursorIsDragging = false;
			m_CurrentTargetWindowNode = NULL;
		}
	}

	// debug
	MkInt2 cp = MK_INPUT_MGR.GetAbsoluteMousePosition(true); // flip y
	MK_DEV_PANEL.MsgToFreeboard(15, L"Mouse position : " + MkStr(cp.x) + L", " + MkStr(cp.y) + L" : " +
		MkStr(MK_INPUT_MGR.GetMouseLeftButtonPushing()) + L", " + MkStr(MK_INPUT_MGR.GetMouseMiddleButtonPushing()) + L", " + MkStr(MK_INPUT_MGR.GetMouseRightButtonPushing()));
	if (m_CursorIsDragging)
	{
		const MkFloatRect& winRect = m_DraggingWindow->GetWindowRect();
		MkStr buf = L"Drag >> ";
		buf += m_DraggingWindow->GetNodeName().GetString();
		buf += L" : (";
		buf += MkStr(winRect.position.x) + L", " + MkStr(winRect.position.y);
		buf += L"), (";
		buf += MkStr(winRect.size.x) + L", " + MkStr(winRect.size.y);
		buf += L") -> ";
		buf += winRect.CheckIntersection(MkFloat2(static_cast<float>(cp.x), static_cast<float>(cp.y)));
		MK_DEV_PANEL.MsgToFreeboard(16, buf);

		MkArray<MkHashStr> keys;
		m_DraggingWindow->GetChildNodeList(keys);
		buf.Flush();
		MK_INDEXING_LOOP(keys, i)
		{
			buf += L"   ";
			buf += keys[i];
			buf += m_DraggingWindow->GetChildNode(keys[i])->GetVisible() ? L"(0) " : L"(X) ";
		}
		if (!buf.Empty())
		{
			buf.BackSpace(1);
			MK_DEV_PANEL.MsgToFreeboard(17, buf);
		}
	}

	if (m_CurrentTargetWindowNode != NULL)
	{
		const MkFloatRect& winRect = m_CurrentTargetWindowNode->GetWindowRect();
		MkStr buf = L"Target >> ";
		buf += m_CurrentTargetWindowNode->GetNodeName().GetString();
		buf += L" : (";
		buf += MkStr(winRect.position.x) + L", " + MkStr(winRect.position.y);
		buf += L"), (";
		buf += MkStr(winRect.size.x) + L", " + MkStr(winRect.size.y);
		buf += L") -> ";
		buf += winRect.CheckIntersection(MkFloat2(static_cast<float>(cp.x), static_cast<float>(cp.y)));
		MK_DEV_PANEL.MsgToFreeboard(18, buf);

		MkArray<MkHashStr> keys;
		m_CurrentTargetWindowNode->GetChildNodeList(keys);
		buf.Flush();
		MK_INDEXING_LOOP(keys, i)
		{
			buf += L"   ";
			buf += keys[i];
			buf += m_CurrentTargetWindowNode->GetChildNode(keys[i])->GetVisible() ? L"(0) " : L"(X) ";
		}
		if (!buf.Empty())
		{
			buf.BackSpace(1);
			MK_DEV_PANEL.MsgToFreeboard(19, buf);
		}
	}
	else
	{
		MK_DEV_PANEL.MsgToFreeboard(18, L"");
		MK_DEV_PANEL.MsgToFreeboard(19, L"");
	}
}

void MkWindowEventManager::BeginWindowDragging(MkBaseWindowNode* targetWindow, const MkFloat2& cursorStartPosition)
{
	if (targetWindow != NULL)
	{
		// 가장 앞에 위치한 윈도우를 대상으로 함
		if ((!m_CursorIsDragging) || (m_DraggingWindow == NULL) || (targetWindow->GetLocalPosition().z < m_DraggingWindow->GetLocalPosition().z))
		{
			m_CursorIsDragging = true;
			m_DraggingWindow = targetWindow;
			m_CursorStartPosition = cursorStartPosition;
			m_WindowStartPosition.x = m_DraggingWindow->GetLocalPosition().x;
			m_WindowStartPosition.y = m_DraggingWindow->GetLocalPosition().y;
		}
	}
}

void MkWindowEventManager::SetCurrentTargetWindowNode(MkBaseWindowNode* targetWindow)
{
	if (targetWindow != NULL)
	{
		// 가장 앞에 위치한 윈도우를 대상으로 함
		if ((m_CurrentTargetWindowNode == NULL) || (targetWindow->GetLocalPosition().z < m_CurrentTargetWindowNode->GetLocalPosition().z))
		{
			m_CurrentTargetWindowNode = targetWindow;
		}
	}
}

void MkWindowEventManager::Clear(void)
{
	m_WindowTable.Clear();
	m_OnActivatingWindows.Clear();
	m_WaitForActivatingWindows.Clear();
	m_WaitForDeactivatingWindows.Clear();
	m_LastFocusWindow.Clear();
	m_FocusLostByClick = false;

	m_CursorIsDragging = false;
}

MkWindowEventManager::MkWindowEventManager() : MkSingletonPattern<MkWindowEventManager>()
{
	SetDepthBandwidth();
	m_CursorIsDragging = false;
	m_CurrentTargetWindowNode = NULL;
}

void MkWindowEventManager::_LastWindowLostFocus(void)
{
	if (!m_LastFocusWindow.Empty())
	{
		m_WindowTable[m_LastFocusWindow]->SetComponentState(eS2D_TS_LostFocusState);
	}
}

void MkWindowEventManager::_SetFocusToWindowNode(MkBaseWindowNode* targetNode)
{
	targetNode->SetComponentState(eS2D_TS_OnFocusState);
	m_LastFocusWindow = targetNode->GetNodeName();
}

//------------------------------------------------------------------------------------------------//
