#ifndef __MINIKEY_CORE_MKTHREADMANAGER_H__
#define __MINIKEY_CORE_MKTHREADMANAGER_H__

//------------------------------------------------------------------------------------------------//
// thread manager
// 스레드 생성 및 관리자
//
// 스레드를 원형 그대로 쓸 경우 확장이 어려우므로 MkBaseThreadUnit을 사용, 외부에서 만들어진 스레드를 관리
//
// - RegisterThreadUnit()을 통해 외부 생성된 스레드를 등록(sleep 상태)
// - WakeUpAll() 호출로 그 시점까지 등록된 모든 스레드를 활성화
// - StopAll() 호출로 동작중인 모든 스레드 정지 및 삭제 수행
//
// 게임 클라이언트에서 멀티스레딩 활용은 이론적으로는 그럴듯해 보이지만 실제로는 실패의 위험성도 존재
// 멀티스레드의 효율은 병렬화에 달려 있지만 게임 클라이언트의 특성상 만족시키기가 어렵기 때문
// - 애초부터 CPU 클럭은 남아돔
// - 대상 하드웨어 사양이 불특정이라 싱글코어를 염두에 두어야 함(로직은 옵션질이 힘듬)
// - 최소한 프레임당 1회 이상은 동기를 맞추어야 함
// - 아무리 큰 계산이라도 OS의 스케쥴링 텀보다도 짧음(길어봐야 몇 ms 정도)
// - 빠른 피드백이 중요하므로 분산/지연처리가 힘듬
// - 순차식 데이터 흐름이 대부분이므로 병렬 가능한 구간이 적음
// 즉 일반적인 멀티스레딩의 목적인 분산처리를 통한 CPU 활용의 의미는 거의 없다고 봐고 무방함
// 오히려 블로킹을 푼다는 의미로서의 가치가 대부분을 차지
// - DX의 present() 블록동안 cpu로 로직계산을 해 바로 다음 렌더링 진입 가능
// - 파일 IO로 인한 순간적인 멈춤현상 완화
// - 사운드 갱신시의 멈춤현상 제거
//
// 결론 :
// - logic, draw, io, sound만 스레딩 권장
// - 공유대상을 줄이고 공유하더라도 특정 시점에 일괄적으로 수행
// - 완벽한 lock에 집착하지 말고 큰 문제 없다면 민영화나 운용 제한이 유리
//   (ex> 특정 데이터는 특정 스레드에서만 사용)
// - 컨텐츠 제작자가 멀티스레드 환경을 고려하지 않게 할 것
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkLockable.h"
#include "MkCore_MkBaseThreadUnit.h"


class MkStr;

class MkThreadManager
{
public:

	//------------------------------------------------------------------------------------------------//

	// MkBaseThreadUnit 객체로 관리되는 스레드 등록
	// 객체는 alloc되어 있어야 함(dealloc은 StopAll()시 수행됨)
	// 스레드 생성은 되었지만 실행되지는 않음(CREATE_SUSPENDED 상태)
	// StartAllThreads() 호출 이후부터는 추가 등록 불가
	bool RegisterThreadUnit(MkBaseThreadUnit* threadUnit);

	// 스레드 등록 종료 후 등록된 스레드 일괄 시작
	void WakeUpAll(void);

	// 등록된 모든 스레드의 SetUp()이 완료될때까지 대기
	// 반환값은 모든 스레드의 SetUp() 성공 여부
	bool WaitTillAllThreadsAreRunning(void);

	// 등록된 스레드 일괄 종료 후 삭제
	void StopAll(void);

	//------------------------------------------------------------------------------------------------//

	// 모든 스레드의 procedure
	static unsigned int WINAPI __ThreadProc(LPVOID lpParam);

	//------------------------------------------------------------------------------------------------//

	MkThreadManager();

protected:

	typedef struct __ThreadInfo
	{
		MkBaseThreadUnit* unit;
		unsigned int id;
		HANDLE handle;
	}
	_ThreadInfo;

protected:

	MkCriticalSection m_CS;
	MkArray<MkBaseThreadUnit*> m_ProcedureList;
	MkArray<HANDLE> m_HandleList;

	MkHashMap<MkHashStr, _ThreadInfo> m_ThreadInfos;

	MkLockable<bool> m_Lock;
};

//------------------------------------------------------------------------------------------------//

#endif
