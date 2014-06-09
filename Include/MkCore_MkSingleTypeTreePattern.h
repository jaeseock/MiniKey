#ifndef __MINIKEY_CORE_MKSINGLETYPETREEPATTERN_H__
#define __MINIKEY_CORE_MKSINGLETYPETREEPATTERN_H__

//------------------------------------------------------------------------------------------------//
// ���� Ÿ�� Ʈ�� ����
// - leaf�� Ư������ ���� (��� ���� �ڽ��� ���� �� ����)
// - detach�� �и��� ���� ���������� ������
// - clear�� ���� ������� ��� ������
// - ��� �̸��� ������ ���̿��� unique�ؾ� ��
// - read-only �Ӽ��� �����Ǹ� �б⸸ ��������
//
// class A : public MkSingleTypeTreePattern<A>
// {
//     ...
// };
// ���·� ���
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"


template <class TargetClass>
class MkSingleTypeTreePattern
{
public:

	//------------------------------------------------------------------------------------------------//
	// �̸�
	//------------------------------------------------------------------------------------------------//

	// ��� �̸� ����
	// return : ��������
	virtual bool ChangeNodeName(const MkHashStr& newName)
	{
		if (GetReadOnly() || newName.Empty())
			return false;

		if (m_NodeName == newName)
			return true;

		if (m_ParentNodePtr == NULL)
		{
			m_NodeName = newName;
			return true;
		}

		return m_ParentNodePtr->ChangeChildNodeName(m_NodeName, newName); // �θ𿡰� �ڽ� ����Ʈ�� �����ϰ� �ڽ��� �̸��� �ٲٵ��� ��
	}

	// ��� �̸� ��ȯ
	inline const MkHashStr& GetNodeName(void) const { return m_NodeName; }

