
//------------------------------------------------------------------------------------------------//
// single/multi-thread rendering application sample
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkPageManager.h"
#include "MkCore_MkInputManager.h"
#include "MkCore_MkTimeManager.h"
#include "MkCore_MkWin32Application.h"
#include "MkCore_MkProfilingManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkUniformDice.h"

#include "MkPA_MkRenderFramework.h"
#include "MkPA_MkRenderer.h"
#include "MkPA_MkWindowManagerNode.h"
#include "MkPA_MkTitleBarControlNode.h"
#include "MkPA_MkBodyFrameControlNode.h"
#include "MkPA_MkSliderControlNode.h"
#include "MkPA_MkDrawSceneNodeStep.h"
#include "MkPA_MkWindowFactory.h"

#include "codedef.h"

//------------------------------------------------------------------------------------------------//
// rigid body
//------------------------------------------------------------------------------------------------//

class BallRigidBody
{
public:
	
	void SetUp(const MkVec2& worldPosition, float mass, float radius, MkPanel* panel, MkLineShape* lineShape)
	{
		m_Mass = mass;
		m_InvMass = 1.f / m_Mass;
		m_Radius = radius;
		m_Panel = panel;
		m_LineShape = lineShape;
		SetWorldPosition(worldPosition);

		Clear();
	}

	void UpdateVelocity(float elapsed)
	{
		MkVec2 sumOfForce;
		MK_INDEXING_LOOP(m_ImpulseForce, i)
		{
			sumOfForce += m_ImpulseForce[i];
		}
		m_ImpulseForce.Clear();

		// F = ma -> a = F/m
		MkVec2 acceleration = sumOfForce * m_InvMass;
		SetLinearVelocity(m_Velocity + acceleration * elapsed);
		SetWorldPosition(m_WorldPosition + m_Velocity * elapsed);
	}

	void UpdateWallCollision(float elapsed, const MkFloat2& wallSize)
	{
		if (!m_Velocity.IsZero())
		{
			MkFloatRect wall(m_Radius, m_Radius, wallSize.x - m_Radius * 2.f, wallSize.y - m_Radius * 2.f);
			MkFloat2 wp(m_WorldPosition.x, m_WorldPosition.y);

			if (!wall.CheckIntersection(wp))
			{
				if (wp.x < wall.position.x) // left
				{
					_CheckAndReflect(MkVec2::AxisX);
				}
				else if (wp.x > (wall.position.x + wall.size.x)) // right
				{
					_CheckAndReflect(-MkVec2::AxisX);
				}
				if (wp.y < wall.position.y) // bottom
				{
					_CheckAndReflect(MkVec2::AxisY);
				}
				else if (wp.y > (wall.position.y + wall.size.y)) // top
				{
					_CheckAndReflect(-MkVec2::AxisY);
				}
			}
		}
	}

	void Clear(void)
	{
		SetLinearVelocity(MkVec2::Zero);
		m_ImpulseForce.Clear();
	}

	inline void AddImpulseForce(const MkVec2& impulseVector) { m_ImpulseForce.PushBack(impulseVector); }

	inline float GetMass(void) const { return m_Mass; }
	inline float GetRadius(void) const { return m_Radius; }

	inline void SetLinearVelocity(const MkVec2& velocity) { m_Velocity = velocity; }
	inline const MkVec2& GetLinearVelocity(void) const { return m_Velocity; }

	void SetWorldPosition(const MkVec2& position) { m_WorldPosition = position; _UpdateAABB(); }

	inline const MkVec2& GetWorldPosition(void) const { return m_WorldPosition; }
	inline const MkFloatRect& GetWorldAABB(void) const { return m_WorldAABB; }

	inline MkPanel* GetPanel(void) { return m_Panel; }
	inline MkLineShape* GetLineShape(void) { return m_LineShape; }

