#ifndef __MINIKEY_CORE_MKVALUEDECISION_H__
#define __MINIKEY_CORE_MKVALUEDECISION_H__

//------------------------------------------------------------------------------------------------//
// ���ſ��θ� �Ǵ��ϴ� ��
// ���� ũ�� �ڷ����̸� �޸� �񱳸�, ���� ũ��� operator != ���
// (NOTE) ��� �� SetUp() �ʼ�
//------------------------------------------------------------------------------------------------//

#include <memory.h>
#include "MkCore_MkContainerDefinition.h"


template<class DataType>
class MkValueDecision
{
public:

	// ������ ������ �ʱ�ȭ
	inline void SetUp(void)
	{
		m_Changed = true;
		m_WaitFirstInput = true;
	}

	// �� �Ҵ����� �ʱ�ȭ
	inline void SetUp(const DataType& value)
	{
		m_Decision = m_Buffer = value;
		SetUp();
	}

	// �� �Ҵ�
	inline DataType& operator = (const DataType& value)
	{
		m_Buffer = value;
		return m_Buffer;
	}

	// ���� ��ȯ
	inline const DataType& GetBuffer(void) const { return m_Buffer; }

	// ������ ��ȯ
	inline operator DataType() const { return m_Decision; }
	inline const DataType& GetDecision(void) const { return m_Decision; }

	// �� �ݿ�, ���ſ��� �Ǵ�
	// SetUp()�� ���� ȣ���̸� ������ �ݿ�
	inline bool Commit(void)
	{
		m_Changed = (m_WaitFirstInput || (IsFixedSizeType<DataType>::Get() ? (memcmp(&m_Buffer, &m_Decision, sizeof(DataType)) != 0) : (m_Buffer != m_Decision)));
		m_WaitFirstInput = false;

		if (m_Changed)
		{
			m_Decision = m_Buffer;
		}

		return m_Changed;
	}

	// �� �Ҵ� ���� �ݿ�, ���ſ��� �Ǵ�
	// SetUp()�� ���� ȣ���̸� ������ �ݿ�
	inline bool Commit(const DataType& value)
	{
		m_Buffer = value;
		return Commit();
	}

	// ���� Commit()�� ���� ����Ǿ����� ���� ��ȯ
	inline bool Changed(void) const { return m_Changed; }

	MkValueDecision() {}
	MkValueDecision(const DataType& value) { SetUp(value); }
	
protected:

	DataType m_Buffer;
	DataType m_Decision;
	bool m_WaitFirstInput;
	bool m_Changed;
};

//------------------------------------------------------------------------------------------------//

// operator != �� ���� �ڷ����� ��� Commit()�� (m_Buffer != m_Decision)���� ������ ������ �߻�
// ���� ũ�� �ڷ����� ��� ���ø� Ư��ȭ�� ȸ��

#define MKDEF_VALUE_DECISION_SIZE_ONLY_COMPARED_COMMIT(t) \
	template <> bool MkValueDecision<t>::Commit(void) \
	{ \
		m_Changed = (m_WaitFirstInput || (memcmp(&m_Buffer, &m_Decision, sizeof(t)) != 0)); \
		m_WaitFirstInput = false; \
		if (m_Changed) { m_Decision = m_Buffer; } \
		return m_Changed; \
	}

//------------------------------------------------------------------------------------------------//

#endif
