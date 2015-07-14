#ifndef __MINIKEY_CORE_MKEVENTQUEUEPATTERN_H__
#define __MINIKEY_CORE_MKEVENTQUEUEPATTERN_H__

//------------------------------------------------------------------------------------------------//
// event queue pattern
//
// ��ü ������ �ܼ��� �̺�Ʈ ���� �� ó�� ���
// �̺�Ʈ ��ü�� ������ ������ �������� �鿡�� MkEventUnitPack(N) ��� ����
//
// ex> MkDevPanel������ ���
//
//	// �����
//	enum eEventType
//	{
//		eMsgToFreeboard = 0, // arg0 : line index, arg1 : msg
//		eMsgToSystem, // arg0 : line index, arg1 : msg
//		eMsgToDrawing, // arg0 : line index, arg1 : msg
//		eMsgToLog, // arg0 : 0/1(addtime false/true), arg1 : msg
//		eChangeSnapPosition, // arg0 : eRectAlignmentPosition
//		eResetSnapPosition,
//		eQuit
//	};
//
//	typedef MkEventUnitPack2<eEventType, unsigned int, MkStr> _PanelEvent;
//	...
//	MkEventQueuePattern<_PanelEvent> m_EventQueue;
//
//	// ������
//	m_EventQueue.Reserve(256); // ���� ����
//	...
//	m_EventQueue.RegisterEvent(_PanelEvent(eMsgToFreeboard, index, msg)); // event ����
//	m_EventQueue.RegisterEvent(_PanelEvent(eMsgToLog, (addTime) ? TRUE : FALSE, msg)); // event ����
//	...
//	MkArray<_PanelEvent> eventQueue;
//	if (m_EventQueue.Consume(eventQueue)) // event ó��
//	{
//		MK_INDEXING_LOOP(eventQueue, i)
//		{
//			const _PanelEvent& evt = eventQueue[i];
//			switch (evt.eventType)
//			{
//			case eMsgToFreeboard:
//				_SetBoardMsg(eFreeboard, evt.arg0, evt.arg1);
//				break;
//			...
//			case eQuit:
//				MkBaseFramework::Close();
//				break;
//			}
//		}
//	}
//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"
#include "MkCore_MkCriticalSection.h"

//------------------------------------------------------------------------------------------------//

// event type only
template <class ET>
class MkEventUnitPack0
{
public:
	MkEventUnitPack0() {}
	MkEventUnitPack0(const ET& evt) { eventType = evt; }
public:
	ET eventType;
};

// event type, 1 argument
template <class ET, class A0T>
class MkEventUnitPack1 : public MkEventUnitPack0<ET>
{
public:
	MkEventUnitPack1() : MkEventUnitPack0<ET>() {}
	MkEventUnitPack1(const ET& evt, const A0T& a0) :
		MkEventUnitPack0<ET>(evt) { arg0 = a0; }
public:
	A0T arg0;
};

// event type, 2 arguments
template <class ET, class A0T, class A1T>
class MkEventUnitPack2 : public MkEventUnitPack1<ET, A0T>
{
public:
	MkEventUnitPack2() : MkEventUnitPack1<ET, A0T>() {}
	MkEventUnitPack2(const ET& evt, const A0T& a0, const A1T& a1) :
		MkEventUnitPack1<ET, A0T>(evt, a0) { arg1 = a1; }
public:
	A1T arg1;
};

// event type, 3 arguments
template <class ET, class A0T, class A1T, class A2T>
class MkEventUnitPack3 : public MkEventUnitPack2<ET, A0T, A1T>
{
public:
	MkEventUnitPack3() : MkEventUnitPack2<ET, A0T, A1T>() {}
	MkEventUnitPack3(const ET& evt, const A0T& a0, const A1T& a1, const A2T& a2) :
		MkEventUnitPack2<ET, A0T, A1T>(evt, a0, a1) { arg2 = a2; }
public:
	A2T arg2;
};

// event type, 4 arguments
template <class ET, class A0T, class A1T, class A2T, class A3T>
class MkEventUnitPack4 : public MkEventUnitPack3<ET, A0T, A1T, A2T>
{
public:
	MkEventUnitPack4() : MkEventUnitPack3<ET, A0T, A1T, A2T>() {}
	MkEventUnitPack4(const ET& evt, const A0T& a0, const A1T& a1, const A2T& a2, const A3T& a3) :
		MkEventUnitPack3<ET, A0T, A1T, A2T>(evt, a0, a1, a2) { arg3 = a3; }
public:
	A3T arg3;
};

// event type, 5 arguments
template <class ET, class A0T, class A1T, class A2T, class A3T, class A4T>
class MkEventUnitPack5 : public MkEventUnitPack4<ET, A0T, A1T, A2T, A3T>
{
public:
	MkEventUnitPack5() : MkEventUnitPack4<ET, A0T, A1T, A2T, A3T>() {}
	MkEventUnitPack5(const ET& evt, const A0T& a0, const A1T& a1, const A2T& a2, const A3T& a3, const A4T& a4) :
		MkEventUnitPack4<ET, A0T, A1T, A2T, A3T>(evt, a0, a1, a2, a3) { arg4 = a4; }
public:
	A4T arg4;
};

// event type, 6 arguments
template <class ET, class A0T, class A1T, class A2T, class A3T, class A4T, class A5T>
class MkEventUnitPack6 : public MkEventUnitPack5<ET, A0T, A1T, A2T, A3T, A4T>
{
public:
	MkEventUnitPack6() : MkEventUnitPack5<ET, A0T, A1T, A2T, A3T, A4T>() {}
	MkEventUnitPack6(const ET& evt, const A0T& a0, const A1T& a1, const A2T& a2, const A3T& a3, const A4T& a4, const A5T& a5) :
		MkEventUnitPack5<ET, A0T, A1T, A2T, A3T, A4T>(evt, a0, a1, a2, a3, a4) { arg5 = a5; }
public:
	A5T arg5;
};

//------------------------------------------------------------------------------------------------//

template <class EventType>
class MkEventQueuePattern
{
public:

	// ť ũ�� ����
	void Reserve(unsigned int size)
	{
		MkScopedCriticalSection(m_CS);
		m_InitSize = size;
		m_EventQueue.Reserve(m_InitSize);
	}

	// �̺�Ʈ ���
	void RegisterEvent(const EventType& evt)
	{
		MkScopedCriticalSection(m_CS);
		m_EventQueue.PushBack(evt);
	}

	// �̺�Ʈ ���翩�� ��ȯ
	bool Exist(void)
	{
		MkScopedCriticalSection(m_CS);
		return (!m_EventQueue.Empty());
	}

	// �̺�Ʈť ���� �� ť�� ���
	bool Consume(MkArray<EventType>& buffer)
	{
		MkScopedCriticalSection(m_CS);
		bool ok = !m_EventQueue.Empty();
		if (ok)
		{
			buffer = m_EventQueue;
			m_EventQueue.Clear(); // Flush()�� ��� �������� ���������� shared pointer�� ���ԵǾ� ���� ��� �޸� ���� �߻�
			m_EventQueue.Reserve(m_InitSize);
		}
		return ok;
	}

	void Clear(void)
	{
		MkScopedCriticalSection(m_CS);
		m_EventQueue.Clear();
	}

	MkEventQueuePattern() { Reserve(MKDEF_DEFAULT_SIZE_IN_EVENT_QUEUE); }
	~MkEventQueuePattern() { Clear(); }

protected:

	unsigned int m_InitSize;
	MkArray<EventType> m_EventQueue;
	MkCriticalSection m_CS;
};

//------------------------------------------------------------------------------------------------//

#endif