	static bool CheckAndUpdatePairCollision(BallRigidBody& a, BallRigidBody& b)
	{
		// AABB
		if (!a.GetWorldAABB().CheckIntersection(b.GetWorldAABB()))
			return false;

		// circle
		MkVec2 a_to_b = b.GetWorldPosition() - a.GetWorldPosition();
		if (a_to_b.Length() >= (a.GetRadius() + b.GetRadius()))
			return false;

		// reposition
		MkVec2 c = (b.GetWorldPosition() + a.GetWorldPosition()) * 0.5f;
		MkVec2 c_to_a = a.GetWorldPosition() - c;
		c_to_a.Normalize();
		a.SetWorldPosition(c + c_to_a * a.GetRadius());

		MkVec2 c_to_b = b.GetWorldPosition() - c;
		c_to_b.Normalize();
		b.SetWorldPosition(c + c_to_b * b.GetRadius());

		// reflection
		float ma = a.GetMass();
		float mb = b.GetMass();
		float ims = 1.f / (ma + mb);
		float e = 1.f;
		MkVec2 nva = (ma-mb*e) * ims * a.GetLinearVelocity() + (mb+mb*e) * ims * b.GetLinearVelocity();
		MkVec2 nvb = (ma+ma*e) * ims * a.GetLinearVelocity() + (mb-ma*e) * ims * b.GetLinearVelocity();

		a.SetLinearVelocity(nva);
		b.SetLinearVelocity(nvb);
		return true;
	}

	BallRigidBody() { SetUp(MkVec2::Zero, 1000.f, 1.f, NULL, NULL); }
	~BallRigidBody() { Clear(); }

protected:

	void _UpdateAABB(void)
	{
		MkFloat2 r2(m_Radius, m_Radius);
		m_WorldAABB.position = MkFloat2(m_WorldPosition.x, m_WorldPosition.y) - r2;
		m_WorldAABB.size = r2 * 2.f;
	}

	void _CheckAndReflect(const MkVec2& normal)
	{
		if (normal.DotProduct(m_Velocity) < 0.f)
		{
			m_Velocity = m_Velocity.Reflect(normal);
		}
	}

protected:

	// linear velocity
	float m_Mass;
	float m_InvMass;

	MkVec2 m_Velocity;
	MkVec2 m_WorldPosition;
	MkFloatRect m_WorldAABB;

	MkArray<MkVec2> m_ImpulseForce;

	// rigid body
	float m_Radius;

	// scene
	MkPanel* m_Panel;
	MkLineShape* m_LineShape;
};


//------------------------------------------------------------------------------------------------//
// system window
//------------------------------------------------------------------------------------------------//

class SystemWindowBodyNode : public MkBodyFrameControlNode
{
public:
	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
	{
		// pick region
		if (((eventType == ePA_SNE_CursorLBtnPressed) && (path.GetSize() == 0)) ||
			(m_MinimapHit && (eventType == ePA_SNE_CursorMoved)))
		{
			MkVec2 pt;
			if (argument->GetData(MkWindowBaseNode::ArgKey_CursorLocalPosition, pt, 0))
			{
				MkPanel* mmPanel = GetPanel(L"MM");
				MkFloatRect mmRect;
				mmRect.position = mmPanel->GetLocalPosition();
				mmRect.size = mmPanel->GetPanelSize();
				MkFloat2 hitPos(pt.x, pt.y);
				m_MinimapHit = mmRect.CheckIntersection(hitPos);
				if (m_MinimapHit)
				{
					m_HitPosition = hitPos - mmPanel->GetLocalPosition();
				}
			}
		}
		// btn click
		else if (eventType == ePA_SNE_CursorLBtnReleased)
		{
			m_MinimapHit = false;

			if ((path.GetSize() == 1))
			{
				if (path[0].Equals(0, L"__#WC:Cancel"))
				{
					m_StopAll = true;
				}
				else if (path[0].Equals(0, L"__#WC:Ok"))
				{
					m_AddRandomImpulse = true;
				}
			}
		}
		// object count
		else if (eventType == ePA_SNE_SliderMoved)
		{
			argument->GetData(MkSliderControlNode::ArgKey_SliderPos, m_BallCount, 0);
		}

		MkBodyFrameControlNode::SendNodeReportTypeEvent(eventType, path, argument);
	}

	inline bool DoesMinimapHit(void) const { return m_MinimapHit; }
	inline const MkFloat2& GetMinimapHitPosition(void) const { return m_HitPosition; }

	inline int GetBallCount(void) const { return m_BallCount; }

	inline bool GetStopAll(void) { bool ok = m_StopAll; m_StopAll = false; return ok; }
	inline bool AddRandomImpulse(void) { bool ok = m_AddRandomImpulse; m_AddRandomImpulse = false; return ok; }

