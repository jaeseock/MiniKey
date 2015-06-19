#pragma once


//------------------------------------------------------------------------------------------------//
// window control node : check box
//
// ex>
//	MkCheckBoxControlNode* checkBox = MkCheckBoxControlNode::CreateChildNode(bodyFrame, L"CheckBox");
//	checkBox->SetCheckBox(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, L"üũ�ڽ� ����", true);
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowBaseNode.h"


class MkCheckBoxControlNode : public MkWindowBaseNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_CheckBoxControlNode; }

	// alloc child instance
	static MkCheckBoxControlNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// control interface
	//------------------------------------------------------------------------------------------------//

	// static text node�� check box ����
	void SetCheckBox(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, const MkArray<MkHashStr>& textNode, bool onCheck = false);

	// �ֹ߼� caption���� check box ����
	// text node�� theme�� ����
	void SetCheckBox(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, const MkStr& caption, bool onCheck = false);

	// (NOTE) ȣ�� �� SetCheckBox()�� �ùٸ��� ȣ��� �����̾�� ��
	void SetCheck(bool onCheck);

	inline bool GetCheck(void) const { return m_OnCheck; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	//------------------------------------------------------------------------------------------------//
	// MkSceneObject
	//------------------------------------------------------------------------------------------------//

	virtual void Save(MkDataNode& node) const;

	MKDEF_DECLARE_SCENE_OBJECT_HEADER;

	MkCheckBoxControlNode(const MkHashStr& name);
	virtual ~MkCheckBoxControlNode() {}

protected:

	void _SetCheckBox(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, const MkArray<MkHashStr>& textNode, const MkStr& caption, bool onCheck);
	void _SetCaption(const MkArray<MkHashStr>& textNode, const MkStr& caption);
	void _SetCheck(bool onCheck, bool makeEvent);

protected:

	MkStr m_CaptionString;
	bool m_OnCheck;

public:

	static const MkHashStr CaptionNodeName;
	static const MkHashStr CheckIconName;

	static const MkHashStr ObjKey_CaptionTextName;
	static const MkHashStr ObjKey_CaptionString;
	static const MkHashStr ObjKey_OnCheck;
};
