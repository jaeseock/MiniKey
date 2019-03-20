#pragma once

//------------------------------------------------------------------------------------------------//
// window - system
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkBodyFrameControlNode.h"


class SystemWindowBodyNode : public MkBodyFrameControlNode
{
public:
	enum eWindowEvent
	{
		eWE_Load = 0,
		eWE_Reload,
		eWE_SaveMII,
		eWE_ApplyGroupName,
		eWE_ApplySubsetName,
		eWE_ApplySubsetRect,
		eWE_AddSeqName,
		eWE_ApplySeqName,
		eWE_EditCurrSeq,
		eWE_DelCurrSeq,
		eWE_ToggleCurrSeq
	};

	void CreateControls(void);

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);
	
	MkBitField32 ConsumeEvent(void);

	inline void GetSubsetRect(MkIntRect& rect) const { rect.position = m_SubsetPosition; rect.size = m_SubsetSize; }

	inline const MkArray<MkHashStr>& GetTargetSequenceStr(void) const { return m_TargetSequenceStr; }

	SystemWindowBodyNode(const MkHashStr& name) : MkBodyFrameControlNode(name) {}
	virtual ~SystemWindowBodyNode() {}

protected:

	bool _UpdateSubsetTransform(const MkStr& text, MkInt2& buffer);
	
protected:

	MkBitField32 m_BitField;
	MkInt2 m_SubsetPosition;
	MkInt2 m_SubsetSize;
	MkArray<MkHashStr> m_TargetSequenceStr;
};
