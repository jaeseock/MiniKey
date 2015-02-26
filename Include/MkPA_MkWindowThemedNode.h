#pragma once


//------------------------------------------------------------------------------------------------//
// window themed node
//
// - window theme
//   * window system의 경우 외부로 표현되는 형식의 종류는 얼마 되지 않음
//   * 자유도와 구현 난이도는 반비례 관계이므로 자유도를 규격화해 제한함으로서 보다 쉽고 실수의 여지가
//     적은 구현 환경을 목표로 함
//   * 내부적으로는 scene node와 panel들이 복잡하게 구성되어 있더라도 black box화 하여 외부적으로는
//     규격화 된 몇 정류의 pattern으로 보이게 되는데, 이 pattern의 집합을 theme라 함
//   * theme의 구성 설정은 static data 기반
//   * 설정이 완료되면 window/client rect를 가지게 됨
//   * form panel들의 depth는 0.1f
//   * 그림자를 사용 할 경우 그림자 form panel들의 depth는 0.2f
//
// - tag (MkWindowTagNode)
//   * 최대한 실수 여지를 줄이기 위해 자식 node를 통한 간접 관리(wrapping)
//   * (NOTE) 모든 tag들은 같은 depth(0.f)를 가지므로 겹치지 않도록 주의
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
	// tag
	//------------------------------------------------------------------------------------------------//

	// tag 생성. 같은 이름의 tag가 이미 존재하면 삭제 후 재생성
	void CreateTag(const MkHashStr& name);

	// tagInfo로 tag 설정
	bool SetTagInfo(const MkHashStr& name, const MkWindowTagNode::TagInfo& tagInfo);

	// tagInfo 반환
	bool GetTagInfo(const MkHashStr& name, MkWindowTagNode::TagInfo& buffer) const;

	// tag 삭제
	void DeleteTag(const MkHashStr& name);
	void DeleteAllTags(void);

	// text에 대한 휘발성 수정(저장되지 않음)을 위한 text node pointer 반환
	// (NOTE) 수정사항은 CommitTagText() 호출 이후 적용됨
	MkTextNode* GetTagTextPtr(const MkHashStr& name);
	void CommitTagText(const MkHashStr& name);

	// text에 대한 휘발성 수정의 특수예로 적용된 root text node의 font style을 변경
	// 자체적으로 적용까지 완료하므로 별도의 CommitTagText() 호출은 필요 없음
	bool SetTagTextFontStyle(const MkHashStr& name, const MkHashStr& fontStyle);

	//------------------------------------------------------------------------------------------------//
	// region
	//------------------------------------------------------------------------------------------------//

	// client size 적용
	// (NOTE) 적용된 form의 unit type이 image일 경우 image source size로 재설정됨
	// 따라서 입력된 clientSize를 믿지 말고 호출 후 갱신 된 GetClientRect().size를 사용하기를 권장
	virtual void SetClientSize(const MkFloat2& clientSize);

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	enum eWindowThemedNodeAttribute
	{
		eAT_UseShadow = eAT_VisualPatternNodeBandwidth, // 그림자 사용 여부

		eAT_WindowThemedNodeBandwidth = eAT_VisualPatternNodeBandwidth + 4 // 4bit 대역폭 확보
	};

	// 그림자 사용여부. default는 false
	void SetShadowEnable(bool enable);
	inline bool GetShadowEnable(void) const { return m_Attribute[eAT_UseShadow]; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

protected:

	enum eWindowThemedNodeEventType
	{
		eET_SetTheme = eET_VisualPatternNodeBandwidth, // theme 설정

		eET_WindowThemedNodeBandwidth
	};

public:

	virtual void __SendNodeEvent(const _NodeEvent& evt);

	//------------------------------------------------------------------------------------------------//

	// 해제
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
	MkHashStr m_ThemeName; // 보존
	MkWindowThemeData::eComponentType m_ComponentType; // 보존
	MkWindowThemeFormData::ePosition m_FormPosition; // 휘발

	// tag
	MkHashMap<MkHashStr, _TagData> m_Tags;

public:

	static const MkHashStr ShadowNodeName;
};
