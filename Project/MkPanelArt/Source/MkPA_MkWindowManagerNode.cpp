
#include "MkCore_MkCheck.h"
#include "MkCore_MkInputManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkHiddenEditBox.h"
#include "MkPA_MkScrollBarControlNode.h"
#include "MkPA_MkSliderControlNode.h"
#include "MkPA_MkEditBoxControlNode.h"
#include "MkPA_MkWindowManagerNode.h"


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

#if (MKDEF_PA_SHOW_WIN_MGR_EVENT_REPORT)

const MkStr NODE_EVT_NAME[] =
{
	MK_VALUE_TO_STRING(ePA_SNE_DragMovement),
	MK_VALUE_TO_STRING(ePA_SNE_BeginDrag),
	MK_VALUE_TO_STRING(ePA_SNE_EndDrag),
	MK_VALUE_TO_STRING(ePA_SNE_ChangeTheme),
	MK_VALUE_TO_STRING(ePA_SNE_CursorEntered),
	MK_VALUE_TO_STRING(ePA_SNE_CursorLeft),
	MK_VALUE_TO_STRING(ePA_SNE_CursorLBtnPressed),
	MK_VALUE_TO_STRING(ePA_SNE_CursorLBtnReleased),
	MK_VALUE_TO_STRING(ePA_SNE_CursorLBtnDBClicked),
	MK_VALUE_TO_STRING(ePA_SNE_CursorMBtnPressed),
	MK_VALUE_TO_STRING(ePA_SNE_CursorMBtnReleased),
	MK_VALUE_TO_STRING(ePA_SNE_CursorMBtnDBClicked),
	MK_VALUE_TO_STRING(ePA_SNE_CursorRBtnPressed),
	MK_VALUE_TO_STRING(ePA_SNE_CursorRBtnReleased),
	MK_VALUE_TO_STRING(ePA_SNE_CursorRBtnDBClicked),
	MK_VALUE_TO_STRING(ePA_SNE_WheelMoved),
	MK_VALUE_TO_STRING(ePA_SNE_Activate),
	MK_VALUE_TO_STRING(ePA_SNE_Deactivate),
	MK_VALUE_TO_STRING(ePA_SNE_OnFocus),
	MK_VALUE_TO_STRING(ePA_SNE_LostFocus),
	MK_VALUE_TO_STRING(ePA_SNE_CloseWindow),
	MK_VALUE_TO_STRING(ePA_SNE_CheckOn),
	MK_VALUE_TO_STRING(ePA_SNE_CheckOff),
	MK_VALUE_TO_STRING(ePA_SNE_ScrollBarMoved),
	MK_VALUE_TO_STRING(ePA_SNE_SliderMoved),
	MK_VALUE_TO_STRING(ePA_SNE_TextModified),
	MK_VALUE_TO_STRING(ePA_SNE_TextCommitted)
};

#endif

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

