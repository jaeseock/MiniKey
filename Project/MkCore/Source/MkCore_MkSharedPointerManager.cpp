
#include <assert.h>
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkSharedPointerManager.h"

//------------------------------------------------------------------------------------------------//

#if (MKDEF_SHOW_SHARED_POINTER_REF_INFO)
void MkSharedPointerManager::AssignPointer(ID64 id, const MkStr& name)
#else
void MkSharedPointerManager::AssignPointer(ID64 id)
#endif
{
	assert(id != 0);
	EnterCriticalSection(&m_CS);
	if (m_RefCounter.Exist(id))
	{
		++m_RefCounter[id];

#if (MKDEF_SHOW_SHARED_POINTER_REF_INFO)
		if (m_InstanceNameTable.Exist(id))
		{
			MK_DEV_PANEL.MsgToLog(L"smart ptr ref. 刘啊 : (" + m_InstanceNameTable[id] + L") " + MkStr(m_RefCounter[id] - 1) + L" -> " + MkStr(m_RefCounter[id]), true);
		}
#endif
	}
	else
	{
		m_RefCounter.Create(id, 1);

#if (MKDEF_SHOW_SHARED_POINTER_REF_INFO)
		if (!name.Empty())
		{
			m_InstanceNameTable.Create(id, name);
			MK_DEV_PANEL.MsgToLog(L"smart ptr ref. 积己 : (" + name + L")", true);
		}
#endif
	}
	LeaveCriticalSection(&m_CS);
}

bool MkSharedPointerManager::ReleasePointer(ID64 id)
{
	assert(id != 0);
	EnterCriticalSection(&m_CS);
	assert(m_RefCounter.Exist(id));
	assert(m_RefCounter[id] > 0);
	bool deletePtr = (m_RefCounter[id] == 1);
	if (deletePtr)
	{
		m_RefCounter.Erase(id);

#if (MKDEF_SHOW_SHARED_POINTER_REF_INFO)
		if (m_InstanceNameTable.Exist(id))
		{
			MK_DEV_PANEL.MsgToLog(L"smart ptr ref. 昏力 : (" + m_InstanceNameTable[id] + L")", true);
			m_InstanceNameTable.Erase(id);
		}
#endif
	}
	else
	{
		--m_RefCounter[id];

#if (MKDEF_SHOW_SHARED_POINTER_REF_INFO)
		if (m_InstanceNameTable.Exist(id))
		{
			MK_DEV_PANEL.MsgToLog(L"smart ptr ref. 皑家 : (" + m_InstanceNameTable[id] + L") " + MkStr(m_RefCounter[id] + 1) + L" -> " + MkStr(m_RefCounter[id]), true);
		}
#endif
	}
	LeaveCriticalSection(&m_CS);
	return deletePtr;
}

unsigned int MkSharedPointerManager::GetReferenceCounter(ID64 id)
{
	assert(id != 0);
	EnterCriticalSection(&m_CS);
	assert(m_RefCounter.Exist(id));
	unsigned int refCnt = m_RefCounter[id];
	LeaveCriticalSection(&m_CS);
	return refCnt;
}

MkSharedPointerManager::MkSharedPointerManager() : MkSingletonPattern<MkSharedPointerManager>()
{
	InitializeCriticalSectionAndSpinCount(&m_CS, 4000);
}

MkSharedPointerManager::~MkSharedPointerManager()
{
	EnterCriticalSection(&m_CS);
	assert(m_RefCounter.Empty());

#if (MKDEF_SHOW_SHARED_POINTER_REF_INFO)
	m_InstanceNameTable.Clear();
#endif

	LeaveCriticalSection(&m_CS);

	DeleteCriticalSection(&m_CS);
}

//------------------------------------------------------------------------------------------------//
