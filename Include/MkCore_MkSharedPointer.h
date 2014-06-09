#ifndef __MINIKEY_CORE_MKSHAREDPOINTER_H__
#define __MINIKEY_CORE_MKSHAREDPOINTER_H__

//------------------------------------------------------------------------------------------------//
// reference counter로 메모리를 공유하는 shared smart pointer
// 일반적인 사용법과 같이 NULL을 할당하면 해제 명령임
//
// 디버깅을 위해 MKDEF_SHOW_SHARED_POINTER_REF_INFO 플래그가 TRUE일 경우 instance name 할당 가능
// SetInstanceName()으로 이름이 할당 된 객체의 참조 정보 추적
// ex>
//	MkSceneNodePtr pnode;
//	pnode.SetInstanceName(L"parent");
//	pnode = new MkSceneNode(L"parent");
//
// (NOTE) 스마트 포인터는 에러가 곧 크래쉬이기 때문에 어설픈 예외처리는 오히려 폭탄을
// 안고 가는 격이나 마찬가지. 따라서 에러 발생시 바로 assert로 터트림
//
// (NOTE) reference counting이 operator=를 통해 이루어지기 때문에 이를 사용하지 않는 할당은 문제가 됨
// ex> 기본 STL 컨테이너(vector, deque, map, etc...)
//------------------------------------------------------------------------------------------------//

#include <assert.h>
#include "MkCore_MkSharedPointerManager.h"

#if (MKDEF_SHOW_SHARED_POINTER_REF_INFO)
#include "MkCore_MkStr.h"
#endif


template <class TargetClass>
class MkSharedPointer
{
public:

	inline MkSharedPointer& operator = (TargetClass* ptr)
	{
		if (ptr == NULL)
		{
			_Unload();
			m_Ptr = NULL;
		}
		else
		{
			assert(m_Ptr == NULL);
			m_Ptr = ptr;

#if (MKDEF_SHOW_SHARED_POINTER_REF_INFO)
			MK_SHARED_PTR_MGR.AssignPointer(MK_PTR_TO_ID64(m_Ptr), m_InstanceName);
#else
			MK_SHARED_PTR_MGR.AssignPointer(MK_PTR_TO_ID64(m_Ptr));
#endif
		}
		return *this;
	}

	inline MkSharedPointer& operator = (const MkSharedPointer& ptr) { return operator=(const_cast<TargetClass*>(ptr.GetPtr())); }

	inline TargetClass* GetPtr(void) { return m_Ptr; }
	inline const TargetClass* GetPtr(void) const { return m_Ptr; }
	
	inline operator TargetClass*() { return m_Ptr; }
	inline operator const TargetClass*() const { return m_Ptr; }

	inline TargetClass* operator ->() { return m_Ptr; }
	inline const TargetClass* operator ->() const { return m_Ptr; }

	inline unsigned int GetReferenceCounter(void) const
	{
		return (m_Ptr == NULL) ? 0 : MK_SHARED_PTR_MGR.GetReferenceCounter(MK_PTR_TO_ID64(m_Ptr));
	}

	inline void SetInstanceName(const MkStr& name)
	{
#if (MKDEF_SHOW_SHARED_POINTER_REF_INFO)
		m_InstanceName = name;
#endif
	}

	MkSharedPointer()
	{
		m_Ptr = NULL;
	}

	MkSharedPointer(TargetClass* ptr)
	{
		m_Ptr = NULL;
		*this = ptr;
	}

	virtual ~MkSharedPointer()
	{
		_Unload();
	}

private:

	inline void _Unload(void)
	{
		if (m_Ptr != NULL)
		{
			if (MK_SHARED_PTR_MGR.ReleasePointer(MK_PTR_TO_ID64(m_Ptr)))
			{
				delete m_Ptr;
				m_Ptr = NULL;
			}
		}
	}

private:

	TargetClass* m_Ptr;

#if (MKDEF_SHOW_SHARED_POINTER_REF_INFO)
protected:
	MkStr m_InstanceName;
#endif
};

//------------------------------------------------------------------------------------------------//

#endif
