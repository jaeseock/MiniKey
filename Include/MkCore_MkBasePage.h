#ifndef __MINIKEY_CORE_MKBASEPAGE_H__
#define __MINIKEY_CORE_MKBASEPAGE_H__

//------------------------------------------------------------------------------------------------//
// MkPageManager에 등록되는 페이지 원형
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashStr.h"


class MkTimeState;
class MkDataNode;

class MkBasePage
{
public:

	//------------------------------------------------------------------------------------------------//
	// FSM
	//------------------------------------------------------------------------------------------------//

	// 페이지 테이블(FSM) 등록
	void SetPageFlowTable(const MkHashStr& condition, const MkHashStr& targetPage) const;

	// 현재 page가 자신일 경우 FSM에 조건 메세지를 보내서 페이지 이동
	// MkPageManager::Update()시 적용됨
	bool SetMoveMessage(const MkHashStr& condition) const;

	//------------------------------------------------------------------------------------------------//
	// proceed
	//------------------------------------------------------------------------------------------------//

	// 초기화 성공 여부 리턴
	// sharingNode : 페이지 전환시 순차적으로 호출되는 Clear() ... SetUp() 과정에서만 공유되는 임시 MkDataNode
	virtual bool SetUp(MkDataNode& sharingNode) { return true; }

	// 갱신
	virtual void Update(const MkTimeState& timeState) {}

	// 해제
	virtual void Clear(MkDataNode* sharingNode = NULL) {}

	// 현재 페이지 이름 반환
	inline const MkHashStr& GetPageName(void) const { return m_PageName; }

	MkBasePage(const MkHashStr& name) { m_PageName = name; }
	virtual ~MkBasePage() {}

protected:

	MkHashStr m_PageName;
};

//------------------------------------------------------------------------------------------------//

#endif
