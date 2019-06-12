
#include "MkCore_MkCheck.h"
#include "MkCore_MkInterfaceForDataWriting.h"
#include "MkCore_MkTagDefinitionForDataNode.h"
#include "MkCore_MkHelperForDataNodeConverter.h"
#include "MkCore_MkExcelFileInterface.h"
#include "MkCore_MkExcelToMemoryConverter.h"


//------------------------------------------------------------------------------------------------//

bool MkExcelToMemoryConverter::Convert(const MkPathName& filePath, MkByteArray& destination)
{
	m_TargetFilePath = filePath;

	MkExcelFileInterface excelReader;
	MK_CHECK(excelReader.SetUp(filePath), L"������ ��� ������ ���� " + MkStr(filePath) + L" ���� ���� ���� ����")
		return false;

	MK_CHECK(excelReader.SheetExist(MKDEF_EXCEL_SHEET_ROOT_NAME), MkStr(filePath) + L" ���� ���Ͽ� " + MkStr(MKDEF_EXCEL_SHEET_ROOT_NAME) + L" ��Ʈ�� �������� ����")
		return false;

	bool ok = _BuildSheetData(excelReader);
	excelReader.Clear();
	if (ok)
	{
		MkInterfaceForDataWriting dwInterface;
		_ReserveInterface(dwInterface);
		ok = _ConvertSheetToBinary(MKDEF_EXCEL_SHEET_ROOT_NAME, dwInterface);
		if (ok)
		{
			dwInterface.Flush(destination);
		}
	}
	Clear();
	return ok;
}

void MkExcelToMemoryConverter::Clear(void)
{
	m_TargetFilePath.Clear();
	m_TargetSheetName.Clear();
	m_SheetNameList.Clear();
	m_SheetNameList.Clear();
}

//------------------------------------------------------------------------------------------------//

bool MkExcelToMemoryConverter::_BuildSheetData(MkExcelFileInterface& reader)
{
	unsigned int sheetCount = reader.GetSheetSize();
	m_SheetNameList.Reserve(sheetCount);
	m_SheetDataList.Reserve(sheetCount);

	MkArray<MkStr> typeFilter;
	typeFilter.Reserve(2);
	typeFilter.PushBack(MKDEF_EXCEL_SHEET_SINGLE_TYPE);
	typeFilter.PushBack(MKDEF_EXCEL_SHEET_TABLE_TYPE);

	MkArray<MkStr> anchorFilter;
	typeFilter.Reserve(2);
	typeFilter.PushBack(MKDEF_EXCEL_SHEET_VERTICAL_ANCHOR);
	typeFilter.PushBack(MKDEF_EXCEL_SHEET_HORIZONTAL_ANCHOR);

	MkArray<MkStr> emptyFilter;

	// sheet ��ȸ
	for (unsigned int i=0; i<sheetCount; ++i)
	{
		MK_CHECK(reader.SetActiveSheet(i), m_TargetFilePath + L" ������ " + MkStr(i) + L"��° sheet�� ������ ����")
			return false;

		MkStr sheetName = reader.GetSheetName(i);
		MK_CHECK(!sheetName.Empty(), m_TargetFilePath + L" ������ " + MkStr(i) + L"��° sheet �̸��� �����")
			return false;

		if (!sheetName.CheckPrefix(MKDEF_EXCEL_SHEET_COMMENT_TAG)) // �ּ� ��Ʈ�� �ƴϸ� ����
		{
			m_TargetSheetName = sheetName;

			// �ʼ� ���� ���� �Ľ� �� ����ó��
			// type
			MkStr typeStr;
			if (!_GetBaseValue(reader, MKDEF_EXCEL_SHEET_TYPE_POS, typeFilter, typeStr))
				return false;

			// anchor
			MkStr anchorStr;
			if (!_GetBaseValue(reader, MKDEF_EXCEL_SHEET_ANCHOR_POS, anchorFilter, anchorStr))
				return false;

			// start position
			MkStr startPositionStr;
			if (!_GetBaseValue(reader, MKDEF_EXCEL_SHEET_STARTPOSITION_POS, emptyFilter, startPositionStr))
				return false;

			m_SheetNameList.PushBack(sheetName);
			SheetData& sheetData = m_SheetDataList.PushBack();

			// ������ġ�� �����ϸ� ���� �Ľ� ����
			if (!startPositionStr.Empty())
			{
				if (!_ParseTable(reader, typeStr, anchorStr, startPositionStr, sheetData))
					return false;
			}
		}
	}
	return true;
}

