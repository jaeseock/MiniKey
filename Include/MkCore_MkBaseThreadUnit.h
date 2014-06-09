#ifndef __MINIKEY_CORE_MKBASETHREADUNIT_H__
#define __MINIKEY_CORE_MKBASETHREADUNIT_H__

//------------------------------------------------------------------------------------------------//
// thread unit
// ���� ������ ���� ����
//
// (NOTE) MkThreadManager�� ��ϵ� ������ �̸��� �����ؾ� ��
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
		eReady = 0, // ������ �� ����
		eRunning, // SetUp() ���� ������ �������
		eClosing, // MkThreadMananger�κ��� ���� ����� ������ ����
		eEnd // �۾� �� Clear() �Ϸ�. ���� ������
	};

	//------------------------------------------------------------------------------------------------//

	// ������ �̸� ��ȯ
	inline const MkHashStr& GetThreadName(void) const { return m_ThreadName; }

	// �ʱ�ȭ
	// �ڽ� �����忡 ���� ȣ��
	// �������� ��ȯ
	virtual bool SetUp(void) { return true; }

	// SetUp() ���� �� ������Ʈ(����)
	// �ڽ� �����忡 ���� ȣ��
	// �⺻�����δ� MkTimeManager�� ���࿡ ���߾������ ���� ���� ������ �ʿ��� ��� Ȯ��
	// (NOTE) Ȯ��� ������ ���� �κп��� Update() ȣ�� �� ��
	virtual void Run(void);

	// �� ������ ����
	// Run()�� ���� ȣ���
	// (NOTE) Run()�� �������� �ʾҴٸ� m_CurrentState�� eRunning�� �ƴ� ��� �ڵ����� Ż���
	virtual void Update(const MkTimeState& timeState) {}

	// ����
	// �ڽ� �����忡 ���� ȣ��
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
