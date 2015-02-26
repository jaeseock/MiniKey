#pragma once


//------------------------------------------------------------------------------------------------//
// window visual pattern node
// window system�� base node
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkSceneNode.h"


class MkVisualPatternNode : public MkSceneNode
{
public:

	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_VisualPatternNode; }

	//------------------------------------------------------------------------------------------------//
	// region
	//------------------------------------------------------------------------------------------------//

	// client size ����
	virtual void SetClientSize(const MkFloat2& clientSize) { m_ClientRect.size = clientSize; }

	// client rect ��ȯ
	inline const MkFloatRect& GetClientRect(void) const { return m_ClientRect; }

	// window size ��ȯ
	inline const MkFloat2& GetWindowSize(void) const { return m_WindowSize; }

	//------------------------------------------------------------------------------------------------//
	// ����
	// (NOTE) ȣ�� �� ��ȿ�� window rect�� ���� �Ǿ� �ִ� �����̾�� ��
	//------------------------------------------------------------------------------------------------//

	// ��ȿ�� target rect�� alignmentPosition�� ��ġ�ϵ��� local position�� ����
	void SnapTo(const MkFloatRect& targetRect, eRectAlignmentPosition alignmentPosition);

	// �θ� MkVisualPatternNode �� ���� class�� ��� �θ� clirent rect�� alignmentPosition�� ��ġ�ϵ��� local position�� ����
	// �θ� ���ų� �ִ��� MkSceneNode�� ��� ����
	void SnapToParentClientRect(eRectAlignmentPosition alignmentPosition);

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	enum eVisualPatternNodeAttribute
	{
		eAT_AcceptInput = eAT_SceneNodeBandwidth, // �Է� ��� ����

		eAT_VisualPatternNodeBandwidth = eAT_SceneNodeBandwidth + 4 // 4bit �뿪�� Ȯ��
	};

	// �Է� ��뿩��. default�� false
	inline void SetAcceptInput(bool enable) { m_Attribute.Assign(eAT_AcceptInput, enable); }
	inline bool GetAcceptInput(void) const { return m_Attribute[eAT_AcceptInput]; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

protected:

	enum eVisualPatternNodeEventType
	{
		eET_VisualPatternNodeBandwidth = 0 // event ����
	};

	//------------------------------------------------------------------------------------------------//

	MkVisualPatternNode(const MkHashStr& name) : MkSceneNode(name) {}
	virtual ~MkVisualPatternNode() {}

protected:

	MkFloatRect m_ClientRect; // size : ����, position : �ֹ�
	MkFloat2 m_WindowSize; // �ֹ�

public:

	static const MkStr NamePrefix;
};
