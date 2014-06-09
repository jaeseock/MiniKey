#ifndef __MINIKEY_CORE_MKPAGEMANAGER_H__
#define __MINIKEY_CORE_MKPAGEMANAGER_H__


//------------------------------------------------------------------------------------------------//
// global instance - page manager
//
// 메인 로직을 구역화 하고 SetUp/Clear/Update를 체계적으로 자동화 하기 위한 시스템
// - 각 page는 트리구조를 이룸
// - Root 페이지로 시작
// - SetUp 순서는 root부터 leaf
// - Update 순서는 root부터 leaf
// - Clear 순서는 leaf 부터 root
// - leaf 페이지에서 leaf 페이지로 이동시, 경로 최단거리를 기준으로 Clear()/SetUp() 실행
//
// (NOTE) thread-safe는 고려하지 않음
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkDeque.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkFiniteStateMachinePattern.h"
#include "MkCore_MkUniqueKeyTreePattern.h"
#include "MkCore_MkBasePage.h"
#include "MkCore_MkHashStr.h"

#define MK_PAGE_MGR MkPageManager::Instance()


class MkPageManager : public MkSingletonPattern<MkPageManager>
{
public:
	//------------------------------------------------------------------------------------------------//

	// 초기화
	// 루트 페이지 등록
	bool SetUp(MkBasePage* rootPage);
	
	// 페이지 등록
	bool RegisterChildPage(const MkHashStr& parentPage, MkBasePage* childPage);

	// 페이지 테이블(FSM) 등록
	void SetPageFlowTable(const MkHashStr& pageIn, const MkHashStr& condition, const MkHashStr& pageOut);

	// FSM에 조건 메세지를 보내서 페이지 이동
	// Update()시 적용됨
	bool SetMoveMessage(const MkHashStr& moveMessage);

	// 직접 leaf 페이즈로 이동
	// Update()시 적용됨
	bool ChangePageDirectly(const MkHashStr& pageName);

	// 주어진 페이지가 leaf인지 여부 반환
	inline bool IsLeaf(const MkHashStr& pageName) const { return m_PageTree.IsLeaf(pageName); }

	// 현 페이지 이름
	inline const MkHashStr& GetCurrentPageName(void) const { return m_StateFSM.GetCurrentState(); }

	//------------------------------------------------------------------------------------------------//
	// implementation

	void __Update(void);

	void __Clear(void);

	MkPageManager() : MkSingletonPattern<MkPageManager>() {}
	virtual ~MkPageManager() { __Clear(); }

protected:

	bool _CheckAndUpdatePageMovement(void);

	//------------------------------------------------------------------------------------------------//

protected:

	// FSM
	MkFiniteStateMachinePattern<MkHashStr, MkHashStr> m_StateFSM;

	// 페이지
	MkUniqueKeyTreePattern<MkHashStr, MkBasePage> m_PageTree;
	MkDeque<MkHashStr> m_PathFromRootToActivatingLeaf;
	MkHashStr m_LastTargetPageName;

	// message
	MkHashStr m_MoveMessage;
	MkHashStr m_ChangeMessage;
};

#endif
