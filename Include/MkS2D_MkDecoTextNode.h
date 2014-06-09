#pragma once


//------------------------------------------------------------------------------------------------//
// deco text table
// ��Ģ������ �� ���ڿ��� ��ϵ� �� ����(���õ�)
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingleTypeTreePattern.h"
#include "MkS2D_MkDecoStr.h"


class MkPathName;
class MkDataNode;

class MkDecoTextNode : public MkSingleTypeTreePattern<MkDecoTextNode>
{
public:

	// ���Ϸ� �ʱ�ȭ
	bool SetUp(const MkPathName& filePath);

	// ���� �ʱ�ȭ
	void SetUp(const MkDataNode& node);

	// deco string ��ȯ
	// �������� �ʴ� node�� key��� MkDecoStr::Null ��ȯ
	//
	// ��Ģ��ζ�� GetChildNode() ȣ�� �ݺ����� �����ϰ� Ž���� ������ ������ ���Ǽ� ���鿡��
	// �� ���� ȣ��� Ž���� �Ϸ��� �� �ֵ��� ��. �ʿ��ϸ� �� �߰��� ���
	// ex>
	//	MkDecoStr buffer;
	//	MkDecoTextNode* targetNode = beginNode->GetChildNode(L"character");
	//	if (targetNode != NULL)
	//		targetNode = targetNode->GetChildNode(L"status");
	//	if (targetNode != NULL)
	//		buffer = targetNode->GetDecoText(L"strength desc");
	//
	// �� �ڵ�� �Ʒ� �ڵ�� ����
	//	const MkDecoStr& buffer = beginNode->GetDecoText(L"character", L"status", L"strength desc");
	//
	// Ȥ�� �Ʒ� �ڵ�� ����
	//	MkArray<MkHashStr> nodeNameAndKey(3);
	//	nodeNameAndKey.PushBack(L"character");
	//	nodeNameAndKey.PushBack(L"status");
	//	nodeNameAndKey.PushBack(L"strength desc");
	//	const MkDecoStr& buffer = beginNode->GetDecoText(nodeNameAndKey);

	const MkDecoStr& GetDecoText(const MkHashStr& key) const;
	const MkDecoStr& GetDecoText(const MkHashStr& n0, const MkHashStr& key) const;
	const MkDecoStr& GetDecoText(const MkHashStr& n0, const MkHashStr& n1, const MkHashStr& key) const;
	const MkDecoStr& GetDecoText(const MkHashStr& n0, const MkHashStr& n1, const MkHashStr& n2, const MkHashStr& key) const;
	const MkDecoStr& GetDecoText(const MkHashStr& n0, const MkHashStr& n1, const MkHashStr& n2, const MkHashStr& n3, const MkHashStr& key) const;
	const MkDecoStr& GetDecoText(const MkHashStr& n0, const MkHashStr& n1, const MkHashStr& n2, const MkHashStr& n3, const MkHashStr& n4, const MkHashStr& key) const;

	const MkDecoStr& GetDecoText(const MkArray<MkHashStr>& nodeNameAndKey, unsigned int offset = 0) const;

	// ����
	virtual void Clear(void);

	MkDecoTextNode();
	MkDecoTextNode(const MkHashStr& name) : MkSingleTypeTreePattern<MkDecoTextNode>(name) {}
	virtual ~MkDecoTextNode() { Clear(); }

protected:

	MkHashMap<MkHashStr, MkDecoStr> m_Table;
};