	SystemWindowBodyNode(const MkHashStr& name) : MkBodyFrameControlNode(name)
	{
		m_MinimapHit = false;
		m_StopAll = false;
		m_AddRandomImpulse = false;
	}
	virtual ~SystemWindowBodyNode() {}
	
protected:

	bool m_MinimapHit;
	MkFloat2 m_HitPosition;

	int m_BallCount;

	bool m_StopAll;
	bool m_AddRandomImpulse;
};


//------------------------------------------------------------------------------------------------//
// TestPage 선언
//------------------------------------------------------------------------------------------------//

class TestPage : public MkBasePage
{
public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		// const
		const unsigned int MAX_OBJ_CNT = 1000;
		const float OBJ_RADIUS = 14.f;

		// root
		m_SceneRootNode = new MkSceneNode(L"<Root>");

		// background
		MkSceneNode* bgNode = m_SceneRootNode->CreateChildNode(L"<BG>");
		MkPanel& bgPanel = bgNode->CreatePanel(L"P");
		bgPanel.SetTexture(L"Image\\bg.png");
		bgNode->SetLocalDepth(10000.f);

		// window mgr
		m_WindowMgrNode = MkWindowManagerNode::CreateChildNode(NULL, L"<WinMgr>");
		m_WindowMgrNode->SetDepthBandwidth(1000.f);

		// window : system
		MkTitleBarControlNode* titleBar = MkTitleBarControlNode::CreateChildNode(NULL, L"<System>");
		titleBar->SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 256.f, true);
		titleBar->SetIcon(MkWindowThemeData::eIT_Notice);
		titleBar->SetCaption(L"시스템", eRAP_LeftCenter);
		titleBar->SetAlignmentPosition(eRAP_RightTop);
		titleBar->SetAlignmentOffset(MkFloat2(-10.f, -10.f));

		m_WindowMgrNode->AttachWindow(titleBar);
		m_WindowMgrNode->ActivateWindow(L"<System>");

		// system - body frame
		m_SystemWindowBodyNode = new SystemWindowBodyNode(L"BodyFrame");
		titleBar->AttachChildNode(m_SystemWindowBodyNode);
		m_SystemWindowBodyNode->SetBodyFrame(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(256.f, 350.f));

		// system - minimap
		MkPanel& mmPanel = m_SystemWindowBodyNode->CreatePanel(L"MM");
		mmPanel.SetTexture(L"Image\\minimap.png");
		mmPanel.SetLocalDepth(-1.f);
		mmPanel.SetLocalPosition(MkFloat2(3.f, 75.f));

		MkLineShape& srLine = m_SystemWindowBodyNode->CreateLine(L"SR");
		srLine.SetColor(MkColor::Yellow);
		srLine.SetLocalDepth(-3.f);

		// system - object count
		MkSliderControlNode* vSliderNode = MkSliderControlNode::CreateChildNode(m_SystemWindowBodyNode, L"Slider");
		vSliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 220.f, 0, MAX_OBJ_CNT, MAX_OBJ_CNT);
		vSliderNode->SetAlignmentPosition(eRAP_MiddleBottom);
		vSliderNode->SetAlignmentOffset(MkFloat2(0.f, 40.f));
		vSliderNode->SetLocalDepth(-1.f);

		// system - btns
		MkWindowFactory wndFactory;
		wndFactory.SetMinClientSizeForButton(MkFloat2(100.f, 20.f));

		MkWindowBaseNode* btnInst = wndFactory.CreateCancelButtonNode(L"모두가 멈춰라!");
		btnInst->SetAlignmentPosition(eRAP_LeftBottom);
		m_SystemWindowBodyNode->AttachChildNode(btnInst);

		btnInst = wndFactory.CreateOkButtonNode(L"오빠 달려!");
		btnInst->SetAlignmentPosition(eRAP_RightBottom);
		m_SystemWindowBodyNode->AttachChildNode(btnInst);

		// object(balls)
		m_ObjectNode = m_SceneRootNode->CreateChildNode(L"<Balls>");
		m_ObjectNode->SetLocalDepth(8000.f);

		m_Balls.Reserve(MAX_OBJ_CNT);

		int border = static_cast<int>(OBJ_RADIUS) * 2;
		MkUniformDice ballPosXDice(border, static_cast<int>(bgPanel.GetPanelSize().x) - border, 12345);
		MkUniformDice ballPosYDice(border, static_cast<int>(bgPanel.GetPanelSize().y) - border, 321);

		for (unsigned int i=0; i<MAX_OBJ_CNT; ++i)
		{
			MkVec2 wpos(static_cast<float>(ballPosXDice.GenerateRandomNumber()), static_cast<float>(ballPosYDice.GenerateRandomNumber()));

			unsigned int index = m_Balls.GetSize();
			MkStr indexStr = index;
			MkStr seqStr = index % 8;

			MkPanel& panel = m_ObjectNode->CreatePanel(indexStr);
			panel.SetTexture(L"Image\\obj.png");
			MkStr ssName = L"OBJ_" + seqStr;
			panel.SetSubsetOrSequenceName(ssName);
			panel.SetLocalPosition(MkFloat2(wpos.x - OBJ_RADIUS, wpos.y - OBJ_RADIUS));
			panel.SetLocalDepth(static_cast<float>(index) * 0.1f);

			MkLineShape& lineShape = m_SystemWindowBodyNode->CreateLine(indexStr);
			lineShape.SetColor(MkColor::Red);
			lineShape.SetLocalDepth(-2.f);

			BallRigidBody& object = m_Balls.PushBack();
			object.SetUp(wpos, 1.f, OBJ_RADIUS, &panel, &lineShape);
		}
		
		// draw step
		m_DrawStep = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep(L"Scene");
		m_DrawStep->SetSceneNode(m_SceneRootNode);

		MkFloat2 camCenterOffset = (bgPanel.GetPanelSize() - m_DrawStep->GetRegionRect().size) * 0.5f;
		m_DrawStep->SetCameraOffset(camCenterOffset);

		MkDrawSceneNodeStep* winMgrStep = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep(L"WinMgr");
		winMgrStep->SetSceneNode(m_WindowMgrNode);
		winMgrStep->SetClearLastRenderTarget(false);

		// finish
		m_SceneRootNode->Update();
		m_WindowMgrNode->Update();

		MK_DEV_PANEL.MsgToFreeboard(0, L"Enter 키 : 시스템 윈도우 토글");
		MK_DEV_PANEL.MsgToFreeboard(1, L"방향키 : 포커스 이동");
		MK_DEV_PANEL.MsgToFreeboard(3, L"월드 크기 : " + MkStr(bgPanel.GetPanelSize().x) + L" * " + MkStr(bgPanel.GetPanelSize().y));
		MK_DEV_PANEL.MsgToFreeboard(4, L"오브젝트 수 : " + MkStr(m_CurrBallCount));

