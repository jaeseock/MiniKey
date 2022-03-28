
#include "MkCore_MkCheck.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkInterfaceForDataWriting.h"
#include "MkCore_MkTagDefinitionForDataNode.h"
#include "MkCore_MkHelperForDataNodeConverter.h"
#include "MkCore_MkDataTextToMemoryConverter.h"

//------------------------------------------------------------------------------------------------//

bool MkDataTextToMemoryConverter::Convert(const MkPathName& filePath, MkByteArray& destination)
{
	MkStr textSource;
	MK_CHECK(textSource.ReadTextFile(filePath), L"������ ��� ������ ���� " + MkStr(filePath) + L" �ؽ�Ʈ ���� �б� ����")
		return false;

	return _Convert(textSource, filePath, destination);
}

bool MkDataTextToMemoryConverter::Convert(const MkByteArray& source, MkByteArray& destination)
{
	MkStr textSource;
	textSource.ReadTextStream(source, true, false); // UTF-8�� ����

	return _Convert(textSource, L"<TextData>", destination);
}

void MkDataTextToMemoryConverter::Clear(void)
{
	m_TargetFilePath.Clear();
	m_StringTable.Clear();
}

//------------------------------------------------------------------------------------------------//

bool MkDataTextToMemoryConverter::_Convert(MkStr& textSource, const MkPathName& filePath, MkByteArray& destination)
{
	m_TargetFilePath = filePath;

	m_StringTable.BuildStringTable(textSource);

	MkInterfaceForDataWriting dwInterface;
	MkHelperForDataNodeConverter::ReserveInterface(textSource, dwInterface);

	bool ok = _ParseText(textSource, dwInterface);
	if (ok)
	{
		dwInterface.Flush(destination);
	}
	Clear();
	return ok;
}

