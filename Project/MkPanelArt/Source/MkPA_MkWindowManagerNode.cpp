
#include "MkCore_MkCheck.h"
#include "MkCore_MkInputManager.h"

#include "MkPA_MkWindowBaseNode.h"
#include "MkPA_MkWindowManagerNode.h"


//const MkStr MkWindowManagerNode::NamePrefix(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX);

//------------------------------------------------------------------------------------------------//

MkWindowManagerNode* MkWindowManagerNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkWindowManagerNode* node = __TSI_SceneNodeDerivedInstanceOp<MkWindowManagerNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowManagerNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

bool MkWindowManagerNode::AttachWindow(MkWindowBaseNode* windowNode, eLayerType layerType)
{
	MK_CHECK(windowNode != NULL, L"MkWindowManagerNode에 NULL window 등록 시도")
		return false;

	const MkHashStr& windowName = windowNode->GetNodeName();
	MK_CHECK(!m_UpdateLock, L"MkWindowManagerNode Update() 중에 " + windowName.GetString() + " window 등록 시도")
		return false;

	MK_CHECK(!ChildExist(windowName), L"MkWindowManagerNode에 이미 존재하는 " + windowName.GetString() + " window 등록 시도")
		return false;

	bool ok = AttachChildNode(windowNode);
	if (ok)
	{
		m_DeactivatingWindows.PushBack(windowName);
		windowNode->SetVisible(false);

		_RootWindowInfo& wi = m_RootWindowList.Create(windowName);
		wi.node = windowNode;
		wi.layerType = layerType;
	}
	return ok;
}

bool MkWindowManagerNode::DeleteWindow(const MkHashStr& windowName)
{
	MK_CHECK(!m_UpdateLock, L"MkWindowManagerNode Update() 중에 " + windowName.GetString() + " window 삭제 시도")
		return false;

	bool ok = m_RootWindowList.Exist(windowName);
	if (ok)
	{
		m_DeactivatingWindows.EraseFirstInclusion(MkArraySection(0), windowName);
		m_ActivatingWindows[m_RootWindowList[windowName].layerType].EraseFirstInclusion(MkArraySection(0), windowName);
		if (windowName == m_ModalWindow)
		{
			m_ModalWindow.Clear();
		}

		m_RootWindowList.Erase(windowName);

		RemoveChildNode(windowName);
	}
	return ok;
}

bool MkWindowManagerNode::IsActivating(const MkHashStr& windowName) const
{
	return m_RootWindowList.Exist(windowName) ? (m_DeactivatingWindows.FindFirstInclusion(MkArraySection(0), windowName) == MKDEF_ARRAY_ERROR) : false;
}

void MkWindowManagerNode::ActivateWindow(const MkHashStr& windowName, bool modal)
{
	if (m_RootWindowList.Exist(windowName))
	{
		_ActivationEvent& evt = m_ActivationEvent.PushBack();
		evt.windowName = windowName;
		evt.activate = true;
		evt.modal = modal;
	}
}

void MkWindowManagerNode::DeactivateWindow(const MkHashStr& windowName)
{
	if (m_RootWindowList.Exist(windowName))
	{
		_ActivationEvent& evt = m_ActivationEvent.PushBack();
		evt.windowName = windowName;
		evt.activate = false;
		evt.modal = false;
	}
}

//------------------------------------------------------------------------------------------------//

void MkWindowManagerNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	switch (eventType)
	{
	case ePA_SNE_CloseWindow:
		{
			if (!path.Empty())
			{
				DeactivateWindow(path[0]); // path의 root를 닫음
			}
		}
		break;
	}
}

//------------------------------------------------------------------------------------------------//

