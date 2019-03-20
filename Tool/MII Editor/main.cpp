
//------------------------------------------------------------------------------------------------//
// single/multi-thread rendering application sample
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkCheck.h"
#include "MkCore_MkPageManager.h"
#include "MkCore_MkInputManager.h"
//#include "MkCore_MkTimeManager.h"
#include "MkCore_MkTimeState.h"
#include "MkCore_MkWin32Application.h"
#include "MkCore_MkProfilingManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"
//#include "MkCore_MkUniformDice.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkRenderFramework.h"
#include "MkPA_MkRenderer.h"
#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkWindowManagerNode.h"
#include "MkPA_MkTitleBarControlNode.h"
#include "MkPA_MkEditBoxControlNode.h"
#include "MkPA_MkCheckBoxControlNode.h"
#include "MkPA_MkDropDownListControlNode.h"
#include "MkPA_MkDrawSceneNodeStep.h"
#include "MkPA_MkWindowFactory.h"

#include "MinimapWindowBodyNode.h"
#include "SystemWindowBodyNode.h"
#include "SequenceWindowBodyNode.h"
#include "AnimationViewWindowNode.h"


#define APPDEF_SYSTEM_UI_WIDTH 256.f
#define APPDEF_SYSTEM_UI_BORDER_SIZE MkFloat2(6.f, 6.f)
#define APPDEF_IMAGE_BORDER_SIZE MkFloat2(100.f, 100.f)

#define APPDEF_DARK_SUBSET_GUIDE_COLOR MkColor(0.6f, 0.f, 0.f)
#define APPDEF_BRIGHT_SUBSET_GUIDE_COLOR MkColor(1.f, 0.f, 0.f)



// MkImageInfo와 동일 key
const static MkHashStr GROUP_KEY = L"__#Group";

const static MkHashStr POSITION_KEY = L"Position";
const static MkHashStr SIZE_KEY = L"Size";
const static MkHashStr TABLE_KEY = L"Table";
const static MkHashStr OFFSET_KEY = L"Offset";

const static MkHashStr TOTAL_RANGE_KEY = L"TotalRange";
const static MkHashStr SUBSET_LIST_KEY = L"SubsetList";
const static MkHashStr TIME_LIST_KEY = L"TimeList";
const static MkHashStr PIVOT_KEY = L"Pivot";
const static MkHashStr LOOP_KEY = L"Loop";


//------------------------------------------------------------------------------------------------//

class EditPage : public MkBasePage
{
public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		// root
		m_SceneRootNode = new MkSceneNode(L"<Root>");

		// background
		m_MainImageNode = NULL;

		// subset guide
		m_SubsetGuideNode = m_SceneRootNode->CreateChildNode(L"<SubseGuide>");
		m_SubsetGuideNode->SetLocalDepth(9000.f);

		// window mgr
		m_WindowMgrNode = MkWindowManagerNode::CreateChildNode(NULL, L"<WinMgr>");
		m_WindowMgrNode->SetDepthBandwidth(1000.f);

		// window : system
		{
			MkTitleBarControlNode* titleBar = MkTitleBarControlNode::CreateChildNode(NULL, L"<System>");
			titleBar->SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, APPDEF_SYSTEM_UI_WIDTH, true);
			titleBar->SetIcon(MkWindowThemeData::eIT_Notice);
			titleBar->SetCaption(L"시스템", eRAP_LeftCenter);
			titleBar->SetAlignmentPosition(eRAP_RightTop);
			titleBar->SetAlignmentOffset(-APPDEF_SYSTEM_UI_BORDER_SIZE);

			m_WindowMgrNode->AttachWindow(titleBar);
			m_WindowMgrNode->ActivateWindow(L"<System>");

