#pragma once


//------------------------------------------------------------------------------------------------//
// progress bar node
//
// ex>
//	MkProgressBarNode* pbNode = MkProgressBarNode::CreateChildNode(bodyFrame, L"ProgBar");
//	pbNode->SetProgressBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Medium, 250.f, 50, 200, MkProgressBarNode::eSPM_Percentage);
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowThemedNode.h"


class MkProgressBarNode : public MkWindowThemedNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_ProgressBarNode; }

	// alloc child instance
	static MkProgressBarNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// node interface
	//------------------------------------------------------------------------------------------------//

	enum eShowProgressMode
	{
		eSPM_None = 0,
		eSPM_CurrentAndMax,
		eSPM_Percentage
	};

	// �ʱ�ȭ
	void SetProgressBar(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length,
		int initValue = 0, int maxValue = 100, eShowProgressMode mode = eSPM_None);

	// ���� �� ����/��ȯ
	// (NOTE) 0 �̻��� ��. maximum value�� �Ѿ ��
	void SetCurrentValue(int value);
	inline int GetCurrentValue(void) const { return m_CurrentValue; }

	// �ִ� �� ����/��ȯ
	// (NOTE) 1 �̻��� ��
	void SetMaximumValue(int value);
	inline int GetMaximumValue(void) const { return m_MaximumValue; }

	// ���� ���� ������ ������� ��ȯ
	int GetProgressPercentage(void) const;

	// �� text ǥ�� ���
	void SetShowProgressMode(eShowProgressMode mode);
	inline eShowProgressMode GetShowProgressMode(void) const { return m_ShowProgressMode; }

	//------------------------------------------------------------------------------------------------//
	// MkSceneObject
	//------------------------------------------------------------------------------------------------//

	virtual void Save(MkDataNode& node) const;

	MKDEF_DECLARE_SCENE_OBJECT_HEADER;

	// ����
	virtual void Clear(void);

	MkProgressBarNode(const MkHashStr& name);
	virtual ~MkProgressBarNode() { Clear(); }

protected:

	void _UpdateProgressBar(void);
	void _UpdateProgressTag(void);

	//------------------------------------------------------------------------------------------------//
	// update command
	//------------------------------------------------------------------------------------------------//

	enum eProgressBarNodeUpdateCommand
	{
		eUC_ProgressBar = eUC_WindowThemedNodeBandwidth,
		eUC_ProgressTag,

		eUC_ProgressBarNodeBandwidth
	};

	virtual void _ExcuteUpdateCommand(void);

protected:

	int m_CurrentValue;
	int m_MaximumValue;
	eShowProgressMode m_ShowProgressMode;

public:

	static const MkHashStr ProgressBarName;
	static const MkHashStr ProgressTagName;

	static const MkHashStr ObjKey_CurrentValue;
	static const MkHashStr ObjKey_MaximumValue;
	static const MkHashStr ObjKey_ShowProgressMode;
};
