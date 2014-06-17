#pragma once


//------------------------------------------------------------------------------------------------//
// base window node : MkSceneNode
//	+ enable
//	+ alignment
//	+ window rect interface
//	+ window preset
//	+ component state(eS2D_BackgroundState, eS2D_TitleState, eS2D_WindowState)
//	+ attribute
//	+ event call
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBitFieldDW.h"
#include "MkS2D_MkSceneNode.h"


class MkBaseWindowNode : public MkSceneNode
{
public:

	enum eAttribute
	{
		eIgnoreFocus = 0, // focus window가 될 수 없음
		eDragMovement, // 커서 드래그 이동(window rect)
		eConfinedToParent, // 이동시 부모 영역으로 범위 제한(부모 window node가 없으면 스크린 영역)
		eConfinedToScreen, // 이동시 스크린 영역으로 범위 제한. eConfinedToRect보다 우선순위 높음
		eDragToHandling, // 커서 드래그로 이미지 이동 허용
	};

public:

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_BaseWindowNode; }

	MkBaseWindowNode* GetAncestorWindowNode(void) const;

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

	void SetEnable(bool enable);
	inline bool GetEnable(void) const { return m_Enable; }

	//------------------------------------------------------------------------------------------------//
	// alignment
	//------------------------------------------------------------------------------------------------//

	// set alignment info
	// - pivotWinNodeName은 자신의 조상 노드 중에서 가장 가까운 MkBaseWindowNode 계열 노드
	// - 만약 적합한 pivotWinNodeName가 존재하지 않는다면 Update시 주어지는 rootRegion을 기준으로 함
	bool SetAlignment(const MkHashStr& pivotWinNodeName, eRectAlignmentPosition alignment, const MkInt2& border);

	// get alignment info
	inline eRectAlignmentPosition GetAlignmentType(void) const { return m_AlignmentType; }
	inline const MkHashStr& GeTargetAlignmentWindowName(void) const { return m_TargetAlignmentWindowName; }
	inline const MkFloat2& GetAlignmentBorder(void) const { return m_AlignmentBorder; }
	inline const MkBaseWindowNode* GetTargetAlignmentWindowNode(void) const { return m_TargetAlignmentWindowNode; }
	
	//------------------------------------------------------------------------------------------------//
	// window rect interface
	//------------------------------------------------------------------------------------------------//

	const MkFloatRect& GetWindowRect(void) const;

	//------------------------------------------------------------------------------------------------//
	// window preset
	//------------------------------------------------------------------------------------------------//

	// 자식으로 window preset이 적용된 component 노드 생성
	MkBaseWindowNode* CreateWindowPreset(const MkHashStr& nodeName, const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& bodySize);

	// 해당 윈도우 노드와 모든 자식 윈도우 노드에 window preset이 적용된 노드들이 있으면 모두 테마 변경
	void SetPresetThemeName(const MkHashStr& themeName);

	// 해당 윈도우 노드가 window preset이 적용된 component 노드면 크기 변경
	void SetPresetComponentBodySize(const MkFloat2& bodySize);

	// 정보 반환
	inline const MkHashStr& GetPresetThemeName(void) const { return m_PresetThemeName; }
	inline const MkFloat2& GetPresetBodySize(void) const { return m_PresetBodySize; }
	inline const MkHashStr& GetPresetComponentName(void) const { return m_PresetComponentName; }

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	inline void SetAttribute(eAttribute attribute, bool enable) { m_Attribute.Assign(attribute, enable); }
	inline bool GetAttribute(eAttribute attribute) const { return m_Attribute.Check(attribute); }

	//------------------------------------------------------------------------------------------------//
	// event call
	//------------------------------------------------------------------------------------------------//

	// input
	virtual bool InputEventKeyPress(unsigned int keyCode) { return false; }
	virtual bool InputEventKeyRelease(unsigned int keyCode) { return false; }
	virtual bool InputEventMousePress(unsigned int button, const MkFloat2& position);
	virtual bool InputEventMouseRelease(unsigned int button, const MkFloat2& position);
	virtual bool InputEventMouseDoubleClick(unsigned int button, const MkFloat2& position);
	virtual bool InputEventMouseWheelMove(int delta, const MkFloat2& position);
	virtual bool InputEventMouseMove(bool inside, bool (&btnPushing)[3], const MkFloat2& position);

	// activation
	virtual void Activate(void) {}
	virtual void Deactivate(void) {}

	// focus
	virtual void OnFocus(void);
	virtual void LostFocus(void);

	//------------------------------------------------------------------------------------------------//

	MkBaseWindowNode(const MkHashStr& name);
	MkBaseWindowNode(const MkHashStr& name, const MkHashStr& themeName, const MkFloat2& bodySize, const MkHashStr& componentName);
	virtual ~MkBaseWindowNode() {}

public:

	static void __GenerateBuildingTemplate(void);

	virtual void __UpdateWindow(const MkFloatRect& rootRegion);

	inline bool __CheckFocusingTarget(void) const { return (GetVisible() && GetEnable() && (!GetAttribute(eIgnoreFocus))); }
	inline bool __CheckInputTarget(const MkFloat2& cursorPosition) const { return (__CheckFocusingTarget() && GetWorldAABR().CheckIntersection(cursorPosition)); }

	void __GetFrontHitWindow(const MkFloat2& position, MkBaseWindowNode* (&frontWindow)[2]);

protected:

	bool _CollectUpdatableWindowNodes(bool skipCondition, MkArray<MkBaseWindowNode*>& buffer);
	bool _CollectUpdatableWindowNodes(const MkFloat2& position, MkArray<MkBaseWindowNode*>& buffer);

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
	MkHashStr m_PresetComponentName;

	// attribute
	MkBitFieldDW m_Attribute;
};
