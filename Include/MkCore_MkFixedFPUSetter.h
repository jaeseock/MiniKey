#ifndef __MINIKEY_CORE_MKFIXEDFPUSETTER_H__
#define __MINIKEY_CORE_MKFIXEDFPUSETTER_H__

//------------------------------------------------------------------------------------------------//
// FPU�� �� �е� �ε��Ҽ��� �������� ��� ��ȯ
// Intel, AMD�� fpu ������ <-> �����е� ��ȯ ���̷� ���� ����ȭ�� ������ ���� �� �ִµ� �̸� �ذ���
//
// ���±�(NetKnife) ��Բ� ���� ������!!! (-_-)/
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkStackPattern.h"


class MkFixedFPUSetter
{
public:

	// precision�� 24 bit�� ����
	void FixPrecision(void);

	// �� ���� ����
	void Restore(void);

	~MkFixedFPUSetter() { Restore(); }

protected:

	MkStackPattern<unsigned int> m_ControlWord;
};

#endif