unsigned int MkDataTextToMemoryConverter::_SplitBlock
(const MkStr& source, unsigned int startPosition, int& blockType, MkStr& blockName, MkStr& templateName, MkStr& body) const
{
	unsigned int currentPosition = startPosition;

	// ��� Ÿ�� (Node/Template)
	MkStr typeKeyword;
	currentPosition = source.GetFirstWord(currentPosition, typeKeyword);
	if (currentPosition == MKDEF_ARRAY_ERROR) // Ű���� �±� �����Ƿ� ����
		return MKDEF_DNC_MSG_COMPLETE;

	if (typeKeyword == MkTagDefinitionForDataNode::TagForNode) // normal node?
	{
		blockType = MkTagDefinitionForDataNode::IndexMarkForNodeBegin;
	}
	else if (typeKeyword == MkTagDefinitionForDataNode::TagForTemplate) // template?
	{
		blockType = MkTagDefinitionForDataNode::IndexMarkForTemplateBegin;
	}
	else if (typeKeyword == MkTagDefinitionForDataNode::TagForPushingTemplate) // pushing template?
	{
		blockType = MkTagDefinitionForDataNode::IndexMarkForPushingTemplate;
	}
	else
	{
		ePrimitiveDataType unitType = MkPrimitiveDataType::GetEnum(typeKeyword); // or unit?
		MK_CHECK(unitType != ePDT_Undefined, m_TargetFilePath + L" ���Ͽ��� ������ �±�, " + typeKeyword + L" ���")
			return MKDEF_DNC_MSG_ERROR;
		
		blockType = static_cast<int>(unitType);
	}

	// ��� �̸���
	// ����� ���
	if ((blockType == MkTagDefinitionForDataNode::IndexMarkForNodeBegin) || (blockType == MkTagDefinitionForDataNode::IndexMarkForTemplateBegin))
	{
		MkStr nameKeyword;
		currentPosition = source.GetFirstBlock(currentPosition, MkTagDefinitionForDataNode::TagForBlockBegin, nameKeyword);
		MK_CHECK(currentPosition != MKDEF_ARRAY_ERROR, m_TargetFilePath + L" ���Ͽ� ��� ���� ��� ����")
			return MKDEF_DNC_MSG_ERROR;

		nameKeyword.RemoveBlank();
		MK_CHECK(!nameKeyword.Empty(), m_TargetFilePath + L" ���Ͽ� ��� �̸� ���� �κ��� ����")
			return MKDEF_DNC_MSG_ERROR;
		
		unsigned int applyTemplateKeywordPosition = nameKeyword.GetFirstKeywordPosition(MkTagDefinitionForDataNode::TagForApplyTemplate);

		// ���ø� ����
		if (applyTemplateKeywordPosition == MKDEF_ARRAY_ERROR)
		{
			blockName = m_StringTable.GetString(nameKeyword);
			templateName.Clear();
		}
		// ���ø� ���� Ű���� ����
		else
		{
			MkStr blockKey, templateKey;
			nameKeyword.GetSubStr(MkArraySection(0, applyTemplateKeywordPosition), blockKey); // ��� �̸�
			nameKeyword.GetSubStr(MkArraySection(applyTemplateKeywordPosition + MkTagDefinitionForDataNode::TagForApplyTemplate.GetSize()), templateKey); // ������ ���ø� �̸�
			blockName = m_StringTable.GetString(blockKey);
			templateName = m_StringTable.GetString(templateKey);

			MK_CHECK((!blockName.Empty()) && (!templateName.Empty()), m_TargetFilePath + L" ������ ��� �̸� ���� �κ� ����")
				return MKDEF_DNC_MSG_ERROR;
		}

		// ��ü
		unsigned int blockBeginPos = currentPosition - MkTagDefinitionForDataNode::TagForBlockBegin.GetSize();
		currentPosition = source.GetFirstStackBlock
			(blockBeginPos, MkTagDefinitionForDataNode::TagForBlockBegin, MkTagDefinitionForDataNode::TagForBlockEnd, body);

		MK_CHECK(currentPosition != MKDEF_ARRAY_ERROR, m_TargetFilePath + L" ������ " + blockName + L"����� �������� ����")
			return MKDEF_DNC_MSG_ERROR;
	}
	// ���ø� �о�ֱ�
	else if (blockType == MkTagDefinitionForDataNode::IndexMarkForPushingTemplate)
	{
		MkStr nameKeyword;
		currentPosition = source.GetFirstBlock(currentPosition, MkTagDefinitionForDataNode::TagForUnitEnd, nameKeyword);
		MK_CHECK(currentPosition != MKDEF_ARRAY_ERROR, m_TargetFilePath + L" ���Ͽ� �����ڰ� ���� ���ø� �о�ֱ� �±� ����")
			return MKDEF_DNC_MSG_ERROR;

		nameKeyword.RemoveBlank();
		MK_CHECK(!nameKeyword.Empty(), m_TargetFilePath + L" ���Ͽ� �̸��� ��� �ִ� ���ø� �о�ֱ� �±� ����")
			return MKDEF_DNC_MSG_ERROR;

		blockName = m_StringTable.GetString(nameKeyword);
	}
	// ����
	else
	{
		MkStr nameKeyword;
		currentPosition = source.GetFirstBlock(currentPosition, MkTagDefinitionForDataNode::TagForUnitAssign, nameKeyword);
		MK_CHECK(currentPosition != MKDEF_ARRAY_ERROR, m_TargetFilePath + L" ���Ͽ� �Ҵ��� ���� ���� ����")
			return MKDEF_DNC_MSG_ERROR;

		nameKeyword.RemoveBlank();
		MK_CHECK(!nameKeyword.Empty(), m_TargetFilePath + L" ���Ͽ� �̸��� ���� ���� ����")
			return MKDEF_DNC_MSG_ERROR;

		MkStr bodyBuffer;
		currentPosition = source.GetFirstBlock(currentPosition, MkTagDefinitionForDataNode::TagForUnitEnd, bodyBuffer);
		MK_CHECK(currentPosition != MKDEF_ARRAY_ERROR, m_TargetFilePath + L" ������ " + nameKeyword + L" ���� �����ڰ� ����")
			return MKDEF_DNC_MSG_ERROR;

		bodyBuffer.RemoveBlank();
		MK_CHECK(!bodyBuffer.Empty(), m_TargetFilePath + L" ������ " + nameKeyword + L" ���� ���� ����")
			return MKDEF_DNC_MSG_ERROR;

		blockName = nameKeyword;
		body = bodyBuffer;
	}
	
	return currentPosition;
}

bool MkDataTextToMemoryConverter::_ParseText(const MkStr& source, MkInterfaceForDataWriting& dwInterface) const
{
	unsigned int currentPosition = 0;
	while ((currentPosition != MKDEF_DNC_MSG_COMPLETE) && (currentPosition != MKDEF_DNC_MSG_ERROR))
	{
		currentPosition = _ParseBlock(source, dwInterface, currentPosition);
	}
	return (currentPosition == MKDEF_DNC_MSG_COMPLETE);
}