#if (DEF_METHOD_TYPE == 1)

		const int GRID_COUNT = 32;

		// GRID_COUNT * GRID_COUNT * 12
		m_Cells.Fill(GRID_COUNT);

		MK_INDEXING_LOOP(m_Cells, i)
		{
			MkArray< MkArray<int> >& row = m_Cells[i];
			row.Fill(GRID_COUNT);

			MK_INDEXING_LOOP(row, j)
			{
				row[j].Reserve(12);
			}
		}

		m_GridSize = bgPanel.GetPanelSize() / static_cast<float>(GRID_COUNT);

		MK_DEV_PANEL.MsgToFreeboard(6, L"grid count / cell size : " + MkStr(GRID_COUNT) + L" / " + MkStr(m_GridSize.x) + L", " + MkStr(m_GridSize.y));
#endif
		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		MkFocusProfiler focusProfiler(L"TestPage::Update()");

		if ((m_SceneRootNode != NULL) && (m_WindowMgrNode != NULL) && (m_DrawStep != NULL))
		{
			// toggle system window
			if (MK_INPUT_MGR.GetKeyPressed(VK_RETURN))
			{
				if (m_WindowMgrNode->IsActivating(L"<System>"))
				{
					m_WindowMgrNode->DeactivateWindow(L"<System>");
				}
				else
				{
					m_WindowMgrNode->ActivateWindow(L"<System>");
				}
			}

			// update region
			const MkFloat2& worldSize = m_SceneRootNode->GetChildNode(L"<BG>")->GetPanel(L"P")->GetPanelSize();
			MkFloat2 screenSize = m_DrawStep->GetRegionRect().size;
			MkPanel* mmPanel = m_SystemWindowBodyNode->GetPanel(L"MM");
			const MkFloat2& mmSize = mmPanel->GetPanelSize();

			// camera reposition
			MkFloat2 camOffset;

			// moved by click?
			if (m_SystemWindowBodyNode->DoesMinimapHit())
			{
				camOffset = m_SystemWindowBodyNode->GetMinimapHitPosition();
				camOffset.x *= worldSize.x / mmSize.x;
				camOffset.y *= worldSize.y / mmSize.y;
				camOffset -= screenSize * 0.5f;
			}
			// moved by keyboard?
			else
			{
				float velocity = static_cast<float>(timeState.elapsed) * 2048.f;
				if (MK_INPUT_MGR.GetKeyPushing(VK_LEFT)) camOffset.x -= velocity;
				if (MK_INPUT_MGR.GetKeyPushing(VK_RIGHT)) camOffset.x += velocity;
				if (MK_INPUT_MGR.GetKeyPushing(VK_UP)) camOffset.y += velocity;
				if (MK_INPUT_MGR.GetKeyPushing(VK_DOWN)) camOffset.y -= velocity;
				camOffset += m_DrawStep->GetCameraOffset();
			}
			
			// check region
			camOffset.CompareAndKeepMin(worldSize - screenSize);
			camOffset.CompareAndKeepMax(MkFloat2::Zero);

			// position apply
			m_DrawStep->SetCameraOffset(camOffset);

			// screen region
			MkFloat2 w_to_m_ratio(mmSize.x / worldSize.x, mmSize.y / worldSize.y);
			MkFloat2 screenRegionMin = camOffset * w_to_m_ratio.x + mmPanel->GetLocalPosition();
			MkFloat2 screenRegionMax = (camOffset + screenSize) * w_to_m_ratio.y + mmPanel->GetLocalPosition();

			MkLineShape* srLine = m_SystemWindowBodyNode->GetLine(L"SR");
			srLine->Clear();
			MkArray<MkFloat2> lineVertices(5);
			lineVertices.PushBack(screenRegionMin);
			lineVertices.PushBack(MkFloat2(screenRegionMin.x, screenRegionMax.y));
			lineVertices.PushBack(screenRegionMax);
			lineVertices.PushBack(MkFloat2(screenRegionMax.x, screenRegionMin.y));
			lineVertices.PushBack(screenRegionMin);
			srLine->AddLine(lineVertices);

			// object
			int ballCount = m_SystemWindowBodyNode->GetBallCount();
			if (ballCount != m_CurrBallCount)
			{
				if (ballCount > m_CurrBallCount)
				{
					for (int i=m_CurrBallCount; i<ballCount; ++i)
					{
						m_Balls[i].GetPanel()->SetVisible(true);
					}
				}
				else
				{
					for (int i=m_CurrBallCount-1; i>=ballCount; --i)
					{
						BallRigidBody& currBall = m_Balls[i];
						currBall.GetPanel()->SetVisible(false);
						currBall.GetLineShape()->Clear();
					}
				}

				m_CurrBallCount = ballCount;

				MK_DEV_PANEL.MsgToFreeboard(4, L"오브젝트 수 : " + MkStr(m_CurrBallCount));
			}

			// stop
			if (m_SystemWindowBodyNode->GetStopAll())
			{
				for (int i=0; i<m_CurrBallCount; ++i)
				{
					m_Balls[i].Clear();
				}
			}

			// bomb
			if (m_SystemWindowBodyNode->AddRandomImpulse())
			{
				unsigned int POWER = 30000;

				MkUniformDice impulseXDice(0, POWER * 2, 2345);
				MkUniformDice impulseYDice(0, POWER * 2, 9999);
				
				for (int i=0; i<m_CurrBallCount; ++i)
				{
					MkVec2 force;
					force.x = static_cast<float>(impulseXDice.GenerateRandomNumber()) - static_cast<float>(POWER);
					force.y = static_cast<float>(impulseYDice.GenerateRandomNumber()) - static_cast<float>(POWER);
					m_Balls[i].AddImpulseForce(force);
				}
			}

			// update object
			float elapsed = static_cast<float>(timeState.elapsed);

			{
				MkFocusProfiler focusProfiler(L"TestPage::Update(0) 강체 속도 갱신");
				for (int i=0; i<m_CurrBallCount; ++i)
				{
					m_Balls[i].UpdateVelocity(elapsed);
				}
			}

			{
				MkFocusProfiler focusProfiler(L"TestPage::Update(1) 상호 충돌 검사");

				// ~(-_-)~
#if (DEF_METHOD_TYPE == 0)
				_UpdateInterObjectCollision_Stupid(m_Balls, m_CurrBallCount);
#elif (DEF_METHOD_TYPE == 1)
				_UpdateInterObjectCollision_Cell(m_Balls, m_CurrBallCount);
#endif
			}

			{
				MkFocusProfiler focusProfiler(L"TestPage::Update(2) 외곽 충돌 검사");
			
				for (int i=0; i<m_CurrBallCount; ++i)
				{
					BallRigidBody& currBall = m_Balls[i];
					currBall.UpdateWallCollision(elapsed, worldSize);
					const MkVec2& wp = currBall.GetWorldPosition();
					currBall.GetPanel()->SetLocalPosition(MkFloat2(wp.x - currBall.GetRadius(), wp.y - currBall.GetRadius()));
				}
			}

			{
				MkFocusProfiler focusProfiler(L"TestPage::Update(3) 미니맵 갱신");
			
				for (int i=0; i<m_CurrBallCount; ++i)
				{
					BallRigidBody& currBall = m_Balls[i];
					const MkVec2& wp = currBall.GetWorldPosition();
					MkFloat2 cp(wp.x * w_to_m_ratio.x, wp.y * w_to_m_ratio.y);
					cp += mmPanel->GetLocalPosition();
					lineVertices[0] = cp + MkFloat2(-1.f, 0.f);
					lineVertices[1] = cp + MkFloat2(0.f, 1.f);
					lineVertices[2] = cp + MkFloat2(1.f, 0.f);
					lineVertices[3] = cp + MkFloat2(0.f, -1.f);
					lineVertices[4] = lineVertices[0];
					MkLineShape* lineShape = currBall.GetLineShape();
					lineShape->Clear();
					lineShape->AddLine(lineVertices);
				}
			}

			// update scene
			{
				MkFocusProfiler focusProfiler(L"TestPage::Update(4) scene 갱신");
				m_SceneRootNode->Update(timeState.fullTime);
				m_WindowMgrNode->Update(timeState.fullTime);
			}
		}
	}

	virtual void Clear(MkDataNode* sharingNode = NULL)
	{
		MK_DELETE(m_SceneRootNode);
		MK_DELETE(m_WindowMgrNode);
		
		MK_RENDERER.GetDrawQueue().Clear();
	}

	TestPage(const MkHashStr& name) : MkBasePage(name)
	{
		m_SceneRootNode = NULL;
		m_WindowMgrNode = NULL;
		m_SystemWindowBodyNode = NULL;
		m_ObjectNode = NULL;
		m_DrawStep = NULL;
		m_CurrBallCount = 0;
	}

	virtual ~TestPage() { Clear(); }

