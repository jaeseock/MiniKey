
#include "MkCore_MkProjectDefinition.h"
#include "MkCore_MkCheck.h"
#include "MkCore_MkSlangFilter.h"


//------------------------------------------------------------------------------------------------//

void MkSlangFilter::RegisterKeyword(const MkStr& keyword)
{
	MkStr buffer = keyword;
	buffer.ToLower(); // 영문자의 경우 소문자로 변환
	m_RootNode.RegisterKeyword(buffer);
}

bool MkSlangFilter::CheckString(const MkStr& src) const
{
	MkStr buffer = src;
	buffer.ToLower(); // 영문자의 경우 소문자로 변환
	return (m_RootNode.FindSlang(buffer, 0) > 0);
}

bool MkSlangFilter::CheckString(const MkStr& src, MkStr& dest) const
{
	MkStr msg = src;
	MkStr buffer;
	MkArray<bool> converting;
	_ConvertTextToLower(msg, converting); // 영문자의 경우 소문자로 변환
	bool ok = m_RootNode.CheckString(msg, buffer);
	if (ok)
	{
		_ConvertTextToOriginal(buffer, converting); // 변환된 소문자를 복구
		dest = buffer;
	}
	else
	{
		dest = src;
	}
	return ok;
}

//------------------------------------------------------------------------------------------------//

void MkSlangFilter::CNode::Clear(void)
{
	MkHashMapLooper<wchar_t, CNode*> looper(m_Children);
	MK_STL_LOOP(looper)
	{
		delete looper.GetCurrentField();
	}
	m_Children.Clear();
}

MkSlangFilter::CNode* MkSlangFilter::CNode::RegisterCharacter(const MkStr& keyword, unsigned int index, bool eok)
{
	const wchar_t& c = keyword[index];

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

	// eok가 true인데(비속어의 끝) 다음 노드(c를 key로 가진 노드)의 eok가 false면 true로 설정
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

void MkSlangFilter::CNode::RegisterKeyword(const MkStr& keyword)
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

const MkSlangFilter::CNode* MkSlangFilter::CNode::CheckCharacter(const MkStr& keyword, unsigned int index) const
{
	const wchar_t& c = keyword[index];

	// 현 노드의 자식들 중 c 노드가 없으면 비속어가 없다고 판단하고 NULL 반환하고
	// c를 key로 가진 노드가 존재하면 비속어 가능성 있으므로 해당 노드 반환
	return m_Children.Exist(c) ? m_Children[c] : NULL;
}

unsigned int MkSlangFilter::CNode::FindSlang(const MkStr& keyword, unsigned int beginPos) const
{
	unsigned int cnt = keyword.GetSize();
	const CNode* nextNode = this; // 현재 노드부터 시작
	unsigned int countOfSlang = 0; // 검출된 비속어 글자 수

	// beginPos부터 문자열의 끝까지 탐색
	for (unsigned int i=beginPos; i<cnt; ++i)
	{
		// 해당 문자를 key로 가진 노드가 있는지 탐색
		nextNode = nextNode->CheckCharacter(keyword, i);

		// 비속어 가능성이 있는 노드가 탐색됨
		if (nextNode != NULL)
		{
			// 비속어 확실!!! (ex> "fuck"의 'k')
			if (nextNode->m_EOK)
			{
				// 비속어 크기 저장 후 계속 탐색
				// 계속 탐색해야 하는 이유는 현재 검출된 비속어를 포함하는 더 큰 비속어가
				// 존재할 수 있기 때문
				// ex>
				// "fuck", "fucking"이 비속어일 경우 "fuck"이 검출되었다고 탈출해버리면 "****ing"가 되버림
				// 따라서 끝까지 탐색해 "fucking"을 검출해야 함
				countOfSlang = i + 1 - beginPos;
			}
			// 비속어 가능성은 있지만 아직 확정은 안된 상태(ex> "fuck"의 'f', 'u', 'c')
			// 계속 탐색
			else {}
		}
		// NULL이 반환되었으면 더이상 비속어는 없다는 의미. 루프 탈출
		else
			break;
	}

	return countOfSlang;
}

bool MkSlangFilter::CNode::CheckString(const MkStr& src, MkStr& dest) const
{
	bool hasSlang = false; // 비속어 포함여부

	// 의미 있는 문자열일경우 진행
	unsigned int cnt = src.GetSize();
	if (cnt > 0)
	{
		// 치환된 문자열을 담을 임시 버퍼
		MkStr finalString;
		finalString.Reserve(cnt);

		unsigned int beginPos = 0; // 문자열 검사 시작위치
		while (beginPos < cnt)
		{
			// 문자열의 beginPos부터 끝까지 검사해 비속어 크기 반환
			unsigned int countOfSlang = FindSlang(src, beginPos);

			// 비속어 없음
			if (countOfSlang == 0)
			{
				// 해당 문자(src[beginPos])로부터 시작되는 문자열은 깨끗하므로 해당 글자 저장
				finalString += src[beginPos];
				
				// 다음 위치부터 계속 탐색
				++beginPos;
			}
			// 비속어 존재!!!
			else
			{
				// 비속어 글자수만큼 루핑
				for (unsigned int i=0; i<countOfSlang; ++i)
				{
					// 비속어 문자를 '*'로 치환
					finalString += MKDEF_SLANG_REPLACEMENT_TAG;
				}

				// 비속어 크기만큼 건너뛰어 계속 탐색
				beginPos += countOfSlang;

				// 딱 걸렸음
				hasSlang = true;
			}
		}

		// 반환 버퍼에 변환된 문자열을 저장
		dest = finalString;
	}

	return hasSlang;
}

void MkSlangFilter::_ConvertTextToLower(MkStr& msg, MkArray<bool>& converting) const
{
	if (!msg.Empty())
	{
		converting.Reserve(msg.GetSize());

		MK_INDEXING_LOOP(msg, i)
		{
			wchar_t& c = msg[i];
			if ((c >= L'A') && (c <= L'Z'))
			{
				c += 32;
				converting.PushBack(true);
			}
			else
			{
				converting.PushBack(false);
			}
		}
	}
}

void MkSlangFilter::_ConvertTextToOriginal(MkStr& msg, const MkArray<bool>& converting) const
{
	if (!converting.Empty())
	{
		MK_INDEXING_LOOP(converting, i)
		{
			wchar_t& c = msg[i];
			if (converting[i] && (c != MKDEF_SLANG_REPLACEMENT_TAG))
			{
				c -= 32;
			}
		}
	}
}
//------------------------------------------------------------------------------------------------//