#if (MKDEF_PA_SHOW_WIN_MGR_EVENT_REPORT)

	MkStr msg;
	msg.Reserve(256);
	msg += GetNodeName().GetString();
	msg += L" reports ";
	msg += NODE_EVT_NAME[eventType];
	msg += L"(";
	msg += path[path.GetSize()-1].GetString();
	msg += L")";

	if (argument != NULL)
	{
		msg += L", arg : ";

		switch (eventType)
		{
		case ePA_SNE_DragMovement:
			{
				MkVec2 buffer;
				if (argument->GetData(MkSceneNode::ArgKey_DragMovement, buffer, 0))
				{
					msg += buffer;
				}
			}
			break;

		case ePA_SNE_ChangeTheme:
			{
				MkArray<MkStr> buffer;
				if (argument->GetData(MkWindowThemedNode::ArgKey_ChangeTheme, buffer))
				{
					msg += buffer[0];
					msg += L" -> ";
					msg += buffer[1];
				}
			}
			break;

		case ePA_SNE_CursorLBtnPressed:
		case ePA_SNE_CursorLBtnReleased:
		case ePA_SNE_CursorLBtnDBClicked:
		case ePA_SNE_CursorMBtnPressed:
		case ePA_SNE_CursorMBtnReleased:
		case ePA_SNE_CursorMBtnDBClicked:
		case ePA_SNE_CursorRBtnPressed:
		case ePA_SNE_CursorRBtnReleased:
		case ePA_SNE_CursorRBtnDBClicked:
			{
				MkVec2 buffer;
				if (argument->GetData(MkWindowBaseNode::ArgKey_CursorLocalPosition, buffer, 0))
				{
					msg += buffer;
				}
			}
			break;

		case ePA_SNE_WheelMoved:
			{
				int buffer;
				if (argument->GetData(MkWindowBaseNode::ArgKey_WheelDelta, buffer, 0))
				{
					msg += buffer;
				}
			}
			break;

		case ePA_SNE_ScrollBarMoved:
			{
				MkArray<int> buffer;
				if (argument->GetData(MkScrollBarControlNode::ArgKey_NewItemPosOfScrollBar, buffer) && (buffer.GetSize() == 3))
				{
					msg += buffer[0];
					msg += L" <- ";
					msg += buffer[1];
					msg += L" (";
					msg += buffer[2];
					msg += L")";
				}
			}
			break;

		case ePA_SNE_SliderMoved:
			{
				int buffer;
				if (argument->GetData(MkSliderControlNode::ArgKey_SliderPos, buffer, 0))
				{
					msg += buffer;
				}
			}
			break;

		case ePA_SNE_TextModified:
		case ePA_SNE_TextCommitted:
			{
				MkStr buffer;
				if (argument->GetData(MkEditBoxControlNode::ArgKey_Text, buffer, 0))
				{
					msg += buffer;
				}
			}
			break;
		}
	}

	MK_DEV_PANEL.MsgToLog(msg);

#endif
}

//------------------------------------------------------------------------------------------------//

