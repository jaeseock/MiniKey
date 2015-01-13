
#include "MkCore_MkCheck.h"
#include "MkCore_MkInterfaceForDataWriting.h"
#include "MkCore_MkTagDefinitionForDataNode.h"
#include "MkCore_MkHelperForDataNodeConverter.h"
#include "MkCore_MkExcelFileReader.h"
#include "MkCore_MkExcelToMemoryConverter.h"

#define MKDEF_E_TO_M_COMMENT_TAG L"_"

#define MKDEF_E_TO_M_TYPE_POS L"B1"
#define MKDEF_E_TO_M_ANCHOR_POS L"B2"
#define MKDEF_E_TO_M_STARTPOSITION_POS L"B3"

#define MKDEF_E_TO_M_SINGLE_TYPE L"Single"
#define MKDEF_E_TO_M_TABLE_TYPE L"Table"

#define MKDEF_E_TO_M_VERTICAL_ANCHOR L"Vertical"
#define MKDEF_E_TO_M_HORIZONTAL_ANCHOR L"Horizontal"

#define MKDEF_E_TO_M_ROOT_SHEETNAME L"ROOT"


//------------------------------------------------------------------------------------------------//

bool MkExcelToMemoryConverter::Convert(const MkPathName& filePath, MkByteArray& destination)
{
	m_TargetFilePath = filePath;

	MkExcelFileReader excelReader;
	MK_CHECK(excelReader.SetUp(filePath), L"데이터 노드 구성을 위한 " + MkStr(filePath) + L" 엑셀 파일 열기 실패")
		return false;

	MK_CHECK(excelReader.SheetExist(MKDEF_E_TO_M_ROOT_SHEETNAME), MkStr(filePath) + L" 엑셀 파일에 " + MkStr(MKDEF_E_TO_M_ROOT_SHEETNAME) + L"시트가 존재하지 않음")
		return false;

	bool ok = _BuildSheetData(excelReader);
	excelReader.Clear();
	if (ok)
	{
		MkInterfaceForDataWriting dwInterface;
		_ReserveInterface(dwInterface);
		ok = _ConvertSheetToBinary(MKDEF_E_TO_M_ROOT_SHEETNAME, dwInterface);
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

bool MkExcelToMemoryConverter::_BuildSheetData(MkExcelFileReader& reader)
{
	unsigned int sheetCount = reader.GetSheetSize();
	m_SheetNameList.Reserve(sheetCount);
	m_SheetDataList.Reserve(sheetCount);

	MkArray<MkStr> typeFilter;
	typeFilter.Reserve(2);
	typeFilter.PushBack(MKDEF_E_TO_M_SINGLE_TYPE);
	typeFilter.PushBack(MKDEF_E_TO_M_TABLE_TYPE);

	MkArray<MkStr> anchorFilter;
	typeFilter.Reserve(2);
	typeFilter.PushBack(MKDEF_E_TO_M_VERTICAL_ANCHOR);
	typeFilter.PushBack(MKDEF_E_TO_M_HORIZONTAL_ANCHOR);

	MkArray<MkStr> emptyFilter;

	// sheet 순회
	for (unsigned int i=0; i<sheetCount; ++i)
	{
		MK_CHECK(reader.SetActiveSheet(i), m_TargetFilePath + L" 파일의 " + MkStr(i) + L"번째 sheet가 열리지 않음")
			return false;

		MkStr sheetName = reader.GetSheetName(i);
		MK_CHECK(!sheetName.Empty(), m_TargetFilePath + L" 파일의 " + MkStr(i) + L"번째 sheet 이름이 비었음")
			return false;

		if (!sheetName.CheckPrefix(MKDEF_E_TO_M_COMMENT_TAG)) // 주석 시트가 아니면 진행
		{
			m_TargetSheetName = sheetName;

			// 필수 값에 대한 파싱 및 예외처리
			// type
			MkStr typeStr;
			if (!_GetBaseValue(reader, MKDEF_E_TO_M_TYPE_POS, typeFilter, typeStr))
				return false;

			// anchor
			MkStr anchorStr;
			if (!_GetBaseValue(reader, MKDEF_E_TO_M_ANCHOR_POS, anchorFilter, anchorStr))
				return false;

			// start position
			MkStr startPositionStr;
			if (!_GetBaseValue(reader, MKDEF_E_TO_M_STARTPOSITION_POS, emptyFilter, startPositionStr))
				return false;

			// 시작위치가 존재하면 내부 파싱 진행
			if (!startPositionStr.Empty())
			{
				m_SheetNameList.PushBack(sheetName);
				SheetData& sheetData = m_SheetDataList.PushBack();
				if (!_ParseTable(reader, typeStr, anchorStr, startPositionStr, sheetData))
					return false;
			}
		}
	}
	return true;
}

// 입력값 예외검사 하지 않음
bool MkExcelToMemoryConverter::_GetBaseValue
(MkExcelFileReader& reader, const MkStr& position, const MkArray<MkStr>& availableList, MkStr& result) const
{
	MkStr buffer;
	MK_CHECK(reader.GetData(position, buffer), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + position + L"위치에서 값 읽기 실패")
		return false;

	if (!availableList.Empty()) // 필터가 존재하면
	{
		if (buffer.Empty())
		{
			buffer = availableList[0]; // 비었을 경우 첫번째 필터가 디폴트
		}
		else
		{
			MK_CHECK(availableList.Exist(MkArraySection(0), buffer), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + position + L"위치에 비정상적인 값이 존재")
				return false;
		}
	}

	result = buffer;
	return true;
}

bool MkExcelToMemoryConverter::_ParseTable
(MkExcelFileReader& reader, const MkStr& type, const MkStr& anchor, const MkStr& startPosition, SheetData& dest)
{
	bool isSingleType = (type == MKDEF_E_TO_M_SINGLE_TYPE);
	bool isVertical = (anchor == MKDEF_E_TO_M_VERTICAL_ANCHOR);

	// type, key 진행방향. vertical일 경우 row, horizontal일 경우 column 방향으로 진행
	_CellPos tkDir = (isVertical) ? _CellPos(1, 0) : _CellPos(0, 1);
	// field 진행방향. tkDir와 직교
	_CellPos fieldDir = _CellPos(tkDir.c, tkDir.r);

	_CellPos dataStartPos; // 데이터 시작위치
	MK_CHECK(MkExcelFileReader::ConvertPosition(startPosition, dataStartPos.r, dataStartPos.c), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 시작위치가 정상적이지 않음 : " + startPosition)
		return false;

	_CellPos typeStartPos = (isSingleType) ? dataStartPos : (dataStartPos + tkDir); // type 시작위치
	_CellPos nameStartPos; // name 시작위치. Table type sheet에서만 의미 있음
	
	// type과 key는 빈 셀 없이 연속되어 존재하므로 카운팅 가능
	unsigned int typeCount = (isVertical) ? _GetRowCount(reader, typeStartPos) : _GetColumnCount(reader, typeStartPos);
	MK_CHECK(typeCount != MKDEF_ARRAY_ERROR, m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 type list 읽기 실패")
		return false;
	MK_CHECK(typeCount > 0, m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 type 시작위치가 비어있음")
		return false;

	// 크기 예약
	dest.type.Reserve(typeCount);
	dest.key.Reserve(typeCount);

	unsigned int nameCount = 1;
	if (!isSingleType)
	{
		// 노드명도 빈 셀 없이 연속되어 존재하므로 카운팅 가능
		nameStartPos = dataStartPos + fieldDir + fieldDir;
		nameCount = (isVertical) ? _GetColumnCount(reader, nameStartPos) : _GetRowCount(reader, nameStartPos);
		MK_CHECK(nameCount != MKDEF_ARRAY_ERROR, m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 name list 읽기 실패")
			return false;
		MK_CHECK(nameCount > 0, m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 노드명 시작위치가 비어있음")
			return false;

		// 공통 템플릿명
		MK_CHECK(reader.GetDataRC(dataStartPos.r, dataStartPos.c, dest.templateName), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + MkExcelFileReader::ConvertPosition(dataStartPos.r, dataStartPos.c) + L"위치 읽기 실패")
			return false;

		// 네임 리스트 읽기
		MkArray<MkStr>& nameList = dest.name;
		nameList.Reserve(nameCount);
		_CellPos namePos = nameStartPos;
		for (unsigned int i=0; i<nameCount; ++i)
		{
			MkStr nameBuf;
			MK_CHECK(reader.GetDataRC(namePos.r, namePos.c, nameBuf), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + MkExcelFileReader::ConvertPosition(namePos.r, namePos.c) + L"위치 읽기 실패")
				return false;
			MK_CHECK(!nameBuf.Empty(), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + MkExcelFileReader::ConvertPosition(namePos.r, namePos.c) + L"위치의 노드명이 비어있음")
				return false;

			nameList.PushBack(nameBuf);
			namePos += fieldDir;
		}
	}

	// data field 예약
	dest.data.Fill(nameCount);
	for (unsigned int i=0; i<nameCount; ++i)
	{
		dest.data[i].Fill(typeCount);
	}

	// type 진행방향으로 한 라인씩 읽어가며 저장
	_CellPos typePos = typeStartPos;
	for (unsigned int i=0; i<typeCount; ++i)
	{
		// type
		MkStr typeBuf;
		MK_CHECK(reader.GetDataRC(typePos.r, typePos.c, typeBuf), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + MkExcelFileReader::ConvertPosition(typePos.r, typePos.c) + L"위치 읽기 실패")
			return false;
		MK_CHECK(!typeBuf.Empty(), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + MkExcelFileReader::ConvertPosition(typePos.r, typePos.c) + L"위치의 타입 값이 비어있음")
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
				else
					availableUnitType = false;
			}
			else
				availableUnitType = false;

			MK_CHECK(availableUnitType, m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + MkExcelFileReader::ConvertPosition(typePos.r, typePos.c) + L"위치의 타입 값은 정의되지 않았음 : "  + typeBuf)
				return false;
		}
		else
		{
			typeIndex = static_cast<int>(typeEnum);
		}
		dest.type.PushBack(typeIndex);

		// key
		MkStr keyBuf;
		_CellPos keyPos = typePos + fieldDir;
		MK_CHECK(reader.GetDataRC(keyPos.r, keyPos.c, keyBuf), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + MkExcelFileReader::ConvertPosition(keyPos.r, keyPos.c) + L"위치 읽기 실패")
			return false;
		MK_CHECK(!keyBuf.Empty(), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + MkExcelFileReader::ConvertPosition(keyPos.r, keyPos.c) + L"위치의 키 값이 비어있음")
			return false;
		dest.key.PushBack(keyBuf);

		// field
		_CellPos fieldPos = typePos + fieldDir + fieldDir;
		if (isSingleType) // single
		{
			// 노드 타입이면
			if ((typeBuf == MkTagDefinitionForDataNode::TagForNode) || (typeBuf == MkTagDefinitionForDataNode::TagForTemplate))
			{
				_CellPos currFieldPos = fieldPos;

				MkStr nodeNameBuf;
				MK_CHECK(reader.GetDataRC(currFieldPos.r, currFieldPos.c, nodeNameBuf), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + MkExcelFileReader::ConvertPosition(currFieldPos.r, currFieldPos.c) + L"위치 읽기 실패")
					return false;

				currFieldPos += fieldDir;
				MkStr sheetNameBuf;
				MK_CHECK(reader.GetDataRC(currFieldPos.r, currFieldPos.c, sheetNameBuf), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + MkExcelFileReader::ConvertPosition(currFieldPos.r, currFieldPos.c) + L"위치 읽기 실패")
					return false;

				MkArray<MkStr>& currArray = dest.data[0][i];
				currArray.Reserve(2);
				currArray.PushBack(nodeNameBuf);
				currArray.PushBack(sheetNameBuf);
			}
			// 유닛 타입이면
			else
			{
				// 필드 수
				unsigned int dataCount = (isVertical) ? _GetColumnCount(reader, fieldPos) : _GetRowCount(reader, fieldPos);
				MK_CHECK(dataCount != MKDEF_ARRAY_ERROR, m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + MkExcelFileReader::ConvertPosition(fieldPos.r, fieldPos.c) + L"위치부터 필드 길이 읽기 실패")
					return false;

				if (dataCount > 0)
				{
					MkArray<MkStr>& currArray = dest.data[0][i];
					currArray.Reserve(dataCount);

					_CellPos currFieldPos = fieldPos;
					for (unsigned int j=0; j<dataCount; ++j)
					{
						MkStr fieldBuf;
						MK_CHECK(reader.GetDataRC(currFieldPos.r, currFieldPos.c, fieldBuf), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + MkExcelFileReader::ConvertPosition(currFieldPos.r, currFieldPos.c) + L"위치 읽기 실패")
							return false;
						MK_CHECK(!fieldBuf.Empty(), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + MkExcelFileReader::ConvertPosition(currFieldPos.r, currFieldPos.c) + L"위치의 필드 값이 비어있음")
							return false;

						if (typeEnum == ePDT_Str)
						{
							MkStr tmpStr;
							MK_CHECK(_StringFilter(fieldBuf, tmpStr), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + MkExcelFileReader::ConvertPosition(currFieldPos.r, currFieldPos.c) + L"위치의 문자열이 정상적이지 않음 : " + fieldBuf)
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
			// 노드명 리스트만큼 돌며 필드를 분석
			_CellPos currFieldPos = fieldPos;
			for (unsigned int j=0; j<nameCount; ++j)
			{
				MkStr fieldBuf;
				MK_CHECK(reader.GetDataRC(currFieldPos.r, currFieldPos.c, fieldBuf), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + MkExcelFileReader::ConvertPosition(currFieldPos.r, currFieldPos.c) + L"위치 읽기 실패")
					return false;

				if (!fieldBuf.Empty())
				{
					MkArray<MkStr>& currArray = dest.data[j][i];
					if (typeEnum == ePDT_Str)
					{
						MK_CHECK(_StringFilter(fieldBuf, currArray), m_TargetFilePath + L" 파일 " + m_TargetSheetName + L" sheet의 " + MkExcelFileReader::ConvertPosition(currFieldPos.r, currFieldPos.c) + L"위치의 문자열이 정상적이지 않음 : " + fieldBuf)
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

unsigned int MkExcelToMemoryConverter::_GetRowCount(const MkExcelFileReader& reader, const _CellPos& startPos) const
{
	unsigned int pos = reader.GetLastBlankRow(startPos.c, startPos.r);
	return (pos == MKDEF_ARRAY_ERROR) ? MKDEF_ARRAY_ERROR : (pos - startPos.r);
}

unsigned int MkExcelToMemoryConverter::_GetColumnCount(const MkExcelFileReader& reader, const _CellPos& startPos) const
{
	unsigned int pos = reader.GetLastBlankColumn(startPos.r, startPos.c);
	return (pos == MKDEF_ARRAY_ERROR) ? MKDEF_ARRAY_ERROR : (pos - startPos.c);
}

bool MkExcelToMemoryConverter::_StringFilter(const MkStr& source, MkStr& buffer) const
{
	MkStr tmp = source;
	tmp.RemoveFrontSideBlank();
	tmp.RemoveRearSideBlank();
	if ((!tmp.CheckPrefix(L"\"")) || (!tmp.CheckPostfix(L"\""))) // 문자열은 DQM으로 둘러 쌓여 있어야 함
		return false;

	MkStr subset;
	tmp.GetSubStr(MkArraySection(1, tmp.GetSize() - 2), subset);

	// 탈출문자 반영
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
	if (source.Exist(MkTagDefinitionForDataNode::TagForArrayDivider)) // 배열 가능성 존재
	{
		MkStr tmp = source;
		buffer.Reserve(tmp.CountKeyword(MkTagDefinitionForDataNode::TagForArrayDivider) + 1);

		// 파싱중 변조 여유를 위한 버퍼 추가(100%)
		tmp.Reserve(tmp.GetSize() * 2);

		// 파싱에 영향을 주지 않도록 DQM(double quotation marks) 탈출문자 치환
		tmp.ReplaceKeyword(L"\\\"", MkTagDefinitionForDataNode::TagForDQM);

		unsigned int currentPos = 0;
		static const MkStr dqmTag = L"\"";
		while (true)
		{
			MkStr stringBuffer;
			currentPos = tmp.GetFirstBlock(currentPos, dqmTag, dqmTag, stringBuffer);
			if (currentPos != MKDEF_ARRAY_ERROR)
			{
				if (!stringBuffer.Empty()) // 탈출문자 반영
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
	else // 배열 아님
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
	if (source.Exist(MkTagDefinitionForDataNode::TagForArrayDivider)) // 배열 가능성 존재
	{
		buffer.Reserve(tmp.CountKeyword(MkTagDefinitionForDataNode::TagForArrayDivider) + 1);
		tmp.Tokenize(buffer, MkTagDefinitionForDataNode::TagForArrayDivider);
	}
	else // 배열 아님
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
	MK_CHECK(sheetIndex != MKDEF_ARRAY_ERROR, m_TargetFilePath + L" 파일에 존재하지 않는 " + sheetName + L" sheet 참조")
		return false;

	const SheetData& sheetData = m_SheetDataList[sheetIndex]; // index에 대한 data 존재는 논리적으로 보장됨
	if (sheetData.type.Empty())
		return true; // 내용 없음

	unsigned int nameSize = sheetData.name.GetSize();
	bool isSingleType = (nameSize == 0);
	if (isSingleType) // single type
	{
		MK_INDEXING_LOOP(sheetData.type, i) // unit type 기준 순회
		{
			int currType = sheetData.type[i];
			ePrimitiveDataType typeEnum = static_cast<ePrimitiveDataType>(currType);
			const MkStr& currKey = sheetData.key[i];
			const MkArray<MkStr>& values = sheetData.data[0][i];
			unsigned int valueCount = values.GetSize();

			if (MkPrimitiveDataType::IsValid(typeEnum)) // unit
			{
				if (valueCount > 0) // 값이 없으면 unit 무시
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
			else // node
			{
				MK_CHECK(valueCount == 2, m_TargetFilePath + L" 파일 " + sheetName + L" sheet에서 " + currKey + L" 노드를 정의하는 필드 오류")
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
		}
	}
	else // table type
	{
		bool hasTemplate = !sheetData.templateName.Empty();
		MK_INDEXING_LOOP(sheetData.name, i) // 노드명 기준 순회
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
