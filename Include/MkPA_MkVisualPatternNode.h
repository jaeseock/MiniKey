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
	inline bool GetAlignmentPivotIsWindowRect(void) const { return m_AlignmentPivotIsWindowRect; }

	void SetAlignmentPosition(eRectAlignmentPosition position);
	inline eRectAlignmentPosition GetAlignmentPosition(void) const { return m_AlignmentPosition; }

	void SetAlignmentOffset(const MkFloat2& offset);
	inline const MkFloat2& GetAlignmentOffset(void) const { return m_AlignmentOffset; }

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

	MkVisualPatternNode(const MkHashStr& name);
	virtual ~MkVisualPatternNode() {}

	// ������ position, offset���� parentNode�� client rect�� ���� ���� ����
	// (NOTE) parentNode�� client rect, �ڽ��� window rect�� ��ȿ�ؾ� ��
	void __UpdateAlignment(const MkVisualPatternNode* parentNode);
	void __UpdateAlignment(void);

	//------------------------------------------------------------------------------------------------//
	// update command
	//------------------------------------------------------------------------------------------------//

protected:

	enum eVisualPatternNodeUpdateCommand
	{
		eUC_Alignment = 0,

		eUC_VisualPatternNodeBandwidth,
	};

	virtual void _ExcuteUpdateCommand(void);

protected:

	// region
	MkFloatRect m_ClientRect;
	MkFloatRect m_WindowRect;

	// align
	bool m_AlignmentPivotIsWindowRect;
	eRectAlignmentPosition m_AlignmentPosition;
	MkFloat2 m_AlignmentOffset;

	// update command
	MkBitField32 m_UpdateCommand;

public:

	static const MkStr NamePrefix;
};
