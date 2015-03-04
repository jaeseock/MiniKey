#pragma once


//------------------------------------------------------------------------------------------------//
// window themed node
// - window system의 경우 외부로 표현되는 형식의 종류는 얼마 되지 않음
// - 자유도와 구현 난이도는 반비례 관계이므로 자유도를 규격화 함으로서 보다 쉽고 실수의 여지가 적은
//   구현 환경 제시
// - 내부 구조와 상관 없이 외부적으로는 black box화 된 pattern들로 노출되는데,
//   이 pattern들을 component라 하고 component의 집합을 theme라 함
// - theme에 대한 세부 설정은 MkWindowThemeData 참조
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

	// themeName이 empty면 삭제
	void SetThemeName(const MkHashStr& themeName);
	inline const MkHashStr& GetThemeName(void) const { return m_ThemeName; }

	// componentType이 MkWindowThemeData::eCT_None이면 삭제
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

	// 현재 적용중인 form type 반환
	// (NOTE) 호출 전 유효한 theme name, component type이 설정되어 있어야 함
	MkWindowThemeFormData::eFormType GetFormType(void) const;

	// 해당 form type에 맞는 state 적용
	// eFT_SingleUnit : eS_Single
	// eFT_DualUnit : eS_Back, eS_Front
	// eFT_QuadUnit : eS_Normal, eS_Focus, eS_Pushing, eS_Disable
	// (NOTE) eS_None일 경우 무시
	void SetFormState(MkWindowThemeFormData::eState formState);
	inline MkWindowThemeFormData::eState GetFormState(void) const { return m_FormState; }
	
	//------------------------------------------------------------------------------------------------//
	// attribute, event 없음
	//------------------------------------------------------------------------------------------------//

	MkWindowThemedNode(const MkHashStr& name);
	virtual ~MkWindowThemedNode() {}

	bool __UpdateThemeComponent(void);

	// (NOTE) 호출 전 유효한 theme name, component type이 설정되어 있어야 함
	// (NOTE) 적용중인 component의 unit type이 image일 경우 client size는 source size로 고정 됨
	// 자세한 설명은 MkWindowThemeUnitData::SetClientSize() 참조
	bool __UpdateRegion(void);

	// (NOTE) 호출 전 유효한 theme name, component type이 설정되어 있어야 함
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
