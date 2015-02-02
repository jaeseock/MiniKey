#ifndef __MINIKEY_CORE_MKTIMEMANAGER_H__
#define __MINIKEY_CORE_MKTIMEMANAGER_H__


//------------------------------------------------------------------------------------------------//
// global instance - time manager
//
// 시간 관리자(루핑 관리) 구현은 크게 두가지 방식이 존재
// - 일반적인 비결정론 방식
//   * elapsed가 가변적이라 루핑회수는 의미가 없음
//   * 의사난수라도 랜덤결과 예측이 어려움
//   * 구현비용이 낮고 거의 제한이 없음
//   * 프레임 스킵이 필요 없음
// - RTS게임에서 쓰이는 결정론 방식
//   * 초당 루핑횟수, elapsed 완전 고정(프레임 스킵으로 루프 카운트 보장)
//   * 렌더링은 스킵 할 수 있지만 상태를 변화시키는 모든 로직의 호출회수, 시간팩터 고정
//   * 랜덤은 의사난수 형태로 seed 및 호출회수 고정해 동일 결론을 내놓음
//   * 구현비용이 높고 로직 구현시 규칙을 엄격히 준수해야 함
// - MkTimeManager는 두번째 방식을 채택
//
// GetTickCount() 기반이므로 SetUp()후 최대 유효기간은 49.7일(60fps로 대략 259,200,000 frames)
//
// 결과값을 얻기 위해서는 직접 참조가 아닌 state의 카피본을 떠 참조
//
// thread-safe
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkTimeState.h"
#include "MkCore_MkLockable.h"
#include "MkCore_MkSingletonPattern.h"


#define MK_TIME_MGR MkTimeManager::Instance()


//------------------------------------------------------------------------------------------------//

class MkStr;

class MkTimeManager : public MkSingletonPattern<MkTimeManager>
{
public:

	// 초기화
	void SetUp(void);

	// 갱신 및 진행여부 결정
	void Update(void);

	// 상태 복사
	inline void GetCurrentTimeState(MkTimeState& buffer) { buffer = m_CurrentTimeState; }

	// 상태를 "시:분:초:밀리초 (프레임카운트)" 형태의 문자열로 반환
	void GetCurrentTimeState(MkStr& buffer);

	MkTimeManager();
	virtual ~MkTimeManager() {}

protected:

	MkLockable<DWORD> m_ZeroTime; // SetUp()이 호출된 시점의 시간
	MkLockable<MkTimeState> m_CurrentTimeState;
};

//------------------------------------------------------------------------------------------------//

#endif
