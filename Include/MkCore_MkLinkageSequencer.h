#ifndef __MINIKEY_CORE_MKLINKAGESEQUENCER_H__
#define __MINIKEY_CORE_MKLINKAGESEQUENCER_H__

//------------------------------------------------------------------------------------------------//
// link ���踦 ���� ������ ���� ������
// - ���� �ϳ��� target ��带 ����(target�� ������, �ڽ��� �ļ���). �� �� �ڽ��� target�� follower�� ��
// - ���� ��尡 �ϳ��� ��带 Ÿ���� �ϴ� ���� ����
// - ��������� �� ���� �ϳ��� target(������) ���� ������ follower(�ļ���) ��带 ����
// - ��ũ�� ���� ���� ���� ��� ����. ��ũ�� �����ϴ� ��常 �������� �ǹ� ����
// - ȯ�� ��ũ�� ������� ����(ȯ�� ��ũ�� ����� ���� ������� ���� ��)
//
// ���, ��ũ ��� �� UpdateSequence()�� ȣ���ϸ� data�� ������ ���� ��
//
// ex> a, b, c, d, e, f, g, h, i �� 9�� ��尡 ���� ��, a -> c -> d, f -> d, g -> c, i -> h �� ��ũ�� �����ϸ�,
//     - a : target(c), follower ����
//     - b : target ����, follower ����
//     - c : target(d), follower(a, g)
//     - d : target ����, follower(c, f)
//     - e : target ����, follower ����
//     - f : target(d), follower ����
//     - g : target(c), follower ����
//     - h : target ����, follower(i)
//     - i : target(h), follower ����
//     �� ���谡 �ǰ� ������ ���� ������ ����,
//     - b
//     - d > c > a
//             > g
//         > f
//     - e
//     - h > i
//     �� �ǹǷ� ��������� ������ b, d, c, a, g, f, e, h, i�� ��
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

	// ��� ���. �������� ��ȯ
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

	// ��� ����. �������� ��ȯ
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

	// ��� ��� ����
	inline void Clear(void)
	{
		m_Nodes.Clear();
		m_DataSequence.Clear();
		m_NeedUpdate = true;
	}

	// ��ũ ����(follower -> target). �������� ��ȯ
	inline bool Link(const KeyType& followerKey, const KeyType& targetKey)
	{
		bool ok = (m_Nodes.Exist(followerKey) && m_Nodes.Exist(targetKey));
		if (ok)
		{
			_Node& followNode = m_Nodes[followerKey];

			// ���� follower�� �̹� target�� �����ϸ� ��ũ�� ����
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

			// follower�� target ����
			followNode.target.PushBack(targetKey);

			// target�� follower �߰�
			_Node& targetNode = m_Nodes[targetKey];
			targetNode.follower.PushBack(followerKey);

			m_NeedUpdate = true;
		}
		return ok;
	}

	// ��ũ ����. target ��ȿȭ. �������� ��ȯ
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

	// ���� ����
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
						// ��ũ ����. ���� ��� �����Ƿ� �ٷ� ���
						if  (currNode.follower.Empty())
						{
							m_DataSequence.PushBack(currNode.data);
						}
						// target�� ���� follower�� ����. ��ũ ���� ��
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

	// ���� ��� ��ȯ
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
