#pragma once


//------------------------------------------------------------------------------------------------//
// (PVC)draw step interface
//------------------------------------------------------------------------------------------------//

class MkBaseTexture;

class MkDrawStepInterface
{
public:

	// (PVF)��� �ؽ��� ��ȯ
	virtual MkBaseTexture* GetTargetTexture(unsigned int index = 0) const = NULL;

	// (PVF)�׸���. ��ȯ ���� ��� ���� ����
	virtual bool Draw(void) = NULL;

	MkDrawStepInterface() {}
	virtual ~MkDrawStepInterface() {}
};
