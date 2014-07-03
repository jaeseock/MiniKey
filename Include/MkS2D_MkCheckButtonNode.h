#pragma once


//------------------------------------------------------------------------------------------------//
// check button node : MkBaseWindowNode
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkCheckButtonNode : public MkBaseWindowNode
{
public:

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_CheckButtonNode; }

	//------------------------------------------------------------------------------------------------//
	// button control
	//------------------------------------------------------------------------------------------------//

	// ��ư ����
	bool CreateCheckButton(const MkHashStr& themeName, const CaptionDesc& captionDesc, bool onCheck = false);

	// üũ ���� on/off
	void SetCheck(bool enable);

	// üũ ���� ��ȯ
	inline bool GetCheck(void) const { return m_OnCheck; }

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	// MkDataNode�� ����. MkSceneNode()�� ���� ��Ģ�� ����
	virtual void Load(const MkDataNode& node);

	// MkDataNode�� ���
	virtual void Save(MkDataNode& node);

	//------------------------------------------------------------------------------------------------//
	// event call
	//------------------------------------------------------------------------------------------------//

	virtual bool HitEventMouseRelease(unsigned int button, const MkFloat2& position);

	//------------------------------------------------------------------------------------------------//

	MkCheckButtonNode(const MkHashStr& name);
	virtual ~MkCheckButtonNode() {}

public:

	static void __GenerateBuildingTemplate(void);

protected:

	virtual bool _CheckCursorHitCondition(const MkFloat2& position) const;

protected:

	CaptionDesc m_CaptionDesc;

	bool m_OnCheck;
};
