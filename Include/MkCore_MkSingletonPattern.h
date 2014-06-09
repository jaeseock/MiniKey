#ifndef __MINIKEY_CORE_MKSINGLETONPATTERN_H__
#define __MINIKEY_CORE_MKSINGLETONPATTERN_H__


//------------------------------------------------------------------------------------------------//
// singleton pattern
//
// - 단일 객체, 전역 접근이라는 기본적인 싱글톤의 조건을 만족
//
// - 상속 문제가 없으며 끼어들기식의 다중 상속 가능. 특히 인터페이스 활용에 유용
//   ex> class Base : public MkSingletonPattern<Base> {}
//       class D3D : public Base {}
//       class GL : public Base {}
//       >> Base::Instance()만으로 D3D와 GL의 구현을 선택적으로 사용 가능
//
// - 명시적 생성/종료 필수
//
// (NOTE) singleton은 사실상 전역변수나 마찬가지이며, static의 장단점을 그대로 가져간다
//    따라서 singleton의 사용은 필수적으로 c++에서의 static에 대한 정확한 이해를 필요로 한다
//    http://agbird.egloos.com/4296119
//
// (NOTE) 생성시 thread-safe를 보장하지 못하므로 multi-thread application의 경우 반드시 single-thread 상태에서
//    최소 한 번 이상 참조되게 할 것 (http://parkpd.egloos.com/2643054)
//
// ex>
//	* 정의
//	class TS : public MkSingletonPattern<TS>
//	{
//	public:
//		void DoSomething(void) {}
//	};
//
//	class TC : public MkSwitchedSingletonPattern<TC>
//	{
//	public:
//		void DoSomething(void) {}
//		TC(bool enable) : MkSwitchedSingletonPattern(enable) {}
//	};
//
//	* 단일 생성
//	new TS;
//	new TC(false);
//
//	* 동작
//	TS::Instance().DoSomething();
//	TC::Instance().DoSomething();
//
//	* 단일 해제
//	delete TS::InstancePtr();
//	delete TC::InstancePtr();
//
//	* 생성 후 MkInstanceDeallocator에 등록
//	MkInstanceDeallocator deallocator(64);
//	deallocator.RegisterInstance(new TS);
//	deallocator.RegisterInstance(new TC(false));
//
//	* MkInstanceDeallocator를 통한 등록 역순의 일괄 해제
//	deallocator.ClearRearToFront();
//------------------------------------------------------------------------------------------------//

#include <assert.h>
#include "MkCore_MkGlobalDefinition.h"


//------------------------------------------------------------------------------------------------//

template <class TargetClass>
class MkSingletonPattern : public MkVirtualInstance
{
public:

	static TargetClass* InstancePtr() { return m_InstancePtr; }

	static TargetClass& Instance()
	{
		assert(m_InstancePtr);
		return *m_InstancePtr;
	}

	MkSingletonPattern() : MkVirtualInstance()
	{
		assert(!m_InstancePtr);
		m_InstancePtr = static_cast<TargetClass*>(this);
	}

	virtual ~MkSingletonPattern()
	{
		assert(m_InstancePtr == static_cast<TargetClass*>(this));
		m_InstancePtr = 0;
	}

protected:

	static TargetClass* m_InstancePtr;
};
 
template <class TargetClass>
TargetClass* MkSingletonPattern<TargetClass>::m_InstancePtr = 0;

//------------------------------------------------------------------------------------------------//

template <class TargetClass>
class MkSwitchedSingletonPattern : public MkSingletonPattern<TargetClass>
{
public:

	inline bool IsEnable(void) const { return m_Enable; }

	MkSwitchedSingletonPattern() : MkSingletonPattern() { m_Enable = true; }
	MkSwitchedSingletonPattern(bool enable) : MkSingletonPattern() { m_Enable = enable; }

	virtual ~MkSwitchedSingletonPattern() {}

protected:

	bool m_Enable;
};

//------------------------------------------------------------------------------------------------//

#endif
