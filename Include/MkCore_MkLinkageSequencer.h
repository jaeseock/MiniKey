#ifndef __MINIKEY_CORE_MKLINKAGESEQUENCER_H__
#define __MINIKEY_CORE_MKLINKAGESEQUENCER_H__

//------------------------------------------------------------------------------------------------//
// link 관계를 통한 의존성 순서 결정기
// - 노드는 하나의 target 노드를 따름(target이 선순위, 자신이 후순위). 이 때 자신은 target의 follower가 됨
// - 여러 노드가 하나의 노드를 타게팅 하는 것은 가능
// - 결과적으로 한 노드는 하나의 target(선순위) 노드와 복수의 follower(후순위) 노드를 가짐
// - 링크가 없는 노드는 순서 상관 없음. 링크가 존재하는 노드만 순서성이 의미 있음
// - 환형 링크는 허용하지 않음(환형 링크에 관계된 노드는 결과에서 제외 됨)
//
// 노드, 링크 등록 후 UpdateSequence()를 호출하면 data별 순서가 생성 됨
//
// ex> a, b, c, d, e, f, g, h, i 의 9개 노드가 있을 때, a -> c -> d, f -> d, g -> c, i -> h 의 링크가 존재하면,
//     - a : target(c), follower 없음
//     - b : target 없음, follower 없음
//     - c : target(d), follower(a, g)
//     - d : target 없음, follower(c, f)
//     - e : target 없음, follower 없음
//     - f : target(d), follower 없음
//     - g : target(c), follower 없음
//     - h : target 없음, follower(i)
//     - i : target(h), follower 없음
//     의 관계가 되고 성질에 따른 순서로 보면,
//     - b
//     - d > c > a
//             > g
//         > f
//     - e
//     - h > i
//     가 되므로 결과적으로 순서는 b, d, c, a, g, f, e, h, i가 됨
//
//	wchar_t a = L'a';
//	wchar_t b = L'b';
//	wchar_t c = L'c';
//	wchar_t d = L'd';
//	wchar_t e = L'e';
//	wchar_t f = L'f';
//	wchar_t g = L'g';
//	wchar_t h = L'h';
//	wchar_t i = L'i';
//	MkLinkageSequencer<wchar_t, wchar_t*> seq;
//	seq.RegisterNode(a, &a);
//	seq.RegisterNode(b, &b);
//	seq.RegisterNode(c, &c);
//	seq.RegisterNode(d, &d);
//	seq.RegisterNode(e, &e);
//	seq.RegisterNode(f, &f);
//	seq.RegisterNode(g, &g);
//	seq.RegisterNode(h, &h);
//	seq.RegisterNode(i, &i);
//
//	seq.Link(a, c); // a -> c -> d
//	seq.Link(c, d);
//	seq.Link(f, d); // f -> d
//	seq.Link(g, c); // g -> c
//	seq.Link(i, h); // i -> h
//
//	seq.UpdateSequence();
//
//	MkStr result;
//	result.Reserve(9);
//	const MkArray<wchar_t*>& dataSeq = seq.GetDataSequence();
//	MK_INDEXING_LOOP(dataSeq, n)
//	{
//		result += (*dataSeq[n]);
//	}
//
//	MK_DEV_PANEL.MsgToLog(result, false); // L"bdcagfehi"
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"


template <class KeyType, class DataType>
class MkLinkageSequencer
{
protected:

	typedef struct __Node
	{
		MkArray<KeyType> target; // 0, 1
		MkArray<KeyType> follower; // 0, 1, 2, ..., n

		DataType data;
	}
	_Node;

public:

	// 노드 등록. 성공여부 반환
	inline bool RegisterNode(const KeyType& key, const DataType& data)
	{
		bool ok = !m_Nodes.Exist(key);
		if (ok)
		{
			_Node& newNode = m_Nodes.Create(key);
			newNode.data = data;

			m_NeedUpdate = true;
		}
		return ok;
	}

	// 노드 삭제. 성공여부 반환
	inline bool EraseNode(const KeyType& key)
	{
		bool ok = m_Nodes.Exist(key);
		if (ok)
		{
			m_Nodes.Erase(key);

			m_NeedUpdate = true;
		}
		return ok;
	}

