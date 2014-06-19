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
		eIgnoreFocus = 0, // focus window�� �� �� ����
		eDragMovement, // Ŀ�� �巡�� �̵�(window rect)
		eConfinedToParent, // �̵��� �θ� �������� ���� ����(�θ� window node�� ������ ��ũ�� ����)
		eConfinedToScreen, // �̵��� ��ũ�� �������� ���� ����. eConfinedToRect���� �켱���� ����
		eDragToHandling, // Ŀ�� �巡�׷� �̹��� �̵� ���
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
		MkHashStr themeName; // preset theme �̸�

		// title
		bool hasTitle; // title ���翩��
		float titleHeight; // title ����
		bool hasIcon; // title ������. �⺻������ ���� �߾ӿ� margin��ŭ ������ ���ĵ�
		MkPathName iconImageFilePath; // title �������� image file ���
		MkHashStr iconImageSubsetName; // title �������� subset name
		float iconImageHeightOffset; // title ������ ���� �� �߰� y�� ��ġ offset. ������ ũ�Ⱑ ������ �����ɰ� ������ Ÿ��Ʋ���� Ŭ �� �ֱ� ����
		bool hasCancelIcon; // cancel icon ���翩��
		bool dragMovement; // title bar �巡������ ������ �̵� ���� ����

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

	// �ڽ����� window preset�� ����� component ��� ����
	MkBaseWindowNode* CreateWindowPreset(const MkHashStr& nodeName, const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& bodySize);

	// CreateWindowPreset()���� eS2D_WPC_BackgroundWindow�� �ش�Ǵ� ���� preset�� �ƴ� free image�� ����
	// (NOTE) theme ����� �ƴϱ� ������ SetPresetThemeName()�� ������ ���� ����. ��� SetFreeImageToBackgroundWindow()�� ���� ����
	// (NOTE) MkWindowTypeImageSet::eSingleType ���·� �����ǹǷ� ũ�Ⱑ image ũ��� ���� �ǰ� SetPresetComponentBodySize()�� ������ ���� ����
	MkBaseWindowNode* CreateFreeImageBaseBackgroundWindow(const MkHashStr& nodeName, const MkPathName& imagePath, const MkHashStr& subsetName);

	// BasicPresetWindowDesc�� ����� �⺻ ������ ����
	MkBaseWindowNode* CreateBasicWindow(const MkHashStr& nodeName, const BasicPresetWindowDesc& desc);

	// �ش� ������ ���� ��� �ڽ� ������ ��忡 window preset�� ����� ������ ������ ��� �׸� ����
	void SetPresetThemeName(const MkHashStr& themeName);

	// �ش� ������ ��尡 window preset�� ����� component ���� ũ�� ����
	// (NOTE) MkWindowTypeImageSet::eSingleType image set ������� ������ component�� ������ ���� ����(image ũ��� ����)
	void SetPresetComponentBodySize(const MkFloat2& bodySize);

	// �ش� ������ ��尡 CreateFreeImageBaseBackgroundWindow()���� ������ �������� ��� �̹��� ����
	bool SetFreeImageToBackgroundWindow(const MkPathName& imagePath, const MkHashStr& subsetName);

	// �ش� ������ ��尡 eS2D_TitleState ��� window preset�� ����� component ���� �ش� state�� ��ū(SRect) ����
	// (NOTE) ������ ���� ���׿� ���ؼ��� MkSRect�� �׸� ����
	bool SetPresetComponentToken(eS2D_TitleState state, eRectAlignmentPosition alignment, const MkFloat2& border, const MkBaseTexturePtr& texture, const MkHashStr& subsetName); // SetTexture()
	bool SetPresetComponentToken(eS2D_TitleState state, eRectAlignmentPosition alignment, const MkFloat2& border, const MkStr& decoStr); // SetDecoString()
	bool SetPresetComponentToken(eS2D_TitleState state, eRectAlignmentPosition alignment, const MkFloat2& border, const MkArray<MkHashStr>& nodeNameAndKey); // SetDecoString()

	// �ش� ������ ��尡 eS2D_WindowState ��� window preset�� ����� component ���� �ش� state�� ��ū(SRect) ����
	// (NOTE) ������ ���� ���׿� ���ؼ��� MkSRect�� �׸� ����
	bool SetPresetComponentToken(eS2D_WindowState state, eRectAlignmentPosition alignment, const MkFloat2& border, const MkBaseTexturePtr& texture, const MkHashStr& subsetName); // SetTexture()
	bool SetPresetComponentToken(eS2D_WindowState state, eRectAlignmentPosition alignment, const MkFloat2& border, const MkStr& decoStr); // SetDecoString()
	bool SetPresetComponentToken(eS2D_WindowState state, eRectAlignmentPosition alignment, const MkFloat2& border, const MkArray<MkHashStr>& nodeNameAndKey); // SetDecoString()

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