unsigned int MkDataTextToMemoryConverter::_ParseBlock(const MkStr& source, MkInterfaceForDataWriting& dwInterface, unsigned int startPosition) const
{
	// ��� ��� ������ �Ľ�
	int blockType;
	MkStr blockName, templateName, body;
	unsigned int currentPosition = _SplitBlock(source, startPosition, blockType, blockName, templateName, body);
	if ((currentPosition == MKDEF_DNC_MSG_COMPLETE) || (currentPosition == MKDEF_DNC_MSG_ERROR))
		return currentPosition;

	// ��� ��� ������ �޸𸮿� ���
	dwInterface.Write(blockType); // block type
	dwInterface.Write(blockName); // block name

	// ����� ��� ���ø� ���� ����� body �Ľ�
	if ((blockType == MkTagDefinitionForDataNode::IndexMarkForNodeBegin) || (blockType == MkTagDefinitionForDataNode::IndexMarkForTemplateBegin))
	{
		if (templateName.Empty())
		{
			dwInterface.Write(false); // normal node
		}
		else
		{
			dwInterface.Write(true); // template applied
			dwInterface.Write(templateName); // template name
		}

		if (!_ParseText(body, dwInterface)) // body
			return MKDEF_DNC_MSG_ERROR;

		dwInterface.Write(MkTagDefinitionForDataNode::IndexMarkForNodeBlockEnd); // end of node
	}
	// ���ø� �о�ֱ��� ��� ������ �̹� ������ ��������Ƿ� �Ѿ
	else if (blockType == MkTagDefinitionForDataNode::IndexMarkForPushingTemplate)
	{
		// do nothing
	}
	// ������ ��� �� ������ ���
	else
	{
		ePrimitiveDataType typeEnum = static_cast<ePrimitiveDataType>(blockType);
		MkArray<MkStr> tokens;
		unsigned int valueCount = body.Tokenize(tokens, MkTagDefinitionForDataNode::TagForArrayDivider);

		// automation?
		if ((typeEnum == ePDT_Str) && (valueCount >= 4) && ((valueCount % 2) == 0) &&
			(m_StringTable.GetString(tokens[0]) == MkTagDefinitionForDataNode::StringArrayAutomationKeyword))
		{
			unsigned int indexPartSize = valueCount - 2;
			int totalArrayCount = 0;
			MkArray<int> indexTable(indexPartSize);
			for (unsigned int i=2; i<valueCount; i+=2)
			{
				const MkStr& firstStr = m_StringTable.GetString(tokens[i]);
				const MkStr& lastStr = m_StringTable.GetString(tokens[i+1]);
				if (firstStr.IsDigit() && lastStr.IsDigit())
				{
					int firstIndex = firstStr.ToInteger();
					int lastIndex = lastStr.ToInteger();
					
					totalArrayCount += abs(lastIndex - firstIndex) + 1;
					
					indexTable.PushBack(firstIndex);
					indexTable.PushBack(lastIndex);
				}
				else
					break;
			}
			MK_CHECK(indexTable.GetSize() == indexPartSize, L"���� �迭 �ڵ��ϼ� ���� ���� : index ��Ʈ")
				return currentPosition;

			dwInterface.Write(totalArrayCount);

			const MkStr& prefix = m_StringTable.GetString(tokens[1]);
			for (unsigned int i=0; i<indexPartSize; i+=2)
			{
				int firstIndex = indexTable[i];
				int lastIndex = indexTable[i+1];

				if (firstIndex <= lastIndex)
				{
					for (int j=firstIndex; j<=lastIndex; ++j)
					{
						MkHelperForDataNodeConverter::WriteUnitString(typeEnum, prefix + MkStr(j), dwInterface);
					}
				}
				else
				{
					for (int j=firstIndex; j>=lastIndex; --j)
					{
						MkHelperForDataNodeConverter::WriteUnitString(typeEnum, prefix + MkStr(j), dwInterface);
					}
				}
			}
		}
		else
		{
			dwInterface.Write(valueCount);

			MK_INDEXING_LOOP(tokens, i)
			{
				MkHelperForDataNodeConverter::WriteUnitString(typeEnum, (typeEnum == ePDT_Str) ? m_StringTable.GetString(tokens[i]) : tokens[i], dwInterface);
			}
		}
	}
	
	return currentPosition;
}

//------------------------------------------------------------------------------------------------//
