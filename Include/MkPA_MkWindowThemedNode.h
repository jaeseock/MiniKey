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
// 하나의 MkWindowThemedNode에 하나의 theme component 설정 가능
// tag 추가는 제한 없음
//
// 정규(regular) component 외에 전용 form을 사용하고 싶으면 custom form을 설정해 사용 가능
// 이 경우 theme를 변경해도 해당 theme에 동일한 custom form이 존재하지 않는 한 변경이 무시됨
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
	// (NOTE) regular component와 custom form은 배타적. 어느 한 쪽이 설정되면 다른 쪽 설정은 삭제됨
	//------------------------------------------------------------------------------------------------//

	// themeName이 empty면 삭제
	// (NOTE) custom form이 사용중일 때 주어진 theme에 동일한 custom form이 존재하지 않으면 무시
	void SetThemeName(const MkHashStr& themeName);
	inline const MkHashStr& GetThemeName(void) const { return m_ThemeName; }

	// regular component 설정
	// componentType이 MkWindowThemeData::eCT_None이면 삭제, MkWindowThemeData::eCT_CustomForm면 무시
	// (NOTE) custom form 설정 삭제 됨
	void SetComponentType(MkWindowThemeData::eComponentType componentType);
	inline MkWindowThemeData::eComponentType GetComponentType(void) const { return m_ComponentType; }

	// custom form 설정
	// (NOTE) regular component와는 달리 설정 실패 할 가능성 존재
	// (NOTE) 설정 성공시 regular component 설정은 삭제 됨
	bool SetCustomForm(const MkHashStr& customFormName);
	inline const MkHashStr& GetCustomForm(void) const { return m_CustomFormName; }

	// 그림자 사용 여부
	void SetShadowUsage(bool enable);
	inline bool GetShadowUsage(void) const { return m_UseShadow; }

	// 해당 node 및 하위 모든 node를 대상으로 srcThemeName에 해당하는 theme가 적용되어 있으면 destThemeName로 변경
	// srcThemeName이 empty면 모든 node를 대상으로 일괄 교체
	void ChangeThemeName(const MkHashStr& srcThemeName, const MkHashStr& destThemeName);

	//------------------------------------------------------------------------------------------------//
	// region
	// (NOTE) custom image type이거나 component의 unit type이 image일 경우 의미 없음
	//------------------------------------------------------------------------------------------------//

	virtual void SetClientSize(const MkFloat2& clientSize);
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
	// (NOTE) custom image를 사용중이거나 eS_None일 경우 무시
	void SetFormState(MkWindowThemeFormData::eState formState);
	inline MkWindowThemeFormData::eState GetFormState(void) const { return m_FormState; }

	//------------------------------------------------------------------------------------------------//
	// frame type
	//------------------------------------------------------------------------------------------------//

	// window frame type 반환
	MkWindowThemeData::eFrameType GetFrameType(void) const { return m_FrameType; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	virtual void SendNodeCommandTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode* argument);

	//------------------------------------------------------------------------------------------------//

	// 해제
	virtual void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// MkSceneObject
	//------------------------------------------------------------------------------------------------//

	virtual void Save(MkDataNode& node) const;

	MKDEF_DECLARE_SCENE_OBJECT_HEADER;

	MkWindowThemedNode(const MkHashStr& name);
	virtual ~MkWindowThemedNode() { Clear(); }

	//------------------------------------------------------------------------------------------------//

	// window size를 regular component/custom form margin을 반영한 client size로 변환
	static MkFloat2 ConvertWindowToClientSize
		(const MkHashStr& themeName, MkWindowThemeData::eComponentType componentType, const MkHashStr& customFormName, const MkFloat2& windowSize);

	// client size를 regular component/custom form margin을 반영한 window size로 변환
	static MkFloat2 ConvertClientToWindowSize
		(const MkHashStr& themeName, MkWindowThemeData::eComponentType componentType, const MkHashStr& customFormName, const MkFloat2& clientSize);

	// 현 객체에 세팅된 theme 및 client size를 기준으로 예상되는 window size를 계산해 반환
	// 일반적인 객체는 Update() 호출 이후에 window size가 갱신되기 때문에 미리 필요할 경우에 사용
	// (NOTE) theme name, component type(custom form), client size가 세팅되어 있어야 함
	MkFloat2 CalculateWindowSize(void) const;

	//------------------------------------------------------------------------------------------------//

protected:

	bool _UpdateThemeComponent(void);

	// (NOTE) 호출 전 유효한 theme name, component type이 설정되어 있어야 함
	// (NOTE) 적용중인 component의 unit type이 image일 경우 client size는 source size로 고정 됨
	// 자세한 설명은 MkWindowThemeUnitData::SetClientSize() 참조
	bool _UpdateRegion(void);

	// (NOTE) 호출 전 유효한 theme name, component type이 설정되어 있어야 함
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
