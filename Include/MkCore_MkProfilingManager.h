#ifndef __MINIKEY_CORE_MKPROFILINGMANAGER_H__
#define __MINIKEY_CORE_MKPROFILINGMANAGER_H__


//------------------------------------------------------------------------------------------------//
// global instance - profiling(time stamp)
//
// 특정 구간의 수행시간측정에는 두가지 방법이 존재
// - 구간의 time stamp의 차이 계산
//   * 인터럽트나 스케쥴링으로 인한 비용이 끼어들 수 있으므로 상황에 따라 부정확할 가능성이 높음
//   * 시작과 끝의 지정이 자유로움
// - 실제 코드 수행을 위해 소요된 커널+스레드 time stamp의 합
//   * 원 의미에 충실한 실제 소모량
//   * 스레드를 넘나드는 측정은 불가능
// MkProfilingManager의 목적은 평균적인 시간 측정과 범용적인 사용이므로 첫번째 방식을 채택
//
// 1회 측정은 신뢰 할 수 없으므로 1초간 측정 후 평균을 구해 반환
// 범용적이어야 하므로 thread-safe를 만족하지만 switching시 오버헤드 발생해 정확도가 떨어지므로 주의
//
// 프로파일러 자체가 무거우면 구간이 짧을 수록 오버헤드가 커지므로 최대한 단순화
// (그래도 thread-safe에 indexing 방식이므로 가볍지는 않음. 갱신시의 오버헤드가 대략 1300클럭 정도)
//
// QueryPerformanceFrequency() 기반이므로 tick freq.가 변경되는 하드웨어(모바일 CPU 등)에서는
// 부정확한 결과를 보일 수 있음
//
// 일반적인 사용법
// ex>
//	const MkHashStr pkey = L"load";
//	for (unsigned int i=0; i<15; ++i)
//	{
//		MK_PROF_MGR.Begin(pkey);
//
//		MkBaseWindowNode* winNode = new MkBaseWindowNode(MkStr(i));
//		winNode->Load(node);
//		
//		MK_PROF_MGR.End(pkey, i == 14);
//
//		MK_WIN_EVENT_MGR.RegisterWindow(winNode, true);
//	}
//	MkStr sbuf;
//	MK_PROF_MGR.GetEverageTimeStr(pkey, sbuf); // flush 시점의(15회 수행) 평균 수행 시간을 출력
//	MK_DEV_PANEL.MsgToLog(sbuf);
//
// MkFocusProfiler를 통해 포커싱 내의 프로파일링 수행 가능
// ex>
//	{
//		MkFocusProfiler _fp(L"key");
//		...
//		if (...) return;
//		...
//		if (...) return;
//		...
//	}
//
// thread-safe
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkCriticalSection.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"

#define MK_PROF_MGR MkProfilingManager::Instance()


//------------------------------------------------------------------------------------------------//

class MkProfilingManager : public MkSwitchedSingletonPattern<MkProfilingManager>
{
public:

	// 해당 키에 따른 체크 시작
	void Begin(const MkHashStr& key);

	// 해당 키에 따른 체크 종료
	// (in) flushImmediately : true일 경우 샘플링 카운트 상관 없이 즉각 갱신
	void End(const MkHashStr& key, bool flushImmediately = false);

	// 해당 키에 대한 결과가 갱신되었는지 여부 반환
	bool GetUpdated(const MkHashStr& key);

	// 해당 키 평균 수행 틱 결과 반환
	__int64 GetEverageTick(const MkHashStr& key);

	// 해당 키 평균 수행 시간 결과(micro sec 단위) 반환
	unsigned int GetEverageTime(const MkHashStr& key);

	// 해당 키 평균 수행 시간 결과를 문자열로 변환해 buffer에 담아 반환
	void GetEverageTimeStr(const MkHashStr& key, MkStr& buffer);

	// 등록된 모든 키 리스트를 반환
	unsigned int GetKeyList(MkArray<MkHashStr>& buffer);
	
	MkProfilingManager();
	MkProfilingManager(bool enable);
	virtual ~MkProfilingManager() {}

	//------------------------------------------------------------------------------------------------//

protected:

	// 프로파일러 단위 유닛
	class _ProfilerUnit
	{
	public:

		// 초기화
		void SetUp(double systemFrequenceFactor);

		// 현재 CPU 클럭 얻기
		// 클럭보다는 시간이 좀 더 직관적이므로 현재는 사용 안 함
		void GetClock(unsigned __int64& container);

		// 체크 시작
		void Begin(void);

		// 체크 종료
		void End(bool flushImmediately);

		// 결과 갱신 여부
		inline bool GetUpdated(void) const { return m_Updated; }

		// 평균 수행 틱 반환
		inline __int64 GetEverageTick(void) const { return m_EverageTick; }

		// 평균 수행 시간(micro sec 단위) 반환
		inline unsigned int GetEverageTime(void) const { return m_EverageTime; }

	protected:

		double m_SystemFrequenceFactor; // const

		// Begin() <-> End() 페어 판별을 위해 Begin() 호출시 세팅됨
		bool m_BeginSet;

		// 이전 결과와 다른 결과가 나온 프레임시 true
		bool m_Updated;

		// 보관용
		LARGE_INTEGER m_StartTick;
		LARGE_INTEGER m_EndTick;

		// 결과 저장용 풀
		MkArray<__int64> m_TickPool;

		// 결과
		__int64 m_EverageTick;
		unsigned int m_EverageTime;
	};

	//------------------------------------------------------------------------------------------------//

protected:

	bool _CheckSystemSupport(void);

protected:

	// 1,000,000 / 초당 틱 수
	double m_SystemFrequenceFactor;

	// 유닛 리스트
	MkHashMap<MkHashStr, _ProfilerUnit> m_Units;

	// critical section
	MkCriticalSection m_CS_Units;
};

//------------------------------------------------------------------------------------------------//

class MkFocusProfiler
{
public:

	MkFocusProfiler(const MkHashStr& key);
	~MkFocusProfiler();

protected:

	MkHashStr m_Key;
};

//------------------------------------------------------------------------------------------------//

#endif