protected:

	//------------------------------------------------------------------------------------------------//
	// ~(-_-)~
	//------------------------------------------------------------------------------------------------//

#if (DEF_METHOD_TYPE == 0)

	// 바보짓
	void _UpdateInterObjectCollision_Stupid(MkArray<BallRigidBody>& balls, int count)
	{
		if (count > 1)
		{
			for (int i=0; i<count; ++i)
			{
				for (int j=i+1; j<count; ++j)
				{
					BallRigidBody::CheckAndUpdatePairCollision(balls[i], balls[j]); // O(n^2)
				}
			}
		}
	}

#elif (DEF_METHOD_TYPE == 1)

	void _UpdateInterObjectCollision_Cell(MkArray<BallRigidBody>& balls, int count)
	{
		if (count > 1)
		{
			MkInt2 cellLimit(m_Cells.GetSize() - 1, m_Cells.GetSize() - 1);

			// 각 cell들에 오브젝트 배치
			for (int i=0; i<count; ++i)
			{
				BallRigidBody& ball = balls[i];

				MkFloat2 ptLB = ball.GetWorldAABB().GetAbsolutePosition(MkFloatRect::eLeftBottom);
				MkInt2 cellLB(static_cast<int>(ptLB.x / m_GridSize.x), static_cast<int>(ptLB.y / m_GridSize.y));
				cellLB.CompareAndKeepMin(cellLimit);

				MkFloat2 ptRT = ball.GetWorldAABB().GetAbsolutePosition(MkFloatRect::eRightTop);
				MkInt2 cellRT(static_cast<int>(ptRT.x / m_GridSize.x), static_cast<int>(ptRT.y / m_GridSize.y));
				cellRT.CompareAndKeepMin(cellLimit);

				for (int columnIndex = cellLB.y; columnIndex <= cellRT.y; ++columnIndex)
				{
					for (int rowIndex = cellLB.x; rowIndex <= cellRT.x; ++rowIndex)
					{
						m_Cells[columnIndex][rowIndex].PushBack(i);
					}
				}
			}
			
			// cell마다 배치된 오브젝트끼리 pair로 묶음
			MkArray<int> listA(count);
			MkArray<int> listB(count);

			int occupiedCellCnt = 0;

			MK_INDEXING_LOOP(m_Cells, i)
			{
				MkArray< MkArray<int> >& row = m_Cells[i];
				MK_INDEXING_LOOP(row, j)
				{
					MkArray<int>& occupy = row[j];
					if (occupy.GetSize() > 1)
					{
						MK_INDEXING_LOOP(occupy, k)
						{
							for (unsigned int cnt = occupy.GetSize(), m=k+1; m<cnt; ++m)
							{
								listA.PushBack(occupy[k]);
								listB.PushBack(occupy[m]);
							}
						}
						occupy.Flush();

						++occupiedCellCnt;
					}
				}
			}

			MK_DEV_PANEL.MsgToFreeboard(7, L"오브젝트가 차지하고 있는 cell 수 : " + MkStr(occupiedCellCnt) + L" / " + MkStr(m_Cells.GetSize() * m_Cells.GetSize()));
			MK_DEV_PANEL.MsgToFreeboard(8, L"오브젝트 pair 수 : " + MkStr(listA.GetSize()));

			// pair끼리 검사
			MK_INDEXING_LOOP(listA, i)
			{
				BallRigidBody::CheckAndUpdatePairCollision(balls[listA[i]], balls[listB[i]]);
			}
		}
		else
		{
			MK_DEV_PANEL.MsgToFreeboard(7, L"오브젝트가 차지하고 있는 cell 수 : 0 / " + MkStr(m_Cells.GetSize() * m_Cells.GetSize()));
			MK_DEV_PANEL.MsgToFreeboard(8, L"오브젝트 pair 수 : 0");
		}
	}