// �Է°� ���ܰ˻� ���� ����
bool MkExcelToMemoryConverter::_GetBaseValue
(MkExcelFileInterface& reader, const MkStr& position, const MkArray<MkStr>& availableList, MkStr& result) const
{
	MkStr buffer;
	MK_CHECK(reader.GetData(position, buffer), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + position + L"��ġ���� �� �б� ����")
		return false;

	if (!availableList.Empty()) // ���Ͱ� �����ϸ�
	{
		if (buffer.Empty())
		{
			buffer = availableList[0]; // ����� ��� ù��° ���Ͱ� ����Ʈ
		}
		else
		{
			MK_CHECK(availableList.Exist(MkArraySection(0), buffer), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + position + L"��ġ�� ���������� ���� ����")
				return false;
		}
	}

	result = buffer;
	return true;
}

bool MkExcelToMemoryConverter::_ParseTable
(MkExcelFileInterface& reader, const MkStr& type, const MkStr& anchor, const MkStr& startPosition, SheetData& dest)
{
	bool isSingleType = (type == MKDEF_EXCEL_SHEET_SINGLE_TYPE);
	bool isVertical = (anchor == MKDEF_EXCEL_SHEET_VERTICAL_ANCHOR);

	// type, key �������. vertical�� ��� row, horizontal�� ��� column �������� ����
	MkUInt2 tkDir = (isVertical) ? MkUInt2(1, 0) : MkUInt2(0, 1);
	// field �������. tkDir�� ����
	MkUInt2 fieldDir = MkUInt2(tkDir.y, tkDir.x);

	MkUInt2 dataStartPos; // ������ ������ġ
	MK_CHECK(MkExcelFileInterface::ConvertPosition(startPosition, dataStartPos), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� ������ġ�� ���������� ���� : " + startPosition)
		return false;

	MkUInt2 typeStartPos = (isSingleType) ? dataStartPos : (dataStartPos + tkDir); // type ������ġ
	MkUInt2 nameStartPos; // name ������ġ. Table type sheet������ �ǹ� ����
	
	// type�� key�� �� �� ���� ���ӵǾ� �����ϹǷ� ī���� ����
	unsigned int typeCount = (isVertical) ? _GetRowCount(reader, typeStartPos) : _GetColumnCount(reader, typeStartPos);
	MK_CHECK(typeCount != MKDEF_ARRAY_ERROR, m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� type list �б� ����")
		return false;
	MK_CHECK(typeCount > 0, m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� type ������ġ�� �������")
		return false;

	// ũ�� ����
	dest.type.Reserve(typeCount);
	dest.key.Reserve(typeCount);

	unsigned int nameCount = 1;
	if (!isSingleType)
	{
		// ���� �� �� ���� ���ӵǾ� �����ϹǷ� ī���� ����
		nameStartPos = dataStartPos + fieldDir + fieldDir;
		nameCount = (isVertical) ? _GetColumnCount(reader, nameStartPos) : _GetRowCount(reader, nameStartPos);
		MK_CHECK(nameCount != MKDEF_ARRAY_ERROR, m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� name list �б� ����")
			return false;
		MK_CHECK(nameCount > 0, m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� ���� ������ġ�� �������")
			return false;

		// ���� ���ø���
		{
			MkStr tmp;
			MK_CHECK(reader.GetData(dataStartPos.x, dataStartPos.y, tmp), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(dataStartPos) + L"��ġ �б� ����")
				return false;

			if (!tmp.Empty())
			{
				MK_CHECK(_StringFilter(tmp, dest.templateName), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(dataStartPos) + L"��ġ�� ���ø����� ���������� ���� : " + tmp)
					return false;
			}
		}

		// ���� ����Ʈ �б�
		MkArray<MkStr>& nameList = dest.name;
		nameList.Reserve(nameCount);
		MkUInt2 namePos = nameStartPos;
		for (unsigned int i=0; i<nameCount; ++i)
		{
			MkStr nameBuf;
			MK_CHECK(reader.GetData(namePos.x, namePos.y, nameBuf), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(namePos) + L"��ġ �б� ����")
				return false;
			MK_CHECK(!nameBuf.Empty(), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(namePos) + L"��ġ�� ������ �������")
				return false;

			MkStr tmp;
			MK_CHECK(_StringFilter(nameBuf, tmp), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(namePos) + L"��ġ�� ������ ���������� ���� : " + nameBuf)
				return false;

			nameList.PushBack(tmp);
			namePos += fieldDir;
		}
	}

	// data field ����
	dest.data.Fill(nameCount);
	for (unsigned int i=0; i<nameCount; ++i)
	{
		dest.data[i].Fill(typeCount);
	}

	// type ����������� �� ���ξ� �о�� ����
	MkUInt2 typePos = typeStartPos;
	for (unsigned int i=0; i<typeCount; ++i)
	{
		// type
		MkStr typeBuf;
		MK_CHECK(reader.GetData(typePos.x, typePos.y, typeBuf), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(typePos) + L"��ġ �б� ����")
			return false;
		MK_CHECK(!typeBuf.Empty(), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(typePos) + L"��ġ�� Ÿ�� ���� �������")
			return false;
		
		// type check
		ePrimitiveDataType typeEnum = MkPrimitiveDataType::GetEnum(typeBuf);
		int typeIndex;
		if (typeEnum == ePDT_Undefined)
		{
			bool availableUnitType = true;
			if (isSingleType)
			{
				if (typeBuf == MkTagDefinitionForDataNode::TagForNode)
				{
					typeIndex = MkTagDefinitionForDataNode::IndexMarkForNodeBegin;
				}
				else if (typeBuf == MkTagDefinitionForDataNode::TagForTemplate)
				{
					typeIndex = MkTagDefinitionForDataNode::IndexMarkForTemplateBegin;
				}
				else if (typeBuf == MkTagDefinitionForDataNode::TagForPushingTemplate)
				{
					typeIndex = MkTagDefinitionForDataNode::IndexMarkForPushingTemplate;
				}
				else
					availableUnitType = false;
			}
			else
				availableUnitType = false;

			MK_CHECK(availableUnitType, m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(typePos) + L"��ġ�� Ÿ�� ���� ���ǵ��� �ʾ��� : "  + typeBuf)
				return false;
		}
		else
		{
			typeIndex = static_cast<int>(typeEnum);
		}
		dest.type.PushBack(typeIndex);

		// key
		MkStr keyBuf;
		MkUInt2 keyPos = typePos + fieldDir;
		MK_CHECK(reader.GetData(keyPos.x, keyPos.y, keyBuf), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(keyPos) + L"��ġ �б� ����")
			return false;
		MK_CHECK(!keyBuf.Empty(), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(keyPos) + L"��ġ�� Ű ���� �������")
			return false;

		if ((typeIndex == MkTagDefinitionForDataNode::IndexMarkForNodeBegin) ||
			(typeIndex == MkTagDefinitionForDataNode::IndexMarkForTemplateBegin) ||
			(typeIndex == MkTagDefinitionForDataNode::IndexMarkForPushingTemplate))
		{
			MkStr tmp;
			MK_CHECK(_StringFilter(keyBuf, tmp), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(keyPos) + L"��ġ�� ������ ���������� ���� : " + keyBuf)
				return false;
			keyBuf = tmp;
		}

		dest.key.PushBack(keyBuf);

		// field
		MkUInt2 fieldPos = typePos + fieldDir + fieldDir;
		if (isSingleType) // single
		{
			// ��� Ÿ���̸�
			if ((typeIndex == MkTagDefinitionForDataNode::IndexMarkForNodeBegin) || (typeIndex == MkTagDefinitionForDataNode::IndexMarkForTemplateBegin))
			{
				MkUInt2 currFieldPos = fieldPos;

				MkStr nodeNameBuf;
				MK_CHECK(reader.GetData(currFieldPos.x, currFieldPos.y, nodeNameBuf), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(currFieldPos) + L"��ġ �б� ����")
					return false;

				if (!nodeNameBuf.Empty())
				{
					MkStr tmp;
					MK_CHECK(_StringFilter(nodeNameBuf, tmp), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(currFieldPos) + L"��ġ�� ���ø����� ���������� ���� : " + nodeNameBuf)
						return false;
					nodeNameBuf = tmp;
				}

				currFieldPos += fieldDir;
				MkStr sheetNameBuf;
				MK_CHECK(reader.GetData(currFieldPos.x, currFieldPos.y, sheetNameBuf), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(currFieldPos) + L"��ġ �б� ����")
					return false;

				if (!sheetNameBuf.Empty())
				{
					MkStr tmp;
					MK_CHECK(_StringFilter(sheetNameBuf, tmp), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(currFieldPos) + L"��ġ�� ���ø����� ���������� ���� : " + sheetNameBuf)
						return false;
					sheetNameBuf = tmp;
				}

				MkArray<MkStr>& currArray = dest.data[0][i];
				currArray.Reserve(2);
				currArray.PushBack(nodeNameBuf);
				currArray.PushBack(sheetNameBuf);
			}
			// template pushing�̸� field�� �ʿ� ����
			else if (typeIndex == MkTagDefinitionForDataNode::IndexMarkForPushingTemplate)
			{
				// do nothing
			}
			// ���� Ÿ���̸�
			else
			{
				// �ʵ� ��
				unsigned int dataCount = (isVertical) ? _GetColumnCount(reader, fieldPos) : _GetRowCount(reader, fieldPos);
				MK_CHECK(dataCount != MKDEF_ARRAY_ERROR, m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(fieldPos) + L"��ġ���� �ʵ� ���� �б� ����")
					return false;

				if (dataCount > 0)
				{
					MkArray<MkStr>& currArray = dest.data[0][i];
					currArray.Reserve(dataCount);

					MkUInt2 currFieldPos = fieldPos;
					for (unsigned int j=0; j<dataCount; ++j)
					{
						MkStr fieldBuf;
						MK_CHECK(reader.GetData(currFieldPos.x, currFieldPos.y, fieldBuf), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(currFieldPos) + L"��ġ �б� ����")
							return false;
						MK_CHECK(!fieldBuf.Empty(), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(currFieldPos) + L"��ġ�� �ʵ� ���� �������")
							return false;

						if (typeEnum == ePDT_Str)
						{
							MkStr tmpStr;
							MK_CHECK(_StringFilter(fieldBuf, tmpStr), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(currFieldPos) + L"��ġ�� ���ڿ��� ���������� ���� : " + fieldBuf)
								return false;
							fieldBuf = tmpStr;
						}
						else
						{
							fieldBuf.RemoveBlank();
						}

						currArray.PushBack(fieldBuf);
						currFieldPos += fieldDir;
					}
				}
			}
		}
		else // table
		{
			// ���� ����Ʈ��ŭ ���� �ʵ带 �м�
			MkUInt2 currFieldPos = fieldPos;
			for (unsigned int j=0; j<nameCount; ++j)
			{
				MkStr fieldBuf;
				MK_CHECK(reader.GetData(currFieldPos.x, currFieldPos.y, fieldBuf), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(currFieldPos) + L"��ġ �б� ����")
					return false;

				if (!fieldBuf.Empty())
				{
					MkArray<MkStr>& currArray = dest.data[j][i];
					if (typeEnum == ePDT_Str)
					{
						MK_CHECK(_StringFilter(fieldBuf, currArray), m_TargetFilePath + L" ���� " + m_TargetSheetName + L" sheet�� " + MkExcelFileInterface::ConvertPosition(currFieldPos) + L"��ġ�� ���ڿ��� ���������� ���� : " + fieldBuf)
							return false;
					}
					else
					{
						_ValueFilter(fieldBuf, currArray);
					}
				}

				currFieldPos += fieldDir;
			}
		}

		// next position
		typePos += tkDir;
	}
	
	return true;
}

unsigned int MkExcelToMemoryConverter::_GetRowCount(const MkExcelFileInterface& reader, const MkUInt2& startPos) const
{
	unsigned int pos = reader.GetLastBlankRow(startPos.y, startPos.x);
	return (pos == MKDEF_ARRAY_ERROR) ? MKDEF_ARRAY_ERROR : (pos - startPos.x);
}

unsigned int MkExcelToMemoryConverter::_GetColumnCount(const MkExcelFileInterface& reader, const MkUInt2& startPos) const
{
	unsigned int pos = reader.GetLastBlankColumn(startPos.x, startPos.y);
	return (pos == MKDEF_ARRAY_ERROR) ? MKDEF_ARRAY_ERROR : (pos - startPos.y);
}

bool MkExcelToMemoryConverter::_StringFilter(const MkStr& source, MkStr& buffer) const
{
	MkStr tmp = source;
	tmp.RemoveFrontSideBlank();
	tmp.RemoveRearSideBlank();
	if ((!tmp.CheckPrefix(L"\"")) || (!tmp.CheckPostfix(L"\""))) // ���ڿ��� DQM���� �ѷ� �׿� �־�� ��
		return false;

	MkStr subset;
	tmp.GetSubStr(MkArraySection(1, tmp.GetSize() - 2), subset);

	// Ż�⹮�� �ݿ�
	if (!subset.Empty())
	{
		subset.ReplaceKeyword(L"\\\"", L"\""); // DQM
		subset.ReplaceTagtoCRLF();
	}

	buffer = subset;
	return true;
}

bool MkExcelToMemoryConverter::_StringFilter(const MkStr& source, MkArray<MkStr>& buffer) const
{
	if (source.Exist(MkTagDefinitionForDataNode::TagForArrayDivider)) // �迭 ���ɼ� ����
	{
		MkStr tmp = source;
		buffer.Reserve(tmp.CountKeyword(MkTagDefinitionForDataNode::TagForArrayDivider) + 1);

		// �Ľ��� ���� ������ ���� ���� �߰�(100%)
		tmp.Reserve(tmp.GetSize() * 2);

		// �Ľ̿� ������ ���� �ʵ��� DQM(double quotation marks) Ż�⹮�� ġȯ
		tmp.ReplaceKeyword(L"\\\"", MkTagDefinitionForDataNode::TagForDQM);

		unsigned int currentPos = 0;
		static const MkStr dqmTag = L"\"";
		while (true)
		{
			MkStr stringBuffer;
			currentPos = tmp.GetFirstBlock(currentPos, dqmTag, dqmTag, stringBuffer);
			if (currentPos != MKDEF_ARRAY_ERROR)
			{
				if (!stringBuffer.Empty()) // Ż�⹮�� �ݿ�
				{
					stringBuffer.ReplaceKeyword(MkTagDefinitionForDataNode::TagForDQM, dqmTag); // DQM
					stringBuffer.ReplaceTagtoCRLF();
				}
				buffer.PushBack(stringBuffer);
			}
			else
				break;
		}
	}
	else // �迭 �ƴ�
	{
		MkStr tmp;
		if (!_StringFilter(source, tmp))
			return false;

		buffer.PushBack(tmp);
	}
	return (!buffer.Empty());
}

void MkExcelToMemoryConverter::_ValueFilter(const MkStr& source, MkArray<MkStr>& buffer) const
{
	MkStr tmp = source;
	tmp.RemoveBlank();
	if (source.Exist(MkTagDefinitionForDataNode::TagForArrayDivider)) // �迭 ���ɼ� ����
	{
		buffer.Reserve(tmp.CountKeyword(MkTagDefinitionForDataNode::TagForArrayDivider) + 1);
		tmp.Tokenize(buffer, MkTagDefinitionForDataNode::TagForArrayDivider);
	}
	else // �迭 �ƴ�
	{
		buffer.PushBack(tmp);
	}
}

void MkExcelToMemoryConverter::_ReserveInterface(MkInterfaceForDataWriting& dwInterface) const
{
	unsigned int nodeCount = 0;
	unsigned int templateCount = 0;
	unsigned int unitCount = 0;
	unsigned int valueCount[ePDT_MaxCount];
	for (int i=ePDT_IndexBegin; i<ePDT_IndexEnd; ++i)
	{
		valueCount[i] = 0;
	}

	MK_INDEXING_LOOP(m_SheetDataList, pos)
	{
		const SheetData& field = m_SheetDataList[pos];
		unsigned int nameSize = field.name.GetSize();
		bool isSingleType = (nameSize == 0);

		// node count
		++nodeCount;
		if (!isSingleType)
		{
			nodeCount += nameSize;
		}

		// template count
		if (!field.templateName.Empty())
		{
			templateCount += (isSingleType) ? 1 : nameSize;
		}

		// unit, value count
		MK_INDEXING_LOOP(field.type, i)
		{
			int currType = field.type[i];
			if (MkPrimitiveDataType::IsValid(static_cast<ePrimitiveDataType>(currType)))
			{
				unitCount += (isSingleType) ? 1 : nameSize;

				MK_INDEXING_LOOP(field.data, j) // loop by name
				{
					valueCount[currType] += field.data[j][i].GetSize();
				}
			}
		}
	}

	MkHelperForDataNodeConverter::ReserveInterface(nodeCount, templateCount, unitCount, valueCount, dwInterface);
}

bool MkExcelToMemoryConverter::_ConvertSheetToBinary(const MkStr& sheetName, MkInterfaceForDataWriting& dwInterface) const
{
	unsigned int sheetIndex = m_SheetNameList.FindFirstInclusion(MkArraySection(0), sheetName);
	MK_CHECK(sheetIndex != MKDEF_ARRAY_ERROR, m_TargetFilePath + L" ���Ͽ� �������� �ʴ� " + sheetName + L" sheet ����")
		return false;

	const SheetData& sheetData = m_SheetDataList[sheetIndex]; // index�� ���� data ����� �������� �����
	if (sheetData.type.Empty())
		return true; // ���� ����

	unsigned int nameSize = sheetData.name.GetSize();
	bool isSingleType = (nameSize == 0);
	if (isSingleType) // single type
	{
		MK_INDEXING_LOOP(sheetData.type, i) // unit type ���� ��ȸ
		{
			int currType = sheetData.type[i];
			const MkStr& currKey = sheetData.key[i];

			// node
			if ((currType == MkTagDefinitionForDataNode::IndexMarkForNodeBegin) || (currType == MkTagDefinitionForDataNode::IndexMarkForTemplateBegin))
			{
				const MkArray<MkStr>& values = sheetData.data[0][i];
				unsigned int valueCount = values.GetSize();
				MK_CHECK(valueCount == 2, m_TargetFilePath + L" ���� " + sheetName + L" sheet���� " + currKey + L" ��带 �����ϴ� �ʵ� ����")
					return false;

				dwInterface.Write(currType); // begin of node
				dwInterface.Write(currKey); // node name

				const MkStr& templateName = values[0];
				if (templateName.Empty())
				{
					dwInterface.Write(false); // normal node
				}
				else
				{
					dwInterface.Write(true); // template applied
					dwInterface.Write(templateName); // template name
				}

				const MkStr& targetSheetName = values[1];
				if (!targetSheetName.Empty())
				{
					if (!_ConvertSheetToBinary(values[1], dwInterface)) // body
						return false;
				}

				dwInterface.Write(MkTagDefinitionForDataNode::IndexMarkForNodeBlockEnd); // end of node
			}
			// pushing template
			else if (currType == MkTagDefinitionForDataNode::IndexMarkForPushingTemplate)
			{
				dwInterface.Write(currType);
				dwInterface.Write(currKey);
			}
			// unit
			else
			{
				ePrimitiveDataType typeEnum = static_cast<ePrimitiveDataType>(currType);
				const MkArray<MkStr>& values = sheetData.data[0][i];
				unsigned int valueCount = values.GetSize();

				if (MkPrimitiveDataType::IsValid(typeEnum) && (valueCount > 0)) // ���� ������ unit ����
				{
					dwInterface.Write(currType); // type
					dwInterface.Write(currKey); // key
					dwInterface.Write(valueCount); // count
					MK_INDEXING_LOOP(values, j)
					{
						MkHelperForDataNodeConverter::WriteUnitString(typeEnum, values[j], dwInterface);
					}
				}
			}
		}
	}
	else // table type
	{
		bool hasTemplate = !sheetData.templateName.Empty();
		MK_INDEXING_LOOP(sheetData.name, i) // ���� ���� ��ȸ
		{
			dwInterface.Write(MkTagDefinitionForDataNode::IndexMarkForNodeBegin); // begin of node
			dwInterface.Write(sheetData.name[i]); // node name
			dwInterface.Write(hasTemplate); // template applied
			if (hasTemplate)
			{
				dwInterface.Write(sheetData.templateName); // template name
			}

			MK_INDEXING_LOOP(sheetData.type, j)
			{
				const MkArray<MkStr>& values = sheetData.data[i][j];
				unsigned int valueCount = values.GetSize();
				if (valueCount > 0)
				{
					int currType = sheetData.type[j];
					ePrimitiveDataType typeEnum = static_cast<ePrimitiveDataType>(currType);
					dwInterface.Write(currType); // type
					dwInterface.Write(sheetData.key[j]); // key
					dwInterface.Write(valueCount); // count
					MK_INDEXING_LOOP(values, k)
					{
						MkHelperForDataNodeConverter::WriteUnitString(typeEnum, values[k], dwInterface);
					}
				}
			}

			dwInterface.Write(MkTagDefinitionForDataNode::IndexMarkForNodeBlockEnd); // end of node
		}
	}

	return true;
}

//------------------------------------------------------------------------------------------------//
