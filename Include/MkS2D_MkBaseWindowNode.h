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
		eIgnoreFocus = 0, // focus window�� �� �� ����
		eDragMovement, // Ŀ�� �巡�� �̵�(window rect)
		eConfinedToParent, // �̵��� �θ� �������� ���� ����(�θ� window node�� ������ ��ũ�� ����)
		eConfinedToScreen, // �̵��� ��ũ�� �������� ���� ����. eConfinedToRect���� �켱���� ����
		eDragToHandling, // Ŀ�� �巡�׷� �̹��� �̵� ���
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
	// alignment
	//------------------------------------------------------------------------------------------------//

	// set alignment info
	// - pivotWinNodeName�� �ڽ��� ���� ��� �߿��� ���� ����� MkBaseWindowNode �迭 ���
	// - ���� ������ pivotWinNodeName�� �������� �ʴ´ٸ� Update�� �־����� rootRegion�� �������� ��
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

	// �ڽ����� window preset�� ����� component ��� ����
	MkBaseWindowNode* CreateWindowPreset(const MkHashStr& nodeName, const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& bodySize);

	// �ش� ������ ���� ��� �ڽ� ������ ��忡 window preset�� ����� ������ ������ ��� �׸� ����
	void SetPresetThemeName(const MkHashStr& themeName);

	// �ش� ������ ��尡 window preset�� ����� component ���� ũ�� ����
	void SetPresetComponentBodySize(const MkFloat2& bodySize);

	// ���� ��ȯ
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
