#pragma once


//------------------------------------------------------------------------------------------------//
// window base node
// window system의 action part의 base class
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowThemedNode.h"


class MkWindowBaseNode : public MkWindowThemedNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_WindowBaseNode; }

	// alloc child instance
	static MkWindowBaseNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// positioning
	//------------------------------------------------------------------------------------------------//

	typedef struct _PositionInfo
	{
		MkHashStr pivotComponent;
		eRectAlignmentPosition align;
		MkFloat2 offset;

		float depth;

		_PositionInfo();
	}
	PositionInfo;

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

		_TagInfo();
	}
	TagInfo;

	// tag 생성. 같은 이름의 tag가 이미 존재하면 삭제 후 재생성
	void CreateTag(const MkHashStr& name);

	// tagInfo로 tag 설정
	bool SetTagInfo(const MkHashStr& name, const TagInfo& tagInfo);

	// tagInfo 반환
	bool GetTagInfo(const MkHashStr& name, TagInfo& buffer) const;

	// posInfo로 tag 설정
	bool SetTagPositionInfo(const MkHashStr& name, const PositionInfo& posInfo);

	// posInfo 반환
	bool GetTagPositionInfo(const MkHashStr& name, PositionInfo& buffer) const;

	// tag 삭제
	void DeleteTag(const MkHashStr& name);
	void DeleteAllTags(void);

	// text에 대한 휘발성 수정(저장되지 않음)을 위한 text node pointer 반환
	// (NOTE) 수정사항은 CommitTagText() 호출 이후 적용됨
	MkTextNode* GetTagTextPtr(const MkHashStr& name);
	void CommitTagText(const MkHashStr& name);

	// text에 대한 휘발성 수정의 특수예로 적용된 root text node의 font style을 변경
	// 자체적으로 적용까지 완료하므로 별도의 CommitTagText() 호출은 필요 없음
	bool SetTagTextFontStyle(const MkHashStr& name, const MkHashStr& fontStyle);

	//------------------------------------------------------------------------------------------------//
	// themed component
	//------------------------------------------------------------------------------------------------//

	typedef struct _ComponentInfo
	{
		// component
		MkHashStr themeName;
		MkWindowThemeData::eComponentType componentType;

		// shadow
		bool useShadow;

		// region
		MkFloat2 clientSize;

		// form state
		MkWindowThemeFormData::eState formState;

		_ComponentInfo();
	}
	ComponentInfo;

	// component생성. 같은 이름의 component가 이미 존재하면 삭제 후 재생성
	void CreateComponent(const MkHashStr& name);

	// compInfo로 component 설정
	bool SetComponentInfo(const MkHashStr& name, const ComponentInfo& compInfo);

	// compInfo 반환
	bool GetComponentInfo(const MkHashStr& name, ComponentInfo& buffer) const;

	// posInfo로 component 설정
	bool SetComponentPositionInfo(const MkHashStr& name, const PositionInfo& posInfo);

	// posInfo 반환
	bool GetComponentPositionInfo(const MkHashStr& name, PositionInfo& buffer) const;

	// component 삭제
	void DeleteComponent(const MkHashStr& name);
	void DeleteAllComponents(void);

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	enum eWindowThemedNodeAttribute
	{
		eAT_WindowThemedNodeBandwidth = eAT_VisualPatternNodeBandwidth + 4 // 4bit 대역폭 확보
	};

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//
/*
protected:

	enum eWindowThemedNodeEventType
	{
		// theme 설정
		eET_SetTheme = eET_VisualPatternNodeBandwidth,

		// theme 변경
		eET_ChangeTheme,

		eET_WindowThemedNodeBandwidth
	};

public:

	virtual void __SendNodeEvent(const _NodeEvent& evt);
*/
	//------------------------------------------------------------------------------------------------//

	// 해제
	virtual void Clear(void);

	MkWindowBaseNode(const MkHashStr& name) : MkWindowThemedNode(name) {}
	virtual ~MkWindowBaseNode() { Clear(); }

protected:

	typedef struct __TagData
	{
		TagInfo tagInfo;
		PositionInfo posInfo;

		MkHashStr nodeName;
		MkWindowTagNode* node;
	}
	_TagData;

	typedef struct __CompData
	{
		ComponentInfo compInfo;
		PositionInfo posInfo;

		MkHashStr nodeName;
		MkWindowThemedNode* node;
	}
	_CompData;

	void _ApplyPosition(MkVisualPatternNode* node, const PositionInfo& info);

	void _UpdateRegion(void);

protected:

	// tag
	MkHashMap<MkHashStr, _TagData> m_Tags;

	// themed component
	MkHashMap<MkHashStr, _CompData> m_Components;
};
