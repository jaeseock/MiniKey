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
		eIgnoreFocus = 0, // focus window�� �� �� ����
		eDragMovement, // Ŀ�� �巡�� �̵�(window rect)
		eConfinedToRect, // Ŀ�� �巡�� �̵��� ��ũ�� ���� ����(�θ� ������ �θ��� client rect, ������ ��ũ�� ����)
		eDragToHandling, // Ŀ�� �巡�׷� �̹��� �̵� ���

		// edit mode ����
		eArrowKeyMovement // ȭ��ǥ Ű �̵�
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
	// - outside alignment�� pivot node�� window rect��, inside alignment�� client rect ����
	// - ���� ������ pivotWinNodeName�� �������� �ʴ´ٸ� Update�� �־����� rootRegion�� �������� ��
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

	// �ڽ����� window preset�� ����� component ��� ����
	MkBaseWindowNode* CreateWindowPreset(const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& bodySize);

	// window preset�� ����� �ڽ� component ��� ��ȯ
	MkSceneNode* GetWindowPresetNode(eS2D_WindowPresetComponent component);

	// window preset ���� component type ��ȯ
	eS2D_WindowPresetComponent GetWindowPresetComponentType(void) const;

	// �ڽ����� window preset�� ����� ������ ������ ��� �׸� ����
	void SetPresetThemeName(const MkHashStr& themeName);

	// �ڽ����� window preset�� ����� component ��尡 ������ ũ�� ����
	void SetPresetComponentBodySize(eS2D_WindowPresetComponent component, const MkFloat2& bodySize);

	// ���� ��ȯ
	inline const MkHashStr& GetPresetThemeName(void) const { return m_PresetThemeName; }
	inline const MkFloat2& GetPresetBodySize(void) const { return m_PresetBodySize; }

	//------------------------------------------------------------------------------------------------//
	// component state
	//------------------------------------------------------------------------------------------------//

	// �ڽ����� eS2D_WPC_BackgroundWindow�� ����� ��尡 ������ ����
	// (NOTE) ����� eS2D_BackgroundState�� �ϳ��� ���¸� �����ϱ⶧���� �ǹ� ����
	void SetComponentState(eS2D_BackgroundState state) {}

	// �ڽ����� eS2D_WPC_TitleWindow�� ����� ��尡 ������ ����
	void SetComponentState(eS2D_TitleState state);

	// �ڽ����� eS2D_WindowState�� �ش�Ǵ� component�� ����� ��尡 ������ ����
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
