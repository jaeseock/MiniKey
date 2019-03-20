#pragma once

//------------------------------------------------------------------------------------------------//
// window - sequence
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkImageInfo.h"
#include "MkPA_MkBodyFrameControlNode.h"


class MkPathName;

class SequenceWindowBodyNode : public MkBodyFrameControlNode
{
public:
	void CreateControls(void);

	void WakeUp(const MkPathName& imagePath, const MkHashStr& name);

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	SequenceWindowBodyNode(const MkHashStr& name) : MkBodyFrameControlNode(name) {}
	virtual ~SequenceWindowBodyNode() {}
	
protected:

	void _GetFrameDataItemMsg(unsigned int frameIndex, MkStr& buffer) const;

	void _UpdateFrameDataControl(void);

	void _UpdateSequenceAniView(void);

	typedef struct __FrameData
	{
		float timeStame;
		MkHashStr subsetName;
	}
	_FrameData;

protected:

	int m_FramesPerSec;

	MkHashStr m_SeqName;
	MkImageInfo::Sequence m_Sequence;
	MkArray<MkHashStr> m_AllSubsets;

	MkArray<_FrameData> m_FrameData;
};
