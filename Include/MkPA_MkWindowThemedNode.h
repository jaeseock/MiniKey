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
// �ϳ��� MkWindowThemedNode�� �ϳ��� theme component ���� ����
// tag �߰��� ���� ����
//
// ����(regular) component �ܿ� ���� form�� ����ϰ� ������ custom form�� ������ ��� ����
// �� ��� theme�� �����ص� �ش� theme�� ������ custom form�� �������� �ʴ� �� ������ ���õ�
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
	// (NOTE) regular component�� custom form�� ��Ÿ��. ��� �� ���� �����Ǹ� �ٸ� �� ������ ������
	//------------------------------------------------------------------------------------------------//

	// themeName�� empty�� ����
	// (NOTE) custom form�� ������� �� �־��� theme�� ������ custom form�� �������� ������ ����
	void SetThemeName(const MkHashStr& themeName);
	inline const MkHashStr& GetThemeName(void) const { return m_ThemeName; }

	// regular component ����
	// componentType�� MkWindowThemeData::eCT_None�̸� ����, MkWindowThemeData::eCT_CustomForm�� ����
	// (NOTE) custom form ���� ���� ��
	void SetComponentType(MkWindowThemeData::eComponentType componentType);
	inline MkWindowThemeData::eComponentType GetComponentType(void) const { return m_ComponentType; }

	// custom form ����
	// (NOTE) regular component�ʹ� �޸� ���� ���� �� ���ɼ� ����
	// (NOTE) ���� ������ regular component ������ ���� ��
	bool SetCustomForm(const MkHashStr& customFormName);
	inline const MkHashStr& GetCustomForm(void) const { return m_CustomFormName; }

	// �׸��� ��� ����
	void SetShadowUsage(bool enable);
	inline bool GetShadowUsage(void) const { return m_UseShadow; }

	// �ش� node �� ���� ��� node�� ������� srcThemeName�� �ش��ϴ� theme�� ����Ǿ� ������ destThemeName�� ����
	// srcThemeName�� empty�� ��� node�� ������� �ϰ� ��ü
	void ChangeThemeName(const MkHashStr& srcThemeName, const MkHashStr& destThemeName);

	//------------------------------------------------------------------------------------------------//
	// region
	// (NOTE) custom image type�̰ų� component�� unit type�� image�� ��� �ǹ� ����
	//------------------------------------------------------------------------------------------------//

	virtual void SetClientSize(const MkFloat2& clientSize);
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
	// (NOTE) custom image�� ������̰ų� eS_None�� ��� ����
	void SetFormState(MkWindowThemeFormData::eState formState);
	inline MkWindowThemeFormData::eState GetFormState(void) const { return m_FormState; }

	//------------------------------------------------------------------------------------------------//
	// frame type
	//------------------------------------------------------------------------------------------------//

	// window frame type ��ȯ
	MkWindowThemeData::eFrameType GetFrameType(void) const { return m_FrameType; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	virtual void SendNodeCommandTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode* argument);

	//------------------------------------------------------------------------------------------------//

	// ����
	virtual void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// MkSceneObject
	//------------------------------------------------------------------------------------------------//

	virtual void Save(MkDataNode& node) const;

	MKDEF_DECLARE_SCENE_OBJECT_HEADER;

	MkWindowThemedNode(const MkHashStr& name);
	virtual ~MkWindowThemedNode() { Clear(); }

	//------------------------------------------------------------------------------------------------//

	// window size�� regular component/custom form margin�� �ݿ��� client size�� ��ȯ
	static MkFloat2 ConvertWindowToClientSize
		(const MkHashStr& themeName, MkWindowThemeData::eComponentType componentType, const MkHashStr& customFormName, const MkFloat2& windowSize);

	// client size�� regular component/custom form margin�� �ݿ��� window size�� ��ȯ
	static MkFloat2 ConvertClientToWindowSize
		(const MkHashStr& themeName, MkWindowThemeData::eComponentType componentType, const MkHashStr& customFormName, const MkFloat2& clientSize);

	// �� ��ü�� ���õ� theme �� client size�� �������� ����Ǵ� window size�� ����� ��ȯ
	// �Ϲ����� ��ü�� Update() ȣ�� ���Ŀ� window size�� ���ŵǱ� ������ �̸� �ʿ��� ��쿡 ���
	// (NOTE) theme name, component type(custom form), client size�� ���õǾ� �־�� ��
	MkFloat2 CalculateWindowSize(void) const;

	//------------------------------------------------------------------------------------------------//

protected:

	bool _UpdateThemeComponent(void);

	// (NOTE) ȣ�� �� ��ȿ�� theme name, component type�� �����Ǿ� �־�� ��
	// (NOTE) �������� component�� unit type�� image�� ��� client size�� source size�� ���� ��
	// �ڼ��� ������ MkWindowThemeUnitData::SetClientSize() ����
	bool _UpdateRegion(void);

	// (NOTE) ȣ�� �� ��ȿ�� theme name, component type�� �����Ǿ� �־�� ��
	bool _UpdateFormState(void);

	//------------------------------------------------------------------------------------------------//
	// update command
	//------------------------------------------------------------------------------------------------//

	enum eWindowThemedNodeUpdateCommand
	{
		eUC_ThemeComponent = eUC_VisualPatternNodeBandwidth,
		eUC_Region,
		eUC_FormState,

		eUC_WindowThemedNodeBandwidth
	};

	virtual void _ExcuteUpdateCommand(void);

protected:

	// theme
	MkHashStr m_ThemeName;
	MkWindowThemeData::eComponentType m_ComponentType;
	MkHashStr m_CustomFormName;
	bool m_UseShadow;

	// form state
	MkWindowThemeFormData::eState m_FormState;

	// etc
	MkWindowThemeData::eFrameType m_FrameType;

public:

	static const MkHashStr ShadowNodeName;

	static const MkHashStr ArgKey_ChangeTheme;

	static const MkHashStr ObjKey_ThemeName;
	static const MkHashStr ObjKey_ComponentType;
	static const MkHashStr ObjKey_CustomFormName;
	static const MkHashStr ObjKey_UseShadow;
	static const MkHashStr ObjKey_ClientSize;
	static const MkHashStr ObjKey_FrameType;
};
