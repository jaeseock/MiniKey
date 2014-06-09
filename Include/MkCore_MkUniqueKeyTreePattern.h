#ifndef __MINIKEY_CORE_MKUNIQUEKEYTREEPATTERN_H__
#define __MINIKEY_CORE_MKUNIQUEKEYTREEPATTERN_H__


//--------------------------------------------------------------------------------------------//
// 유일 키 트리 패턴
//
// unique key
// - 노드의 이름(key)은 전체 트리에서 유일
// - 참조시 단 한번의 인덱싱만 요구
// - 필드 인스턴스(노드) 자체는 이름을 가지지 않음
//
// tree instance가 모든 노드들을 소유
// - 하위 노드 유지는 반드시 상위노드 유지를 담보로 함
// - 따라서 하위노드 제거는 하위군을 모두 삭제(RemoveSubTree)하거나,
//   제거 후 자식들을 제거된 노드의 부모에 이어주는(DetachChildNode) 두가지 방법 존재
//--------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"


template <class KeyType, class InstanceType> // type key, type instance
class MkUniqueKeyTreePattern
{
public:

	//--------------------------------------------------------------------------------------------//
	// 생성 및 해제
	//--------------------------------------------------------------------------------------------//

	// 해제 후 초기화 (루트노드 등록)
	virtual void SetUp(const KeyType& rootNodeName, InstanceType* rootInstancePtr)
	{
		Clear();
		m_Pairs.Create(rootNodeName).instancePtr = rootInstancePtr;
		m_RootNodeName = rootNodeName;
	}

	// 해제 (루트노드 해제)
	virtual void Clear(void)
	{
		if (!m_RootNodeName.Empty())
		{
			RemoveSubTree(m_RootNodeName);
			m_RootNodeName.Clear();
		}
	}

	//--------------------------------------------------------------------------------------------//
	// 링크(attach, detach, remove), 최적화(rehash)
	//--------------------------------------------------------------------------------------------//

	// 자식노드 추가
	virtual bool AttachChildNode(const KeyType& parentNodeName, const KeyType& nodeName, InstanceType* nodeInstancePtr)
	{
		if (!m_Pairs.Exist(parentNodeName)) // 부모노드 존재하지 않음
			return false;

		if (m_Pairs.Exist(nodeName)) // 이미 해당 노드 존재
			return false;

		// 생성
		Linkage& linkage = m_Pairs.Create(nodeName);
		linkage.parent = parentNodeName;
		linkage.instancePtr = nodeInstancePtr;

		// 부모에 등록
		m_Pairs[parentNodeName].children.PushBack(nodeName);
		return true;
	}

	// 자식노드만 제거
	// 제거된 노드의 자식들은 부모에게 링크됨
	// (NOTE) 루트 노드는 호출 불가. 루트가 삭제되면 자식들이 붙을 노드가 없음
	virtual bool DetachChildNode(const KeyType& nodeName)
	{
		if ((nodeName == m_RootNodeName) || (!m_Pairs.Exist(nodeName))) // 루트 노드이거나 해당 노드 없음
			return false;

		Linkage& myLinkage = m_Pairs[nodeName];
		Linkage& parentLinkage = m_Pairs[myLinkage.parent];

		// 부모에서 링크 삭제
		parentLinkage.children.EraseFirstInclusion(MkArraySection(0), nodeName);

		// 자식 순회
		MK_INDEXING_LOOP(myLinkage.children, i)
		{
			const KeyType& key = myLinkage.children[i];

			// 현 부모에 자식들을 등록
			parentLinkage.children.PushBack(key);

			// 자식들의 부모로 현 부모를 링크
			m_Pairs[key].parent = myLinkage.parent;
		}

		// 인스턴스 삭제
		delete myLinkage.instancePtr;

		// 노드 삭제
		m_Pairs.Erase(nodeName);

		return true;
	}

	// 자식노드 및 하위군 삭제
	virtual bool RemoveSubTree(const KeyType& nodeName)
	{
		if (!m_Pairs.Exist(nodeName)) // 노드가 존재하지 않음
			return false;

		// 부모에서 링크 삭제
		Linkage& myLinkage = m_Pairs[nodeName];
		if (!myLinkage.parent.Empty())
		{
			Linkage& parentLinkage = m_Pairs[myLinkage.parent];
			parentLinkage.children.EraseFirstInclusion(MkArraySection(0), nodeName);
		}
		
		// 재귀 삭제 호출
		_DeleteNode(nodeName);

		return true;
	}

	// 전체 노드 최적화(rehash)
	// hash map 최적화를 위해 노드 attach/detach/remove 작업이 완료된 후 한 번 호출하길 권장
	inline void Rehash(void) { m_Pairs.Rehash(); }

	//--------------------------------------------------------------------------------------------//
	// 정보 참조
	//--------------------------------------------------------------------------------------------//

	// 루트로부터 타겟 노드까지의 경로를 반환
	virtual void GetPathToTargetNode(const KeyType& nodeName, MkArray<KeyType>& pathBuffer) const
	{
		if (m_RootNodeName.Empty() || (!m_Pairs.Exist(nodeName))) // 노드가 존재하지 않음
			return;

		// 타겟 노드에서 루트까지의 경로를 역산출(타겟 노드 포함)
		KeyType target = nodeName;
		while (true)
		{
			pathBuffer.PushBack(target);
			if (target != m_RootNodeName)
			{
				target = m_Pairs[target].parent; // 부모 탐색
			}
			else
				break; // 루트 도달
		}

		pathBuffer.ReverseOrder();
	}

	// 노드 존재 여부
	inline bool Exist(const KeyType& nodeName) const { return m_Pairs.Exist(nodeName); }

	// instance ptr 참조
	inline InstanceType* GetInstancePtr(const KeyType& nodeName) { return m_Pairs.Exist(nodeName) ? m_Pairs[nodeName].instancePtr : NULL; }
	inline const InstanceType* GetInstancePtr(const KeyType& nodeName) const { return m_Pairs.Exist(nodeName) ? m_Pairs[nodeName].instancePtr : NULL; }

	// instance 참조
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

	// parent 참조
	inline const KeyType& GetParentKey(const KeyType& nodeName) const
	{
		assert(m_Pairs.Exist(nodeName));
		return m_Pairs[nodeName].parent;
	}

	// children 참조
	inline const MkArray<KeyType>& GetChildNodeList(const KeyType& nodeName) const
	{
		assert(m_Pairs.Exist(nodeName));
		return m_Pairs[nodeName].children;
	}

	// leaf 여부
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
		// 해당 노드는 무조건 존재 가정
		Linkage& myLinkage = m_Pairs[nodeName];

		// 인스턴스 삭제
		delete myLinkage.instancePtr;

		// 자식 삭제
		MK_INDEXING_LOOP(myLinkage.children, i)
		{
			_DeleteNode(myLinkage.children[i]);
		}

		// 노드 삭제
		m_Pairs.Erase(nodeName);
	}

	//-------------------------------------------------------------//

protected:

	MkHashMap<KeyType, Linkage> m_Pairs;
	KeyType m_RootNodeName;
};

#endif