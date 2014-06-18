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
#include "MkCore_MkPairArray.h"

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

	// 해당 윈도우 노드가 eS2D_TitleState 기반 window preset이 적용된 component 노드면 해당 state에 토큰(SRect) 설정
	// (NOTE) 각각의 설정 사항에 대해서는 MkSRect의 항목 참조
	bool SetPresetComponentToken(eS2D_TitleState state, eRectAlignmentPosition alignment, const MkFloat2& border, const MkBaseTexturePtr& texture, const MkHashStr& subsetName); // SetTexture()
	bool SetPresetComponentToken(eS2D_TitleState state, eRectAlignmentPosition alignment, const MkFloat2& border, const MkStr& decoStr); // SetDecoString()
	bool SetPresetComponentToken(eS2D_TitleState state, eRectAlignmentPosition alignment, const MkFloat2& border, const MkArray<MkHashStr>& nodeNameAndKey); // SetDecoString()

	// 해당 윈도우 노드가 eS2D_WindowState 기반 window preset이 적용된 component 노드면 해당 state에 토큰(SRect) 설정
	// (NOTE) 각각의 설정 사항에 대해서는 MkSRect의 항목 참조
	bool SetPresetComponentToken(eS2D_WindowState state, eRectAlignmentPosition alignment, const MkFloat2& border, const MkBaseTexturePtr& texture, const MkHashStr& subsetName); // SetTexture()
	bool SetPresetComponentToken(eS2D_WindowState state, eRectAlignmentPosition alignment, const MkFloat2& border, const MkStr& decoStr); // SetDecoString()
	bool SetPresetComponentToken(eS2D_WindowState state, eRectAlignmentPosition alignment, const MkFloat2& border, const MkArray<MkHashStr>& nodeNameAndKey); // SetDecoString()

	// 정보 반환
	inline const MkHashStr& GetPresetThemeName(void) const { return m_PresetThemeName; }
	inline const MkHashStr& GetPresetComponentName(void) const { return m_PresetComponentName; }
	inline const MkFloat2& GetPresetBodySize(void) const { return m_PresetBodySize; }
	inline const MkFloat2& GetPresetFullSize(void) const { return m_PresetFullSize; }

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	inline void SetAttribute(eAttribute attribute, bool enable) { m_Attribute.Assign(attribute, enable); }
	inline bool GetAttribute(eAttribute attribute) const { return m_Attribute.Check(attribute); }

	//------------------------------------------------------------------------------------------------//
	// event call
	//------------------------------------------------------------------------------------------------//

	// input
	virtual void InputEventKeyPress(unsigned int keyCode) {}
	virtual void InputEventKeyRelease(unsigned int keyCode) {}
	virtual void InputEventMousePress(unsigned int button, const MkFloat2& position); // sample : how to get hit window?
	virtual void InputEventMouseRelease(unsigned int button, const MkFloat2& position) {}
	virtual void InputEventMouseDoubleClick(unsigned int button, const MkFloat2& position) {}
	virtual void InputEventMouseWheelMove(int delta, const MkFloat2& position) {}
	virtual void InputEventMouseMove(bool inside, bool (&btnPushing)[3], const MkFloat2& position);

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

	inline bool __CheckFocusingTarget(void) const { return (GetVisible() && GetEnable() && (!GetAttribute(eIgnoreFocus))); }
	inline bool __CheckInputTarget(const MkFloat2& cursorPosition) const { return (__CheckFocusingTarget() && GetWorldAABR().CheckIntersection(cursorPosition)); }

	inline void __SetFullSize(const MkFloat2& size) { m_PresetFullSize = size; }

	void __GetHitWindows(const MkFloat2& position, MkPairArray<float, MkBaseWindowNode*>& hitWindows);

protected:

	bool _CollectUpdatableWindowNodes(bool skipCondition, MkArray<MkBaseWindowNode*>& buffer);
	bool _CollectUpdatableWindowNodes(const MkFloat2& position, MkArray<MkBaseWindowNode*>& buffer);

	const MkHashStr& _GetFrontHitWindowName(const MkFloat2& position);

protected:

	// enable
	bool m_Enable;

	// window preset
	MkHashStr m_PresetThemeName;
	MkHashStr m_PresetComponentName;
	MkFloat2 m_PresetBodySize;
	MkFloat2 m_PresetFullSize;
	
	// attribute
	MkBitFieldDW m_Attribute;
};
