
#include "MkCore_MkCheck.h"
#include "MkCore_MkInputManager.h"

#include "MkCore_MkDevPanel.h"

#include "MkS2D_MkBaseWindowNode.h"
#include "MkS2D_MkWindowEventManager.h"


#define MKDEF_ARRAY_EXIST(names, target) (names.FindFirstInclusion(MkArraySection(0), target) != MKDEF_ARRAY_ERROR)
#define MKDEF_ARRAY_ERASE(names, target) names.EraseFirstInclusion(MkArraySection(0), target)

//------------------------------------------------------------------------------------------------//

bool MkWindowEventManager::RegisterWindow(MkBaseWindowNode* windowNode, bool activate)
{
	MK_CHECK(windowNode != NULL, L"MkWindowEventManager에 NULL window 등록 시도")
		return false;

	const MkHashStr& name = windowNode->GetNodeName();
	MK_CHECK(!m_WindowTable.Exist(name), L"MkWindowEventManager에 이미 존재하는 " + name.GetString() + L" window 등록 시도")
		return false;

	m_WindowTable.Create(name, windowNode);

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

void MkWindowEventManager::Update(void)
{
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
					MkFloat2 cursorPosition(static_cast<float>(evt.arg1), static_cast<float>(evt.arg2));

					// 클릭 이벤트가 발생했을 경우 뒤부터 앞으로 순회하며 focus 윈도우를 찾는다
					for (unsigned int j=onFocusIndex; j!=0xffffffff; --j)
					{
						MkBaseWindowNode* windowNode = m_WindowTable[m_OnActivatingWindows[j]];
						if (windowNode->__CheckInputTarget(cursorPosition))
						{
							if (i < onFocusIndex)
							{
								m_OnActivatingWindows.Erase(MkArraySection(j, 1));
								m_OnActivatingWindows.PushBack(windowNode->GetNodeName());
							}
							break;
						}
					}
				}
			}
		}

		// window focus. 활성화중인 윈도우 중 가장 마지막에 위치한 유효 윈도우가 focus를 가진다
		for (unsigned int i=onFocusIndex; i!=0xffffffff; --i)
		{
			const MkHashStr& currWindowName = m_OnActivatingWindows[i];
			MkBaseWindowNode* windowNode = m_WindowTable[currWindowName];
			if (windowNode->__CheckEffectiveTarget())
			{
				if (currWindowName != m_LastFocusWindow)
				{
					if (m_LastFocusWindow.Empty()) // 최초 실행
					{
						for (unsigned int cnt=onFocusIndex, j=0; j<cnt; ++j)
						{
							MkBaseWindowNode* lostFocusWindowNode = m_WindowTable[m_OnActivatingWindows[j]];
							lostFocusWindowNode->SetComponentState(eS2D_TS_LostFocusState);
							lostFocusWindowNode->LostFocus();
						}
					}
					else
					{
						MkBaseWindowNode* lostFocusWindowNode = m_WindowTable[m_LastFocusWindow];
						lostFocusWindowNode->SetComponentState(eS2D_TS_LostFocusState);
						lostFocusWindowNode->LostFocus();
					}

					windowNode->SetComponentState(eS2D_TS_OnFocusState);
					windowNode->OnFocus();

					m_LastFocusWindow = currWindowName;
				}
				break;
			}
		}
			/*
			// 커서 push에 의한 
			MK_INDEXING_LOOP(inputEventList, i)
			{
				const MkInputManager::InputEvent& evt = inputEventList[i];
				switch (evt.eventType)
				{
				case MkInputManager::eKeyPress:
					onFocusWindowNode->InputEventKeyPress(evt.arg0);
					break;

				case MkInputManager::eKeyRelease:
					onFocusWindowNode->InputEventKeyRelease(evt.arg0);
					break;

				case MkInputManager::eMousePress:
				case MkInputManager::eMouseRelease:
				case MkInputManager::eMouseDoubleClick:
				case MkInputManager::eMouseWheelMove:
				case MkInputManager::eMouseMove:
					{
						MkFloat2 position(static_cast<float>(evt.arg1), static_cast<float>(evt.arg2));
						if (onFocusWindowNode->GetWorldAABR().CheckIntersection(position))
						{
							switch (evt.eventType)
							{
							case MkInputManager::eMousePress: onFocusWindowNode->InputEventMousePress(evt.arg0, position); break;
							case MkInputManager::eMouseRelease: onFocusWindowNode->InputEventMouseRelease(evt.arg0, position); break;
							case MkInputManager::eMouseDoubleClick: onFocusWindowNode->InputEventMouseDoubleClick(evt.arg0, position); break;
							case MkInputManager::eMouseWheelMove: onFocusWindowNode->InputEventMouseWheelMove(evt.arg0, position); break;
							case MkInputManager::eMouseMove: onFocusWindowNode->InputEventMouseMove(evt.arg0 == 1, position); break;
							}
						}
						else
						{
						}

					}
					break;
				}
			}
			*/
	}
}

void MkWindowEventManager::Clear(void)
{
	m_WindowTable.Clear();
	m_OnActivatingWindows.Clear();
	m_WaitForActivatingWindows.Clear();
	m_WaitForDeactivatingWindows.Clear();
	m_LastFocusWindow.Clear();
}

//------------------------------------------------------------------------------------------------//
