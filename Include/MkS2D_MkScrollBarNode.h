#pragma once


//------------------------------------------------------------------------------------------------//
// smart button node : MkBaseWindowNode
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkScrollBarNode : public MkBaseWindowNode
{
public:

	enum eBarDirection
	{
		eVertical = 0,
		eHorizontal
	};

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_ScrollBarNode; }

	//------------------------------------------------------------------------------------------------//
	// component control
	//------------------------------------------------------------------------------------------------//

	// 생성
	// hasDirBtn : prev/next 버튼 존재여부
	// length : 버튼과 스크롤 바를 포함한 전체 길이
	bool CreateScrollBar(const MkHashStr& themeName, eBarDirection direction, bool hasDirBtn, float length);

	// 길이 설정
	// (NOTE) 호출 전 초기화가 이루어진 상태이어야 함
	bool SetScrollBarLength(float length);

	// 길이 반환
	// (NOTE) 호출 전 초기화가 이루어진 상태이어야 함
	float GetScrollBarLength(void) const;

	// 정보 입력
	// totalPageSize : 전체에 해당하는 논리적 길이
	// onePageSize : 한 페이지에 해당하는 논리적 길이(최소 1 이상)
	// sizePerGrid : 위치 단위 하나에 해당하는 논리적 길이(onePageSize 이하)
	// gridsPerAction : 이동 버튼/스크롤시 이동할 단위 갯수
	void SetPageInfo(unsigned int totalPageSize, unsigned int onePageSize, unsigned int sizePerGrid, unsigned int gridsPerAction = 1);

	// 전체에 해당하는 논리적 길이만 설정
	void SetPageInfo(unsigned int totalPageSize);

	// 현재 위치 단위 설정
	void SetGridPosition(unsigned int gridPosition);

	// 총 논리적 길이 반환
	inline unsigned int GetTotalPageSize(void) const { return m_TotalPageSize; }

	// 총 위치 단위 수 반환
	inline unsigned int GetGridCount(void) const { return m_GridCount; }

	// 현재 위치 단위 반환
	inline unsigned int GetCurrentGridPosition(void) const { return m_CurrentGridPosition; }

	// 현재 논리적 위치 반환(0 ~ [totalPageSize - onePageSize])
	inline unsigned int GetCurrentPagePosition(void) const { return m_CurrentPagePosition; }

	//------------------------------------------------------------------------------------------------//
	// 구성
	//------------------------------------------------------------------------------------------------//

	// MkDataNode로 구성. MkSceneNode()의 구성 규칙과 동일
	virtual void Load(const MkDataNode& node);

	// MkDataNode로 출력
	virtual void Save(MkDataNode& node);

	//------------------------------------------------------------------------------------------------//
	// event call
	//------------------------------------------------------------------------------------------------//

	virtual bool CheckCursorHitCondition(const MkFloat2& position) const { return true; } // 아무런 component도 없으므로 HitEventMousePress()를 받기 위해 통과

	virtual bool HitEventMousePress(unsigned int button, const MkFloat2& position);

	virtual bool HitEventMouseWheelMove(int delta, const MkFloat2& position);

	virtual void StartDragMovement(MkBaseWindowNode* targetWindow);
	virtual bool ConfirmDragMovement(MkBaseWindowNode* targetWindow, MkFloat2& positionOffset);

	virtual void UseWindowEvent(WindowEvent& evt);

	//------------------------------------------------------------------------------------------------//

	MkScrollBarNode(const MkHashStr& name);
	virtual ~MkScrollBarNode() {}

public:

	static void __GenerateBuildingTemplate(void);

protected:

	void _UpdateSlideTransform(void);

	unsigned int _AssignGridOffset(int offset) const;

	bool _CheckSlideButtonDragging(MkBaseWindowNode* targetWindow) const;

protected:

	eBarDirection m_BarDirection;
	unsigned int m_TotalPageSize;
	unsigned int m_OnePageSize;
	unsigned int m_SizePerGrid;
	int m_GridsPerAction;

	MkBaseWindowNode* m_SlideNode;
	float m_MaxSlideSize;
	unsigned int m_GridCount;
	unsigned int m_CurrentGridPosition;
	unsigned int m_CurrentPagePosition;

	int m_GridPositionAtDragStart;
};
