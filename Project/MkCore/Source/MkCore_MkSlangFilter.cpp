
#include "MkCore_MkProjectDefinition.h"
#include "MkCore_MkCheck.h"
#include "MkCore_MkSlangFilter.h"


//------------------------------------------------------------------------------------------------//

void MkSlangFilter::RegisterKeyword(const MkStr& keyword)
{
	MkStr buffer = keyword;
	buffer.ToLower(); // �������� ��� �ҹ��ڷ� ��ȯ
	m_RootNode.RegisterKeyword(buffer);
}

bool MkSlangFilter::CheckString(const MkStr& src) const
{
	MkStr buffer = src;
	buffer.ToLower(); // �������� ��� �ҹ��ڷ� ��ȯ
	return (m_RootNode.FindSlang(buffer, 0) > 0);
}

bool MkSlangFilter::CheckString(const MkStr& src, MkStr& dest) const
{
	MkStr msg = src;
	MkStr buffer;
	MkArray<bool> converting;
	_ConvertTextToLower(msg, converting); // �������� ��� �ҹ��ڷ� ��ȯ
	bool ok = m_RootNode.CheckString(msg, buffer);
	if (ok)
	{
		_ConvertTextToOriginal(buffer, converting); // ��ȯ�� �ҹ��ڸ� ����
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

	// eok�� true�ε�(��Ӿ��� ��) ���� ���(c�� key�� ���� ���)�� eok�� false�� true�� ����
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

void MkSlangFilter::CNode::RegisterKeyword(const MkStr& keyword)
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

const MkSlangFilter::CNode* MkSlangFilter::CNode::CheckCharacter(const MkStr& keyword, unsigned int index) const
{
	const wchar_t& c = keyword[index];

	// �� ����� �ڽĵ� �� c ��尡 ������ ��Ӿ ���ٰ� �Ǵ��ϰ� NULL ��ȯ�ϰ�
	// c�� key�� ���� ��尡 �����ϸ� ��Ӿ� ���ɼ� �����Ƿ� �ش� ��� ��ȯ
	return m_Children.Exist(c) ? m_Children[c] : NULL;
}

unsigned int MkSlangFilter::CNode::FindSlang(const MkStr& keyword, unsigned int beginPos) const
{
	unsigned int cnt = keyword.GetSize();
	const CNode* nextNode = this; // ���� ������ ����
	unsigned int countOfSlang = 0; // ����� ��Ӿ� ���� ��

	// beginPos���� ���ڿ��� ������ Ž��
	for (unsigned int i=beginPos; i<cnt; ++i)
	{
		// �ش� ���ڸ� key�� ���� ��尡 �ִ��� Ž��
		nextNode = nextNode->CheckCharacter(keyword, i);

		// ��Ӿ� ���ɼ��� �ִ� ��尡 Ž����
		if (nextNode != NULL)
		{
			// ��Ӿ� Ȯ��!!! (ex> "fuck"�� 'k')
			if (nextNode->m_EOK)
			{
				// ��Ӿ� ũ�� ���� �� ��� Ž��
				// ��� Ž���ؾ� �ϴ� ������ ���� ����� ��Ӿ �����ϴ� �� ū ��Ӿ
				// ������ �� �ֱ� ����
				// ex>
				// "fuck", "fucking"�� ��Ӿ��� ��� "fuck"�� ����Ǿ��ٰ� Ż���ع����� "****ing"�� �ǹ���
				// ���� ������ Ž���� "fucking"�� �����ؾ� ��
				countOfSlang = i + 1 - beginPos;
			}
			// ��Ӿ� ���ɼ��� ������ ���� Ȯ���� �ȵ� ����(ex> "fuck"�� 'f', 'u', 'c')
			// ��� Ž��
			else {}
		}
		// NULL�� ��ȯ�Ǿ����� ���̻� ��Ӿ�� ���ٴ� �ǹ�. ���� Ż��
		else
			break;
	}

	return countOfSlang;
}

bool MkSlangFilter::CNode::CheckString(const MkStr& src, MkStr& dest) const
{
	bool hasSlang = false; // ��Ӿ� ���Կ���

	// �ǹ� �ִ� ���ڿ��ϰ�� ����
	unsigned int cnt = src.GetSize();
	if (cnt > 0)
	{
		// ġȯ�� ���ڿ��� ���� �ӽ� ����
		MkStr finalString;
		finalString.Reserve(cnt);

		unsigned int beginPos = 0; // ���ڿ� �˻� ������ġ
		while (beginPos < cnt)
		{
			// ���ڿ��� beginPos���� ������ �˻��� ��Ӿ� ũ�� ��ȯ
			unsigned int countOfSlang = FindSlang(src, beginPos);

			// ��Ӿ� ����
			if (countOfSlang == 0)
			{
				// �ش� ����(src[beginPos])�κ��� ���۵Ǵ� ���ڿ��� �����ϹǷ� �ش� ���� ����
				finalString += src[beginPos];
				
				// ���� ��ġ���� ��� Ž��
				++beginPos;
			}
			// ��Ӿ� ����!!!
			else
			{
				// ��Ӿ� ���ڼ���ŭ ����
				for (unsigned int i=0; i<countOfSlang; ++i)
				{
					// ��Ӿ� ���ڸ� '*'�� ġȯ
					finalString += MKDEF_SLANG_REPLACEMENT_TAG;
				}

				// ��Ӿ� ũ�⸸ŭ �ǳʶپ� ��� Ž��
				beginPos += countOfSlang;

				// �� �ɷ���
				hasSlang = true;
			}
		}

		// ��ȯ ���ۿ� ��ȯ�� ���ڿ��� ����
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