void MkWindowManagerNode::Update(double currTime)
{
	// LOCK!!!
	m_UpdateLock = true;

	//------------------------------------------------------------------------------------------------//
	// activation event 반영
	//------------------------------------------------------------------------------------------------//

	if (!m_ActivationEvent.Empty())
	{
		MK_INDEXING_LOOP(m_ActivationEvent, i)
		{
			const _ActivationEvent& evt = m_ActivationEvent[i];
			const MkHashStr& windowName = evt.windowName;

			if (m_RootWindowList.Exist(windowName))
			{
				unsigned int deactIndex = m_DeactivatingWindows.FindFirstInclusion(MkArraySection(0), windowName);
				_RootWindowInfo& winInfo = m_RootWindowList[windowName];

				MkArray<MkHashStr>& activatingLayer = m_ActivatingWindows[winInfo.layerType];
				activatingLayer.EraseFirstInclusion(MkArraySection(0), windowName);

				winInfo.node->SetVisible(evt.activate);

				if (evt.activate) // 활성화
				{
					if (deactIndex != MKDEF_ARRAY_ERROR)
					{
						m_DeactivatingWindows.Erase(MkArraySection(deactIndex, 1));
					}

					if (evt.modal) // modal
					{
						m_ModalWindow = windowName;
					}
					else // modaless
					{
						if (windowName == m_ModalWindow)
						{
							m_ModalWindow.Clear();
						}

						activatingLayer.PushBack(windowName);
					}
				}
				else // 비활성화
				{
					if (deactIndex == MKDEF_ARRAY_ERROR)
					{
						m_DeactivatingWindows.PushBack(windowName);

						if (windowName == m_ModalWindow)
						{
							m_ModalWindow.Clear();
						}
					}
				}
			}
		}

		m_ActivationEvent.Clear();
	}

	//------------------------------------------------------------------------------------------------//
	// cursor input 처리
	// snapshot(특정 시점 기준)과 event(발생 시점 기준) 중 snapshot으로 처리
	// 이론상으로는 한 프레임에 여러번의 cursor event가 발생 할 수 있지만, 프로게이머의 apm이 최대 600정도고
	// 60fps의 분당 모니터링 횟수는 3600회이므로 현실적으로는 한 프레임에 event 하나 발생하기도 힘들기 때문
	// (NOTE) 중복 처리 비용을 줄일 수 있지만 낮은 프레임에서는 일부 입력이 무시될 수 있음
	//------------------------------------------------------------------------------------------------//

	// cursor position
	MkInt2 cursorPosition = MK_INPUT_MGR.GetAbsoluteMousePosition(true);
	MkInt2 cursorMovement = MK_INPUT_MGR.GetAbsoluteMouseMovement(true);
	bool cursorInside = MK_INPUT_MGR.GetMousePointerAvailable();

	// 현 프레임에서 cursor를 소유한 window path 목록
	MkArray< MkArray<MkHashStr> > cursorOwnedPathBuffer;

	// cursor가 client 영역 안에 존재하면
	if (cursorInside)
	{
		// cursor가 위치한 window를 picking으로 검출
		MkArray<MkWindowBaseNode*> nodeBuffer;
		if (_PickWindowBaseNode(nodeBuffer, cursorOwnedPathBuffer, MkFloat2(static_cast<float>(cursorPosition.x), static_cast<float>(cursorPosition.y))))
		{
			eCursorState leftState = MK_INPUT_MGR.GetMouseLeftButtonState();
			eCursorState middleState = MK_INPUT_MGR.GetMouseMiddleButtonState();
			eCursorState rightState = MK_INPUT_MGR.GetMouseRightButtonState();
			int wheelDelta = MK_INPUT_MGR.GetMouseWheelMovement();
			
			MK_INDEXING_LOOP(nodeBuffer, i) // 통지
			{
				nodeBuffer[i]->UpdateCursorState(cursorPosition, cursorMovement, true, leftState, middleState, rightState, wheelDelta);
			}

			// 만약 어느 button이라도 입력되었으면 해당 window들을 입력 대상 window로 지정
			if ((leftState == eCS_Pressed) || (middleState == eCS_Pressed) || (rightState == eCS_Pressed))
			{
				m_KeyInputTargetWindowPath = cursorOwnedPathBuffer;
			}

			// left button에 변화가 발생하면
			if ((leftState != eCS_None) && (leftState != eCS_Pushing))
			{
				m_LeftCursorDraggingNodePath.Clear();

				// left button 입력이 감지되었다면 dragging node 저장
				if ((leftState == eCS_Pressed) || (leftState == eCS_DoubleClicked))
				{
					m_LeftCursorDraggingNodePath.Reserve(cursorOwnedPathBuffer.GetSize());

					MK_INDEXING_LOOP(nodeBuffer, i)
					{
						if (nodeBuffer[i]->GetMovableByDragging())
						{
							m_LeftCursorDraggingNodePath.PushBack(cursorOwnedPathBuffer[i]);
						}
					}
				}
			}
		}
	}

	// 이전 프레임의 cursor owner 중 현재 프레임에 cursor를 잃은 window를 추출
	MkArray< MkArray<MkHashStr> > diffSets;
	m_CursorOwnedWindowPath.GetDefferenceOfSets(cursorOwnedPathBuffer, diffSets); // diffSets = m_CursorOwnedWindowPath - cursorOwnedPathBuffer

	MkArray<MkWindowBaseNode*> nodeBuffer;
	_UpdateWindowPath(diffSets, nodeBuffer);

	MK_INDEXING_LOOP(nodeBuffer, i) // 통지
	{
		nodeBuffer[i]->UpdateCursorState(cursorPosition, cursorMovement, false, eCS_None, eCS_None, eCS_None, 0);
	}

	m_CursorOwnedWindowPath = cursorOwnedPathBuffer; // cursor owner 갱신

	// left cursor dragging 적용
	if (cursorInside)
	{
		if (MK_INPUT_MGR.GetMouseLeftButtonPushing() && (!m_LeftCursorDraggingNodePath.Empty()))
		{
			MkFloat2 cursorFMov(static_cast<float>(cursorMovement.x), static_cast<float>(cursorMovement.y));

			MkArray<MkSceneNode*> targetBuffer;
			_UpdateSceneNodePath(m_LeftCursorDraggingNodePath, targetBuffer);

			MK_INDEXING_LOOP(targetBuffer, i)
			{
				targetBuffer[i]->SetLocalPosition(targetBuffer[i]->GetLocalPosition() + cursorFMov);
			}
		}
	}
	else
	{
		m_LeftCursorDraggingNodePath.Clear();
	}

	//------------------------------------------------------------------------------------------------//
	// key input 처리
	// snapshot(특정 시점 기준)과 event(발생 시점 기준) 중 event로 처리
	//------------------------------------------------------------------------------------------------//

	MkArray<MkWindowBaseNode*> keyInputTargetBuffer;
	_UpdateWindowPath(m_KeyInputTargetWindowPath, keyInputTargetBuffer);

	// key 입력을 받아들일 window가 존재하면
	if (!keyInputTargetBuffer.Empty())
	{
		// key event 생성
		MkArray<MkWindowBaseNode::KeyInputEvent> keyEventList;

		MkArray<MkInputManager::InputEvent> inputEventList;
		MK_INPUT_MGR.GetInputEvent(inputEventList);

		MK_INDEXING_LOOP(inputEventList, i)
		{
			const MkInputManager::InputEvent& evt = inputEventList[i];
			if (evt.eventType == MkInputManager::eKeyPress)
			{
				MkWindowBaseNode::KeyInputEvent& ke = keyEventList.PushBack();
				ke.type = MkWindowBaseNode::eKIT_Press;
				ke.keyCode = evt.arg0;
			}
			else if (evt.eventType == MkInputManager::eKeyRelease)
			{
				MkWindowBaseNode::KeyInputEvent& ke = keyEventList.PushBack();
				ke.type = MkWindowBaseNode::eKIT_Release;
				ke.keyCode = evt.arg0;
			}
		}

		// key 입력이 존재하면
		if (!keyEventList.Empty())
		{
			MK_INDEXING_LOOP(keyInputTargetBuffer, i)
			{
				keyInputTargetBuffer[i]->UpdateKeyInput(keyEventList);
			}
		}
	}
	

	//------------------------------------------------------------------------------------------------//

	MkSceneNode::Update(currTime);

	// LOCK!!!
	m_UpdateLock = false;
}

