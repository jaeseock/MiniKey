#pragma once


//------------------------------------------------------------------------------------------------//
// (PVC)draw step interface
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkBaseTexture.h"


class MkDrawStepInterface
{
public:

	// (PVF)결과 텍스쳐 반환
	virtual void GetTargetTexture(unsigned int index, MkBaseTexturePtr& buffer) const = NULL;

	// (PVF)그리기. 반환 값은 출력 성공 여부
	virtual bool Draw(void) = NULL;

	MkDrawStepInterface() {}
	virtual ~MkDrawStepInterface() {}
};
