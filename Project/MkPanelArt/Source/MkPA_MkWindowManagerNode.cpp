
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
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowManagerNode ���� ����") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

bool MkWindowManagerNode::AttachWindow(MkWindowBaseNode* windowNode, eLayerType layerType)
{
	MK_CHECK(windowNode != NULL, L"MkWindowManagerNode�� NULL window ��� �õ�")
		return false;

	const MkHashStr& windowName = windowNode->GetNodeName();
	MK_CHECK(!m_UpdateLock, L"MkWindowManagerNode Update() �߿� " + windowName.GetString() + " window ��� �õ�")
		return false;

	MK_CHECK(!ChildExist(windowName), L"MkWindowManagerNode�� �̹� �����ϴ� " + windowName.GetString() + " window ��� �õ�")
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
	MK_CHECK(!m_UpdateLock, L"MkWindowManagerNode Update() �߿� " + windowName.GetString() + " window ���� �õ�")
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
				DeactivateWindow(path[0]); // path�� root�� ����
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
	// world position �� �ݿ�
	// �⺻������ window system�� root�� world position�� ������� �ʴ´ٰ� �����ϰ� ��������� ������
	// [input ó�� -> child window���� local transform ���� -> world transform Ȯ��] �̶�� ������ ������
	// ���� �� ������ ������ root�� �����̸� input(cursor)�� ��ġ�� ���� �ʰ� ��
	// ���� ���� root world transform�� �� ����� ������ �����Ǹ� �������� ó�� ���� ������ ���� ��ü��
	// transform�� ���� �� �ʿ伺�� ����
	//------------------------------------------------------------------------------------------------//

	MkFloat2 oldWorldPos = m_Transform.GetWorldPosition();
	m_Transform.Update((m_ParentNodePtr == NULL) ? NULL : m_ParentNodePtr->__GetTransformPtr());
	if (m_Transform.GetWorldPosition() != oldWorldPos)
	{
		MkSceneNode::Update(currTime);
	}

	//------------------------------------------------------------------------------------------------//
	// activation event �ݿ�
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

				if (evt.activate) // Ȱ��ȭ. // active/deactive -> active ����(���� ����)
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
				else // ��Ȱ��ȭ. active -> deactive�� ���
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
	// input ó��
	//
	// cursor
	//   - snapshot(Ư�� ���� ����)�� event(�߻� ���� ����) �� snapshot���� ó��
	//   - �̷л����δ� �� �����ӿ� �������� cursor event�� �߻� �� �� ������, ���ΰ��̸��� apm�� �ִ� 600������
	//     60fps�� �д� ����͸� Ƚ���� 3600ȸ�̹Ƿ� ���������δ� �� �����ӿ� event �ϳ� �߻��ϱ⵵ ����� ����
	//   (NOTE) �ߺ� ó�� ����� ���� �� ������ ���� �����ӿ����� �Ϻ� �Է��� ���õ� �� ����
	//
	// key
	//   - ��� �Է��� �޾Ƶ鿩�� �ϱ� ������ snapshot�� �ƴ� event�� ó��
	//------------------------------------------------------------------------------------------------//

	// cursor
	// input snapshot ����
	MkInt2 cursorPosition = MK_INPUT_MGR.GetAbsoluteMousePosition(true) - m_InputPivotPosition;
	MkInt2 cursorMovement = MK_INPUT_MGR.GetAbsoluteMouseMovement(true);

	eButtonState leftBtnState = MK_INPUT_MGR.GetMouseLeftButtonState();
	eButtonState middleBtnState = MK_INPUT_MGR.GetMouseMiddleButtonState();
	eButtonState rightBtnState = MK_INPUT_MGR.GetMouseRightButtonState();

	bool cursorInside = MK_INPUT_MGR.GetMousePointerAvailable(); // device ���� ���� �ִ��� üũ�ϰ�
	if (cursorInside)
	{
		MkIntRect regionRect(MkInt2::Zero, m_TargetRegion);
		cursorInside = regionRect.CheckIntersection(cursorPosition); // whole region ���� �ִ����� üũ
	}

	bool anyBtnPressed = (leftBtnState == eBS_Pressed) || (middleBtnState == eBS_Pressed) || (rightBtnState == eBS_Pressed);
	MkEditBoxControlNode* targetEditBoxNode = NULL;

	int wheelDelta = MK_INPUT_MGR.GetMouseWheelMovement();
	bool leftBtnPushing = MK_INPUT_MGR.GetMouseLeftButtonPushing();

	// �� �����ӿ��� cursor�� ������ window ���
	MkArray<MkWindowBaseNode*> cursorOwnedNodeBuffer;
	MkArray< MkArray<MkHashStr> > cursorOwnedPathBuffer;

	// cursor�� client ���� �ȿ� �����ϸ� cursor�� ��ġ�� window�� picking���� ����
	if (cursorInside)
	{
		MkFloat2 fCursorPos(static_cast<float>(cursorPosition.x), static_cast<float>(cursorPosition.y));
		_PickWindowBaseNode(cursorOwnedNodeBuffer, cursorOwnedPathBuffer, fCursorPos);
	}

	// ���� �������� cursor owner �� ���� �����ӿ� cursor�� ���� window�� ������ cursor lost ����
	MkArray< MkArray<MkHashStr> > diffSets;
	m_CursorOwnedWindowPath.GetDefferenceOfSets(cursorOwnedPathBuffer, diffSets); // diffSets = m_CursorOwnedWindowPath - cursorOwnedPathBuffer

	MkArray<MkWindowBaseNode*> cursorLostNodeBuffer;
	_UpdateWindowPath(diffSets, cursorLostNodeBuffer);

	MK_INDEXING_LOOP(cursorLostNodeBuffer, i)
	{
		cursorLostNodeBuffer[i]->UpdateCursorInput(cursorPosition, cursorMovement, false, eBS_None, eBS_None, eBS_None, 0);
	}

	// �� �����ӿ��� dragging ������� ���� �� window path ���
	MkArray< MkArray<MkHashStr> > cursorDraggingNodePath;
	if (leftBtnState != eBS_Released)
	{
		cursorDraggingNodePath = m_LeftCursorDraggingNodePath;
	}

	// ���� �����ӿ� cursor owner�� �����ϸ�
	if (!cursorOwnedNodeBuffer.Empty())
	{
		MK_INDEXING_LOOP(cursorOwnedNodeBuffer, i) // ����
		{
			cursorOwnedNodeBuffer[i]->UpdateCursorInput(cursorPosition, cursorMovement, true, leftBtnState, middleBtnState, rightBtnState, wheelDelta);
		}

		// ���� ��� button�̶� �ԷµǾ����� �ش� window���� �Է� ��� window�� ����
		if (anyBtnPressed)
		{
			m_KeyInputTargetWindowPath = cursorOwnedPathBuffer;

			// �Էµ� node�� edit box �迭�� ��� ��� ����
			MK_INDEXING_LOOP(cursorOwnedNodeBuffer, i)
			{
				if (cursorOwnedNodeBuffer[i]->IsDerivedFrom(ePA_SNT_EditBoxControlNode))
				{
					targetEditBoxNode = dynamic_cast<MkEditBoxControlNode*>(cursorOwnedNodeBuffer[i]);
					break; // ���� �ϳ��� ���
				}
			}
		}

		// left button�� ��ȭ�� �߻��ϸ�
		if ((leftBtnState != eBS_None) && (leftBtnState != eBS_Pushing))
		{
			cursorDraggingNodePath.Clear();

			// window base �Ļ� ��ü���� left button �Է��� �����Ǿ��ٸ� ���⼭ dragging node ����
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

	// cursor owner ����
	m_CursorOwnedWindowPath = cursorOwnedPathBuffer;

	// cursor �Է��� �߻������� edit box node ����
	if (anyBtnPressed)
	{
		MK_EDIT_BOX.SetTargetEditBoxNode(this, targetEditBoxNode);
	}

	// ���� node�� ��� ������ edit box�� �����ϸ� ����
	MK_EDIT_BOX.Update(this);

	// left cursor dragging ����
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

				// MkWindowBaseNode �Ļ� ��ü�� ��ü���� ��� �ݿ�
				// cursorDraggingNodePath ������ ���� ó������ �ʰ� �������� ���� ������ cursorDraggingNodePath����
				// MkWindowBaseNode �Ļ� ��ü �ܿ��� ���Ѵٸ� �ٸ� ��ü(ex> MkSceneNode)�� ���� �� �� �ֱ� ����
				if (targetSNode->IsDerivedFrom(ePA_SNT_WindowBaseNode))
				{
					MkWindowBaseNode* targetWNode = dynamic_cast<MkWindowBaseNode*>(targetSNode);
					if ((targetWNode != NULL) && targetWNode->GetMovableByDragging())
					{
						// �θ� node�� movement lock�� ������ ������ ����
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
					// ��ġ ����
					targetSNode->SetLocalPosition(targetLocalPos);

					// event ����
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
		MkArray< MkArray<MkHashStr> > dragBeginSets; // �� �����ӿ� dragging ���۵� path��
		diffSets.Clear();
		MkArray< MkArray<MkHashStr> > dragEndSets; // �� �����ӿ� dragging ����� path��
		cursorDraggingNodePath.IntersectionTest(m_LeftCursorDraggingNodePath, dragBeginSets, diffSets, dragEndSets);

		_SendCursorDraggingEvent(dragBeginSets, 0);
		_SendCursorDraggingEvent(dragEndSets, 1);

		m_LeftCursorDraggingNodePath = cursorDraggingNodePath; // dragging node ����
	}

	// key input ó��
	MkArray<MkWindowBaseNode*> keyInputTargetBuffer;
	_UpdateWindowPath(m_KeyInputTargetWindowPath, keyInputTargetBuffer);

	// key �Է��� �޾Ƶ��� window�� �����ϸ�
	if (!keyInputTargetBuffer.Empty())
	{
		// key input snapshot ����
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

		// key event�� �����ϸ� ����
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
	filterAttr.Set(ePA_SNA_AcceptInput); // input�� �޾Ƶ��̴� node�� ������� ��

	// panel picking
	if (PickPanel(panelBuffer, worldPoint, GetWorldDepth(), filterAttr))
	{
		nodeBuffer.Reserve(panelBuffer.GetSize());

		MK_INDEXING_LOOP(panelBuffer, i)
		{
			// MkWindowBaseNode �Ļ� ��ü�� ������� ��
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
