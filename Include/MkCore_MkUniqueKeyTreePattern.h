#ifndef __MINIKEY_CORE_MKUNIQUEKEYTREEPATTERN_H__
#define __MINIKEY_CORE_MKUNIQUEKEYTREEPATTERN_H__


//--------------------------------------------------------------------------------------------//
// ���� Ű Ʈ�� ����
//
// unique key
// - ����� �̸�(key)�� ��ü Ʈ������ ����
// - ������ �� �ѹ��� �ε��̸� �䱸
// - �ʵ� �ν��Ͻ�(���) ��ü�� �̸��� ������ ����
//
// tree instance�� ��� ������ ����
// - ���� ��� ������ �ݵ�� ������� ������ �㺸�� ��
// - ���� ������� ���Ŵ� �������� ��� ����(RemoveSubTree)�ϰų�,
//   ���� �� �ڽĵ��� ���ŵ� ����� �θ� �̾��ִ�(DetachChildNode) �ΰ��� ��� ����
//--------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"


template <class KeyType, class InstanceType> // type key, type instance
class MkUniqueKeyTreePattern
{
public:

	//--------------------------------------------------------------------------------------------//
	// ���� �� ����
	//--------------------------------------------------------------------------------------------//

	// ���� �� �ʱ�ȭ (��Ʈ��� ���)
	virtual void SetUp(const KeyType& rootNodeName, InstanceType* rootInstancePtr)
	{
		Clear();
		m_Pairs.Create(rootNodeName).instancePtr = rootInstancePtr;
		m_RootNodeName = rootNodeName;
	}

	// ���� (��Ʈ��� ����)
	virtual void Clear(void)
	{
		if (!m_RootNodeName.Empty())
		{
			RemoveSubTree(m_RootNodeName);
			m_RootNodeName.Clear();
		}
	}

	//--------------------------------------------------------------------------------------------//
	// ��ũ(attach, detach, remove), ����ȭ(rehash)
	//--------------------------------------------------------------------------------------------//

	// �ڽĳ�� �߰�
	virtual bool AttachChildNode(const KeyType& parentNodeName, const KeyType& nodeName, InstanceType* nodeInstancePtr)
	{
		if (!m_Pairs.Exist(parentNodeName)) // �θ��� �������� ����
			return false;

		if (m_Pairs.Exist(nodeName)) // �̹� �ش� ��� ����
			return false;

		// ����
		Linkage& linkage = m_Pairs.Create(nodeName);
		linkage.parent = parentNodeName;
		linkage.instancePtr = nodeInstancePtr;

		// �θ� ���
		m_Pairs[parentNodeName].children.PushBack(nodeName);
		return true;
	}

	// �ڽĳ�常 ����
	// ���ŵ� ����� �ڽĵ��� �θ𿡰� ��ũ��
	// (NOTE) ��Ʈ ���� ȣ�� �Ұ�. ��Ʈ�� �����Ǹ� �ڽĵ��� ���� ��尡 ����
	virtual bool DetachChildNode(const KeyType& nodeName)
	{
		if ((nodeName == m_RootNodeName) || (!m_Pairs.Exist(nodeName))) // ��Ʈ ����̰ų� �ش� ��� ����
			return false;

		Linkage& myLinkage = m_Pairs[nodeName];
		Linkage& parentLinkage = m_Pairs[myLinkage.parent];

		// �θ𿡼� ��ũ ����
		parentLinkage.children.EraseFirstInclusion(MkArraySection(0), nodeName);

		// �ڽ� ��ȸ
		MK_INDEXING_LOOP(myLinkage.children, i)
		{
			const KeyType& key = myLinkage.children[i];

			// �� �θ� �ڽĵ��� ���
			parentLinkage.children.PushBack(key);

			// �ڽĵ��� �θ�� �� �θ� ��ũ
			m_Pairs[key].parent = myLinkage.parent;
		}

		// �ν��Ͻ� ����
		delete myLinkage.instancePtr;

		// ��� ����
		m_Pairs.Erase(nodeName);

		return true;
	}

