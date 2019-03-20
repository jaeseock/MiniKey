#pragma once

//------------------------------------------------------------------------------------------------//
// window - animation view
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkImageInfo.h"
#include "MkPA_MkWindowBaseNode.h"


class MkPathName;
class MkBaseTexture;

class AnimationViewWindowNode : public MkWindowBaseNode
{
public:

	void CreateControls(const MkFloat2& maxSubsetSize);

	void SetImagePath(const MkPathName& imagePath);

	void UpdateSequence(const MkHashStr& key, const MkImageInfo::Sequence& sequence, MkFloat2& clientSize);

	void SetSequenceData(const MkHashStr& key, const MkImageInfo::Sequence& sequence);

	virtual void Update(double currTime = 0.);

	AnimationViewWindowNode(const MkHashStr& name) : MkWindowBaseNode(name) { m_Texture = NULL; }
	virtual ~AnimationViewWindowNode() {}
	
protected:

	MkBaseTexture* m_Texture;
	MkFloat2 m_MaxSubsetSize;
};
