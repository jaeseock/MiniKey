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

	// 초기화 성공 여부 리턴
	// sharingNode : 페이지 전환시 순차적으로 호출되는 Clear() ... SetUp() 과정에서만 공유되는 임시 MkDataNode
	virtual bool SetUp(MkDataNode& sharingNode) { return true; }

	// 갱신
	virtual void Update(const MkTimeState& timeState) {}

	// 해제
	virtual void Clear(MkDataNode* sharingNode = NULL) {}

	// 현재 페이지 이름 반환
	inline const MkHashStr& GetPageName(void) { return m_PageName; }

	MkBasePage(const MkHashStr& name) { m_PageName = name; }
	virtual ~MkBasePage() {}

protected:

	MkHashStr m_PageName;
};

//------------------------------------------------------------------------------------------------//

#endif
