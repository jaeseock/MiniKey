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
	inline bool GetAlignmentPivotIsWindowRect(void) const { return m_AlignmentPivotIsWindowRect; }

	void SetAlignmentPosition(eRectAlignmentPosition position);
	inline eRectAlignmentPosition GetAlignmentPosition(void) const { return m_AlignmentPosition; }

	void SetAlignmentOffset(const MkFloat2& offset);
	inline const MkFloat2& GetAlignmentOffset(void) const { return m_AlignmentOffset; }

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

	MkVisualPatternNode(const MkHashStr& name);
	virtual ~MkVisualPatternNode() {}

	// 설정된 position, offset으로 parentNode의 client rect에 대한 정렬 실행
	// (NOTE) parentNode의 client rect, 자신의 window rect는 유효해야 함
	void __UpdateAlignment(const MkVisualPatternNode* parentNode);
	void __UpdateAlignment(void);

	//------------------------------------------------------------------------------------------------//
	// update command
	//------------------------------------------------------------------------------------------------//

protected:

	enum eVisualPatternNodeUpdateCommand
	{
		eUC_Alignment = 0,

		eUC_VisualPatternNodeBandwidth,
	};

	virtual void _ExcuteUpdateCommand(void);

protected:

	// region
	MkFloatRect m_ClientRect;
	MkFloatRect m_WindowRect;

	// align
	bool m_AlignmentPivotIsWindowRect;
	eRectAlignmentPosition m_AlignmentPosition;
	MkFloat2 m_AlignmentOffset;

	// update command
	MkBitField32 m_UpdateCommand;

public:

	static const MkStr NamePrefix;
};
