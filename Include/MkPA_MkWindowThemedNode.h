#pragma once


//------------------------------------------------------------------------------------------------//
// window themed node
// - window system�� ��� �ܺη� ǥ���Ǵ� ������ ������ �� ���� ����
// - �������� ���� ���̵��� �ݺ�� �����̹Ƿ� �������� �԰�ȭ �����μ� ���� ���� �Ǽ��� ������ ����
//   ���� ȯ�� ����
// - ���� ������ ��� ���� �ܺ������δ� black boxȭ �� pattern��� ����Ǵµ�,
//   �� pattern���� component�� �ϰ� component�� ������ theme�� ��
// - theme�� ���� ���� ������ MkWindowThemeData ����
//
// ex>
//	MkWindowThemedNode* tbgNode = MkWindowThemedNode::CreateChildNode(mainNode, L"Themed BG");
//	tbgNode->SetLocalPosition(MkFloat2(500.f, 100.f));
//	tbgNode->SetLocalDepth(2.f);
//	tbgNode->SetThemeName(MkWindowThemeData::DefaultThemeName);
//	tbgNode->SetComponentType(MkWindowThemeData::eCT_DefaultBox);
//	tbgNode->SetShadowUsage(true);
//	tbgNode->SetClientSize(MkFloat2(300.f, 200.f));
//	tbgNode->SetFormState(MkWindowThemeFormData::eS_Default);
//	tbgNode->SetAcceptInput(true);
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowThemeData.h"
#include "MkPA_MkVisualPatternNode.h"


class MkWindowThemedNode : public MkVisualPatternNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_WindowThemedNode; }

	// alloc child instance
	static MkWindowThemedNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// theme component
	//------------------------------------------------------------------------------------------------//

	// themeName�� empty�� ����
	void SetThemeName(const MkHashStr& themeName);
	inline const MkHashStr& GetThemeName(void) const { return m_ThemeName; }

	// componentType�� MkWindowThemeData::eCT_None�̸� ����
	void SetComponentType(MkWindowThemeData::eComponentType componentType);
	inline MkWindowThemeData::eComponentType GetComponentType(void) const { return m_ComponentType; }

	void SetShadowUsage(bool enable);
	inline bool GetShadowUsage(void) const { return m_UseShadow; }

	//------------------------------------------------------------------------------------------------//
	// region
	//------------------------------------------------------------------------------------------------//

	void SetClientSize(const MkFloat2& clientSize);
	inline const MkFloat2& GetClientSize(void) const { return m_ClientRect.size; }

	//------------------------------------------------------------------------------------------------//
	// form state
	//------------------------------------------------------------------------------------------------//

	// ���� �������� form type ��ȯ
	// (NOTE) ȣ�� �� ��ȿ�� theme name, component type�� �����Ǿ� �־�� ��
	MkWindowThemeFormData::eFormType GetFormType(void) const;

	// �ش� form type�� �´� state ����
	// eFT_SingleUnit : eS_Single
	// eFT_DualUnit : eS_Back, eS_Front
	// eFT_QuadUnit : eS_Normal, eS_Focus, eS_Pushing, eS_Disable
	// (NOTE) eS_None�� ��� ����
	void SetFormState(MkWindowThemeFormData::eState formState);
	inline MkWindowThemeFormData::eState GetFormState(void) const { return m_FormState; }
	
	//------------------------------------------------------------------------------------------------//
	// attribute, event ����
	//------------------------------------------------------------------------------------------------//

	MkWindowThemedNode(const MkHashStr& name);
	virtual ~MkWindowThemedNode() {}

	bool __UpdateThemeComponent(void);

	// (NOTE) ȣ�� �� ��ȿ�� theme name, component type�� �����Ǿ� �־�� ��
	// (NOTE) �������� component�� unit type�� image�� ��� client size�� source size�� ���� ��
	// �ڼ��� ������ MkWindowThemeUnitData::SetClientSize() ����
	bool __UpdateRegion(void);

	// (NOTE) ȣ�� �� ��ȿ�� theme name, component type�� �����Ǿ� �־�� ��
	bool __UpdateFormState(void);

protected:

	//------------------------------------------------------------------------------------------------//
	// update command
	//------------------------------------------------------------------------------------------------//

	enum eWindowThemedNodeUpdateCommand
	{
		eUC_ThemeComponent = eUC_VisualPatternNodeBandwidth,
		eUC_Region,
		eUC_FormState,

		eUC_WindowThemedNodeBandwidth,
	};

	virtual void _ExcuteUpdateCommand(void);

protected:

	// theme component
	MkHashStr m_ThemeName;
	MkWindowThemeData::eComponentType m_ComponentType;
	bool m_UseShadow;

	// form state
	MkWindowThemeFormData::eState m_FormState;

public:

	static const MkHashStr NodeNamePrefix;

	static const MkHashStr ShadowNodeName;
};
