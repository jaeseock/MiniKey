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

		eIgnoreMovement, // ����Ʈ ��� ��� ���� Ŀ�� �巡��, Ű���� �̵� ���� ����

		eDragMovement, // Ŀ�� �巡�׷� �̵�
		eConfinedToParent, // �̵��� �θ� �������� ���� ����(�θ� window node�� ������ ��ũ�� ����)
		eConfinedToScreen, // �̵��� ��ũ�� �������� ���� ����. eConfinedToRect���� �켱���� ����

		eDragToHandling // Ŀ�� �巡�׷� �ڵ鸵 ���. eDragMovement���� �켱���� ����
	};

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
		float titleHeight; // title ����
		bool hasIcon; // title ������. �⺻������ ���� �߾ӿ� margin��ŭ ������ ���ĵ�
		MkPathName iconImageFilePath; // title �������� image file ���
		MkHashStr iconImageSubsetName; // title �������� subset name
		float iconImageHeightOffset; // title ������ ���� �� �߰� y�� ��ġ offset. ������ ũ�Ⱑ ������ �����ɰ� ������ Ÿ��Ʋ���� Ŭ �� �ֱ� ����
		MkStr titleCaption;
		bool hasCancelIcon; // cancel icon ���翩��

		// background
		bool hasFreeImageBG; // bg�� preset�� �ƴ� free image�� �������� ����
		MkPathName bgImageFilePath; // free BG image file ���
		MkHashStr bgImageSubsetName; // free BG image subset name
		MkFloat2 windowSize; // ������ ũ��. hasFreeImageBG�� true�� �̹��� ũ�⸦ ������ false�� �ݵ�� ���� �Ǿ� �־�� ��

		// ok(possitive) button
		bool hasOKButton;

		// cancel(negative) button
		bool hasCancelButton;

		BasicPresetWindowDesc();
		~BasicPresetWindowDesc() {}
	};

	//------------------------------------------------------------------------------------------------//

	// caption ǥ�� ���� ����
	// msg(MkStr) Ȥ�� deco text�� ���ǵ� nodeNameAndKey �� �� �ϳ��� ǥ��
	class CaptionDesc
	{
	public:
		void SetString(const MkStr& msg); // SetNameList()�� ��Ÿ��
		void SetNameList(const MkArray<MkHashStr>& nameList); // SetString()�� ��Ÿ��

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

	// MkWindowEventManager�� ��ϵ� ancestor window ��ȯ
	MkBaseWindowNode* GetManagedRoot(void) const;

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
	bool CreateWindowPreset(const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& bodySize);

	// CreateWindowPreset()���� eS2D_WPC_BackgroundWindow�� �ش�Ǵ� ���� preset�� �ƴ� free image�� ����
	// �̹� component�� ����Ǿ� ������ false ��ȯ
	// (NOTE) theme ����� �ƴϱ� ������ SetPresetThemeName()�� ������ ���� ����. ��� SetFreeImageToBackgroundWindow()�� ���� ����
	// (NOTE) MkWindowTypeImageSet::eSingleType ���·� �����ǹǷ� ũ�Ⱑ image ũ��� ���� �ǰ� SetPresetComponentBodySize()�� ������ ���� ����
	bool CreateFreeImageBaseBackgroundWindow(const MkPathName& imagePath, const MkHashStr& subsetName);

	// �ش� ������ ���� ��� �ڽ� ������ ��忡 window preset�� ����� ������ ������ ��� �׸� ����
	virtual void SetPresetThemeName(const MkHashStr& themeName);

	// �ش� ������ ��尡 window preset�� ����� component ���� ũ�� ����
	// (NOTE) MkWindowTypeImageSet::eSingleType image set ������� ������ component�� ������ ���� ����(image ũ��� ����)
	void SetPresetComponentBodySize(const MkFloat2& bodySize);

	// �ش� ������ ��尡 CreateFreeImageBaseBackgroundWindow()���� ������ �������� ��� �̹��� ����
	bool SetFreeImageToBackgroundWindow(const MkPathName& imagePath, const MkHashStr& subsetName);

	// �ش� ������ ��尡 window preset�� ����� component ���� �Ϲ� icon(SRect) ����
	// (NOTE) ������ ���� ���׿� ���ؼ��� MkSRect�� �׸� ����
	bool SetPresetComponentIcon(const MkHashStr& iconName, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset, const MkPathName& imagePath, const MkHashStr& subsetName);
	bool SetPresetComponentIcon(const MkHashStr& iconName, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset, const MkHashStr& forcedState, const CaptionDesc& captionDesc);

	// �ش� ������ ��尡 eS2D_TitleState, eS2D_WindowState ��� window preset�� ����� component ���� highlight/normal icon(SRect) ����
	bool SetPresetComponentIcon(bool highlight, eRectAlignmentPosition alignment, const MkFloat2& border, const MkPathName& imagePath, const MkHashStr& subsetName);
	bool SetPresetComponentIcon(bool highlight, eRectAlignmentPosition alignment, const MkFloat2& border, const CaptionDesc& captionDesc);

	// �ش� ������ ��尡 eS2D_TitleState, eS2D_WindowState ��� window preset�� ����� component ���� highlight/normal caption ����, captionDesc�� ������� ����
	bool SetPresetComponentCaption(const MkHashStr& themeName, const CaptionDesc& captionDesc, eRectAlignmentPosition alignment = eRAP_MiddleCenter, const MkFloat2& border = MkFloat2(0.f, 0.f));

	// �ش� ������ ��尡 window preset�� ����� component ���� �׸��� ��ȯ
	inline const MkHashStr& GetPresetThemeName(void) const { return m_PresetThemeName; }

	// �ش� ������ ��尡 window preset�� ����� component ���� �������� component ��ȯ
	inline const MkHashStr& GetPresetComponentName(void) const { return m_PresetComponentName; }
	eS2D_WindowPresetComponent GetPresetComponentType(void) const;

	// �ش� ������ ��尡 window preset�� ����� component ���� margin�� ������� ���� ũ�� ��ȯ
	inline const MkFloat2& GetPresetBodySize(void) const { return m_PresetBodySize; }

	// �ش� ������ ��尡 window preset�� ����� component ���� margin���� ����� ũ�� ��ȯ
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
	virtual bool InputEventMousePress(unsigned int button, const MkFloat2& position, bool managedRoot);
	virtual bool InputEventMouseRelease(unsigned int button, const MkFloat2& position, bool managedRoot);
	virtual bool InputEventMouseDoubleClick(unsigned int button, const MkFloat2& position, bool managedRoot);
	virtual bool InputEventMouseWheelMove(int delta, const MkFloat2& position, bool managedRoot);
	virtual void InputEventMouseMove(bool inside, bool (&btnPushing)[3], const MkFloat2& position, bool managedRoot);

	// input ó���� ����ǰ� focus window�� ��� ȣ���
	virtual void UpdateManagedRoot(void);

	// activation
	virtual void Activate(void) {}
	virtual void Deactivate(void) {}

	// focus
	virtual void OnFocus(bool managedRoot);
	virtual void LostFocus(bool managedRoot);

	//------------------------------------------------------------------------------------------------//

	MkBaseWindowNode(const MkHashStr& name);
	virtual ~MkBaseWindowNode() {}

public:

	static void __GenerateBuildingTemplate(void);

	static MkBaseWindowNode* __CreateWindowPreset
		(MkSceneNode* parentNode, const MkHashStr& nodeName, const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& bodySize);

	inline void __SetFullSize(const MkFloat2& size) { m_PresetFullSize = size; }

	inline void __PushEvent(const WindowEvent& evt) { m_WindowEvents.PushBack(evt); }

	MkBaseWindowNode* __GetFrontHitWindow(const MkFloat2& position);
	void __GetHitWindows(const MkFloat2& position, MkPairArray<float, MkBaseWindowNode*>& hitWindows); // all hits

protected:

	bool _CollectUpdatableWindowNodes(MkArray<MkBaseWindowNode*>& buffer);
	bool _CollectUpdatableWindowNodes(const MkFloat2& position, MkArray<MkBaseWindowNode*>& buffer); // + position check & enable

	virtual bool _CheckCursorHitCondition(const MkFloat2& position) const;
	virtual bool _CheckWheelMoveCondition(const MkFloat2& position) const { return false; }

	void _PushWindowEvent(MkSceneNodeFamilyDefinition::eWindowEvent type);

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

	// event
	MkArray<WindowEvent> m_WindowEvents;
};

MKDEF_DECLARE_FIXED_SIZE_TYPE(MkBaseWindowNode::WindowEvent)