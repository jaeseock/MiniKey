#pragma once


//------------------------------------------------------------------------------------------------//
// themed node
//
// - window theme
//   * window system�� ��� �ܺη� ǥ���Ǵ� ������ ������ �� ���� �ʴ´�
//   * �������� ���� ���̵��� �ݺ�� �����̹Ƿ� �������� ����ȭ�� ���������μ� ���� ���� �Ǽ��� ������
//     ���� ���� ȯ���� ��ǥ�� �Ѵ�
//   * ���������δ� scene node�� panel���� �����ϰ� �����Ǿ� �ִ��� �ܺ��� �ü����� �� ������ ���� ��
//     �ϳ��� �νĵǰ� �Ǵµ�, �� ������ ������ theme�� �Ѵ�
//
// - window tag
//   * icon�� text�� �̷���� pair
//   * �Է��� ���� �ʰ� ��¿뵵�θ� ����
//   * default�� theme�� ������ �������� �������� ���浵 ����
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowThemeData.h"
#include "MkPA_MkSceneNode.h"


class MkThemedNode : public MkSceneNode
{
public:

	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_ThemedNode; }

	// alloc child instance
	static MkThemedNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

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

	// client size ����
	// (NOTE) image type�� ��� image source size�� �缳����
	// ���� �Էµ� clientSize ��� ȣ�� �� ���� �� GetClientRect().size�� ����ϱ⸦ ����
	void SetClientSize(const MkFloat2& clientSize);

	// client rect ��ȯ
	inline const MkFloatRect& GetClientRect(void) const { return m_ClientRect; }

	// window size ��ȯ
	inline const MkFloat2& GetWindowSize(void) const { return m_WindowSize; }

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
	// ����
	// (NOTE) ȣ�� �� ��ȿ�� window rect�� ���� �Ǿ� �ִ� �����̾�� ��
	//------------------------------------------------------------------------------------------------//

	// ��ȿ�� target rect�� alignmentPosition�� ��ġ�ϵ��� local position�� ����
	void SnapTo(const MkFloatRect& targetRect, eRectAlignmentPosition alignmentPosition);

	// �θ� MkThemedNode �� ���� class�� ��� �θ� clirent rect�� alignmentPosition�� ��ġ�ϵ��� local position�� ����
	// �θ� ���ų� �ִ��� MkSceneNode�� ��� ����
	void SnapToParentClientRect(eRectAlignmentPosition alignmentPosition);

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	enum eThemedNodeAttribute
	{
		eAT_UseShadow = eAT_SceneNodeBandwidth,
		eAT_AcceptInput,

		eAT_ThemedNodeBandwidth = eAT_SceneNodeBandwidth + 4 // 4bit �뿪�� Ȯ��
	};

	// �׸��� ��뿩��. default�� false
	void SetShadowEnable(bool enable);
	inline bool GetShadowEnable(void) const { return m_Attribute[eAT_UseShadow]; }

	// �Է� ��뿩��. default�� false
	inline void SetAcceptInput(bool enable) { m_Attribute.Assign(eAT_AcceptInput, enable); }
	inline bool GetAcceptInput(void) const { return m_Attribute[eAT_AcceptInput]; }

	//------------------------------------------------------------------------------------------------//

	// ����
	virtual void Clear(void);

	MkThemedNode(const MkHashStr& name);
	virtual ~MkThemedNode() { Clear(); }

protected:

	enum eThemedNodeEventType
	{
		eSetTheme = 0,
		eThemedNodeETBandwidth = 4 // MkThemedNode : 4bit �뿪�� Ȯ��
	};

public:

	virtual void __SendNodeEvent(const _NodeEvent& evt);

protected:

	bool _SetTheme(const MkHashStr& themeName);

	const MkWindowThemeFormData* _GetValidForm(void) const;

	void _DeleteThemeForm(const MkWindowThemeFormData* form);
	void _ApplyThemeForm(const MkWindowThemeFormData* form);
	void _ApplyThemeSize(const MkWindowThemeFormData* form);
	void _ApplyThemePosition(const MkWindowThemeFormData* form);
	void _DeleteShadowNode(void);
	void _ApplyShadowNode(void);
	void _ApplyShadowSize(MkThemedNode* shadowNode);

protected:

	// theme
	MkHashStr m_ThemeName; // ����
	MkWindowThemeData::eComponentType m_ComponentType; // ����
	MkWindowThemeFormData::ePosition m_FormPosition; // �ֹ�

	// region
	MkFloatRect m_ClientRect; // size : ����, position : �ֹ�
	MkFloat2 m_WindowSize; // �ֹ�

public:

	static const MkHashStr ShadowNodeName;
};
