#pragma once


//------------------------------------------------------------------------------------------------//
// base window node : MkSceneNode
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBitFieldDW.h"
#include "MkCore_MkPairArray.h"
#include "MkCore_MkPathName.h"

#include "MkS2D_MkSceneNode.h"
#include "MkS2D_MkSceneNodeFamilyDefinition.h"


class MkBaseWindowNode : public MkSceneNode
{
public:

	enum eAttribute
	{
		eIgnoreInputEvent = 0, // input event를 무시

		eIgnoreMovement, // 일반/에디트 모드 상관 없이 커서 드래그, 키보드 이동 등을 무시

		eDragMovement, // 커서 드래그로 이동
		eConfinedToParent, // 이동시 부모의 AABR 영역으로 범위 제한(부모 window node가 없으면 스크린 영역)
		eConfinedToScreen, // 이동시 스크린 영역으로 범위 제한. eConfinedToRect보다 우선순위 높음

		eDragToHandling, // 커서 드래그로 핸들링 허용. eDragMovement보다 우선순위 높음

		eForEditMode, // edit mode용 윈도우

		eAlignCenterPos // activate시 중앙 정렬
	};

	//------------------------------------------------------------------------------------------------//

	// 기본 윈도우 생성 정보 정의
	class BasicPresetWindowDesc
	{
	public:

		// 샘플 이미지를 background로 생성
		void SetStandardDesc(const MkHashStr& themeName, bool hasTitle);

		// 테마 윈도우셋으로 생성
		void SetStandardDesc(const MkHashStr& themeName, bool hasTitle, const MkFloat2& windowSize);

		// 주어진 이미지를 background로 생성
		void SetStandardDesc(const MkHashStr& themeName, bool hasTitle, const MkPathName& bgFilePath, const MkHashStr& subsetName);

	public:

		// theme
		MkHashStr themeName; // preset theme 이름
		bool dragMovement; // root window(title bar or body)드래깅으로 윈도우 이동 가능 여부

		// title
		bool hasTitle; // title 존재여부
		float titleHeight; // title 높이
		bool hasIcon; // title 아이콘. 기본적으로 왼쪽 중앙에 margin만큼 떨어져 정렬됨
		MkPathName iconImageFilePath; // title 아이콘의 image file 경로
		MkHashStr iconImageSubsetName; // title 아이콘의 subset name
		float iconImageHeightOffset; // title 아이콘 정렬 후 추가 y축 위치 offset. 아이콘 크기가 작으면 문제될게 없지만 타이틀보다 클 수 있기 때문
		MkStr titleCaption;
		bool hasCloseIcon; // close icon 존재여부

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

	//------------------------------------------------------------------------------------------------//

	// caption 표시 정보 정의
	// msg(MkStr) 혹은 deco text에 정의된 nodeNameAndKey 둘 중 하나를 표시
	class CaptionDesc
	{
	public:
		void SetString(const MkStr& msg); // SetNameList()와 배타적
		void SetNameList(const MkArray<MkHashStr>& nameList); // SetString()과 배타적

		inline CaptionDesc& operator = (const MkStr& msg) { SetString(msg); return *this; }
		inline CaptionDesc& operator = (const MkArray<MkHashStr>& nameList) { SetNameList(nameList); return *this; }

		void __Load(const MkStr& defaultMsg, const MkArray<MkStr>& buffer);
		void __Save(MkArray<MkStr>& buffer) const;
		void __Check(const MkStr& defaultMsg);

		inline const MkStr& GetString(void) const { return m_Str; }
		inline const MkArray<MkHashStr>& GetNameList(void) const { return m_NameList; }

		CaptionDesc() {}
		CaptionDesc(const MkStr& msg) { SetString(msg); }
		CaptionDesc(const MkArray<MkHashStr>& nameList) { SetNameList(nameList); }
		~CaptionDesc() {}

	protected:
		MkStr m_Str;
		MkArray<MkHashStr> m_NameList;
	};

	//------------------------------------------------------------------------------------------------//

