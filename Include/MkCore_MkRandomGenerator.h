#ifndef __MINIKEY_CORE_MKRANDOMGENERATOR_H__
#define __MINIKEY_CORE_MKRANDOMGENERATOR_H__

//------------------------------------------------------------------------------------------------//
// ���� ������
//
// Uniformly Distributed Random Number ���� �˰���
// �Ϻ��� uniform ������ �ƴ����� ����� ������ ��ġ�� ����
// (���� ���� ���� ���� ���� ���� ���� ���̰� ���� �׽�Ʈ�� 4%, 10���� �׽�Ʈ�� 0.5% �̸�)
//
// �ǻ糭��(seed�� �����ϸ� ������ ����� ����)
//
// http://blog.yahoo.com/_XJA6C52DU5D4GVLPC7QW3R2EMM/articles/174940
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"


class MkRandomGenerator
{
public:

	// �õ� ����(0 ~ 20000)
	void SetSeed(unsigned int seed);

	// �õ带 Ȱ���Ͽ� 0 �̻�, 1 �̸��� ������ ��ȯ�ϰ� �õ� ����
	double GetRandomValue(void);

	// 0 �̻�, offset �̸��� ���� ��ȯ
	// offset�� 2 �̻��̾�� ��
	unsigned int GetRandomNumber(unsigned int offset);

	// ������
	MkRandomGenerator();
	MkRandomGenerator(unsigned int seed);

protected:

	// �ð��� ���� �õ� ���
	unsigned int _GetSeedFromTime(void) const;

protected:

	unsigned int m_Seed[3];
};

//------------------------------------------------------------------------------------------------//

// ����ũ��
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkRandomGenerator)

//------------------------------------------------------------------------------------------------//

#endif
