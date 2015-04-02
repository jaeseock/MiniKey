
#include "MkCore_MkProjectDefinition.h"
#include "MkCore_MkCheck.h"
#include "MkCore_MkKeywordFilter.h"


//------------------------------------------------------------------------------------------------//

void MkKeywordFilter::RegisterSlang(const MkStr& keyword)
{
	m_SlangRootNode.RegisterKeyword(keyword);
}

bool MkKeywordFilter::CheckSlang(const MkStr& src) const
{
	return (m_SlangRootNode.FindKeyword(src, 0) > 0);
}

bool MkKeywordFilter::CheckSlang(const MkStr& src, MkStr& dest) const
{
	return _CheckString(src, dest, true);
}

void MkKeywordFilter::SetTag(const MkHashStr& keyword, const MkStr& tag)
{
	if (m_TagMap.Exist(keyword))
	{
		m_TagMap[keyword] = tag;
	}
	else
	{
		m_TagRootNode.RegisterKeyword(keyword.GetString());
		m_TagMap.Create(keyword, tag);
	}
}

void MkKeywordFilter::CheckTag(const MkStr& src, MkStr& dest) const
{
	_CheckString(src, dest, false);
}

MkKeywordFilter::~MkKeywordFilter()
{
	m_SlangRootNode.Clear();
	m_TagRootNode.Clear();
	m_TagMap.Clear();
}

bool MkKeywordFilter::_CheckString(const MkStr& src, MkStr& dest, bool slang) const
{
	bool hasKeyword = false; // keyword 포함여부

	// 의미 있는 문자열일경우 진행
	unsigned int cnt = src.GetSize();
	if (cnt > 0)
	{
		// 치환된 문자열을 담을 임시 버퍼
		MkArray<wchar_t> charBuffer((slang) ? cnt : (cnt * 4));

		// 대상 node
		const CNode& targetNode = (slang) ? m_SlangRootNode : m_TagRootNode;

		// 검사 시작
		unsigned int beginPos = 0;
		while (beginPos < cnt)
		{
			// 문자열의 beginPos부터 끝까지 검사해 keyword 크기 반환
			unsigned int countOfKeyword = targetNode.FindKeyword(src, beginPos);

			// keyword 없음
			if (countOfKeyword == 0)
			{
				// 해당 문자(src[beginPos])로부터 시작되는 문자열은 깨끗하므로 해당 글자 저장
				charBuffer.PushBack(src[beginPos]);
				
				// 다음 위치부터 계속 탐색
				++beginPos;
			}
			// keyword 존재!!!
			else
			{
				// keyword 글자수만큼 대체어로 치환
				if (slang)
				{
					for (unsigned int i=0; i<countOfKeyword; ++i)
					{
						charBuffer.PushBack(MKDEF_SLANG_REPLACEMENT_TAG);
					}

					hasKeyword = true;
				}
				// tag
				else
				{
					// keyword 추출
					MkStr keyBuf;
					src.GetSubStr(MkArraySection(beginPos, countOfKeyword), keyBuf);
					MkHashStr keyword = keyBuf;

					// table에 존재하면 tag 문자열로 치환
					if (m_TagMap.Exist(keyword))
					{
						const MkStr& tag = m_TagMap[keyword];
						MK_INDEXING_LOOP(tag, i)
						{
							charBuffer.PushBack(tag[i]);
						}

						hasKeyword = true;
					}
					// table에 없으면 대소문자가 다른 경우이므로 원본 복사(skip)
					else
					{
						for (unsigned int i=0; i<countOfKeyword; ++i)
						{
							charBuffer.PushBack(src[beginPos + i]);
						}
					}
				}

				// 건너뛰어 계속 탐색
				beginPos += countOfKeyword;
			}
		}

		// 반환 버퍼에 변환된 문자열을 저장
		dest.m_Str.PopBack(1);
		dest.m_Str.PushBack(charBuffer);
		dest.m_Str.PushBack(NULL);
	}

	return hasKeyword;
}

//------------------------------------------------------------------------------------------------//

void MkKeywordFilter::CNode::Clear(void)
{
	MkHashMapLooper<wchar_t, CNode*> looper(m_Children);
	MK_STL_LOOP(looper)
	{
		delete looper.GetCurrentField();
	}
	m_Children.Clear();
}

