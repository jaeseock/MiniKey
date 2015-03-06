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

	void SetAlignmentPosition(eRectAlignmentPosition position);
	inline eRectAlignmentPosition GetAlignmentPosition(void) const { return m_AlignmentPosition; }

	void SetAlignmentOffset(const MkFloat2& offset);
	inline const MkFloat2& GetAlignmentOffset(void) const { return m_AlignmentOffset; }

	//------------------------------------------------------------------------------------------------//
	// proceed
	//------------------------------------------------------------------------------------------------//

	virtual void Update(double currTime = 0.);

	//------------------------------------------------------------------------------------------------//
	// attribute. data에 저장되는 값이므로 대역폭 확보 중요
	//------------------------------------------------------------------------------------------------//

	enum eVisualPatternNodeAttribute
	{
		// 입력 허용 여부
		eAT_AcceptInput = eAT_SceneNodeBandwidth,

		// 4bit 대역폭 확보
		eAT_VisualPatternNodeBandwidth = eAT_SceneNodeBandwidth + 4
	};

	// 입력 허용여부. default는 false
	inline void SetAcceptInput(bool enable) { m_Attribute.Assign(eAT_AcceptInput, enable); }
	inline bool GetAcceptInput(void) const { return m_Attribute[eAT_AcceptInput]; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	enum eVisualPatternNodeEventType
	{
		eET_VisualPatternNodeBandwidth = eET_SceneNodeBandwidth, // 없음
	};

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
	eRectAlignmentPosition m_AlignmentPosition;
	MkFloat2 m_AlignmentOffset;

	// update command
	MkBitField32 m_UpdateCommand;

public:

	static const MkStr NamePrefix;
};
