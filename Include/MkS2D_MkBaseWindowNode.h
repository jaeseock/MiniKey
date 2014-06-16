#pragma once


//------------------------------------------------------------------------------------------------//
// base window node : MkSceneNode
//	+ enable
//	+ alignment
//	+ window/client rect interface
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
		eConfinedToRect, // 커서 드래그 이동시 스크린 영역 제한(부모가 있으면 부모의 client rect, 없으면 스크린 영역)
		eDragToHandling, // 커서 드래그로 이미지 이동 허용

		// edit mode 전용
		eArrowKeyMovement // 화살표 키 이동
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
	// - outside alignment는 pivot node의 window rect를, inside alignment는 client rect 기준
	// - 만약 적합한 pivotWinNodeName가 존재하지 않는다면 Update시 주어지는 rootRegion을 기준으로 함
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

	// window preset이 적용된 자식 component 노드 반환
	MkSceneNode* GetWindowPresetNode(eS2D_WindowPresetComponent component);

	// window preset 노드면 component type 반환
	eS2D_WindowPresetComponent GetWindowPresetComponentType(void) const;

	// 자식으로 window preset이 적용된 노드들이 있으면 모두 테마 변경
	void SetPresetThemeName(const MkHashStr& themeName);

	// 자식으로 window preset이 적용된 component 노드가 있으면 크기 변경
	void SetPresetComponentBodySize(eS2D_WindowPresetComponent component, const MkFloat2& bodySize);

	// 정보 반환
	inline const MkHashStr& GetPresetThemeName(void) const { return m_PresetThemeName; }
	inline const MkFloat2& GetPresetBodySize(void) const { return m_PresetBodySize; }

	//------------------------------------------------------------------------------------------------//
	// component state
	//------------------------------------------------------------------------------------------------//

	// 자식으로 eS2D_WPC_BackgroundWindow가 적용된 노드가 있으면 적용
	// (NOTE) 현재는 eS2D_BackgroundState가 하나의 상태만 존재하기때문에 의미 없음
	void SetComponentState(eS2D_BackgroundState state) {}

	// 자식으로 eS2D_WPC_TitleWindow가 적용된 노드가 있으면 적용
	void SetComponentState(eS2D_TitleState state);

	// 자식으로 eS2D_WindowState에 해당되는 component가 적용된 노드가 있으면 적용
	void SetComponentState(eS2D_WindowPresetComponent component, eS2D_WindowState state);

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	inline void SetAttribute(eAttribute attribute, bool enable) { m_Attribute.Assign(attribute, enable); }
	inline bool GetAttribute(eAttribute attribute) const { return m_Attribute.Check(attribute); }

	//------------------------------------------------------------------------------------------------//
	// event call
	//------------------------------------------------------------------------------------------------//

	// input
	virtual bool InputEventKeyPress(unsigned int keyCode);
	virtual bool InputEventKeyRelease(unsigned int keyCode);
	virtual bool InputEventMousePress(unsigned int button, const MkFloat2& position);
	virtual bool InputEventMouseRelease(unsigned int button, const MkFloat2& position);
	virtual bool InputEventMouseDoubleClick(unsigned int button, const MkFloat2& position);
	virtual bool InputEventMouseWheelMove(int delta, const MkFloat2& position);
	virtual bool InputEventMouseMove(bool inside, const MkFloat2& position);

	// activation
	virtual void Activate(void) {}
	virtual void Deactivate(void) {}

	// focus
	virtual void OnFocus(void) {}
	virtual void LostFocus(void) {}

	//------------------------------------------------------------------------------------------------//

	MkBaseWindowNode(const MkHashStr& name);
	virtual ~MkBaseWindowNode() {}

public:

	static void __GenerateBuildingTemplate(void);

	virtual void __UpdateWindow(const MkFloatRect& rootRegion);

	inline void __SetThemeName(const MkHashStr& themeName) { m_PresetThemeName = themeName; }
	inline void __SetBodySize(const MkFloat2& bodySize) { m_PresetBodySize = bodySize; }

	inline bool __CheckFocusingTarget(void) const { return (GetVisible() && GetEnable() && (!GetAttribute(eIgnoreFocus))); }
	inline bool __CheckInputTarget(const MkFloat2& cursorPosition) const { return (__CheckFocusingTarget() && GetWorldAABR().CheckIntersection(cursorPosition)); }

protected:

	bool _GetInputUpdatableNodes(bool skipCondition, MkArray<MkBaseWindowNode*>& buffer);
	bool _GetInputUpdatableNodes(const MkFloat2& position, MkArray<MkBaseWindowNode*>& buffer);

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

	// attribute
	MkBitFieldDW m_Attribute;
};
