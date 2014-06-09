#pragma once


//------------------------------------------------------------------------------------------------//
// deco text table
// 원칙적으로 빈 문자열은 등록될 수 없음(무시됨)
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingleTypeTreePattern.h"
#include "MkS2D_MkDecoStr.h"


class MkPathName;
class MkDataNode;

class MkDecoTextNode : public MkSingleTypeTreePattern<MkDecoTextNode>
{
public:

	// 파일로 초기화
	bool SetUp(const MkPathName& filePath);

	// 노드로 초기화
	void SetUp(const MkDataNode& node);

	// deco string 반환
	// 존재하지 않는 node나 key라면 MkDecoStr::Null 반환
	//
	// 원칙대로라면 GetChildNode() 호출 반복으로 안전하게 탐색해 나가야 하지만 편의성 측면에서
	// 한 번의 호출로 탐색을 완료할 수 있도록 함. 필요하면 더 추가해 사용
	// ex>
	//	MkDecoStr buffer;
	//	MkDecoTextNode* targetNode = beginNode->GetChildNode(L"character");
	//	if (targetNode != NULL)
	//		targetNode = targetNode->GetChildNode(L"status");
	//	if (targetNode != NULL)
	//		buffer = targetNode->GetDecoText(L"strength desc");
	//
	// 위 코드는 아래 코드와 동일
	//	const MkDecoStr& buffer = beginNode->GetDecoText(L"character", L"status", L"strength desc");
	//
	// 혹은 아래 코드와 동일
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

	// 해제
	virtual void Clear(void);

	MkDecoTextNode();
	MkDecoTextNode(const MkHashStr& name) : MkSingleTypeTreePattern<MkDecoTextNode>(name) {}
	virtual ~MkDecoTextNode() { Clear(); }

protected:

	MkHashMap<MkHashStr, MkDecoStr> m_Table;
};
