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
	// transform
	//------------------------------------------------------------------------------------------------//

	virtual void SetLocalPosition(const MkFloat2& position);

	//------------------------------------------------------------------------------------------------//
	// region
	//------------------------------------------------------------------------------------------------//

	// client size ����
	virtual void SetClientSize(const MkFloat2& clientSize) { m_WindowSize = m_ClientRect.size = clientSize; }

	// client rect ��ȯ
	inline const MkFloatRect& GetClientRect(void) const { return m_ClientRect; }

	// window size ��ȯ
	inline const MkFloat2& GetWindowSize(void) const { return m_WindowSize; }

	//------------------------------------------------------------------------------------------------//
	// ����
	// (NOTE) ȣ�� �� ��ȿ�� window rect�� ���� �Ǿ� �ִ� �����̾�� ��
	//------------------------------------------------------------------------------------------------//

	// ���� ��ġ ����
	// UpdateAlignmentPosition()�� ȣ��Ǹ� targetRect�� position�� ���ĵ� �� offset �ݿ�
	// �⺻ ���� eRAP_NonePosition, MkFloat2::Zero
	void SetAlignmentPosition(eRectAlignmentPosition position, const MkFloat2& offset = MkFloat2::Zero);

	// ��ȿ�� target rect�� ���� ������ position, offset�� �ݿ�
	// (NOTE) eAT_RestrictedWithinParentClient ������ ����(�θ� �ܺη� ���� �� ��� ������ �θ��� client rect�� ���߾��� �� ����)
	void UpdateAlignmentPosition(const MkFloatRect& targetRect);

	// �θ� MkVisualPatternNode �� ���� class�� ��� �θ��� client rect�� target rect�� ��� ������� ����
	void UpdateAlignmentPosition(void);

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	enum eVisualPatternNodeAttribute
	{
		// �Է� ��� ����
		eAT_AcceptInput = eAT_SceneNodeBandwidth,

		// ��ġ�� �θ��� client rect ������ ����
		eAT_RestrictedWithinParentClient,

		eAT_VisualPatternNodeBandwidth = eAT_SceneNodeBandwidth + 4 // 4bit �뿪�� Ȯ��
	};

	// �Է� ��뿩��. default�� false
	inline void SetAcceptInput(bool enable) { m_Attribute.Assign(eAT_AcceptInput, enable); }
	inline bool GetAcceptInput(void) const { return m_Attribute[eAT_AcceptInput]; }

	// �̵��� �θ��� client rect ������ ����. default�� false
	// �θ� MkVisualPatternNode �� ���� class�� ��츸 �����ϸ� ���ų� �ִ��� MkSceneNode�� ��� ����
	// (NOTE) �ڽ��� window rect�� �θ��� client rect���� ũ�� �ʾƾ� ���� ����
	inline void SetRestrictedWithinParentClient(bool enable) { m_Attribute.Assign(eAT_RestrictedWithinParentClient, enable); }
	inline bool GetRestrictedWithinParentClient(void) const { return m_Attribute[eAT_RestrictedWithinParentClient]; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

protected:

	enum eVisualPatternNodeEventType
	{
		// ���� ��ġ ����
		eET_UpdateAlignmentPosition = 0,

		eET_VisualPatternNodeBandwidth,
	};

	virtual void __SendNodeEvent(const _NodeEvent& evt);

	//------------------------------------------------------------------------------------------------//

	MkVisualPatternNode(const MkHashStr& name);
	virtual ~MkVisualPatternNode() {}

protected:

	// region
	MkFloatRect m_ClientRect; // size : ����, position : �ֹ�
	MkFloat2 m_WindowSize; // �ֹ�

	// alignment
	eRectAlignmentPosition m_AlignmentPosition; // ����
	MkFloat2 m_AlignmentOffset; // ����

public:

	static const MkStr NamePrefix;
};
