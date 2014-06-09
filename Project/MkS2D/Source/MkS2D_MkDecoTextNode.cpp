
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkDecoTextNode.h"


//------------------------------------------------------------------------------------------------//

bool MkDecoTextNode::SetUp(const MkPathName& filePath)
{
	MkDataNode node;
	bool ok = node.Load(filePath);
	if (ok)
	{
		SetUp(node);

		Rehash();
	}
	return ok;
}

void MkDecoTextNode::SetUp(const MkDataNode& node)
{
	Clear();

	// deco string 등록
	if (node.GetUnitCount() > 0)
	{
		MkArray<MkHashStr> keys;
		node.GetUnitKeyList(keys);

		MK_INDEXING_LOOP(keys, i)
		{
			MkStr buffer;
			if (node.GetData(keys[i], buffer, 0) && (buffer.GetFirstValidPosition() != MKDEF_ARRAY_ERROR))
			{
				m_Table.Create(keys[i]).SetUp(buffer);
			}
		}

		m_Table.Rehash();
	}

	// child node 생성
	if (node.GetChildNodeCount() > 0)
	{
		MkArray<MkHashStr> keys;
		node.GetChildNodeList(keys);

		MK_INDEXING_LOOP(keys, i)
		{
			const MkHashStr& currKey = keys[i];
			CreateChildNode(currKey)->SetUp(*node.GetChildNode(currKey));
		}
	}
}

const MkDecoStr& MkDecoTextNode::GetDecoText(const MkHashStr& key) const
{
	return m_Table.Exist(key) ? m_Table[key] : MkDecoStr::Null;
}

const MkDecoStr& MkDecoTextNode::GetDecoText(const MkHashStr& n0, const MkHashStr& key) const
{
	const MkDecoTextNode* childNode = GetChildNode(n0);
	return (childNode == NULL) ? MkDecoStr::Null : childNode->GetDecoText(key);
}

const MkDecoStr& MkDecoTextNode::GetDecoText(const MkHashStr& n0, const MkHashStr& n1, const MkHashStr& key) const
{
	const MkDecoTextNode* childNode = GetChildNode(n0);
	return (childNode == NULL) ? MkDecoStr::Null : childNode->GetDecoText(n1, key);
}

const MkDecoStr& MkDecoTextNode::GetDecoText(const MkHashStr& n0, const MkHashStr& n1, const MkHashStr& n2, const MkHashStr& key) const
{
	const MkDecoTextNode* childNode = GetChildNode(n0);
	return (childNode == NULL) ? MkDecoStr::Null : childNode->GetDecoText(n1, n2, key);
}

const MkDecoStr& MkDecoTextNode::GetDecoText(const MkHashStr& n0, const MkHashStr& n1, const MkHashStr& n2, const MkHashStr& n3, const MkHashStr& key) const
{
	const MkDecoTextNode* childNode = GetChildNode(n0);
	return (childNode == NULL) ? MkDecoStr::Null : childNode->GetDecoText(n1, n2, n3, key);
}

const MkDecoStr& MkDecoTextNode::GetDecoText(const MkHashStr& n0, const MkHashStr& n1, const MkHashStr& n2, const MkHashStr& n3, const MkHashStr& n4, const MkHashStr& key) const
{
	const MkDecoTextNode* childNode = GetChildNode(n0);
	return (childNode == NULL) ? MkDecoStr::Null : childNode->GetDecoText(n1, n2, n3, n4, key);
}

const MkDecoStr& MkDecoTextNode::GetDecoText(const MkArray<MkHashStr>& nodeNameAndKey, unsigned int offset) const
{
	unsigned int size = nodeNameAndKey.GetSize();
	if (offset < size)
	{
		size -= offset;
		if (size == 1)
		{
			return GetDecoText(nodeNameAndKey[offset]);
		}
		else // (size > 1)
		{
			const MkDecoTextNode* childNode = GetChildNode(nodeNameAndKey[offset]);
			if (childNode != NULL)
			{
				return childNode->GetDecoText(nodeNameAndKey, offset + 1);
			}
		}
	}
	return MkDecoStr::Null;
}

void MkDecoTextNode::Clear(void)
{
	MkSingleTypeTreePattern<MkDecoTextNode>::Clear();

	m_Table.Clear();
}

MkDecoTextNode::MkDecoTextNode() : MkSingleTypeTreePattern<MkDecoTextNode>(L"Root")
{
}

//------------------------------------------------------------------------------------------------//
