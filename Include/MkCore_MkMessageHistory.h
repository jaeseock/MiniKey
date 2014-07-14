#ifndef __MINIKEY_CORE_MKMESSAGEHISTORY_H__
#define __MINIKEY_CORE_MKMESSAGEHISTORY_H__


//------------------------------------------------------------------------------------------------//
// ���ڿ� �����
// ä��/ġƮ �޼��� ������
//
// ���� ����� �ܼ�(ex> ���������� ��� ������Ʈ)�� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkDeque.h"
#include "MkCore_MkStr.h"
#include "MkCore_MkCriticalSection.h"


class MkMessageHistory
{
public:

	// �ʱ�ȭ
	// capacity : �ִ� ���� ũ��. 0:���Ѿ��� <= capacity < 0xffffffff)
	void SetUp(unsigned int capacity);

	// ������ ��ϰ� �ٸ��� �޼��� ���
	void Record(const MkStr& msg);

	// ��ĭ ���� ������� �̵��� �ش� �޼��� ��ȯ
	MkStr StepBack(void);

	// ��ĭ ���� ������� �̵��� �ش� �޼��� ��ȯ
	// ��ȯ���� false�̸� ���� �� ���� �ʿ� ����
	bool StepForward(MkStr& msg);

	void Clear(void);

	MkMessageHistory() { SetUp(0); }
	MkMessageHistory(unsigned int capacity) { SetUp(capacity); }
	~MkMessageHistory() { Clear(); }

protected:

	MkCriticalSection m_CS;
	MkDeque<MkStr> m_History;
	unsigned int m_CurrentPosition;
};

//------------------------------------------------------------------------------------------------//

#endif
