#ifndef __MINIKEY_CORE_MKMESSAGEHISTORY_H__
#define __MINIKEY_CORE_MKMESSAGEHISTORY_H__


//------------------------------------------------------------------------------------------------//
// 문자열 저장소
// 채팅/치트 메세지 보관용
//
// 동작 방식은 콘솔(ex> 윈도우즈의 명령 프롬프트)과 동일
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkDeque.h"
#include "MkCore_MkStr.h"
#include "MkCore_MkCriticalSection.h"


class MkMessageHistory
{
public:

	// 초기화
	// capacity : 최대 저장 크기. 0:제한없음 <= capacity < 0xffffffff)
	void SetUp(unsigned int capacity);

	// 마지막 기록과 다르면 메세지 기록
	void Record(const MkStr& msg);

	// 한칸 이전 기록으로 이동해 해당 메세지 반환
	MkStr StepBack(void);

	// 한칸 이후 기록으로 이동해 해당 메세지 반환
	// 반환값이 false이면 기존 값 변경 필요 없음
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
