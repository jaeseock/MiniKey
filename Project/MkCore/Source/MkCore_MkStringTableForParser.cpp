
#include "MkCore_MkTagDefinitionForDataNode.h"
#include "MkCore_MkStringTableForParser.h"

//------------------------------------------------------------------------------------------------//

void MkStringTableForParser::BuildStringTable(MkStr& source)
{
	// 파싱중 변조 여유를 위한 버퍼 추가(10%)
	source.Reserve(source.GetSize() + source.GetSize() / 10 + 10);

	// 파싱에 영향을 주지 않도록 DQM(double quotation marks) 탈출문자 치환
	source.ReplaceKeyword(L"\\\"", MkTagDefinitionForDataNode::TagForDQM);

	// DQM으로 둘러싸인 문자열들이 파싱에 영향을 주지 않도록 치환하고 복구를 위해 string table을 구성
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
			// 탈출문자 반영
			if (!stringBuffer.Empty())
			{
				stringBuffer.ReplaceKeyword(MkTagDefinitionForDataNode::TagForDQM, dqmTag); // DQM
				stringBuffer.ReplaceTagtoCRLF();
			}

			// 페어 추가
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
