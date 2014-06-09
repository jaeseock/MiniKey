#ifndef __MINIKEY_CORE_MKSHAREDPOINTER_H__
#define __MINIKEY_CORE_MKSHAREDPOINTER_H__

//------------------------------------------------------------------------------------------------//
// reference counter�� �޸𸮸� �����ϴ� shared smart pointer
// �Ϲ����� ������ ���� NULL�� �Ҵ��ϸ� ���� �����
//
// ������� ���� MKDEF_SHOW_SHARED_POINTER_REF_INFO �÷��װ� TRUE�� ��� instance name �Ҵ� ����
// SetInstanceName()���� �̸��� �Ҵ� �� ��ü�� ���� ���� ����
// ex>
//	MkSceneNodePtr pnode;
//	pnode.SetInstanceName(L"parent");
//	pnode = new MkSceneNode(L"parent");
//
// (NOTE) ����Ʈ �����ʹ� ������ �� ũ�����̱� ������ ��� ����ó���� ������ ��ź��
// �Ȱ� ���� ���̳� ��������. ���� ���� �߻��� �ٷ� assert�� ��Ʈ��
//
// (NOTE) reference counting�� operator=�� ���� �̷������ ������ �̸� ������� �ʴ� �Ҵ��� ������ ��
// ex> �⺻ STL �����̳�(vector, deque, map, etc...)
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
