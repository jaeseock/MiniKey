#pragma once


//------------------------------------------------------------------------------------------------//
// base window node : MkSceneNode
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
		eIgnoreInputEvent = 0, // input event�� ����

		eIgnoreMovement, // ����Ʈ ��� ��� ���� Ŀ�� �巡��, Ű���� �̵� ���� ����

		eDragMovement, // Ŀ�� �巡�׷� �̵�
		eConfinedToParent, // �̵��� �θ� �������� ���� ����(�θ� window node�� ������ ��ũ�� ����)
		eConfinedToScreen, // �̵��� ��ũ�� �������� ���� ����. eConfinedToRect���� �켱���� ����

		eDragToHandling // Ŀ�� �巡�׷� �ڵ鸵 ���. eDragMovement���� �켱���� ����
	};

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

public:

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_BaseWindowNode; }

	MkBaseWindowNode* GetAncestorWindowNode(void) const;

	// MkWindowEventManager�� ��ϵ� ancestor window ȯ
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
	MkBaseWindowNode* CreateBasicWindow(const MkHashStr& nodeName, const BasicPresetWindowDesc& desc);

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

	// �ش� ������ ��尡 window preset�� ����� component ���� icon(SRect) ����
	// (NOTE) ������ ���� ���׿� ���ؼ��� MkSRect�� �׸� ����
	bool SetPresetComponentIcon(const MkHashStr& iconName, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset, const MkPathName& imagePath, const MkHashStr& subsetName);
	bool SetPresetComponentIcon(const MkHashStr& iconName, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset, const MkHashStr& forcedType, const MkHashStr& forcedState, const MkStr& decoStr);
	bool SetPresetComponentIcon(const MkHashStr& iconName, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset, const MkHashStr& forcedType, const MkHashStr& forcedState, const MkArray<MkHashStr>& nodeNameAndKey);

	// �ش� ������ ��尡 eS2D_TitleState, eS2D_WindowState ��� window preset�� ����� component ���� highlight/normal icon(SRect) ����
	bool SetPresetComponentIcon(bool highlight, eRectAlignmentPosition alignment, const MkFloat2& border, const MkPathName& imagePath, const MkHashStr& subsetName);
	bool SetPresetComponentIcon(bool highlight, eRectAlignmentPosition alignment, const MkFloat2& border, const MkStr& decoStr);
	bool SetPresetComponentIcon(bool highlight, eRectAlignmentPosition alignment, const MkFloat2& border, const MkArray<MkHashStr>& nodeNameAndKey);

	// �ش� ������ ��尡 eS2D_TitleState, eS2D_WindowState ��� window preset�� ����� component ���� caption ����, caption�� ������� ����
	bool SetPresetComponentCaption(const MkHashStr& themeName, const MkStr& caption, eRectAlignmentPosition alignment = eRAP_MiddleCenter, const MkFloat2& border = MkFloat2(0.f, 0.f));
	bool SetPresetComponentCaption(const MkHashStr& themeName, const MkArray<MkHashStr>& caption, eRectAlignmentPosition alignment = eRAP_MiddleCenter, const MkFloat2& border = MkFloat2(0.f, 0.f));

	// ���� ��ȯ
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
	virtual bool InputEventMousePress(unsigned int button, const MkFloat2& position, bool managedRoot);
	virtual bool InputEventMouseRelease(unsigned int button, const MkFloat2& position, bool managedRoot);
	virtual bool InputEventMouseDoubleClick(unsigned int button, const MkFloat2& position, bool managedRoot);
	virtual bool InputEventMouseWheelMove(int delta, const MkFloat2& position, bool managedRoot);
	virtual void InputEventMouseMove(bool inside, bool (&btnPushing)[3], const MkFloat2& position, bool managedRoot);

	// clickWindow���� left click �߻�
	// (NOTE) eS2D_WindowState �迭���� �ƴ� ��� MkBaseWindowNode�� ������� ��(title, background, etc...)
	virtual void OnLeftClick(const MkFloat2& position) {} // clickWindow ���
	virtual void OnLeftClick(MkBaseWindowNode* clickWindow, const MkFloat2& position) {} // managed root ���

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

	inline void __SetFullSize(const MkFloat2& size) { m_PresetFullSize = size; }

	MkBaseWindowNode* __CreateWindowPreset(const MkHashStr& nodeName, const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& bodySize);

	MkBaseWindowNode* __GetFrontHitWindow(const MkFloat2& position);
	void __GetHitWindows(const MkFloat2& position, MkPairArray<float, MkBaseWindowNode*>& hitWindows); // all hits

protected:

	bool _CollectUpdatableWindowNodes(MkArray<MkBaseWindowNode*>& buffer);
	bool _CollectUpdatableWindowNodes(const MkFloat2& position, MkArray<MkBaseWindowNode*>& buffer); // + position check & enable

	void _DeletePresetCaption(void);

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