			// body frame
			SystemWindowBodyNode* bodyNode = new SystemWindowBodyNode(L"BodyFrame");
			titleBar->AttachChildNode(bodyNode);
			MkFloat2 bodySize(APPDEF_SYSTEM_UI_WIDTH, 346.f);
			bodyNode->SetBodyFrame(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, bodySize);
			bodyNode->CreateControls();
		}
		
		// window : sequence
		{
			MkTitleBarControlNode* titleBar = MkTitleBarControlNode::CreateChildNode(NULL, L"<Sequence>");
			titleBar->SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 1000.f, true);
			titleBar->SetIcon(MkWindowThemeData::eIT_EditMode);
			titleBar->SetCaption(L"시퀀스 편집", eRAP_LeftCenter);
			titleBar->SetAlignmentPosition(eRAP_MiddleTop);
			titleBar->SetAlignmentOffset(MkFloat2(0.f, -40.f));
			m_WindowMgrNode->AttachWindow(titleBar);

			// body frame
			SequenceWindowBodyNode* bodyNode = new SequenceWindowBodyNode(L"BodyFrame");
			titleBar->AttachChildNode(bodyNode);
			MkFloat2 bodySize(1000.f, 680.f);
			bodyNode->SetBodyFrame(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, bodySize);
			bodyNode->CreateControls();
		}
		
		// draw step
		m_DrawStep = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep(L"Scene");
		m_DrawStep->SetSceneNode(m_SceneRootNode);
		m_DrawStep->SetBackgroundColor(MkColor::Blue);
		m_DrawStep->SetClearLastRenderTarget(true);

		m_LastDrawSize = m_DrawStep->GetRegionRect().size;

		MkDrawSceneNodeStep* winMgrStep = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep(L"WinMgr");
		winMgrStep->SetSceneNode(m_WindowMgrNode);

		// finish
		m_SceneRootNode->Update();
		m_WindowMgrNode->Update();

		//MK_DEV_PANEL.MsgToFreeboard(0, L"Enter 키 : 시스템 윈도우 토글");
		//MK_DEV_PANEL.MsgToFreeboard(1, L"방향키 : 포커스 이동");
		//MK_DEV_PANEL.MsgToFreeboard(3, L"월드 크기 : " + MkStr(bgPanel.GetPanelSize().x) + L" * " + MkStr(bgPanel.GetPanelSize().y));
		//MK_DEV_PANEL.MsgToFreeboard(4, L"오브젝트 수 : " + MkStr(m_CurrBallCount));
		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		if ((m_SceneRootNode != NULL) && (m_WindowMgrNode != NULL) && (m_DrawStep != NULL))
		{
			// window size change?
			MkFloat2 currDrawSize = m_DrawStep->GetRegionRect().size;
			if (currDrawSize != m_LastDrawSize)
			{
				MkFloat2 offset = (m_LastDrawSize - currDrawSize) * 0.5f;
				MkFloatRect rect(m_DrawStep->GetCameraOffset(), m_LastDrawSize);
				_UpdateCamera(rect.GetCenterPosition() + MkFloat2(-offset.x, offset.y));

				m_LastDrawSize = currDrawSize;
			}

			// toggle windows
			if (MK_INPUT_MGR.GetKeyPressed(VK_F12))
			{
				if (m_WindowMgrNode->IsActivating(L"<Minimap>") ||
					m_WindowMgrNode->IsActivating(L"<System>"))
				{
					m_WindowMgrNode->DeactivateWindow(L"<Minimap>");
					m_WindowMgrNode->DeactivateWindow(L"<System>");
				}
				else
				{
					m_WindowMgrNode->ActivateWindow(L"<Minimap>");
					m_WindowMgrNode->ActivateWindow(L"<System>");
				}
			}

			bool seqWindowIsOff = !m_WindowMgrNode->IsActivating(L"<Sequence>");

			if (seqWindowIsOff && (m_MainImageNode != NULL) && (m_SubsetGuideNode != NULL) && (m_ImageInfoPtr != NULL))
			{
				if (MK_INPUT_MGR.GetKeyPressed(VK_DELETE))
				{
					// focus subset 삭제
					if (!m_OnFocusSubset.Empty())
					{
						// sequence에 등록된 subset이면 삭제 불가
						MkStr refSeqName;
						MkConstHashMapLooper<MkHashStr, MkImageInfo::Sequence> seqLooper(m_ImageInfoPtr->GetSequences());
						MK_STL_LOOP(seqLooper)
						{
							MkConstMapLooper<double, MkHashStr> trackLooper(seqLooper.GetCurrentField().tracks);
							MK_STL_LOOP(trackLooper)
							{
								if (trackLooper.GetCurrentField() == m_OnFocusSubset)
								{
									refSeqName = seqLooper.GetCurrentKey().GetString();
									break;
								}
							}
							if (!refSeqName.Empty())
								break;
						}

						if (refSeqName.Empty())
						{
							m_ImageInfoPtr->RemoveSubset(m_OnFocusSubset);

							_UpdateSubsetGuideNode(m_OnFocusSubset, false, false);

							if (m_OnCursorSubset == m_OnFocusSubset)
							{
								m_OnCursorSubset.Clear();
							}
							m_OnFocusSubset.Clear();
							m_OnFocusEdge = 0;
							_UpdateFocusSubsetInfoUI();
						}
						else
							_CreateModalMessageBox(L"해당 서브셋은 [" + refSeqName + L"]\r\n시퀀스에 참조되어 있어 삭제가 불가능합니다");
					}
				}

				if (!m_OnFocusSubset.Empty())
				{
					const int velocity = 1;
					MkIntRect offset;
					DWORD lastEdge = m_OnFocusEdge;

					if (MK_INPUT_MGR.GetKeyPushing(VK_SHIFT))
					{
						// shift 상태라면 한 변만 선택해 증감
						if (m_OnFocusEdge == 0)
						{
							if (MK_INPUT_MGR.GetKeyPressed(VK_LEFT)) m_OnFocusEdge = VK_LEFT;
							if (MK_INPUT_MGR.GetKeyPressed(VK_RIGHT)) m_OnFocusEdge = VK_RIGHT;
							if (MK_INPUT_MGR.GetKeyPressed(VK_UP)) m_OnFocusEdge = VK_UP;
							if (MK_INPUT_MGR.GetKeyPressed(VK_DOWN)) m_OnFocusEdge = VK_DOWN;
						}
						else
						{
							switch (m_OnFocusEdge)
							{
							case VK_LEFT:
								if (MK_INPUT_MGR.GetKeyPressed(VK_LEFT)) { offset.position.x -= velocity; offset.size.x += velocity; }
								if (MK_INPUT_MGR.GetKeyPressed(VK_RIGHT)) { offset.position.x += velocity; offset.size.x -= velocity; }
								if (MK_INPUT_MGR.GetKeyPressed(VK_UP)) m_OnFocusEdge = VK_UP;
								if (MK_INPUT_MGR.GetKeyPressed(VK_DOWN)) m_OnFocusEdge = VK_DOWN;
								break;
							case VK_UP:
								if (MK_INPUT_MGR.GetKeyPressed(VK_LEFT)) m_OnFocusEdge = VK_LEFT;
								if (MK_INPUT_MGR.GetKeyPressed(VK_RIGHT)) m_OnFocusEdge = VK_RIGHT;
								if (MK_INPUT_MGR.GetKeyPressed(VK_UP)) { offset.position.y -= velocity; offset.size.y += velocity; }
								if (MK_INPUT_MGR.GetKeyPressed(VK_DOWN)) { offset.position.y += velocity; offset.size.y -= velocity; }
								break;
							case VK_RIGHT:
								if (MK_INPUT_MGR.GetKeyPressed(VK_LEFT)) { offset.size.x -= velocity; }
								if (MK_INPUT_MGR.GetKeyPressed(VK_RIGHT)) { offset.size.x += velocity; }
								if (MK_INPUT_MGR.GetKeyPressed(VK_UP)) m_OnFocusEdge = VK_UP;
								if (MK_INPUT_MGR.GetKeyPressed(VK_DOWN)) m_OnFocusEdge = VK_DOWN;
								break;
							case VK_DOWN:
								if (MK_INPUT_MGR.GetKeyPressed(VK_LEFT)) m_OnFocusEdge = VK_LEFT;
								if (MK_INPUT_MGR.GetKeyPressed(VK_RIGHT)) m_OnFocusEdge = VK_RIGHT;
								if (MK_INPUT_MGR.GetKeyPressed(VK_UP)) { offset.size.y -= velocity; }
								if (MK_INPUT_MGR.GetKeyPressed(VK_DOWN)) { offset.size.y += velocity; }
								break;
							}
						}
					}
					else
					{
						// control 상태라면 전체 크기를 증감
						if (MK_INPUT_MGR.GetKeyPushing(VK_CONTROL))
						{
							m_OnFocusEdge = 1;

							if (MK_INPUT_MGR.GetKeyPressed(VK_UP))
							{
								offset.position.x -= velocity;
								offset.position.y -= velocity;
								offset.size.x += velocity * 2;
								offset.size.y += velocity * 2;
							}
							if (MK_INPUT_MGR.GetKeyPressed(VK_DOWN))
							{
								offset.position.x += velocity;
								offset.position.y += velocity;
								offset.size.x -= velocity * 2;
								offset.size.y -= velocity * 2;
							}
						}
						else
						{
							m_OnFocusEdge = 0;

							// 그냥 이동
							if (MK_INPUT_MGR.GetKeyPressed(VK_LEFT)) { offset.position.x -= velocity; }
							if (MK_INPUT_MGR.GetKeyPressed(VK_RIGHT)) { offset.position.x += velocity; }
							if (MK_INPUT_MGR.GetKeyPressed(VK_UP)) { offset.position.y -= velocity; }
							if (MK_INPUT_MGR.GetKeyPressed(VK_DOWN)) { offset.position.y += velocity; }
						}
					}

					if ((lastEdge != m_OnFocusEdge) || (!offset.position.IsZero()) || (!offset.size.IsZero()))
					{
						MkIntRect rect;
						_GetSubsetRect(m_OnFocusSubset, rect);
						rect.position += offset.position;
						rect.size += offset.size;

						m_ImageInfoPtr->UpdateSubset(m_OnFocusSubset, rect, m_ImageSize);

						_UpdateSubsetGuideNode(m_OnFocusSubset, false, true);
						_UpdateFocusSubsetInfoUI();
					}
				}

				// update main scene
				MkInt2 currenCursorPos = MK_INPUT_MGR.GetAbsoluteMousePosition(true);
				MkFloat2 screenCursorPos = MkFloat2(static_cast<float>(currenCursorPos.x), static_cast<float>(currenCursorPos.y));
				MkFloat2 worldCursorPos = m_DrawStep->GetCameraOffset() + screenCursorPos;
				bool cursorOnUI = (m_WindowMgrNode->PickPanel(screenCursorPos) != NULL); // 커서가 ui 내부에 있는지?

				if (cursorOnUI)
				{
					// minimap을 통한 camera 이동
					MinimapWindowBodyNode* bodyNode = _GetMinimapWindowBodyNode();
					if ((bodyNode != NULL) && (bodyNode->IsHitting()))
					{
						const MkFloat2& hitPos = bodyNode->GetHitPosition();
						const MkFloat2& worldSize = m_MainImageNode->GetPanel(L"Image")->GetPanelSize();
						MkFloat2 pickPos(worldSize.x * hitPos.x, worldSize.y * hitPos.y);
						_UpdateCamera(pickPos);
					}

					_EndOfLeftDrag();
				}
				else
				{
					if (MK_INPUT_MGR.GetMousePointerAvailable())
					{
						// left drag 처리
						if (m_CursorDragState == eCDS_None)
						{
							if (MK_INPUT_MGR.GetMouseLeftButtonDoubleClicked() && MK_INPUT_MGR.GetKeyPushing(VK_SHIFT)) // shift & double click
							{
								MkIntRect rect;
								if (_FindSubsetRegion(worldCursorPos, rect))
								{
									_AddNewSubsetFromSelection(_GetNoNameString(), rect.position, rect.size);
								}
							}
							else if (MK_INPUT_MGR.GetMouseLeftButtonPressed()) // left drag start
							{
								m_LeftCursorDragPosBuffer[0] = screenCursorPos;
								m_CursorDragState = MK_INPUT_MGR.GetKeyPushing(VK_SHIFT) ? eCDS_Selection : eCDS_CameraMove;

								if (m_CursorDragState == eCDS_Selection)
								{
									m_LeftCursorDragPosBuffer[1] = worldCursorPos;
								}
								else if (m_CursorDragState == eCDS_CameraMove)
								{
									m_LeftCursorDragPosBuffer[1] = m_DrawStep->GetCameraOffset() + m_DrawStep->GetRegionRect().size * 0.5f;
								}
							}
						}
						else
						{
							if (MK_INPUT_MGR.GetMouseLeftButtonPushing()) // left draging
							{
								if (m_CursorDragState == eCDS_Selection) // drag를 통한 selection
								{
									// guide
									MkLineShape& srLine = m_SubsetGuideNode->CreateLine(L"DragGuide");
									srLine.SetColor(APPDEF_BRIGHT_SUBSET_GUIDE_COLOR);
									srLine.SetLocalDepth(-2.f);
									srLine.Clear();

									MkFloat2 LB = m_LeftCursorDragPosBuffer[1];
									LB.CompareAndKeepMin(worldCursorPos);
									MkFloat2 RT = m_LeftCursorDragPosBuffer[1];
									RT.CompareAndKeepMax(worldCursorPos);
									MkFloat2 LT(LB.x, RT.y);

									MkArray<MkFloat2> lineVertices(5);
									lineVertices.PushBack(LB);
									lineVertices.PushBack(LT);
									lineVertices.PushBack(RT);
									lineVertices.PushBack(MkFloat2(RT.x, LB.y));
									lineVertices.PushBack(LB);
									srLine.AddLine(lineVertices);

									// position, size
									MkPanel& infoPanel = m_SubsetGuideNode->PanelExist(L"DragSelInfo") ? (*m_SubsetGuideNode->GetPanel(L"DragSelInfo")) : m_SubsetGuideNode->CreatePanel(L"DragSelInfo");
									MkStr msg;
									msg.Reserve(32);
									msg += MkInt2(static_cast<int>(LT.x), static_cast<int>(LT.y));
									msg += L" / ";
									msg += MkInt2(static_cast<int>(RT.x - LB.x), static_cast<int>(RT.y - LB.y));
									infoPanel.SetTextMsg(msg);
									infoPanel.SetLocalPosition(LT + MkFloat2(0.f, 3.f));
								}
								else if (m_CursorDragState == eCDS_CameraMove)
								{
									_UpdateCamera(m_LeftCursorDragPosBuffer[1] - screenCursorPos + m_LeftCursorDragPosBuffer[0]); // drag를 통한 camera 이동
								}
							}
							else if (MK_INPUT_MGR.GetMouseLeftButtonReleased()) // left drag end
							{
								if (m_CursorDragState == eCDS_Selection)
								{
									MkIntRect rect;
									if (_FindSubsetRegion(m_LeftCursorDragPosBuffer[1], worldCursorPos, rect))
									{
										_AddNewSubsetFromSelection(_GetNoNameString(), rect.position, rect.size);
									}
								}
								
								_EndOfLeftDrag();
							}
						}
					}
					else
					{
						_EndOfLeftDrag();
					}
				}

				// subset guide
				if (MK_INPUT_MGR.GetMousePointerAvailable())
				{
					// update subset guide
					MkInt2 cursorPt(static_cast<int>(worldCursorPos.x), static_cast<int>(worldCursorPos.y));
					MkInt2 borderSize(static_cast<int>(APPDEF_IMAGE_BORDER_SIZE.x), static_cast<int>(APPDEF_IMAGE_BORDER_SIZE.y));

					// cursor를 가진 subset 찾음
					MkHashStr onCursorSubset;
					if (!cursorOnUI)
					{
						MkFocusProfiler _fp(L"FIND", false);

						MkArray<MkHashStr> subsets;
						m_ImageInfoPtr->GetSubsets().GetKeyList(subsets);
						MK_INDEXING_LOOP(subsets, i)
						{
							const MkHashStr& ssName = subsets[i];
							if (ssName.Empty())
								continue;

							MkIntRect rect;
							_GetSubsetRect(subsets[i], rect);
							MkIntRect subsetRegion(MkInt2(rect.position.x, m_ImageSize.y - rect.position.y - rect.size.y) + borderSize, rect.size);
							if (subsetRegion.CheckIntersection(cursorPt))
							{
								onCursorSubset = subsets[i];
								break;
							}
						}
					}

					if ((!cursorOnUI) && MK_INPUT_MGR.GetMouseLeftButtonPressed())
					{
						if ((!m_OnFocusSubset.Empty()) && (onCursorSubset.Empty() || (onCursorSubset != m_OnFocusSubset)))
						{
							_UpdateSubsetGuideNode(m_OnFocusSubset, false, false);
							m_OnFocusSubset.Clear();
							m_OnFocusEdge = 0;
							_UpdateFocusSubsetInfoUI();
						}

						if ((!onCursorSubset.Empty()) && (onCursorSubset != m_OnFocusSubset))
						{
							m_OnFocusSubset = onCursorSubset;
							m_OnFocusEdge = 0;
							_UpdateSubsetGuideNode(m_OnFocusSubset, false, true);
							_UpdateFocusSubsetInfoUI();
						}
					}

					if (onCursorSubset != m_OnCursorSubset)
					{
						if ((!m_OnCursorSubset.Empty()) && (m_OnCursorSubset != m_OnFocusSubset))
						{
							_UpdateSubsetGuideNode(m_OnCursorSubset, false, false);
						}

						m_OnCursorSubset = onCursorSubset;
						m_OnFocusEdge = 0;

						if ((!m_OnCursorSubset.Empty()) && (m_OnCursorSubset != m_OnFocusSubset))
						{
							_UpdateSubsetGuideNode(m_OnCursorSubset, true, false);
						}
					}
				}
			}

			// event
			SystemWindowBodyNode* systemWindow = _GetSystemWindowBodyNode();
			if (systemWindow != NULL)
			{
				MkBitField32 events = systemWindow->ConsumeEvent();

				if (events.Check(SystemWindowBodyNode::eWE_Load))
				{
					MkArray<MkStr> exts(2);
					exts.PushBack(L"png");
					exts.PushBack(MKDEF_PA_IMAGE_INFO_FILE_EXT);
					MkPathName filePath;
					if (filePath.GetSingleFilePathFromDialog(exts, m_hWnd))
					{
						LoadImageFile(filePath);
					}
				}
				if (events.Check(SystemWindowBodyNode::eWE_Reload) && (m_MainImageNode != NULL))
				{
					_ClearMainImage();
					_LoadMainImage(m_CurrentImageFilePath);
				}
				if (events.Check(SystemWindowBodyNode::eWE_SaveMII))
				{
					_SaveCurrentImageInfo();
				}
				if (events.Check(SystemWindowBodyNode::eWE_ApplyGroupName))
				{
					MkEditBoxControlNode* winNode = dynamic_cast<MkEditBoxControlNode*>(systemWindow->GetChildNode(L"GroupNameEB"));
					if (winNode != NULL)
					{
						m_ImageInfoPtr->SetGroup(winNode->GetText());
					}
				}
				if (events.Check(SystemWindowBodyNode::eWE_ApplySubsetName))
				{
					if (!m_OnFocusSubset.Empty())
					{
						MkEditBoxControlNode* winNode = dynamic_cast<MkEditBoxControlNode*>(systemWindow->GetChildNode(L"SubsetNameEB"));
						if (winNode != NULL)
						{
							MkHashStr subsetName = winNode->GetText();
							if ((!subsetName.Empty()) && (subsetName != m_OnFocusSubset.GetString()))
							{
								if ((!m_ImageInfoPtr->GetSubsets().Exist(subsetName)) && (!m_ImageInfoPtr->GetSequences().Exist(subsetName)))
								{
									m_ImageInfoPtr->ChangeSubset(m_OnFocusSubset, subsetName);
									_UpdateSubsetGuideNode(m_OnFocusSubset, false, false);
									_UpdateSubsetGuideNode(subsetName, false, true);
									m_OnFocusSubset = subsetName;
								}
								else
									_CreateModalMessageBox(L"이미 존재하는 이름입니다");
							}
						}
					}
					else
						_CreateModalMessageBox(L"지정된 서브셋이 없습니다");
				}
				if (events.Check(SystemWindowBodyNode::eWE_ApplySubsetRect))
				{
					if (!m_OnFocusSubset.Empty())
					{
						MkIntRect rect;
						systemWindow->GetSubsetRect(rect);
						m_ImageInfoPtr->UpdateSubset(m_OnFocusSubset, rect, m_ImageSize);
						_UpdateSubsetGuideNode(m_OnFocusSubset, false, true);
					}
					else
						_CreateModalMessageBox(L"지정된 서브셋이 없습니다");
				}
				if (events.Check(SystemWindowBodyNode::eWE_AddSeqName))
				{
					if (!m_OnFocusSubset.Empty())
					{
						const MkHashStr& seqName = systemWindow->GetTargetSequenceStr()[0];
						if ((!m_ImageInfoPtr->GetSubsets().Exist(seqName)) && (!m_ImageInfoPtr->GetSequences().Exist(seqName)))
						{
							MkDropDownListControlNode* listNode = dynamic_cast<MkDropDownListControlNode*>(systemWindow->GetChildNode(L"SeqDDList"));
							if (listNode != NULL)
							{
								listNode->AddItem(seqName, seqName.GetString());
								listNode->SetTargetItemKey(seqName);
								listNode->SortItemSequenceInAscendingOrder();
							}

							MkImageInfo::Sequence initSeq;
							initSeq.totalRange = 1.;
							initSeq.pivot = eRAP_LeftBottom;
							initSeq.loop = true;
							initSeq.tracks.Create(0.f, m_OnFocusSubset);
							m_ImageInfoPtr->UpdateSequence(seqName, initSeq);

							if (m_WindowMgrNode->IsActivating(L"<PlaySeq>"))
							{
								_CreateAniviewWindow(seqName);
							}
						}
						else
							_CreateModalMessageBox(L"이미 존재하는 이름입니다");
					}
					else
						_CreateModalMessageBox(L"지정된 서브셋이 없습니다");
				}
				if (events.Check(SystemWindowBodyNode::eWE_ApplySeqName))
				{
					const MkHashStr& delSeqName = systemWindow->GetTargetSequenceStr()[0];
					const MkHashStr& newSeqName = systemWindow->GetTargetSequenceStr()[1];
					if (m_ImageInfoPtr->GetSequences().Exist(delSeqName) && (!m_ImageInfoPtr->GetSubsets().Exist(newSeqName)) && (!m_ImageInfoPtr->GetSequences().Exist(newSeqName)))
					{
						MkDropDownListControlNode* listNode = dynamic_cast<MkDropDownListControlNode*>(systemWindow->GetChildNode(L"SeqDDList"));
						if (listNode != NULL)
						{
							listNode->RemoveItem(delSeqName);
							listNode->AddItem(newSeqName, newSeqName.GetString());
							listNode->SetTargetItemKey(newSeqName);
							listNode->SortItemSequenceInAscendingOrder();
						}

						m_ImageInfoPtr->ChangeSequence(delSeqName, newSeqName);

						if (m_WindowMgrNode->IsActivating(L"<PlaySeq>"))
						{
							_CreateAniviewWindow(newSeqName, true);
						}
					}
				}
				if (events.Check(SystemWindowBodyNode::eWE_EditCurrSeq))
				{
					const MkHashStr& seqName = systemWindow->GetTargetSequenceStr()[0];
					if (m_ImageInfoPtr->GetSequences().Exist(seqName))
					{
						SequenceWindowBodyNode* seqNode = _GetSequenceWindowBodyNode();
						if (seqNode != NULL)
						{
							seqNode->WakeUp(m_CurrentImageFilePath, seqName);

							m_WindowMgrNode->DeleteWindow(L"<PlaySeq>");
							m_WindowMgrNode->ActivateWindow(L"<Sequence>", true);
						}
					}
				}
				if (events.Check(SystemWindowBodyNode::eWE_DelCurrSeq))
				{
					m_ImageInfoPtr->RemoveSequence(systemWindow->GetTargetSequenceStr()[0]);
					m_WindowMgrNode->DeleteWindow(L"<PlaySeq>");
				}
				if (events.Check(SystemWindowBodyNode::eWE_ToggleCurrSeq))
				{
					if (m_WindowMgrNode->IsActivating(L"<PlaySeq>"))
					{
						m_WindowMgrNode->DeleteWindow(L"<PlaySeq>");
					}
					else
					{
						_CreateAniviewWindow(systemWindow->GetTargetSequenceStr()[0]);
					}
				}
			}

			// update scene
			m_SceneRootNode->Update(timeState.fullTime);
			m_WindowMgrNode->Update(timeState.fullTime);
		}
	}

	virtual void Clear(MkDataNode* sharingNode = NULL)
	{
		m_CurrentImageFilePath.Clear();
		m_CurrentImageInfoPath.Clear();
		m_ImageSize.Clear();
		m_SourcePixel.Clear();
		
		MK_DELETE(m_SceneRootNode);
		m_MainImageNode = NULL;
		m_SubsetGuideNode = NULL;

		MK_DELETE(m_WindowMgrNode);
		
		MK_RENDERER.GetDrawQueue().Clear();
		m_DrawStep = NULL;
	}

	inline void SetHWND(HWND hWnd) { m_hWnd = hWnd; }

	void LoadImageFile(const MkPathName& filePath)
	{
		if (m_MainImageNode != NULL)
		{
			int result = ::MessageBox(m_hWnd, L"기존에 편집중인 이미지가 존재합니다.\r\n편집중인 이미지를 저장할까요?", L"주의", MB_YESNOCANCEL);
			if (result == IDYES)
			{
				_SaveCurrentImageInfo();
				_ClearMainImage();
			}
			else if (result == IDNO)
			{
				_ClearMainImage();
			}
			else // cancel
				return;
		}

		_LoadMainImage(filePath);
	}

	EditPage(const MkHashStr& name) : MkBasePage(name)
	{
		m_hWnd = NULL;

		m_SceneRootNode = NULL;
		m_MainImageNode = NULL;
		m_SubsetGuideNode = NULL;
		m_WindowMgrNode = NULL;
		m_DrawStep = NULL;
		m_ImageInfoPtr = NULL;

		m_AutoIncCounter = 0;
	}

	virtual ~EditPage() { Clear(); }

	//------------------------------------------------------------------------------------------------//

