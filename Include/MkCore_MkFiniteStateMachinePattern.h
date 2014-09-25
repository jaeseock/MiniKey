#ifndef __MINIKEY_CORE_MKFINITESTATEMACHINEPATTERN_H__
#define __MINIKEY_CORE_MKFINITESTATEMACHINEPATTERN_H__

//------------------------------------------------------------------------------------------------//
// 유한상태기계 패턴
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
		// 존재하지 않는 조건 입력
		eNoneCondition = 0,

		// 정상 진행. 스테이트 변화 없음
		eStateUnchanged,

		// 정상 진행. 스테이트 변화
		eStateChanged
	};

	//------------------------------------------------------------------------------------------------//

public:

	inline void Clear(void) { m_State.Clear(); }

	// 초기화시 테이블 생성
	// 기본적으로 최초 생성된 stateID가 현재 스테이트로 지정 됨
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

	// SetTable() 종료 선언. 내부 최적화
	inline void Rehash(void)
	{
		m_State.Rehash();
	}

	// 조건을 넣어 진행시킴
	// updateNextState : 스테이트 갱신. false일 경우 eFeedBackMsg만 시뮬레이션 함
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

	// 현 스테이트 선언
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

	// 현 스테이트 반환
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

		// 새 스테이트 삽입
		inline void AddState(const ConditionType& condition, const StateType& outState)
		{
			m_Table.Create(condition, outState);
		}

		// 조건에 따른 스테이트 리턴
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