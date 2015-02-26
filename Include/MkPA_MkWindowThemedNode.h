#pragma once


//------------------------------------------------------------------------------------------------//
// window themed node
//
// - window theme
//   * window system�� ��� �ܺη� ǥ���Ǵ� ������ ������ �� ���� ����
//   * �������� ���� ���̵��� �ݺ�� �����̹Ƿ� �������� �԰�ȭ�� ���������μ� ���� ���� �Ǽ��� ������
//     ���� ���� ȯ���� ��ǥ�� ��
//   * ���������δ� scene node�� panel���� �����ϰ� �����Ǿ� �ִ��� black boxȭ �Ͽ� �ܺ������δ�
//     �԰�ȭ �� �� ������ pattern���� ���̰� �Ǵµ�, �� pattern�� ������ theme�� ��
//   * theme�� ���� ������ static data ���
//   * ������ �Ϸ�Ǹ� window/client rect�� ������ ��
//   * form panel���� depth�� 0.1f
//   * �׸��ڸ� ��� �� ��� �׸��� form panel���� depth�� 0.2f
//
// - tag (MkWindowTagNode)
//   * �ִ��� �Ǽ� ������ ���̱� ���� �ڽ� node�� ���� ���� ����(wrapping)
//   * (NOTE) ��� tag���� ���� depth(0.f)�� �����Ƿ� ��ġ�� �ʵ��� ����
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowThemeData.h"
#include "MkPA_MkWindowTagNode.h"


class MkWindowTagNode;

class MkWindowThemedNode : public MkVisualPatternNode
{
public:

	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_WindowThemedNode; }

	// alloc child instance
	static MkWindowThemedNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// theme
	//------------------------------------------------------------------------------------------------//

	// �ڽŰ� ���� ��� node���� ������� theme ����
	// (NOTE) ����� ��� ������ �ν�
	bool SetTheme(const MkHashStr& themeName);

	// theme ��ȯ
	inline const MkHashStr& GetTheme(void) const { return m_ThemeName; }

	// componet ����
	// (NOTE) eCT_None�� ��� ������ �ν�
	void SetComponent(MkWindowThemeData::eComponentType componentType);

	// componet ��ȯ
	inline MkWindowThemeData::eComponentType GetComponent(void) const { return m_ComponentType; }

	// �ش� form type�� �´� position ����
	// eFT_SingleUnit : eP_Single
	// eFT_DualUnit : eP_Back, eP_Front
	// eFT_QuadUnit : eP_Normal, eP_Focus, eP_Pushing, eP_Disable
	// (NOTE) : eP_None�� ��� ����
	void SetFormPosition(MkWindowThemeFormData::ePosition position);

	// form type ��ȯ
	// (NOTE) ȣ�� �� ������ theme�� component�� ������ �����̾�� ��
	MkWindowThemeFormData::eFormType GetFormType(void) const;

	// form position ��ȯ
	inline MkWindowThemeFormData::ePosition GetFormPosition(void) const { return m_FormPosition; }

	//------------------------------------------------------------------------------------------------//
	// tag
	//------------------------------------------------------------------------------------------------//

	// tag ����. ���� �̸��� tag�� �̹� �����ϸ� ���� �� �����
	void CreateTag(const MkHashStr& name);

	// tagInfo�� tag ����
	bool SetTagInfo(const MkHashStr& name, const MkWindowTagNode::TagInfo& tagInfo);

	// tagInfo ��ȯ
	bool GetTagInfo(const MkHashStr& name, MkWindowTagNode::TagInfo& buffer) const;

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
	// region
	//------------------------------------------------------------------------------------------------//

	// client size ����
	// (NOTE) ����� form�� unit type�� image�� ��� image source size�� �缳����
	// ���� �Էµ� clientSize�� ���� ���� ȣ�� �� ���� �� GetClientRect().size�� ����ϱ⸦ ����
	virtual void SetClientSize(const MkFloat2& clientSize);

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	enum eWindowThemedNodeAttribute
	{
		eAT_UseShadow = eAT_VisualPatternNodeBandwidth, // �׸��� ��� ����

		eAT_WindowThemedNodeBandwidth = eAT_VisualPatternNodeBandwidth + 4 // 4bit �뿪�� Ȯ��
	};

	// �׸��� ��뿩��. default�� false
	void SetShadowEnable(bool enable);
	inline bool GetShadowEnable(void) const { return m_Attribute[eAT_UseShadow]; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

protected:

	enum eWindowThemedNodeEventType
	{
		eET_SetTheme = eET_VisualPatternNodeBandwidth, // theme ����

		eET_WindowThemedNodeBandwidth
	};

public:

	virtual void __SendNodeEvent(const _NodeEvent& evt);

	//------------------------------------------------------------------------------------------------//

	// ����
	virtual void Clear(void);

	MkWindowThemedNode(const MkHashStr& name);
	virtual ~MkWindowThemedNode() { Clear(); }

protected:

	bool _SetTheme(const MkHashStr& themeName);

	const MkWindowThemeFormData* _GetValidForm(void) const;

	void _DeleteThemeForm(const MkWindowThemeFormData* form);
	void _ApplyThemeForm(const MkWindowThemeFormData* form);
	void _ApplyThemeSize(const MkWindowThemeFormData* form);
	void _ApplyThemePosition(const MkWindowThemeFormData* form);

	void _DeleteShadowNode(void);
	void _ApplyShadowNode(void);
	void _ApplyShadowSize(MkWindowThemedNode* shadowNode);

	typedef struct __TagData
	{
		MkWindowTagNode::TagInfo tagInfo;
		MkHashStr nodeName;
	}
	_TagData;

	MkWindowTagNode* _GetTagNode(const MkHashStr& nodeName);

protected:

	// theme
	MkHashStr m_ThemeName; // ����
	MkWindowThemeData::eComponentType m_ComponentType; // ����
	MkWindowThemeFormData::ePosition m_FormPosition; // �ֹ�

	// tag
	MkHashMap<MkHashStr, _TagData> m_Tags;

public:

	static const MkHashStr ShadowNodeName;
};