void MkWindowManagerNode::Clear(void)
{
	m_DeactivatingWindows.Flush();
	m_ActivatingWindows[eLT_Low].Flush();
	m_ActivatingWindows[eLT_Normal].Flush();
	m_ActivatingWindows[eLT_High].Flush();
	m_ModalWindow.Clear();

	m_ActivationEvent.Clear();

	m_RootWindowList.Clear();

	m_UpdateLock = false;
	m_ActivationEvent.Clear();
	m_CursorOwnedWindowPath.Clear();
	m_KeyInputTargetWindowPath.Clear();
	m_LeftCursorDraggingNodePath.Clear();
	
	MkSceneNode::Clear();
}

MkWindowManagerNode::MkWindowManagerNode(const MkHashStr& name) : MkSceneNode(name)
{
	m_DepthBandwidth = 1000.f;
	
	m_DeactivatingWindows.Reserve(256);
	m_ActivatingWindows.Create(eLT_Low).Reserve(64);
	m_ActivatingWindows.Create(eLT_Normal).Reserve(256);
	m_ActivatingWindows.Create(eLT_High).Reserve(64);

	m_UpdateLock = false;
}

//------------------------------------------------------------------------------------------------//

void MkWindowManagerNode::_UpdateSceneNodePath(MkArray< MkArray<MkHashStr> >& targetPath, MkArray<MkSceneNode*>& nodeBuffer)
{
	MkArray<unsigned int> killList(targetPath.GetSize());
	nodeBuffer.Reserve(targetPath.GetSize());

	MK_INDEXING_LOOP(targetPath, i)
	{
		MkSceneNode* finalNode = GetChildNode(targetPath[i]);
		if (finalNode == NULL)
		{
			killList.PushBack(i);
		}
		else
		{
			nodeBuffer.PushBack(finalNode);
		}
	}

	targetPath.Erase(killList);
}

