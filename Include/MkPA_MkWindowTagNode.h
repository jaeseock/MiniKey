#pragma once


//------------------------------------------------------------------------------------------------//
// window tag node
// icon과 text로 이루어진 pair
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

	// iconPath가 empty면 삭제
	void SetIconPath(const MkHashStr& iconPath);
	inline const MkHashStr& GetIconPath(void) const { return m_IconPath; }

	void SetIconSubsetOrSequenceName(const MkHashStr& subsetOrSequenceName);
	inline const MkHashStr& GetIconSubsetOrSequenceName(void) const { return m_IconSubsetOrSequenceName; }

	//------------------------------------------------------------------------------------------------//
	// text
	//------------------------------------------------------------------------------------------------//

	// textName가 empty면 삭제
	void SetTextName(const MkArray<MkHashStr>& textName);

	// text node를 설정하면서 바로 text를 변경
	void SetTextName(const MkArray<MkHashStr>& textName, const MkStr& msg);

	inline const MkArray<MkHashStr>& GetTextName(void) const { return m_TextName; }

	// text에 대한 휘발성 수정(저장되지 않음)을 위한 text node pointer 반환
	MkTextNode* GetTagTextPtr(void);

	// text 수정 사항 반영
	void CommitTagText(void);

	//------------------------------------------------------------------------------------------------//
	// region
	//------------------------------------------------------------------------------------------------//

	void SetLengthOfBetweenIconAndText(float length);
	inline float GetLengthOfBetweenIconAndText(void) const { return m_LengthOfBetweenIconAndText; }

	//------------------------------------------------------------------------------------------------//

	// 해제
	virtual void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// MkSceneObject
	//------------------------------------------------------------------------------------------------//

	virtual void Save(MkDataNode& node) const;

	MKDEF_DECLARE_SCENE_OBJECT_HEADER;

	MkWindowTagNode(const MkHashStr& name);
	virtual ~MkWindowTagNode() { Clear(); }

protected:

	bool _UpdateIcon(void);
	bool _UpdateText(void);
	bool _UpdateRegion(void); // (NOTE) 호출 전 icon과 text 중 최소 하나는 존재해야 함

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
	MkArray<MkHashStr> m_TextName;

	// region
	float m_LengthOfBetweenIconAndText;

public:

	static const MkHashStr IconPanelName;
	static const MkHashStr TextPanelName;

	static const MkHashStr ObjKey_IconPath;
	static const MkHashStr ObjKey_IconSOSName;
	static const MkHashStr ObjKey_TextName;
	static const MkHashStr ObjKey_LengthOfBetweenIT;
};
