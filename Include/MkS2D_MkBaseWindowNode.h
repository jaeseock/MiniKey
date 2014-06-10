#pragma once


//------------------------------------------------------------------------------------------------//
// base window node : MkSceneNode
//	+ enable
//	+ alignment
//	+ window/client rect interface
//	+ window preset
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkSceneNode.h"


class MkBaseWindowNode : public MkSceneNode
{
public:

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

	virtual void SetEnable(bool enable) { m_Enable = enable; }
	inline bool GetEnable(void) const { return m_Enable; }

	//------------------------------------------------------------------------------------------------//
	// alignment
	//------------------------------------------------------------------------------------------------//

	// set alignment info
	// pivotWinNodeName은 자신의 조상 노드 중에서 가장 가까운 MkBaseWindowNode 계열 노드
	// (NOTE) pivot node의 window(outside alignment)/client(inside alignment) rect가 명확하지 않으면
	// 정렬시 문제가 생길 수 있으므로 적절한 window/client 영역 설정이 필요
	bool SetAlignment(const MkHashStr& pivotWinNodeName, eRectAlignmentPosition alignment, const MkInt2& border);

	// get alignment info
	inline eRectAlignmentPosition GetAlignmentType(void) const { return m_AlignmentType; }
	inline const MkHashStr& GeTargetAlignmentWindowName(void) const { return m_TargetAlignmentWindowName; }
	inline const MkFloat2& GetAlignmentBorder(void) const { return m_AlignmentBorder; }
	inline const MkBaseWindowNode* GetTargetAlignmentWindowNode(void) const { return m_TargetAlignmentWindowNode; }
	
	//------------------------------------------------------------------------------------------------//
	// window/client rect interface
	//------------------------------------------------------------------------------------------------//

	virtual const MkFloatRect& GetWindowRect(void) const { return m_WorldAABR; }
	virtual const MkFloatRect& GetClientRect(void) const { return m_WorldAABR; }

	//------------------------------------------------------------------------------------------------//
	// window preset
	//------------------------------------------------------------------------------------------------//

	// 자식으로 window preset이 적용된 component 노드 생성
	MkBaseWindowNode* CreateWindowPreset(const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& bodySize);

	// 자식으로 window preset이 적용된 노드들이 있으면 모두 테마 변경
	void SetPresetThemeName(const MkHashStr& themeName);

	// 자식으로 window preset이 적용된 component 노드가 있으면 크기 변경
	void SetPresetComponentBodySize(eS2D_WindowPresetComponent component, const MkFloat2& bodySize);

	// 정보 반환
	inline const MkHashStr& GetPresetThemeName(void) const { return m_PresetThemeName; }
	inline const MkFloat2& GetPresetBodySize(void) const { return m_PresetBodySize; }

	//------------------------------------------------------------------------------------------------//

	MkBaseWindowNode(const MkHashStr& name);
	virtual ~MkBaseWindowNode() {}

public:

	static void __GenerateBuildingTemplate(void);

	virtual void __UpdateWindow(const MkFloatRect& rootRegion);

	inline void __SetThemeName(const MkHashStr& themeName) { m_PresetThemeName = themeName; }
	inline void __SetBodySize(const MkFloat2& bodySize) { m_PresetBodySize = bodySize; }

protected:

	// enable
	bool m_Enable;

	// alignment
	eRectAlignmentPosition m_AlignmentType;
	MkHashStr m_TargetAlignmentWindowName;
	MkFloat2 m_AlignmentBorder;
	MkBaseWindowNode* m_TargetAlignmentWindowNode;

	// window preset
	MkHashStr m_PresetThemeName;
	MkFloat2 m_PresetBodySize;
};
