#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"
#include "MkS2D_MkBaseTexture.h"


class MkBitmapTexture : public MkBaseTexture
{
public:

	// �޸𸮷� ����
	bool SetUp(const MkByteArray& srcData, const MkDataNode* subsetNode);

	MkBitmapTexture() : MkBaseTexture() {}
	virtual ~MkBitmapTexture() {}
};