protected:

	void _CreateModalMessageBox(const MkStr& message)
	{
		if (m_WindowMgrNode != NULL)
		{
			const MkHashStr WindowName = L"<ModalMsgBox>";
			m_WindowMgrNode->DeleteWindow(WindowName);

			MkWindowFactory wndFactory;
			MkWindowBaseNode* msgBoxNode = wndFactory.CreateMessageBox(WindowName, L"우효~", message, NULL, MkWindowFactory::eMBT_Warning, MkWindowFactory::eMBB_None);
			msgBoxNode->SetAlignmentPosition(eRAP_MiddleCenter);
			m_WindowMgrNode->AttachWindow(msgBoxNode);
			m_WindowMgrNode->ActivateWindow(WindowName, true);
		}
	}

	//------------------------------------------------------------------------------------------------//

	bool _LoadMainImage(const MkPathName& filePath)
	{
		if ((m_SceneRootNode == NULL) ||
			(m_MainImageNode != NULL) ||
			(m_SubsetGuideNode == NULL) ||
			filePath.IsDirectoryPath() ||
			(!filePath.CheckAvailable()))
			return false;

		MkPathName path;
		MkStr name, extension;
		filePath.SplitPath(path, name, extension);
		extension.ToLower();

		// png/mii 경로 생성
		MkPathName imagePath;
		MkPathName miiPath;

		if (extension.Equals(0, L"png"))
		{
			imagePath = filePath;
			miiPath = path;
			miiPath += name;
			miiPath += L".";
			miiPath += MKDEF_PA_IMAGE_INFO_FILE_EXT;
		}
		else if (extension.Equals(0, MKDEF_PA_IMAGE_INFO_FILE_EXT))
		{
			imagePath = path;
			imagePath += name;
			imagePath += L".png";

			if (!imagePath.CheckAvailable()) // png 파일이 반드시 존재해야 함
				return false;

			miiPath = filePath;
		}

		do
		{
			m_MainImageNode = m_SceneRootNode->CreateChildNode(L"<MainImage>");
			if (m_MainImageNode == NULL)
				break;

			m_MainImageNode->SetLocalDepth(9999.f);

			// 출력 패널 생성
			MkPanel& panel = m_MainImageNode->CreatePanel(L"Image");
			panel.SetLocalPosition(APPDEF_IMAGE_BORDER_SIZE);

			// 실제 이미지 로드
			if (!panel.SetTexture(imagePath))
				break;

			MkBaseTexture* texture = MK_BITMAP_POOL.GetBitmapTexture(imagePath);
			if (texture == NULL)
				break;

			m_ImageSize = texture->GetSize();
			m_ImageInfoPtr = &texture->GetImageInfo();

			// 유효 픽셀 검출
			MkArray<D3DCOLOR> pixelData;
			if (!texture->GetPixelTable(pixelData))
				break;

			// alpha값 존재에 따라 사용 가능한 픽셀들을 골라냄
			m_SourcePixel.Fill(pixelData.GetSize());

			MK_INDEXING_LOOP(pixelData, i)
			{
				m_SourcePixel[i] = (((pixelData[i] >> 24) & 0xff) > 0);
			}
			pixelData.Clear();
			
			m_CurrentImageFilePath = imagePath;
			m_CurrentImageInfoPath = miiPath;

			// 미니맵 윈도우 생성
			_CreateMinimapWindow(imagePath, panel.GetTextureSize());

			// 카메라 초기화. 좌상단 기준
			_UpdateCamera(MkFloat2(APPDEF_IMAGE_BORDER_SIZE.x, APPDEF_IMAGE_BORDER_SIZE.y + panel.GetTextureSize().y));

			// image info 처리
			SystemWindowBodyNode* systemWindow = _GetSystemWindowBodyNode();
			if (systemWindow == NULL)
				break;

			MkDropDownListControlNode* seqDDLNode = dynamic_cast<MkDropDownListControlNode*>(systemWindow->GetChildNode(L"SeqDDList"));
			if (seqDDLNode == NULL)
				break;

			// group name
			MkStr groupName;
			if (systemWindow != NULL)
			{
				MkEditBoxControlNode* winNode = dynamic_cast<MkEditBoxControlNode*>(systemWindow->GetChildNode(L"GroupNameEB"));
				if (winNode != NULL)
				{
					winNode->SetText(m_ImageInfoPtr->GetGroup().GetString());
				}
			}

			// subset
			MkConstHashMapLooper<MkHashStr, MkImageInfo::Subset> sslooper(m_ImageInfoPtr->GetSubsets());
			MK_STL_LOOP(sslooper)
			{
				if (!sslooper.GetCurrentKey().Empty())
				{
					_UpdateSubsetGuideNode(sslooper.GetCurrentKey(), false, false);
				}
			}
			
			// sequence
			MkConstHashMapLooper<MkHashStr, MkImageInfo::Sequence> sqlooper(m_ImageInfoPtr->GetSequences());
			MK_STL_LOOP(sqlooper)
			{
				seqDDLNode->AddItem(sqlooper.GetCurrentKey(), sqlooper.GetCurrentKey().GetString());
			}
			seqDDLNode->SortItemSequenceInAscendingOrder();

			return true;
		}
		while (false);

		_ClearMainImage();
		return false;
	}

	void _UpdateSubsetGuideNode(const MkHashStr& name, bool hasCursor, bool hasFocus)
	{
		if ((m_SubsetGuideNode == NULL) || (m_ImageInfoPtr == NULL))
			return;

		if (m_ImageInfoPtr->GetSubsets().Exist(name))
		{
			MkIntRect rect;
			_GetSubsetRect(name, rect);

			MkSceneNode* subsetNode = m_SubsetGuideNode->ChildExist(name) ? m_SubsetGuideNode->GetChildNode(name) : m_SubsetGuideNode->CreateChildNode(name);
			MkFloat2 nodePos(static_cast<float>(rect.position.x), static_cast<float>(m_ImageSize.y - rect.position.y - rect.size.y));
			subsetNode->SetLocalPosition(nodePos + APPDEF_IMAGE_BORDER_SIZE);

			MkPanel* namePanel = NULL;
			if (subsetNode->PanelExist(L"Name"))
			{
				namePanel = subsetNode->GetPanel(L"Name");
			}
			else
			{
				namePanel = &subsetNode->CreatePanel(L"Name");
				namePanel->SetTextMsg(name.GetString());
			}
			namePanel->SetLocalPosition(MkFloat2(0.f, static_cast<float>(rect.size.y + 3)));
			namePanel->SetVisible(hasCursor || hasFocus);
			
			MkPanel& infoPanel = subsetNode->PanelExist(L"Info") ? (*subsetNode->GetPanel(L"Info")) : subsetNode->CreatePanel(L"Info");
			MkStr msg;
			msg.Reserve(32);
			msg += rect.position;
			msg += L" / ";
			msg += rect.size;
			infoPanel.SetTextMsg(msg);
			infoPanel.SetLocalPosition(MkFloat2(0.f, -16.f));
			infoPanel.SetVisible(hasCursor || hasFocus);

			{
				MkLineShape& lineShape = subsetNode->CreateLine(L"Rect");
				lineShape.SetColor((hasFocus) ? APPDEF_BRIGHT_SUBSET_GUIDE_COLOR : APPDEF_DARK_SUBSET_GUIDE_COLOR);
				lineShape.SetLocalDepth((hasFocus) ? -1.f : 0.f);
				MkArray<MkFloat2> lineVertices(5);
				lineVertices.PushBack(MkFloat2::Zero);
				lineVertices.PushBack(MkFloat2(0.f, static_cast<float>(rect.size.y - 1)));
				lineVertices.PushBack(MkFloat2(static_cast<float>(rect.size.x - 1), static_cast<float>(rect.size.y - 1)));
				lineVertices.PushBack(MkFloat2(static_cast<float>(rect.size.x - 1), 0.f));
				lineVertices.PushBack(MkFloat2::Zero);
				lineShape.AddLine(lineVertices);
			}

			if (hasFocus && (m_OnFocusEdge != 0))
			{
				MkLineShape& lineShape = subsetNode->CreateLine(L"Edge");
				lineShape.SetColor(MkColor(1.f, 0.7f, 0.7f));
				lineShape.SetLocalDepth(-2.f);

				MkFloat2 center, offset(3.f, 3.f);
				switch (m_OnFocusEdge)
				{
				case 1: // control
					offset.x = center.x = static_cast<float>(rect.size.x - 1) * 0.5f;
					offset.y = center.y = static_cast<float>(rect.size.y - 1) * 0.5f;
					break;
				case VK_LEFT:
					center.y = static_cast<float>(rect.size.y - 1) * 0.5f;
					break;
				case VK_UP:
					center.x = static_cast<float>(rect.size.x - 1) * 0.5f;
					center.y = static_cast<float>(rect.size.y - 1);
					break;
				case VK_RIGHT:
					center.x = static_cast<float>(rect.size.x - 1);
					center.y = static_cast<float>(rect.size.y - 1) * 0.5f;
					break;
				case VK_DOWN:
					center.x = static_cast<float>(rect.size.x - 1) * 0.5f;
					break;
				}

				MkArray<MkFloat2> lineVertices(5);
				lineVertices.PushBack(center + MkFloat2(-offset.x, 0.f));
				lineVertices.PushBack(center + MkFloat2(0.f, offset.y));
				lineVertices.PushBack(center + MkFloat2(offset.x, 0.f));
				lineVertices.PushBack(center + MkFloat2(0.f, -offset.y));
				lineVertices.PushBack(center + MkFloat2(-offset.x, 0.f));
				lineShape.AddLine(lineVertices);
			}
			else
			{
				subsetNode->DeleteLine(L"Edge");
			}
		}
		else
		{
			if (m_SubsetGuideNode->ChildExist(name))
			{
				m_SubsetGuideNode->RemoveChildNode(name);
			}
		}
	}

	void _UpdateFocusSubsetInfoUI(void)
	{
		SystemWindowBodyNode* systemWindow = _GetSystemWindowBodyNode();
		if (systemWindow != NULL)
		{
			MkEditBoxControlNode* ebNameNode = dynamic_cast<MkEditBoxControlNode*>(systemWindow->GetChildNode(L"SubsetNameEB"));
			MkEditBoxControlNode* ebPosNode = dynamic_cast<MkEditBoxControlNode*>(systemWindow->GetChildNode(L"SubsetPosEB"));
			MkEditBoxControlNode* ebSizeNode = dynamic_cast<MkEditBoxControlNode*>(systemWindow->GetChildNode(L"SubsetSizeEB"));
			if ((ebNameNode != NULL) && (ebPosNode != NULL) && (ebSizeNode != NULL) && (m_ImageInfoPtr != NULL))
			{
				if (m_OnFocusSubset.Empty() || (!m_ImageInfoPtr->GetSubsets().Exist(m_OnFocusSubset)))
				{
					ebNameNode->SetText(MkStr::EMPTY);
					ebPosNode->SetText(MkStr::EMPTY);
					ebSizeNode->SetText(MkStr::EMPTY);
				}
				else
				{
					ebNameNode->SetText(m_OnFocusSubset.GetString());

					MkIntRect rect;
					_GetSubsetRect(m_OnFocusSubset, rect);
					ebPosNode->SetText(MkStr(rect.position.x) + L", " + MkStr(rect.position.y));
					ebSizeNode->SetText(MkStr(rect.size.x) + L", " + MkStr(rect.size.y));
				}
			}
		}
	}
	
	MkHashStr _GetNoNameString(void)
	{
		MkStr name;
		name.Reserve(16);
		while (true)
		{
			name.Flush();
			name += L"_NONAME_";
			name += MkStr(m_AutoIncCounter++);

			if ((!m_ImageInfoPtr->GetSubsets().Exist(name)) && (!m_ImageInfoPtr->GetSequences().Exist(name)))
				break;
		}
		return name;
	}

	void _AddNewSubsetFromSelection(const MkHashStr& name, const MkInt2& position, const MkInt2& size)
	{
		if ((!m_ImageInfoPtr->GetSubsets().Exist(name)) && (!m_ImageInfoPtr->GetSequences().Exist(name)))
		{
			if (!m_OnFocusSubset.Empty())
			{
				_UpdateSubsetGuideNode(m_OnFocusSubset, false, false);
			}

			MkIntRect rect(position, size);
			m_ImageInfoPtr->UpdateSubset(name, rect, m_ImageSize);

			m_OnFocusEdge = 0;
			_UpdateSubsetGuideNode(name, false, true);

			m_OnFocusSubset = name;
			_UpdateFocusSubsetInfoUI();
		}
	}

	bool _SaveCurrentImageInfo(void)
	{
		if (m_CurrentImageInfoPath.Empty() || (m_ImageInfoPtr == NULL))
			return false;

		if (m_CurrentImageInfoPath.CheckAvailable())
		{
			MkStr msg = m_CurrentImageInfoPath;
			msg += MkStr::CRLF;
			msg += MkStr::CRLF;
			msg += L"경로에 덮어쓸까요?";
			if (::MessageBox(m_hWnd, msg.GetPtr(), L"주의", MB_YESNO) == IDNO)
			{
				MkPathName miiPath;
				if (miiPath.GetSaveFilePathFromDialog(MKDEF_PA_IMAGE_INFO_FILE_EXT, m_hWnd))
				{
					m_CurrentImageInfoPath = miiPath;
				}
				else
					return false;
			}
		}

		bool saveToText = false;
		SystemWindowBodyNode* systemWindow = _GetSystemWindowBodyNode();
		if (systemWindow != NULL)
		{
			MkCheckBoxControlNode* winNode = dynamic_cast<MkCheckBoxControlNode*>(systemWindow->GetChildNode(L"SaveMIIToText"));
			if (winNode != NULL)
			{
				saveToText = winNode->GetCheck();
			}
		}
		
		return m_ImageInfoPtr->SaveToFile(m_CurrentImageInfoPath, saveToText);
	}

	void _ClearMainImage(void)
	{
		if (m_MainImageNode == NULL)
			return;

		m_MainImageNode->DetachFromParentNode();
		MK_DELETE(m_MainImageNode);

		if (m_WindowMgrNode != NULL)
		{
			m_WindowMgrNode->DeleteWindow(L"<Minimap>");

			if (m_WindowMgrNode->ChildExist(L"<System>"))
			{
				MkVisualPatternNode* systemNode = dynamic_cast<MkVisualPatternNode*>(m_WindowMgrNode->GetChildNode(L"<System>"));
				if (systemNode != NULL)
				{
					systemNode->SetAlignmentOffset(-APPDEF_SYSTEM_UI_BORDER_SIZE); // system window 위치 재설정
				}
			}

			m_WindowMgrNode->DeleteWindow(L"<PlaySeq>");
		}

		m_ImageSize.Clear();
		m_SourcePixel.Clear();

		SystemWindowBodyNode* systemWindow = _GetSystemWindowBodyNode();
		if (systemWindow != NULL)
		{
			MkEditBoxControlNode* winNode = dynamic_cast<MkEditBoxControlNode*>(systemWindow->GetChildNode(L"GroupNameEB"));
			if (winNode != NULL)
			{
				winNode->SetText(MkStr::EMPTY);
			}

			winNode = dynamic_cast<MkEditBoxControlNode*>(systemWindow->GetChildNode(L"SeqNameEB"));
			if (winNode != NULL)
			{
				winNode->SetText(MkStr::EMPTY);
			}

			MkDropDownListControlNode* listNode = dynamic_cast<MkDropDownListControlNode*>(systemWindow->GetChildNode(L"SeqDDList"));
			if (listNode != NULL)
			{
				listNode->ClearAllItems();
			}
		}

		if (m_SubsetGuideNode != NULL)
		{
			m_SubsetGuideNode->Clear();
		}

		m_OnCursorSubset.Clear();
		m_OnFocusSubset.Clear();
		m_OnFocusEdge = 0;
		_UpdateFocusSubsetInfoUI();

		m_ImageInfoPtr = NULL;

		MK_BITMAP_POOL.UnloadBitmapTexture(m_CurrentImageFilePath); // 캐시를 날려줘야 동일 파일이 최신으로 로딩된다
	}

	void _GetSubsetRect(const MkHashStr& name, MkIntRect& rect) const
	{
		if ((m_ImageInfoPtr != NULL) && m_ImageInfoPtr->GetSubsets().Exist(name))
		{
			const MkImageInfo::Subset ss = m_ImageInfoPtr->GetSubsets()[name];
			rect.position = ss.position;
			rect.size = MkInt2(static_cast<int>(ss.rectSize.x), static_cast<int>(ss.rectSize.y));
		}
	}

	//------------------------------------------------------------------------------------------------//

	void _CreateMinimapWindow(const MkPathName& imagePath, const MkFloat2& imageSize)
	{
		if (m_WindowMgrNode == NULL)
			return;

		// title
		MkTitleBarControlNode* titleBar = MkTitleBarControlNode::CreateChildNode(NULL, L"<Minimap>");
		titleBar->SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, APPDEF_SYSTEM_UI_WIDTH, true);
		titleBar->SetIcon(MkWindowThemeData::eIT_Information);
		titleBar->SetCaption(L"미니맵", eRAP_LeftCenter);
		titleBar->SetAlignmentPosition(eRAP_RightTop);
		titleBar->SetAlignmentOffset(-APPDEF_SYSTEM_UI_BORDER_SIZE);

		m_WindowMgrNode->AttachWindow(titleBar);
		m_WindowMgrNode->ActivateWindow(L"<Minimap>");

		// body frame
		MinimapWindowBodyNode* bodyNode = new MinimapWindowBodyNode(L"BodyFrame");
		titleBar->AttachChildNode(bodyNode);
		bodyNode->SetBodyFrame(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true,
			MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(APPDEF_SYSTEM_UI_WIDTH, APPDEF_SYSTEM_UI_WIDTH + 20.f));

		// 크기 계산
		MkFloat2 worldSize(APPDEF_IMAGE_BORDER_SIZE * 2.f + imageSize);
		MkFloat2 bgSize(APPDEF_SYSTEM_UI_WIDTH, APPDEF_SYSTEM_UI_WIDTH);
		if (worldSize.x > worldSize.y)
		{
			bgSize.y = APPDEF_SYSTEM_UI_WIDTH * worldSize.y / worldSize.x;
		}
		else
		{
			bgSize.x = APPDEF_SYSTEM_UI_WIDTH * worldSize.x / worldSize.y;
		}
		MkFloat2 bgPos = MkFloat2(3.f + (APPDEF_SYSTEM_UI_WIDTH - bgSize.x) / 2.f, 3.f + (APPDEF_SYSTEM_UI_WIDTH - bgSize.y) / 2.f);
		MkFloat2 newImgSize = imageSize * bgSize.x / worldSize.x;
		
		// bg
		{
			MkPanel& panel = bodyNode->CreatePanel(L"BG");
			panel.SetSmallerSourceOp(MkPanel::eExpandSource);
			panel.SetBiggerSourceOp(MkPanel::eReduceSource);
			panel.SetTexture(L"Image\\minimap_bg.png");
			panel.SetLocalPosition(bgPos);
			panel.SetPanelSize(bgSize);
			panel.SetLocalDepth(-1.f);
		}

		// image
		{
			MkPanel& panel = bodyNode->CreatePanel(L"IMG");
			panel.SetSmallerSourceOp(MkPanel::eExpandSource);
			panel.SetBiggerSourceOp(MkPanel::eReduceSource);
			panel.SetTexture(imagePath);
			panel.SetPanelSize(newImgSize);
			panel.SetLocalPosition(MkFloat2(bgPos.x + (bgSize.x - newImgSize.x) / 2.f, bgPos.y + (bgSize.y - newImgSize.y) / 2.f));
			panel.SetLocalDepth(-2.f);
		}

		MkLineShape& srLine = bodyNode->CreateLine(L"SR");
		srLine.SetColor(MkColor::Yellow);
		srLine.SetLocalDepth(-3.f);

		if (m_WindowMgrNode->ChildExist(L"<System>"))
		{
			MkVisualPatternNode* systemNode = dynamic_cast<MkVisualPatternNode*>(m_WindowMgrNode->GetChildNode(L"<System>"));
			if (systemNode != NULL)
			{
				MkFloat2 offset = -APPDEF_SYSTEM_UI_BORDER_SIZE;
				offset.y -= APPDEF_SYSTEM_UI_WIDTH + 34.f;
				systemNode->SetAlignmentOffset(offset); // system window 위치 재설정
			}
		}
	}

	void _CreateAniviewWindow(const MkHashStr& seqName, bool keepPosition = false)
	{
		MkFloat2 lastPos;
		if (keepPosition && m_WindowMgrNode->ChildExist(L"<PlaySeq>"))
		{
			lastPos = m_WindowMgrNode->GetChildNode(L"<PlaySeq>")->GetLocalPosition();
		}

		m_WindowMgrNode->DeleteWindow(L"<PlaySeq>");

		MkFloat2 clientSize;
		AnimationViewWindowNode* viewNode = new AnimationViewWindowNode(L"AniView");
		viewNode->SetLocalDepth(-1.f);
		viewNode->SetImagePath(m_CurrentImageFilePath);
		viewNode->UpdateSequence(seqName, m_ImageInfoPtr->GetSequences()[seqName], clientSize);

		MkTitleBarControlNode* titleBar = MkTitleBarControlNode::CreateChildNode(NULL, L"<PlaySeq>");
		titleBar->SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, clientSize.x, true);
		titleBar->SetIcon(MkWindowThemeData::eIT_CheckMark);
		titleBar->SetCaption(seqName.GetString(), eRAP_LeftCenter);

		if (keepPosition)
		{
			titleBar->SetLocalPosition(lastPos);
		}
		else
		{
			titleBar->SetAlignmentPosition(eRAP_MiddleCenter);
			titleBar->SetAlignmentOffset(MkFloat2(0.f, clientSize.y));
		}

		MkBodyFrameControlNode* bodyNode = MkBodyFrameControlNode::CreateChildNode(titleBar, L"BodyFrame");
		bodyNode->SetBodyFrame(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, clientSize + MkFloat2(0.f, 20.f));
		bodyNode->AttachChildNode(viewNode);

		m_WindowMgrNode->AttachWindow(titleBar);
		m_WindowMgrNode->ActivateWindow(L"<PlaySeq>");
	}

	//------------------------------------------------------------------------------------------------//

	SystemWindowBodyNode* _GetSystemWindowBodyNode(void)
	{
		if (m_WindowMgrNode != NULL)
		{
			MkSceneNode* titleNode = m_WindowMgrNode->GetChildNode(L"<System>");
			if (titleNode != NULL)
			{
				MkSceneNode* bodyNode = titleNode->GetChildNode(L"BodyFrame");
				if ((bodyNode != NULL) && bodyNode->IsDerivedFrom(ePA_SNT_BodyFrameControlNode))
				{
					return dynamic_cast<SystemWindowBodyNode*>(bodyNode);
				}
			}
		}
		return NULL;
	}

	MinimapWindowBodyNode* _GetMinimapWindowBodyNode(void)
	{
		if (m_WindowMgrNode != NULL)
		{
			MkSceneNode* titleNode = m_WindowMgrNode->GetChildNode(L"<Minimap>");
			if (titleNode != NULL)
			{
				MkSceneNode* bodyNode = titleNode->GetChildNode(L"BodyFrame");
				if ((bodyNode != NULL) && bodyNode->IsDerivedFrom(ePA_SNT_BodyFrameControlNode))
				{
					return dynamic_cast<MinimapWindowBodyNode*>(bodyNode);
				}
			}
		}
		return NULL;
	}

	SequenceWindowBodyNode* _GetSequenceWindowBodyNode(void)
	{
		if (m_WindowMgrNode != NULL)
		{
			MkSceneNode* titleNode = m_WindowMgrNode->GetChildNode(L"<Sequence>");
			if (titleNode != NULL)
			{
				MkSceneNode* bodyNode = titleNode->GetChildNode(L"BodyFrame");
				if ((bodyNode != NULL) && bodyNode->IsDerivedFrom(ePA_SNT_BodyFrameControlNode))
				{
					return dynamic_cast<SequenceWindowBodyNode*>(bodyNode);
				}
			}
		}
		return NULL;
	}

	//------------------------------------------------------------------------------------------------//

	void _UpdateCamera(const MkFloat2& pickPos)
	{
		if ((m_DrawStep == NULL) || (m_MainImageNode == NULL))
			return;

		const MkPanel* imgPanel = m_MainImageNode->GetPanel(L"Image");
		if (imgPanel == NULL)
			return;

		const MkFloat2& panelSize = imgPanel->GetPanelSize();
		MkFloat2 worldSize = APPDEF_IMAGE_BORDER_SIZE * 2.f + panelSize;
		MkFloat2 screenSize = m_DrawStep->GetRegionRect().size;
		MkFloatRect pickRect(pickPos - screenSize / 2.f, screenSize);

		if ((pickRect.position.x + pickRect.size.x) > worldSize.x) pickRect.position.x = worldSize.x - pickRect.size.x;
		if (pickRect.position.x < 0.f) pickRect.position.x = 0.f;

		if (pickRect.position.y < 0.f) pickRect.position.y = 0.f;
		if ((pickRect.position.y + pickRect.size.y) > worldSize.y) pickRect.position.y = worldSize.y - pickRect.size.y;
		
		MkFloatRect worldRect(MkFloat2::Zero, worldSize);
		MkFloat2 camOffset = worldRect.Confine(pickRect);
		m_DrawStep->SetCameraOffset(camOffset);

		MinimapWindowBodyNode* bodyNode = _GetMinimapWindowBodyNode();
		if (bodyNode == NULL)
			return;
		
		const MkPanel* bgPanel = bodyNode->GetPanel(L"BG");
		MkLineShape* srLine = bodyNode->GetLine(L"SR");
		if ((bgPanel == NULL) || (srLine == NULL))
			return;

		float sizeFactor = bgPanel->GetPanelSize().x / worldSize.x;
		
		pickRect.size *= sizeFactor;
		pickRect.position = camOffset * sizeFactor;
		pickRect.position += bgPanel->GetLocalPosition();

		MkFloat2 LB = pickRect.GetAbsolutePosition(MkFloatRect::eLeftBottom);
		MkFloat2 RT = pickRect.GetAbsolutePosition(MkFloatRect::eRightTop);
		LB.CompareAndKeepMax(bgPanel->GetLocalPosition());
		RT.CompareAndKeepMin(bgPanel->GetLocalPosition() + bgPanel->GetPanelSize());

		srLine->Clear();
		MkArray<MkFloat2> lineVertices(5);
		lineVertices.PushBack(LB);
		lineVertices.PushBack(MkFloat2(LB.x, RT.y));
		lineVertices.PushBack(RT);
		lineVertices.PushBack(MkFloat2(RT.x, LB.y));
		lineVertices.PushBack(LB);
		srLine->AddLine(lineVertices);
	}

	void _EndOfLeftDrag(void)
	{
		if (m_CursorDragState != eCDS_None)
		{
			m_CursorDragState = eCDS_None; // left drag 취소
			if (m_SubsetGuideNode != NULL)
			{
				m_SubsetGuideNode->DeleteLine(L"DragGuide");
				m_SubsetGuideNode->DeletePanel(L"DragSelInfo");
			}
		}
	}

	//------------------------------------------------------------------------------------------------//
	// subset 영역 선택

	bool _FindSubsetRegion(const MkFloat2& a, const MkFloat2& b, MkIntRect& rect)
	{
		MkFloat2 LB = a;
		LB.CompareAndKeepMin(b);
		MkFloat2 RT = a;
		RT.CompareAndKeepMax(b);

		MkInt2 selectionSize(static_cast<int>(RT.x - LB.x), static_cast<int>(RT.y - LB.y));
		if (!selectionSize.IsPositive())
			return false;

		MkInt2 LT(static_cast<int>(LB.x - APPDEF_IMAGE_BORDER_SIZE.x), m_ImageSize.y - static_cast<int>(RT.y - APPDEF_IMAGE_BORDER_SIZE.y));
		LT.CompareAndKeepMax(MkInt2::Zero);

		MkInt2 RB = LT + selectionSize;
		RB.CompareAndKeepMin(m_ImageSize);
		
		MkInt2 minPt = RB;
		MkInt2 maxPt = LT;

		for (int y = LT.y; y < RB.y; ++y)
		{
			for (int x = LT.x; x < RB.x; ++x)
			{
				if (m_SourcePixel[y * m_ImageSize.x + x])
				{
					MkInt2 currPos(x, y);
					minPt.CompareAndKeepMin(currPos);
					maxPt.CompareAndKeepMax(currPos);
				}
			}
		}

		MkInt2 newSize = maxPt - minPt;
		if (!newSize.IsPositive())
			return false;

		rect.position = minPt;
		rect.size = newSize;
		return true;
	}

	bool _GetBorderLine(const MkInt2& minPt, const MkInt2& maxPt, eRectAlignmentType alignmentType, MkArray<MkInt2>& lineBuffer)
	{
		lineBuffer.Flush();

		MkInt2 beginPt, direction;
		int endCnt;
		switch (alignmentType)
		{
		case eRAT_Left:
			beginPt = minPt - MkInt2(1, 1);
			direction.y = 1;
			endCnt = beginPt.x + maxPt.y + 1;
			break;
		case eRAT_Right:
			beginPt = MkInt2(maxPt.x + 1, minPt.y - 1);
			direction.y = 1;
			endCnt = beginPt.x + maxPt.y + 1;
			break;
		case eRAT_Top:
			beginPt = minPt - MkInt2(1, 1);
			direction.x = 1;
			endCnt = maxPt.x + 1 + beginPt.y;
			break;
		case eRAT_Bottom:
			beginPt = MkInt2(minPt.x - 1, maxPt.y + 1);
			direction.x = 1;
			endCnt = maxPt.x + 1 + beginPt.y;
			break;
		}

		for (MkInt2 pos = beginPt; (pos.x + pos.y) < endCnt; pos += direction)
		{
			if ((pos.x > 0) && (pos.x < m_ImageSize.x) && (pos.y > 0) && (pos.y < m_ImageSize.y))
			{
				lineBuffer.PushBack(pos);
			}
		}
		return (!lineBuffer.Empty());
	}

	bool _FindSubsetRegion(const MkFloat2& pt, MkIntRect& rect)
	{
		MkInt2 localPt(static_cast<int>(pt.x - APPDEF_IMAGE_BORDER_SIZE.x), m_ImageSize.y - static_cast<int>(pt.y - APPDEF_IMAGE_BORDER_SIZE.y));
		MkInt2 minPt = localPt, maxPt = localPt;
		bool leftDir = true, rightDir = true, upDir = true, downDir = true;
		MkArray<MkInt2> lineBuffer(GetMax<int>(m_ImageSize.x, m_ImageSize.y));

		while (leftDir || rightDir || upDir || downDir)
		{
			MkInt2 LT = minPt, RB = maxPt;

			if (leftDir)
			{
				leftDir = false;
				if (_GetBorderLine(minPt, maxPt, eRAT_Left, lineBuffer))
				{
					MK_INDEXING_LOOP(lineBuffer, i)
					{
						if (m_SourcePixel[lineBuffer[i].y * m_ImageSize.x + lineBuffer[i].x])
						{
							--LT.x;
							leftDir = true;
							break;
						}
					}
				}
			}
			if (rightDir)
			{
				rightDir = false;
				if (_GetBorderLine(minPt, maxPt, eRAT_Right, lineBuffer))
				{
					MK_INDEXING_LOOP(lineBuffer, i)
					{
						if (m_SourcePixel[lineBuffer[i].y * m_ImageSize.x + lineBuffer[i].x])
						{
							++RB.x;
							rightDir = true;
							break;
						}
					}
				}
			}
			if (upDir)
			{
				upDir = false;
				if (_GetBorderLine(minPt, maxPt, eRAT_Top, lineBuffer))
				{
					MK_INDEXING_LOOP(lineBuffer, i)
					{
						if (m_SourcePixel[lineBuffer[i].y * m_ImageSize.x + lineBuffer[i].x])
						{
							--LT.y;
							upDir = true;
							break;
						}
					}
				}
			}
			if (downDir)
			{
				downDir = false;
				if (_GetBorderLine(minPt, maxPt, eRAT_Bottom, lineBuffer))
				{
					MK_INDEXING_LOOP(lineBuffer, i)
					{
						if (m_SourcePixel[lineBuffer[i].y * m_ImageSize.x + lineBuffer[i].x])
						{
							++RB.y;
							downDir = true;
							break;
						}
					}
				}
			}

			minPt = LT;
			maxPt = RB;
		}

		MkInt2 size = maxPt - minPt;
		bool ok = size.IsPositive();
		if (ok)
		{
			rect.position = minPt;
			rect.size = size;
		}
		return ok;
	}

	//------------------------------------------------------------------------------------------------//