	// 모든 노드 삭제
	inline void Clear(void)
	{
		m_Nodes.Clear();
		m_DataSequence.Clear();
		m_NeedUpdate = true;
	}

	// 링크 설정(follower -> target). 성공여부 반환
	inline bool Link(const KeyType& followerKey, const KeyType& targetKey)
	{
		bool ok = (m_Nodes.Exist(followerKey) && m_Nodes.Exist(targetKey));
		if (ok)
		{
			_Node& followNode = m_Nodes[followerKey];

			// 만약 follower에 이미 target이 존재하면 링크를 끊음
			if (!followNode.target.Empty())
			{
				const KeyType& oldTargetKey = followNode.target[0];
				if (targetKey != oldTargetKey)
				{
					_Node& oldTargetNode = m_Nodes[oldTargetKey];
					oldTargetNode.follower.EraseFirstInclusion(MkArraySection(0), followerKey);
					followNode.target.Flush();
				}
				else
					return true;
			}

			// follower의 target 지정
			followNode.target.PushBack(targetKey);

			// target의 follower 추가
			_Node& targetNode = m_Nodes[targetKey];
			targetNode.follower.PushBack(followerKey);

			m_NeedUpdate = true;
		}
		return ok;
	}

	// 링크 해제. target 무효화. 성공여부 반환
	inline bool Unlink(const KeyType& followerKey)
	{
		bool ok = m_Nodes.Exist(followerKey);
		if (ok)
		{
			_Node& followNode = m_Nodes[followerKey];
			if (!followNode.target.Empty())
			{
				const KeyType& oldTargetKey = followNode.target[0];
				_Node& oldTargetNode = m_Nodes[oldTargetKey];
				oldTargetNode.follower.EraseFirstInclusion(MkArraySection(0), followerKey);
				followNode.target.Flush();

				m_NeedUpdate = true;
			}
		}
		return ok;
	}

	// 순서 갱신
	inline void UpdateSequence(void)
	{
		if (m_NeedUpdate)
		{
			m_DataSequence.Flush();

			unsigned int nodeCount = m_Nodes.GetSize();
			if (nodeCount == 1)
			{
				const KeyType& key = m_Nodes.GetFirstKey();
				m_DataSequence.PushBack(m_Nodes[key].data);
			}
			else if (nodeCount > 1)
			{
				m_DataSequence.Reserve(nodeCount);

				MkMapLooper<KeyType, _Node> looper(m_Nodes);
				MK_STL_LOOP(looper)
				{
					const KeyType& currKey = looper.GetCurrentKey();
					_Node& currNode = looper.GetCurrentField();

					if (currNode.target.Empty())
					{
						// 링크 없음. 순서 상관 없으므로 바로 등록
						if  (currNode.follower.Empty())
						{
							m_DataSequence.PushBack(currNode.data);
						}
						// target은 없고 follower는 존재. 링크 시작 점
						else
						{
							_AddFollowers(currNode, m_DataSequence);
						}
					}
				}
			}

			m_NeedUpdate = false;
		}
	}

	// 순서 결과 반환
	inline MkArray<DataType>& GetDataSequence(void) { return m_DataSequence; }
	inline const MkArray<DataType>& GetDataSequence(void) const { return m_DataSequence; }

	MkLinkageSequencer() { m_NeedUpdate = false; }
	~MkLinkageSequencer() { Clear(); }

protected:

	inline void _RemoveTargetNode(const KeyType& followerKey, _Node& followNode)
	{
		if (!followNode.target.Empty())
		{
			const KeyType& oldTargetKey = followNode.target[0];
			_Node& oldTargetNode = m_Nodes[oldTargetKey];
			oldTargetNode.follower.EraseFirstInclusion(MkArraySection(0), followerKey);
			followNode.target.Flush();
		}
	}

	inline void _AddFollowers(_Node& currNode, MkArray<DataType>& dataSequence)
	{
		dataSequence.PushBack(currNode.data);

		MkArray<KeyType>& followerKeys = currNode.follower;
		if (!followerKeys.Empty())
		{
			MK_INDEXING_LOOP(followerKeys, i)
			{
				_AddFollowers(m_Nodes[followerKeys[i]], dataSequence);
			}
		}
	}

protected:

	MkMap<KeyType, _Node> m_Nodes;

	MkArray<DataType> m_DataSequence;

	bool m_NeedUpdate;
};

#endif
