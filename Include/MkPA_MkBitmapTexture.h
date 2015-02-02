#pragma once


//------------------------------------------------------------------------------------------------//
// 老馆利牢 static texture
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkBaseTexture.h"


class MkBitmapTexture : public MkBaseTexture
{
public:

	// 皋葛府肺 积己
	bool SetUp(const MkByteArray& srcData, const MkDataNode* infoNode = NULL);

	MkBitmapTexture() : MkBaseTexture() {}
	virtual ~MkBitmapTexture() {}
};