	// ���� �ڽ� ��� �̸� ����
	// return : ��������
	virtual bool ChangeChildNodeName(const MkHashStr& oldName, const MkHashStr& newName)
	{
		// �̸��� ����ų�, ���� �̸��� �ڽĸ���Ʈ�� ���ų�, �� �̸��� �̹� �ڽĸ���Ʈ�� �����ϸ� ����
		if (GetReadOnly() || oldName.Empty() || newName.Empty() || (!m_ChildrenNode.Exist(oldName)) || m_ChildrenNode.Exist(newName))
			return false;

		if (oldName != newName)
		{
			MkHashStr lastName = oldName; // �������� �ʰ� ������ �״�� ���� ���ڿ��� �������
			TargetClass* childNode = m_ChildrenNode[lastName];
			childNode->__ChangeNodeName(newName);
			m_ChildrenNode.Erase(lastName);
			m_ChildrenNode.Create(newName, childNode);
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------//
	// ��ũ(attach, detach)
	//------------------------------------------------------------------------------------------------//

	// childNode�� ���� �ڽ� ���� ���
	// dynamic_cast 1ȸ ����
	// return : ��������
	virtual bool AttachChildNode(TargetClass* childNode)
	{
		if (GetReadOnly() || (childNode == NULL))
			return false;

		TargetClass* myNode = dynamic_cast<TargetClass*>(this);
		if (myNode == NULL)
			return false;

		if (myNode == childNode->GetParentNode())
			return true;

		const MkHashStr& childNodeName = childNode->GetNodeName();
		if (m_ChildrenNode.Exist(childNodeName)) // �̹� ���� �̸��� ���� �ڽĳ�尡 ����
			return false;

		childNode->DetachFromParentNode(); // ���� �θ� ���κ��� ���
		m_ChildrenNode.Create(childNodeName, childNode); // �� �ڽĳ��� �߰�
		childNode->__SetParentNode(myNode); // �� �θ�� �ڽ� ����
		return true;
	}

	// �θ� ���κ��� ���
	virtual void DetachFromParentNode(void)
	{
		if ((!GetReadOnly()) && (m_ParentNodePtr != NULL))
		{
			m_ParentNodePtr->DetachChildNode(GetNodeName());
		}
	}

	// childNodeName�� ���� ���� �ڽ� ��带 ���
	// return : ��������
	virtual bool DetachChildNode(const MkHashStr& childNodeName)
	{
		if (GetReadOnly() || (!m_ChildrenNode.Exist(childNodeName))) // �ش� �̸��� ���� �ڽĳ�尡 ����
			return false;

		TargetClass* childNode = m_ChildrenNode[childNodeName];
		m_ChildrenNode.Erase(childNodeName);
		childNode->__SetParentNode(NULL);
		return true;
	}

	//------------------------------------------------------------------------------------------------//
	// ����(create), ����(clear), ����ȭ(rehash)
	//------------------------------------------------------------------------------------------------//

	// childNodeName�� ���� ���� �ڽ� ��带 ������ ��ȯ
	// dynamic_cast 1ȸ ����
	// return : ������ �ڽ� ���
	inline TargetClass* CreateChildNode(const MkHashStr& childNodeName)
	{
		if (GetReadOnly() || m_ChildrenNode.Exist(childNodeName)) // �̹� ���� �̸��� ���� �ڽĳ�尡 ����
			return NULL;

		TargetClass* myNode = dynamic_cast<TargetClass*>(this);
		if (myNode == NULL)
			return NULL;

		TargetClass* childNode = new TargetClass(childNodeName);
		m_ChildrenNode.Create(childNodeName, childNode);
		childNode->__SetParentNode(myNode);
		return childNode;
	}

	// ���� ��� �ڽ� ��� ����
	virtual void Clear(void)
	{
		if ((!GetReadOnly()) && (!m_ChildrenNode.Empty()))
		{
			MkHashMapLooper<MkHashStr, TargetClass*> looper(m_ChildrenNode);
			MK_STL_LOOP(looper)
			{
				delete looper.GetCurrentField();
			}
			m_ChildrenNode.Clear();
		}
	}

	// ��� ����ȭ
	// attach/detach/create/clear ȣ���� ����ǰ� ��� ������ ������ �ʰ� �Ǹ� �� �� ȣ���� ����
	virtual void Rehash(void)
	{
		if ((!GetReadOnly()) && (!m_ChildrenNode.Empty()))
		{
			m_ChildrenNode.Rehash();

			MkHashMapLooper<MkHashStr, TargetClass*> looper(m_ChildrenNode);
			MK_STL_LOOP(looper)
			{
				looper.GetCurrentField()->Rehash();
			}
		}
	}

	//------------------------------------------------------------------------------------------------//
	// read-only
	//------------------------------------------------------------------------------------------------//

	virtual void SetReadOnly(bool enable)
	{
		if ((GetReadOnly() && (!enable)) || // on -> off
			((!GetReadOnly()) && enable)) // off -> on
		{
			m_ReadOnly = enable;

			MkHashMapLooper<MkHashStr, TargetClass*> looper(m_ChildrenNode);
			MK_STL_LOOP(looper)
			{
				looper.GetCurrentField()->SetReadOnly(enable);
			}
		}
	}

	// read-only ���� ��ȯ
	inline bool GetReadOnly(void) const { return m_ReadOnly; }

	//------------------------------------------------------------------------------------------------//
	// ���� ��ȯ
	//------------------------------------------------------------------------------------------------//

	// �θ� ��� ������ ��ȯ
	inline TargetClass* GetParentNode(void) const { return m_ParentNodePtr; }

	// ���� �ڽ� ��� ���翩�� ��ȯ
	inline bool ChildExist(const MkHashStr& childNodeName) const { return m_ChildrenNode.Exist(childNodeName); }

	// ���� �ڽ� ��� ������ ��ȯ
	inline TargetClass* GetChildNode(const MkHashStr& childNodeName) { return (m_ChildrenNode.Exist(childNodeName)) ? m_ChildrenNode[childNodeName] : NULL; }
	inline const TargetClass* GetChildNode(const MkHashStr& childNodeName) const { return (m_ChildrenNode.Exist(childNodeName)) ? m_ChildrenNode[childNodeName] : NULL; }

	// ���� ��� �� �ش� �̸��� ���� ���� ����� ������ ��ȯ
	inline TargetClass* GetAncestorNode(const MkHashStr& nodeName)
	{
		TargetClass* parentNode = m_ParentNodePtr;
		while (true)
		{
			if (parentNode == NULL)
				return NULL;

			if (parentNode->GetNodeName() == nodeName)
				return parentNode;

			parentNode = parentNode->GetParentNode();
		}
	}

	inline const TargetClass* GetAncestorNode(const MkHashStr& nodeName) const
	{
		TargetClass* parentNode = m_ParentNodePtr;
		while (true)
		{
			if (parentNode == NULL)
				return NULL;

			if (parentNode->GetNodeName() == nodeName)
				return parentNode;

			parentNode = parentNode->GetParentNode();
		}
	}

	// �ڼ� ��� ������ ��ȯ. ���踸�� �ƴ϶� ���� ��� ��带 ������� Ž��
	// (NOTE) �̸� ��Ģ�� ������ ���̸��� �ƴ� ���� ��ü���� �����ؾ� �ŷڼ��� ���� ��
	inline TargetClass* GetDecendentNode(const MkHashStr& childNodeName)
	{
		TargetClass* targetNode = GetChildNode(childNodeName);
		if ((targetNode == NULL) && (!m_ChildrenNode.Empty()))
		{
			MkHashMapLooper<MkHashStr, TargetClass*> looper(m_ChildrenNode);
			MK_STL_LOOP(looper)
			{
				targetNode = looper.GetCurrentField()->GetDecendentNode(childNodeName);
				if (targetNode != NULL)
					return targetNode;
			}
		}
		return targetNode;
	}

	inline const TargetClass* GetDecendentNode(const MkHashStr& childNodeName) const
	{
		const TargetClass* targetNode = GetChildNode(childNodeName);
		if ((targetNode == NULL) && (!m_ChildrenNode.Empty()))
		{
			MkConstHashMapLooper<MkHashStr, TargetClass*> looper(m_ChildrenNode);
			MK_STL_LOOP(looper)
			{
				const TargetClass* node = looper.GetCurrentField()->GetDecendentNode(childNodeName);
				if (node != NULL)
					return node;
			}
		}
		return targetNode;
	}

	// ���� �ڽ� ��� ���� ��ȯ
	inline TargetClass& operator [] (const MkHashStr& childNodeName)
	{
		assert(!GetReadOnly());
		TargetClass* childNode = GetChildNode(childNodeName);
		assert(childNode != NULL);
		return *childNode;
	}

	inline const TargetClass& operator [] (const MkHashStr& childNodeName) const
	{
		const TargetClass* childNode = GetChildNode(childNodeName);
		assert(childNode != NULL);
		return *childNode;
	}

	// ��Ʈ ���κ����� ���̸� ��ȯ (root�� 0)
	unsigned int GetDepthFromRootNode(void) const
	{
		TargetClass* parentNode = m_ParentNodePtr;
		unsigned int depthFromRoot = 0;
		while (true)
		{
			if (parentNode == NULL)
				return depthFromRoot;

			parentNode = parentNode->GetParentNode();
			++depthFromRoot;
		}
	}

	// ��Ʈ ���κ��� �ڽ� �������� ��θ��� ������� MkArray�� ��� ��ȯ
	// �ּ��� �ڽ� �̸��� �� ����
	inline void GetNodeNameListFromRootNode(MkArray<MkHashStr>& nameList) const
	{
		nameList.Clear();
		unsigned int depth = GetDepthFromRootNode();
		nameList.Reserve(depth + 1);
		nameList.PushBack(GetNodeName()); // �ڽ� �̸�

		TargetClass* parentNode = m_ParentNodePtr;
		while (true)
		{
			if (parentNode == NULL)
				break;

			nameList.PushBack(parentNode->GetNodeName()); // �θ� �̸�
			parentNode = parentNode->GetParentNode();
		}

		nameList.ReverseOrder();
	}

	// ���� �ڽ� ��� �̸� ����Ʈ ��ȯ
	inline unsigned int GetChildNodeList(MkArray<MkHashStr>& childNodeList) const { return m_ChildrenNode.GetKeyList(childNodeList); }

	// ���� �ڽ� ��� �� ��ȯ
	inline unsigned int GetChildNodeCount(void) const { return m_ChildrenNode.GetSize(); }

	//------------------------------------------------------------------------------------------------//

	MkSingleTypeTreePattern()
	{
		m_ParentNodePtr = NULL;
		m_ReadOnly = false;
	}

	MkSingleTypeTreePattern(const MkHashStr& name)
	{
		m_NodeName = name;
		m_ParentNodePtr = NULL;
		m_ReadOnly = false;
	}
	virtual ~MkSingleTypeTreePattern()
	{
		m_ReadOnly = false;
		Clear();
	}

public:

	inline void __ChangeNodeName(const MkHashStr& newName) { m_NodeName = newName; }
	inline void __SetParentNode(TargetClass* parentNode) { m_ParentNodePtr = parentNode; }

protected:

	MkHashStr m_NodeName;

	TargetClass* m_ParentNodePtr;
	MkHashMap<MkHashStr, TargetClass*> m_ChildrenNode;

	bool m_ReadOnly;
};

//------------------------------------------------------------------------------------------------//

#endif