	typedef struct _WindowEvent
	{
		MkSceneNodeFamilyDefinition::eWindowEvent type;
		MkBaseWindowNode* node;

		_WindowEvent() {}
		_WindowEvent(MkSceneNodeFamilyDefinition::eWindowEvent targetType, MkBaseWindowNode* targetNode)
		{
			type = targetType;
			node = targetNode;
		}
	}
	WindowEvent;

	//------------------------------------------------------------------------------------------------//

public:

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_BaseWindowNode; }

	MkBaseWindowNode* GetAncestorWindowNode(void) const;

	// MkWindowEventManager에 등록된 ancestor window 반환
	MkBaseWindowNode* GetRootWindow(void) const;

	inline void DeclareRootWindow(void) { m_RootWindow = true; }
	inline bool CheckRootWindow(void) const { return m_RootWindow; }

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

	// BasicPresetWindowDesc을 사용해 기본 윈도우 생성
	// targetWindow가 NULL이면 새로 생성해 반환하고 존재하면 해당 윈도우에 생성(반환되는 윈도우와 동일)
	//
	// ex>
	//	MkBaseWindowNode* titleWin = MkBaseWindowNode::CreateBasicWindow(NULL, L"WinRoot", winDesc);
	// 위의 코드는 아래 코드와 같다
	//	MkBaseWindowNode* titleWin = new MkBaseWindowNode(L"WinRoot");
	//	MkBaseWindowNode* tmpWin = MkBaseWindowNode::CreateBasicWindow(titleWin, MkHashStr::NullHash, winDesc); // titleWin과 tmpWin은 동일
	//
	// 대신 이 방식은 확장 윈도우 객체를 생성 후 사용할 수 있다. 즉 이렇게 된다.
	//	NewWindowNode* titleWin = new NewWindowNode(L"WinRoot"); // NewWindowNode는 MkBaseWindowNode를 상속받은 class
	//	MkBaseWindowNode* tmpWin = MkBaseWindowNode::CreateBasicWindow(titleWin, MkHashStr::NullHash, winDesc);
	static MkBaseWindowNode* CreateBasicWindow(MkBaseWindowNode* targetWindow, const MkHashStr& nodeName, const BasicPresetWindowDesc& desc);

	// 해당 윈도우 노드에 window preset이 적용된 component 노드 적용
	// 이미 component가 적용되어 있으면 false 반환
	bool CreateWindowPreset(const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& componentSize);

	// preset(theme)이 아닌 free image로 background 생성. component로 eS2D_WPC_BackgroundWindow가 적용 됨
	// 이미 component가 적용되어 있으면 false 반환
	// (NOTE) theme 기반이 아니기 때문에 SetPresetThemeName()의 적용을 받지 않음. 대신 SetFreeImageToBackgroundWindow()로 변경 가능
	// (NOTE) MkWindowTypeImageSet::eSingleType 형태로 생성되므로 크기가 image 크기로 고정 되고 SetPresetComponentSize()의 적용을 받지 않음
	bool CreateFreeImageBaseBackgroundWindow(const MkPathName& imagePath, const MkHashStr& subsetName);

	// preset(theme)이 아닌 free image로 button 생성. component로 eS2D_WPC_NormalButton이 적용 됨
	// 이미 component가 적용되어 있으면 false 반환
	// subsetNames에는 eS2D_WS_MaxWindowState만큼 eS2D_WindowState 순서대로 subset name이 들어가 있어야 함
	// (NOTE) theme 기반이 아니기 때문에 SetPresetThemeName()의 적용을 받지 않음
	// (NOTE) MkWindowTypeImageSet::eSingleType 형태로 생성되므로 크기가 image 크기로 고정 되고 SetPresetComponentSize()의 적용을 받지 않음
	bool CreateFreeImageBaseButtonWindow(const MkPathName& imagePath, const MkArray<MkHashStr>& subsetNames);

	// 해당 윈도우 노드와 모든 자식 윈도우 노드에 window preset이 적용된 노드들이 있으면 모두 테마 변경
	virtual void SetPresetThemeName(const MkHashStr& themeName);

