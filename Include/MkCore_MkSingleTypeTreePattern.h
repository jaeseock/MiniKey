#ifndef __MINIKEY_CORE_MKSINGLETYPETREEPATTERN_H__
#define __MINIKEY_CORE_MKSINGLETYPETREEPATTERN_H__

//------------------------------------------------------------------------------------------------//
// 단일 타입 트리 패턴
// - leaf를 특정짓지 않음 (모든 노드는 자식을 가질 수 있음)
// - detach로 분리된 노드는 독자적으로 유지됨
// - clear는 하위 노드들까지 모두 삭제함
// - 노드 이름은 형제들 사이에서 unique해야 함
// - read-only 속성이 설정되면 읽기만 가능해짐
//
// class A : public MkSingleTypeTreePattern<A>
// {
//     ...
// };
// 형태로 사용
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"


template <class TargetClass>
class MkSingleTypeTreePattern
{
public:

	//------------------------------------------------------------------------------------------------//
	// 이름
	//------------------------------------------------------------------------------------------------//

	// 노드 이름 변경
	// return : 성공여부
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

		return m_ParentNodePtr->ChangeChildNodeName(m_NodeName, newName); // 부모에게 자식 리스트를 변경하고 자신의 이름을 바꾸도록 함
	}

	// 노드 이름 반환
	inline const MkHashStr& GetNodeName(void) const { return m_NodeName; }

	// 직계 자식 노드 이름 변경
	// return : 성공여부
	virtual bool ChangeChildNodeName(const MkHashStr& oldName, const MkHashStr& newName)
	{
		// 이름이 비었거나, 이전 이름이 자식리스트에 없거나, 새 이름이 이미 자식리스트에 존재하면 실패
		if (GetReadOnly() || oldName.Empty() || newName.Empty() || (!m_ChildrenNode.Exist(oldName)) || m_ChildrenNode.Exist(newName))
			return false;

		if (oldName != newName)
		{
			MkHashStr lastName = oldName; // 복사하지 않고 참조자 그대로 쓰면 문자열이 덮어씌워짐
			TargetClass* childNode = m_ChildrenNode[lastName];
			childNode->__ChangeNodeName(newName);
			m_ChildrenNode.Erase(lastName);
			m_ChildrenNode.Create(newName, childNode);
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------//
	// 링크(attach, detach)
	//------------------------------------------------------------------------------------------------//

	// childNode를 직계 자식 노드로 등록
	// dynamic_cast 1회 실행
	// return : 성공여부
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
		if (m_ChildrenNode.Exist(childNodeName)) // 이미 동일 이름을 가진 자식노드가 존재
			return false;

		childNode->DetachFromParentNode(); // 이전 부모 노드로부터 떼어냄
		m_ChildrenNode.Create(childNodeName, childNode); // 새 자식노드로 추가
		childNode->__SetParentNode(myNode); // 새 부모로 자신 설정
		return true;
	}

	// 부모 노드로부터 떼어냄
	virtual void DetachFromParentNode(void)
	{
		if ((!GetReadOnly()) && (m_ParentNodePtr != NULL))
		{
			m_ParentNodePtr->DetachChildNode(GetNodeName());
		}
	}

	// childNodeName을 가진 직계 자식 노드를 떼어냄
	// return : 성공여부
	virtual bool DetachChildNode(const MkHashStr& childNodeName)
	{
		if (GetReadOnly() || (!m_ChildrenNode.Exist(childNodeName))) // 해당 이름을 가진 자식노드가 없음
			return false;

		TargetClass* childNode = m_ChildrenNode[childNodeName];
		m_ChildrenNode.Erase(childNodeName);
		childNode->__SetParentNode(NULL);
		return true;
	}

	//------------------------------------------------------------------------------------------------//
	// 생성(create), 삭제(clear), 최적화(rehash)
	//------------------------------------------------------------------------------------------------//

	// childNodeName을 가진 직계 자식 노드를 생성해 반환
	// dynamic_cast 1회 실행
	// return : 생성된 자식 노드
	inline TargetClass* CreateChildNode(const MkHashStr& childNodeName)
	{
		if (GetReadOnly() || m_ChildrenNode.Exist(childNodeName)) // 이미 동일 이름을 가진 자식노드가 존재
			return NULL;

		TargetClass* myNode = dynamic_cast<TargetClass*>(this);
		if (myNode == NULL)
			return NULL;

		TargetClass* childNode = new TargetClass(childNodeName);
		m_ChildrenNode.Create(childNodeName, childNode);
		childNode->__SetParentNode(myNode);
		return childNode;
	}

	// 하위 모든 자식 노드 삭제
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

	// 노드 최적화
	// attach/detach/create/clear 호출이 종료되고 노드 구조가 변하지 않게 되면 한 번 호출을 권장
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

	// read-only 여부 반환
	inline bool GetReadOnly(void) const { return m_ReadOnly; }

	//------------------------------------------------------------------------------------------------//
	// 정보 반환
	//------------------------------------------------------------------------------------------------//

	// 부모 노드 포인터 반환
	inline TargetClass* GetParentNode(void) const { return m_ParentNodePtr; }

	// 직계 자식 노드 존재여부 반환
	inline bool ChildExist(const MkHashStr& childNodeName) const { return m_ChildrenNode.Exist(childNodeName); }

	// 직계 자식 노드 포인터 반환
	inline TargetClass* GetChildNode(const MkHashStr& childNodeName) { return (m_ChildrenNode.Exist(childNodeName)) ? m_ChildrenNode[childNodeName] : NULL; }
	inline const TargetClass* GetChildNode(const MkHashStr& childNodeName) const { return (m_ChildrenNode.Exist(childNodeName)) ? m_ChildrenNode[childNodeName] : NULL; }

	// 조상 노드 중 해당 이름을 가진 가장 가까운 포인터 반환
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

	// 자손 노드 포인터 반환. 직계만이 아니라 하위 모든 노드를 대상으로 탐색
	// (NOTE) 이름 규칙이 형제들 사이만이 아닌 하위 전체에서 유일해야 신뢰성이 보장 됨
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

	// 직계 자식 노드 참조 반환
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

	// 루트 노드로부터의 깊이를 반환 (root가 0)
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

	// 루트 노드로부터 자신 노드까지의 경로명을 순서대로 MkArray에 담아 반환
	// 최소한 자신 이름은 들어가 있음
	inline void GetNodeNameListFromRootNode(MkArray<MkHashStr>& nameList) const
	{
		nameList.Clear();
		unsigned int depth = GetDepthFromRootNode();
		nameList.Reserve(depth + 1);
		nameList.PushBack(GetNodeName()); // 자신 이름

		TargetClass* parentNode = m_ParentNodePtr;
		while (true)
		{
			if (parentNode == NULL)
				break;

			nameList.PushBack(parentNode->GetNodeName()); // 부모 이름
			parentNode = parentNode->GetParentNode();
		}

		nameList.ReverseOrder();
	}

	// 직계 자식 노드 이름 리스트 반환
	inline unsigned int GetChildNodeList(MkArray<MkHashStr>& childNodeList) const { return m_ChildrenNode.GetKeyList(childNodeList); }

	// 직계 자식 노드 수 반환
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
