#pragma once


//------------------------------------------------------------------------------------------------//
// window visual pattern node
// window system�� visual part�� base class
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkSceneNode.h"


class MkVisualPatternNode : public MkSceneNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_VisualPatternNode; }

	// alloc child instance
	static MkVisualPatternNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// region
	//------------------------------------------------------------------------------------------------//

	// client rect ��ȯ
	inline const MkFloatRect& GetClientRect(void) const { return m_ClientRect; }

	// window rect ��ȯ
	inline const MkFloatRect& GetWindowRect(void) const { return m_WindowRect; }

	//------------------------------------------------------------------------------------------------//
	// alignment(within parent client)
	// (NOTE) �θ� MkVisualPatternNode �� �Ļ� class instance�� ��쿡�� ��ȿ
	//------------------------------------------------------------------------------------------------//

	// ������ �Ǵ� �θ��� rect�� client���� window���� ���� ����/��ȯ. default�� client rect(false)
	void SetAlignmentPivotIsWindowRect(bool enable);
	inline bool GetAlignmentPivotIsWindowRect(void) const { return m_Attribute[ePA_SNA_AlignmentPivotIsWindowRect]; }

	// ����� �Ǵ� �ڽ��� rect�� client���� window���� ���� ����/��ȯ. default�� window rect(true)
	void SetAlignmentTargetIsWindowRect(bool enable);
	inline bool GetAlignmentTargetIsWindowRect(void) const { return m_Attribute[ePA_SNA_AlignmentTargetIsWindowRect]; }

	// ���� ���
	void SetAlignmentPosition(eRectAlignmentPosition position);
	inline eRectAlignmentPosition GetAlignmentPosition(void) const { return m_AlignmentPosition; }

	// ���� �� �߰� position offset
	void SetAlignmentOffset(const MkFloat2& offset);
	inline const MkFloat2& GetAlignmentOffset(void) const { return m_AlignmentOffset; }

	// Update()�� ���� ���
	inline void SetAlignmentCommand(void) { m_UpdateCommand.Set(eUC_Alignment); }

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	// �Է� ��뿩��. default�� false
	inline void SetAcceptInput(bool enable) { m_Attribute.Assign(ePA_SNA_AcceptInput, enable); }
	inline bool GetAcceptInput(void) const { return m_Attribute[ePA_SNA_AcceptInput]; }

	//------------------------------------------------------------------------------------------------//
	// proceed
	//------------------------------------------------------------------------------------------------//

	virtual void Update(double currTime = 0.);

	MKDEF_DECLARE_SCENE_OBJECT_HEADER; // MkSceneObject

	MkVisualPatternNode(const MkHashStr& name);
	virtual ~MkVisualPatternNode() {}

protected:

	void _UpdateAlignment(void);

	//------------------------------------------------------------------------------------------------//
	// update command
	//------------------------------------------------------------------------------------------------//

	enum eVisualPatternNodeUpdateCommand
	{
		eUC_Alignment = 0,

		eUC_VisualPatternNodeBandwidth
	};

	virtual void _ExcuteUpdateCommand(void);

protected:

	// region
	MkFloatRect m_ClientRect;
	MkFloatRect m_WindowRect;

	// align
	eRectAlignmentPosition m_AlignmentPosition;
	MkFloat2 m_AlignmentOffset;

	// update command
	MkBitField32 m_UpdateCommand;

public:

	static const MkHashStr ObjKey_AlignPosition;
	static const MkHashStr ObjKey_AlignOffset;
};