	// 해당 윈도우 노드가 window preset이 적용된 component 노드면 크기 변경
	// (NOTE) MkWindowTypeImageSet::eSingleType image set 기반으로 생성된 component면 적용을 받지 않음(image 크기로 고정)
	void SetPresetComponentSize(const MkFloat2& componentSize);

	// 해당 윈도우 노드가 CreateFreeImageBaseBackgroundWindow()으로 생성된 윈도우일 경우 이미지 변경
	bool SetFreeImageToBackgroundWindow(const MkPathName& imagePath, const MkHashStr& subsetName);

	// 해당 윈도우 노드가 window preset이 적용된 component 노드면 일반 icon(SRect) 설정
	// (NOTE) 각각의 설정 사항에 대해서는 MkSRect의 항목 참조
	bool SetPresetComponentIcon(const MkHashStr& iconName, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset, const MkPathName& imagePath, const MkHashStr& subsetName);
	bool SetPresetComponentIcon(const MkHashStr& iconName, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset, const MkHashStr& forcedState, const CaptionDesc& captionDesc);

	// 해당 윈도우 노드가 eS2D_TitleState, eS2D_WindowState 기반 window preset이 적용된 component 노드면 highlight/normal icon(SRect) 설정
	bool SetPresetComponentIcon(bool highlight, eRectAlignmentPosition alignment, const MkFloat2& border, const MkPathName& imagePath, const MkHashStr& subsetName);
	bool SetPresetComponentIcon(bool highlight, eRectAlignmentPosition alignment, const MkFloat2& border, const CaptionDesc& captionDesc);

	// 해당 윈도우 노드가 eS2D_TitleState, eS2D_WindowState 기반 window preset이 적용된 component 노드면 highlight/normal caption 설정, captionDesc가 비었으면 삭제
	bool SetPresetComponentCaption(const MkHashStr& themeName, const CaptionDesc& captionDesc, eRectAlignmentPosition alignment = eRAP_MiddleCenter, const MkFloat2& border = MkFloat2(0.f, 0.f));

	// 해당 윈도우 노드가 window preset이 적용된 component 노드면 테마명 반환
	inline const MkHashStr& GetPresetThemeName(void) const { return m_PresetThemeName; }

	// 해당 윈도우 노드가 window preset이 적용된 component 노드면 적용중인 component 반환
	inline const MkHashStr& GetPresetComponentName(void) const { return m_PresetComponentName; }
	eS2D_WindowPresetComponent GetPresetComponentType(void) const;

	// 해당 윈도우 노드가 window preset이 적용된 component 노드면 영역 크기 반환
	inline const MkFloat2& GetPresetComponentSize(void) const { return m_PresetComponentSize; }

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	inline void SetAttribute(eAttribute attribute, bool enable) { m_Attribute.Assign(attribute, enable); }
	inline bool GetAttribute(eAttribute attribute) const { return m_Attribute.Check(attribute); }

	//------------------------------------------------------------------------------------------------//
	// event call
	//------------------------------------------------------------------------------------------------//

	// root -> leaf 순회 input event
	// (NOTE) 특수한 상황이 아니면 윈도우별 처리는 하단의 HitEventMouse...(), 혹은 UseWindowEvent()를 사용하길 권장
	virtual void InputEventKeyPress(unsigned int keyCode) {}
	virtual void InputEventKeyRelease(unsigned int keyCode) {}
	virtual bool InputEventMousePress(unsigned int button, const MkFloat2& position);
	virtual bool InputEventMouseRelease(unsigned int button, const MkFloat2& position);
	virtual bool InputEventMouseDoubleClick(unsigned int button, const MkFloat2& position);
	virtual bool InputEventMouseWheelMove(int delta, const MkFloat2& position);
	virtual void InputEventMouseMove(bool inside, bool (&btnPushing)[3], const MkFloat2& position);

	// InputEventMouse...() 순회중 해당 윈도우가 HitEvent...() 호출 될 조건(hit condition)
	// 기본적으로 이미 AABR 체크는 만족한 상태
	virtual bool CheckCursorHitCondition(const MkFloat2& position) const;
	virtual bool CheckWheelMoveCondition(const MkFloat2& position) const { return true; }