protected:

	enum eCursorDragState
	{
		eCDS_None = 0,
		eCDS_CameraMove,
		eCDS_Selection
	};

protected:

	HWND m_hWnd;

	// scene
	MkSceneNode* m_SceneRootNode;
	MkSceneNode* m_MainImageNode;
	MkSceneNode* m_SubsetGuideNode;
	MkWindowManagerNode* m_WindowMgrNode;

	// image data
	MkPathName m_CurrentImageFilePath;
	MkPathName m_CurrentImageInfoPath;
	MkInt2 m_ImageSize;
	MkArray<bool> m_SourcePixel;
	
	// image info
	MkImageInfo* m_ImageInfoPtr;
	MkHashStr m_OnCursorSubset; // 현재 커서를 보유하고 있는 subset
	MkHashStr m_OnFocusSubset; // 현재 지정 된 subset
	DWORD m_OnFocusEdge;
	
	// draw
	MkDrawSceneNodeStep* m_DrawStep;
	MkFloat2 m_LastDrawSize;

	// left draging
	eCursorDragState m_CursorDragState;
	MkFloat2 m_LeftCursorDragPosBuffer[2];

	// etc
	unsigned int m_AutoIncCounter;
};

//------------------------------------------------------------------------------------------------//

class MainFramework : public MkRenderFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		m_EditPage = new EditPage(L"EditPage");
		MK_PAGE_MGR.SetUp(m_EditPage);
		MK_PAGE_MGR.ChangePageDirectly(L"EditPage");
		
		bool ok = MkRenderFramework::SetUp(clientWidth, clientHeight, fullScreen, cmdLine);
		m_EditPage->SetHWND(m_MainWindow.GetWindowHandle());
		return ok;

	}

	virtual void ConsumeDraggedFiles(MkArray<MkPathName>& draggedFilePathList)
	{
		if ((m_EditPage == NULL) || draggedFilePathList.Empty())
			return;

		m_EditPage->LoadImageFile(draggedFilePathList[0]);
		draggedFilePathList.Clear();
	}

	MainFramework() : MkRenderFramework() { m_EditPage = NULL; }
	virtual ~MainFramework() {}

protected:

	EditPage* m_EditPage;
};

class MainApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new MainFramework; }

public:
	MainApplication() : MkWin32Application() {}
	virtual ~MainApplication() {}
};

// 엔트리 포인트에서의 MainApplication 실행
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	MainApplication application;
	application.Run(hI, L"MiniKeyImageInfoEditor", L"..\\FileRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 768, false, true);

	return 0;
}

