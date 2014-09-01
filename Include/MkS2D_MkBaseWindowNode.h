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
		eIgnoreInputEvent = 0, // input event�� ����

		eIgnoreMovement, // �Ϲ�/����Ʈ ��� ��� ���� Ŀ�� �巡��, Ű���� �̵� ���� ����

		eSystemWindow, // �ý��� ������

		eDragMovement, // Ŀ�� �巡�׷� �̵�
		eConfinedToParent, // �̵��� �θ��� AABR �������� ���� ����(�θ� window node�� ������ ��ũ�� ����)
		eConfinedToScreen, // �̵��� ��ũ�� �������� ���� ����. eConfinedToRect���� �켱���� ����

		eAllowItemDrag, // ������ �巡�� ���. eDragMovement���� �켱���� ����
		eAllowItemDrop, // ������ ��� ���

		eAlignCenterPos, // activate�� �߾� ����

		eShowActionCursor, // cursor over�� action cursor ǥ�� ����

		eMaxAttribute
	};

	//------------------------------------------------------------------------------------------------//

	// caption ǥ�� ���� ����
	// msg(MkStr) Ȥ�� deco text�� ���ǵ� nodeNameAndKey �� �� �ϳ��� ǥ��
	class CaptionDesc
	{
	public:
		void SetString(const MkStr& msg); // SetNameList()�� ��Ÿ��
		void SetNameList(const MkArray<MkHashStr>& nameList); // SetString()�� ��Ÿ��

		inline bool GetEnable(void) const { return ((!m_Str.Empty()) || (!m_NameList.Empty())); }

		inline CaptionDesc& operator = (const MkStr& msg) { SetString(msg); return *this; }
		inline CaptionDesc& operator = (const MkArray<MkHashStr>& nameList) { SetNameList(nameList); return *this; }

		void __Load(const MkStr& defaultMsg, const MkArray<MkStr>& buffer);
		void __Save(MkArray<MkStr>& buffer) const;
		void __Check(const MkStr& defaultMsg);

		inline const MkStr& GetString(void) const { return m_Str; }
		inline const MkArray<MkHashStr>& GetNameList(void) const { return m_NameList; }

		inline void Clear(void) { m_Str.Clear(); m_NameList.Clear(); }

		CaptionDesc() {}
		CaptionDesc(const MkStr& msg) { SetString(msg); }
		CaptionDesc(const MkArray<MkHashStr>& nameList) { SetNameList(nameList); }
		~CaptionDesc() {}

	protected:
		MkStr m_Str;
		MkArray<MkHashStr> m_NameList;
	};

	//------------------------------------------------------------------------------------------------//

	typedef struct _ItemTagInfo
	{
		MkPathName iconPath;
		MkHashStr iconSubset;
		CaptionDesc captionDesc;
	}
	ItemTagInfo;

	//------------------------------------------------------------------------------------------------//

	// �⺻ ������ ���� ���� ����
	class BasicPresetWindowDesc
	{
	public:

		// ���� �̹����� background�� ����
		void SetStandardDesc(const MkHashStr& themeName, bool hasTitle);

		// �׸� ����������� ����
		void SetStandardDesc(const MkHashStr& themeName, bool hasTitle, const MkFloat2& windowSize);

		// �־��� �̹����� background�� ����
		void SetStandardDesc(const MkHashStr& themeName, bool hasTitle, const MkPathName& bgFilePath, const MkHashStr& subsetName);

	public:

		// theme
		MkHashStr themeName; // preset theme �̸�
		bool dragMovement; // root window(title bar or body)�巡������ ������ �̵� ���� ����

		// title
		bool hasTitle; // title ���翩��
		eS2D_WindowPresetComponent titleType; // title component type. default�� eS2D_WPC_NormalTitle
		float titleHeight; // title ����
		bool hasIcon; // title ������. �⺻������ ���� �߾ӿ� margin��ŭ ������ ���ĵ�
		MkPathName iconImageFilePath; // title �������� image file ���
		MkHashStr iconImageSubsetName; // title �������� subset name
		float iconImageHeightOffset; // title ������ ���� �� �߰� y�� ��ġ offset. ������ ũ�Ⱑ ������ �����ɰ� ������ Ÿ��Ʋ���� Ŭ �� �ֱ� ����
		CaptionDesc titleCaption;
		bool hasCloseButton; // close button ���翩��

		// background
		bool hasFreeImageBG; // bg�� preset�� �ƴ� free image�� �������� ����
		MkPathName bgImageFilePath; // free BG image file ���
		MkHashStr bgImageSubsetName; // free BG image subset name
		MkFloat2 windowSize; // ������ ũ��. hasFreeImageBG�� true�� �̹��� ũ�⸦ ������ false�� �ݵ�� ���� �Ǿ� �־�� ��

		// ok(possitive) button
		bool hasOKButton;
		CaptionDesc okBtnCaption;

		// cancel(negative) button
		bool hasCancelButton;
		CaptionDesc cancelBtnCaption;

		BasicPresetWindowDesc();
		~BasicPresetWindowDesc() {}
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

	// MkWindowEventManager�� ��ϵ� ancestor window ��ȯ
	MkBaseWindowNode* GetRootWindow(void) const;

	inline void DeclareRootWindow(void) { m_RootWindow = true; }
	inline bool CheckRootWindow(void) const { return m_RootWindow; }

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	// MkDataNode�� ����. MkSceneNode()�� ���� ��Ģ�� ����
	virtual void Load(const MkDataNode& node);

	// MkDataNode�� ���
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

	// BasicPresetWindowDesc�� ����� �⺻ ������ ����
	// targetWindow�� NULL�̸� ���� ������ ��ȯ�ϰ� �����ϸ� �ش� �����쿡 ����(��ȯ�Ǵ� ������� ����)
	//
	// ex>
	//	MkBaseWindowNode* titleWin = MkBaseWindowNode::CreateBasicWindow(NULL, L"WinRoot", winDesc);
	// ���� �ڵ�� �Ʒ� �ڵ�� ����
	//	MkBaseWindowNode* titleWin = new MkBaseWindowNode(L"WinRoot");
	//	MkBaseWindowNode* tmpWin = MkBaseWindowNode::CreateBasicWindow(titleWin, MkHashStr::NullHash, winDesc); // titleWin�� tmpWin�� ����
	//
	// ��� �� ����� Ȯ�� ������ ��ü�� ���� �� ����� �� �ִ�. �� �̷��� �ȴ�.
	//	NewWindowNode* titleWin = new NewWindowNode(L"WinRoot"); // NewWindowNode�� MkBaseWindowNode�� ��ӹ��� class
	//	MkBaseWindowNode* tmpWin = MkBaseWindowNode::CreateBasicWindow(titleWin, MkHashStr::NullHash, winDesc);
	static MkBaseWindowNode* CreateBasicWindow(MkBaseWindowNode* targetWindow, const MkHashStr& nodeName, const BasicPresetWindowDesc& desc);

	// �ش� ������ ��忡 window preset�� ����� component ��� ����
	// �̹� component�� ����Ǿ� ������ false ��ȯ
	bool CreateWindowPreset(const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& componentSize);

	// preset(theme)�� �ƴ� free image�� background ����. component�� eS2D_WPC_BackgroundWindow�� ���� ��
	// �̹� component�� ����Ǿ� ������ false ��ȯ
	// (NOTE) theme ����� �ƴϱ� ������ SetPresetThemeName()�� ������ ���� ����. ��� SetFreeImageToBackgroundWindow()�� ���� ����
	// (NOTE) MkWindowTypeImageSet::eSingleType ���·� �����ǹǷ� ũ�Ⱑ image ũ��� ���� �ǰ� SetPresetComponentSize()�� ������ ���� ����
	bool CreateFreeImageBaseBackgroundWindow(const MkPathName& imagePath, const MkHashStr& subsetName);

	// �ش� ������ ��尡 CreateFreeImageBaseBackgroundWindow()���� ������ �������� ��� �̹��� ����
	bool SetFreeImageToBackgroundWindow(const MkPathName& imagePath, const MkHashStr& subsetName, bool keepSize = false);

	// preset(theme)�� �ƴ� free image�� button ����. component�� eS2D_WPC_NormalButton�� ���� ��
	// �̹� component�� ����Ǿ� ������ false ��ȯ
	// subsetNames���� eS2D_WS_MaxWindowState��ŭ eS2D_WindowState ������� subset name�� �� �־�� ��
	// (NOTE) theme ����� �ƴϱ� ������ SetPresetThemeName()�� ������ ���� ����
	// (NOTE) MkWindowTypeImageSet::eSingleType ���·� �����ǹǷ� ũ�Ⱑ image ũ��� ���� �ǰ� SetPresetComponentSize()�� ������ ���� ����
	bool CreateFreeImageBaseButtonWindow(const MkPathName& imagePath, const MkArray<MkHashStr>& subsetNames);

	// �ش� ������ ��尡 CreateFreeImageBaseButtonWindow()���� ������ �������� ��� �̹��� ����
	bool SetFreeImageBaseButtonWindow(const MkPathName& imagePath, const MkArray<MkHashStr>& subsetNames, bool keepSize = false);
	bool SetFreeImageBaseButtonWindow(const MkPathName& imagePath, eS2D_WindowState state, const MkHashStr& subsetName, bool keepSize = false);

	// �ش� ������ ���� ��� �ڽ� ������ ��忡 window preset�� ����� ������ ������ ��� �׸� ����
	virtual void SetPresetThemeName(const MkHashStr& themeName);

	// �ش� ������ ��尡 window preset�� ����� component ���� �׸��� ��ȯ
	inline const MkHashStr& GetPresetThemeName(void) const { return m_PresetThemeName; }

	// �ش� ������ ��尡 window preset�� ����� component ���� ũ�� ���� ���ɿ��� ��ȯ
	// MkWindowTypeImageSet::eNull, eSingleType : false, false (��� �Ұ���)
	// MkWindowTypeImageSet::e3And1Type : true, false (���θ� ����)
	// MkWindowTypeImageSet::e1And3Type : false, true (���θ� ����)
	// MkWindowTypeImageSet::e3And3Type : true, true (��� ����)
	void CheckPresetComponentSizeAvailable(bool& width, bool& height) const;

	// �ش� ������ ��尡 window preset�� ����� component ���� ũ�� ����
	// (NOTE) componentSize�� x, y ��� ������ CheckPresetComponentSizeAvailable()�� ��Ģ�� ����
	virtual void SetPresetComponentSize(const MkFloat2& componentSize);

	// �ش� ������ ��尡 window preset�� ����� component ���� �Ϲ� icon(SRect) ����
	bool SetPresetComponentIcon(const MkHashStr& iconName, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset, const MkPathName& imagePath, const MkHashStr& subsetName);
	bool SetPresetComponentIcon(const MkHashStr& iconName, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset, const MkHashStr& forcedState, const CaptionDesc& captionDesc);

	// �ش� ������ ��忡 highlight/normal icon(SRect) ����
	// highlight�� true�� eS2D_TitleState, eS2D_WindowState ��� window preset�� ����� component ����� ��츸 ����, false�� ��� component ��� ����
	// highlight�� �⺻������ invisible ���·� �ʱ�ȭ��
	bool SetPresetComponentIcon(bool highlight, eRectAlignmentPosition alignment, const MkFloat2& border, const MkPathName& imagePath, const MkHashStr& subsetName);
	bool SetPresetComponentIcon(bool highlight, eRectAlignmentPosition alignment, const MkFloat2& border, const CaptionDesc& captionDesc);

	// �ش� ������ ��忡 highlight/normal caption ����, captionDesc�� ������� ����
	// ���� ��Ģ�� SetPresetComponentIcon()�� ����
	bool SetPresetComponentCaption(const MkHashStr& themeName, const CaptionDesc& captionDesc, eRectAlignmentPosition alignment = eRAP_MiddleCenter, const MkFloat2& border = MkFloat2(0.f, 0.f));

	// �ش� ������ ��忡 ItemTagInfo �ݿ�
	// - icon, Ȥ�� caption�� ���� : �߾� ����
	// - icon, caption ��� ���� : ���� ����
	virtual bool SetPresetComponentItemTag(const ItemTagInfo& tagInfo, bool deleteIconIfEmpty = true, bool deleteCaptionIfEmpty = true);

	// �ش� ������ ��尡 window preset�� ����� component ���� �������� component ��ȯ
	const MkHashStr& GetPresetComponentName(void) const;
	inline eS2D_WindowPresetComponent GetPresetComponentType(void) const { return m_PresetComponentType; }

	// �ش� ������ ��尡 window preset�� ����� component ���� ���� ũ�� ��ȯ
	inline const MkFloat2& GetPresetComponentSize(void) const { return m_PresetComponentSize; }

	// �ش� ������ ��尡 enable ������ window state ��� preset�� ����� component ���� eS2D_WS_DefaultState�� ��ȯ
	void SetPresetComponentWindowStateToDefault(void);

	// �ش� ������ ����� component state�� �ش��ϴ� ��� ��ȯ
	MkSceneNode* GetComponentStateNode(eS2D_BackgroundState state);
	MkSceneNode* GetComponentStateNode(eS2D_WindowState state);

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	inline void SetAttribute(eAttribute attribute, bool enable) { m_Attribute.Assign(attribute, enable); }
	inline bool GetAttribute(eAttribute attribute) const { return m_Attribute.Check(attribute); }

	//------------------------------------------------------------------------------------------------//
	// event call
	//------------------------------------------------------------------------------------------------//

	// root -> leaf ��ȸ input event
	// (NOTE) Ư���� ��Ȳ�� �ƴϸ� �����캰 ó���� �ϴ��� HitEventMouse...(), Ȥ�� UseWindowEvent()�� ����ϱ� ����
	virtual void InputEventKeyPress(unsigned int keyCode) {}
	virtual void InputEventKeyRelease(unsigned int keyCode) {}
	virtual bool InputEventMousePress(unsigned int button, const MkFloat2& position);
	virtual bool InputEventMouseRelease(unsigned int button, const MkFloat2& position);
	virtual bool InputEventMouseDoubleClick(unsigned int button, const MkFloat2& position);
	virtual bool InputEventMouseWheelMove(int delta, const MkFloat2& position);
	virtual void InputEventMouseMove(bool inside, bool (&btnPushing)[3], const MkFloat2& position);

	// InputEventMouse...() ��ȸ�� �ش� �����찡 HitEvent...() ȣ�� �� ����(hit condition)
	// �⺻������ �̹� AABR üũ�� ������ ����
	virtual bool CheckCursorHitCondition(const MkFloat2& position) const;
	virtual bool CheckWheelMoveCondition(const MkFloat2& position) const { return true; }

	// hit condition�� ������ �������� input event�� ���⼭ ó��
	virtual bool HitEventMousePress(unsigned int button, const MkFloat2& position) { return false; } // CheckCursorHitCondition
	virtual bool HitEventMouseRelease(unsigned int button, const MkFloat2& position) { return false; } // CheckCursorHitCondition
	virtual bool HitEventMouseDoubleClick(unsigned int button, const MkFloat2& position) { return false; } // CheckCursorHitCondition
	virtual bool HitEventMouseWheelMove(int delta, const MkFloat2& position) { return false; } // CheckWheelMoveCondition

	// ���콺 �巡�װ� �������� ��� ��� ������(targetWindow)�� �־� ȣ��� ��������� root window���� ���������� ȣ��
	virtual void StartDragMovement(MkBaseWindowNode* targetWindow);

	// ���콺 �巡�װ� �߻����� ��� ��� ������(targetWindow)�� ���� ���������� �̵�����(offset)�� �־� ȣ��� ��������� root window���� ���������� ȣ��
	virtual bool ConfirmDragMovement(MkBaseWindowNode* targetWindow, MkFloat2& positionOffset);

	// ���� ��������� �߻���Ų window event�� ���� �� ��� ȣ���
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

	virtual bool ChangeNodeName(const MkHashStr& newName);

	virtual bool ChangeChildNodeName(const MkHashStr& oldName, const MkHashStr& newName);

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

	inline void __SetCurrentComponentState(int state) { m_CurrentComponentState = state; }

	inline void __AddActiveWindowStateCounter(int offset) { m_ActiveWindowStateCounter += offset; }
	void __SetActiveWindowStateCounter(int offset);
	int __CountActiveWindowStateCounter(void) const;

	inline void __PushEvent(const WindowEvent& evt) { m_WindowEvents.PushBack(evt); }
	
	MkBaseWindowNode* __GetFrontHitWindow(const MkFloat2& position);
	void __GetHitWindows(const MkFloat2& position, MkPairArray<float, MkBaseWindowNode*>& hitWindows); // all hits

	void __ConsumeWindowEvent(void);

	// for edit mode
	void __BuildInformationTree(MkBaseWindowNode* targetParentNode, unsigned int depth, MkArray<MkBaseWindowNode*>& buffer);
	void __SetWindowInformation(MkBaseWindowNode* targetNode) const;
	unsigned int __CountTotalWindowBasedChildren(void) const;
	inline MkBaseWindowNode* __GetWindowBasedChild(unsigned int index) { return m_ChildWindows.IsValidIndex(index) ? m_ChildWindows[index] : NULL; }
	void __ClearCurrentTheme(void);
	void __ApplyDefaultTheme(void);

protected:

	bool _CollectUpdatableWindowNodes(const MkFloat2& position, MkArray<MkBaseWindowNode*>& buffer); // + position check & enable
	void _PushWindowEvent(MkSceneNodeFamilyDefinition::eWindowEvent type);
	bool _OnActiveWindowState(void) const;

protected:

	bool m_RootWindow;

	// enable
	bool m_Enable;

	// window preset
	MkHashStr m_PresetThemeName;
	eS2D_WindowPresetComponent m_PresetComponentType;
	MkFloat2 m_PresetComponentSize;

	// attribute
	MkBitFieldDW m_Attribute;

	// ���� component�� state
	int m_CurrentComponentState;

	// ���� on cursor/click���� �ڼ� window state component ����
	// InputEventMouseMove() ����ȭ�� ī����
	int m_ActiveWindowStateCounter;

	// child window
	MkArray<MkBaseWindowNode*> m_ChildWindows;

	// event
	MkArray<WindowEvent> m_WindowEvents;
};

MKDEF_DECLARE_FIXED_SIZE_TYPE(MkBaseWindowNode::WindowEvent)