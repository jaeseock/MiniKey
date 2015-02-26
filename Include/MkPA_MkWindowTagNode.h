#pragma once


//------------------------------------------------------------------------------------------------//
// window tag node
// - icon과 text로 이루어진 pair
// - 최대한 실수 여지를 줄이기 위해 parent node를 통한 간접 관리(wrapping)
// - 정상적인 사용이라면 MkWindowTagNode::TagInfo 객체 이외에는 노출 될 일이 없음
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

	// icon panel 반영
	bool UpdateIconInfo(const TagInfo& tagInfo);

	// text panel 반영
	bool UpdateTextInfo(const TagInfo& tagInfo);

	// region(client/window size) 계산
	// (NOTE) 호출 전 icon과 text panel 중 최소 하나는 존재해야 함
	bool UpdateRegionInfo(const TagInfo& tagInfo);

	// position 반영
	void UpdatePositionInfo(const TagInfo& tagInfo, const MkFloatRect& parentClientRect);

	// text panel에 반영 된 text node 반환
	MkTextNode* GetTagTextPtr(void);

	//------------------------------------------------------------------------------------------------//
	// region
	//------------------------------------------------------------------------------------------------//

	// client size 적용
	// tag는 client rect가 곧 window rect임
	virtual void SetClientSize(const MkFloat2& clientSize) { m_WindowSize = m_ClientRect.size = clientSize; }

	//------------------------------------------------------------------------------------------------//
	// attribute, event 없음
	//------------------------------------------------------------------------------------------------//

	MkWindowTagNode(const MkHashStr& name) : MkVisualPatternNode(name) {}
	virtual ~MkWindowTagNode() { Clear(); }

public:

	static const MkHashStr NodeNamePrefix;

	static const MkHashStr IconPanelName;
	static const MkHashStr TextPanelName;
};
