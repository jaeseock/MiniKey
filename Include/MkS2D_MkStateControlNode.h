#pragma once


//------------------------------------------------------------------------------------------------//
// icon in single subset node : MkBaseWindowNode
//	+ window state
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkStateControlNode : public MkBaseWindowNode
{
public:

	//------------------------------------------------------------------------------------------------//
	// 초기화
	//------------------------------------------------------------------------------------------------//

	//------------------------------------------------------------------------------------------------//
	// 구성
	//------------------------------------------------------------------------------------------------//

	// MkDataNode로 구성. MkSceneNode()의 구성 규칙과 동일
	virtual void Load(const MkDataNode& node);

	// MkDataNode로 출력
	virtual void Save(MkDataNode& node);

	//------------------------------------------------------------------------------------------------//
	// enable
	//------------------------------------------------------------------------------------------------//

	virtual void SetEnable(bool enable);

	//------------------------------------------------------------------------------------------------//
	// window state
	//------------------------------------------------------------------------------------------------//

	virtual void SetWindowState(eS2D_WindowState windowState);
	inline eS2D_WindowState GetWindowState(void) const { return m_WindowState; }

	//------------------------------------------------------------------------------------------------//
	// window/client rect interface
	//------------------------------------------------------------------------------------------------//

	virtual const MkFloatRect& GetWindowRect(void) const { return m_WorldAABR; }
	virtual const MkFloatRect& GetClientRect(void) const { return m_WorldAABR; }

	MkStateControlNode(const MkHashStr& name);
	virtual ~MkStateControlNode() {}

public:

	static void __GenerateBuildingTemplate(void);

protected:

	eS2D_WindowState m_WindowState;

	MkArray<MkBaseWindowNode*> m_WindowPerState;
};
