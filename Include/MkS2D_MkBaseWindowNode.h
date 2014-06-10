#pragma once


//------------------------------------------------------------------------------------------------//
// base window node : MkSceneNode
//	+ enable
//	+ alignment
//	+ window/client rect interface
//	+ window preset
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkSceneNode.h"


class MkBaseWindowNode : public MkSceneNode
{
public:

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

	virtual void SetEnable(bool enable) { m_Enable = enable; }
	inline bool GetEnable(void) const { return m_Enable; }

	//------------------------------------------------------------------------------------------------//
	// alignment
	//------------------------------------------------------------------------------------------------//

	// set alignment info
	// pivotWinNodeName�� �ڽ��� ���� ��� �߿��� ���� ����� MkBaseWindowNode �迭 ���
	// (NOTE) pivot node�� window(outside alignment)/client(inside alignment) rect�� ��Ȯ���� ������
	// ���Ľ� ������ ���� �� �����Ƿ� ������ window/client ���� ������ �ʿ�
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

	// �ڽ����� window preset�� ����� ������ ������ ��� �׸� ����
	void SetPresetThemeName(const MkHashStr& themeName);

	// �ڽ����� window preset�� ����� component ��尡 ������ ũ�� ����
	void SetPresetComponentBodySize(eS2D_WindowPresetComponent component, const MkFloat2& bodySize);

	// ���� ��ȯ
	inline const MkHashStr& GetPresetThemeName(void) const { return m_PresetThemeName; }
	inline const MkFloat2& GetPresetBodySize(void) const { return m_PresetBodySize; }

	//------------------------------------------------------------------------------------------------//

	MkBaseWindowNode(const MkHashStr& name);
	virtual ~MkBaseWindowNode() {}

public:

	static void __GenerateBuildingTemplate(void);

	virtual void __UpdateWindow(const MkFloatRect& rootRegion);

	inline void __SetThemeName(const MkHashStr& themeName) { m_PresetThemeName = themeName; }
	inline void __SetBodySize(const MkFloat2& bodySize) { m_PresetBodySize = bodySize; }

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
};