void MkWindowManagerNode::Update(double currTime)
{
	// LOCK!!!
	m_UpdateLock = true;

	//------------------------------------------------------------------------------------------------//
	// world position 선 반영
	// 기본적으로 window system은 root의 world position이 변경되지 않는다고 간주하고 만들어지기 때문에
	// [input 처리 -> child window들의 local transform 변경 -> world transform 확정] 이라는 순서가 지켜짐
	// 만약 이 전제가 깨지고 root가 움직이면 input(cursor)과 위치가 맞지 않게 됨
	// 따라서 먼저 root world transform을 선 계산해 변경이 감지되면 정상적인 처리 순서 이전에 하위 전체의
	// transform을 갱신 할 필요성이 있음
	//------------------------------------------------------------------------------------------------//

	MkFloat2 oldWorldPos = m_Transform.GetWorldPosition();
	m_Transform.Update((m_ParentNodePtr == NULL) ? NULL : m_ParentNodePtr->__GetTransformPtr());
	if (m_Transform.GetWorldPosition() != oldWorldPos)
	{
		MkSceneNode::Update(currTime);
	}

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

				if (evt.activate) // 활성화. // active/deactive -> active 가능(순서 변경)
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

					// send command
					winInfo.node->SendNodeCommandTypeEvent(ePA_SNE_Activate, NULL);
				}
				else // 비활성화. active -> deactive만 허용
				{
					if (deactIndex == MKDEF_ARRAY_ERROR)
					{
						m_DeactivatingWindows.PushBack(windowName);

						if (windowName == m_ModalWindow)
						{
							m_ModalWindow.Clear();
						}

						// send command
						winInfo.node->SendNodeCommandTypeEvent(ePA_SNE_Deactivate, NULL);
					}
				}
			}
		}

		m_ActivationEvent.Clear();
	}

	//------------------------------------------------------------------------------------------------//
	// input 처리
	//
	// cursor
	//   - snapshot(특정 시점 기준)과 event(발생 시점 기준) 중 snapshot으로 처리
	//   - 이론상으로는 한 프레임에 여러번의 cursor event가 발생 할 수 있지만, 프로게이머의 apm이 최대 600정도고
	//     60fps의 분당 모니터링 횟수는 3600회이므로 현실적으로는 한 프레임에 event 하나 발생하기도 힘들기 때문
	//   (NOTE) 중복 처리 비용을 줄일 수 있지만 낮은 프레임에서는 일부 입력이 무시될 수 있음
	//
	// key
	//   - 모든 입력을 받아들여야 하기 때문에 snapshot이 아닌 event로 처리
	//------------------------------------------------------------------------------------------------//

	// cursor
	// input snapshot 생성
	MkInt2 cursorPosition = MK_INPUT_MGR.GetAbsoluteMousePosition(true) - m_InputPivotPosition;
	MkInt2 cursorMovement = MK_INPUT_MGR.GetAbsoluteMouseMovement(true);

	eButtonState leftBtnState = MK_INPUT_MGR.GetMouseLeftButtonState();
	eButtonState middleBtnState = MK_INPUT_MGR.GetMouseMiddleButtonState();
	eButtonState rightBtnState = MK_INPUT_MGR.GetMouseRightButtonState();

	bool cursorInside = MK_INPUT_MGR.GetMousePointerAvailable(); // device 영역 내에 있는지 체크하고
	if (cursorInside)
	{
		MkIntRect regionRect(MkInt2::Zero, m_TargetRegion);
		cursorInside = regionRect.CheckIntersection(cursorPosition); // whole region 내에 있는지도 체크
	}

	bool anyBtnPressed = (leftBtnState == eBS_Pressed) || (middleBtnState == eBS_Pressed) || (rightBtnState == eBS_Pressed);
	MkEditBoxControlNode* targetEditBoxNode = NULL;

	int wheelDelta = MK_INPUT_MGR.GetMouseWheelMovement();
	bool leftBtnPushing = MK_INPUT_MGR.GetMouseLeftButtonPushing();

	// 현 프레임에서 cursor를 소유한 window 목록
	MkArray<MkWindowBaseNode*> cursorOwnedNodeBuffer;
	MkArray< MkArray<MkHashStr> > cursorOwnedPathBuffer;

	// cursor가 client 영역 안에 존재하면 cursor가 위치한 window를 picking으로 검출
	if (cursorInside)
	{
		MkFloat2 fCursorPos(static_cast<float>(cursorPosition.x), static_cast<float>(cursorPosition.y));
		_PickWindowBaseNode(cursorOwnedNodeBuffer, cursorOwnedPathBuffer, fCursorPos);
	}

	// 이전 프레임의 cursor owner 중 현재 프레임에 cursor를 잃은 window를 추출해 cursor lost 통지
	MkArray< MkArray<MkHashStr> > diffSets;
	m_CursorOwnedWindowPath.GetDefferenceOfSets(cursorOwnedPathBuffer, diffSets); // diffSets = m_CursorOwnedWindowPath - cursorOwnedPathBuffer

	MkArray<MkWindowBaseNode*> cursorLostNodeBuffer;
	_UpdateWindowPath(diffSets, cursorLostNodeBuffer);

	MK_INDEXING_LOOP(cursorLostNodeBuffer, i)
	{
		cursorLostNodeBuffer[i]->UpdateCursorInput(cursorPosition, cursorMovement, false, eBS_None, eBS_None, eBS_None, 0);
	}

	// 현 프레임에서 dragging 대상으로 지정 된 window path 목록
	MkArray< MkArray<MkHashStr> > cursorDraggingNodePath;
	if (leftBtnState != eBS_Released)
	{
		cursorDraggingNodePath = m_LeftCursorDraggingNodePath;
	}

	// 현재 프레임에 cursor owner가 존재하면
	if (!cursorOwnedNodeBuffer.Empty())
	{
		MK_INDEXING_LOOP(cursorOwnedNodeBuffer, i) // 통지
		{
			cursorOwnedNodeBuffer[i]->UpdateCursorInput(cursorPosition, cursorMovement, true, leftBtnState, middleBtnState, rightBtnState, wheelDelta);
		}

		// 만약 어느 button이라도 입력되었으면 해당 window들을 입력 대상 window로 지정
		if (anyBtnPressed)
		{
			m_KeyInputTargetWindowPath = cursorOwnedPathBuffer;

			// 입력된 node가 edit box 계열일 경우 대상 지정
			MK_INDEXING_LOOP(cursorOwnedNodeBuffer, i)
			{
				if (cursorOwnedNodeBuffer[i]->IsDerivedFrom(ePA_SNT_EditBoxControlNode))
				{
					targetEditBoxNode = dynamic_cast<MkEditBoxControlNode*>(cursorOwnedNodeBuffer[i]);
					break; // 최초 하나만 대상
				}
			}
		}

		// left button에 변화가 발생하면
		if ((leftBtnState != eBS_None) && (leftBtnState != eBS_Pushing))
		{
			cursorDraggingNodePath.Clear();

			// window base 파생 객체들은 left button 입력이 감지되었다면 여기서 dragging node 저장
			if ((leftBtnState == eBS_Pressed) || (leftBtnState == eBS_DoubleClicked))
			{
				cursorDraggingNodePath.Reserve(cursorOwnedPathBuffer.GetSize());

				MK_INDEXING_LOOP(cursorOwnedNodeBuffer, i)
				{
					if (cursorOwnedNodeBuffer[i]->GetMovableByDragging())
					{
						cursorDraggingNodePath.PushBack(cursorOwnedPathBuffer[i]);
					}
				}
			}
		}
	}

	// cursor owner 갱신
	m_CursorOwnedWindowPath = cursorOwnedPathBuffer;

	// cursor 입력이 발생했으면 edit box node 지정
	if (anyBtnPressed)
	{
		MK_EDIT_BOX.SetTargetEditBoxNode(this, targetEditBoxNode);
	}

	// 하위 node중 대상 지정된 edit box가 존재하면 갱신
	MK_EDIT_BOX.Update(this);

	// left cursor dragging 적용
	if (cursorInside)
	{
		if (leftBtnPushing && (!cursorDraggingNodePath.Empty()))
		{
			MkFloat2 fCursorMov(static_cast<float>(cursorMovement.x), static_cast<float>(cursorMovement.y));

			MkArray<MkSceneNode*> targetBuffer;
			_UpdateSceneNodePath(cursorDraggingNodePath, targetBuffer);

			MK_INDEXING_LOOP(targetBuffer, i)
			{
				MkSceneNode* targetSNode = targetBuffer[i];
				const MkFloat2& oldLocalPos = targetSNode->GetLocalPosition();
				MkFloat2 targetLocalPos = oldLocalPos + fCursorMov;

				// MkWindowBaseNode 파생 객체면 자체적인 계산 반영
				// cursorDraggingNodePath 생성시 직접 처리하지 않고 이중으로 나눈 이유는 cursorDraggingNodePath에는
				// MkWindowBaseNode 파생 객체 외에도 원한다면 다른 객체(ex> MkSceneNode)도 지정 될 수 있기 때문
				if (targetSNode->IsDerivedFrom(ePA_SNT_WindowBaseNode))
				{
					MkWindowBaseNode* targetWNode = dynamic_cast<MkWindowBaseNode*>(targetSNode);
					if ((targetWNode != NULL) && targetWNode->GetMovableByDragging())
					{
						// 부모 node가 movement lock을 가지고 있으면 제한
						MkSceneNode* parentSNode = targetWNode->GetParentNode();
						if ((parentSNode != NULL) && parentSNode->IsDerivedFrom(ePA_SNT_WindowBaseNode))
						{
							MkWindowBaseNode* parentWNode = dynamic_cast<MkWindowBaseNode*>(parentSNode);
							if (parentWNode != NULL)
							{
								const MkFloatRect* movementLock = parentWNode->GetDraggingMovementLock();
								if (movementLock != NULL)
								{
									MkFloatRect targetRect(targetLocalPos, targetWNode->GetWindowRect().size);
									targetLocalPos = movementLock->Confine(targetRect);
								}
							}
						}
					}
				}

				MkFloat2 movement = targetLocalPos - oldLocalPos;
				if (!movement.IsZero())
				{
					// 위치 조정
					targetSNode->SetLocalPosition(targetLocalPos);

					// event 통지
					MkDataNode arg;
					arg.CreateUnit(MkSceneNode::ArgKey_DragMovement, MkVec2(movement.x, movement.y));
					targetSNode->StartNodeReportTypeEvent(ePA_SNE_DragMovement, &arg);
				}
			}
		}
	}
	else
	{
		cursorDraggingNodePath.Clear();
	}

	// send drag event
	if (cursorDraggingNodePath != m_LeftCursorDraggingNodePath)
	{
		MkArray< MkArray<MkHashStr> > dragBeginSets; // 현 프레임에 dragging 시작된 path들
		diffSets.Clear();
		MkArray< MkArray<MkHashStr> > dragEndSets; // 현 프레임에 dragging 종료된 path들
		cursorDraggingNodePath.IntersectionTest(m_LeftCursorDraggingNodePath, dragBeginSets, diffSets, dragEndSets);

		_SendCursorDraggingEvent(dragBeginSets, 0);
		_SendCursorDraggingEvent(dragEndSets, 1);

		m_LeftCursorDraggingNodePath = cursorDraggingNodePath; // dragging node 갱신
	}

	// key input 처리
	MkArray<MkWindowBaseNode*> keyInputTargetBuffer;
	_UpdateWindowPath(m_KeyInputTargetWindowPath, keyInputTargetBuffer);

	// key 입력을 받아들일 window가 존재하면
	if (!keyInputTargetBuffer.Empty())
	{
		// key input snapshot 생성
		MkArray<MkInputManager::InputEvent> inputEventList;
		MK_INPUT_MGR.GetInputEvent(inputEventList);

		MkArray<MkInt2> keyInputSnapshot;
		MK_INDEXING_LOOP(inputEventList, i)
		{
			const MkInputManager::InputEvent& evt = inputEventList[i];
			if (evt.eventType == MkInputManager::eKeyPress)
			{
				keyInputSnapshot.PushBack(MkInt2(1, evt.arg0));
			}
			else if (evt.eventType == MkInputManager::eKeyRelease)
			{
				keyInputSnapshot.PushBack(MkInt2(0, evt.arg0));
			}
		}

		// key event가 존재하면 통지
		if (!keyInputSnapshot.Empty())
		{
			MK_INDEXING_LOOP(keyInputTargetBuffer, i)
			{
				keyInputTargetBuffer[i]->UpdateKeyInput(keyInputSnapshot);
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
	m_TargetRegion = MK_DEVICE_MGR.GetCurrentResolution();
	
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

void MkWindowManagerNode::_SendCursorDraggingEvent(MkArray< MkArray<MkHashStr> >& pathBuffer, int type)
{
	if (!pathBuffer.Empty())
	{
		MkArray<MkSceneNode*> targetNodes;
		_UpdateSceneNodePath(pathBuffer, targetNodes);

		MK_INDEXING_LOOP(targetNodes, i)
		{
			switch (type)
			{
			case 0: targetNodes[i]->StartNodeReportTypeEvent(ePA_SNE_BeginDrag, NULL); break;
			case 1: targetNodes[i]->StartNodeReportTypeEvent(ePA_SNE_EndDrag, NULL); break;
			}
		}
	}
}

//------------------------------------------------------------------------------------------------//
