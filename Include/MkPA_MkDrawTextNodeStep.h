#pragma once


//------------------------------------------------------------------------------------------------//
// MkTextNode�� static image�� ����� ���� draw scene
// alpha ������ ���� color key ���(MKDEF_PA_TEXTNODE_COLOR_KEY)
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkDrawToStaticTextureStep.h"


class MkTextNode;

class MkDrawTextNodeStep : public MkDrawToStaticTextureStep
{
public:

	// text node ����
	// (NOTE) textNode ��ü�� �̹� ����� �����̾�� ��
	void SetUp(const MkTextNode* textNode);

	virtual void Clear(void);

	MkDrawTextNodeStep();
	virtual ~MkDrawTextNodeStep() { Clear(); }

protected:

	virtual bool _DrawContents(void);

protected:
	
	const MkTextNode* m_TextNode;
};
