#pragma once


//------------------------------------------------------------------------------------------------//
// window tag node
// - icon�� text�� �̷���� pair
// - �ִ��� �Ǽ� ������ ���̱� ���� parent node�� ���� ���� ����(wrapping)
// - �������� ����̶�� MkWindowTagNode::TagInfo ��ü �̿ܿ��� ���� �� ���� ����
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkVisualPatternNode.h"


class MkWindowTagNode : public MkVisualPatternNode
{
public:

	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_WindowTagNode; }

	// alloc child instance
	static MkWindowTagNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// tag
	//------------------------------------------------------------------------------------------------//

	typedef struct _TagInfo
	{
		// icon
		MkHashStr iconPath;
		MkHashStr iconSubsetOrSequenceName;

		// text
		MkHashStr textName;

		// region
		float lengthOfBetweenIconAndText;

		// postion
		eRectAlignmentPosition alignmentPosition;
		MkFloat2 offset;
	}
	TagInfo;

	// icon panel �ݿ�
	bool UpdateIconInfo(const TagInfo& tagInfo);

	// text panel �ݿ�
	bool UpdateTextInfo(const TagInfo& tagInfo);

	// region(client/window size) ���
	// (NOTE) ȣ�� �� icon�� text panel �� �ּ� �ϳ��� �����ؾ� ��
	bool UpdateRegionInfo(const TagInfo& tagInfo);

	// position �ݿ�
	void UpdatePositionInfo(const TagInfo& tagInfo, const MkFloatRect& parentClientRect);

	// text panel�� �ݿ� �� text node ��ȯ
	MkTextNode* GetTagTextPtr(void);

	//------------------------------------------------------------------------------------------------//
	// region
	//------------------------------------------------------------------------------------------------//

	// client size ����
	// tag�� client rect�� �� window rect��
	virtual void SetClientSize(const MkFloat2& clientSize) { m_WindowSize = m_ClientRect.size = clientSize; }

	//------------------------------------------------------------------------------------------------//
	// attribute, event ����
	//------------------------------------------------------------------------------------------------//

	MkWindowTagNode(const MkHashStr& name) : MkVisualPatternNode(name) {}
	virtual ~MkWindowTagNode() { Clear(); }

public:

	static const MkHashStr NodeNamePrefix;

	static const MkHashStr IconPanelName;
	static const MkHashStr TextPanelName;
};
