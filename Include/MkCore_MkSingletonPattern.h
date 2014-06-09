#ifndef __MINIKEY_CORE_MKSINGLETONPATTERN_H__
#define __MINIKEY_CORE_MKSINGLETONPATTERN_H__


//------------------------------------------------------------------------------------------------//
// singleton pattern
//
// - ���� ��ü, ���� �����̶�� �⺻���� �̱����� ������ ����
//
// - ��� ������ ������ ��������� ���� ��� ����. Ư�� �������̽� Ȱ�뿡 ����
//   ex> class Base : public MkSingletonPattern<Base> {}
//       class D3D : public Base {}
//       class GL : public Base {}
//       >> Base::Instance()������ D3D�� GL�� ������ ���������� ��� ����
//
// - ����� ����/���� �ʼ�
//
// (NOTE) singleton�� ��ǻ� ���������� ���������̸�, static�� ������� �״�� ��������
//    ���� singleton�� ����� �ʼ������� c++������ static�� ���� ��Ȯ�� ���ظ� �ʿ�� �Ѵ�
//    http://agbird.egloos.com/4296119
//
// (NOTE) ������ thread-safe�� �������� ���ϹǷ� multi-thread application�� ��� �ݵ�� single-thread ���¿���
//    �ּ� �� �� �̻� �����ǰ� �� �� (http://parkpd.egloos.com/2643054)
//
// ex>
//	* ����
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
//	* ���� ����
//	new TS;
//	new TC(false);
//
//	* ����
//	TS::Instance().DoSomething();
//	TC::Instance().DoSomething();
//
//	* ���� ����
//	delete TS::InstancePtr();
//	delete TC::InstancePtr();
//
//	* ���� �� MkInstanceDeallocator�� ���
//	MkInstanceDeallocator deallocator(64);
//	deallocator.RegisterInstance(new TS);
//	deallocator.RegisterInstance(new TC(false));
//
//	* MkInstanceDeallocator�� ���� ��� ������ �ϰ� ����
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
