#ifndef __MINIKEY_CORE_MKFINITESTATEMACHINEPATTERN_H__
#define __MINIKEY_CORE_MKFINITESTATEMACHINEPATTERN_H__

//------------------------------------------------------------------------------------------------//
// ���ѻ��±�� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"

/*
	MkFiniteStateMachinePattern<int, int> fsm;

	fsm.SetTable(0, 0, 1);
	fsm.SetTable(0, 1, 2);
	fsm.SetTable(1, 0, 2);
	fsm.SetTable(2, 0, 0);
	fsm.SetTable(2, 1, 3);
	fsm.SetTable(3, 0, 1);
	fsm.SetTable(3, 1, 2);
	fsm.Rehash();
	fsm.SetCurrentState(0);

	MK_LOG_MGR.Msg(BsStr(fsm.GetCurrentState()));
	fsm.Proceed(0);
	MK_LOG_MGR.Msg(BsStr(fsm.GetCurrentState()));
	fsm.Proceed(0);
	MK_LOG_MGR.Msg(BsStr(fsm.GetCurrentState()));
	fsm.Proceed(1);
	MK_LOG_MGR.Msg(BsStr(fsm.GetCurrentState()));
	fsm.Proceed(0);
	MK_LOG_MGR.Msg(BsStr(fsm.GetCurrentState()));
	fsm.Proceed(0);
	MK_LOG_MGR.Msg(BsStr(fsm.GetCurrentState()));
	fsm.Proceed(0);
	MK_LOG_MGR.Msg(BsStr(fsm.GetCurrentState()));
	fsm.Proceed(1);
	MK_LOG_MGR.Msg(BsStr(fsm.GetCurrentState()));

	// out
	0, 1, 2, 3, 1, 2, 0, 2
*/

template <class StateType, class ConditionType>
class MkFiniteStateMachinePattern
{
public :

	//------------------------------------------------------------------------------------------------//

	enum eFeedBackMsg
	{
		// �������� �ʴ� ���� �Է�
		eNoneCondition = 0,

		// ���� ����. ������Ʈ ��ȭ ����
		eStateUnchanged,

		// ���� ����. ������Ʈ ��ȭ
		eStateChanged
	};

	//------------------------------------------------------------------------------------------------//

public:

	inline void Clear(void) { m_State.Clear(); }

	// �ʱ�ȭ�� ���̺� ����
	// �⺻������ ���� ������ stateID�� ���� ������Ʈ�� ���� ��
	inline void SetTable(const StateType& stateID, const ConditionType& condition, const StateType& outState)
	{
		if (m_State.Empty())
		{
			m_CurrentStateID = stateID;
		}

		if (m_State.Exist(stateID))
		{
			m_State[stateID].AddState(condition, outState);
		}
		else
		{
			m_State.Create(stateID).AddState(condition, outState);
		}
	}

	// SetTable() ���� ����. ���� ����ȭ
	inline void Rehash(void)
	{
		m_State.Rehash();
	}

	// ������ �־� �����Ŵ
	// updateNextState : ������Ʈ ����. false�� ��� eFeedBackMsg�� �ùķ��̼� ��
	inline eFeedBackMsg Proceed(const ConditionType& condition, bool updateNextState = true)
	{
		StateType nextState;
		if ((!m_State.Exist(m_CurrentStateID)) || (!m_State[m_CurrentStateID].GetState(condition, nextState)))
		{
			return eNoneCondition;
		}

		eFeedBackMsg fbMsg;
		if (updateNextState)
		{
			fbMsg = SetCurrentState(nextState);
		}
		else
		{
			fbMsg = (m_CurrentStateID == nextState) ? eStateUnchanged : eStateChanged;
		}
		return fbMsg;
	}

	// �� ������Ʈ ����
	inline eFeedBackMsg SetCurrentState(const StateType& newState)
	{
		eFeedBackMsg fbMsg;
		if (m_CurrentStateID == newState)
		{
			fbMsg = eStateUnchanged;
		}
		else
		{
			fbMsg = eStateChanged;
			m_CurrentStateID = newState;
		}
		return fbMsg;
	}

	// �� ������Ʈ ��ȯ
	inline const StateType& GetCurrentState(void) const
	{
		return m_CurrentStateID;
	}

protected:

	//------------------------------------------------------------------------------------------------//

	template <class ConditionType, class StateType>
	class _FSM_State
	{
	public:

		// �� ������Ʈ ����
		inline void AddState(const ConditionType& condition, const StateType& outState)
		{
			m_Table.Create(condition, outState);
		}

		// ���ǿ� ���� ������Ʈ ����
		inline bool GetState(const ConditionType& condition, StateType& outState)
		{
			bool hasState = m_Table.Exist(condition);
			if (hasState)
			{
				outState = m_Table[condition];
			}
			return hasState;
		}

	protected:

		MkMap<ConditionType, StateType> m_Table;
	};

	//------------------------------------------------------------------------------------------------//

protected:

	StateType m_CurrentStateID;
	
	MkHashMap<StateType, _FSM_State<ConditionType, StateType> > m_State;
};

#endif