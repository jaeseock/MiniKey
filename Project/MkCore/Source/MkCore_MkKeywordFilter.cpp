
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
	bool hasKeyword = false; // keyword ���Կ���

	// �ǹ� �ִ� ���ڿ��ϰ�� ����
	unsigned int cnt = src.GetSize();
	if (cnt > 0)
	{
		// ġȯ�� ���ڿ��� ���� �ӽ� ����
		MkArray<wchar_t> charBuffer((slang) ? cnt : (cnt * 4));

		// ��� node
		const CNode& targetNode = (slang) ? m_SlangRootNode : m_TagRootNode;

		// �˻� ����
		unsigned int beginPos = 0;
		while (beginPos < cnt)
		{
			// ���ڿ��� beginPos���� ������ �˻��� keyword ũ�� ��ȯ
			unsigned int countOfKeyword = targetNode.FindKeyword(src, beginPos);

			// keyword ����
			if (countOfKeyword == 0)
			{
				// �ش� ����(src[beginPos])�κ��� ���۵Ǵ� ���ڿ��� �����ϹǷ� �ش� ���� ����
				charBuffer.PushBack(src[beginPos]);
				
				// ���� ��ġ���� ��� Ž��
				++beginPos;
			}
			// keyword ����!!!
			else
			{
				// keyword ���ڼ���ŭ ��ü��� ġȯ
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
					// keyword ����
					MkStr keyBuf;
					src.GetSubStr(MkArraySection(beginPos, countOfKeyword), keyBuf);
					MkHashStr keyword = keyBuf;

					// table�� �����ϸ� tag ���ڿ��� ġȯ
					if (m_TagMap.Exist(keyword))
					{
						const MkStr& tag = m_TagMap[keyword];
						MK_INDEXING_LOOP(tag, i)
						{
							charBuffer.PushBack(tag[i]);
						}

						hasKeyword = true;
					}
					// table�� ������ ��ҹ��ڰ� �ٸ� ����̹Ƿ� ���� ����(skip)
					else
					{
						for (unsigned int i=0; i<countOfKeyword; ++i)
						{
							charBuffer.PushBack(src[beginPos + i]);
						}
					}
				}

				// �ǳʶپ� ��� Ž��
				beginPos += countOfKeyword;
			}
		}

		// ��ȯ ���ۿ� ��ȯ�� ���ڿ��� ����
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

	// �����ڸ� �ҹ��ڷ� ��ȯ
	c += ((c >= L'A') && (c <= L'Z')) ? 32 : 0;

	// �ڽ� ���� �� c�� key�� ���� ��尡 �����ϸ� �׳� �װ� ���� ���� �����ϰ�
	// ������ c�� key�� �� ��带 ������ ���� ���� ����
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

	// eok�� true�ε�(keyword�� ��) ���� ���(c�� key�� ���� ���)�� eok�� false�� true�� ����
	// ex>
	// "fucking" ��� �� "fuck"�� ����� �� 'k' ����� eok�� false->true�� �ٲ�
	// ���� ������ �����ϸ� "fuck" ��� �� "fucking"�� ����� �� 'k' ����� eok�� true->false�� �ٲ�
	// eok�� �� �� true�� �Ǹ� false�� �Ǹ� �� ��
	if (eok && (!nextNode->m_EOK))
	{
		nextNode->m_EOK = true;
	}

	return nextNode;
}

void MkKeywordFilter::CNode::RegisterKeyword(const MkStr& keyword)
{
	// ���ǹ��� ���ڿ��̸�
	unsigned int cnt = keyword.GetSize();
	if (cnt > 0)
	{
		unsigned int lastIndex = cnt - 1; // ������ ���� �ε���
		CNode* nextNode = this; // ���� ������ ����

		// ���ڿ� ����
		for (unsigned int i=0; i<cnt; ++i)
		{
			// �� ���ڰ� eok���� �˻�(ex> "fuck"�� 'k')
			bool eok = (i == lastIndex);

			// ���ڸ� ����ϰ� ��ȯ�� ��带 ���� ���۳��� ����
			nextNode = nextNode->RegisterCharacter(keyword, i, eok);
		}
	}
}

const MkKeywordFilter::CNode* MkKeywordFilter::CNode::CheckCharacter(const MkStr& src, unsigned int index) const
{
	wchar_t c = src[index];

	// �����ڸ� �ҹ��ڷ� ��ȯ
	c += ((c >= L'A') && (c <= L'Z')) ? 32 : 0;

	// �� ����� �ڽĵ� �� c ��尡 ������ keyword�� ���ٰ� �Ǵ��ϰ� NULL ��ȯ,
	// c�� key�� ���� ��尡 �����ϸ� keyword�� ���ɼ��� �����Ƿ� �ش� ��� ��ȯ
	return m_Children.Exist(c) ? m_Children[c] : NULL;
}

unsigned int MkKeywordFilter::CNode::FindKeyword(const MkStr& src, unsigned int beginPos) const
{
	unsigned int cnt = src.GetSize();
	const CNode* nextNode = this; // ���� ������ ����
	unsigned int countOfKeyword = 0; // ����� keyword ���� ��

	// beginPos���� ���ڿ��� ������ Ž��
	for (unsigned int i=beginPos; i<cnt; ++i)
	{
		// �ش� ���ڸ� key�� ���� ��尡 �ִ��� Ž��
		nextNode = nextNode->CheckCharacter(src, i);

		// keyword ���ɼ��� �ִ� ��尡 Ž����
		if (nextNode != NULL)
		{
			// keyword Ȯ��!!! (ex> "fuck"�� 'k')
			if (nextNode->m_EOK)
			{
				// keyword ũ�� ���� �� ��� Ž��
				// ��� Ž���ؾ� �ϴ� ������ ���� ����� keyword�� �����ϴ� �� ū keyword��
				// ������ �� �ֱ� ����
				// ex>
				// "fuck", "fucking"�� keyword�� ��� "fuck"�� ����Ǿ��ٰ� Ż���ع����� "****ing"�� �ǹ���
				// ���� ������ Ž���� "fucking"�� �����ؾ� ��
				countOfKeyword = i + 1 - beginPos;
			}

			// keyword ���ɼ��� ������ ���� Ȯ���� �ȵ� ����(ex> "fuck"�� 'f', 'u', 'c')
			// ��� Ž��
		}
		// NULL�� ��ȯ�Ǿ����� ���̻� keyword�� ���ٴ� �ǹ�. ���� Ż��
		else
			break;
	}

	return countOfKeyword;
}

//------------------------------------------------------------------------------------------------//
