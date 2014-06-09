#ifndef __MINIKEY_CORE_MKINSTANCEDEALLOCATOR_H__
#define __MINIKEY_CORE_MKINSTANCEDEALLOCATOR_H__

//------------------------------------------------------------------------------------------------//
// ��� ����/�������� ��ϵ� ��ü���� �ϰ� �Ҹ�
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

	// ũ�� ����
	inline void Reserve(unsigned int size) { m_InstanceList.reserve(size); }
	inline void Expand(unsigned int size) { m_InstanceList.reserve(m_InstanceList.size() + size); }

	// singleton ���
	void RegisterInstance(MkVirtualInstance* singletonPtr);

	// ��ϼ������ ��� �̱��� ����
	void ClearFrontToRear(void);

	// ��ϼ����� �������� ��� �̱��� ����
	void ClearRearToFront(void);

	MkInstanceDeallocator() {}
	MkInstanceDeallocator(unsigned int initSize);
	virtual ~MkInstanceDeallocator() { ClearFrontToRear(); }

private:

	std::vector<MkVirtualInstance*> m_InstanceList;
};

//------------------------------------------------------------------------------------------------//

#endif
