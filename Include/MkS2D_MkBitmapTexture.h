#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"
#include "MkS2D_MkBaseTexture.h"


class MkBitmapTexture : public MkBaseTexture
{
public:

	// 메모리로 생성
	bool SetUp(const MkByteArray& srcData, const MkDataNode* subsetNode);

	MkBitmapTexture() : MkBaseTexture() {}
	virtual ~MkBitmapTexture() {}
};
