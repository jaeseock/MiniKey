
#include "MkCore_MkCheck.h"
#include "MkCore_MkPairArray.h"
#include "MkCore_MkInputManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkRenderer.h"
#include "MkPA_MkHiddenEditBox.h"
#include "MkPA_MkScrollBarControlNode.h"
#include "MkPA_MkSliderControlNode.h"
#include "MkPA_MkEditBoxControlNode.h"
#include "MkPA_MkListBoxControlNode.h"
#include "MkPA_MkWindowManagerNode.h"


const MkHashStr MkWindowManagerNode::ModalEffectNodeName(MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"MEN");

const MkHashStr MkWindowManagerNode::ObjKey_DepthBandwidth(L"DepthBW");
const MkHashStr MkWindowManagerNode::ObjKey_DeactivatingWindows(L"DeactivateWins");
const MkHashStr MkWindowManagerNode::ObjKey_ActivatingWindows(L"ActivateWins");
const MkHashStr MkWindowManagerNode::ObjKey_ModalWindow(L"Modal");

#define MKDEF_DEAULT_DEPTH_BANDWIDTH 1000.f

//------------------------------------------------------------------------------------------------//

MkWindowManagerNode* MkWindowManagerNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkWindowManagerNode* node = __TSI_SceneNodeDerivedInstanceOp<MkWindowManagerNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowManagerNode ���� ����") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

bool MkWindowManagerNode::AttachWindow(MkWindowBaseNode* windowNode)
{
	MK_CHECK(windowNode != NULL, L"MkWindowManagerNode�� NULL window ��� �õ�")
		return false;

	const MkHashStr& windowName = windowNode->GetNodeName();
	MK_CHECK(!m_AttachWindowLock, L"MkSceneNode::Update() �߿� " + windowName.GetString() + " window ��� �õ�")
		return false;

	MK_CHECK(!ChildExist(windowName), L"MkWindowManagerNode�� �̹� �����ϴ� " + windowName.GetString() + " window ��� �õ�")
		return false;

	bool ok = AttachChildNode(windowNode);
	if (ok)
	{
		windowNode->SetVisible(false);
		windowNode->SetSkipUpdateWhileInvisible(true); // visible�� �ƴϸ� update ���� ����

		m_RootWindowList.Create(windowName, windowNode);
	}
	return ok;
}

bool MkWindowManagerNode::DeleteWindow(const MkHashStr& windowName)
{
	bool ok = m_RootWindowList.Exist(windowName);
	if (ok)
	{
		// ���� lock�� �ɷ� ������ ���� Update()�� ����
		if (m_DeleteWindowLock)
		{
			m_DeletingWindow.PushBack(windowName);
		}
		// �׷��� ������ �ٷ� �ݿ�
		else
		{
			m_ActivatingWindows.EraseFirstInclusion(MkArraySection(0), windowName);

			if (windowName == m_ModalWindow)
			{
				m_ModalWindow.Clear();
			}

			if (windowName == m_CurrentFocusWindow)
			{
				if (m_ActivatingWindows.Empty())
				{
					m_CurrentFocusWindow.Clear();
				}
				else
				{
					m_CurrentFocusWindow = m_ActivatingWindows[0];
					m_RootWindowList[m_CurrentFocusWindow]->SendNodeCommandTypeEvent(ePA_SNE_OnFocus, NULL);
				}
			}

			if ((!m_ExclusiveOpenningWindow.Empty()) && (windowName == m_ExclusiveOpenningWindow[0]))
			{
				m_ExclusiveOpenningWindow.Clear();
				m_ExclusiveWindowException.Clear();
			}

			m_RootWindowList.Erase(windowName);

			RemoveChildNode(windowName);
		}
	}
	return ok;
}

bool MkWindowManagerNode::IsActivating(const MkHashStr& windowName) const
{
	return m_RootWindowList.Exist(windowName) ?
		((m_ActivatingWindows.FindFirstInclusion(MkArraySection(0), windowName) != MKDEF_ARRAY_ERROR) || (windowName == m_ModalWindow)) : false;
}

MkHashStr MkWindowManagerNode::GetFrontWindowName(void) const
{
	if (!m_ModalWindow.Empty())
		return m_ModalWindow;

	if (!m_ActivatingWindows.Empty())
		return m_ActivatingWindows[0];

	return MkHashStr::EMPTY;
}

