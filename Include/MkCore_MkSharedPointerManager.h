#ifndef __MINIKEY_CORE_MKSHAREDPOINTERMANAGER_H__
#define __MINIKEY_CORE_MKSHAREDPOINTERMANAGER_H__


//------------------------------------------------------------------------------------------------//
// global instance - shared pointer manager
//
// MkSharedPointer의 reference counter 관리자
//
// thread-safe
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include "MkCore_MkHashMap.h"
#include "MkCore_MkSingletonPattern.h"

#if MKDEF_SHOW_SHARED_POINTER_REF_INFO
#include "MkCore_MkStr.h"
#endif

#define MK_SHARED_PTR_MGR MkSharedPointerManager::Instance()


//------------------------------------------------------------------------------------------------//

class MkSharedPointerManager : public MkSingletonPattern<MkSharedPointerManager>
{
public:

#if (MKDEF_SHOW_SHARED_POINTER_REF_INFO)
	void AssignPointer(ID64 id, const MkStr& name);
#else
	void AssignPointer(ID64 id);
#endif

	bool ReleasePointer(ID64 id);

	unsigned int GetReferenceCounter(ID64 id);

	MkSharedPointerManager();
	virtual ~MkSharedPointerManager();

protected:

	MkHashMap<ID64, unsigned int> m_RefCounter;
	CRITICAL_SECTION m_CS;

#if (MKDEF_SHOW_SHARED_POINTER_REF_INFO)
	MkHashMap<ID64, MkStr> m_InstanceNameTable;
#endif
};

//------------------------------------------------------------------------------------------------//

#endif
