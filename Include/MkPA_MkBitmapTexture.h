#pragma once


//------------------------------------------------------------------------------------------------//
// �Ϲ����� static texture
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkBaseTexture.h"


class MkBitmapTexture : public MkBaseTexture
{
public:

	// �޸𸮷� ����
	bool SetUp(const MkByteArray& srcData, const MkDataNode* infoNode = NULL);

	MkBitmapTexture() : MkBaseTexture() {}
	virtual ~MkBitmapTexture() {}
};