#endif

	//------------------------------------------------------------------------------------------------//

protected:

	// scene
	MkSceneNode* m_SceneRootNode;
	MkWindowManagerNode* m_WindowMgrNode;
	SystemWindowBodyNode* m_SystemWindowBodyNode;
	MkSceneNode* m_ObjectNode;
	
	// draw
	MkDrawSceneNodeStep* m_DrawStep;

	// object
	MkArray<BallRigidBody> m_Balls;
	int m_CurrBallCount;

#if (DEF_METHOD_TYPE == 1)
	MkArray< MkArray< MkArray<int> > > m_Cells; // column * row
	MkFloat2 m_GridSize;
#endif
};

// TestFramework 선언
class TestFramework : public MkRenderFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		MK_PAGE_MGR.SetUp(new MkBasePage(L"Root"));
		MK_PAGE_MGR.RegisterChildPage(L"Root", new TestPage(L"TestPage"));
		MK_PAGE_MGR.ChangePageDirectly(L"TestPage");
		
		return MkRenderFramework::SetUp(clientWidth, clientHeight, fullScreen, cmdLine);
	}

	virtual ~TestFramework() {}
};

// TestApplication 선언
class TestApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new TestFramework; }

public:
	TestApplication() : MkWin32Application() {}
	virtual ~TestApplication() {}
};

// 엔트리 포인트에서의 TestApplication 실행
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"떼스또", L"..\\FileRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768);

	return 0;
}

