#ifndef __MINIKEY_CORE_MKINSTANCEDEALLOCATOR_H__
#define __MINIKEY_CORE_MKINSTANCEDEALLOCATOR_H__

//------------------------------------------------------------------------------------------------//
// 등록 순서/역순서로 등록된 객체들을 일괄 소멸
//
// ex>
//	MkInstanceDeallocator deallocator(64);
//	deallocator.RegisterInstance(new TS);
//	deallocator.RegisterInstance(new TC(false));
//	...
//	deallocator.ClearFrontToRear(); // TS, TC
//	deallocator.ClearRearToFront(); // TC, TS
//------------------------------------------------------------------------------------------------//

#include <vector>
#include "MkCore_MkGlobalDefinition.h"


class MkInstanceDeallocator
{
public:

	// 크기 예약
	inline void Reserve(unsigned int size) { m_InstanceList.reserve(size); }
	inline void Expand(unsigned int size) { m_InstanceList.reserve(m_InstanceList.size() + size); }

	// singleton 등록
	void RegisterInstance(MkVirtualInstance* singletonPtr);

	// 등록순서대로 모든 싱글톤 해제
	void ClearFrontToRear(void);

	// 등록순서의 역순으로 모든 싱글톤 해제
	void ClearRearToFront(void);

	MkInstanceDeallocator() {}
	MkInstanceDeallocator(unsigned int initSize);
	virtual ~MkInstanceDeallocator() { ClearFrontToRear(); }

private:

	std::vector<MkVirtualInstance*> m_InstanceList;
};

//------------------------------------------------------------------------------------------------//

#endif
