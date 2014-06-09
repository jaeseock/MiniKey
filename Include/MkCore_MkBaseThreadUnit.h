#ifndef __MINIKEY_CORE_MKBASETHREADUNIT_H__
#define __MINIKEY_CORE_MKBASETHREADUNIT_H__

//------------------------------------------------------------------------------------------------//
// thread unit
// 개별 스레드 유닛 원형
//
// (NOTE) MkThreadManager에 등록될 스레드 이름은 유일해야 함
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkLockable.h"
#include "MkCore_MkHashStr.h"


class MkTimeState;

class MkBaseThreadUnit
{
public:

	//------------------------------------------------------------------------------------------------//

	// thread state
	enum eThreadState
	{
		eReady = 0, // 생성만 된 상태
		eRunning, // SetUp() 실행 성공후 진행상태
		eClosing, // MkThreadMananger로부터 종료 명령이 내려진 상태
		eEnd // 작업 및 Clear() 완료. 삭제 대기상태
	};

	//------------------------------------------------------------------------------------------------//

	// 스레드 이름 반환
	inline const MkHashStr& GetThreadName(void) const { return m_ThreadName; }

	// 초기화
	// 자신 스레드에 의해 호출
	// 성공여부 반환
	virtual bool SetUp(void) { return true; }

	// SetUp() 성공 후 실행파트(루핑)
	// 자신 스레드에 의해 호출
	// 기본적으로는 MkTimeManager의 진행에 맞추어나가지만 전용 루프 관리가 필요할 경우 확장
	// (NOTE) 확장시 프레임 갱신 부분에서 Update() 호출 할 것
	virtual void Run(void);

	// 한 프레임 갱신
	// Run()에 의해 호출됨
	// (NOTE) Run()을 수정하지 않았다면 m_CurrentState가 eRunning이 아닌 경우 자동으로 탈출됨
	virtual void Update(const MkTimeState& timeState) {}

	// 해제
	// 자신 스레드에 의해 호출
	virtual void Clear(void) {}

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	inline void __ChangeStateToRunning(void) { m_CurrentState = eRunning; }
	inline void __ChangeStateToClosing(void) { m_CurrentState = eClosing; }
	inline void __ChangeStateToEnd(void) { m_CurrentState = eEnd; }

	inline eThreadState __GetThreadState(void) { return m_CurrentState; }

	//------------------------------------------------------------------------------------------------//

	MkBaseThreadUnit(const MkHashStr& threadName);
	virtual ~MkBaseThreadUnit() {}

private:

	MkHashStr m_ThreadName;

protected:

	MkLockable<eThreadState> m_CurrentState;
};

//------------------------------------------------------------------------------------------------//

#endif
