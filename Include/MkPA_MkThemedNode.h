#pragma once


//------------------------------------------------------------------------------------------------//
// themed node
// - window system�� ��� �ܺη� ǥ���Ǵ� ������ ������ �� ���� �ʴ´�
// - �������� ���� ���̵��� �ݺ�� �����̹Ƿ� �������� ����ȭ�� ���������μ� ���� ���� �Ǽ��� ������
//   ���� ���� ȯ���� ��ǥ�� �Ѵ�
// - ���������δ� scene node�� panel���� �����ϰ� �����Ǿ� ������ �ܺ��� �ü����� ��� �� ������
//   ���� �� �ϳ��� �νĵǰ� �Ǵµ�, �� ������ ������ theme�� �Ѵ�
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowThemeData.h"
#include "MkPA_MkSceneNode.h"


class MkThemedNode : public MkSceneNode
{
public:

	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_ThemedNode; }

	//------------------------------------------------------------------------------------------------//
	// theme
	//------------------------------------------------------------------------------------------------//

	// theme ����
	// (NOTE) ����� ��� ������ �ν�
	bool SetTheme(const MkHashStr& themeName);

	// componet ����
	// (NOTE) eCT_None�� ��� ������ �ν�
	void SetComponent(MkWindowThemeData::eComponentType componentType);

	// client size ����
	// (NOTE) image type�� ��� image source size�� �缳����
	void SetClientSize(const MkFloat2& clientSize);

	// client rect ��ȯ
	inline const MkFloatRect& GetClientRect(void) const { return m_ClientRect; }

	// window size ��ȯ
	inline const MkFloat2& GetWindowSize(void) const { return m_WindowSize; }

	// �ش� form�� �´� position ����
	// eFT_SingleUnit : eP_Single
	// eFT_DualUnit : eP_Back, eP_Front
	// eFT_QuadUnit : eP_Normal, eP_Focus, eP_Pushing, eP_Disable
	// (NOTE) : eP_None�� ��� ����
	void SetFormPosition(MkWindowThemeFormData::ePosition position);

	void UseShadow(bool enable);

	// ����
	virtual void Clear(void);

	MkThemedNode(const MkHashStr& name);
	virtual ~MkThemedNode() { Clear(); }

protected:

	const MkWindowThemeFormData* _GetValidForm(void) const;

	void _DeleteThemeForm(const MkWindowThemeFormData* form);
	void _ApplyThemeForm(const MkWindowThemeFormData* form);
	void _ApplyThemeSize(const MkWindowThemeFormData* form);
	void _ApplyThemePosition(const MkWindowThemeFormData* form);

protected:

	MkFloatRect m_ClientRect; // size : ����, position : �ֹ�
	MkFloat2 m_WindowSize; // �ֹ�

	MkHashStr m_ThemeName; // ����
	MkWindowThemeData::eComponentType m_ComponentType; // ����
	bool m_UseShadow; // ����
	MkWindowThemeFormData::ePosition m_FormPosition; // �ֹ�

public:

	static const MkHashStr ShadowNodeName;
};
