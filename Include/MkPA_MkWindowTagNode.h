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

		// alignment
		eRectAlignmentPosition alignmentPosition;
		MkFloat2 alignmentOffset;

		_TagInfo()
		{
			alignmentPosition = eRAP_LeftBottom;
		}
	}
	TagInfo;

	// icon panel �ݿ�
	bool UpdateIconInfo(const TagInfo& tagInfo);

	// text panel �ݿ�
	bool UpdateTextInfo(const TagInfo& tagInfo);

	// region(client/window size) ���
	// (NOTE) ȣ�� �� icon�� text panel �� �ּ� �ϳ��� �����ؾ� ��
	bool UpdateRegionInfo(const TagInfo& tagInfo);

	// text panel�� �ݿ� �� text node ��ȯ
	MkTextNode* GetTagTextPtr(void);

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
