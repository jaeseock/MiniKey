#pragma once


//------------------------------------------------------------------------------------------------//
// themed node
//
// - window theme
//   * window system의 경우 외부로 표현되는 형식의 종류는 얼마 되지 않는다
//   * 자유도와 구현 난이도는 반비례 관계이므로 자유도를 패턴화해 제한함으로서 보다 쉽고 실수의 여지가
//     적은 구현 환경을 목표로 한다
//   * 내부적으로는 scene node와 panel들이 복잡하게 구성되어 있더라도 외부의 시선에는 몇 종류의 패턴 중
//     하나로 인식되게 되는데, 이 패턴의 집합을 theme라 한다
//
// - window tag
//   * icon과 text로 이루어진 pair
//   * 입력은 받지 않고 출력용도로만 사용됨
//   * default는 theme의 설정을 따르지만 세부적인 변경도 가능
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

	// 자신과 하위 모든 node들을 대상으로 theme 적용
	// (NOTE) 비었을 경우 삭제로 인식
	bool SetTheme(const MkHashStr& themeName);

	// theme 반환
	inline const MkHashStr& GetTheme(void) const { return m_ThemeName; }

	// componet 적용
	// (NOTE) eCT_None일 경우 삭제로 인식
	void SetComponent(MkWindowThemeData::eComponentType componentType);

	// componet 반환
	inline MkWindowThemeData::eComponentType GetComponent(void) const { return m_ComponentType; }

	// client size 적용
	// (NOTE) image type일 경우 image source size로 재설정됨
	// 따라서 입력된 clientSize 대신 호출 후 갱신 된 GetClientRect().size를 사용하기를 권장
	void SetClientSize(const MkFloat2& clientSize);

	// client rect 반환
	inline const MkFloatRect& GetClientRect(void) const { return m_ClientRect; }

	// window size 반환
	inline const MkFloat2& GetWindowSize(void) const { return m_WindowSize; }

	// 해당 form type에 맞는 position 적용
	// eFT_SingleUnit : eP_Single
	// eFT_DualUnit : eP_Back, eP_Front
	// eFT_QuadUnit : eP_Normal, eP_Focus, eP_Pushing, eP_Disable
	// (NOTE) : eP_None일 경우 무시
	void SetFormPosition(MkWindowThemeFormData::ePosition position);

	// form type 반환
	// (NOTE) 호출 전 적합한 theme와 component가 설정된 상태이어야 함
	MkWindowThemeFormData::eFormType GetFormType(void) const;

	// form position 반환
	inline MkWindowThemeFormData::ePosition GetFormPosition(void) const { return m_FormPosition; }

	//------------------------------------------------------------------------------------------------//
	// 정렬
	// (NOTE) 호출 전 유효한 window rect가 설정 되어 있는 상태이어야 함
	//------------------------------------------------------------------------------------------------//

	// 유효한 target rect의 alignmentPosition에 위치하도록 local position을 갱신
	void SnapTo(const MkFloatRect& targetRect, eRectAlignmentPosition alignmentPosition);

	// 부모가 MkThemedNode 및 하위 class일 경우 부모 clirent rect의 alignmentPosition에 위치하도록 local position을 갱신
	// 부모가 없거나 있더라도 MkSceneNode일 경우 무시
	void SnapToParentClientRect(eRectAlignmentPosition alignmentPosition);

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	enum eThemedNodeAttribute
	{
		eAT_UseShadow = eAT_SceneNodeBandwidth,
		eAT_AcceptInput,

		eAT_ThemedNodeBandwidth = eAT_SceneNodeBandwidth + 4 // 4bit 대역폭 확보
	};

	// 그림자 사용여부. default는 false
	void SetShadowEnable(bool enable);
	inline bool GetShadowEnable(void) const { return m_Attribute[eAT_UseShadow]; }

	// 입력 허용여부. default는 false
	inline void SetAcceptInput(bool enable) { m_Attribute.Assign(eAT_AcceptInput, enable); }
	inline bool GetAcceptInput(void) const { return m_Attribute[eAT_AcceptInput]; }

	//------------------------------------------------------------------------------------------------//

	// 해제
	virtual void Clear(void);

	MkThemedNode(const MkHashStr& name);
	virtual ~MkThemedNode() { Clear(); }

protected:

	enum eThemedNodeEventType
	{
		eSetTheme = 0,
		eThemedNodeETBandwidth = 4 // MkThemedNode : 4bit 대역폭 확보
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
	MkHashStr m_ThemeName; // 보존
	MkWindowThemeData::eComponentType m_ComponentType; // 보존
	MkWindowThemeFormData::ePosition m_FormPosition; // 휘발

	// region
	MkFloatRect m_ClientRect; // size : 보존, position : 휘발
	MkFloat2 m_WindowSize; // 휘발

public:

	static const MkHashStr ShadowNodeName;
};