MkHashStr MkWindowManagerNode::GetFocusWindowName(void) const
{
	return m_CurrentFocusWindow;
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
	MK_VALUE_TO_STRING(ePA_SNE_CursorLBtnHold),
	MK_VALUE_TO_STRING(ePA_SNE_CursorMBtnPressed),
	MK_VALUE_TO_STRING(ePA_SNE_CursorMBtnReleased),
	MK_VALUE_TO_STRING(ePA_SNE_CursorMBtnDBClicked),
	MK_VALUE_TO_STRING(ePA_SNE_CursorMBtnHold),
	MK_VALUE_TO_STRING(ePA_SNE_CursorRBtnPressed),
	MK_VALUE_TO_STRING(ePA_SNE_CursorRBtnReleased),
	MK_VALUE_TO_STRING(ePA_SNE_CursorRBtnDBClicked),
	MK_VALUE_TO_STRING(ePA_SNE_CursorRBtnHold),
	MK_VALUE_TO_STRING(ePA_SNE_WheelMoved),
	MK_VALUE_TO_STRING(ePA_SNE_Activate),
	MK_VALUE_TO_STRING(ePA_SNE_Deactivate),
	MK_VALUE_TO_STRING(ePA_SNE_OnFocus),
	MK_VALUE_TO_STRING(ePA_SNE_LostFocus),
	MK_VALUE_TO_STRING(ePA_SNE_ToggleExclusiveWindow),
	MK_VALUE_TO_STRING(ePA_SNE_CloseWindow),
	MK_VALUE_TO_STRING(ePA_SNE_CheckOn),
	MK_VALUE_TO_STRING(ePA_SNE_CheckOff),
	MK_VALUE_TO_STRING(ePA_SNE_ScrollBarMoved),
	MK_VALUE_TO_STRING(ePA_SNE_SliderMoved),
	MK_VALUE_TO_STRING(ePA_SNE_TextModified),
	MK_VALUE_TO_STRING(ePA_SNE_TextCommitted),
	MK_VALUE_TO_STRING(ePA_SNE_ItemEntered),
	MK_VALUE_TO_STRING(ePA_SNE_ItemLeft),
	MK_VALUE_TO_STRING(ePA_SNE_ItemLBtnPressed),
	MK_VALUE_TO_STRING(ePA_SNE_ItemLBtnReleased),
	MK_VALUE_TO_STRING(ePA_SNE_ItemLBtnDBClicked),
	MK_VALUE_TO_STRING(ePA_SNE_ItemMBtnPressed),
	MK_VALUE_TO_STRING(ePA_SNE_ItemMBtnReleased),
	MK_VALUE_TO_STRING(ePA_SNE_ItemMBtnDBClicked),
	MK_VALUE_TO_STRING(ePA_SNE_ItemRBtnPressed),
	MK_VALUE_TO_STRING(ePA_SNE_ItemRBtnReleased),
	MK_VALUE_TO_STRING(ePA_SNE_ItemRBtnDBClicked),
	MK_VALUE_TO_STRING(ePA_SNE_DropDownItemSet)
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

	case ePA_SNE_ToggleExclusiveWindow:
		{
			MkArray<MkHashStr> targetPath;
			MkArray<MkHashStr> exceptionPath;
			if (argument != NULL)
			{
				MkArray<MkHashStr> buffer;
				if (argument->GetDataEx(MkWindowBaseNode::ArgKey_ExclusiveWindow, buffer))
				{
					targetPath = path;
					targetPath += buffer; // path�� �߰� ���� ��θ� ���� ���� ��θ� ����
				}

				buffer.Clear();
				if (argument->GetDataEx(MkWindowBaseNode::ArgKey_ExclusiveException, buffer))
				{
					exceptionPath = path;
					exceptionPath += buffer; // path�� �߰� ���� ��θ� ���� ���� ��θ� ����
				}
			}

			if ((!targetPath.Empty()) && (targetPath.GetSize() > 1)) // window manager�� root window�� �ƴϰ�
			{
				MkSceneNode* targetNode = GetChildNode(targetPath);
				if ((targetNode != NULL) && targetNode->IsDerivedFrom(ePA_SNT_WindowBaseNode)) // window base �Ļ� ��ü��
				{
					bool newOpen = (m_ExclusiveOpenningWindow.Empty() || (targetPath != m_ExclusiveOpenningWindow));

					if (!m_ExclusiveOpenningWindow.Empty())
					{
						_CloseExclusiveOpenningWindow();
					}

					if (newOpen)
					{
						targetNode->SetVisible(true);
						m_ExclusiveOpenningWindow = targetPath;
						m_ExclusiveWindowException = exceptionPath;
					}
				}
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
		case ePA_SNE_CursorLBtnHold:
		case ePA_SNE_CursorMBtnPressed:
		case ePA_SNE_CursorMBtnReleased:
		case ePA_SNE_CursorMBtnDBClicked:
		case ePA_SNE_CursorMBtnHold:
		case ePA_SNE_CursorRBtnPressed:
		case ePA_SNE_CursorRBtnReleased:
		case ePA_SNE_CursorRBtnDBClicked:
		case ePA_SNE_CursorRBtnHold:
			{
				MkVec2 buffer;
				if (argument->GetData(MkWindowBaseNode::ArgKey_CursorLocalPosition, buffer, 0))
				{
					msg += L"L";
					msg += buffer;
				}
				if (argument->GetData(MkWindowBaseNode::ArgKey_CursorWorldPosition, buffer, 0))
				{
					msg += L", W";
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

		case ePA_SNE_ToggleExclusiveWindow:
			{
				MkArray<MkHashStr> buffer;
				argument->GetDataEx(MkWindowBaseNode::ArgKey_ExclusiveWindow, buffer);
				// ArgKey_ExclusiveException ������ ǥ������ ����

				MK_INDEXING_LOOP(path, i)
				{
					if (i > 0)
					{
						msg += L".";
					}
					msg += path[i].GetString();
				}

				if ((!path.Empty()) && (!buffer.Empty()))
				{
					msg += L".";
				}

				MK_INDEXING_LOOP(buffer, i)
				{
					if (i > 0)
					{
						msg += L".";
					}
					msg += buffer[i].GetString();
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

		case ePA_SNE_ItemEntered:
		case ePA_SNE_ItemLeft:
		case ePA_SNE_ItemLBtnPressed:
		case ePA_SNE_ItemLBtnReleased:
		case ePA_SNE_ItemLBtnDBClicked:
		case ePA_SNE_ItemMBtnPressed:
		case ePA_SNE_ItemMBtnReleased:
		case ePA_SNE_ItemMBtnDBClicked:
		case ePA_SNE_ItemRBtnPressed:
		case ePA_SNE_ItemRBtnReleased:
		case ePA_SNE_ItemRBtnDBClicked:
		case ePA_SNE_DropDownItemSet:
			{
				MkHashStr buffer;
				if (argument->GetDataEx(MkListBoxControlNode::ArgKey_Item, buffer, 0))
				{
					msg += buffer.GetString();
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
	//------------------------------------------------------------------------------------------------//
	// reserved deletion
	//------------------------------------------------------------------------------------------------//

	if (!m_DeletingWindow.Empty())
	{
		MK_INDEXING_LOOP(m_DeletingWindow, i)
		{
			DeleteWindow(m_DeletingWindow[i]);
		}

		m_DeletingWindow.Clear();
	}

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

	// �� �����ӿ��� focusing ������ window
	// eLT_Normal, modal window ������� active window���� ������ ����� �� ����
	MkHashStr focusWindowName;
	bool clearAllFocus = false; // cursor click�� window ���� �ܸ� �������� ���

	// window depth �缳�� ����. deactive�� �����ϰ� active�϶��� ����
	bool updateWindowDepth = false;
	bool updateModalDepth = false;
	bool updateModalEffectDepth = false;

	if (!m_ActivationEvent.Empty())
	{
		MkPairArray<MkWindowBaseNode*, bool> commandTarget(m_ActivationEvent.GetSize());

		MK_INDEXING_LOOP(m_ActivationEvent, i)
		{
			const _ActivationEvent& evt = m_ActivationEvent[i];
			const MkHashStr& windowName = evt.windowName;

			MkWindowBaseNode* winNode = m_RootWindowList[windowName];
			winNode->SetVisible(evt.activate);
			
			if (evt.activate) // Ȱ��ȭ. active/deactive -> active ����(���� ����)
			{
				m_ActivatingWindows.EraseFirstInclusion(MkArraySection(0), windowName);

				if (evt.modal && m_ModalWindow.Empty()) // modal
				{
					m_ModalWindow = windowName;

					updateModalDepth = true;
					updateModalEffectDepth = true;
					focusWindowName = windowName;
				}
				else // modaless
				{
					if (windowName == m_ModalWindow)
					{
						updateModalEffectDepth = true;
						m_ModalWindow.Clear();
					}
					
					m_ActivatingWindows.Insert(0, windowName); // 0�� ��ġ�� �������

					updateWindowDepth = true;

					if (m_ModalWindow.Empty()) // �̹� modal window�� �����ϸ� ��Ŀ���� ���� ����
					{
						focusWindowName = windowName;
					}
				}

				commandTarget.PushBack(winNode, true);
			}
			else // ��Ȱ��ȭ. active -> deactive�� ���
			{
				if (IsActivating(windowName))
				{
					if (windowName == m_ModalWindow)
					{
						updateModalEffectDepth = true;
						m_ModalWindow.Clear();
					}
					else
					{
						m_ActivatingWindows.EraseFirstInclusion(MkArraySection(0), windowName);
					}

					if (m_ModalWindow.Empty() && (!m_ActivatingWindows.Empty()))
					{
						focusWindowName = m_ActivatingWindows[0];
					}

					commandTarget.PushBack(winNode, false);
				}
			}
		}

		m_ActivationEvent.Clear();

		// send command
		MK_INDEXING_LOOP(commandTarget, i)
		{
			commandTarget.GetKeyAt(i)->SendNodeCommandTypeEvent(commandTarget.GetFieldAt(i) ? ePA_SNE_Activate : ePA_SNE_Deactivate, NULL);
		}
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

	// scene portal legacy�� ���� �ʾ����� ���� ����
	if (!m_HasScenePortalLegacy)
	{
		m_SystemRect.position.Clear();
		m_SystemRect.size = MK_DEVICE_MGR.GetCurrentResolution();
	}

	// cursor snapshot ����
	MkInt2 cursorPosition = MK_INPUT_MGR.GetAbsoluteMousePosition(true) - m_SystemRect.position;
	MkInt2 cursorMovement = MK_INPUT_MGR.GetAbsoluteMouseMovement(true);

	eButtonState leftBtnState = MK_INPUT_MGR.GetMouseLeftButtonState();
	eButtonState middleBtnState = MK_INPUT_MGR.GetMouseMiddleButtonState();
	eButtonState rightBtnState = MK_INPUT_MGR.GetMouseRightButtonState();

	bool anyBtnPressed = (leftBtnState == eBS_Pressed) || (middleBtnState == eBS_Pressed) || (rightBtnState == eBS_Pressed);
	bool leftBtnPushing = MK_INPUT_MGR.GetMouseLeftButtonPushing();
	int wheelDelta = MK_INPUT_MGR.GetMouseWheelMovement();

	// system ���� ���� �ִ��� ���� �˻�
	bool cursorInside = MK_INPUT_MGR.GetMousePointerAvailable();

	if (cursorInside && m_HasScenePortalLegacy) // scene portal legacy�� �޾����� �Է� ���� ���ɿ��� üũ
	{
		cursorInside = m_CursorIsInsideOfScenePortal;
		m_CursorIsInsideOfScenePortal = false;
	}
	
	if (cursorInside) // whole region ���� �ִ��� üũ
	{
		MkIntRect regionRect(MkInt2::Zero, m_SystemRect.size);
		cursorInside = regionRect.CheckIntersection(cursorPosition);
	}

	if (cursorInside && (MK_RENDERER.GetWebDialog() != NULL)) // renderer�� dialog ������ ����
	{
		const RECT& dr = MK_RENDERER.GetDialogRect(); // dialog ����(top-down ��ǥ��)
		MkInt2 cp = MK_INPUT_MGR.GetAbsoluteMousePosition(false); // top-down ��ǥ���� cursor position
		cursorInside = ((cp.x < dr.left) || (cp.x > dr.right) || (cp.y < dr.top) || (cp.x > dr.bottom));
	}

	m_HasScenePortalLegacy = false;

	// �� �����ӿ��� cursor�� ������ window ���
	MkArray<MkWindowBaseNode*> cursorOwnedNodeBuffer;
	MkArray< MkArray<MkHashStr> > cursorOwnedPathBuffer;

	// LOCK!!!
	m_DeleteWindowLock = true;

	// cursor�� client ���� �ȿ� �����ϸ� cursor�� ��ġ�� window�� picking���� ����
	if (cursorInside)
	{
		// modal�� �����ϸ� �ش� window��, ������ ���� ��ü ���
		const MkSceneNode* pickTarget = this;
		if (!m_ModalWindow.Empty())
		{
			pickTarget = m_RootWindowList[m_ModalWindow];
		}

		MkFloat2 fCursorPos(static_cast<float>(cursorPosition.x), static_cast<float>(cursorPosition.y));
		_PickWindowBaseNode(pickTarget, cursorOwnedNodeBuffer, cursorOwnedPathBuffer, fCursorPos);
	}

	// ��Ÿ�� ���� ���� �����찡 ���� �� ��� ���� ������ ������Ű�� ���ϸ� ����
	if (!m_ExclusiveOpenningWindow.Empty())
	{
		bool closeExclusiveOpenningWindow = true;
		if (m_ExclusiveOpenningWindow[0] == GetFrontWindowName()) // �Ҽ� window�� front��
		{
			if (anyBtnPressed)
			{
				if (cursorInside && (!cursorOwnedNodeBuffer.Empty())) // ���� �ȿ��� hit�� �߻�������
				{
					MkSceneNode* targetNode = GetChildNode(m_ExclusiveOpenningWindow);
					MkSceneNode* exceptionNode = m_ExclusiveWindowException.Empty() ? NULL : GetChildNode(m_ExclusiveWindowException);
					
					if (targetNode != NULL)
					{
						MK_INDEXING_LOOP(cursorOwnedNodeBuffer, i)
						{
							MkSceneNode* hitNode = cursorOwnedNodeBuffer[i];

							// ���/���� node�� hit node Ȥ�� hit node�� �����̸� ����
							if (((targetNode == hitNode) || hitNode->IsAncestorNode(targetNode)) ||
								((exceptionNode != NULL) && ((exceptionNode == hitNode) || hitNode->IsAncestorNode(exceptionNode))))
							{
								closeExclusiveOpenningWindow = false; 
								break;
							}
						}
					}
				}
			}
			else
			{
				closeExclusiveOpenningWindow = false; // btn �Է��� ������ ����
			}
		}

		if (closeExclusiveOpenningWindow)
		{
			_CloseExclusiveOpenningWindow();
		}
	}

	// ���� �������� cursor owner �� ���� �����ӿ� cursor�� ���� window�� ������ cursor lost ����
	MkArray< MkArray<MkHashStr> > diffSets;
	m_CursorOwnedWindowPath.GetDifferenceOfSets(cursorOwnedPathBuffer, diffSets); // diffSets = m_CursorOwnedWindowPath - cursorOwnedPathBuffer

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

	MkEditBoxControlNode* targetEditBoxNode = NULL;

	// ���� �����ӿ� cursor owner�� ������
	if (cursorOwnedNodeBuffer.Empty())
	{
		clearAllFocus = anyBtnPressed;
	}
	// ���� �����ӿ� cursor owner�� �����ϸ�
	else
	{
		MK_INDEXING_LOOP(cursorOwnedNodeBuffer, i) // ����
		{
			cursorOwnedNodeBuffer[i]->UpdateCursorInput(cursorPosition, cursorMovement, true, leftBtnState, middleBtnState, rightBtnState, wheelDelta);
		}

		// ���� ��� button�̶� �ԷµǾ�����
		if (anyBtnPressed)
		{
			// �ش� window���� �Է� ��� window�� ����
			m_KeyInputTargetWindowPath = cursorOwnedPathBuffer;

			// �Էµ� node�� edit box �迭�� ��� edit ��� ����
			MK_INDEXING_LOOP(cursorOwnedNodeBuffer, i)
			{
				if (cursorOwnedNodeBuffer[i]->IsDerivedFrom(ePA_SNT_EditBoxControlNode))
				{
					targetEditBoxNode = dynamic_cast<MkEditBoxControlNode*>(cursorOwnedNodeBuffer[i]);
					break; // ���� �ϳ��� ���
				}
			}

			// picking�� node�� root window�� front�� �ø�. ���� �ϳ��� ���
			const MkHashStr& targetRootWindowName = cursorOwnedPathBuffer[0][0];
			if (m_RootWindowList.Exist(targetRootWindowName) &&
				(targetRootWindowName != m_ModalWindow)) // modal window�� �̹� ���� �տ� ��ġ�ϰ� �����Ƿ� ����
			{
				if (targetRootWindowName != m_ActivatingWindows[0])
				{
					m_ActivatingWindows.EraseFirstInclusion(MkArraySection(0), targetRootWindowName);
					m_ActivatingWindows.Insert(0, targetRootWindowName);
				}

				updateWindowDepth = true;
				focusWindowName = targetRootWindowName;
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

		// ���� �� ��ȿŬ�� �߻��ߴµ� edit box�� �ƴϸ� focus ����
		// renderer�� dialog window�� focus�� ������ ���� ��� ���⼭ ���� ��
		if (cursorInside && (!MK_EDIT_BOX.IsBinding()))
		{
			HWND mainHandle = MK_DEVICE_MGR.GetTargetWindow()->GetWindowHandle();
			if (mainHandle != GetFocus())
			{
				::SetFocus(mainHandle);
			}
		}
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
	// focus window ����
	//------------------------------------------------------------------------------------------------//

	bool clearFocusingWindow = false;
	bool giveFocusToTarget = false;

	// 1st. modal window�� ���� ��
	if ((!focusWindowName.Empty()) && (focusWindowName == m_ModalWindow))
	{
		clearFocusingWindow = true;
		giveFocusToTarget = true;
	}
	// modal window�� �����ϸ� focus event�� ���õ�
	else if (m_ModalWindow.Empty())
	{
		// 2nd. click���� ���� focus ����
		if (clearAllFocus)
		{
			clearFocusingWindow = true;
		}
		// 3rd. normal window ���� �������� ���� ����
		else if (!focusWindowName.Empty())
		{
			clearFocusingWindow = true;
			giveFocusToTarget = true;
		}
	}

	if (clearFocusingWindow)
	{
		if ((!m_CurrentFocusWindow.Empty()) && m_RootWindowList.Exist(m_CurrentFocusWindow))
		{
			m_RootWindowList[m_CurrentFocusWindow]->SendNodeCommandTypeEvent(ePA_SNE_LostFocus, NULL);
		}
		m_CurrentFocusWindow.Clear();
	}

	if (giveFocusToTarget)
	{
		m_CurrentFocusWindow = focusWindowName;
		m_RootWindowList[m_CurrentFocusWindow]->SendNodeCommandTypeEvent(ePA_SNE_OnFocus, NULL);
	}

	//------------------------------------------------------------------------------------------------//
	// ���� �뿪�� �� ����
	//------------------------------------------------------------------------------------------------//

	// activating window
	if (updateWindowDepth && (!m_ActivatingWindows.Empty()))
	{
		float frontDepth = m_DepthBandwidth * 0.2f; // 20% ~ 100%
		float gridPerWindow = (m_DepthBandwidth - frontDepth) / static_cast<float>(m_ActivatingWindows.GetSize()); // root window�� ������
		float targetDepth = frontDepth;

		MK_INDEXING_LOOP(m_ActivatingWindows, i)
		{
			m_RootWindowList[m_ActivatingWindows[i]]->SetLocalDepth(targetDepth);
			targetDepth += gridPerWindow;
		}
	}

	// modal effect(darken)
	if (updateModalEffectDepth)
	{
		MkWindowThemedNode* effectNode = NULL;
		if (ChildExist(ModalEffectNodeName))
		{
			effectNode = dynamic_cast<MkWindowThemedNode*>(GetChildNode(ModalEffectNodeName));
		}
		else
		{
			effectNode = MkWindowThemedNode::CreateChildNode(this, ModalEffectNodeName);
			if (effectNode != NULL)
			{
				effectNode->SetThemeName(MkWindowThemeData::DefaultThemeName);
				effectNode->SetComponentType(MkWindowThemeData::eCT_DarkZone);
				effectNode->SetFormState(MkWindowThemeFormData::eS_Default);
				effectNode->SetSkipUpdateWhileInvisible(true);
			}
		}

		if (effectNode != NULL)
		{
			bool enable = !m_ModalWindow.Empty();
			effectNode->SetVisible(enable);
			if (enable)
			{
				effectNode->SetClientSize(MkFloat2(static_cast<float>(m_SystemRect.size.x), static_cast<float>(m_SystemRect.size.y)));
				effectNode->SetLocalDepth(m_DepthBandwidth * 0.15f); // 15%
			}
		}
	}

	// modal
	if (updateModalDepth && (!m_ModalWindow.Empty()))
	{
		m_RootWindowList[m_ModalWindow]->SetLocalDepth(m_DepthBandwidth * 0.1f); // 10%
	}
	
	//------------------------------------------------------------------------------------------------//

	// LOCK!!!
	m_AttachWindowLock = true;

	// update
	MkSceneNode::Update(currTime);

	// LOCK!!!
	m_AttachWindowLock = false;
	m_DeleteWindowLock = false;
}

void MkWindowManagerNode::Clear(void)
{
	m_ActivatingWindows.Flush();
	m_ModalWindow.Clear();
	m_CurrentFocusWindow.Clear();

	m_DeletingWindow.Clear();
	m_ActivationEvent.Clear();

	MkHashMapLooper<MkHashStr, MkWindowBaseNode*> looper(m_RootWindowList);
	MK_STL_LOOP(looper)
	{
		RemoveChildNode(looper.GetCurrentKey());
	}
	m_RootWindowList.Clear();

	m_AttachWindowLock = false;
	m_DeleteWindowLock = false;
	m_CursorIsInsideOfScenePortal = false;
	m_ActivationEvent.Clear();
	m_CursorOwnedWindowPath.Clear();
	m_KeyInputTargetWindowPath.Clear();
	m_LeftCursorDraggingNodePath.Clear();
	m_ExclusiveOpenningWindow.Clear();
	m_ExclusiveWindowException.Clear();
	
	MkSceneNode::Clear();
}

void MkWindowManagerNode::Save(MkDataNode& node) const
{
	// modal effect node ����
	static MkArray<MkHashStr> exceptions;
	if (exceptions.Empty())
	{
		exceptions.PushBack(ModalEffectNodeName);
	}
	_AddExceptionList(node, SystemKey_NodeExceptions, exceptions);

	// run
	MkSceneNode::Save(node);
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkWindowManagerNode);

void MkWindowManagerNode::SetObjectTemplate(MkDataNode& node)
{
	MkSceneNode::SetObjectTemplate(node);

	node.CreateUnit(ObjKey_DepthBandwidth, MKDEF_DEAULT_DEPTH_BANDWIDTH);
	// ObjKey_DeactivatingWindows
	// ObjKey_ActivatingWindows
	node.CreateUnit(ObjKey_ModalWindow, MkStr::EMPTY);
}

void MkWindowManagerNode::LoadObject(const MkDataNode& node)
{
	MkSceneNode::LoadObject(node);

	// depth bandwidth
	node.GetData(ObjKey_DepthBandwidth, m_DepthBandwidth, 0);
}

void MkWindowManagerNode::LoadComplete(const MkDataNode& node)
{
	// window system ������ ���ؼ��� ���� �ڽĵ��� �ʿ�
	MkArray<MkHashStr> deactivatingWins;
	if (node.GetDataEx(ObjKey_DeactivatingWindows, deactivatingWins))
	{
		MK_INDEXING_LOOP(deactivatingWins, i)
		{
			_CheckAndRegisterWindowNode(deactivatingWins[i]);
		}
	}

	MkArray<MkHashStr> activatingWins;
	if (node.GetDataEx(ObjKey_ActivatingWindows, activatingWins))
	{
		MK_INDEXING_LOOP(activatingWins, i)
		{
			_CheckAndRegisterWindowNode(activatingWins[i]);
		}
	}

	MkHashStr modalWin;
	if (node.GetDataEx(ObjKey_ModalWindow, modalWin, 0) && (!modalWin.Empty()))
	{
		_CheckAndRegisterWindowNode(modalWin);
	}

	// activation(����)
	MK_INDEXING_RLOOP(activatingWins, i)
	{
		ActivateWindow(activatingWins[i], false);
	}

	if (!modalWin.Empty())
	{
		ActivateWindow(modalWin, true);
	}
}

void MkWindowManagerNode::SaveObject(MkDataNode& node) const
{
	MkSceneNode::SaveObject(node);

	// depth bandwidth
	node.SetData(ObjKey_DepthBandwidth, m_DepthBandwidth, 0);

	// deactivating window
	MkArray<MkStr> deactivatingWins(m_RootWindowList.GetSize());

	MkConstHashMapLooper<MkHashStr, MkWindowBaseNode*> looper(m_RootWindowList);
	MK_STL_LOOP(looper)
	{
		const MkHashStr& winName = looper.GetCurrentKey();
		if (!IsActivating(winName))
		{
			deactivatingWins.PushBack(winName.GetString());
		}
	}

	if (!deactivatingWins.Empty())
	{
		node.CreateUnit(ObjKey_DeactivatingWindows, deactivatingWins);
	}

	// activating window
	if (!m_ActivatingWindows.Empty())
	{
		node.CreateUnitEx(ObjKey_ActivatingWindows, m_ActivatingWindows);
	}

	// modal window
	node.SetDataEx(ObjKey_ModalWindow, m_ModalWindow, 0);
}

MkWindowManagerNode::MkWindowManagerNode(const MkHashStr& name) : MkSceneNode(name)
{
	m_DepthBandwidth = MKDEF_DEAULT_DEPTH_BANDWIDTH;
	
	m_ActivatingWindows.Reserve(256);

	m_AttachWindowLock = false;
	m_DeleteWindowLock = false;
	m_HasScenePortalLegacy = false;
	m_CursorIsInsideOfScenePortal = false;
}

void MkWindowManagerNode::__SetScenePortalLegacy(bool cursorInside, const MkIntRect& systemRect)
{
	m_HasScenePortalLegacy = true;
	m_CursorIsInsideOfScenePortal = cursorInside;
	m_SystemRect = systemRect;
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

bool MkWindowManagerNode::_PickWindowBaseNode(const MkSceneNode* targetNode, MkArray<MkWindowBaseNode*>& nodeBuffer, const MkFloat2& worldPoint) const
{
	nodeBuffer.Clear();

	MkArray<MkPanel*> panelBuffer;
	MkBitField32 filterAttr;
	filterAttr.Set(ePA_SNA_AcceptInput); // input�� �޾Ƶ��̴� node�� ������� ��

	// panel picking
	if (targetNode->PickPanel(panelBuffer, worldPoint, GetWorldDepth(), filterAttr))
	{
		nodeBuffer.Reserve(panelBuffer.GetSize());

		MK_INDEXING_LOOP(panelBuffer, i)
		{
			// MkWindowBaseNode �Ļ� ��ü�� ������� ��
			MkSceneNode* sceneNode = panelBuffer[i]->GetParentNode()->FindNearestDerivedNode(ePA_SNT_WindowBaseNode);
			if (sceneNode != NULL)
			{
				MkWindowBaseNode* winBaseNode = dynamic_cast<MkWindowBaseNode*>(sceneNode);
				if ((winBaseNode != NULL) && (nodeBuffer.FindFirstInclusion(MkArraySection(0), winBaseNode) == MKDEF_ARRAY_ERROR))
				{
					nodeBuffer.PushBack(winBaseNode);
				}
			}
		}
	}
	return !nodeBuffer.Empty();
}

bool MkWindowManagerNode::_PickWindowBaseNode
(const MkSceneNode* targetNode, MkArray<MkWindowBaseNode*>& nodeBuffer, MkArray< MkArray<MkHashStr> >& pathBuffer, const MkFloat2& worldPoint) const
{
	pathBuffer.Clear();

	if (_PickWindowBaseNode(targetNode, nodeBuffer, worldPoint))
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

void MkWindowManagerNode::_CheckAndRegisterWindowNode(const MkHashStr& name)
{
	MkSceneNode* childNode = GetChildNode(name);
	MK_CHECK(childNode != NULL, GetNodeName().GetString() + L" window mgr�� ������" + name.GetString() + L" window�� ����")
		return;

	MK_CHECK(childNode->IsDerivedFrom(ePA_SNT_WindowBaseNode), GetNodeName().GetString() + L" window mgr�� ���� " + name.GetString() + L" �� MkWindowBaseNode �Ļ� ��ü�� �ƴ�")
		return;

	MkWindowBaseNode* winNode = dynamic_cast<MkWindowBaseNode*>(childNode);
	MK_CHECK(winNode != NULL, GetNodeName().GetString() + L" window mgr�� ���� " + name.GetString() + L" �� MkWindowBaseNode �Ļ� ��ü�� �ƴ�")
		return;

	m_RootWindowList.Create(name, winNode);
}

void MkWindowManagerNode::_CloseExclusiveOpenningWindow(void)
{
	if (!m_ExclusiveOpenningWindow.Empty())
	{
		MkSceneNode* targetNode = GetChildNode(m_ExclusiveOpenningWindow);
		if (targetNode != NULL)
		{
			targetNode->SetVisible(false);
		}

		m_ExclusiveOpenningWindow.Clear();
		m_ExclusiveWindowException.Clear();
	}
}

//------------------------------------------------------------------------------------------------//
