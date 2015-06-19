#pragma once


//------------------------------------------------------------------------------------------------//
// window visual pattern node
// window system의 visual part의 base class
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkSceneNode.h"


class MkVisualPatternNode : public MkSceneNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_VisualPatternNode; }

	// alloc child instance
	static MkVisualPatternNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// region
	//------------------------------------------------------------------------------------------------//

	// client rect 반환
	inline const MkFloatRect& GetClientRect(void) const { return m_ClientRect; }

	// window rect 반환
	inline const MkFloatRect& GetWindowRect(void) const { return m_WindowRect; }

	//------------------------------------------------------------------------------------------------//
	// alignment(within parent client)
	// (NOTE) 부모가 MkVisualPatternNode 및 파생 class instance일 경우에만 유효
	//------------------------------------------------------------------------------------------------//

	// 기준이 되는 부모의 rect가 client인지 window인지 여부 설정/반환. default는 client rect(false)
	void SetAlignmentPivotIsWindowRect(bool enable);
	inline bool GetAlignmentPivotIsWindowRect(void) const { return m_Attribute[ePA_SNA_AlignmentPivotIsWindowRect]; }

	// 대상이 되는 자신의 rect가 client인지 window인지 여부 설정/반환. default는 window rect(true)
	void SetAlignmentTargetIsWindowRect(bool enable);
	inline bool GetAlignmentTargetIsWindowRect(void) const { return m_Attribute[ePA_SNA_AlignmentTargetIsWindowRect]; }

	// 정렬 방식
	void SetAlignmentPosition(eRectAlignmentPosition position);
	inline eRectAlignmentPosition GetAlignmentPosition(void) const { return m_AlignmentPosition; }

	// 정렬 후 추가 position offset
	void SetAlignmentOffset(const MkFloat2& offset);
	inline const MkFloat2& GetAlignmentOffset(void) const { return m_AlignmentOffset; }

	// Update()시 정렬 명령
	inline void SetAlignmentCommand(void) { m_UpdateCommand.Set(eUC_Alignment); }

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	// 입력 허용여부. default는 false
	inline void SetAcceptInput(bool enable) { m_Attribute.Assign(ePA_SNA_AcceptInput, enable); }
	inline bool GetAcceptInput(void) const { return m_Attribute[ePA_SNA_AcceptInput]; }

	//------------------------------------------------------------------------------------------------//
	// proceed
	//------------------------------------------------------------------------------------------------//

	virtual void Update(double currTime = 0.);

	MKDEF_DECLARE_SCENE_OBJECT_HEADER; // MkSceneObject

	MkVisualPatternNode(const MkHashStr& name);
	virtual ~MkVisualPatternNode() {}

protected:

	void _UpdateAlignment(void);

	//------------------------------------------------------------------------------------------------//
	// update command
	//------------------------------------------------------------------------------------------------//

	enum eVisualPatternNodeUpdateCommand
	{
		eUC_Alignment = 0,

		eUC_VisualPatternNodeBandwidth
	};

	virtual void _ExcuteUpdateCommand(void);

protected:

	// region
	MkFloatRect m_ClientRect;
	MkFloatRect m_WindowRect;

	// align
	eRectAlignmentPosition m_AlignmentPosition;
	MkFloat2 m_AlignmentOffset;

	// update command
	MkBitField32 m_UpdateCommand;

public:

	static const MkHashStr ObjKey_AlignPosition;
	static const MkHashStr ObjKey_AlignOffset;
};
