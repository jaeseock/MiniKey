#ifndef __MINIKEY_CORE_MKPROFILINGMANAGER_H__
#define __MINIKEY_CORE_MKPROFILINGMANAGER_H__


//------------------------------------------------------------------------------------------------//
// global instance - profiling(time stamp)
//
// Ư�� ������ ����ð��������� �ΰ��� ����� ����
// - ������ time stamp�� ���� ���
//   * ���ͷ�Ʈ�� �����층���� ���� ����� ����� �� �����Ƿ� ��Ȳ�� ���� ����Ȯ�� ���ɼ��� ����
//   * ���۰� ���� ������ �����ο�
// - ���� �ڵ� ������ ���� �ҿ�� Ŀ��+������ time stamp�� ��
//   * �� �ǹ̿� ����� ���� �Ҹ�
//   * �����带 �ѳ���� ������ �Ұ���
// MkProfilingManager�� ������ ������� �ð� ������ �������� ����̹Ƿ� ù��° ����� ä��
//
// 1ȸ ������ �ŷ� �� �� �����Ƿ� 1�ʰ� ���� �� ����� ���� ��ȯ
// �������̾�� �ϹǷ� thread-safe�� ���������� switching�� ������� �߻��� ��Ȯ���� �������Ƿ� ����
//
// �������Ϸ� ��ü�� ���ſ�� ������ ª�� ���� ������尡 Ŀ���Ƿ� �ִ��� �ܼ�ȭ
// (�׷��� thread-safe�� indexing ����̹Ƿ� �������� ����. ���Ž��� ������尡 �뷫 1300Ŭ�� ����)
//
// QueryPerformanceFrequency() ����̹Ƿ� tick freq.�� ����Ǵ� �ϵ����(����� CPU ��)������
// ����Ȯ�� ����� ���� �� ����
//
// �Ϲ����� ����
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
//	MK_PROF_MGR.GetEverageTimeStr(pkey, sbuf); // flush ������(15ȸ ����) ��� ���� �ð��� ���
//	MK_DEV_PANEL.MsgToLog(sbuf);
//
// MkFocusProfiler�� ���� ��Ŀ�� ���� �������ϸ� ���� ����
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

	// �ش� Ű�� ���� üũ ����
	void Begin(const MkHashStr& key);

	// �ش� Ű�� ���� üũ ����
	// (in) flushImmediately : true�� ��� ���ø� ī��Ʈ ��� ���� �ﰢ ����
	void End(const MkHashStr& key, bool flushImmediately = false);

	// �ش� Ű�� ���� ����� ���ŵǾ����� ���� ��ȯ
	bool GetUpdated(const MkHashStr& key);

	// �ش� Ű ��� ���� ƽ ��� ��ȯ
	__int64 GetEverageTick(const MkHashStr& key);

	// �ش� Ű ��� ���� �ð� ���(micro sec ����) ��ȯ
	unsigned int GetEverageTime(const MkHashStr& key);

	// �ش� Ű ��� ���� �ð� ����� ���ڿ��� ��ȯ�� buffer�� ��� ��ȯ
	void GetEverageTimeStr(const MkHashStr& key, MkStr& buffer);

	// ��ϵ� ��� Ű ����Ʈ�� ��ȯ
	unsigned int GetKeyList(MkArray<MkHashStr>& buffer);
	
	MkProfilingManager();
	MkProfilingManager(bool enable);
	virtual ~MkProfilingManager() {}

	//------------------------------------------------------------------------------------------------//

protected:

	// �������Ϸ� ���� ����
	class _ProfilerUnit
	{
	public:

		// �ʱ�ȭ
		void SetUp(double systemFrequenceFactor);

		// ���� CPU Ŭ�� ���
		// Ŭ�����ٴ� �ð��� �� �� �������̹Ƿ� ����� ��� �� ��
		void GetClock(unsigned __int64& container);

		// üũ ����
		void Begin(void);

		// üũ ����
		void End(bool flushImmediately);

		// ��� ���� ����
		inline bool GetUpdated(void) const { return m_Updated; }

		// ��� ���� ƽ ��ȯ
		inline __int64 GetEverageTick(void) const { return m_EverageTick; }

		// ��� ���� �ð�(micro sec ����) ��ȯ
		inline unsigned int GetEverageTime(void) const { return m_EverageTime; }

	protected:

		double m_SystemFrequenceFactor; // const

		// Begin() <-> End() ��� �Ǻ��� ���� Begin() ȣ��� ���õ�
		bool m_BeginSet;

		// ���� ����� �ٸ� ����� ���� �����ӽ� true
		bool m_Updated;

		// ������
		LARGE_INTEGER m_StartTick;
		LARGE_INTEGER m_EndTick;

		// ��� ����� Ǯ
		MkArray<__int64> m_TickPool;

		// ���
		__int64 m_EverageTick;
		unsigned int m_EverageTime;
	};

	//------------------------------------------------------------------------------------------------//

protected:

	bool _CheckSystemSupport(void);

protected:

	// 1,000,000 / �ʴ� ƽ ��
	double m_SystemFrequenceFactor;

	// ���� ����Ʈ
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
