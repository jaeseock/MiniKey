
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
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowManagerNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

bool MkWindowManagerNode::AttachWindow(MkWindowBaseNode* windowNode)
{
	MK_CHECK(windowNode != NULL, L"MkWindowManagerNode에 NULL window 등록 시도")
		return false;

	const MkHashStr& windowName = windowNode->GetNodeName();
	MK_CHECK(!m_AttachWindowLock, L"MkSceneNode::Update() 중에 " + windowName.GetString() + " window 등록 시도")
		return false;

	MK_CHECK(!ChildExist(windowName), L"MkWindowManagerNode에 이미 존재하는 " + windowName.GetString() + " window 등록 시도")
		return false;

	bool ok = AttachChildNode(windowNode);
	if (ok)
	{
		windowNode->SetVisible(false);
		windowNode->SetSkipUpdateWhileInvisible(true); // visible이 아니면 update 하지 않음

		m_RootWindowList.Create(windowName, windowNode);
	}
	return ok;
}

bool MkWindowManagerNode::DeleteWindow(const MkHashStr& windowName)
{
	bool ok = m_RootWindowList.Exist(windowName);
	if (ok)
	{
		// 현재 lock이 걸려 있으면 다음 Update()시 삭제
		if (m_DeleteWindowLock)
		{
			m_DeletingWindow.PushBack(windowName);
		}
		// 그렇지 않으면 바로 반영
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
				DeactivateWindow(path[0]); // path의 root를 닫음
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
					targetPath += buffer; // path에 추가 하위 경로를 더해 최종 경로를 만듬
				}

				buffer.Clear();
				if (argument->GetDataEx(MkWindowBaseNode::ArgKey_ExclusiveException, buffer))
				{
					exceptionPath = path;
					exceptionPath += buffer; // path에 추가 하위 경로를 더해 최종 경로를 만듬
				}
			}

			if ((!targetPath.Empty()) && (targetPath.GetSize() > 1)) // window manager나 root window가 아니고
			{
				MkSceneNode* targetNode = GetChildNode(targetPath);
				if ((targetNode != NULL) && targetNode->IsDerivedFrom(ePA_SNT_WindowBaseNode)) // window base 파생 객체면
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
				// ArgKey_ExclusiveException 정보는 표시하지 않음

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

	// 현 프레임에서 focusing 지정된 window
	// eLT_Normal, modal window 대상으로 active window들의 순서가 변경될 때 지정
	MkHashStr focusWindowName;
	bool clearAllFocus = false; // cursor click이 window 영역 외를 지정했을 경우

	// window depth 재설정 여부. deactive는 무시하고 active일때만 설정
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
			
			if (evt.activate) // 활성화. active/deactive -> active 가능(순서 변경)
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
					
					m_ActivatingWindows.Insert(0, windowName); // 0번 위치에 집어넣음

					updateWindowDepth = true;

					if (m_ModalWindow.Empty()) // 이미 modal window가 존재하면 포커싱은 되지 않음
					{
						focusWindowName = windowName;
					}
				}

				commandTarget.PushBack(winNode, true);
			}
			else // 비활성화. active -> deactive만 허용
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

	// scene portal legacy를 받지 않았으면 영역 갱신
	if (!m_HasScenePortalLegacy)
	{
		m_SystemRect.position.Clear();
		m_SystemRect.size = MK_DEVICE_MGR.GetCurrentResolution();
	}

	// cursor snapshot 생성
	MkInt2 cursorPosition = MK_INPUT_MGR.GetAbsoluteMousePosition(true) - m_SystemRect.position;
	MkInt2 cursorMovement = MK_INPUT_MGR.GetAbsoluteMouseMovement(true);

	eButtonState leftBtnState = MK_INPUT_MGR.GetMouseLeftButtonState();
	eButtonState middleBtnState = MK_INPUT_MGR.GetMouseMiddleButtonState();
	eButtonState rightBtnState = MK_INPUT_MGR.GetMouseRightButtonState();

	bool anyBtnPressed = (leftBtnState == eBS_Pressed) || (middleBtnState == eBS_Pressed) || (rightBtnState == eBS_Pressed);
	bool leftBtnPushing = MK_INPUT_MGR.GetMouseLeftButtonPushing();
	int wheelDelta = MK_INPUT_MGR.GetMouseWheelMovement();

	// system 영역 내에 있는지 여부 검사
	bool cursorInside = MK_INPUT_MGR.GetMousePointerAvailable();

	if (cursorInside && m_HasScenePortalLegacy) // scene portal legacy를 받았으면 입력 갱신 가능여부 체크
	{
		cursorInside = m_CursorIsInsideOfScenePortal;
		m_CursorIsInsideOfScenePortal = false;
	}
	
	if (cursorInside) // whole region 내에 있는지 체크
	{
		MkIntRect regionRect(MkInt2::Zero, m_SystemRect.size);
		cursorInside = regionRect.CheckIntersection(cursorPosition);
	}

	if (cursorInside && (MK_RENDERER.GetWebDialog() != NULL)) // renderer의 dialog 영역은 제외
	{
		const RECT& dr = MK_RENDERER.GetDialogRect(); // dialog 영역(top-down 좌표계)
		MkInt2 cp = MK_INPUT_MGR.GetAbsoluteMousePosition(false); // top-down 좌표계의 cursor position
		cursorInside = ((cp.x < dr.left) || (cp.x > dr.right) || (cp.y < dr.top) || (cp.x > dr.bottom));
	}

	m_HasScenePortalLegacy = false;

	// 현 프레임에서 cursor를 소유한 window 목록
	MkArray<MkWindowBaseNode*> cursorOwnedNodeBuffer;
	MkArray< MkArray<MkHashStr> > cursorOwnedPathBuffer;

	// LOCK!!!
	m_DeleteWindowLock = true;

	// cursor가 client 영역 안에 존재하면 cursor가 위치한 window를 picking으로 검출
	if (cursorInside)
	{
		// modal이 존재하면 해당 window만, 없으면 하위 전체 대상
		const MkSceneNode* pickTarget = this;
		if (!m_ModalWindow.Empty())
		{
			pickTarget = m_RootWindowList[m_ModalWindow];
		}

		MkFloat2 fCursorPos(static_cast<float>(cursorPosition.x), static_cast<float>(cursorPosition.y));
		_PickWindowBaseNode(pickTarget, cursorOwnedNodeBuffer, cursorOwnedPathBuffer, fCursorPos);
	}

	// 배타적 열림 상태 윈도우가 존재 할 경우 유지 조건을 만족시키지 못하면 닫음
	if (!m_ExclusiveOpenningWindow.Empty())
	{
		bool closeExclusiveOpenningWindow = true;
		if (m_ExclusiveOpenningWindow[0] == GetFrontWindowName()) // 소속 window가 front면
		{
			if (anyBtnPressed)
			{
				if (cursorInside && (!cursorOwnedNodeBuffer.Empty())) // 영역 안에서 hit가 발생했으면
				{
					MkSceneNode* targetNode = GetChildNode(m_ExclusiveOpenningWindow);
					MkSceneNode* exceptionNode = m_ExclusiveWindowException.Empty() ? NULL : GetChildNode(m_ExclusiveWindowException);
					
					if (targetNode != NULL)
					{
						MK_INDEXING_LOOP(cursorOwnedNodeBuffer, i)
						{
							MkSceneNode* hitNode = cursorOwnedNodeBuffer[i];

							// 대상/예외 node가 hit node 혹은 hit node의 조상이면 유지
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
				closeExclusiveOpenningWindow = false; // btn 입력이 없으면 유지
			}
		}

		if (closeExclusiveOpenningWindow)
		{
			_CloseExclusiveOpenningWindow();
		}
	}

	// 이전 프레임의 cursor owner 중 현재 프레임에 cursor를 잃은 window를 추출해 cursor lost 통지
	MkArray< MkArray<MkHashStr> > diffSets;
	m_CursorOwnedWindowPath.GetDifferenceOfSets(cursorOwnedPathBuffer, diffSets); // diffSets = m_CursorOwnedWindowPath - cursorOwnedPathBuffer

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

	MkEditBoxControlNode* targetEditBoxNode = NULL;

	// 현재 프레임에 cursor owner가 없으면
	if (cursorOwnedNodeBuffer.Empty())
	{
		clearAllFocus = anyBtnPressed;
	}
	// 현재 프레임에 cursor owner가 존재하면
	else
	{
		MK_INDEXING_LOOP(cursorOwnedNodeBuffer, i) // 통지
		{
			cursorOwnedNodeBuffer[i]->UpdateCursorInput(cursorPosition, cursorMovement, true, leftBtnState, middleBtnState, rightBtnState, wheelDelta);
		}

		// 만약 어느 button이라도 입력되었으면
		if (anyBtnPressed)
		{
			// 해당 window들을 입력 대상 window로 지정
			m_KeyInputTargetWindowPath = cursorOwnedPathBuffer;

			// 입력된 node가 edit box 계열일 경우 edit 대상 지정
			MK_INDEXING_LOOP(cursorOwnedNodeBuffer, i)
			{
				if (cursorOwnedNodeBuffer[i]->IsDerivedFrom(ePA_SNT_EditBoxControlNode))
				{
					targetEditBoxNode = dynamic_cast<MkEditBoxControlNode*>(cursorOwnedNodeBuffer[i]);
					break; // 최초 하나만 대상
				}
			}

			// picking된 node의 root window를 front로 올림. 최초 하나만 대상
			const MkHashStr& targetRootWindowName = cursorOwnedPathBuffer[0][0];
			if (m_RootWindowList.Exist(targetRootWindowName) &&
				(targetRootWindowName != m_ModalWindow)) // modal window는 이미 가장 앞에 위치하고 있으므로 무시
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

		// 영역 안 유효클릭 발생했는데 edit box가 아니면 focus 지정
		// renderer의 dialog window가 focus를 가지고 있을 경우 여기서 복원 됨
		if (cursorInside && (!MK_EDIT_BOX.IsBinding()))
		{
			HWND mainHandle = MK_DEVICE_MGR.GetTargetWindow()->GetWindowHandle();
			if (mainHandle != GetFocus())
			{
				::SetFocus(mainHandle);
			}
		}
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
	// focus window 갱신
	//------------------------------------------------------------------------------------------------//

	bool clearFocusingWindow = false;
	bool giveFocusToTarget = false;

	// 1st. modal window가 지정 됨
	if ((!focusWindowName.Empty()) && (focusWindowName == m_ModalWindow))
	{
		clearFocusingWindow = true;
		giveFocusToTarget = true;
	}
	// modal window가 존재하면 focus event는 무시됨
	else if (m_ModalWindow.Empty())
	{
		// 2nd. click으로 인한 focus 해제
		if (clearAllFocus)
		{
			clearFocusingWindow = true;
		}
		// 3rd. normal window 순서 변경으로 인한 지정
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
	// 깊이 대역폭 내 정렬
	//------------------------------------------------------------------------------------------------//

	// activating window
	if (updateWindowDepth && (!m_ActivatingWindows.Empty()))
	{
		float frontDepth = m_DepthBandwidth * 0.2f; // 20% ~ 100%
		float gridPerWindow = (m_DepthBandwidth - frontDepth) / static_cast<float>(m_ActivatingWindows.GetSize()); // root window간 깊이차
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
	// modal effect node 제외
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
	// window system 구성을 위해서는 하위 자식들이 필요
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

	// activation(역순)
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
	filterAttr.Set(ePA_SNA_AcceptInput); // input을 받아들이는 node만 대상으로 함

	// panel picking
	if (targetNode->PickPanel(panelBuffer, worldPoint, GetWorldDepth(), filterAttr))
	{
		nodeBuffer.Reserve(panelBuffer.GetSize());

		MK_INDEXING_LOOP(panelBuffer, i)
		{
			// MkWindowBaseNode 파생 객체만 대상으로 함
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
	MK_CHECK(childNode != NULL, GetNodeName().GetString() + L" window mgr의 하위에" + name.GetString() + L" window가 없음")
		return;

	MK_CHECK(childNode->IsDerivedFrom(ePA_SNT_WindowBaseNode), GetNodeName().GetString() + L" window mgr의 하위 " + name.GetString() + L" 는 MkWindowBaseNode 파생 객체가 아님")
		return;

	MkWindowBaseNode* winNode = dynamic_cast<MkWindowBaseNode*>(childNode);
	MK_CHECK(winNode != NULL, GetNodeName().GetString() + L" window mgr의 하위 " + name.GetString() + L" 는 MkWindowBaseNode 파생 객체가 아님")
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
