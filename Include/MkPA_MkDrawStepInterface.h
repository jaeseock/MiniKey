#pragma once


//------------------------------------------------------------------------------------------------//
// (PVC)draw step interface
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkBaseTexture.h"


class MkDrawStepInterface
{
public:

	// (PVF)��� �ؽ��� ��ȯ
	virtual void GetTargetTexture(unsigned int index, MkBaseTexturePtr& buffer) const = NULL;

	// (PVF)�׸���. ��ȯ ���� ��� ���� ����
	virtual bool Draw(void) = NULL;

	MkDrawStepInterface() {}
	virtual ~MkDrawStepInterface() {}
};
