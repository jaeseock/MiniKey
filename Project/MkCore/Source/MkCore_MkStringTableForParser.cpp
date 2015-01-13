
#include "MkCore_MkTagDefinitionForDataNode.h"
#include "MkCore_MkStringTableForParser.h"

//------------------------------------------------------------------------------------------------//

void MkStringTableForParser::BuildStringTable(MkStr& source)
{
	// �Ľ��� ���� ������ ���� ���� �߰�(10%)
	source.Reserve(source.GetSize() + source.GetSize() / 10 + 10);

	// �Ľ̿� ������ ���� �ʵ��� DQM(double quotation marks) Ż�⹮�� ġȯ
	source.ReplaceKeyword(L"\\\"", MkTagDefinitionForDataNode::TagForDQM);

	// DQM���� �ѷ����� ���ڿ����� �Ľ̿� ������ ���� �ʵ��� ġȯ�ϰ� ������ ���� string table�� ����
	m_StringTable.Clear();
	unsigned int currentPos = 0;
	unsigned int stringCount = 0;
	
	static const MkStr stPrefix = L"__#ST_";
	static const MkStr dqmTag = L"\"";
	MkStr currentKey = stPrefix;
	currentKey += stringCount;

	while (true)
	{
		MkStr stringBuffer;
		currentPos = source.ReplaceFirstBlock(currentPos, dqmTag, dqmTag, currentKey, stringBuffer);
		if (currentPos != MKDEF_ARRAY_ERROR)
		{
			// Ż�⹮�� �ݿ�
			if (!stringBuffer.Empty())
			{
				stringBuffer.ReplaceKeyword(MkTagDefinitionForDataNode::TagForDQM, dqmTag); // DQM
				stringBuffer.ReplaceTagtoCRLF();
			}

			// ��� �߰�
			m_StringTable.Create(currentKey, stringBuffer);

			++stringCount;
			currentKey = stPrefix;
			currentKey += stringCount;
		}
		else
			break;
	}

	m_StringTable.Rehash();
}

bool MkStringTableForParser::Exist(const MkHashStr& key)
{
	return m_StringTable.Exist(key);
}

void MkStringTableForParser::Clear(void)
{
	m_StringTable.Clear();
}

//------------------------------------------------------------------------------------------------//
