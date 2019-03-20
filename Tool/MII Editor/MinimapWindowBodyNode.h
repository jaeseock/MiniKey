#pragma once

//------------------------------------------------------------------------------------------------//
// window - minimap
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkBodyFrameControlNode.h"


class MinimapWindowBodyNode : public MkBodyFrameControlNode
{
public:
	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	inline bool IsHitting(void) const { return m_Hitting; }
	inline const MkFloat2& GetHitPosition(void) const { return m_HitPosition; }

	MinimapWindowBodyNode(const MkHashStr& name) : MkBodyFrameControlNode(name) { m_Hitting = false; }
	virtual ~MinimapWindowBodyNode() {}
	
protected:

	bool m_Hitting;
	MkFloat2 m_HitPosition;
};
