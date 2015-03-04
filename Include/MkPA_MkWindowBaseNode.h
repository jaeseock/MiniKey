#pragma once


//------------------------------------------------------------------------------------------------//
// window base node
// window system�� action part�� base class
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

	// tag ����. ���� �̸��� tag�� �̹� �����ϸ� ���� �� �����
	void CreateTag(const MkHashStr& name);

	// tagInfo�� tag ����
	bool SetTagInfo(const MkHashStr& name, const TagInfo& tagInfo);

	// tagInfo ��ȯ
	bool GetTagInfo(const MkHashStr& name, TagInfo& buffer) const;

	// posInfo�� tag ����
	bool SetTagPositionInfo(const MkHashStr& name, const PositionInfo& posInfo);

	// posInfo ��ȯ
	bool GetTagPositionInfo(const MkHashStr& name, PositionInfo& buffer) const;

	// tag ����
	void DeleteTag(const MkHashStr& name);
	void DeleteAllTags(void);

	// text�� ���� �ֹ߼� ����(������� ����)�� ���� text node pointer ��ȯ
	// (NOTE) ���������� CommitTagText() ȣ�� ���� �����
	MkTextNode* GetTagTextPtr(const MkHashStr& name);
	void CommitTagText(const MkHashStr& name);

	// text�� ���� �ֹ߼� ������ Ư������ ����� root text node�� font style�� ����
	// ��ü������ ������� �Ϸ��ϹǷ� ������ CommitTagText() ȣ���� �ʿ� ����
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

	// component����. ���� �̸��� component�� �̹� �����ϸ� ���� �� �����
	void CreateComponent(const MkHashStr& name);

	// compInfo�� component ����
	bool SetComponentInfo(const MkHashStr& name, const ComponentInfo& compInfo);

	// compInfo ��ȯ
	bool GetComponentInfo(const MkHashStr& name, ComponentInfo& buffer) const;

	// posInfo�� component ����
	bool SetComponentPositionInfo(const MkHashStr& name, const PositionInfo& posInfo);

	// posInfo ��ȯ
	bool GetComponentPositionInfo(const MkHashStr& name, PositionInfo& buffer) const;

	// component ����
	void DeleteComponent(const MkHashStr& name);
	void DeleteAllComponents(void);

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	enum eWindowThemedNodeAttribute
	{
		eAT_WindowThemedNodeBandwidth = eAT_VisualPatternNodeBandwidth + 4 // 4bit �뿪�� Ȯ��
	};

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//
/*
protected:

	enum eWindowThemedNodeEventType
	{
		// theme ����
		eET_SetTheme = eET_VisualPatternNodeBandwidth,

		// theme ����
		eET_ChangeTheme,

		eET_WindowThemedNodeBandwidth
	};

public:

	virtual void __SendNodeEvent(const _NodeEvent& evt);
*/
	//------------------------------------------------------------------------------------------------//

	// ����
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