void MkWindowManagerNode::_UpdateWindowPath(MkArray< MkArray<MkHashStr> >& targetPath, MkArray<MkWindowBaseNode*>& nodeBuffer)
{
	MkArray<unsigned int> killList(targetPath.GetSize());
	nodeBuffer.Reserve(targetPath.GetSize());

	MK_INDEXING_LOOP(targetPath, i)
	{
		MkSceneNode* targetNode = GetChildNode(targetPath[i]);
		MkWindowBaseNode* finalNode = (targetNode == NULL) ?
			NULL : (targetNode->IsDerivedFrom(ePA_SNT_WindowBaseNode) ? dynamic_cast<MkWindowBaseNode*>(targetNode) : NULL);
		if (finalNode == NULL)
		{
			killList.PushBack(i);
		}
		else
		{
			nodeBuffer.PushBack(finalNode);
		}
	}

	targetPath.Erase(killList);
}

bool MkWindowManagerNode::_PickWindowBaseNode(MkArray<MkWindowBaseNode*>& nodeBuffer, const MkFloat2& worldPoint) const
{
	nodeBuffer.Clear();

	MkArray<MkPanel*> panelBuffer;
	MkBitField32 filterAttr;
	filterAttr.Set(ePA_SNA_AcceptInput); // input을 받아들이는 node만 대상으로 함

	// panel picking
	if (PickPanel(panelBuffer, worldPoint, GetWorldDepth(), filterAttr))
	{
		nodeBuffer.Reserve(panelBuffer.GetSize());

		MK_INDEXING_LOOP(panelBuffer, i)
		{
			// MkWindowBaseNode 파생 객체만 대상으로 함
			MkSceneNode* targetNode = panelBuffer[i]->GetParentNode()->FindNearestDerivedNode(ePA_SNT_WindowBaseNode);
			if (targetNode != NULL)
			{
				MkWindowBaseNode* winBaseNode = dynamic_cast<MkWindowBaseNode*>(targetNode);
				if ((winBaseNode != NULL) && (nodeBuffer.FindFirstInclusion(MkArraySection(0), winBaseNode) == MKDEF_ARRAY_ERROR))
				{
					nodeBuffer.PushBack(winBaseNode);
				}
			}
		}
	}
	return !nodeBuffer.Empty();
}

bool MkWindowManagerNode::_PickWindowBaseNode(MkArray<MkWindowBaseNode*>& nodeBuffer, MkArray< MkArray<MkHashStr> >& pathBuffer, const MkFloat2& worldPoint) const
{
	pathBuffer.Clear();

	if (_PickWindowBaseNode(nodeBuffer, worldPoint))
	{
		pathBuffer.Reserve(nodeBuffer.GetSize());

		MK_INDEXING_LOOP(nodeBuffer, i)
		{
			nodeBuffer[i]->GetWindowPath(pathBuffer.PushBack());
		}
	}

	return !pathBuffer.Empty();
}

//------------------------------------------------------------------------------------------------//
