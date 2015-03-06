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

	void SetAlignmentPosition(eRectAlignmentPosition position);
	inline eRectAlignmentPosition GetAlignmentPosition(void) const { return m_AlignmentPosition; }

	void SetAlignmentOffset(const MkFloat2& offset);
	inline const MkFloat2& GetAlignmentOffset(void) const { return m_AlignmentOffset; }

	//------------------------------------------------------------------------------------------------//
	// proceed
	//------------------------------------------------------------------------------------------------//

	virtual void Update(double currTime = 0.);

	//------------------------------------------------------------------------------------------------//
	// attribute. data�� ����Ǵ� ���̹Ƿ� �뿪�� Ȯ�� �߿�
	//------------------------------------------------------------------------------------------------//

	enum eVisualPatternNodeAttribute
	{
		// �Է� ��� ����
		eAT_AcceptInput = eAT_SceneNodeBandwidth,

		// 4bit �뿪�� Ȯ��
		eAT_VisualPatternNodeBandwidth = eAT_SceneNodeBandwidth + 4
	};

	// �Է� ��뿩��. default�� false
	inline void SetAcceptInput(bool enable) { m_Attribute.Assign(eAT_AcceptInput, enable); }
	inline bool GetAcceptInput(void) const { return m_Attribute[eAT_AcceptInput]; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	enum eVisualPatternNodeEventType
	{
		eET_VisualPatternNodeBandwidth = eET_SceneNodeBandwidth, // ����
	};

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
	eRectAlignmentPosition m_AlignmentPosition;
	MkFloat2 m_AlignmentOffset;

	// update command
	MkBitField32 m_UpdateCommand;

public:

	static const MkStr NamePrefix;
};
