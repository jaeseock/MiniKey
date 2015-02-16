#pragma once


//------------------------------------------------------------------------------------------------//
// themed node
// - window system의 경우 외부로 표현되는 형식의 종류는 얼마 되지 않는다
// - 자유도와 구현 난이도는 반비례 관계이므로 자유도를 패턴화해 제한함으로서 보다 쉽고 실수의 여지가
//   적은 구현 환경을 목표로 한다
// - 내부적으로는 scene node와 panel들이 복잡하게 구성되어 있지만 외부의 시선에는 모두 몇 종류의
//   패턴 중 하나로 인식되게 되는데, 이 패턴의 집합을 theme라 한다
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

	// theme 적용
	// (NOTE) 비었을 경우 삭제로 인식
	bool SetTheme(const MkHashStr& themeName);

	// componet 적용
	// (NOTE) eCT_None일 경우 삭제로 인식
	void SetComponent(MkWindowThemeData::eComponentType componentType);

	// client size 적용
	// (NOTE) image type일 경우 image source size로 재설정됨
	void SetClientSize(const MkFloat2& clientSize);

	// client rect 반환
	inline const MkFloatRect& GetClientRect(void) const { return m_ClientRect; }

	// window size 반환
	inline const MkFloat2& GetWindowSize(void) const { return m_WindowSize; }

	// 해당 form에 맞는 position 적용
	// eFT_SingleUnit : eP_Single
	// eFT_DualUnit : eP_Back, eP_Front
	// eFT_QuadUnit : eP_Normal, eP_Focus, eP_Pushing, eP_Disable
	// (NOTE) : eP_None일 경우 무시
	void SetFormPosition(MkWindowThemeFormData::ePosition position);

	void UseShadow(bool enable);

	// 해제
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

	MkFloatRect m_ClientRect; // size : 보존, position : 휘발
	MkFloat2 m_WindowSize; // 휘발

	MkHashStr m_ThemeName; // 보존
	MkWindowThemeData::eComponentType m_ComponentType; // 보존
	bool m_UseShadow; // 보존
	MkWindowThemeFormData::ePosition m_FormPosition; // 휘발

public:

	static const MkHashStr ShadowNodeName;
};