	// �ڽĳ�� �� ������ ����
	virtual bool RemoveSubTree(const KeyType& nodeName)
	{
		if (!m_Pairs.Exist(nodeName)) // ��尡 �������� ����
			return false;

		// �θ𿡼� ��ũ ����
		Linkage& myLinkage = m_Pairs[nodeName];
		if (!myLinkage.parent.Empty())
		{
			Linkage& parentLinkage = m_Pairs[myLinkage.parent];
			parentLinkage.children.EraseFirstInclusion(MkArraySection(0), nodeName);
		}
		
		// ��� ���� ȣ��
		_DeleteNode(nodeName);

		return true;
	}

	// ��ü ��� ����ȭ(rehash)
	// hash map ����ȭ�� ���� ��� attach/detach/remove �۾��� �Ϸ�� �� �� �� ȣ���ϱ� ����
	inline void Rehash(void) { m_Pairs.Rehash(); }

	//--------------------------------------------------------------------------------------------//
	// ���� ����
	//--------------------------------------------------------------------------------------------//

	// ��Ʈ�κ��� Ÿ�� �������� ��θ� ��ȯ
	virtual void GetPathToTargetNode(const KeyType& nodeName, MkArray<KeyType>& pathBuffer) const
	{
		if (m_RootNodeName.Empty() || (!m_Pairs.Exist(nodeName))) // ��尡 �������� ����
			return;

		// Ÿ�� ��忡�� ��Ʈ������ ��θ� ������(Ÿ�� ��� ����)
		KeyType target = nodeName;
		while (true)
		{
			pathBuffer.PushBack(target);
			if (target != m_RootNodeName)
			{
				target = m_Pairs[target].parent; // �θ� Ž��
			}
			else
				break; // ��Ʈ ����
		}

		pathBuffer.ReverseOrder();
	}

	// ��� ���� ����
	inline bool Exist(const KeyType& nodeName) const { return m_Pairs.Exist(nodeName); }

	// instance ptr ����
	inline InstanceType* GetInstancePtr(const KeyType& nodeName) { return m_Pairs.Exist(nodeName) ? m_Pairs[nodeName].instancePtr : NULL; }
	inline const InstanceType* GetInstancePtr(const KeyType& nodeName) const { return m_Pairs.Exist(nodeName) ? m_Pairs[nodeName].instancePtr : NULL; }

	// instance ����
	inline InstanceType& operator [] (const KeyType& nodeName)
	{
		assert(m_Pairs.Exist(nodeName));
		return *m_Pairs[nodeName].instancePtr;
	}

	inline const InstanceType& operator [] (const KeyType& nodeName) const
	{
		assert(m_Pairs.Exist(nodeName));
		return *m_Pairs[nodeName].instancePtr;
	}

	// parent ����
	inline const KeyType& GetParentKey(const KeyType& nodeName) const
	{
		assert(m_Pairs.Exist(nodeName));
		return m_Pairs[nodeName].parent;
	}

	// children ����
	inline const MkArray<KeyType>& GetChildNodeList(const KeyType& nodeName) const
	{
		assert(m_Pairs.Exist(nodeName));
		return m_Pairs[nodeName].children;
	}

	// leaf ����
	inline bool IsLeaf(const KeyType& nodeName) const { return GetChildNodeList(nodeName).Empty(); }

	virtual ~MkUniqueKeyTreePattern() { Clear(); }

	//-------------------------------------------------------------//

protected:

	typedef struct _Linkage
	{
		KeyType parent;
		InstanceType* instancePtr;
		MkArray<KeyType> children;
	}
	Linkage;

	//-------------------------------------------------------------//

protected:

	inline void _DeleteNode(const KeyType& nodeName)
	{
		// �ش� ���� ������ ���� ����
		Linkage& myLinkage = m_Pairs[nodeName];

		// �ν��Ͻ� ����
		delete myLinkage.instancePtr;

		// �ڽ� ����
		MK_INDEXING_LOOP(myLinkage.children, i)
		{
			_DeleteNode(myLinkage.children[i]);
		}

		// ��� ����
		m_Pairs.Erase(nodeName);
	}

	//-------------------------------------------------------------//

protected:

	MkHashMap<KeyType, Linkage> m_Pairs;
	KeyType m_RootNodeName;
};

#endif