#pragma once


//------------------------------------------------------------------------------------------------//
// �Ϲ����� static texture
//------------------------------------------------------------------------------------------------//

#include "MkPE_MkBaseTexture.h"


class MkBitmapTexture : public MkBaseTexture
{
public:

	// �޸𸮷� ����
	bool SetUp(const MkByteArray& srcData);

	MkBitmapTexture() : MkBaseTexture() {}
	virtual ~MkBitmapTexture() {}
};
