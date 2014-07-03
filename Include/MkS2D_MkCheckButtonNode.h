#pragma once


//------------------------------------------------------------------------------------------------//
// check button node : MkBaseWindowNode
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkCheckButtonNode : public MkBaseWindowNode
{
public:

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_CheckButtonNode; }

	//------------------------------------------------------------------------------------------------//
	// button control
	//------------------------------------------------------------------------------------------------//

	// 버튼 생성
	bool CreateCheckButton(const MkHashStr& themeName, const CaptionDesc& captionDesc, bool onCheck = false);

	// 체크 상태 on/off
	void SetCheck(bool enable);

	// 체크 여부 반환
	inline bool GetCheck(void) const { return m_OnCheck; }

	//------------------------------------------------------------------------------------------------//
	// 구성
	//------------------------------------------------------------------------------------------------//

	// MkDataNode로 구성. MkSceneNode()의 구성 규칙과 동일
	virtual void Load(const MkDataNode& node);

	// MkDataNode로 출력
	virtual void Save(MkDataNode& node);

	//------------------------------------------------------------------------------------------------//
	// event call
	//------------------------------------------------------------------------------------------------//

	virtual bool HitEventMouseRelease(unsigned int button, const MkFloat2& position);

	//------------------------------------------------------------------------------------------------//

	MkCheckButtonNode(const MkHashStr& name);
	virtual ~MkCheckButtonNode() {}

public:

	static void __GenerateBuildingTemplate(void);

protected:

	virtual bool _CheckCursorHitCondition(const MkFloat2& position) const;

protected:

	CaptionDesc m_CaptionDesc;

	bool m_OnCheck;
};