MkKeywordFilter::CNode* MkKeywordFilter::CNode::RegisterCharacter(const MkStr& keyword, unsigned int index, bool eok)
{
	wchar_t c = keyword[index];

	// 영문자면 소문자로 변환
	c += ((c >= L'A') && (c <= L'Z')) ? 32 : 0;

	// 자식 노드들 중 c를 key로 가진 노드가 존재하면 그냥 그걸 다음 노드로 설정하고
	// 없으면 c를 key로 새 노드를 생성해 다음 노드로 설정
	CNode* nextNode = NULL;
	if (m_Children.Exist(c))
	{
		nextNode = m_Children[c];
	}
	else
	{
		nextNode = new CNode;
		m_Children.Create(c, nextNode);
	}

	// eok가 true인데(keyword의 끝) 다음 노드(c를 key로 가진 노드)의 eok가 false면 true로 설정
	// ex>
	// "fucking" 등록 후 "fuck"을 등록할 때 'k' 노드의 eok가 false->true로 바뀜
	// 만약 무조건 세팅하면 "fuck" 등록 후 "fucking"을 등록할 때 'k' 노드의 eok가 true->false로 바뀜
	// eok는 한 번 true가 되면 false가 되면 안 됨
	if (eok && (!nextNode->m_EOK))
	{
		nextNode->m_EOK = true;
	}

	return nextNode;
}

void MkKeywordFilter::CNode::RegisterKeyword(const MkStr& keyword)
{
	// 유의미한 문자열이면
	unsigned int cnt = keyword.GetSize();
	if (cnt > 0)
	{
		unsigned int lastIndex = cnt - 1; // 마지막 글자 인덱스
		CNode* nextNode = this; // 현재 노드부터 시작

		// 문자열 루핑
		for (unsigned int i=0; i<cnt; ++i)
		{
			// 현 문자가 eok인지 검사(ex> "fuck"의 'k')
			bool eok = (i == lastIndex);

			// 문자를 등록하고 반환된 노드를 다음 시작노드로 세팅
			nextNode = nextNode->RegisterCharacter(keyword, i, eok);
		}
	}
}

const MkKeywordFilter::CNode* MkKeywordFilter::CNode::CheckCharacter(const MkStr& src, unsigned int index) const
{
	wchar_t c = src[index];

	// 영문자면 소문자로 변환
	c += ((c >= L'A') && (c <= L'Z')) ? 32 : 0;

	// 현 노드의 자식들 중 c 노드가 없으면 keyword가 없다고 판단하고 NULL 반환,
	// c를 key로 가진 노드가 존재하면 keyword일 가능성이 있으므로 해당 노드 반환
	return m_Children.Exist(c) ? m_Children[c] : NULL;
}

unsigned int MkKeywordFilter::CNode::FindKeyword(const MkStr& src, unsigned int beginPos) const
{
	unsigned int cnt = src.GetSize();
	const CNode* nextNode = this; // 현재 노드부터 시작
	unsigned int countOfKeyword = 0; // 검출된 keyword 글자 수

	// beginPos부터 문자열의 끝까지 탐색
	for (unsigned int i=beginPos; i<cnt; ++i)
	{
		// 해당 문자를 key로 가진 노드가 있는지 탐색
		nextNode = nextNode->CheckCharacter(src, i);

		// keyword 가능성이 있는 노드가 탐색됨
		if (nextNode != NULL)
		{
			// keyword 확실!!! (ex> "fuck"의 'k')
			if (nextNode->m_EOK)
			{
				// keyword 크기 저장 후 계속 탐색
				// 계속 탐색해야 하는 이유는 현재 검출된 keyword를 포함하는 더 큰 keyword가
				// 존재할 수 있기 때문
				// ex>
				// "fuck", "fucking"이 keyword일 경우 "fuck"이 검출되었다고 탈출해버리면 "****ing"가 되버림
				// 따라서 끝까지 탐색해 "fucking"을 검출해야 함
				countOfKeyword = i + 1 - beginPos;
			}

			// keyword 가능성은 있지만 아직 확정은 안된 상태(ex> "fuck"의 'f', 'u', 'c')
			// 계속 탐색
		}
		// NULL이 반환되었으면 더이상 keyword는 없다는 의미. 루프 탈출
		else
			break;
	}

	return countOfKeyword;
}

//------------------------------------------------------------------------------------------------//
