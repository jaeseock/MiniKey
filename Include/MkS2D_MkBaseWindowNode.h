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
#include "MkCore_MkPathName.h"

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

	class BasicPresetWindowDesc
	{
	public:

		void SetStandardDesc(const MkHashStr& themeName, const MkFloat2& windowSize);
		void SetStandardDesc(const MkHashStr& themeName, const MkPathName& bgFilePath, const MkHashStr& subsetName);
		void SetNoneTitleDesc(const MkHashStr& themeName, const MkFloat2& windowSize, bool dragMovement = false);
		void SetNoneTitleDesc(const MkHashStr& themeName, const MkPathName& bgFilePath, const MkHashStr& subsetName, bool dragMovement = false);

	public:

		// theme
		MkHashStr themeName; // preset theme 이름

		// title
		bool hasTitle; // title 존재여부
		float titleHeight; // title 높이
		bool hasIcon; // title 아이콘. 기본적으로 왼쪽 중앙에 margin만큼 떨어져 정렬됨
		MkPathName iconImageFilePath; // title 아이콘의 image file 경로
		MkHashStr iconImageSubsetName; // title 아이콘의 subset name
		float iconImageHeightOffset; // title 아이콘 정렬 후 추가 y축 위치 offset. 아이콘 크기가 작으면 문제될게 없지만 타이틀보다 클 수 있기 때문
		bool hasCancelIcon; // cancel icon 존재여부
		bool dragMovement; // title bar 드래깅으로 윈도우 이동 가능 여부

		// background
		bool hasFreeImageBG; // bg를 preset이 아닌 free image로 구성할지 여부
		MkPathName bgImageFilePath; // free BG image file 경로
		MkHashStr bgImageSubsetName; // free BG image subset name
		MkFloat2 windowSize; // 윈도우 크기. hasFreeImageBG가 true면 이미지 크기를 따르고 false면 반드시 지정 되어 있어야 함

		// ok(possitive) button
		bool hasOKButton;

		// cancel(negative) button
		bool hasCancelButton;

		BasicPresetWindowDesc();
		~BasicPresetWindowDesc() {}
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

	// CreateWindowPreset()에서 eS2D_WPC_BackgroundWindow만 해당되는 경우로 preset이 아닌 free image로 생성
	// (NOTE) theme 기반이 아니기 때문에 SetPresetThemeName()의 적용을 받지 않음. 대신 SetFreeImageToBackgroundWindow()로 변경 가능
	// (NOTE) MkWindowTypeImageSet::eSingleType 형태로 생성되므로 크기가 image 크기로 고정 되고 SetPresetComponentBodySize()의 적용을 받지 않음
	MkBaseWindowNode* CreateFreeImageBaseBackgroundWindow(const MkHashStr& nodeName, const MkPathName& imagePath, const MkHashStr& subsetName);

	// BasicPresetWindowDesc을 사용해 기본 윈도우 생성
	MkBaseWindowNode* CreateBasicWindow(const MkHashStr& nodeName, const BasicPresetWindowDesc& desc);

	// 해당 윈도우 노드와 모든 자식 윈도우 노드에 window preset이 적용된 노드들이 있으면 모두 테마 변경
	void SetPresetThemeName(const MkHashStr& themeName);

	// 해당 윈도우 노드가 window preset이 적용된 component 노드면 크기 변경
	// (NOTE) MkWindowTypeImageSet::eSingleType image set 기반으로 생성된 component면 적용을 받지 않음(image 크기로 고정)
	void SetPresetComponentBodySize(const MkFloat2& bodySize);

	// 해당 윈도우 노드가 CreateFreeImageBaseBackgroundWindow()으로 생성된 윈도우일 경우 이미지 변경
	bool SetFreeImageToBackgroundWindow(const MkPathName& imagePath, const MkHashStr& subsetName);

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
