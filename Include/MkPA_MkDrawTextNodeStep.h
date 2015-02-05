#pragma once


//------------------------------------------------------------------------------------------------//
// MkTextNode를 static image로 만들기 위한 draw scene
// alpha 생성을 위해 color key 사용(MKDEF_PA_TEXTNODE_COLOR_KEY)
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkDrawToStaticTextureStep.h"


class MkTextNode;

class MkDrawTextNodeStep : public MkDrawToStaticTextureStep
{
public:

	// text node 지정
	// (NOTE) textNode 객체는 이미 빌드된 상태이어야 함
	void SetUp(const MkTextNode* textNode);

	virtual void Clear(void);

	MkDrawTextNodeStep();
	virtual ~MkDrawTextNodeStep() { Clear(); }

protected:

	virtual bool _DrawContents(void);

protected:
	
	const MkTextNode* m_TextNode;
};
