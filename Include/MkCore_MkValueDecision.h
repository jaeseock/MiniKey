#ifndef __MINIKEY_CORE_MKVALUEDECISION_H__
#define __MINIKEY_CORE_MKVALUEDECISION_H__

//------------------------------------------------------------------------------------------------//
// 갱신여부를 판단하는 값
// 고정 크기 자료형이면 메모리 비교를, 가변 크기면 operator != 사용
// (NOTE) 사용 전 SetUp() 필수
//------------------------------------------------------------------------------------------------//

#include <memory.h>
#include "MkCore_MkContainerDefinition.h"


template<class DataType>
class MkValueDecision
{
public:

	// 기존값 유지로 초기화
	inline void SetUp(void)
	{
		m_Changed = true;
		m_WaitFirstInput = true;
	}

	// 값 할당으로 초기화
	inline void SetUp(const DataType& value)
	{
		m_Decision = m_Buffer = value;
		SetUp();
	}

	// 값 할당
	inline DataType& operator = (const DataType& value)
	{
		m_Buffer = value;
		return m_Buffer;
	}

	// 버퍼 반환
	inline const DataType& GetBuffer(void) const { return m_Buffer; }

	// 결정값 반환
	inline operator DataType() const { return m_Decision; }
	inline const DataType& GetDecision(void) const { return m_Decision; }

	// 값 반영, 갱신여부 판단
	// SetUp()후 최초 호출이면 무조건 반영
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

	// 값 할당 직후 반영, 갱신여부 판단
	// SetUp()후 최초 호출이면 무조건 반영
	inline bool Commit(const DataType& value)
	{
		m_Buffer = value;
		return Commit();
	}

	// 이전 Commit()시 값이 변경되었는지 여부 반환
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

// operator != 가 없는 자료형의 경우 Commit()의 (m_Buffer != m_Decision)에서 컴파일 에러가 발생
// 고정 크기 자료형의 경우 템플릿 특수화로 회피

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
