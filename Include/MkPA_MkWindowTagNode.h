#pragma once


//------------------------------------------------------------------------------------------------//
// window tag node
// icon�� text�� �̷���� pair
//
// ex>
//	MkWindowTagNode* tagNode = MkWindowTagNode::CreateChildNode(tsubNode, L"TAG");
//	tagNode->SetLocalDepth(-1.f);
//	tagNode->SetIconPath(L"Default\\theme_default.png");
//	tagNode->SetIconSubsetOrSequenceName(L"IcnWinDef");
//	tagNode->SetTextName(L"WindowTitle");
//	tagNode->SetAlignmentPosition(eRAP_LeftCenter);
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkVisualPatternNode.h"


class MkWindowTagNode : public MkVisualPatternNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_WindowTagNode; }

	// alloc child instance
	static MkWindowTagNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// icon
	//------------------------------------------------------------------------------------------------//

	// iconPath�� empty�� ����
	void SetIconPath(const MkHashStr& iconPath);
	inline const MkHashStr& GetIconPath(void) const { return m_IconPath; }

	void SetIconSubsetOrSequenceName(const MkHashStr& subsetOrSequenceName);
	inline const MkHashStr& GetIconSubsetOrSequenceName(void) const { return m_IconSubsetOrSequenceName; }

	//------------------------------------------------------------------------------------------------//
	// text
	//------------------------------------------------------------------------------------------------//

	// textName�� empty�� ����
	void SetTextName(const MkHashStr& textName);

	// text node�� �����ϸ鼭 �ٷ� text�� ����
	void SetTextName(const MkHashStr& textName, const MkStr& msg);

	inline const MkHashStr& GetTextName(void) const { return m_TextName; }

	// text�� ���� �ֹ߼� ����(������� ����)�� ���� text node pointer ��ȯ
	MkTextNode* GetTagTextPtr(void);

	// text ���� ���� �ݿ�
	void CommitTagText(void);

	//------------------------------------------------------------------------------------------------//
	// region
	//------------------------------------------------------------------------------------------------//

	void SetLengthOfBetweenIconAndText(float length);
	inline float GetLengthOfBetweenIconAndText(void) const { return m_LengthOfBetweenIconAndText; }

	//------------------------------------------------------------------------------------------------//

	// ����
	virtual void Clear(void);

	MkWindowTagNode(const MkHashStr& name);
	virtual ~MkWindowTagNode() { Clear(); }

	bool __UpdateIcon(void);
	bool __UpdateText(void);

	// (NOTE) ȣ�� �� icon�� text �� �ּ� �ϳ��� �����ؾ� ��
	bool __UpdateRegion(void);

protected:

	//------------------------------------------------------------------------------------------------//
	// update command
	//------------------------------------------------------------------------------------------------//

	enum eWindowTagNodeUpdateCommand
	{
		eUC_Icon = eUC_VisualPatternNodeBandwidth,
		eUC_Text,
		eUC_Region,

		eUC_WindowTagNodeBandwidth,
	};

	virtual void _ExcuteUpdateCommand(void);

protected:

	// icon
	MkHashStr m_IconPath;
	MkHashStr m_IconSubsetOrSequenceName;

	// text
	MkHashStr m_TextName;

	// region
	float m_LengthOfBetweenIconAndText;

public:

	static const MkHashStr NodeNamePrefix;

	static const MkHashStr IconPanelName;
	static const MkHashStr TextPanelName;
};