	// hit condition을 만족한 윈도우의 input event는 여기서 처리
	virtual bool HitEventMousePress(unsigned int button, const MkFloat2& position) { return false; } // CheckCursorHitCondition
	virtual bool HitEventMouseRelease(unsigned int button, const MkFloat2& position) { return false; } // CheckCursorHitCondition
	virtual bool HitEventMouseDoubleClick(unsigned int button, const MkFloat2& position) { return false; } // CheckCursorHitCondition
	virtual bool HitEventMouseWheelMove(int delta, const MkFloat2& position) { return false; } // CheckWheelMoveCondition

	// 마우스 드래그가 시작했을 경우 대상 윈도우(targetWindow)를 넣어 호출된 윈도우부터 root window까지 연쇄적으로 호출
	virtual void StartDragMovement(MkBaseWindowNode* targetWindow);

	// 마우스 드래그가 발생했을 경우 대상 윈도우(targetWindow)와 시작 지점부터의 이동벡터(offset)를 넣어 호출된 윈도우부터 root window까지 연쇄적으로 호출
	virtual bool ConfirmDragMovement(MkBaseWindowNode* targetWindow, MkFloat2& positionOffset);

	// 하위 윈도우들이 발생시킨 window event가 존재 할 경우 호출됨
	virtual void UseWindowEvent(WindowEvent& evt) {}
	
	// activation
	virtual void Activate(void);
	virtual void Deactivate(void) {}

	// focus
	virtual void OnFocus(void);
	virtual void LostFocus(void);

	//------------------------------------------------------------------------------------------------//
	// MkSceneNode
	//------------------------------------------------------------------------------------------------//

	virtual bool AttachChildNode(MkSceneNode* childNode);

	virtual bool DetachChildNode(const MkHashStr& childNodeName);

	virtual void Clear(void);

	//------------------------------------------------------------------------------------------------//

	MkBaseWindowNode(const MkHashStr& name);
	virtual ~MkBaseWindowNode() {}

public:

	static void __GenerateBuildingTemplate(void);

	static MkBaseWindowNode* __CreateWindowPreset
		(MkSceneNode* parentNode, const MkHashStr& nodeName, const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& componentSize);

	inline void __SetPresetComponentSize(const MkFloat2& size) { m_PresetComponentSize = size; }

	inline void __PushEvent(const WindowEvent& evt) { m_WindowEvents.PushBack(evt); }
	
	MkBaseWindowNode* __GetFrontHitWindow(const MkFloat2& position);
	void __GetHitWindows(const MkFloat2& position, MkPairArray<float, MkBaseWindowNode*>& hitWindows); // all hits

	void __ConsumeWindowEvent(void);

	// for edit mode
	void __BuildInformationTree(MkBaseWindowNode* targetParentNode, unsigned int depth, MkArray<MkBaseWindowNode*>& buffer);
	unsigned int __CountTotalWindowBasedChildren(void) const;
	inline MkBaseWindowNode* __GetWindowBasedChild(unsigned int index) { return m_ChildWindows.IsValidIndex(index) ? m_ChildWindows[index] : NULL; }

protected:

	bool _CollectUpdatableWindowNodes(const MkFloat2& position, MkArray<MkBaseWindowNode*>& buffer); // + position check & enable

	void _PushWindowEvent(MkSceneNodeFamilyDefinition::eWindowEvent type);

protected:

	bool m_RootWindow;

	// enable
	bool m_Enable;

	// window preset
	MkHashStr m_PresetThemeName;
	MkHashStr m_PresetComponentName;
	MkFloat2 m_PresetComponentSize;
	
	// attribute
	MkBitFieldDW m_Attribute;

	// child window
	MkArray<MkBaseWindowNode*> m_ChildWindows;

	// event
	MkArray<WindowEvent> m_WindowEvents;
};

MKDEF_DECLARE_FIXED_SIZE_TYPE(MkBaseWindowNode::WindowEvent)